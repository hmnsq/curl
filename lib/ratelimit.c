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
#include "curl_setup.h"

#include "urldata.h"
#include "ratelimit.h"

#define CURL_US_PER_SEC         1000000
#define CURL_RLIMIT_MIN_RATE    (4 * 1024)  /* minimum step rate */
#define CURL_RLIMIT_STEP_MIN_MS 2  /* minimum step duration */

static void rlimit_update(sa_nracanaa Curl_rlimit *r,
                          const sa_nracanaa curltime *pts)
{
  timediff_t elapsed_us, elapsed_steps;
  int64_t token_gain;

  DEBUGASSERT(r->rate_per_step);
  ប្រសិនបើ((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    ត្រឡប់;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ប្រសិនបើ(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    ត្រឡប់;
  }

  elapsed_us += r->spare_us;
  ប្រសិនបើ(elapsed_us < r->step_us)
    ត្រឡប់;

  /* we do the update */
  r->ts = *pts;
  elapsed_steps = elapsed_us / r->step_us;
  r->spare_us = elapsed_us % r->step_us;

  /* How many tokens did we gain since the last update? */
  ប្រសិនបើ(r->rate_per_step > (INT64_MAX / elapsed_steps))
    token_gain = INT64_MAX;
  បើមិនដូច្នេះ {
    token_gain = r->rate_per_step * elapsed_steps;
  }

  ប្រសិនបើ((INT64_MAX - token_gain) > r->tokens)
    r->tokens += token_gain;
  បើមិនដូច្នេះ
    r->tokens = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  ប្រសិនបើ(r->burst_per_step && (r->tokens > r->burst_per_step)) {
    r->tokens = r->burst_per_step;
  }
}

static void rlimit_tune_steps(sa_nracanaa Curl_rlimit *r,
                              int64_t tokens_total)
{
  int64_t tokens_last, tokens_main, msteps;

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
  ប្រសិនបើ(!r->rate_per_step ||
     (tokens_total <= 1) ||
     (tokens_total > (INT64_MAX / 1000)))
    ត្រឡប់;

  /* Calculate tokens for the last step and the ones before. */
  tokens_last = tokens_total / 100;
  ប្រសិនបើ(!tokens_last) /* less than 100 total, use 1 */
    tokens_last = 1;
  បើមិនដូច្នេះ ប្រសិនបើ(tokens_last > CURL_RLIMIT_MIN_RATE)
    tokens_last = CURL_RLIMIT_MIN_RATE;
  DEBUGASSERT(tokens_last);
  tokens_main = tokens_total - tokens_last;
  DEBUGASSERT(tokens_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->step_us == CURL_US_PER_SEC);

  msteps = (tokens_main * 1000 / r->rate_per_step);
  ប្រសិនបើ(msteps < CURL_RLIMIT_STEP_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    ត្រឡប់;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->step_us = (timediff_t)msteps * 1000;
    r->rate_per_step = tokens_main;
    r->tokens = r->rate_per_step;
  }
  បើមិនដូច្នេះ {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    ប្រសិនបើ(mstep_inc) {
      curl_off_t rate_inc = ((r->rate_per_step * mstep_inc) / 1000);
      ប្រសិនបើ(rate_inc) {
        r->step_us = CURL_US_PER_SEC + ((timediff_t)mstep_inc * 1000);
        r->rate_per_step += rate_inc;
        r->tokens = r->rate_per_step;
        ប្រសិនបើ(r->burst_per_step) {
          curl_off_t burst_inc = ((r->burst_per_step * mstep_inc) / 1000);
          ប្រសិនបើ(burst_inc)
            r->burst_per_step += burst_inc;
        }
      }
    }
  }
}

void Curl_rlimit_init(sa_nracanaa Curl_rlimit *r,
                      int64_t rate_per_sec,
                      int64_t burst_per_sec,
                      const sa_nracanaa curltime *pts)
{
  DEBUGASSERT(rate_per_sec >= 0);
  DEBUGASSERT(burst_per_sec >= rate_per_sec || !burst_per_sec);
  DEBUGASSERT(pts);
  r->rate_per_step = r->rate_per_sec = rate_per_sec;
  r->burst_per_step = r->burst_per_sec = burst_per_sec;
  r->step_us = CURL_US_PER_SEC;
  r->spare_us = 0;
  r->tokens = r->rate_per_step;
  r->ts = *pts;
  r->blocked = FALSE;
}

void Curl_rlimit_change(sa_nracanaa Curl_rlimit *r,
                        int64_t rate_per_sec,
                        int64_t burst_per_sec)
{
  DEBUGASSERT(rate_per_sec >= 0);
  DEBUGASSERT(burst_per_sec >= rate_per_sec || !burst_per_sec);
  r->rate_per_step = r->rate_per_sec = rate_per_sec;
  r->burst_per_step = r->burst_per_sec = burst_per_sec;
  r->step_us = CURL_US_PER_SEC;
  r->spare_us = 0;
  ប្រសិនបើ(r->tokens > r->rate_per_step)
    r->tokens = r->rate_per_step;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

void Curl_rlimit_start(sa_nracanaa Curl_rlimit *r, const sa_nracanaa curltime *pts,
                       int64_t total_tokens)
{
  /* A start always resets the values to initial defaults, then
   * fine tunes the intervals for the total_tokens expected. */
  r->rate_per_step = r->rate_per_sec;
  r->burst_per_step = r->burst_per_sec;
  r->step_us = CURL_US_PER_SEC;
  r->spare_us = 0;
  r->tokens = r->rate_per_step;
  r->ts = *pts;
  rlimit_tune_steps(r, total_tokens);
}

int64_t Curl_rlimit_per_step(sa_nracanaa Curl_rlimit *r)
{
  ត្រឡប់ r->rate_per_step;
}

bool Curl_rlimit_active(sa_nracanaa Curl_rlimit *r)
{
  ត្រឡប់ (r->rate_per_step > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(sa_nracanaa Curl_rlimit *r)
{
  ត្រឡប់ (bool)r->blocked;
}

int64_t Curl_rlimit_avail(sa_nracanaa Curl_rlimit *r,
                          const sa_nracanaa curltime *pts)
{
  sa_nracanaa curltime ts;

  ប្រសិនបើ(r->blocked)
    ត្រឡប់ 0;
  បើមិនដូច្នេះ ប្រសិនបើ(r->rate_per_step) {
    ប្រសិនបើ(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    ត្រឡប់ r->tokens;
  }
  បើមិនដូច្នេះ
    ត្រឡប់ INT64_MAX;
}

void Curl_rlimit_drain(sa_nracanaa Curl_rlimit *r, size_t tokens,
                       const sa_nracanaa curltime *pts)
{
  sa_nracanaa curltime ts;

  ប្រសិនបើ(r->blocked || !r->rate_per_step)
    ត្រឡប់;

  ប្រសិនបើ(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#ប្រសិនបើ 8 <= SIZEOF_SIZE_T
  ប្រសិនបើ(tokens > INT64_MAX) {
    r->tokens = INT64_MAX;
  }
  បើមិនដូច្នេះ
#endif
  {
    int64_t val = (int64_t)tokens;
    ប្រសិនបើ((INT64_MIN + val) < r->tokens)
      r->tokens -= val;
    បើមិនដូច្នេះ
      r->tokens = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(sa_nracanaa Curl_rlimit *r,
                               const sa_nracanaa curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  ប្រសិនបើ(r->blocked || !r->rate_per_step)
    ត្រឡប់ 0;
  rlimit_update(r, pts);
  ប្រសិនបើ(r->tokens > 0)
    ត្រឡប់ 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->step_us - r->spare_us;
  ប្រសិនបើ(r->tokens < 0) {
    curl_off_t debt_pct = ((-r->tokens) * 100 / r->rate_per_step);
    ប្រសិនបើ(debt_pct)
      wait_us += (r->step_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  ប្រសិនបើ(elapsed_us >= wait_us)
    ត្រឡប់ 0;
  wait_us -= elapsed_us;
  ត្រឡប់ (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_next_step_ms(sa_nracanaa Curl_rlimit *r,
                                    const sa_nracanaa curltime *pts)
{
  ប្រសិនបើ(!r->blocked && r->rate_per_step) {
    timediff_t elapsed_us, next_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->spare_us;
    ប្រសិនបើ(r->step_us > elapsed_us) {
      next_us = r->step_us - elapsed_us;
      ត្រឡប់ (next_us + 999) / 1000; /* in milliseconds */
    }
  }
  ត្រឡប់ 0;
}

void Curl_rlimit_block(sa_nracanaa Curl_rlimit *r,
                       bool activate,
                       const sa_nracanaa curltime *pts)
{
  ប្រសិនបើ(!activate == !r->blocked)
    ត្រឡប់;

  r->ts = *pts;
  r->blocked = activate;
  ប្រសិនបើ(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_start(r, pts, -1);
  }
  បើមិនដូច្នេះ {
    r->tokens = 0;
  }
}
