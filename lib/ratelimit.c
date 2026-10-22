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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "ratelimit.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_US_ಪ್ರತಿ_SEC         1000000
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_RLIMIT_MIN_ದರ    (4 * 1024)  /* minimum step rate */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_RLIMIT_ಹೆಜ್ಜೆ_MIN_MS 2  /* minimum step duration */

ಸ್ಥಿರ ಶೂನ್ಯ rlimit_update(ರಚನೆ Curl_rlimit *r,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  timediff_t elapsed_us, elapsed_ಹೆಜ್ಜೆಗಳು;
  int64_t token_gain;

  DEBUGASSERT(r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ);
  ಆದರೆ((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    ಹಿಂತಿರುಗಿ;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ಆದರೆ(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }

  elapsed_us += r->ಹೆಚ್ಚುವರಿ_us;
  ಆದರೆ(elapsed_us < r->ಹೆಜ್ಜೆ_us)
    ಹಿಂತಿರುಗಿ;

  /* we do the update */
  r->ts = *pts;
  elapsed_ಹೆಜ್ಜೆಗಳು = elapsed_us / r->ಹೆಜ್ಜೆ_us;
  r->ಹೆಚ್ಚುವರಿ_us = elapsed_us % r->ಹೆಜ್ಜೆ_us;

  /* How many tokens did we gain since the last update? */
  ಆದರೆ(r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ > (INT64_MAX / elapsed_ಹೆಜ್ಜೆಗಳು))
    token_gain = INT64_MAX;
  ಇಲ್ಲದಿದ್ದರೆ {
    token_gain = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ * elapsed_ಹೆಜ್ಜೆಗಳು;
  }

  ಆದರೆ((INT64_MAX - token_gain) > r->ಟೋಕನುಗಳು)
    r->ಟೋಕನುಗಳು += token_gain;
  ಇಲ್ಲದಿದ್ದರೆ
    r->ಟೋಕನುಗಳು = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  ಆದರೆ(r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ && (r->ಟೋಕನುಗಳು > r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ)) {
    r->ಟೋಕನುಗಳು = r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rlimit_tune_ಹೆಜ್ಜೆಗಳು(ರಚನೆ Curl_rlimit *r,
                              int64_t ಟೋಕನುಗಳು_ಒಟ್ಟು)
{
  int64_t ಟೋಕನುಗಳು_ಕೊನೆಯ, ಟೋಕನುಗಳು_main, msteps;

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
  ಆದರೆ(!r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ ||
     (ಟೋಕನುಗಳು_ಒಟ್ಟು <= 1) ||
     (ಟೋಕನುಗಳು_ಒಟ್ಟು > (INT64_MAX / 1000)))
    ಹಿಂತಿರುಗಿ;

  /* Calculate tokens for the last step and the ones before. */
  ಟೋಕನುಗಳು_ಕೊನೆಯ = ಟೋಕನುಗಳು_ಒಟ್ಟು / 100;
  ಆದರೆ(!ಟೋಕನುಗಳು_ಕೊನೆಯ) /* less than 100 total, use 1 */
    ಟೋಕನುಗಳು_ಕೊನೆಯ = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಟೋಕನುಗಳು_ಕೊನೆಯ > CURL_RLIMIT_MIN_ದರ)
    ಟೋಕನುಗಳು_ಕೊನೆಯ = CURL_RLIMIT_MIN_ದರ;
  DEBUGASSERT(ಟೋಕನುಗಳು_ಕೊನೆಯ);
  ಟೋಕನುಗಳು_main = ಟೋಕನುಗಳು_ಒಟ್ಟು - ಟೋಕನುಗಳು_ಕೊನೆಯ;
  DEBUGASSERT(ಟೋಕನುಗಳು_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->ಹೆಜ್ಜೆ_us == CURL_US_ಪ್ರತಿ_SEC);

  msteps = (ಟೋಕನುಗಳು_main * 1000 / r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ);
  ಆದರೆ(msteps < CURL_RLIMIT_ಹೆಜ್ಜೆ_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    ಹಿಂತಿರುಗಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->ಹೆಜ್ಜೆ_us = (timediff_t)msteps * 1000;
    r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = ಟೋಕನುಗಳು_main;
    r->ಟೋಕನುಗಳು = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    ಆದರೆ(mstep_inc) {
      curl_off_t ದರ_inc = ((r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ * mstep_inc) / 1000);
      ಆದರೆ(ದರ_inc) {
        r->ಹೆಜ್ಜೆ_us = CURL_US_ಪ್ರತಿ_SEC + ((timediff_t)mstep_inc * 1000);
        r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ += ದರ_inc;
        r->ಟೋಕನುಗಳು = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
        ಆದರೆ(r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ) {
          curl_off_t ಅವಧಿ_inc = ((r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ * mstep_inc) / 1000);
          ಆದರೆ(ಅವಧಿ_inc)
            r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ += ಅವಧಿ_inc;
        }
      }
    }
  }
}

ಶೂನ್ಯ Curl_rlimit_init(ರಚನೆ Curl_rlimit *r,
                      int64_t ದರ_ಪ್ರತಿ_sec,
                      int64_t ಅವಧಿ_ಪ್ರತಿ_sec,
                      ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  DEBUGASSERT(ದರ_ಪ್ರತಿ_sec >= 0);
  DEBUGASSERT(ಅವಧಿ_ಪ್ರತಿ_sec >= ದರ_ಪ್ರತಿ_sec || !ಅವಧಿ_ಪ್ರತಿ_sec);
  DEBUGASSERT(pts);
  r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ದರ_ಪ್ರತಿ_sec = ದರ_ಪ್ರತಿ_sec;
  r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ಅವಧಿ_ಪ್ರತಿ_sec = ಅವಧಿ_ಪ್ರತಿ_sec;
  r->ಹೆಜ್ಜೆ_us = CURL_US_ಪ್ರತಿ_SEC;
  r->ಹೆಚ್ಚುವರಿ_us = 0;
  r->ಟೋಕನುಗಳು = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
  r->ts = *pts;
  r->blocked = FALSE;
}

ಶೂನ್ಯ Curl_rlimit_change(ರಚನೆ Curl_rlimit *r,
                        int64_t ದರ_ಪ್ರತಿ_sec,
                        int64_t ಅವಧಿ_ಪ್ರತಿ_sec)
{
  DEBUGASSERT(ದರ_ಪ್ರತಿ_sec >= 0);
  DEBUGASSERT(ಅವಧಿ_ಪ್ರತಿ_sec >= ದರ_ಪ್ರತಿ_sec || !ಅವಧಿ_ಪ್ರತಿ_sec);
  r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ದರ_ಪ್ರತಿ_sec = ದರ_ಪ್ರತಿ_sec;
  r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ಅವಧಿ_ಪ್ರತಿ_sec = ಅವಧಿ_ಪ್ರತಿ_sec;
  r->ಹೆಜ್ಜೆ_us = CURL_US_ಪ್ರತಿ_SEC;
  r->ಹೆಚ್ಚುವರಿ_us = 0;
  ಆದರೆ(r->ಟೋಕನುಗಳು > r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ)
    r->ಟೋಕನುಗಳು = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

ಶೂನ್ಯ Curl_rlimit_ಆರಂಭ(ರಚನೆ Curl_rlimit *r, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts,
                       int64_t ಒಟ್ಟು_ಟೋಕನುಗಳು)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ದರ_ಪ್ರತಿ_sec;
  r->ಅವಧಿ_ಪ್ರತಿ_ಹೆಜ್ಜೆ = r->ಅವಧಿ_ಪ್ರತಿ_sec;
  r->ಹೆಜ್ಜೆ_us = CURL_US_ಪ್ರತಿ_SEC;
  r->ಹೆಚ್ಚುವರಿ_us = 0;
  r->ಟೋಕನುಗಳು = r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
  r->ts = *pts;
  rlimit_tune_ಹೆಜ್ಜೆಗಳು(r, ಒಟ್ಟು_ಟೋಕನುಗಳು);
}

int64_t Curl_rlimit_ಪ್ರತಿ_ಹೆಜ್ಜೆ(ರಚನೆ Curl_rlimit *r)
{
  ಹಿಂತಿರುಗಿ r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ;
}

bool Curl_rlimit_ಸಕ್ರಿಯ(ರಚನೆ Curl_rlimit *r)
{
  ಹಿಂತಿರುಗಿ (r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(ರಚನೆ Curl_rlimit *r)
{
  ಹಿಂತಿರುಗಿ (bool)r->blocked;
}

int64_t Curl_rlimit_avail(ರಚನೆ Curl_rlimit *r,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  ರಚನೆ curltime ts;

  ಆದರೆ(r->blocked)
    ಹಿಂತಿರುಗಿ 0;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ) {
    ಆದರೆ(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    ಹಿಂತಿರುಗಿ r->ಟೋಕನುಗಳು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ INT64_MAX;
}

ಶೂನ್ಯ Curl_rlimit_drain(ರಚನೆ Curl_rlimit *r, size_t ಟೋಕನುಗಳು,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  ರಚನೆ curltime ts;

  ಆದರೆ(r->blocked || !r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#ಆದರೆ 8 <= SIZEOF_ಗಾತ್ರ_T
  ಆದರೆ(ಟೋಕನುಗಳು > INT64_MAX) {
    r->ಟೋಕನುಗಳು = INT64_MAX;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  {
    int64_t val = (int64_t)ಟೋಕನುಗಳು;
    ಆದರೆ((INT64_MIN + val) < r->ಟೋಕನುಗಳು)
      r->ಟೋಕನುಗಳು -= val;
    ಇಲ್ಲದಿದ್ದರೆ
      r->ಟೋಕನುಗಳು = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(ರಚನೆ Curl_rlimit *r,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  ಆದರೆ(r->blocked || !r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ)
    ಹಿಂತಿರುಗಿ 0;
  rlimit_update(r, pts);
  ಆದರೆ(r->ಟೋಕನುಗಳು > 0)
    ಹಿಂತಿರುಗಿ 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->ಹೆಜ್ಜೆ_us - r->ಹೆಚ್ಚುವರಿ_us;
  ಆದರೆ(r->ಟೋಕನುಗಳು < 0) {
    curl_off_t debt_pct = ((-r->ಟೋಕನುಗಳು) * 100 / r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ);
    ಆದರೆ(debt_pct)
      wait_us += (r->ಹೆಜ್ಜೆ_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ಆದರೆ(elapsed_us >= wait_us)
    ಹಿಂತಿರುಗಿ 0;
  wait_us -= elapsed_us;
  ಹಿಂತಿರುಗಿ (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_ಮುಂದಿನ_ಹೆಜ್ಜೆ_ms(ರಚನೆ Curl_rlimit *r,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  ಆದರೆ(!r->blocked && r->ದರ_ಪ್ರತಿ_ಹೆಜ್ಜೆ) {
    timediff_t elapsed_us, ಮುಂದಿನ_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->ಹೆಚ್ಚುವರಿ_us;
    ಆದರೆ(r->ಹೆಜ್ಜೆ_us > elapsed_us) {
      ಮುಂದಿನ_us = r->ಹೆಜ್ಜೆ_us - elapsed_us;
      ಹಿಂತಿರುಗಿ (ಮುಂದಿನ_us + 999) / 1000; /* in milliseconds */
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ Curl_rlimit_ಖಂಡ(ರಚನೆ Curl_rlimit *r,
                       bool activate,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  ಆದರೆ(!activate == !r->blocked)
    ಹಿಂತಿರುಗಿ;

  r->ts = *pts;
  r->blocked = activate;
  ಆದರೆ(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_ಆರಂಭ(r, pts, -1);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    r->ಟೋಕನುಗಳು = 0;
  }
}
