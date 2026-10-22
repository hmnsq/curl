#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ASYN_H
#నిర్వచించు శీర్షిక_CURL_ASYN_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_HTTPSRR) && defined(USE_ARES)
#అన్తర్భావ "vdns/httpsrr.h"
#అయితే_ముగింపు

నిర్మాణం Curl_easy;
నిర్మాణం Curl_dns_నమోదు;
నిర్మాణం Curl_resolv_async;
నిర్మాణం Curl_multi;
నిర్మాణం easy_pollset;

#నిర్వచితమైతే CURLRES_ASYNCH

#అన్తర్భావ "curl_addrinfo.h"

నిర్మాణం hostent;
నిర్మాణం connectdata;
నిర్మాణం easy_pollset;

#ఒకవేళ defined(USE_RESOLV_ARES) && defined(USE_RESOLV_THREADED)
#దోషం cannot have both USE_RESOLV_ARES and USE_RESOLV_THREADED defined
#అయితే_ముగింపు

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
పూర్ణాంకం Curl_async_global_init(శూన్య);

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 */
శూన్య Curl_async_global_cleanup(శూన్య);

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
CURLcode Curl_async_getaddrinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async);

#నిర్వచితమైతే USE_HTTPSRR
bool Curl_async_knows_https(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_resolv_async *async);
#అయితే_ముగింపు /* USE_HTTPSRR */

#నిర్వచితమైతే USE_ARES
/* common functions for c-ares and threaded resolver with HTTPSRR */
#అన్తర్భావ <ares.h>

CURLcode Curl_ares_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                           ares_channel channel,
                           నిర్మాణం easy_pollset *ps);

timediff_t Curl_ares_timeout_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async,
                                ares_channel channel);

పూర్ణాంకం Curl_ares_perform(ares_channel channel, timediff_t timeout_ms);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_RESOLV_ARES
/* async resolving implementation using c-ares alone */
నిర్మాణం async_ares_ctx {
  ares_channel channel;
  పూర్ణాంకం ares_స్థితి;               /* ARES_SUCCESS, ARES_ENOTFOUND, etc. */
  CURLcode ఫలితం;               /* CURLE_OK or error handling response */
  బిట్టు(transient_err); /* an A/AAAA query failed without the resolver
                         answering that the name does not exist */
};

శూన్య Curl_async_ares_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async);
శూన్య Curl_async_ares_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_resolv_async *async);

#అయితే_ముగింపు /* USE_RESOLV_ARES */

#నిర్వచితమైతే USE_RESOLV_THREADED

నిర్మాణం async_thrdd_అంశం;

/* Context for threaded resolver */
నిర్మాణం async_thrdd_ctx {
  నిర్మాణం async_thrdd_అంశం *res_A; /* IPv4 final result */
  నిర్మాణం async_thrdd_అంశం *res_AAAA; /* IPv6 final result */
#ఒకవేళ defined(USE_HTTPSRR) && defined(USE_ARES)
  నిర్మాణం {
    ares_channel channel;
  } rr;
#అయితే_ముగింపు
  బిట్టు(processed_A);
  బిట్టు(processed_AAAA);
};

శూన్య Curl_async_thrdd_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_resolv_async *async);
శూన్య Curl_async_thrdd_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async);

CURLcode Curl_async_thrdd_multi_init(నిర్మాణం Curl_multi *multi,
                                     uint32_t min_దారాలు,
                                     uint32_t max_దారాలు,
                                     uint32_t idle_సమయం_ms);
శూన్య Curl_async_thrdd_multi_destroy(నిర్మాణం Curl_multi *multi, bool join);
శూన్య Curl_async_thrdd_multi_process(నిర్మాణం Curl_multi *multi);

CURLcode Curl_async_thrdd_multi_సమితి_props(నిర్మాణం Curl_multi *multi,
                                          uint32_t min_దారాలు,
                                          uint32_t max_దారాలు,
                                          uint32_t idle_సమయం_ms);

#అయితే_ముగింపు /* USE_RESOLV_THREADED */

#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
నిర్మాణం doh_probes;
#అయితే_ముగింపు

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
CURLcode Curl_async_await(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id,
                          నిర్మాణం Curl_dns_నమోదు **pdns);

/*
 * Take the result of an async resolve operation.
 * Returns CURLE_OK with `*pdns` != NULL, CURLE_AGAIN while still
 * ongoing or an error code for a failed resolve.
 */
CURLcode Curl_async_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async,
                                నిర్మాణం Curl_dns_నమోదు **pdns);

/* Curl_async_pollset()
 *
 * This function is called from the Curl_multi_pollset() function.  'sock' is a
 * pointer to an array to hold the file descriptors, with 'numsock' being the
 * size of that array (in number of entries). This function is supposed to
 * return bitmask indicating what file descriptors (referring to array indexes
 * in the 'sock' array) to wait for, read/write.
 */
CURLcode Curl_async_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_resolv_async *async,
                            నిర్మాణం easy_pollset *ps);

#లేకపోతే /* CURLRES_ASYNCH */

/* convert these functions if an asynch resolver is not used */
#నిర్వచించు Curl_async_global_init()        CURLE_OK
#నిర్వచించు Curl_async_global_cleanup()     Curl_nop_stmt
#నిర్వచించు Curl_async_await(a, b, c)       CURLE_COULDNT_RESOLVE_HOST
#నిర్వచించు Curl_async_take_ఫలితం(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#నిర్వచించు Curl_async_pollset(x, y, z)     CURLE_OK
#నిర్వచించు Curl_async_knows_https(x, y)    TRUE
#అయితే_ముగింపు /* !CURLRES_ASYNCH */

#ఒకవేళ defined(CURLRES_ASYNCH) || !defined(CURL_DISABLE_DOH)
#నిర్వచించు USE_CURL_ASYNC
#అయితే_ముగింపు

#నిర్వచితమైతే USE_CURL_ASYNC

నిర్మాణం Curl_resolv_async {
  నిర్మాణం Curl_resolv_async *తదుపరి;
  నిర్మాణం Curl_peer *peer;
  నిర్మాణం Curl_addrinfo *ai_A;
  నిర్మాణం Curl_addrinfo *ai_AAAA;
#నిర్వచితమైతే USE_HTTPSRR
  నిర్మాణం Curl_https_rrinfo *httpsrr;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_RESOLV_ARES
  నిర్మాణం async_ares_ctx ares;
#లేదావేళ defined(USE_RESOLV_THREADED)
  నిర్మాణం async_thrdd_ctx thrdd;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  నిర్మాణం doh_probes *doh; /* DoH specific data for this request */
#అయితే_ముగింపు
  నిర్మాణం curltime ప్రారంభం;
  timediff_t interval_చివర;
  timediff_t timeout_ms;
  CURLcode ఫలితం;
  uint32_t poll_interval;
  uint32_t id; /* unique id per easy handle of the resolve operation */
  uint8_t dns_queries; /* what queries are being performed */
  uint8_t dns_responses; /* what queries had responses so far. */
  uint8_t transport;
  uint8_t queries_ongoing;
  బిట్టు(is_ipaddr);
  బిట్టు(is_ipv4addr);
  బిట్టు(for_proxy);
  బిట్టు(పూర్తి);
  బిట్టు(shutdown);
  బిట్టు(negative_answer); /* resolver answered that the name does not
                           exist. Only such failures may be cached as
                           negative entries, not transient or local
                           resolver failures. */
};

timediff_t Curl_async_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_resolv_async *async);

/* Shut down the given async resolve. */
శూన్య Curl_async_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం Curl_resolv_async *async);

/* Frees the resources of the given async resolve and the struct itself. */
శూన్య Curl_async_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_resolv_async *async);

CURLcode Curl_async_failed(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_resolv_async *async,
                           స్థిరరాశి అక్షరం *detail);

#లేకపోతే /* !USE_CURL_ASYNC */
#నిర్వచించు Curl_async_shutdown(x, y) Curl_nop_stmt
#అయితే_ముగింపు /* USE_CURL_ASYNC */

/********** end of generic resolver interface functions *****************/
#అయితే_ముగింపు /* HEADER_CURL_ASYN_H */
