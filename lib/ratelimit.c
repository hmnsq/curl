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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "ratelimit.h"

#నిర్వచించు CURL_US_చొప్పున_SEC         1000000
#నిర్వచించు CURL_RLIMIT_MIN_రేటు    (4 * 1024)  /* minimum step rate */
#నిర్వచించు CURL_RLIMIT_అడుగు_MIN_MS 2  /* minimum step duration */

స్థిర శూన్య rlimit_update(నిర్మాణం Curl_rlimit *r,
                          స్థిరరాశి నిర్మాణం curltime *pts)
{
  timediff_t elapsed_us, elapsed_అడుగులు;
  int64_t token_gain;

  DEBUGASSERT(r->రేటు_చొప్పున_అడుగు);
  ఒకవేళ((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    తిరిగి;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ఒకవేళ(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    తిరిగి;
  }

  elapsed_us += r->అదనపు_us;
  ఒకవేళ(elapsed_us < r->అడుగు_us)
    తిరిగి;

  /* we do the update */
  r->ts = *pts;
  elapsed_అడుగులు = elapsed_us / r->అడుగు_us;
  r->అదనపు_us = elapsed_us % r->అడుగు_us;

  /* How many tokens did we gain since the last update? */
  ఒకవేళ(r->రేటు_చొప్పున_అడుగు > (INT64_MAX / elapsed_అడుగులు))
    token_gain = INT64_MAX;
  లేకపోతే {
    token_gain = r->రేటు_చొప్పున_అడుగు * elapsed_అడుగులు;
  }

  ఒకవేళ((INT64_MAX - token_gain) > r->టోకెన్లు)
    r->టోకెన్లు += token_gain;
  లేకపోతే
    r->టోకెన్లు = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  ఒకవేళ(r->వ్యవధి_చొప్పున_అడుగు && (r->టోకెన్లు > r->వ్యవధి_చొప్పున_అడుగు)) {
    r->టోకెన్లు = r->వ్యవధి_చొప్పున_అడుగు;
  }
}

స్థిర శూన్య rlimit_tune_అడుగులు(నిర్మాణం Curl_rlimit *r,
                              int64_t టోకెన్లు_మొత్తం)
{
  int64_t టోకెన్లు_చివరి, టోకెన్లు_main, msteps;

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
  ఒకవేళ(!r->రేటు_చొప్పున_అడుగు ||
     (టోకెన్లు_మొత్తం <= 1) ||
     (టోకెన్లు_మొత్తం > (INT64_MAX / 1000)))
    తిరిగి;

  /* Calculate tokens for the last step and the ones before. */
  టోకెన్లు_చివరి = టోకెన్లు_మొత్తం / 100;
  ఒకవేళ(!టోకెన్లు_చివరి) /* less than 100 total, use 1 */
    టోకెన్లు_చివరి = 1;
  లేకపోతే ఒకవేళ(టోకెన్లు_చివరి > CURL_RLIMIT_MIN_రేటు)
    టోకెన్లు_చివరి = CURL_RLIMIT_MIN_రేటు;
  DEBUGASSERT(టోకెన్లు_చివరి);
  టోకెన్లు_main = టోకెన్లు_మొత్తం - టోకెన్లు_చివరి;
  DEBUGASSERT(టోకెన్లు_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->అడుగు_us == CURL_US_చొప్పున_SEC);

  msteps = (టోకెన్లు_main * 1000 / r->రేటు_చొప్పున_అడుగు);
  ఒకవేళ(msteps < CURL_RLIMIT_అడుగు_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    తిరిగి;
  }
  లేకపోతే ఒకవేళ(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->అడుగు_us = (timediff_t)msteps * 1000;
    r->రేటు_చొప్పున_అడుగు = టోకెన్లు_main;
    r->టోకెన్లు = r->రేటు_చొప్పున_అడుగు;
  }
  లేకపోతే {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    ఒకవేళ(mstep_inc) {
      curl_off_t రేటు_inc = ((r->రేటు_చొప్పున_అడుగు * mstep_inc) / 1000);
      ఒకవేళ(రేటు_inc) {
        r->అడుగు_us = CURL_US_చొప్పున_SEC + ((timediff_t)mstep_inc * 1000);
        r->రేటు_చొప్పున_అడుగు += రేటు_inc;
        r->టోకెన్లు = r->రేటు_చొప్పున_అడుగు;
        ఒకవేళ(r->వ్యవధి_చొప్పున_అడుగు) {
          curl_off_t వ్యవధి_inc = ((r->వ్యవధి_చొప్పున_అడుగు * mstep_inc) / 1000);
          ఒకవేళ(వ్యవధి_inc)
            r->వ్యవధి_చొప్పున_అడుగు += వ్యవధి_inc;
        }
      }
    }
  }
}

శూన్య Curl_rlimit_init(నిర్మాణం Curl_rlimit *r,
                      int64_t రేటు_చొప్పున_sec,
                      int64_t వ్యవధి_చొప్పున_sec,
                      స్థిరరాశి నిర్మాణం curltime *pts)
{
  DEBUGASSERT(రేటు_చొప్పున_sec >= 0);
  DEBUGASSERT(వ్యవధి_చొప్పున_sec >= రేటు_చొప్పున_sec || !వ్యవధి_చొప్పున_sec);
  DEBUGASSERT(pts);
  r->రేటు_చొప్పున_అడుగు = r->రేటు_చొప్పున_sec = రేటు_చొప్పున_sec;
  r->వ్యవధి_చొప్పున_అడుగు = r->వ్యవధి_చొప్పున_sec = వ్యవధి_చొప్పున_sec;
  r->అడుగు_us = CURL_US_చొప్పున_SEC;
  r->అదనపు_us = 0;
  r->టోకెన్లు = r->రేటు_చొప్పున_అడుగు;
  r->ts = *pts;
  r->blocked = FALSE;
}

శూన్య Curl_rlimit_change(నిర్మాణం Curl_rlimit *r,
                        int64_t రేటు_చొప్పున_sec,
                        int64_t వ్యవధి_చొప్పున_sec)
{
  DEBUGASSERT(రేటు_చొప్పున_sec >= 0);
  DEBUGASSERT(వ్యవధి_చొప్పున_sec >= రేటు_చొప్పున_sec || !వ్యవధి_చొప్పున_sec);
  r->రేటు_చొప్పున_అడుగు = r->రేటు_చొప్పున_sec = రేటు_చొప్పున_sec;
  r->వ్యవధి_చొప్పున_అడుగు = r->వ్యవధి_చొప్పున_sec = వ్యవధి_చొప్పున_sec;
  r->అడుగు_us = CURL_US_చొప్పున_SEC;
  r->అదనపు_us = 0;
  ఒకవేళ(r->టోకెన్లు > r->రేటు_చొప్పున_అడుగు)
    r->టోకెన్లు = r->రేటు_చొప్పున_అడుగు;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

శూన్య Curl_rlimit_ప్రారంభం(నిర్మాణం Curl_rlimit *r, స్థిరరాశి నిర్మాణం curltime *pts,
                       int64_t మొత్తం_టోకెన్లు)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->రేటు_చొప్పున_అడుగు = r->రేటు_చొప్పున_sec;
  r->వ్యవధి_చొప్పున_అడుగు = r->వ్యవధి_చొప్పున_sec;
  r->అడుగు_us = CURL_US_చొప్పున_SEC;
  r->అదనపు_us = 0;
  r->టోకెన్లు = r->రేటు_చొప్పున_అడుగు;
  r->ts = *pts;
  rlimit_tune_అడుగులు(r, మొత్తం_టోకెన్లు);
}

int64_t Curl_rlimit_చొప్పున_అడుగు(నిర్మాణం Curl_rlimit *r)
{
  తిరిగి r->రేటు_చొప్పున_అడుగు;
}

bool Curl_rlimit_చురుకైన(నిర్మాణం Curl_rlimit *r)
{
  తిరిగి (r->రేటు_చొప్పున_అడుగు > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(నిర్మాణం Curl_rlimit *r)
{
  తిరిగి (bool)r->blocked;
}

int64_t Curl_rlimit_avail(నిర్మాణం Curl_rlimit *r,
                          స్థిరరాశి నిర్మాణం curltime *pts)
{
  నిర్మాణం curltime ts;

  ఒకవేళ(r->blocked)
    తిరిగి 0;
  లేకపోతే ఒకవేళ(r->రేటు_చొప్పున_అడుగు) {
    ఒకవేళ(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    తిరిగి r->టోకెన్లు;
  }
  లేకపోతే
    తిరిగి INT64_MAX;
}

శూన్య Curl_rlimit_drain(నిర్మాణం Curl_rlimit *r, size_t టోకెన్లు,
                       స్థిరరాశి నిర్మాణం curltime *pts)
{
  నిర్మాణం curltime ts;

  ఒకవేళ(r->blocked || !r->రేటు_చొప్పున_అడుగు)
    తిరిగి;

  ఒకవేళ(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#ఒకవేళ 8 <= SIZEOF_కొలత_T
  ఒకవేళ(టోకెన్లు > INT64_MAX) {
    r->టోకెన్లు = INT64_MAX;
  }
  లేకపోతే
#అయితే_ముగింపు
  {
    int64_t val = (int64_t)టోకెన్లు;
    ఒకవేళ((INT64_MIN + val) < r->టోకెన్లు)
      r->టోకెన్లు -= val;
    లేకపోతే
      r->టోకెన్లు = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(నిర్మాణం Curl_rlimit *r,
                               స్థిరరాశి నిర్మాణం curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  ఒకవేళ(r->blocked || !r->రేటు_చొప్పున_అడుగు)
    తిరిగి 0;
  rlimit_update(r, pts);
  ఒకవేళ(r->టోకెన్లు > 0)
    తిరిగి 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->అడుగు_us - r->అదనపు_us;
  ఒకవేళ(r->టోకెన్లు < 0) {
    curl_off_t debt_pct = ((-r->టోకెన్లు) * 100 / r->రేటు_చొప్పున_అడుగు);
    ఒకవేళ(debt_pct)
      wait_us += (r->అడుగు_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ఒకవేళ(elapsed_us >= wait_us)
    తిరిగి 0;
  wait_us -= elapsed_us;
  తిరిగి (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_తదుపరి_అడుగు_ms(నిర్మాణం Curl_rlimit *r,
                                    స్థిరరాశి నిర్మాణం curltime *pts)
{
  ఒకవేళ(!r->blocked && r->రేటు_చొప్పున_అడుగు) {
    timediff_t elapsed_us, తదుపరి_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->అదనపు_us;
    ఒకవేళ(r->అడుగు_us > elapsed_us) {
      తదుపరి_us = r->అడుగు_us - elapsed_us;
      తిరిగి (తదుపరి_us + 999) / 1000; /* in milliseconds */
    }
  }
  తిరిగి 0;
}

శూన్య Curl_rlimit_ఖండం(నిర్మాణం Curl_rlimit *r,
                       bool activate,
                       స్థిరరాశి నిర్మాణం curltime *pts)
{
  ఒకవేళ(!activate == !r->blocked)
    తిరిగి;

  r->ts = *pts;
  r->blocked = activate;
  ఒకవేళ(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_ప్రారంభం(r, pts, -1);
  }
  లేకపోతే {
    r->టోకెన్లు = 0;
  }
}
