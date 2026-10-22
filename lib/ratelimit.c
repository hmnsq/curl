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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "ratelimit.h"

#تعریف_کن CURL_US_در_هر_SEC         1000000
#تعریف_کن CURL_RLIMIT_MIN_نرخ    (4 * 1024)  /* minimum step rate */
#تعریف_کن CURL_RLIMIT_گام_MIN_MS 2  /* minimum step duration */

ساكن فراغ rlimit_update(بنية Curl_rlimit *r,
                          ثابت بنية curltime *pts)
{
  timediff_t elapsed_us, elapsed_گامها;
  int64_t token_gain;

  DEBUGASSERT(r->نرخ_در_هر_گام);
  إذا((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    أرجع;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    أرجع;
  }

  elapsed_us += r->یدکی_us;
  إذا(elapsed_us < r->گام_us)
    أرجع;

  /* we do the update */
  r->ts = *pts;
  elapsed_گامها = elapsed_us / r->گام_us;
  r->یدکی_us = elapsed_us % r->گام_us;

  /* How many tokens did we gain since the last update? */
  إذا(r->نرخ_در_هر_گام > (INT64_MAX / elapsed_گامها))
    token_gain = INT64_MAX;
  وإلا {
    token_gain = r->نرخ_در_هر_گام * elapsed_گامها;
  }

  إذا((INT64_MAX - token_gain) > r->نشانهها)
    r->نشانهها += token_gain;
  وإلا
    r->نشانهها = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  إذا(r->مدت_در_هر_گام && (r->نشانهها > r->مدت_در_هر_گام)) {
    r->نشانهها = r->مدت_در_هر_گام;
  }
}

ساكن فراغ rlimit_tune_گامها(بنية Curl_rlimit *r,
                              int64_t نشانهها_مجموع)
{
  int64_t نشانهها_آخرین, نشانهها_main, msteps;

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
  إذا(!r->نرخ_در_هر_گام ||
     (نشانهها_مجموع <= 1) ||
     (نشانهها_مجموع > (INT64_MAX / 1000)))
    أرجع;

  /* Calculate tokens for the last step and the ones before. */
  نشانهها_آخرین = نشانهها_مجموع / 100;
  إذا(!نشانهها_آخرین) /* less than 100 total, use 1 */
    نشانهها_آخرین = 1;
  وإلا إذا(نشانهها_آخرین > CURL_RLIMIT_MIN_نرخ)
    نشانهها_آخرین = CURL_RLIMIT_MIN_نرخ;
  DEBUGASSERT(نشانهها_آخرین);
  نشانهها_main = نشانهها_مجموع - نشانهها_آخرین;
  DEBUGASSERT(نشانهها_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->گام_us == CURL_US_در_هر_SEC);

  msteps = (نشانهها_main * 1000 / r->نرخ_در_هر_گام);
  إذا(msteps < CURL_RLIMIT_گام_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    أرجع;
  }
  وإلا إذا(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->گام_us = (timediff_t)msteps * 1000;
    r->نرخ_در_هر_گام = نشانهها_main;
    r->نشانهها = r->نرخ_در_هر_گام;
  }
  وإلا {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    إذا(mstep_inc) {
      curl_off_t نرخ_inc = ((r->نرخ_در_هر_گام * mstep_inc) / 1000);
      إذا(نرخ_inc) {
        r->گام_us = CURL_US_در_هر_SEC + ((timediff_t)mstep_inc * 1000);
        r->نرخ_در_هر_گام += نرخ_inc;
        r->نشانهها = r->نرخ_در_هر_گام;
        إذا(r->مدت_در_هر_گام) {
          curl_off_t مدت_inc = ((r->مدت_در_هر_گام * mstep_inc) / 1000);
          إذا(مدت_inc)
            r->مدت_در_هر_گام += مدت_inc;
        }
      }
    }
  }
}

فراغ Curl_rlimit_init(بنية Curl_rlimit *r,
                      int64_t نرخ_در_هر_sec,
                      int64_t مدت_در_هر_sec,
                      ثابت بنية curltime *pts)
{
  DEBUGASSERT(نرخ_در_هر_sec >= 0);
  DEBUGASSERT(مدت_در_هر_sec >= نرخ_در_هر_sec || !مدت_در_هر_sec);
  DEBUGASSERT(pts);
  r->نرخ_در_هر_گام = r->نرخ_در_هر_sec = نرخ_در_هر_sec;
  r->مدت_در_هر_گام = r->مدت_در_هر_sec = مدت_در_هر_sec;
  r->گام_us = CURL_US_در_هر_SEC;
  r->یدکی_us = 0;
  r->نشانهها = r->نرخ_در_هر_گام;
  r->ts = *pts;
  r->blocked = FALSE;
}

فراغ Curl_rlimit_change(بنية Curl_rlimit *r,
                        int64_t نرخ_در_هر_sec,
                        int64_t مدت_در_هر_sec)
{
  DEBUGASSERT(نرخ_در_هر_sec >= 0);
  DEBUGASSERT(مدت_در_هر_sec >= نرخ_در_هر_sec || !مدت_در_هر_sec);
  r->نرخ_در_هر_گام = r->نرخ_در_هر_sec = نرخ_در_هر_sec;
  r->مدت_در_هر_گام = r->مدت_در_هر_sec = مدت_در_هر_sec;
  r->گام_us = CURL_US_در_هر_SEC;
  r->یدکی_us = 0;
  إذا(r->نشانهها > r->نرخ_در_هر_گام)
    r->نشانهها = r->نرخ_در_هر_گام;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

فراغ Curl_rlimit_شروع(بنية Curl_rlimit *r, ثابت بنية curltime *pts,
                       int64_t مجموع_نشانهها)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->نرخ_در_هر_گام = r->نرخ_در_هر_sec;
  r->مدت_در_هر_گام = r->مدت_در_هر_sec;
  r->گام_us = CURL_US_در_هر_SEC;
  r->یدکی_us = 0;
  r->نشانهها = r->نرخ_در_هر_گام;
  r->ts = *pts;
  rlimit_tune_گامها(r, مجموع_نشانهها);
}

int64_t Curl_rlimit_در_هر_گام(بنية Curl_rlimit *r)
{
  أرجع r->نرخ_در_هر_گام;
}

bool Curl_rlimit_فعال(بنية Curl_rlimit *r)
{
  أرجع (r->نرخ_در_هر_گام > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(بنية Curl_rlimit *r)
{
  أرجع (bool)r->blocked;
}

int64_t Curl_rlimit_avail(بنية Curl_rlimit *r,
                          ثابت بنية curltime *pts)
{
  بنية curltime ts;

  إذا(r->blocked)
    أرجع 0;
  وإلا إذا(r->نرخ_در_هر_گام) {
    إذا(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    أرجع r->نشانهها;
  }
  وإلا
    أرجع INT64_MAX;
}

فراغ Curl_rlimit_drain(بنية Curl_rlimit *r, size_t نشانهها,
                       ثابت بنية curltime *pts)
{
  بنية curltime ts;

  إذا(r->blocked || !r->نرخ_در_هر_گام)
    أرجع;

  إذا(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#إذا 8 <= SIZEOF_اندازه_T
  إذا(نشانهها > INT64_MAX) {
    r->نشانهها = INT64_MAX;
  }
  وإلا
#پایان_اگر
  {
    int64_t val = (int64_t)نشانهها;
    إذا((INT64_MIN + val) < r->نشانهها)
      r->نشانهها -= val;
    وإلا
      r->نشانهها = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(بنية Curl_rlimit *r,
                               ثابت بنية curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  إذا(r->blocked || !r->نرخ_در_هر_گام)
    أرجع 0;
  rlimit_update(r, pts);
  إذا(r->نشانهها > 0)
    أرجع 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->گام_us - r->یدکی_us;
  إذا(r->نشانهها < 0) {
    curl_off_t debt_pct = ((-r->نشانهها) * 100 / r->نرخ_در_هر_گام);
    إذا(debt_pct)
      wait_us += (r->گام_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us >= wait_us)
    أرجع 0;
  wait_us -= elapsed_us;
  أرجع (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_بعدی_گام_ms(بنية Curl_rlimit *r,
                                    ثابت بنية curltime *pts)
{
  إذا(!r->blocked && r->نرخ_در_هر_گام) {
    timediff_t elapsed_us, بعدی_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->یدکی_us;
    إذا(r->گام_us > elapsed_us) {
      بعدی_us = r->گام_us - elapsed_us;
      أرجع (بعدی_us + 999) / 1000; /* in milliseconds */
    }
  }
  أرجع 0;
}

فراغ Curl_rlimit_بلوک(بنية Curl_rlimit *r,
                       bool activate,
                       ثابت بنية curltime *pts)
{
  إذا(!activate == !r->blocked)
    أرجع;

  r->ts = *pts;
  r->blocked = activate;
  إذا(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_شروع(r, pts, -1);
  }
  وإلا {
    r->نشانهها = 0;
  }
}
