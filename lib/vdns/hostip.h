#اگر_تعریف_نشده سرآیند_CURL_HOSTIP_H
#تعریف_کن سرآیند_CURL_HOSTIP_H
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

#ضمن "hash.h"
#ضمن "curlx/timeval.h" /* for curltime, timediff_t */

/* Allocate enough memory to hold the full name information structs and
 * everything. OSF1 is known to require at least 8872 bytes. The buffer
 * required for storing all possible aliases and IP numbers is according to
 * Stevens' Unix Network Programming 2nd edition, p. 304: 8192 bytes!
 */
#تعریف_کن CURL_HOSTENT_اندازه 9000

#تعریف_کن CURL_TIMEOUT_RESOLVE_MS (300 * 1000)

بنية addrinfo;
بنية hostent;
بنية Curl_easy;
بنية connectdata;
بنية easy_pollset;
بنية Curl_https_rrinfo;
بنية Curl_multi;
بنية Curl_dns_مدخل;
بنية Curl_peer;

/* DNS query types */
#تعریف_کن CURL_DNSQ_A           (1U << 0)
#تعریف_کن CURL_DNSQ_AAAA        (1U << 1)
#تعریف_کن CURL_DNSQ_HTTPS       (1U << 2)

#تعریف_کن CURL_DNSQ_ADDR        (uint8_t)(CURL_DNSQ_A | CURL_DNSQ_AAAA)
#تعریف_کن CURL_DNSQ_IS_ADDR(x)  (uint8_t)((x)&(CURL_DNSQ_ADDR))

#اگر_تعریف_شده CURLVERBOSE
ثابت حرف *Curl_resolv_query_str(uint8_t dns_queries);
#پایان_اگر

/* Return CURL_DNSQ_* bits for the transfer and ip_version. */
uint8_t Curl_resolv_dns_queries(بنية Curl_easy *داده, uint8_t ip_نسخه);

تعداد alpnid {
  ALPN_none = 0,
  ALPN_h1 = CURLALTSVC_H1,
  ALPN_h2 = CURLALTSVC_H2,
  ALPN_h3 = CURLALTSVC_H3
};

bool Curl_host_is_ipnum(ثابت حرف *hostname);

#اگر_تعریف_شده USE_IPV6
/* probe if it seems to work */
CURLcode Curl_probeipv6(بنية Curl_multi *multi);
#وإلا
#تعریف_کن Curl_probeipv6(x) CURLE_OK
#پایان_اگر

/* IPv4 thread-safe resolve function used for synch and asynch builds */
بنية Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t پورت);

/*
 * Curl_printable_address() returns a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 */
فراغ Curl_printable_آدرس(ثابت بنية Curl_addrinfo *ai,
                            حرف *buf, size_t bufsize);

/* Start DNS resolving for the given parameters. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv(بنية Curl_easy *داده,
                     بنية Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     بنية Curl_dns_مدخل **pdns);

CURLcode Curl_resolv_blocking(بنية Curl_easy *داده,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورت,
                              uint8_t transport,
                              بنية Curl_dns_مدخل **pdns);

/* Announce start of a resolve operation to application callback,
 * passing the resolver implementation (maybe NULL). */
CURLcode Curl_resolv_announce_شروع(بنية Curl_easy *داده,
                                    فراغ *resolver);

#اگر_تعریف_شده USE_CURL_ASYNC

CURLcode Curl_resolv_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps);

/* Get the `async` struct for the given `resolv_id`, if it exists. */
بنية Curl_resolv_async *Curl_async_get(بنية Curl_easy *داده,
                                         uint32_t resolv_id);

/* Shut down all resolves of the given easy handle. */
فراغ Curl_resolv_shutdown_all(بنية Curl_easy *داده);

/* Destroy all resolve resources of the given easy handle. */
فراغ Curl_resolv_destroy_all(بنية Curl_easy *داده);

CURLcode Curl_resolv_take_نتیجه(بنية Curl_easy *داده, uint32_t resolv_id,
                                 بنية Curl_dns_مدخل **pdns);

فراغ Curl_resolv_destroy(بنية Curl_easy *داده, uint32_t resolv_id);

/* How much time has gone by since start of resolve.
 * Returns CURL_TIMEOUT_RESOLVE_MS if `resolv_id` is no longer valid. */
timediff_t Curl_resolv_elapsed_ms(بنية Curl_easy *داده,
                                  uint32_t resolv_id,
                                  ثابت بنية curltime *pnow);

/* Return TRUE if `resolv_id` has answers (positive or negative) to
 * all queries in `dns_queries`.
 * Queries not requested are considered answered. */
bool Curl_resolv_has_answers(بنية Curl_easy *داده,
                             uint32_t resolv_id, uint8_t dns_queries);

ثابت بنية Curl_addrinfo *Curl_resolv_get_ai(بنية Curl_easy *داده,
                                               uint32_t resolv_id,
                                               صحيح ai_family,
                                               بیعلامت صحيح شاخص);
#اگر_تعریف_شده USE_HTTPSRR

/* Start DNS resolving for HTTPS records. Returns
 * - CURLE_OK: `*pdns` is the resolved DNS entry (needs to be unlinked).
    *          `*presolv_id` is 0.
 * - CURLE_AGAIN: resolve is asynchronous and not finished yet.
 *             `presolv_id` is the identifier for querying results later.
 * - other: the operation failed, `*pdns` is NULL, `*presolv_id` is 0.
 */
CURLcode Curl_resolv_https(بنية Curl_easy *داده,
                           بنية Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           بنية Curl_dns_مدخل **pdns);


ثابت بنية Curl_https_rrinfo *Curl_resolv_get_https(بنية Curl_easy *داده,
                                                      uint32_t resolv_id);
bool Curl_resolv_knows_https(بنية Curl_easy *داده, uint32_t resolv_id);
#پایان_اگر /* USE_HTTPSRR */

#وإلا /* !USE_CURL_ASYNC */
#تعریف_کن Curl_resolv_shutdown_all(x)      Curl_nop_stmt
#تعریف_کن Curl_resolv_destroy_all(x)       Curl_nop_stmt
#تعریف_کن Curl_resolv_take_نتیجه(x, y, z) CURLE_NOT_BUILT_درون
#تعریف_کن Curl_resolv_elapsed_ms(x, y, z)  CURL_TIMEOUT_RESOLVE_MS
#تعریف_کن Curl_resolv_has_answers(x, y, z) TRUE
#تعریف_کن Curl_resolv_get_ai(x, y, z, a)   عدم
#تعریف_کن Curl_resolv_get_https(x, y)      عدم
#تعریف_کن Curl_resolv_knows_https(x, y)    TRUE
#تعریف_کن Curl_resolv_pollset(x, y)        CURLE_OK
#تعریف_کن Curl_resolv_destroy(x, y)        Curl_nop_stmt
#پایان_اگر /* USE_CURL_ASYNC */

#اگر_تعریف_شده CURLRES_SYNCH
/*
 * Curl_sync_getaddrinfo() is the non-async low-level name resolve API.
 * There are several versions of this function - depending on IPV6
 * support and platform.
 */
بنية Curl_addrinfo *Curl_sync_getaddrinfo(بنية Curl_easy *داده,
                                            uint8_t dns_queries,
                                            ثابت حرف *hostname,
                                            uint16_t پورت,
                                            uint8_t transport);
#پایان_اگر

#پایان_اگر /* HEADER_CURL_HOSTIP_H */
