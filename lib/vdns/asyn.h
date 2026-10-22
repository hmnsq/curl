#اگر_غیر_تعریف_شدہ سرخی_CURL_ASYN_H
#تعریف_کرو سرخی_CURL_ASYN_H
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
#اگر_ختم

ساخت Curl_easy;
ساخت Curl_dns_اندراج;
ساخت Curl_resolv_async;
ساخت Curl_multi;
ساخت easy_pollset;

#اگر_تعریف_شدہ CURLRES_ASYNCH

#شامل "curl_addrinfo.h"

ساخت hostent;
ساخت connectdata;
ساخت easy_pollset;

#اگر defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#غلطی cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#اگر_ختم

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
صحیح Curl_async_global_init(خالی);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
خالی Curl_async_global_cleanup(خالی);

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
CURLcode Curl_async_getaddrinfo(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async);

#اگر_تعریف_شدہ USE_HTTPSRR
bool Curl_async_knows_https(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_resolv_async *async);
#اگر_ختم /* USE_HTTPSRR */

#اگر_تعریف_شدہ USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#شامل <ares.h>

CURLcode Curl_ares_pollset(ساخت Curl_easy *ڈیٹا,
                           ares_channel channel,
                           ساخت easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async,
                                ares_channel channel);

صحیح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#اگر_ختم

#اگر_تعریف_شدہ USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
ساخت async_ares_ctx {
  ares_channel channel;
  صحیح ares_حالت;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode نتیجہ;               /* CURLE_OK or error handling response */
  ثنائی_ہندسہ(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

خالی Curl_async_ares_shutdown(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async);
خالی Curl_async_ares_destroy(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_resolv_async *async);

#اگر_ختم /* USE_RESOLV_ARES */

#اگر_تعریف_شدہ USE_RESOLV_THREADED

ساخت async_thrdd_شے;

/* Context for threaded resolver */
ساخت async_thrdd_ctx {
  ساخت async_thrdd_شے *res_A; /* IPv4 final result */
  ساخت async_thrdd_شے *res_AAAA; /* IPv6 final result */
#اگر defined(USE_HTTPSRR) && defined(USE_ARES)
  ساخت {
    ares_channel channel;
  } rr;
#اگر_ختم
  ثنائی_ہندسہ(processed_A);
  ثنائی_ہندسہ(processed_AAAA);
};

خالی Curl_async_thrdd_shutdown(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_resolv_async *async);
خالی Curl_async_thrdd_destroy(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(ساخت Curl_multi *multi,
                                     uint32_t min_دھاگے,
                                     uint32_t max_دھاگے,
                                     uint32_t idle_وقت_ms);
خالی Curl_async_thrdd_multi_destroy(ساخت Curl_multi *multi, bool join);
خالی Curl_async_thrdd_multi_process(ساخت Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_سیٹ_props(ساخت Curl_multi *multi,
                                          uint32_t min_دھاگے,
                                          uint32_t max_دھاگے,
                                          uint32_t idle_وقت_ms);

#اگر_ختم /* USE_RESOLV_THREADED */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
ساخت doh_probes;
#اگر_ختم

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
CURLcode Curl_async_await(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id,
                          ساخت Curl_dns_اندراج **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async,
                                ساخت Curl_dns_اندراج **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_resolv_async *async,
                            ساخت easy_pollset *ps);

#ورنہ /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#تعریف_کرو Curl_async_global_init()        CURLE_OK
#تعریف_کرو Curl_async_global_cleanup()     Curl_nop_stmt
#تعریف_کرو Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#تعریف_کرو Curl_async_take_نتیجہ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#تعریف_کرو Curl_async_pollset(x, y, z)     CURLE_OK
#تعریف_کرو Curl_async_knows_https(x, y)    TRUE
#اگر_ختم /* !CURLRES_ASYNCH */

#اگر defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#تعریف_کرو USE_CURL_ASYNC
#اگر_ختم

#اگر_تعریف_شدہ USE_CURL_ASYNC

ساخت Curl_resolv_async {
  ساخت Curl_resolv_async *اگلا;
  ساخت Curl_peer *peer;
  ساخت Curl_addrinfo *ai_A;
  ساخت Curl_addrinfo *ai_AAAA;
#اگر_تعریف_شدہ USE_HTTPSRR
  ساخت Curl_https_rrinfo *httpsrr;
#اگر_ختم
#اگر_تعریف_شدہ USE_RESOLV_ARES
  ساخت async_ares_ctx ares;
#وگرنہ_اگر defined(USE_RESOLV_THREADED)
  ساخت async_thrdd_ctx thrdd;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  ساخت doh_probes *doh; /* DoH specific data for this request */
#اگر_ختم
  ساخت curltime آغاز;
  timediff_t interval_انتہا;
  timediff_t timeout_ms;
  CURLcode نتیجہ;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  ثنائی_ہندسہ(is_ipaddr);
  ثنائی_ہندسہ(is_ipv4addr);
  ثنائی_ہندسہ(for_proxy);
  ثنائی_ہندسہ(مکمل);
  ثنائی_ہندسہ(shutdown);
  ثنائی_ہندسہ(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_resolv_async *async);

/* Shut down the given async resolve. */
خالی Curl_async_shutdown(ساخت Curl_easy *ڈیٹا,
                         ساخت Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
خالی Curl_async_destroy(ساخت Curl_easy *ڈیٹا,
                        ساخت Curl_resolv_async *async);

CURLcode Curl_async_failed(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_resolv_async *async,
                           ثابت حرف *detail);

#ورنہ /* !USE_CURL_ASYNC */
#تعریف_کرو Curl_async_shutdown(x, y) Curl_nop_stmt
#اگر_ختم /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#اگر_ختم /* HEADER_CURL_ASYN_H */
