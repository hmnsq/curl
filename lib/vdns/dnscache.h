#اگر_غیر_تعریف_شدہ سرخی_CURL_DNSCACHE_H
#تعریف_کرو سرخی_CURL_DNSCACHE_H
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
#شامل "curlx/timeval.h"
#شامل "curlx/strparse.h"

ساخت addrinfo;
ساخت hostent;
ساخت Curl_easy;
ساخت connectdata;
ساخت easy_pollset;
ساخت Curl_https_rrinfo;
ساخت Curl_multi;
ساخت Curl_peer;

#تعریف_کرو CURL_DNST_INIT      '\0'
#تعریف_کرو CURL_DNST_ADDR      'A'
#تعریف_کرو CURL_DNST_HTTPS     'H'

ساخت Curl_dns_اندراج {
  ساخت Curl_addrinfo *addr;
#اگر_تعریف_شدہ USE_HTTPSRR
  ساخت Curl_https_rrinfo *hinfo;
#اگر_ختم
  ساخت curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t پورٹ; /* host port */
  حرف type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  ثنائی_ہندسہ(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  حرف hostname[1];
};

ساخت dnsc_id {
  ساخت Curl_str name;
  uint16_t پورٹ;
  حرف type;
};

#تعریف_کرو MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

ساخت dnsc_کلید {
  uint8_t ڈیٹا[MAX_HOSTCACHE_LEN];
  size_t len;
};

/*
 * Create a `Curl_dns_entry` with a reference count of 1.
 * Use `Curl_dns_entry_unlink()` to release your hold on it.
 *
 * The call takes ownership of `paddr`, even in case of failure, and always
 * clears `*paddr`. It makes a copy of `hostname`.
 *
 * Returns entry or NULL on OOM.
 */
ساخت Curl_dns_اندراج *Curl_dnsc_mk_addr(ساخت Curl_easy *ڈیٹا,
                                         uint8_t dns_queries,
                                         ساخت Curl_addrinfo **paddr,
                                         ساخت Curl_peer *peer);

ساخت Curl_dns_اندراج *Curl_dnsc_mk_addr2(ساخت Curl_easy *ڈیٹا,
                                          uint8_t dns_queries,
                                          ساخت Curl_addrinfo **paddr1,
                                          ساخت Curl_addrinfo **paddr2,
                                          ساخت Curl_peer *peer);

#اگر_تعریف_شدہ USE_HTTPSRR
ساخت Curl_dns_اندراج *Curl_dnsc_mk_https(ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_https_rrinfo **phinfo,
                                          ساخت Curl_peer *peer);
#اگر_ختم /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
خالی Curl_dns_اندراج_unlink(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_dns_اندراج **pdns);

ساخت Curl_dnscache {
  ساخت Curl_hash entries;
};

/* init a new dns cache */
خالی Curl_dnscache_init(ساخت Curl_dnscache *dns, size_t سائز);

خالی Curl_dnscache_destroy(ساخت Curl_dnscache *dns);

/* prune old entries from the DNS cache */
خالی Curl_dnscache_prune(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow);

/* clear the DNS cache */
خالی Curl_dnscache_clear(ساخت Curl_easy *ڈیٹا);

/*
 * Curl_dnscache_get() fetches a 'Curl_dns_entry' already in the DNS cache.
 *
 * Returns the Curl_dns_entry entry pointer or NULL if not in the cache.
 *
 * The returned data *MUST* be "released" with Curl_dns_entry_unlink() after
 * use, or we will leak memory!
 * Returns CURLE_OK or CURLE_COULDNT_RESOLVE_HOST when a negative
 * entry was in the cache.
 */
CURLcode Curl_dnscache_get(ساخت Curl_easy *ڈیٹا,
                           uint8_t dns_queries,
                           ساخت Curl_peer *peer,
                           ساخت Curl_dns_اندراج **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_جوڑو(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_dns_اندراج *اندراج_2);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_جوڑو_negative(ساخت Curl_easy *ڈیٹا,
                                    uint8_t dns_queries,
                                    ساخت Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(ساخت Curl_easy *ڈیٹا);

#اگر_ختم /* HEADER_CURL_DNSCACHE_H */
