#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_ASYN_H
#وصف_ڪريو سرو_CURL_ASYN_H
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

#إذا defined(USE_HTTPSRR) && defined(USE_ARES)
#ضمن "vdns/httpsrr.h"
#جيڪڏهن_پورو

بنية Curl_easy;
بنية Curl_dns_داخلا;
بنية Curl_resolv_async;
بنية Curl_multi;
بنية easy_pollset;

#جيڪڏهن_وصف_ٿيل CURLRES_ASYNCH

#ضمن "curl_addrinfo.h"

بنية hostent;
بنية connectdata;
بنية easy_pollset;

#إذا defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#غلطي cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#جيڪڏهن_پورو

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
صحيح Curl_async_global_init(فراغ);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
فراغ Curl_async_global_cleanup(فراغ);

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
CURLcode Curl_async_getaddrinfo(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async);

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
bool Curl_async_knows_https(بنية Curl_easy *معلومات,
                            بنية Curl_resolv_async *async);
#جيڪڏهن_پورو /* USE_HTTPSRR */

#جيڪڏهن_وصف_ٿيل USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#ضمن <ares.h>

CURLcode Curl_ares_pollset(بنية Curl_easy *معلومات,
                           ares_channel channel,
                           بنية easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                ares_channel channel);

صحيح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
بنية async_ares_ctx {
  ares_channel channel;
  صحيح ares_حالت;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode نتيجو;               /* CURLE_OK or error handling response */
  ٻٽو_انگ(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

فراغ Curl_async_ares_shutdown(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async);
فراغ Curl_async_ares_destroy(بنية Curl_easy *معلومات,
                             بنية Curl_resolv_async *async);

#جيڪڏهن_پورو /* USE_RESOLV_ARES */

#جيڪڏهن_وصف_ٿيل USE_RESOLV_THREADED

بنية async_thrdd_شي;

/* Context for threaded resolver */
بنية async_thrdd_ctx {
  بنية async_thrdd_شي *res_A; /* IPv4 final result */
  بنية async_thrdd_شي *res_AAAA; /* IPv6 final result */
#إذا defined(USE_HTTPSRR) && defined(USE_ARES)
  بنية {
    ares_channel channel;
  } rr;
#جيڪڏهن_پورو
  ٻٽو_انگ(processed_A);
  ٻٽو_انگ(processed_AAAA);
};

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *معلومات,
                               بنية Curl_resolv_async *async);
فراغ Curl_async_thrdd_destroy(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_ڌاڳا,
                                     uint32_t max_ڌاڳا,
                                     uint32_t idle_وقت_ms);
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join);
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_سيٽ_props(بنية Curl_multi *multi,
                                          uint32_t min_ڌاڳا,
                                          uint32_t max_ڌاڳا,
                                          uint32_t idle_وقت_ms);

#جيڪڏهن_پورو /* USE_RESOLV_THREADED */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
بنية doh_probes;
#جيڪڏهن_پورو

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
CURLcode Curl_async_await(بنية Curl_easy *معلومات, uint32_t resolv_id,
                          بنية Curl_dns_داخلا **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_نتيجو(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_داخلا **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(بنية Curl_easy *معلومات,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps);

#وإلا /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#وصف_ڪريو Curl_async_global_init()        CURLE_OK
#وصف_ڪريو Curl_async_global_cleanup()     Curl_nop_stmt
#وصف_ڪريو Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#وصف_ڪريو Curl_async_take_نتيجو(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#وصف_ڪريو Curl_async_pollset(x, y, z)     CURLE_OK
#وصف_ڪريو Curl_async_knows_https(x, y)    TRUE
#جيڪڏهن_پورو /* !CURLRES_ASYNCH */

#إذا defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#وصف_ڪريو USE_CURL_ASYNC
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC

بنية Curl_resolv_async {
  بنية Curl_resolv_async *ايندڙ;
  بنية Curl_peer *peer;
  بنية Curl_addrinfo *ai_A;
  بنية Curl_addrinfo *ai_AAAA;
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  بنية Curl_https_rrinfo *httpsrr;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_RESOLV_ARES
  بنية async_ares_ctx ares;
#وإلاإذا defined(USE_RESOLV_THREADED)
  بنية async_thrdd_ctx thrdd;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  بنية doh_probes *doh; /* DoH specific data for this request */
#جيڪڏهن_پورو
  بنية curltime شروعات;
  timediff_t interval_پڇاڙي;
  timediff_t timeout_ms;
  CURLcode نتيجو;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  ٻٽو_انگ(is_ipaddr);
  ٻٽو_انگ(is_ipv4addr);
  ٻٽو_انگ(for_proxy);
  ٻٽو_انگ(پورو);
  ٻٽو_انگ(shutdown);
  ٻٽو_انگ(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(بنية Curl_easy *معلومات,
                                  بنية Curl_resolv_async *async);

/* Shut down the given async resolve. */
فراغ Curl_async_shutdown(بنية Curl_easy *معلومات,
                         بنية Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
فراغ Curl_async_destroy(بنية Curl_easy *معلومات,
                        بنية Curl_resolv_async *async);

CURLcode Curl_async_failed(بنية Curl_easy *معلومات,
                           بنية Curl_resolv_async *async,
                           ثابت حرف *detail);

#وإلا /* !USE_CURL_ASYNC */
#وصف_ڪريو Curl_async_shutdown(x, y) Curl_nop_stmt
#جيڪڏهن_پورو /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#جيڪڏهن_پورو /* HEADER_CURL_ASYN_H */
