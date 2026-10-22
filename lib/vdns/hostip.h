#यदि_अपरिभाषितम् शीर्षकम्_CURL_HOSTIP_H
#परिभाषय शीर्षकम्_CURL_HOSTIP_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "hash.h"
#अन्तर्भाव "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#परिभाषय CURL_HOSTENT_आकार 9000

#परिभाषय CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

संरचना addrinfo;
संरचना hostent;
संरचना Curl_easy;
संरचना connectdata;
संरचना easy_pollset;
संरचना Curl_https_rrinfo;
संरचना Curl_multi;
संरचना Curl_dns_प्रविष्टिः;
संरचना Curl_peer;

/* DNS query types */
#परिभाषय CURL_DNSQ_A           (1U << 0)
#परिभाषय CURL_DNSQ_AAAA        (1U << 1)
#परिभाषय CURL_DNSQ_HTTPS       (1U << 2)

#परिभाषय CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#परिभाषय CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_resolv_query_str(uint8_t dns_queries);
#यदि_अन्तः

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(संरचना Curl_easy *दत्त, uint8_t ip_संस्करणम्);

गणना alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(ध्रुव वर्ण *hostname);

#यदि_परिभाषितम् USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(संरचना Curl_multi *multi);
#अन्यथा
#परिभाषय Curl_probeipv6(x) CURLE_OK
#यदि_अन्तः

/* IPv4 thread-safe resolve function used for synch and asynch builds */
संरचना Curl_addrinfo *Curl_ipv4_resolve_r(ध्रुव वर्ण *hostname, uint16_t पत्तनम्);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
शून्य Curl_printable_स्थानाङ्कः(ध्रुव संरचना Curl_addrinfo *ai,
                            वर्ण *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(संरचना Curl_easy *दत्त,
                     संरचना Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     संरचना Curl_dns_प्रविष्टिः **pdns);

CURLcode Curl_resolv_blocking(संरचना Curl_easy *दत्त,
                              uint8_t dns_queries,
                              ध्रुव वर्ण *hostname,
                              uint16_t पत्तनम्,
                              uint8_t transport,
                              संरचना Curl_dns_प्रविष्टिः **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_start(संरचना Curl_easy *दत्त,
                                    शून्य *resolver);

#यदि_परिभाषितम् USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
संरचना Curl_resolv_async *Curl_async_get(संरचना Curl_easy *दत्त,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
शून्य Curl_resolv_shutdown_all(संरचना Curl_easy *दत्त);

/* Destroy all resolve resources of the given easy handle. */
शून्य Curl_resolv_destroy_all(संरचना Curl_easy *दत्त);

CURLcode Curl_resolv_take_परिणाम(संरचना Curl_easy *दत्त, uint32_t resolv_id,
                                 संरचना Curl_dns_प्रविष्टिः **pdns);

शून्य Curl_resolv_destroy(संरचना Curl_easy *दत्त, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(संरचना Curl_easy *दत्त,
                                  uint32_t resolv_id,
                                  ध्रुव संरचना curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(संरचना Curl_easy *दत्त,
                             uint32_t resolv_id, uint8_t dns_queries);

ध्रुव संरचना Curl_addrinfo *Curl_resolv_get_ai(संरचना Curl_easy *दत्त,
                                               uint32_t resolv_id,
                                               पूर्णाङ्क ai_family,
                                               अचिह्नित पूर्णाङ्क सूचकांक);
#यदि_परिभाषितम् USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(संरचना Curl_easy *दत्त,
                           संरचना Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           संरचना Curl_dns_प्रविष्टिः **pdns);


ध्रुव संरचना Curl_https_rrinfo *Curl_resolv_get_https(संरचना Curl_easy *दत्त,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(संरचना Curl_easy *दत्त, uint32_t resolv_id);
#यदि_अन्तः /* USE_HTTPSRR */

#अन्यथा /* !USE_CURL_ASYNC */
#परिभाषय Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#परिभाषय Curl_resolv_destroy_all(x)       Curl_nop_stmt
#परिभाषय Curl_resolv_take_परिणाम(x, y, z) CURLE_NOT_BUILT_अन्तर्
#परिभाषय Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#परिभाषय Curl_resolv_has_answers(x, y, z) TRUE
#परिभाषय Curl_resolv_get_ai(x, y, z, a)   रिक्त
#परिभाषय Curl_resolv_get_https(x, y)      रिक्त
#परिभाषय Curl_resolv_knows_https(x, y)    TRUE
#परिभाषय Curl_resolv_pollset(x, y)        CURLE_OK
#परिभाषय Curl_resolv_destroy(x, y)        Curl_nop_stmt
#यदि_अन्तः /* USE_CURL_ASYNC */

#यदि_परिभाषितम् CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
संरचना Curl_addrinfo *Curl_sync_getaddrinfo(संरचना Curl_easy *दत्त,
                                            uint8_t dns_queries,
                                            ध्रुव वर्ण *hostname,
                                            uint16_t पत्तनम्,
                                            uint8_t transport);
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_HOSTIP_H */
