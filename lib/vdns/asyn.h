#ifndef HEADER_CURL_ASYN_H
#define HEADER_CURL_ASYN_H
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

#اگر defined(USE_HTTPSRR) && defined(USE_ARES)
#شامل "vdns/httpsrr.h"
#endif

ساختار Curl_easy;
ساختار Curl_dns_entry;
ساختار Curl_resolv_async;
ساختار Curl_multi;
ساختار easy_pollset;

#ifdef CURLRES_ASYNCH

#شامل "curl_addrinfo.h"

ساختار hostent;
ساختار connectdata;
ساختار easy_pollset;

#اگر defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#error cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#endif

/*
 * This header defines all functions in the internal asynch resolver interface.
 * All asynch resolvers need to provide these functions.
 * asyn-ares.c and asyn-thread.c are the current implementations of asynch
 * resolver backends.
 */

/*
 * Curl_async_global_init()
 *
 * Called from curl_global_init() to initialize global resolver environment.
 * Returning anything else than CURLE_OK fails curl_global_init().
 */
صحیح Curl_async_global_init(تهی);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
تهی Curl_async_global_cleanup(تهی);

/*
 * Curl_async_getaddrinfo() - when using this resolver
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'hostent' is returned and the fourth argument will point to
 * memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 *
 * Each resolver backend must of course make sure to return data in the
 * correct format to comply with this.
 */
CURLcode Curl_async_getaddrinfo(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async);

#ifdef USE_HTTPSRR
bool Curl_async_knows_https(ساختار Curl_easy *data,
                            ساختار Curl_resolv_async *async);
#endif /* USE_HTTPSRR */

#ifdef USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#شامل <ares.h>

CURLcode Curl_ares_pollset(ساختار Curl_easy *data,
                           ares_channel channel,
                           ساختار easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async,
                                ares_channel channel);

صحیح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#endif

#ifdef USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
ساختار async_ares_ctx {
  ares_channel channel;
  صحیح ares_status;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode result;               /* CURLE_OK or error handling response */
  BIT(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

تهی Curl_async_ares_shutdown(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async);
تهی Curl_async_ares_destroy(ساختار Curl_easy *data,
                             ساختار Curl_resolv_async *async);

#endif /* USE_RESOLV_ARES */

#ifdef USE_RESOLV_THREADED

ساختار async_thrdd_item;

/* Context for threaded resolver */
ساختار async_thrdd_ctx {
  ساختار async_thrdd_item *res_A; /* IPv4 final result */
  ساختار async_thrdd_item *res_AAAA; /* IPv6 final result */
#اگر defined(USE_HTTPSRR) && defined(USE_ARES)
  ساختار {
    ares_channel channel;
  } rr;
#endif
  BIT(processed_A);
  BIT(processed_AAAA);
};

تهی Curl_async_thrdd_shutdown(ساختار Curl_easy *data,
                               ساختار Curl_resolv_async *async);
تهی Curl_async_thrdd_destroy(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(ساختار Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms);
تهی Curl_async_thrdd_multi_destroy(ساختار Curl_multi *multi, bool join);
تهی Curl_async_thrdd_multi_process(ساختار Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_set_props(ساختار Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms);

#endif /* USE_RESOLV_THREADED */

#ifndef CURL_DISABLE_DOH
ساختار doh_probes;
#endif

/*
 * Curl_async_await()
 *
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 *
 * On return 'dns' is assigned the resolved dns (CURLE_OK or NULL otherwise.
 *
 * Returns CURLE_COULDNT_RESOLVE_HOST if the host was not resolved,
 * CURLE_OPERATION_TIMEDOUT if a time-out occurred, or other errors.
 */
CURLcode Curl_async_await(ساختار Curl_easy *data, uint32_t resolv_id,
                          ساختار Curl_dns_entry **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_result(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async,
                                ساختار Curl_dns_entry **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(ساختار Curl_easy *data,
                            ساختار Curl_resolv_async *async,
                            ساختار easy_pollset *ps);

#وگرنه /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#define Curl_async_global_init()        CURLE_OK
#define Curl_async_global_cleanup()     Curl_nop_stmt
#define Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#define Curl_async_take_result(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#define Curl_async_pollset(x, y, z)     CURLE_OK
#define Curl_async_knows_https(x, y)    TRUE
#endif /* !CURLRES_ASYNCH */

#اگر defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#define USE_CURL_ASYNC
#endif

#ifdef USE_CURL_ASYNC

ساختار Curl_resolv_async {
  ساختار Curl_resolv_async *next;
  ساختار Curl_peer *peer;
  ساختار Curl_addrinfo *ai_A;
  ساختار Curl_addrinfo *ai_AAAA;
#ifdef USE_HTTPSRR
  ساختار Curl_https_rrinfo *httpsrr;
#endif
#ifdef USE_RESOLV_ARES
  ساختار async_ares_ctx ares;
#وگرنهاگر defined(USE_RESOLV_THREADED)
  ساختار async_thrdd_ctx thrdd;
#endif
#ifndef CURL_DISABLE_DOH
  ساختار doh_probes *doh; /* DoH specific data for this request */
#endif
  ساختار curltime start;
  timediff_t interval_end;
  timediff_t timeout_ms;
  CURLcode result;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  BIT(is_ipaddr);
  BIT(is_ipv4addr);
  BIT(for_proxy);
  BIT(done);
  BIT(shutdown);
  BIT(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(ساختار Curl_easy *data,
                                  ساختار Curl_resolv_async *async);

/* Shut down the given async resolve. */
تهی Curl_async_shutdown(ساختار Curl_easy *data,
                         ساختار Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
تهی Curl_async_destroy(ساختار Curl_easy *data,
                        ساختار Curl_resolv_async *async);

CURLcode Curl_async_failed(ساختار Curl_easy *data,
                           ساختار Curl_resolv_async *async,
                           ثابت نویسه *detail);

#وگرنه /* !USE_CURL_ASYNC */
#define Curl_async_shutdown(x, y) Curl_nop_stmt
#endif /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#endif /* HEADER_CURL_ASYN_H */
