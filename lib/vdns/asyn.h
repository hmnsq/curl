#अगर_अघोषित HEADER_CURL_ASYN_H
#घोषणा HEADER_CURL_ASYN_H
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
#समावेश "curl_setup.h"

#अगर defined(USE_HTTPSRR) && defined(USE_ARES)
#समावेश "vdns/httpsrr.h"
#पूर्ण_अगर

काष्ठा Curl_easy;
काष्ठा Curl_dns_entry;
काष्ठा Curl_resolv_async;
काष्ठा Curl_multi;
काष्ठा easy_pollset;

#अगर_घोषित CURLRES_ASYNCH

#समावेश "curl_addrinfo.h"

काष्ठा hostent;
काष्ठा connectdata;
काष्ठा easy_pollset;

#अगर defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#त्रुटि cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#पूर्ण_अगर

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
पूर्णांक Curl_async_global_init(व्योम);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
व्योम Curl_async_global_cleanup(व्योम);

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
CURLcode Curl_async_getaddrinfo(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async);

#अगर_घोषित USE_HTTPSRR
bool Curl_async_knows_https(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_resolv_async *async);
#पूर्ण_अगर /* USE_HTTPSRR */

#अगर_घोषित USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#समावेश <ares.h>

CURLcode Curl_ares_pollset(काष्ठा Curl_easy *तथ्य,
                           ares_channel channel,
                           काष्ठा easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async,
                                ares_channel channel);

पूर्णांक Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#पूर्ण_अगर

#अगर_घोषित USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
काष्ठा async_ares_ctx {
  ares_channel channel;
  पूर्णांक ares_status;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode परिणाम;               /* CURLE_OK or error handling response */
  BIT(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

व्योम Curl_async_ares_shutdown(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async);
व्योम Curl_async_ares_destroy(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_resolv_async *async);

#पूर्ण_अगर /* USE_RESOLV_ARES */

#अगर_घोषित USE_RESOLV_THREADED

काष्ठा async_thrdd_item;

/* Context for threaded resolver */
काष्ठा async_thrdd_ctx {
  काष्ठा async_thrdd_item *res_A; /* IPv4 final result */
  काष्ठा async_thrdd_item *res_AAAA; /* IPv6 final result */
#अगर defined(USE_HTTPSRR) && defined(USE_ARES)
  काष्ठा {
    ares_channel channel;
  } rr;
#पूर्ण_अगर
  BIT(processed_A);
  BIT(processed_AAAA);
};

व्योम Curl_async_thrdd_shutdown(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_resolv_async *async);
व्योम Curl_async_thrdd_destroy(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(काष्ठा Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms);
व्योम Curl_async_thrdd_multi_destroy(काष्ठा Curl_multi *multi, bool join);
व्योम Curl_async_thrdd_multi_process(काष्ठा Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_set_props(काष्ठा Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms);

#पूर्ण_अगर /* USE_RESOLV_THREADED */

#अगर_अघोषित CURL_DISABLE_DOH
काष्ठा doh_probes;
#पूर्ण_अगर

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
CURLcode Curl_async_await(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id,
                          काष्ठा Curl_dns_entry **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async,
                                काष्ठा Curl_dns_entry **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_resolv_async *async,
                            काष्ठा easy_pollset *ps);

#अन्यथा /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#घोषणा Curl_async_global_init()        CURLE_OK
#घोषणा Curl_async_global_cleanup()     Curl_nop_stmt
#घोषणा Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#घोषणा Curl_async_take_परिणाम(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#घोषणा Curl_async_pollset(x, y, z)     CURLE_OK
#घोषणा Curl_async_knows_https(x, y)    TRUE
#पूर्ण_अगर /* !CURLRES_ASYNCH */

#अगर defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#घोषणा USE_CURL_ASYNC
#पूर्ण_अगर

#अगर_घोषित USE_CURL_ASYNC

काष्ठा Curl_resolv_async {
  काष्ठा Curl_resolv_async *अग्र;
  काष्ठा Curl_peer *peer;
  काष्ठा Curl_addrinfo *ai_A;
  काष्ठा Curl_addrinfo *ai_AAAA;
#अगर_घोषित USE_HTTPSRR
  काष्ठा Curl_https_rrinfo *httpsrr;
#पूर्ण_अगर
#अगर_घोषित USE_RESOLV_ARES
  काष्ठा async_ares_ctx ares;
#या_अगर defined(USE_RESOLV_THREADED)
  काष्ठा async_thrdd_ctx thrdd;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DOH
  काष्ठा doh_probes *doh; /* DoH specific data for this request */
#पूर्ण_अगर
  काष्ठा curltime start;
  timediff_t interval_end;
  timediff_t timeout_ms;
  CURLcode परिणाम;
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

timediff_t Curl_async_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_resolv_async *async);

/* Shut down the given async resolve. */
व्योम Curl_async_shutdown(काष्ठा Curl_easy *तथ्य,
                         काष्ठा Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
व्योम Curl_async_destroy(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_resolv_async *async);

CURLcode Curl_async_failed(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_resolv_async *async,
                           स्थिर अक्षर *detail);

#अन्यथा /* !USE_CURL_ASYNC */
#घोषणा Curl_async_shutdown(x, y) Curl_nop_stmt
#पूर्ण_अगर /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#पूर्ण_अगर /* HEADER_CURL_ASYN_H */
