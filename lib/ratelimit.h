#اگر_غیر_تعریف_شدہ سرخی_CURL_RLIMIT_H
#تعریف_کرو سرخی_CURL_RLIMIT_H
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
#شامل "curlx/timeval.h"

ساخت Curl_easy;

/* This is a rate limiter that provides "tokens" to be consumed
 * per second. In the literature, this is referred to as a
 * "token bucket" (https://en.wikipedia.org/wiki/Token_bucket).
 * Example:
 * A rate limit of 1 megabyte per second.
 * - initially 1 million tokens are available.
 * - these are drained in the first second.
 * - checking available tokens before the 2nd second will return 0.
 * - at/after the 2nd second, 1 million tokens are available again.
 * - nothing happens for a second, the 1 million tokens would grow
 *   to 2 million, however the burst limit caps those at 1.5 million.
 * Thus:
 * - setting "burst" to CURL_OFF_T_MAX would average tokens over the
 *   complete lifetime. E.g. for a download, at the *end* of it, the
 *   average rate from start to finish would be the rate limit.
 * - setting "burst" to the same value as "rate" would make a
 *   download always try to stay *at/below* the rate and slow times will
 *   not generate extra tokens.
 *
 * A rate limit can be blocked, causing the available tokens to become
 * always 0 until unblocked. After unblocking, the rate limiting starts
 * again with no history of the past.
 *
 * Finally, a rate limiter with rate 0 will always have CURL_OFF_T_MAX
 * tokens available, unless blocked.
 */

ساخت Curl_rlimit {
  int64_t شرح_فی_sec; /* rate tokens generated per second */
  int64_t مدت_فی_sec; /* burst rate of tokens per second */
  int64_t شرح_فی_قدم; /* rate tokens generated per step us */
  int64_t مدت_فی_قدم; /* burst rate of tokens per step us */
  timediff_t قدم_us;     /* microseconds between token increases */
  int64_t علامات;         /* tokens available in the next second */
  timediff_t فالتو_us;    /* microseconds unaffecting tokens */
  ساخت curltime ts;     /* time of the last update */
  ثنائی_ہندسہ(blocked);           /* blocking sets available tokens to 0 */
};

خالی Curl_rlimit_init(ساخت Curl_rlimit *r,
                      int64_t شرح_فی_sec,
                      int64_t مدت_فی_sec,
                      ثابت ساخت curltime *pts);

/* Start ratelimiting with the given timestamp. Resets available tokens.
 * `total_tokens` is either -1 or the number of total tokens expected
 * to be consumed. */
/* Change the rates of a limiter that may be in use. Keeps the blocked
 * state and the time already accounted for, caps the tokens at the new
 * rate. */
خالی Curl_rlimit_change(ساخت Curl_rlimit *r,
                        int64_t شرح_فی_sec,
                        int64_t مدت_فی_sec);

خالی Curl_rlimit_آغاز(ساخت Curl_rlimit *r, ثابت ساخت curltime *pts,
                       int64_t کل_علامات);

/* How many milliseconds to wait until token are available again. */
timediff_t Curl_rlimit_wait_ms(ساخت Curl_rlimit *r,
                               ثابت ساخت curltime *pts);

/* When the rate limit will update its tokens again */
timediff_t Curl_rlimit_اگلا_قدم_ms(ساخت Curl_rlimit *r,
                                    ثابت ساخت curltime *pts);

/* Return if rate limiting of tokens is active */
bool Curl_rlimit_فعال(ساخت Curl_rlimit *r);
bool Curl_rlimit_is_blocked(ساخت Curl_rlimit *r);
int64_t Curl_rlimit_فی_قدم(ساخت Curl_rlimit *r);

/* Return how many tokens are available to spend, may be negative.
 * Pass timestamp or NULL. */
int64_t Curl_rlimit_avail(ساخت Curl_rlimit *r,
                          ثابت ساخت curltime *pts);

/* Drain tokens from the ratelimit, give an estimate of how many tokens
 * remain to be drained in the future (-1 for unknown).
 * Pass timestamp or NULL. */
خالی Curl_rlimit_drain(ساخت Curl_rlimit *r, size_t علامات,
                       ثابت ساخت curltime *pts);

/* Block/unblock ratelimiting. A blocked ratelimit has 0 tokens available. */
خالی Curl_rlimit_بلاک(ساخت Curl_rlimit *r,
                       bool activate,
                       ثابت ساخت curltime *pts);

#اگر_ختم /* HEADER_CURL_RLIMIT_H */
