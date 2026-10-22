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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "ratelimit.h"

#تعریف_کرو CURL_US_فی_SEC         1000000
#تعریف_کرو CURL_RLIMIT_MIN_شرح    (4 * 1024)  /* minimum step rate */
#تعریف_کرو CURL_RLIMIT_قدم_MIN_MS 2  /* minimum step duration */

ساکن خالی rlimit_update(ساخت Curl_rlimit *r,
                          ثابت ساخت curltime *pts)
{
  timediff_t elapsed_us, elapsed_اقدام;
  int64_t token_gain;

  DEBUGASSERT(r->شرح_فی_قدم);
  اگر((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    واپس;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  اگر(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    واپس;
  }

  elapsed_us += r->فالتو_us;
  اگر(elapsed_us < r->قدم_us)
    واپس;

  /* we do the update */
  r->ts = *pts;
  elapsed_اقدام = elapsed_us / r->قدم_us;
  r->فالتو_us = elapsed_us % r->قدم_us;

  /* How many tokens did we gain since the last update? */
  اگر(r->شرح_فی_قدم > (INT64_MAX / elapsed_اقدام))
    token_gain = INT64_MAX;
  ورنہ {
    token_gain = r->شرح_فی_قدم * elapsed_اقدام;
  }

  اگر((INT64_MAX - token_gain) > r->علامات)
    r->علامات += token_gain;
  ورنہ
    r->علامات = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  اگر(r->مدت_فی_قدم && (r->علامات > r->مدت_فی_قدم)) {
    r->علامات = r->مدت_فی_قدم;
  }
}

ساکن خالی rlimit_tune_اقدام(ساخت Curl_rlimit *r,
                              int64_t علامات_کل)
{
  int64_t علامات_آخری, علامات_main, msteps;

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
  اگر(!r->شرح_فی_قدم ||
     (علامات_کل <= 1) ||
     (علامات_کل > (INT64_MAX / 1000)))
    واپس;

  /* Calculate tokens for the last step and the ones before. */
  علامات_آخری = علامات_کل / 100;
  اگر(!علامات_آخری) /* less than 100 total, use 1 */
    علامات_آخری = 1;
  ورنہ اگر(علامات_آخری > CURL_RLIMIT_MIN_شرح)
    علامات_آخری = CURL_RLIMIT_MIN_شرح;
  DEBUGASSERT(علامات_آخری);
  علامات_main = علامات_کل - علامات_آخری;
  DEBUGASSERT(علامات_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->قدم_us == CURL_US_فی_SEC);

  msteps = (علامات_main * 1000 / r->شرح_فی_قدم);
  اگر(msteps < CURL_RLIMIT_قدم_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    واپس;
  }
  ورنہ اگر(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->قدم_us = (timediff_t)msteps * 1000;
    r->شرح_فی_قدم = علامات_main;
    r->علامات = r->شرح_فی_قدم;
  }
  ورنہ {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    اگر(mstep_inc) {
      curl_off_t شرح_inc = ((r->شرح_فی_قدم * mstep_inc) / 1000);
      اگر(شرح_inc) {
        r->قدم_us = CURL_US_فی_SEC + ((timediff_t)mstep_inc * 1000);
        r->شرح_فی_قدم += شرح_inc;
        r->علامات = r->شرح_فی_قدم;
        اگر(r->مدت_فی_قدم) {
          curl_off_t مدت_inc = ((r->مدت_فی_قدم * mstep_inc) / 1000);
          اگر(مدت_inc)
            r->مدت_فی_قدم += مدت_inc;
        }
      }
    }
  }
}

خالی Curl_rlimit_init(ساخت Curl_rlimit *r,
                      int64_t شرح_فی_sec,
                      int64_t مدت_فی_sec,
                      ثابت ساخت curltime *pts)
{
  DEBUGASSERT(شرح_فی_sec >= 0);
  DEBUGASSERT(مدت_فی_sec >= شرح_فی_sec || !مدت_فی_sec);
  DEBUGASSERT(pts);
  r->شرح_فی_قدم = r->شرح_فی_sec = شرح_فی_sec;
  r->مدت_فی_قدم = r->مدت_فی_sec = مدت_فی_sec;
  r->قدم_us = CURL_US_فی_SEC;
  r->فالتو_us = 0;
  r->علامات = r->شرح_فی_قدم;
  r->ts = *pts;
  r->blocked = FALSE;
}

خالی Curl_rlimit_change(ساخت Curl_rlimit *r,
                        int64_t شرح_فی_sec,
                        int64_t مدت_فی_sec)
{
  DEBUGASSERT(شرح_فی_sec >= 0);
  DEBUGASSERT(مدت_فی_sec >= شرح_فی_sec || !مدت_فی_sec);
  r->شرح_فی_قدم = r->شرح_فی_sec = شرح_فی_sec;
  r->مدت_فی_قدم = r->مدت_فی_sec = مدت_فی_sec;
  r->قدم_us = CURL_US_فی_SEC;
  r->فالتو_us = 0;
  اگر(r->علامات > r->شرح_فی_قدم)
    r->علامات = r->شرح_فی_قدم;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

خالی Curl_rlimit_آغاز(ساخت Curl_rlimit *r, ثابت ساخت curltime *pts,
                       int64_t کل_علامات)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->شرح_فی_قدم = r->شرح_فی_sec;
  r->مدت_فی_قدم = r->مدت_فی_sec;
  r->قدم_us = CURL_US_فی_SEC;
  r->فالتو_us = 0;
  r->علامات = r->شرح_فی_قدم;
  r->ts = *pts;
  rlimit_tune_اقدام(r, کل_علامات);
}

int64_t Curl_rlimit_فی_قدم(ساخت Curl_rlimit *r)
{
  واپس r->شرح_فی_قدم;
}

bool Curl_rlimit_فعال(ساخت Curl_rlimit *r)
{
  واپس (r->شرح_فی_قدم > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(ساخت Curl_rlimit *r)
{
  واپس (bool)r->blocked;
}

int64_t Curl_rlimit_avail(ساخت Curl_rlimit *r,
                          ثابت ساخت curltime *pts)
{
  ساخت curltime ts;

  اگر(r->blocked)
    واپس 0;
  ورنہ اگر(r->شرح_فی_قدم) {
    اگر(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    واپس r->علامات;
  }
  ورنہ
    واپس INT64_MAX;
}

خالی Curl_rlimit_drain(ساخت Curl_rlimit *r, size_t علامات,
                       ثابت ساخت curltime *pts)
{
  ساخت curltime ts;

  اگر(r->blocked || !r->شرح_فی_قدم)
    واپس;

  اگر(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#اگر 8 <= SIZEOF_سائز_T
  اگر(علامات > INT64_MAX) {
    r->علامات = INT64_MAX;
  }
  ورنہ
#اگر_ختم
  {
    int64_t val = (int64_t)علامات;
    اگر((INT64_MIN + val) < r->علامات)
      r->علامات -= val;
    ورنہ
      r->علامات = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(ساخت Curl_rlimit *r,
                               ثابت ساخت curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  اگر(r->blocked || !r->شرح_فی_قدم)
    واپس 0;
  rlimit_update(r, pts);
  اگر(r->علامات > 0)
    واپس 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->قدم_us - r->فالتو_us;
  اگر(r->علامات < 0) {
    curl_off_t debt_pct = ((-r->علامات) * 100 / r->شرح_فی_قدم);
    اگر(debt_pct)
      wait_us += (r->قدم_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  اگر(elapsed_us >= wait_us)
    واپس 0;
  wait_us -= elapsed_us;
  واپس (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_اگلا_قدم_ms(ساخت Curl_rlimit *r,
                                    ثابت ساخت curltime *pts)
{
  اگر(!r->blocked && r->شرح_فی_قدم) {
    timediff_t elapsed_us, اگلا_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->فالتو_us;
    اگر(r->قدم_us > elapsed_us) {
      اگلا_us = r->قدم_us - elapsed_us;
      واپس (اگلا_us + 999) / 1000; /* in milliseconds */
    }
  }
  واپس 0;
}

خالی Curl_rlimit_بلاک(ساخت Curl_rlimit *r,
                       bool activate,
                       ثابت ساخت curltime *pts)
{
  اگر(!activate == !r->blocked)
    واپس;

  r->ts = *pts;
  r->blocked = activate;
  اگر(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_آغاز(r, pts, -1);
  }
  ورنہ {
    r->علامات = 0;
  }
}
