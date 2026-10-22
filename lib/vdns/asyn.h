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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(USE_HTTPSRR) && defined(USE_ARES)
#அந்தர்பாவ "vdns/httpsrr.h"
#endif

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_dns_entry;
கட்டமைப்பு Curl_resolv_async;
கட்டமைப்பு Curl_multi;
கட்டமைப்பு easy_pollset;

#ifdef CURLRES_ASYNCH

#அந்தர்பாவ "curl_addrinfo.h"

கட்டமைப்பு hostent;
கட்டமைப்பு connectdata;
கட்டமைப்பு easy_pollset;

#எனில் defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
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
முழு Curl_async_global_init(வெறுமை);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
வெறுமை Curl_async_global_cleanup(வெறுமை);

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
CURLcode Curl_async_getaddrinfo(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async);

#ifdef USE_HTTPSRR
bool Curl_async_knows_https(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_resolv_async *async);
#endif /* USE_HTTPSRR */

#ifdef USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#அந்தர்பாவ <ares.h>

CURLcode Curl_ares_pollset(கட்டமைப்பு Curl_easy *data,
                           ares_channel channel,
                           கட்டமைப்பு easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async,
                                ares_channel channel);

முழு Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#endif

#ifdef USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
கட்டமைப்பு async_ares_ctx {
  ares_channel channel;
  முழு ares_status;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode result;               /* CURLE_OK or error handling response */
  BIT(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

வெறுமை Curl_async_ares_shutdown(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async);
வெறுமை Curl_async_ares_destroy(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_resolv_async *async);

#endif /* USE_RESOLV_ARES */

#ifdef USE_RESOLV_THREADED

கட்டமைப்பு async_thrdd_item;

/* Context for threaded resolver */
கட்டமைப்பு async_thrdd_ctx {
  கட்டமைப்பு async_thrdd_item *res_A; /* IPv4 final result */
  கட்டமைப்பு async_thrdd_item *res_AAAA; /* IPv6 final result */
#எனில் defined(USE_HTTPSRR) && defined(USE_ARES)
  கட்டமைப்பு {
    ares_channel channel;
  } rr;
#endif
  BIT(processed_A);
  BIT(processed_AAAA);
};

வெறுமை Curl_async_thrdd_shutdown(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_resolv_async *async);
வெறுமை Curl_async_thrdd_destroy(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(கட்டமைப்பு Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms);
வெறுமை Curl_async_thrdd_multi_destroy(கட்டமைப்பு Curl_multi *multi, bool join);
வெறுமை Curl_async_thrdd_multi_process(கட்டமைப்பு Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_set_props(கட்டமைப்பு Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms);

#endif /* USE_RESOLV_THREADED */

#ifndef CURL_DISABLE_DOH
கட்டமைப்பு doh_probes;
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
CURLcode Curl_async_await(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id,
                          கட்டமைப்பு Curl_dns_entry **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_result(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async,
                                கட்டமைப்பு Curl_dns_entry **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_resolv_async *async,
                            கட்டமைப்பு easy_pollset *ps);

#இல்லையெனில் /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#define Curl_async_global_init()        CURLE_OK
#define Curl_async_global_cleanup()     Curl_nop_stmt
#define Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#define Curl_async_take_result(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#define Curl_async_pollset(x, y, z)     CURLE_OK
#define Curl_async_knows_https(x, y)    TRUE
#endif /* !CURLRES_ASYNCH */

#எனில் defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#define USE_CURL_ASYNC
#endif

#ifdef USE_CURL_ASYNC

கட்டமைப்பு Curl_resolv_async {
  கட்டமைப்பு Curl_resolv_async *next;
  கட்டமைப்பு Curl_peer *peer;
  கட்டமைப்பு Curl_addrinfo *ai_A;
  கட்டமைப்பு Curl_addrinfo *ai_AAAA;
#ifdef USE_HTTPSRR
  கட்டமைப்பு Curl_https_rrinfo *httpsrr;
#endif
#ifdef USE_RESOLV_ARES
  கட்டமைப்பு async_ares_ctx ares;
#அல்லதெனில் defined(USE_RESOLV_THREADED)
  கட்டமைப்பு async_thrdd_ctx thrdd;
#endif
#ifndef CURL_DISABLE_DOH
  கட்டமைப்பு doh_probes *doh; /* DoH specific data for this request */
#endif
  கட்டமைப்பு curltime start;
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

timediff_t Curl_async_timeleft_ms(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_resolv_async *async);

/* Shut down the given async resolve. */
வெறுமை Curl_async_shutdown(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
வெறுமை Curl_async_destroy(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு Curl_resolv_async *async);

CURLcode Curl_async_failed(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_resolv_async *async,
                           மாறிலி எழுத்து *detail);

#இல்லையெனில் /* !USE_CURL_ASYNC */
#define Curl_async_shutdown(x, y) Curl_nop_stmt
#endif /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#endif /* HEADER_CURL_ASYN_H */
