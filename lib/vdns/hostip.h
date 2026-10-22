#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HOSTIP_H
#నిర్వచించు శీర్షిక_CURL_HOSTIP_H
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

#అన్తర్భావ "hash.h"
#అన్తర్భావ "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#నిర్వచించు CURL_HOSTENT_కొలత 9000

#నిర్వచించు CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

నిర్మాణం addrinfo;
నిర్మాణం hostent;
నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం easy_pollset;
నిర్మాణం Curl_https_rrinfo;
నిర్మాణం Curl_multi;
నిర్మాణం Curl_dns_నమోదు;
నిర్మాణం Curl_peer;

/* DNS query types */
#నిర్వచించు CURL_DNSQ_A           (1U << 0)
#నిర్వచించు CURL_DNSQ_AAAA        (1U << 1)
#నిర్వచించు CURL_DNSQ_HTTPS       (1U << 2)

#నిర్వచించు CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#నిర్వచించు CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_resolv_query_str(uint8_t dns_queries);
#అయితే_ముగింపు

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(నిర్మాణం Curl_easy *దత్తాంశం, uint8_t ip_సంస్కరణ);

గణన alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(స్థిరరాశి అక్షరం *hostname);

#నిర్వచితమైతే USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(నిర్మాణం Curl_multi *multi);
#లేకపోతే
#నిర్వచించు Curl_probeipv6(x) CURLE_OK
#అయితే_ముగింపు

/* IPv4 thread-safe resolve function used for synch and asynch builds */
నిర్మాణం Curl_addrinfo *Curl_ipv4_resolve_r(స్థిరరాశి అక్షరం *hostname, uint16_t పోర్టు);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
శూన్య Curl_printable_చిరునామా(స్థిరరాశి నిర్మాణం Curl_addrinfo *ai,
                            అక్షరం *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(నిర్మాణం Curl_easy *దత్తాంశం,
                     నిర్మాణం Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     నిర్మాణం Curl_dns_నమోదు **pdns);

CURLcode Curl_resolv_blocking(నిర్మాణం Curl_easy *దత్తాంశం,
                              uint8_t dns_queries,
                              స్థిరరాశి అక్షరం *hostname,
                              uint16_t పోర్టు,
                              uint8_t transport,
                              నిర్మాణం Curl_dns_నమోదు **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    శూన్య *resolver);

#నిర్వచితమైతే USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
నిర్మాణం Curl_resolv_async *Curl_async_get(నిర్మాణం Curl_easy *దత్తాంశం,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
శూన్య Curl_resolv_shutdown_all(నిర్మాణం Curl_easy *దత్తాంశం);

/* Destroy all resolve resources of the given easy handle. */
శూన్య Curl_resolv_destroy_all(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_resolv_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id,
                                 నిర్మాణం Curl_dns_నమోదు **pdns);

శూన్య Curl_resolv_destroy(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  uint32_t resolv_id,
                                  స్థిరరాశి నిర్మాణం curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(నిర్మాణం Curl_easy *దత్తాంశం,
                             uint32_t resolv_id, uint8_t dns_queries);

స్థిరరాశి నిర్మాణం Curl_addrinfo *Curl_resolv_get_ai(నిర్మాణం Curl_easy *దత్తాంశం,
                                               uint32_t resolv_id,
                                               పూర్ణాంకం ai_family,
                                               గుర్తులేని పూర్ణాంకం సూచీ);
#నిర్వచితమైతే USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           నిర్మాణం Curl_dns_నమోదు **pdns);


స్థిరరాశి నిర్మాణం Curl_https_rrinfo *Curl_resolv_get_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id);
#అయితే_ముగింపు /* USE_HTTPSRR */

#లేకపోతే /* !USE_CURL_ASYNC */
#నిర్వచించు Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#నిర్వచించు Curl_resolv_destroy_all(x)       Curl_nop_stmt
#నిర్వచించు Curl_resolv_take_ఫలితం(x, y, z) CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#నిర్వచించు Curl_resolv_has_answers(x, y, z) TRUE
#నిర్వచించు Curl_resolv_get_ai(x, y, z, a)   ఖాళీ
#నిర్వచించు Curl_resolv_get_https(x, y)      ఖాళీ
#నిర్వచించు Curl_resolv_knows_https(x, y)    TRUE
#నిర్వచించు Curl_resolv_pollset(x, y)        CURLE_OK
#నిర్వచించు Curl_resolv_destroy(x, y)        Curl_nop_stmt
#అయితే_ముగింపు /* USE_CURL_ASYNC */

#నిర్వచితమైతే CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
నిర్మాణం Curl_addrinfo *Curl_sync_getaddrinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                            uint8_t dns_queries,
                                            స్థిరరాశి అక్షరం *hostname,
                                            uint16_t పోర్టు,
                                            uint8_t transport);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_HOSTIP_H */
