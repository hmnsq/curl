#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HOSTIP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HOSTIP_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HOSTENT_ਮਾਪ 9000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

ਬਣਤਰ addrinfo;
ਬਣਤਰ hostent;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ easy_pollset;
ਬਣਤਰ Curl_https_rrinfo;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ;
ਬਣਤਰ Curl_peer;

/* DNS query types */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNSQ_A           (1U << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNSQ_AAAA        (1U << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNSQ_HTTPS       (1U << 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_resolv_query_str(uint8_t dns_queries);
#ਜੇ_ਅੰਤ

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint8_t ip_ਸੰਸਕਰਣ);

ਗਿਣਤੀ alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(ਅਚਲ ਅੱਖਰ *hostname);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(ਬਣਤਰ Curl_multi *multi);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_probeipv6(x) CURLE_OK
#ਜੇ_ਅੰਤ

/* IPv4 thread-safe resolve function used for synch and asynch builds */
ਬਣਤਰ Curl_addrinfo *Curl_ipv4_resolve_r(ਅਚਲ ਅੱਖਰ *hostname, uint16_t ਪੋਰਟ);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
ਖਾਲੀ Curl_printable_ਪਤਾ(ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                            ਅੱਖਰ *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਬਣਤਰ Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

CURLcode Curl_resolv_blocking(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              uint8_t dns_queries,
                              ਅਚਲ ਅੱਖਰ *hostname,
                              uint16_t ਪੋਰਟ,
                              uint8_t transport,
                              ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਖਾਲੀ *resolver);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
ਬਣਤਰ Curl_resolv_async *Curl_async_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
ਖਾਲੀ Curl_resolv_shutdown_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Destroy all resolve resources of the given easy handle. */
ਖਾਲੀ Curl_resolv_destroy_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_resolv_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id,
                                 ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

ਖਾਲੀ Curl_resolv_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  uint32_t resolv_id,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             uint32_t resolv_id, uint8_t dns_queries);

ਅਚਲ ਬਣਤਰ Curl_addrinfo *Curl_resolv_get_ai(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                               uint32_t resolv_id,
                                               ਪੂਰਨ ai_family,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);


ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *Curl_resolv_get_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id);
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

#ਨਹੀਂਤਾਂ /* !USE_CURL_ASYNC */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_destroy_all(x)       Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_take_ਨਤੀਜਾ(x, y, z) CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_has_answers(x, y, z) TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_get_ai(x, y, z, a)   ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_get_https(x, y)      ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_knows_https(x, y)    TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_pollset(x, y)        CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_resolv_destroy(x, y)        Curl_nop_stmt
#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
ਬਣਤਰ Curl_addrinfo *Curl_sync_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            uint8_t dns_queries,
                                            ਅਚਲ ਅੱਖਰ *hostname,
                                            uint16_t ਪੋਰਟ,
                                            uint8_t transport);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_HOSTIP_H */
