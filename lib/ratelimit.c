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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "ratelimit.h"

#define CURL_US_PER_SEC         1000000
#define CURL_RLIMIT_MIN_RATE    (4 * 1024)  /* minimum step rate */
#define CURL_RLIMIT_STEP_MIN_MS 2  /* minimum step duration */

સ્થિર શૂન્ય rlimit_update(સંરચના Curl_rlimit *r,
                          અચલ સંરચના curltime *pts)
{
  timediff_t elapsed_us, elapsed_steps;
  int64_t token_gain;

  DEBUGASSERT(r->rate_per_step);
  જો((r->ts.tv_sec == pts->tv_sec) && (r->ts.tv_usec == pts->tv_usec))
    પાછા;

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  જો(elapsed_us < 0) { /* not going back in time */
    DEBUGASSERT(0);
    પાછા;
  }

  elapsed_us += r->spare_us;
  જો(elapsed_us < r->step_us)
    પાછા;

  /* we do the update */
  r->ts = *pts;
  elapsed_steps = elapsed_us / r->step_us;
  r->spare_us = elapsed_us % r->step_us;

  /* How many tokens did we gain since the last update? */
  જો(r->rate_per_step > (INT64_MAX / elapsed_steps))
    token_gain = INT64_MAX;
  નહીંતર {
    token_gain = r->rate_per_step * elapsed_steps;
  }

  જો((INT64_MAX - token_gain) > r->tokens)
    r->tokens += token_gain;
  નહીંતર
    r->tokens = INT64_MAX;

  /* Limit the token again by the burst rate (if set), so we
   * do not suddenly have a huge number of tokens after inactivity. */
  જો(r->burst_per_step && (r->tokens > r->burst_per_step)) {
    r->tokens = r->burst_per_step;
  }
}

સ્થિર શૂન્ય rlimit_tune_steps(સંરચના Curl_rlimit *r,
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
  જો(!r->rate_per_step ||
     (tokens_total <= 1) ||
     (tokens_total > (INT64_MAX / 1000)))
    પાછા;

  /* Calculate tokens for the last step and the ones before. */
  tokens_last = tokens_total / 100;
  જો(!tokens_last) /* less than 100 total, use 1 */
    tokens_last = 1;
  નહીંતર જો(tokens_last > CURL_RLIMIT_MIN_RATE)
    tokens_last = CURL_RLIMIT_MIN_RATE;
  DEBUGASSERT(tokens_last);
  tokens_main = tokens_total - tokens_last;
  DEBUGASSERT(tokens_main);

  /* how many milli-steps will it take to consume those, give the
  * original rate limit per second? */
  DEBUGASSERT(r->step_us == CURL_US_PER_SEC);

  msteps = (tokens_main * 1000 / r->rate_per_step);
  જો(msteps < CURL_RLIMIT_STEP_MIN_MS) {
    /* Steps this small will not work. Do not tune. */
    પાછા;
  }
  નહીંતર જો(msteps < 1000) {
    /* It needs less than one step to provide the needed tokens.
     * Make it exactly that long and with exactly those tokens. */
    r->step_us = (timediff_t)msteps * 1000;
    r->rate_per_step = tokens_main;
    r->tokens = r->rate_per_step;
  }
  નહીંતર {
    /* More than 1 step. Spread the remainder milli steps and
     * the tokens they need to provide across all steps. If integer
     * arithmetic can do it. */
    curl_off_t ms_unaccounted = (msteps % 1000);
    curl_off_t mstep_inc = (ms_unaccounted / (msteps / 1000));
    જો(mstep_inc) {
      curl_off_t rate_inc = ((r->rate_per_step * mstep_inc) / 1000);
      જો(rate_inc) {
        r->step_us = CURL_US_PER_SEC + ((timediff_t)mstep_inc * 1000);
        r->rate_per_step += rate_inc;
        r->tokens = r->rate_per_step;
        જો(r->burst_per_step) {
          curl_off_t burst_inc = ((r->burst_per_step * mstep_inc) / 1000);
          જો(burst_inc)
            r->burst_per_step += burst_inc;
        }
      }
    }
  }
}

શૂન્ય Curl_rlimit_init(સંરચના Curl_rlimit *r,
                      int64_t rate_per_sec,
                      int64_t burst_per_sec,
                      અચલ સંરચના curltime *pts)
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

શૂન્ય Curl_rlimit_change(સંરચના Curl_rlimit *r,
                        int64_t rate_per_sec,
                        int64_t burst_per_sec)
{
  DEBUGASSERT(rate_per_sec >= 0);
  DEBUGASSERT(burst_per_sec >= rate_per_sec || !burst_per_sec);
  r->rate_per_step = r->rate_per_sec = rate_per_sec;
  r->burst_per_step = r->burst_per_sec = burst_per_sec;
  r->step_us = CURL_US_PER_SEC;
  r->spare_us = 0;
  જો(r->tokens > r->rate_per_step)
    r->tokens = r->rate_per_step;
  /* ts and blocked stay as they are, a paused transfer stays paused */
}

શૂન્ય Curl_rlimit_start(સંરચના Curl_rlimit *r, અચલ સંરચના curltime *pts,
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

int64_t Curl_rlimit_per_step(સંરચના Curl_rlimit *r)
{
  પાછા r->rate_per_step;
}

bool Curl_rlimit_active(સંરચના Curl_rlimit *r)
{
  પાછા (r->rate_per_step > 0) || r->blocked;
}

bool Curl_rlimit_is_blocked(સંરચના Curl_rlimit *r)
{
  પાછા (bool)r->blocked;
}

int64_t Curl_rlimit_avail(સંરચના Curl_rlimit *r,
                          અચલ સંરચના curltime *pts)
{
  સંરચના curltime ts;

  જો(r->blocked)
    પાછા 0;
  નહીંતર જો(r->rate_per_step) {
    જો(!pts) {
      curlx_pnow(&ts);
      pts = &ts;
    }
    rlimit_update(r, pts);
    પાછા r->tokens;
  }
  નહીંતર
    પાછા INT64_MAX;
}

શૂન્ય Curl_rlimit_drain(સંરચના Curl_rlimit *r, size_t tokens,
                       અચલ સંરચના curltime *pts)
{
  સંરચના curltime ts;

  જો(r->blocked || !r->rate_per_step)
    પાછા;

  જો(!pts) {
    curlx_pnow(&ts);
    pts = &ts;
  }
  rlimit_update(r, pts);
#જો 8 <= SIZEOF_SIZE_T
  જો(tokens > INT64_MAX) {
    r->tokens = INT64_MAX;
  }
  નહીંતર
#endif
  {
    int64_t val = (int64_t)tokens;
    જો((INT64_MIN + val) < r->tokens)
      r->tokens -= val;
    નહીંતર
      r->tokens = INT64_MIN;
  }
}

timediff_t Curl_rlimit_wait_ms(સંરચના Curl_rlimit *r,
                               અચલ સંરચના curltime *pts)
{
  timediff_t wait_us, elapsed_us;

  જો(r->blocked || !r->rate_per_step)
    પાછા 0;
  rlimit_update(r, pts);
  જો(r->tokens > 0)
    પાછા 0;

  /* How much time will it take tokens to become positive again?
   * Deduct `spare_us` and check against already elapsed time */
  wait_us = r->step_us - r->spare_us;
  જો(r->tokens < 0) {
    curl_off_t debt_pct = ((-r->tokens) * 100 / r->rate_per_step);
    જો(debt_pct)
      wait_us += (r->step_us * debt_pct / 100);
  }

  elapsed_us = curlx_ptimediff_us(pts, &r->ts);
  જો(elapsed_us >= wait_us)
    પાછા 0;
  wait_us -= elapsed_us;
  પાછા (wait_us + 999) / 1000; /* in milliseconds */
}

timediff_t Curl_rlimit_next_step_ms(સંરચના Curl_rlimit *r,
                                    અચલ સંરચના curltime *pts)
{
  જો(!r->blocked && r->rate_per_step) {
    timediff_t elapsed_us, next_us;

    elapsed_us = curlx_ptimediff_us(pts, &r->ts) + r->spare_us;
    જો(r->step_us > elapsed_us) {
      next_us = r->step_us - elapsed_us;
      પાછા (next_us + 999) / 1000; /* in milliseconds */
    }
  }
  પાછા 0;
}

શૂન્ય Curl_rlimit_block(સંરચના Curl_rlimit *r,
                       bool activate,
                       અચલ સંરચના curltime *pts)
{
  જો(!activate == !r->blocked)
    પાછા;

  r->ts = *pts;
  r->blocked = activate;
  જો(!r->blocked) {
    /* Start rate limiting fresh. The amount of time this was blocked
     * does not generate extra tokens. */
    Curl_rlimit_start(r, pts, -1);
  }
  નહીંતર {
    r->tokens = 0;
  }
}
