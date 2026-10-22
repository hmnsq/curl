#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ASYN_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ASYN_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_HTTPSRR) && defined(USE_ARES)
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ Curl_easy;
ರಚನೆ Curl_dns_ನಮೂದು;
ರಚನೆ Curl_resolv_async;
ರಚನೆ Curl_multi;
ರಚನೆ easy_pollset;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_ASYNCH

#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"

ರಚನೆ hostent;
ರಚನೆ connectdata;
ರಚನೆ easy_pollset;

#ಆದರೆ defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#ದೋಷ cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#ಆದರೆ_ಅಂತ್ಯ

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
ಪೂರ್ಣಾಂಕ Curl_async_global_init(ಶೂನ್ಯ);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
ಶೂನ್ಯ Curl_async_global_cleanup(ಶೂನ್ಯ);

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
CURLcode Curl_async_getaddrinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
bool Curl_async_knows_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_resolv_async *async);
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#ಅನ್ತರ್ಭಾವ <ares.h>

CURLcode Curl_ares_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ares_channel channel,
                           ರಚನೆ easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async,
                                ares_channel channel);

ಪೂರ್ಣಾಂಕ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
ರಚನೆ async_ares_ctx {
  ares_channel channel;
  ಪೂರ್ಣಾಂಕ ares_ಸ್ಥಿತಿ;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode ಫಲಿತಾಂಶ;               /* CURLE_OK or error handling response */
  ಬಿಟ್(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

ಶೂನ್ಯ Curl_async_ares_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async);
ಶೂನ್ಯ Curl_async_ares_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_resolv_async *async);

#ಆದರೆ_ಅಂತ್ಯ /* USE_RESOLV_ARES */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED

ರಚನೆ async_thrdd_ಅಂಶ;

/* Context for threaded resolver */
ರಚನೆ async_thrdd_ctx {
  ರಚನೆ async_thrdd_ಅಂಶ *res_A; /* IPv4 final result */
  ರಚನೆ async_thrdd_ಅಂಶ *res_AAAA; /* IPv6 final result */
#ಆದರೆ defined(USE_HTTPSRR) && defined(USE_ARES)
  ರಚನೆ {
    ares_channel channel;
  } rr;
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(processed_A);
  ಬಿಟ್(processed_AAAA);
};

ಶೂನ್ಯ Curl_async_thrdd_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_resolv_async *async);
ಶೂನ್ಯ Curl_async_thrdd_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(ರಚನೆ Curl_multi *multi,
                                     uint32_t min_ಎಳೆಗಳು,
                                     uint32_t max_ಎಳೆಗಳು,
                                     uint32_t idle_ಸಮಯ_ms);
ಶೂನ್ಯ Curl_async_thrdd_multi_destroy(ರಚನೆ Curl_multi *multi, bool join);
ಶೂನ್ಯ Curl_async_thrdd_multi_process(ರಚನೆ Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_ಗಣ_props(ರಚನೆ Curl_multi *multi,
                                          uint32_t min_ಎಳೆಗಳು,
                                          uint32_t max_ಎಳೆಗಳು,
                                          uint32_t idle_ಸಮಯ_ms);

#ಆದರೆ_ಅಂತ್ಯ /* USE_RESOLV_THREADED */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
ರಚನೆ doh_probes;
#ಆದರೆ_ಅಂತ್ಯ

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
CURLcode Curl_async_await(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id,
                          ರಚನೆ Curl_dns_ನಮೂದು **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async,
                                ರಚನೆ Curl_dns_ನಮೂದು **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_resolv_async *async,
                            ರಚನೆ easy_pollset *ps);

#ಇಲ್ಲದಿದ್ದರೆ /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_global_init()        CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_global_cleanup()     Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_take_ಫಲಿತಾಂಶ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_pollset(x, y, z)     CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_knows_https(x, y)    TRUE
#ಆದರೆ_ಅಂತ್ಯ /* !CURLRES_ASYNCH */

#ಆದರೆ defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_CURL_ASYNC
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC

ರಚನೆ Curl_resolv_async {
  ರಚನೆ Curl_resolv_async *ಮುಂದಿನ;
  ರಚನೆ Curl_peer *peer;
  ರಚನೆ Curl_addrinfo *ai_A;
  ರಚನೆ Curl_addrinfo *ai_AAAA;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ರಚನೆ Curl_https_rrinfo *httpsrr;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
  ರಚನೆ async_ares_ctx ares;
#ಅಥವಾಆದರೆ defined(USE_RESOLV_THREADED)
  ರಚನೆ async_thrdd_ctx thrdd;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ರಚನೆ doh_probes *doh; /* DoH specific data for this request */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ curltime ಆರಂಭ;
  timediff_t interval_ಕೊನೆ;
  timediff_t timeout_ms;
  CURLcode ಫಲಿತಾಂಶ;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  ಬಿಟ್(is_ipaddr);
  ಬಿಟ್(is_ipv4addr);
  ಬಿಟ್(for_proxy);
  ಬಿಟ್(ಮುಗಿದ);
  ಬಿಟ್(shutdown);
  ಬಿಟ್(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_resolv_async *async);

/* Shut down the given async resolve. */
ಶೂನ್ಯ Curl_async_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
ಶೂನ್ಯ Curl_async_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_resolv_async *async);

CURLcode Curl_async_failed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_resolv_async *async,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *detail);

#ಇಲ್ಲದಿದ್ದರೆ /* !USE_CURL_ASYNC */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_async_shutdown(x, y) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_ASYN_H */
