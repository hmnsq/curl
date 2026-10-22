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

#وصف_ڪريو CURL_US_في_SEC         1000000
#وصف_ڪريو CURL_RLIMIT_MIN_شرح    (4 * 1024)  /* minimum step rate */
#وصف_ڪريو CURL_RLIMIT_قدم_MIN_MS 2  /* minimum step duration */

ساكن فراغ rlimit_update(بنية Curl_rlimit *r,
                          ثابت بنية curltime *pts)
{
  timediff_t elapsed_us, elapsed_قدمن;
  int64_t token_gain;

  DEBUGASSERT(r->شرح_في_قدم);
  إذا((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    أرجع;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    أرجع;
  }

  elapsed_us += r->اضافي_us;
  إذا(elapsed_us < r->قدم_us)
    أرجع;

  /* we do the update */
  r->ts = *pts;
  elapsed_قدمن = elapsed_us / r->قدم_us;
  r->اضافي_us = elapsed_us % r->قدم_us;

  /* How many tokens did we gain since the last update? */
  إذا(r->شرح_في_قدم > (INT64_MAX / elapsed_قدمن))
    token_gain = INT64_MAX;
  وإلا {
    token_gain = r->شرح_في_قدم * elapsed_قدمن;
  }

  إذا((INT64_MAX - token_gain) > r->ٽوڪن)
    r->ٽوڪن += token_gain;
  وإلا
    r->ٽوڪن = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  إذا(r->مدو_في_قدم && (r->ٽوڪن > r->مدو_في_قدم)) {
    r->ٽوڪن = r->مدو_في_قدم;
  }
}

ساكن فراغ rlimit_tune_قدمن(بنية Curl_rlimit *r,
                              int64_t ٽوڪن_ڪل)
{
  int64_t ٽوڪن_آخري, ٽوڪن_main, msteps;

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
  إذا(!r->شرح_في_قدم ||
     (ٽوڪن_ڪل <= 1) ||
     (ٽوڪن_ڪل > (INT64_MAX / 1000)))
    أرجع;

  /* Calculate tokens for the last step and the ones before. */
  ٽوڪن_آخري = ٽوڪن_ڪل / 100;
  إذا(!ٽوڪن_آخري) /* less than 100 total, use 1 */
    ٽوڪن_آخري = 1;
  وإلا إذا(ٽوڪن_آخري > CURL_RLIMIT_MIN_شرح)
    ٽوڪن_آخري = CURL_RLIMIT_MIN_شرح;
  DEBUGASSERT(ٽوڪن_آخري);
  ٽوڪن_main = ٽوڪن_ڪل - ٽوڪن_آخري;
  DEBUGASSERT(ٽوڪن_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->قدم_us == CURL_US_في_SEC);

  msteps = (ٽوڪن_main * 1000 / r->شرح_في_قدم);
  إذا(msteps < CURL_RLIMIT_قدم_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    أرجع;
  }
  وإلا إذا(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->قدم_us = (timediff_t)msteps * 1000;
    r->شرح_في_قدم = ٽوڪن_main;
    r->ٽوڪن = r->شرح_في_قدم;
  }
  وإلا {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    إذا(mstep_inc) {
      curl_off_t شرح_inc = ((r->شرح_في_قدم * mstep_inc) / 1000);
      إذا(شرح_inc) {
        r->قدم_us = CURL_US_في_SEC + ((timediff_t)mstep_inc * 1000);
        r->شرح_في_قدم += شرح_inc;
        r->ٽوڪن = r->شرح_في_قدم;
        إذا(r->مدو_في_قدم) {
          curl_off_t مدو_inc = ((r->مدو_في_قدم * mstep_inc) / 1000);
          إذا(مدو_inc)
            r->مدو_في_قدم += مدو_inc;
        }
      }
    }
  }
}

فراغ Curl_rlimit_init(بنية Curl_rlimit *r,
                      int64_t شرح_في_sec,
                      int64_t مدو_في_sec,
                      ثابت بنية curltime *pts)
{
  DEBUGASSERT(شرح_في_sec >= 0);
  DEBUGASSERT(مدو_في_sec >= شرح_في_sec || !مدو_في_sec);
  DEBUGASSERT(pts);
  r->شرح_في_قدم = r->شرح_في_sec = شرح_في_sec;
  r->مدو_في_قدم = r->مدو_في_sec = مدو_في_sec;
  r->قدم_us = CURL_US_في_SEC;
  r->اضافي_us = 0;
  r->ٽوڪن = r->شرح_في_قدم;
  r->ts = *pts;
  r->blocked = FALSE;
}

فراغ Curl_rlimit_change(بنية Curl_rlimit *r,
                        int64_t شرح_في_sec,
                        int64_t مدو_في_sec)
{
  DEBUGASSERT(شرح_في_sec >= 0);
  DEBUGASSERT(مدو_في_sec >= شرح_في_sec || !مدو_في_sec);
  r->شرح_في_قدم = r->شرح_في_sec = شرح_في_sec;
  r->مدو_في_قدم = r->مدو_في_sec = مدو_في_sec;
  r->قدم_us = CURL_US_في_SEC;
  r->اضافي_us = 0;
  إذا(r->ٽوڪن > r->شرح_في_قدم)
    r->ٽوڪن = r->شرح_في_قدم;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

فراغ Curl_rlimit_شروعات(بنية Curl_rlimit *r, ثابت بنية curltime *pts,
                       int64_t ڪل_ٽوڪن)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->شرح_في_قدم = r->شرح_في_sec;
  r->مدو_في_قدم = r->مدو_في_sec;
  r->قدم_us = CURL_US_في_SEC;
  r->اضافي_us = 0;
  r->ٽوڪن = r->شرح_في_قدم;
  r->ts = *pts;
  rlimit_tune_قدمن(r, ڪل_ٽوڪن);
}

int64_t Curl_rlimit_في_قدم(بنية Curl_rlimit *r)
{
  أرجع r->شرح_في_قدم;
}

bool Curl_rlimit_سرگرم(بنية Curl_rlimit *r)
{
  أرجع (r->شرح_في_قدم > 0) || r->blocked;
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
  وإلا إذا(r->شرح_في_قدم) {
    إذا(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    أرجع r->ٽوڪن;
  }
  وإلا
    أرجع INT64_MAX;
}

فراغ Curl_rlimit_drain(بنية Curl_rlimit *r, size_t ٽوڪن,
                       ثابت بنية curltime *pts)
{
  بنية curltime ts;

  إذا(r->blocked || !r->شرح_في_قدم)
    أرجع;

  إذا(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#إذا 8 <= SIZEOF_سائيز_T
  إذا(ٽوڪن > INT64_MAX) {
    r->ٽوڪن = INT64_MAX;
  }
  وإلا
#جيڪڏهن_پورو
  {
    int64_t val = (int64_t)ٽوڪن;
    إذا((INT64_MIN + val) < r->ٽوڪن)
      r->ٽوڪن -= val;
    وإلا
      r->ٽوڪن = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(بنية Curl_rlimit *r,
                               ثابت بنية curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  إذا(r->blocked || !r->شرح_في_قدم)
    أرجع 0;
  rlimit_update(r, pts);
  إذا(r->ٽوڪن > 0)
    أرجع 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->قدم_us - r->اضافي_us;
  إذا(r->ٽوڪن < 0) {
    curl_off_t debt_pct = ((-r->ٽوڪن) * 100 / r->شرح_في_قدم);
    إذا(debt_pct)
      wait_us += (r->قدم_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  إذا(elapsed_us >= wait_us)
    أرجع 0;
  wait_us -= elapsed_us;
  أرجع (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_ايندڙ_قدم_ms(بنية Curl_rlimit *r,
                                    ثابت بنية curltime *pts)
{
  إذا(!r->blocked && r->شرح_في_قدم) {
    timediff_t elapsed_us, ايندڙ_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->اضافي_us;
    إذا(r->قدم_us > elapsed_us) {
      ايندڙ_us = r->قدم_us - elapsed_us;
      أرجع (ايندڙ_us + 999) / 1000; /* in milliseconds */
    }
  }
  أرجع 0;
}

فراغ Curl_rlimit_بلاڪ(بنية Curl_rlimit *r,
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
    Curl_rlimit_شروعات(r, pts, -1);
  }
  وإلا {
    r->ٽوڪن = 0;
  }
}
