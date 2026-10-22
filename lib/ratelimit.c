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

#تعریف_کړه CURL_US_فی_SEC         1000000
#تعریف_کړه CURL_RLIMIT_MIN_شرح    (4 * 1024)  /* minimum step rate */
#تعریف_کړه CURL_RLIMIT_ګام_MIN_MS 2  /* minimum step duration */

ساكن فراغ rlimit_update(بنية Curl_rlimit *r,
                          ثابت بنية curltime *pts)
{
  timediff_t elapsed_us, elapsed_ګامونه;
  int64_t token_gain;

  DEBUGASSERT(r->شرح_فی_ګام);
  إذا((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    أرجع;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    أرجع;
  }

  elapsed_us += r->اضافي_us;
  إذا(elapsed_us < r->ګام_us)
    أرجع;

  /* we do the update */
  r->ts = *pts;
  elapsed_ګامونه = elapsed_us / r->ګام_us;
  r->اضافي_us = elapsed_us % r->ګام_us;

  /* How many tokens did we gain since the last update? */
  إذا(r->شرح_فی_ګام > (INT64_MAX / elapsed_ګامونه))
    token_gain = INT64_MAX;
  وإلا {
    token_gain = r->شرح_فی_ګام * elapsed_ګامونه;
  }

  إذا((INT64_MAX - token_gain) > r->نښې)
    r->نښې += token_gain;
  وإلا
    r->نښې = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  إذا(r->موده_فی_ګام && (r->نښې > r->موده_فی_ګام)) {
    r->نښې = r->موده_فی_ګام;
  }
}

ساكن فراغ rlimit_tune_ګامونه(بنية Curl_rlimit *r,
                              int64_t نښې_ټول)
{
  int64_t نښې_وروستی, نښې_main, msteps;

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
  إذا(!r->شرح_فی_ګام ||
     (نښې_ټول <= 1) ||
     (نښې_ټول > (INT64_MAX / 1000)))
    أرجع;

  /* Calculate tokens for the last step and the ones before. */
  نښې_وروستی = نښې_ټول / 100;
  إذا(!نښې_وروستی) /* less than 100 total, use 1 */
    نښې_وروستی = 1;
  وإلا إذا(نښې_وروستی > CURL_RLIMIT_MIN_شرح)
    نښې_وروستی = CURL_RLIMIT_MIN_شرح;
  DEBUGASSERT(نښې_وروستی);
  نښې_main = نښې_ټول - نښې_وروستی;
  DEBUGASSERT(نښې_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->ګام_us == CURL_US_فی_SEC);

  msteps = (نښې_main * 1000 / r->شرح_فی_ګام);
  إذا(msteps < CURL_RLIMIT_ګام_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    أرجع;
  }
  وإلا إذا(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->ګام_us = (timediff_t)msteps * 1000;
    r->شرح_فی_ګام = نښې_main;
    r->نښې = r->شرح_فی_ګام;
  }
  وإلا {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    إذا(mstep_inc) {
      curl_off_t شرح_inc = ((r->شرح_فی_ګام * mstep_inc) / 1000);
      إذا(شرح_inc) {
        r->ګام_us = CURL_US_فی_SEC + ((timediff_t)mstep_inc * 1000);
        r->شرح_فی_ګام += شرح_inc;
        r->نښې = r->شرح_فی_ګام;
        إذا(r->موده_فی_ګام) {
          curl_off_t موده_inc = ((r->موده_فی_ګام * mstep_inc) / 1000);
          إذا(موده_inc)
            r->موده_فی_ګام += موده_inc;
        }
      }
    }
  }
}

فراغ Curl_rlimit_init(بنية Curl_rlimit *r,
                      int64_t شرح_فی_sec,
                      int64_t موده_فی_sec,
                      ثابت بنية curltime *pts)
{
  DEBUGASSERT(شرح_فی_sec >= 0);
  DEBUGASSERT(موده_فی_sec >= شرح_فی_sec || !موده_فی_sec);
  DEBUGASSERT(pts);
  r->شرح_فی_ګام = r->شرح_فی_sec = شرح_فی_sec;
  r->موده_فی_ګام = r->موده_فی_sec = موده_فی_sec;
  r->ګام_us = CURL_US_فی_SEC;
  r->اضافي_us = 0;
  r->نښې = r->شرح_فی_ګام;
  r->ts = *pts;
  r->blocked = FALSE;
}

فراغ Curl_rlimit_change(بنية Curl_rlimit *r,
                        int64_t شرح_فی_sec,
                        int64_t موده_فی_sec)
{
  DEBUGASSERT(شرح_فی_sec >= 0);
  DEBUGASSERT(موده_فی_sec >= شرح_فی_sec || !موده_فی_sec);
  r->شرح_فی_ګام = r->شرح_فی_sec = شرح_فی_sec;
  r->موده_فی_ګام = r->موده_فی_sec = موده_فی_sec;
  r->ګام_us = CURL_US_فی_SEC;
  r->اضافي_us = 0;
  إذا(r->نښې > r->شرح_فی_ګام)
    r->نښې = r->شرح_فی_ګام;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

فراغ Curl_rlimit_پیل(بنية Curl_rlimit *r, ثابت بنية curltime *pts,
                       int64_t ټول_نښې)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->شرح_فی_ګام = r->شرح_فی_sec;
  r->موده_فی_ګام = r->موده_فی_sec;
  r->ګام_us = CURL_US_فی_SEC;
  r->اضافي_us = 0;
  r->نښې = r->شرح_فی_ګام;
  r->ts = *pts;
  rlimit_tune_ګامونه(r, ټول_نښې);
}

int64_t Curl_rlimit_فی_ګام(بنية Curl_rlimit *r)
{
  أرجع r->شرح_فی_ګام;
}

bool Curl_rlimit_فعال(بنية Curl_rlimit *r)
{
  أرجع (r->شرح_فی_ګام > 0) || r->blocked;
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
  وإلا إذا(r->شرح_فی_ګام) {
    إذا(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    أرجع r->نښې;
  }
  وإلا
    أرجع INT64_MAX;
}

فراغ Curl_rlimit_drain(بنية Curl_rlimit *r, size_t نښې,
                       ثابت بنية curltime *pts)
{
  بنية curltime ts;

  إذا(r->blocked || !r->شرح_فی_ګام)
    أرجع;

  إذا(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#إذا 8 <= SIZEOF_اندازه_T
  إذا(نښې > INT64_MAX) {
    r->نښې = INT64_MAX;
  }
  وإلا
#د_که_پای
  {
    int64_t val = (int64_t)نښې;
    إذا((INT64_MIN + val) < r->نښې)
      r->نښې -= val;
    وإلا
      r->نښې = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(بنية Curl_rlimit *r,
                               ثابت بنية curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  إذا(r->blocked || !r->شرح_فی_ګام)
    أرجع 0;
  rlimit_update(r, pts);
  إذا(r->نښې > 0)
    أرجع 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->ګام_us - r->اضافي_us;
  إذا(r->نښې < 0) {
    curl_off_t debt_pct = ((-r->نښې) * 100 / r->شرح_فی_ګام);
    إذا(debt_pct)
      wait_us += (r->ګام_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us >= wait_us)
    أرجع 0;
  wait_us -= elapsed_us;
  أرجع (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_بل_ګام_ms(بنية Curl_rlimit *r,
                                    ثابت بنية curltime *pts)
{
  إذا(!r->blocked && r->شرح_فی_ګام) {
    timediff_t elapsed_us, بل_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->اضافي_us;
    إذا(r->ګام_us > elapsed_us) {
      بل_us = r->ګام_us - elapsed_us;
      أرجع (بل_us + 999) / 1000; /* in milliseconds */
    }
  }
  أرجع 0;
}

فراغ Curl_rlimit_بلاک(بنية Curl_rlimit *r,
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
    Curl_rlimit_پیل(r, pts, -1);
  }
  وإلا {
    r->نښې = 0;
  }
}
