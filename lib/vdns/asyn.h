#اگر_تعریف_نشده سرآیند_CURL_ASYN_H
#تعریف_کن سرآیند_CURL_ASYN_H
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
#پایان_اگر

بنية Curl_easy;
بنية Curl_dns_مدخل;
بنية Curl_resolv_async;
بنية Curl_multi;
بنية easy_pollset;

#اگر_تعریف_شده CURLRES_ASYNCH

#ضمن "curl_addrinfo.h"

بنية hostent;
بنية connectdata;
بنية easy_pollset;

#إذا defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#خطا cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#پایان_اگر

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
CURLcode Curl_async_getaddrinfo(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async);

#اگر_تعریف_شده USE_HTTPSRR
bool Curl_async_knows_https(بنية Curl_easy *داده,
                            بنية Curl_resolv_async *async);
#پایان_اگر /* USE_HTTPSRR */

#اگر_تعریف_شده USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#ضمن <ares.h>

CURLcode Curl_ares_pollset(بنية Curl_easy *داده,
                           ares_channel channel,
                           بنية easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async,
                                ares_channel channel);

صحيح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#پایان_اگر

#اگر_تعریف_شده USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
بنية async_ares_ctx {
  ares_channel channel;
  صحيح ares_وضعیت;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode نتیجه;               /* CURLE_OK or error handling response */
  بیت(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

فراغ Curl_async_ares_shutdown(بنية Curl_easy *داده,
                              بنية Curl_resolv_async *async);
فراغ Curl_async_ares_destroy(بنية Curl_easy *داده,
                             بنية Curl_resolv_async *async);

#پایان_اگر /* USE_RESOLV_ARES */

#اگر_تعریف_شده USE_RESOLV_THREADED

بنية async_thrdd_قلم;

/* Context for threaded resolver */
بنية async_thrdd_ctx {
  بنية async_thrdd_قلم *res_A; /* IPv4 final result */
  بنية async_thrdd_قلم *res_AAAA; /* IPv6 final result */
#إذا defined(USE_HTTPSRR) && defined(USE_ARES)
  بنية {
    ares_channel channel;
  } rr;
#پایان_اگر
  بیت(processed_A);
  بیت(processed_AAAA);
};

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *داده,
                               بنية Curl_resolv_async *async);
فراغ Curl_async_thrdd_destroy(بنية Curl_easy *داده,
                              بنية Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_نخها,
                                     uint32_t max_نخها,
                                     uint32_t idle_زمان_ms);
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join);
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_مجموعه_props(بنية Curl_multi *multi,
                                          uint32_t min_نخها,
                                          uint32_t max_نخها,
                                          uint32_t idle_زمان_ms);

#پایان_اگر /* USE_RESOLV_THREADED */

#اگر_تعریف_نشده CURL_DISABLE_DOH
بنية doh_probes;
#پایان_اگر

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
CURLcode Curl_async_await(بنية Curl_easy *داده, uint32_t resolv_id,
                          بنية Curl_dns_مدخل **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_نتیجه(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_مدخل **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(بنية Curl_easy *داده,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps);

#وإلا /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#تعریف_کن Curl_async_global_init()        CURLE_OK
#تعریف_کن Curl_async_global_cleanup()     Curl_nop_stmt
#تعریف_کن Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#تعریف_کن Curl_async_take_نتیجه(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#تعریف_کن Curl_async_pollset(x, y, z)     CURLE_OK
#تعریف_کن Curl_async_knows_https(x, y)    TRUE
#پایان_اگر /* !CURLRES_ASYNCH */

#إذا defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#تعریف_کن USE_CURL_ASYNC
#پایان_اگر

#اگر_تعریف_شده USE_CURL_ASYNC

بنية Curl_resolv_async {
  بنية Curl_resolv_async *بعدی;
  بنية Curl_peer *peer;
  بنية Curl_addrinfo *ai_A;
  بنية Curl_addrinfo *ai_AAAA;
#اگر_تعریف_شده USE_HTTPSRR
  بنية Curl_https_rrinfo *httpsrr;
#پایان_اگر
#اگر_تعریف_شده USE_RESOLV_ARES
  بنية async_ares_ctx ares;
#وإلاإذا defined(USE_RESOLV_THREADED)
  بنية async_thrdd_ctx thrdd;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DOH
  بنية doh_probes *doh; /* DoH specific data for this request */
#پایان_اگر
  بنية curltime شروع;
  timediff_t interval_انتها;
  timediff_t timeout_ms;
  CURLcode نتیجه;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  بیت(is_ipaddr);
  بیت(is_ipv4addr);
  بیت(for_proxy);
  بیت(انجامشده);
  بیت(shutdown);
  بیت(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(بنية Curl_easy *داده,
                                  بنية Curl_resolv_async *async);

/* Shut down the given async resolve. */
فراغ Curl_async_shutdown(بنية Curl_easy *داده,
                         بنية Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
فراغ Curl_async_destroy(بنية Curl_easy *داده,
                        بنية Curl_resolv_async *async);

CURLcode Curl_async_failed(بنية Curl_easy *داده,
                           بنية Curl_resolv_async *async,
                           ثابت حرف *detail);

#وإلا /* !USE_CURL_ASYNC */
#تعریف_کن Curl_async_shutdown(x, y) Curl_nop_stmt
#پایان_اگر /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#پایان_اگر /* HEADER_CURL_ASYN_H */
