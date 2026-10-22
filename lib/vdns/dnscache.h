#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_DNSCACHE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_DNSCACHE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਬਣਤਰ addrinfo;
ਬਣਤਰ hostent;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ easy_pollset;
ਬਣਤਰ Curl_https_rrinfo;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ Curl_peer;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNST_INIT      '\0'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNST_ADDR      'A'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNST_HTTPS     'H'

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ {
  ਬਣਤਰ Curl_addrinfo *addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਬਣਤਰ Curl_https_rrinfo *hinfo;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t ਪੋਰਟ; /* host port */
  ਅੱਖਰ type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  ਦੋਆਧਾਰੀ_ਅੰਕ(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  ਅੱਖਰ hostname[1];
};

ਬਣਤਰ dnsc_id {
  ਬਣਤਰ Curl_str name;
  uint16_t ਪੋਰਟ;
  ਅੱਖਰ type;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

ਬਣਤਰ dnsc_ਕੁੰਜੀ {
  uint8_t ਅੰਕੜੇ[MAX_HOSTCACHE_LEN];
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
ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         uint8_t dns_queries,
                                         ਬਣਤਰ Curl_addrinfo **paddr,
                                         ਬਣਤਰ Curl_peer *peer);

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_addr2(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          uint8_t dns_queries,
                                          ਬਣਤਰ Curl_addrinfo **paddr1,
                                          ਬਣਤਰ Curl_addrinfo **paddr2,
                                          ਬਣਤਰ Curl_peer *peer);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_https_rrinfo **phinfo,
                                          ਬਣਤਰ Curl_peer *peer);
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
ਖਾਲੀ Curl_dns_ਇੰਦਰਾਜ_unlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

ਬਣਤਰ Curl_dnscache {
  ਬਣਤਰ Curl_hash entries;
};

/* init a new dns cache */
ਖਾਲੀ Curl_dnscache_init(ਬਣਤਰ Curl_dnscache *dns, size_t ਮਾਪ);

ਖਾਲੀ Curl_dnscache_destroy(ਬਣਤਰ Curl_dnscache *dns);

/* prune old entries from the DNS cache */
ਖਾਲੀ Curl_dnscache_prune(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow);

/* clear the DNS cache */
ਖਾਲੀ Curl_dnscache_clear(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

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
CURLcode Curl_dnscache_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           uint8_t dns_queries,
                           ਬਣਤਰ Curl_peer *peer,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_ਜੋੜੋ_negative(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t dns_queries,
                                    ਬਣਤਰ Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* HEADER_CURL_DNSCACHE_H */
