#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HOSTIP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HOSTIP_H
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

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HOSTENT_ಗಾತ್ರ 9000

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

ರಚನೆ addrinfo;
ರಚನೆ hostent;
ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ easy_pollset;
ರಚನೆ Curl_https_rrinfo;
ರಚನೆ Curl_multi;
ರಚನೆ Curl_dns_ನಮೂದು;
ರಚನೆ Curl_peer;

/* DNS query types */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNSQ_A           (1U << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNSQ_AAAA        (1U << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNSQ_HTTPS       (1U << 2)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_resolv_query_str(uint8_t dns_queries);
#ಆದರೆ_ಅಂತ್ಯ

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint8_t ip_ಆವೃತ್ತಿ);

ಎಣಿಕೆ alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(ರಚನೆ Curl_multi *multi);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_probeipv6(x) CURLE_OK
#ಆದರೆ_ಅಂತ್ಯ

/* IPv4 thread-safe resolve function used for synch and asynch builds */
ರಚನೆ Curl_addrinfo *Curl_ipv4_resolve_r(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, uint16_t ಪೋರ್ಟ್);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
ಶೂನ್ಯ Curl_printable_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                            ಅಕ್ಷರ *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ರಚನೆ Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ರಚನೆ Curl_dns_ನಮೂದು **pdns);

CURLcode Curl_resolv_blocking(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              uint8_t dns_queries,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                              uint16_t ಪೋರ್ಟ್,
                              uint8_t transport,
                              ರಚನೆ Curl_dns_ನಮೂದು **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಶೂನ್ಯ *resolver);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
ರಚನೆ Curl_resolv_async *Curl_async_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
ಶೂನ್ಯ Curl_resolv_shutdown_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Destroy all resolve resources of the given easy handle. */
ಶೂನ್ಯ Curl_resolv_destroy_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_resolv_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id,
                                 ರಚನೆ Curl_dns_ನಮೂದು **pdns);

ಶೂನ್ಯ Curl_resolv_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  uint32_t resolv_id,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             uint32_t resolv_id, uint8_t dns_queries);

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *Curl_resolv_get_ai(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                               uint32_t resolv_id,
                                               ಪೂರ್ಣಾಂಕ ai_family,
                                               ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ರಚನೆ Curl_dns_ನಮೂದು **pdns);


ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *Curl_resolv_get_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id);
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

#ಇಲ್ಲದಿದ್ದರೆ /* !USE_CURL_ASYNC */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_destroy_all(x)       Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_take_ಫಲಿತಾಂಶ(x, y, z) CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_has_answers(x, y, z) TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_get_ai(x, y, z, a)   ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_get_https(x, y)      ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_knows_https(x, y)    TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_pollset(x, y)        CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_resolv_destroy(x, y)        Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
ರಚನೆ Curl_addrinfo *Curl_sync_getaddrinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            uint8_t dns_queries,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                            uint16_t ಪೋರ್ಟ್,
                                            uint8_t transport);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HOSTIP_H */
