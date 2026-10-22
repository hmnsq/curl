#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_RLIMIT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_RLIMIT_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ Curl_easy;

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

ਬਣਤਰ Curl_rlimit {
  int64_t ਦਰ_ਪ੍ਰਤੀ_sec; /* rate tokens generated per second */
  int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_sec; /* burst rate of tokens per second */
  int64_t ਦਰ_ਪ੍ਰਤੀ_ਕਦਮ; /* rate tokens generated per step us */
  int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_ਕਦਮ; /* burst rate of tokens per step us */
  timediff_t ਕਦਮ_us;     /* microseconds between token increases */
  int64_t ਟੋਕਨ;         /* tokens available in the next second */
  timediff_t ਵਾਧੂ_us;    /* microseconds unaffecting tokens */
  ਬਣਤਰ curltime ts;     /* time of the last update */
  ਦੋਆਧਾਰੀ_ਅੰਕ(blocked);           /* blocking sets available tokens to 0 */
};

ਖਾਲੀ Curl_rlimit_init(ਬਣਤਰ Curl_rlimit *r,
                      int64_t ਦਰ_ਪ੍ਰਤੀ_sec,
                      int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_sec,
                      ਅਚਲ ਬਣਤਰ curltime *pts);

/* Start ratelimiting with the given timestamp. Resets available tokens.
 * `total_tokens` is either -1 or the number of total tokens expected
 * to be consumed. */
/* Change the rates of a limiter that may be in use. Keeps the blocked
 * state and the time already accounted for, caps the tokens at the new
 * rate. */
ਖਾਲੀ Curl_rlimit_change(ਬਣਤਰ Curl_rlimit *r,
                        int64_t ਦਰ_ਪ੍ਰਤੀ_sec,
                        int64_t ਮਿਆਦ_ਪ੍ਰਤੀ_sec);

ਖਾਲੀ Curl_rlimit_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_rlimit *r, ਅਚਲ ਬਣਤਰ curltime *pts,
                       int64_t ਕੁੱਲ_ਟੋਕਨ);

/* How many milliseconds to wait until token are available again. */
timediff_t Curl_rlimit_wait_ms(ਬਣਤਰ Curl_rlimit *r,
                               ਅਚਲ ਬਣਤਰ curltime *pts);

/* When the rate limit will update its tokens again */
timediff_t Curl_rlimit_ਅਗਲਾ_ਕਦਮ_ms(ਬਣਤਰ Curl_rlimit *r,
                                    ਅਚਲ ਬਣਤਰ curltime *pts);

/* Return if rate limiting of tokens is active */
bool Curl_rlimit_ਸਰਗਰਮ(ਬਣਤਰ Curl_rlimit *r);
bool Curl_rlimit_is_blocked(ਬਣਤਰ Curl_rlimit *r);
int64_t Curl_rlimit_ਪ੍ਰਤੀ_ਕਦਮ(ਬਣਤਰ Curl_rlimit *r);

/* Return how many tokens are available to spend, may be negative.
 * Pass timestamp or NULL. */
int64_t Curl_rlimit_avail(ਬਣਤਰ Curl_rlimit *r,
                          ਅਚਲ ਬਣਤਰ curltime *pts);

/* Drain tokens from the ratelimit, give an estimate of how many tokens
 * remain to be drained in the future (-1 for unknown).
 * Pass timestamp or NULL. */
ਖਾਲੀ Curl_rlimit_drain(ਬਣਤਰ Curl_rlimit *r, size_t ਟੋਕਨ,
                       ਅਚਲ ਬਣਤਰ curltime *pts);

/* Block/unblock ratelimiting. A blocked ratelimit has 0 tokens available. */
ਖਾਲੀ Curl_rlimit_ਖੰਡ(ਬਣਤਰ Curl_rlimit *r,
                       bool activate,
                       ਅਚਲ ਬਣਤਰ curltime *pts);

#ਜੇ_ਅੰਤ /* HEADER_CURL_RLIMIT_H */
