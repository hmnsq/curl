#నిర్వచితం_కాకపోతే శీర్షిక_CURL_RLIMIT_H
#నిర్వచించు శీర్షిక_CURL_RLIMIT_H
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
#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం Curl_easy;

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

నిర్మాణం Curl_rlimit {
  int64_t రేటు_చొప్పున_sec; /* rate tokens generated per second */
  int64_t వ్యవధి_చొప్పున_sec; /* burst rate of tokens per second */
  int64_t రేటు_చొప్పున_అడుగు; /* rate tokens generated per step us */
  int64_t వ్యవధి_చొప్పున_అడుగు; /* burst rate of tokens per step us */
  timediff_t అడుగు_us;     /* microseconds between token increases */
  int64_t టోకెన్లు;         /* tokens available in the next second */
  timediff_t అదనపు_us;    /* microseconds unaffecting tokens */
  నిర్మాణం curltime ts;     /* time of the last update */
  బిట్టు(blocked);           /* blocking sets available tokens to 0 */
};

శూన్య Curl_rlimit_init(నిర్మాణం Curl_rlimit *r,
                      int64_t రేటు_చొప్పున_sec,
                      int64_t వ్యవధి_చొప్పున_sec,
                      స్థిరరాశి నిర్మాణం curltime *pts);

/* Start ratelimiting with the given timestamp. Resets available tokens.
 * `total_tokens` is either -1 or the number of total tokens expected
 * to be consumed. */
/* Change the rates of a limiter that may be in use. Keeps the blocked
 * state and the time already accounted for, caps the tokens at the new
 * rate. */
శూన్య Curl_rlimit_change(నిర్మాణం Curl_rlimit *r,
                        int64_t రేటు_చొప్పున_sec,
                        int64_t వ్యవధి_చొప్పున_sec);

శూన్య Curl_rlimit_ప్రారంభం(నిర్మాణం Curl_rlimit *r, స్థిరరాశి నిర్మాణం curltime *pts,
                       int64_t మొత్తం_టోకెన్లు);

/* How many milliseconds to wait until token are available again. */
timediff_t Curl_rlimit_wait_ms(నిర్మాణం Curl_rlimit *r,
                               స్థిరరాశి నిర్మాణం curltime *pts);

/* When the rate limit will update its tokens again */
timediff_t Curl_rlimit_తదుపరి_అడుగు_ms(నిర్మాణం Curl_rlimit *r,
                                    స్థిరరాశి నిర్మాణం curltime *pts);

/* Return if rate limiting of tokens is active */
bool Curl_rlimit_చురుకైన(నిర్మాణం Curl_rlimit *r);
bool Curl_rlimit_is_blocked(నిర్మాణం Curl_rlimit *r);
int64_t Curl_rlimit_చొప్పున_అడుగు(నిర్మాణం Curl_rlimit *r);

/* Return how many tokens are available to spend, may be negative.
 * Pass timestamp or NULL. */
int64_t Curl_rlimit_avail(నిర్మాణం Curl_rlimit *r,
                          స్థిరరాశి నిర్మాణం curltime *pts);

/* Drain tokens from the ratelimit, give an estimate of how many tokens
 * remain to be drained in the future (-1 for unknown).
 * Pass timestamp or NULL. */
శూన్య Curl_rlimit_drain(నిర్మాణం Curl_rlimit *r, size_t టోకెన్లు,
                       స్థిరరాశి నిర్మాణం curltime *pts);

/* Block/unblock ratelimiting. A blocked ratelimit has 0 tokens available. */
శూన్య Curl_rlimit_ఖండం(నిర్మాణం Curl_rlimit *r,
                       bool activate,
                       స్థిరరాశి నిర్మాణం curltime *pts);

#అయితే_ముగింపు /* HEADER_CURL_RLIMIT_H */
