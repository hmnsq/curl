/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_US_ਪ੍ਰਤੀ_SEC         1000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_RLIMIT_MIN_ਦਰ    (4 * 1024)  /* minimum step rate */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_RLIMIT_ਕਦਮ_MIN_MS 2  /* minimum step duration */

ਸਥਿਰ ਖਾਲੀ rlimit_update(ਬਣਤਰ Curl_rlimit *r,
                          ਅਚਲ ਬਣਤਰ curltime *pts)
{
  timediff_t elapsed_us, elapsed_ਕਦਮਾਂ;
  int64_t token_gain;

  DEBUGASSERT(r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ);
  ਜੇ((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    ਵਾਪਸ;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ਜੇ(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }

  elapsed_us += r->ਵਾਧੂ_us;
  ਜੇ(elapsed_us < r->ਕਦਮ_us)
    ਵਾਪਸ;

  /* we do the update */
  r->ts = *pts;
  elapsed_ਕਦਮਾਂ = elapsed_us / r->ਕਦਮ_us;
  r->ਵਾਧੂ_us = elapsed_us % r->ਕਦਮ_us;

  /* How many tokens did we gain since the last update? */
  ਜੇ(r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ > (INT64_MAX / elapsed_ਕਦਮਾਂ))
    token_gain = INT64_MAX;
  ਨਹੀਂਤਾਂ {
    token_gain = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ * elapsed_ਕਦਮਾਂ;
  }

  ਜੇ((INT64_MAX - token_gain) > r->ਟੋਕਨ)
    r->ਟੋਕਨ += token_gain;
  ਨਹੀਂਤਾਂ
    r->ਟੋਕਨ = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  ਜੇ(r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ && (r->ਟੋਕਨ > r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ)) {
    r->ਟੋਕਨ = r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ;
  }
}

ਸਥਿਰ ਖਾਲੀ rlimit_tune_ਕਦਮਾਂ(ਬਣਤਰ Curl_rlimit *r,
                              int64_t ਟੋਕਨ_ਕੁੱਲ)
{
  int64_t ਟੋਕਨ_ਆਖਰੀ, ਟੋਕਨ_main, msteps;

  /* Tune the ratelimit at the start *if* we know how many tokens
   * are expected to be consumed in total.
   * The reason for tuning is that rlimit provides tokens to be consumed
   * per "step" which starts out to be a second. The tokens may be consumed
   * in full at the beginning of a step. The remainder of the second will
   * have no tokens available, effectively blocking the consumption and
   * so keeping the "step average" in line.
   * This works will up to the last step. When no more tokens are needed,
   * no wait will happen and the last step would be too fast. This is
   * especially noticeable when only a few steps are needed.
   *
   * Example: downloading 1.5kb with a ratelimit of 1k could be done in
   * roughly 1 second (1k in the first second and the 0.5 at the start of
   * the second one).
   *
   * The tuning tries to make the last step small, using only
   * 1 percent of the total tokens (at least 1). The rest of the tokens
   * are to be consumed in the steps before by adjusting the duration of
   * the step and the amount of tokens it provides. */
  ਜੇ(!r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ ||
     (ਟੋਕਨ_ਕੁੱਲ <= 1) ||
     (ਟੋਕਨ_ਕੁੱਲ > (INT64_MAX / 1000)))
    ਵਾਪਸ;

  /* Calculate tokens for the last step and the ones before. */
  ਟੋਕਨ_ਆਖਰੀ = ਟੋਕਨ_ਕੁੱਲ / 100;
  ਜੇ(!ਟੋਕਨ_ਆਖਰੀ) /* less than 100 total, use 1 */
    ਟੋਕਨ_ਆਖਰੀ = 1;
  ਨਹੀਂਤਾਂ ਜੇ(ਟੋਕਨ_ਆਖਰੀ > CURL_RLIMIT_MIN_ਦਰ)
    ਟੋਕਨ_ਆਖਰੀ = CURL_RLIMIT_MIN_ਦਰ;
  DEBUGASSERT(ਟੋਕਨ_ਆਖਰੀ);
  ਟੋਕਨ_main = ਟੋਕਨ_ਕੁੱਲ - ਟੋਕਨ_ਆਖਰੀ;
  DEBUGASSERT(ਟੋਕਨ_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->ਕਦਮ_us == CURL_US_ਪ੍ਰਤੀ_SEC);

  msteps = (ਟੋਕਨ_main * 1000 / r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ);
  ਜੇ(msteps < CURL_RLIMIT_ਕਦਮ_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    ਵਾਪਸ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->ਕਦਮ_us = (timediff_t)msteps * 1000;
    r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ = ਟੋਕਨ_main;
    r->ਟੋਕਨ = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
  }
  ਨਹੀਂਤਾਂ {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    ਜੇ(mstep_inc) {
      curl_off_t ਦਰ_inc = ((r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ * mstep_inc) / 1000);
      ਜੇ(ਦਰ_inc) {
        r->ਕਦਮ_us = CURL_US_ਪ੍ਰਤੀ_SEC + ((timediff_t)mstep_inc * 1000);
        r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ += ਦਰ_inc;
        r->ਟੋਕਨ = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
        ਜੇ(r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ) {
          curl_off_t ਮਿਆਦ_inc = ((r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ * mstep_inc) / 1000);
          ਜੇ(ਮਿਆਦ_inc)
            r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ += ਮਿਆਦ_inc;
        }
      }
    }
  }
}

ਖਾਲੀ Curl_rlimit_init(ਬਣਤਰ Curl_rlimit *r,
                      int64_t ਦਰ_ਪ੍ਰਤੀ_sec,
                      int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_sec,
                      ਅਚਲ ਬਣਤਰ curltime *pts)
{
  DEBUGASSERT(ਦਰ_ਪ੍ਰਤੀ_sec >= 0);
  DEBUGASSERT(ਮਿਆਦ_ਪ੍ਰਤੀ_sec >= ਦਰ_ਪ੍ਰਤੀ_sec || !ਮਿਆਦ_ਪ੍ਰਤੀ_sec);
  DEBUGASSERT(pts);
  r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਦਰ_ਪ੍ਰਤੀ_sec = ਦਰ_ਪ੍ਰਤੀ_sec;
  r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਮਿਆਦ_ਪ੍ਰਤੀ_sec = ਮਿਆਦ_ਪ੍ਰਤੀ_sec;
  r->ਕਦਮ_us = CURL_US_ਪ੍ਰਤੀ_SEC;
  r->ਵਾਧੂ_us = 0;
  r->ਟੋਕਨ = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
  r->ts = *pts;
  r->blocked = FALSE;
}

ਖਾਲੀ Curl_rlimit_change(ਬਣਤਰ Curl_rlimit *r,
                        int64_t ਦਰ_ਪ੍ਰਤੀ_sec,
                        int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_sec)
{
  DEBUGASSERT(ਦਰ_ਪ੍ਰਤੀ_sec >= 0);
  DEBUGASSERT(ਮਿਆਦ_ਪ੍ਰਤੀ_sec >= ਦਰ_ਪ੍ਰਤੀ_sec || !ਮਿਆਦ_ਪ੍ਰਤੀ_sec);
  r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਦਰ_ਪ੍ਰਤੀ_sec = ਦਰ_ਪ੍ਰਤੀ_sec;
  r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਮਿਆਦ_ਪ੍ਰਤੀ_sec = ਮਿਆਦ_ਪ੍ਰਤੀ_sec;
  r->ਕਦਮ_us = CURL_US_ਪ੍ਰਤੀ_SEC;
  r->ਵਾਧੂ_us = 0;
  ਜੇ(r->ਟੋਕਨ > r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ)
    r->ਟੋਕਨ = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

ਖਾਲੀ Curl_rlimit_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_rlimit *r, ਅਚਲ ਬਣਤਰ curltime *pts,
                       int64_t ਕੁੱਲ_ਟੋਕਨ)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਦਰ_ਪ੍ਰਤੀ_sec;
  r->ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ = r->ਮਿਆਦ_ਪ੍ਰਤੀ_sec;
  r->ਕਦਮ_us = CURL_US_ਪ੍ਰਤੀ_SEC;
  r->ਵਾਧੂ_us = 0;
  r->ਟੋਕਨ = r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
  r->ts = *pts;
  rlimit_tune_ਕਦਮਾਂ(r, ਕੁੱਲ_ਟੋਕਨ);
}

int64_t Curl_rlimit_ਪ੍ਰਤੀ_ਕਦਮ(ਬਣਤਰ Curl_rlimit *r)
{
  ਵਾਪਸ r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ;
}

bool Curl_rlimit_ਸਰਗਰਮ(ਬਣਤਰ Curl_rlimit *r)
{
  ਵਾਪਸ (r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(ਬਣਤਰ Curl_rlimit *r)
{
  ਵਾਪਸ (bool)r->blocked;
}

int64_t Curl_rlimit_avail(ਬਣਤਰ Curl_rlimit *r,
                          ਅਚਲ ਬਣਤਰ curltime *pts)
{
  ਬਣਤਰ curltime ts;

  ਜੇ(r->blocked)
    ਵਾਪਸ 0;
  ਨਹੀਂਤਾਂ ਜੇ(r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ) {
    ਜੇ(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    ਵਾਪਸ r->ਟੋਕਨ;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ INT64_MAX;
}

ਖਾਲੀ Curl_rlimit_drain(ਬਣਤਰ Curl_rlimit *r, size_t ਟੋਕਨ,
                       ਅਚਲ ਬਣਤਰ curltime *pts)
{
  ਬਣਤਰ curltime ts;

  ਜੇ(r->blocked || !r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ)
    ਵਾਪਸ;

  ਜੇ(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#ਜੇ 8 <= SIZEOF_ਮਾਪ_T
  ਜੇ(ਟੋਕਨ > INT64_MAX) {
    r->ਟੋਕਨ = INT64_MAX;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  {
    int64_t val = (int64_t)ਟੋਕਨ;
    ਜੇ((INT64_MIN + val) < r->ਟੋਕਨ)
      r->ਟੋਕਨ -= val;
    ਨਹੀਂਤਾਂ
      r->ਟੋਕਨ = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(ਬਣਤਰ Curl_rlimit *r,
                               ਅਚਲ ਬਣਤਰ curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  ਜੇ(r->blocked || !r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ)
    ਵਾਪਸ 0;
  rlimit_update(r, pts);
  ਜੇ(r->ਟੋਕਨ > 0)
    ਵਾਪਸ 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->ਕਦਮ_us - r->ਵਾਧੂ_us;
  ਜੇ(r->ਟੋਕਨ < 0) {
    curl_off_t debt_pct = ((-r->ਟੋਕਨ) * 100 / r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ);
    ਜੇ(debt_pct)
      wait_us += (r->ਕਦਮ_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ਜੇ(elapsed_us >= wait_us)
    ਵਾਪਸ 0;
  wait_us -= elapsed_us;
  ਵਾਪਸ (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_ਅਗਲਾ_ਕਦਮ_ms(ਬਣਤਰ Curl_rlimit *r,
                                    ਅਚਲ ਬਣਤਰ curltime *pts)
{
  ਜੇ(!r->blocked && r->ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ) {
    timediff_t elapsed_us, ਅਗਲਾ_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->ਵਾਧੂ_us;
    ਜੇ(r->ਕਦਮ_us > elapsed_us) {
      ਅਗਲਾ_us = r->ਕਦਮ_us - elapsed_us;
      ਵਾਪਸ (ਅਗਲਾ_us + 999) / 1000; /* in milliseconds */
    }
  }
  ਵਾਪਸ 0;
}

ਖਾਲੀ Curl_rlimit_ਖੰਡ(ਬਣਤਰ Curl_rlimit *r,
                       bool activate,
                       ਅਚਲ ਬਣਤਰ curltime *pts)
{
  ਜੇ(!activate == !r->blocked)
    ਵਾਪਸ;

  r->ts = *pts;
  r->blocked = activate;
  ਜੇ(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_ਸ਼ੁਰੂ(r, pts, -1);
  }
  ਨਹੀਂਤਾਂ {
    r->ਟੋਕਨ = 0;
  }
}
