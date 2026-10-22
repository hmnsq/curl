#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ASYN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ASYN_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_HTTPSRR) && defined(USE_ARES)
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਜੇ_ਅੰਤ

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ;
ਬਣਤਰ Curl_resolv_async;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ easy_pollset;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_ASYNCH

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"

ਬਣਤਰ hostent;
ਬਣਤਰ connectdata;
ਬਣਤਰ easy_pollset;

#ਜੇ defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#ਗਲਤੀ cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#ਜੇ_ਅੰਤ

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
ਪੂਰਨ Curl_async_global_init(ਖਾਲੀ);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
ਖਾਲੀ Curl_async_global_cleanup(ਖਾਲੀ);

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
CURLcode Curl_async_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
bool Curl_async_knows_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_resolv_async *async);
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>

CURLcode Curl_ares_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ares_channel channel,
                           ਬਣਤਰ easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async,
                                ares_channel channel);

ਪੂਰਨ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
ਬਣਤਰ async_ares_ctx {
  ares_channel channel;
  ਪੂਰਨ ares_ਸਥਿਤੀ;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode ਨਤੀਜਾ;               /* CURLE_OK or error handling response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

ਖਾਲੀ Curl_async_ares_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async);
ਖਾਲੀ Curl_async_ares_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_resolv_async *async);

#ਜੇ_ਅੰਤ /* USE_RESOLV_ARES */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED

ਬਣਤਰ async_thrdd_ਵਸਤੂ;

/* Context for threaded resolver */
ਬਣਤਰ async_thrdd_ctx {
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *res_A; /* IPv4 final result */
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *res_AAAA; /* IPv6 final result */
#ਜੇ defined(USE_HTTPSRR) && defined(USE_ARES)
  ਬਣਤਰ {
    ares_channel channel;
  } rr;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(processed_A);
  ਦੋਆਧਾਰੀ_ਅੰਕ(processed_AAAA);
};

ਖਾਲੀ Curl_async_thrdd_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_resolv_async *async);
ਖਾਲੀ Curl_async_thrdd_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(ਬਣਤਰ Curl_multi *multi,
                                     uint32_t min_ਧਾਗੇ,
                                     uint32_t max_ਧਾਗੇ,
                                     uint32_t idle_ਸਮਾਂ_ms);
ਖਾਲੀ Curl_async_thrdd_multi_destroy(ਬਣਤਰ Curl_multi *multi, bool join);
ਖਾਲੀ Curl_async_thrdd_multi_process(ਬਣਤਰ Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_ਸੈੱਟ_props(ਬਣਤਰ Curl_multi *multi,
                                          uint32_t min_ਧਾਗੇ,
                                          uint32_t max_ਧਾਗੇ,
                                          uint32_t idle_ਸਮਾਂ_ms);

#ਜੇ_ਅੰਤ /* USE_RESOLV_THREADED */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
ਬਣਤਰ doh_probes;
#ਜੇ_ਅੰਤ

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
CURLcode Curl_async_await(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id,
                          ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async,
                                ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_resolv_async *async,
                            ਬਣਤਰ easy_pollset *ps);

#ਨਹੀਂਤਾਂ /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_global_init()        CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_global_cleanup()     Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_take_ਨਤੀਜਾ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_pollset(x, y, z)     CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_knows_https(x, y)    TRUE
#ਜੇ_ਅੰਤ /* !CURLRES_ASYNCH */

#ਜੇ defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_CURL_ASYNC
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC

ਬਣਤਰ Curl_resolv_async {
  ਬਣਤਰ Curl_resolv_async *ਅਗਲਾ;
  ਬਣਤਰ Curl_peer *peer;
  ਬਣਤਰ Curl_addrinfo *ai_A;
  ਬਣਤਰ Curl_addrinfo *ai_AAAA;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਬਣਤਰ Curl_https_rrinfo *httpsrr;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
  ਬਣਤਰ async_ares_ctx ares;
#ਜਾਂਜੇ defined(USE_RESOLV_THREADED)
  ਬਣਤਰ async_thrdd_ctx thrdd;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਬਣਤਰ doh_probes *doh; /* DoH specific data for this request */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ curltime ਸ਼ੁਰੂ;
  timediff_t interval_ਸਿਰਾ;
  timediff_t timeout_ms;
  CURLcode ਨਤੀਜਾ;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  ਦੋਆਧਾਰੀ_ਅੰਕ(is_ipaddr);
  ਦੋਆਧਾਰੀ_ਅੰਕ(is_ipv4addr);
  ਦੋਆਧਾਰੀ_ਅੰਕ(for_proxy);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਾ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown);
  ਦੋਆਧਾਰੀ_ਅੰਕ(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_resolv_async *async);

/* Shut down the given async resolve. */
ਖਾਲੀ Curl_async_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
ਖਾਲੀ Curl_async_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_resolv_async *async);

CURLcode Curl_async_failed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_resolv_async *async,
                           ਅਚਲ ਅੱਖਰ *detail);

#ਨਹੀਂਤਾਂ /* !USE_CURL_ASYNC */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_async_shutdown(x, y) Curl_nop_stmt
#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#ਜੇ_ਅੰਤ /* HEADER_CURL_ASYN_H */
