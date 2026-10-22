#अगर_अघोषित HEADER_CURL_HOSTIP_H
#घोषणा HEADER_CURL_HOSTIP_H
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

#समावेश "hash.h"
#समावेश "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#घोषणा CURL_HOSTENT_आकार 9000

#घोषणा CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

काष्ठा addrinfo;
काष्ठा hostent;
काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा easy_pollset;
काष्ठा Curl_https_rrinfo;
काष्ठा Curl_multi;
काष्ठा Curl_dns_entry;
काष्ठा Curl_peer;

/* DNS query types */
#घोषणा CURL_DNSQ_A           (1U << 0)
#घोषणा CURL_DNSQ_AAAA        (1U << 1)
#घोषणा CURL_DNSQ_HTTPS       (1U << 2)

#घोषणा CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#घोषणा CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_resolv_query_str(uint8_t dns_queries);
#पूर्ण_अगर

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(काष्ठा Curl_easy *तथ्य, uint8_t ip_version);

क्रमागत alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(स्थिर अक्षर *hostname);

#अगर_घोषित USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(काष्ठा Curl_multi *multi);
#अन्यथा
#घोषणा Curl_probeipv6(x) CURLE_OK
#पूर्ण_अगर

/* IPv4 thread-safe resolve function used for synch and asynch builds */
काष्ठा Curl_addrinfo *Curl_ipv4_resolve_r(स्थिर अक्षर *hostname, uint16_t port);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
व्योम Curl_printable_address(स्थिर काष्ठा Curl_addrinfo *ai,
                            अक्षर *buf, माप_प्रकार bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(काष्ठा Curl_easy *तथ्य,
                     काष्ठा Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     काष्ठा Curl_dns_entry **pdns);

CURLcode Curl_resolv_blocking(काष्ठा Curl_easy *तथ्य,
                              uint8_t dns_queries,
                              स्थिर अक्षर *hostname,
                              uint16_t port,
                              uint8_t transport,
                              काष्ठा Curl_dns_entry **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_start(काष्ठा Curl_easy *तथ्य,
                                    व्योम *resolver);

#अगर_घोषित USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
काष्ठा Curl_resolv_async *Curl_async_get(काष्ठा Curl_easy *तथ्य,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
व्योम Curl_resolv_shutdown_all(काष्ठा Curl_easy *तथ्य);

/* Destroy all resolve resources of the given easy handle. */
व्योम Curl_resolv_destroy_all(काष्ठा Curl_easy *तथ्य);

CURLcode Curl_resolv_take_परिणाम(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id,
                                 काष्ठा Curl_dns_entry **pdns);

व्योम Curl_resolv_destroy(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(काष्ठा Curl_easy *तथ्य,
                                  uint32_t resolv_id,
                                  स्थिर काष्ठा curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(काष्ठा Curl_easy *तथ्य,
                             uint32_t resolv_id, uint8_t dns_queries);

स्थिर काष्ठा Curl_addrinfo *Curl_resolv_get_ai(काष्ठा Curl_easy *तथ्य,
                                               uint32_t resolv_id,
                                               पूर्णांक ai_family,
                                               अचिन्हित पूर्णांक सूचकांक);
#अगर_घोषित USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           काष्ठा Curl_dns_entry **pdns);


स्थिर काष्ठा Curl_https_rrinfo *Curl_resolv_get_https(काष्ठा Curl_easy *तथ्य,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id);
#पूर्ण_अगर /* USE_HTTPSRR */

#अन्यथा /* !USE_CURL_ASYNC */
#घोषणा Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#घोषणा Curl_resolv_destroy_all(x)       Curl_nop_stmt
#घोषणा Curl_resolv_take_परिणाम(x, y, z) CURLE_NOT_BUILT_IN
#घोषणा Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#घोषणा Curl_resolv_has_answers(x, y, z) TRUE
#घोषणा Curl_resolv_get_ai(x, y, z, a)   शून्य
#घोषणा Curl_resolv_get_https(x, y)      शून्य
#घोषणा Curl_resolv_knows_https(x, y)    TRUE
#घोषणा Curl_resolv_pollset(x, y)        CURLE_OK
#घोषणा Curl_resolv_destroy(x, y)        Curl_nop_stmt
#पूर्ण_अगर /* USE_CURL_ASYNC */

#अगर_घोषित CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
काष्ठा Curl_addrinfo *Curl_sync_getaddrinfo(काष्ठा Curl_easy *तथ्य,
                                            uint8_t dns_queries,
                                            स्थिर अक्षर *hostname,
                                            uint16_t port,
                                            uint8_t transport);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_HOSTIP_H */
