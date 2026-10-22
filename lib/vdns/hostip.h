#اگر_غیر_تعریف_شدہ سرخی_CURL_HOSTIP_H
#تعریف_کرو سرخی_CURL_HOSTIP_H
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

#شامل "hash.h"
#شامل "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#تعریف_کرو CURL_HOSTENT_سائز 9000

#تعریف_کرو CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

ساخت addrinfo;
ساخت hostent;
ساخت Curl_easy;
ساخت connectdata;
ساخت easy_pollset;
ساخت Curl_https_rrinfo;
ساخت Curl_multi;
ساخت Curl_dns_اندراج;
ساخت Curl_peer;

/* DNS query types */
#تعریف_کرو CURL_DNSQ_A           (1U << 0)
#تعریف_کرو CURL_DNSQ_AAAA        (1U << 1)
#تعریف_کرو CURL_DNSQ_HTTPS       (1U << 2)

#تعریف_کرو CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#تعریف_کرو CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#اگر_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_resolv_query_str(uint8_t dns_queries);
#اگر_ختم

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(ساخت Curl_easy *ڈیٹا, uint8_t ip_نسخہ);

شمار alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(ثابت حرف *hostname);

#اگر_تعریف_شدہ USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(ساخت Curl_multi *multi);
#ورنہ
#تعریف_کرو Curl_probeipv6(x) CURLE_OK
#اگر_ختم

/* IPv4 thread-safe resolve function used for synch and asynch builds */
ساخت Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t پورٹ);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
خالی Curl_printable_پتہ(ثابت ساخت Curl_addrinfo *ai,
                            حرف *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(ساخت Curl_easy *ڈیٹا,
                     ساخت Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ساخت Curl_dns_اندراج **pdns);

CURLcode Curl_resolv_blocking(ساخت Curl_easy *ڈیٹا,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورٹ,
                              uint8_t transport,
                              ساخت Curl_dns_اندراج **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_آغاز(ساخت Curl_easy *ڈیٹا,
                                    خالی *resolver);

#اگر_تعریف_شدہ USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
ساخت Curl_resolv_async *Curl_async_get(ساخت Curl_easy *ڈیٹا,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
خالی Curl_resolv_shutdown_all(ساخت Curl_easy *ڈیٹا);

/* Destroy all resolve resources of the given easy handle. */
خالی Curl_resolv_destroy_all(ساخت Curl_easy *ڈیٹا);

CURLcode Curl_resolv_take_نتیجہ(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id,
                                 ساخت Curl_dns_اندراج **pdns);

خالی Curl_resolv_destroy(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(ساخت Curl_easy *ڈیٹا,
                                  uint32_t resolv_id,
                                  ثابت ساخت curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(ساخت Curl_easy *ڈیٹا,
                             uint32_t resolv_id, uint8_t dns_queries);

ثابت ساخت Curl_addrinfo *Curl_resolv_get_ai(ساخت Curl_easy *ڈیٹا,
                                               uint32_t resolv_id,
                                               صحیح ai_family,
                                               غیرعلامتی صحیح اشاریہ);
#اگر_تعریف_شدہ USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ساخت Curl_dns_اندراج **pdns);


ثابت ساخت Curl_https_rrinfo *Curl_resolv_get_https(ساخت Curl_easy *ڈیٹا,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id);
#اگر_ختم /* USE_HTTPSRR */

#ورنہ /* !USE_CURL_ASYNC */
#تعریف_کرو Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#تعریف_کرو Curl_resolv_destroy_all(x)       Curl_nop_stmt
#تعریف_کرو Curl_resolv_take_نتیجہ(x, y, z) CURLE_NOT_BUILT_اندر
#تعریف_کرو Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#تعریف_کرو Curl_resolv_has_answers(x, y, z) TRUE
#تعریف_کرو Curl_resolv_get_ai(x, y, z, a)   تہی
#تعریف_کرو Curl_resolv_get_https(x, y)      تہی
#تعریف_کرو Curl_resolv_knows_https(x, y)    TRUE
#تعریف_کرو Curl_resolv_pollset(x, y)        CURLE_OK
#تعریف_کرو Curl_resolv_destroy(x, y)        Curl_nop_stmt
#اگر_ختم /* USE_CURL_ASYNC */

#اگر_تعریف_شدہ CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
ساخت Curl_addrinfo *Curl_sync_getaddrinfo(ساخت Curl_easy *ڈیٹا,
                                            uint8_t dns_queries,
                                            ثابت حرف *hostname,
                                            uint16_t پورٹ,
                                            uint8_t transport);
#اگر_ختم

#اگر_ختم /* HEADER_CURL_HOSTIP_H */
