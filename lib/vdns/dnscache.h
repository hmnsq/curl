#ifndef HEADER_CURL_DNSCACHE_H
#define HEADER_CURL_DNSCACHE_H
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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "hash.h"
#অন্তর্ভাৱ "curlx/timeval.h"
#অন্তর্ভাৱ "curlx/strparse.h"

সংরচনা addrinfo;
সংরচনা hostent;
সংরচনা Curl_easy;
সংরচনা connectdata;
সংরচনা easy_pollset;
সংরচনা Curl_https_rrinfo;
সংরচনা Curl_multi;
সংরচনা Curl_peer;

#define CURL_DNST_INIT      '\0'
#define CURL_DNST_ADDR      'A'
#define CURL_DNST_HTTPS     'H'

সংরচনা Curl_dns_entry {
  সংরচনা Curl_addrinfo *addr;
#ifdef USE_HTTPSRR
  সংরচনা Curl_https_rrinfo *hinfo;
#endif
  সংরচনা curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  ৱর্ণ type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  ৱর্ণ hostname[1];
};

সংরচনা dnsc_id {
  সংরচনা Curl_str name;
  uint16_t port;
  ৱর্ণ type;
};

#define MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

সংরচনা dnsc_চাবি {
  uint8_t data[MAX_HOSTCACHE_LEN];
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
সংরচনা Curl_dns_entry *Curl_dnsc_mk_addr(সংরচনা Curl_easy *data,
                                         uint8_t dns_queries,
                                         সংরচনা Curl_addrinfo **paddr,
                                         সংরচনা Curl_peer *peer);

সংরচনা Curl_dns_entry *Curl_dnsc_mk_addr2(সংরচনা Curl_easy *data,
                                          uint8_t dns_queries,
                                          সংরচনা Curl_addrinfo **paddr1,
                                          সংরচনা Curl_addrinfo **paddr2,
                                          সংরচনা Curl_peer *peer);

#ifdef USE_HTTPSRR
সংরচনা Curl_dns_entry *Curl_dnsc_mk_https(সংরচনা Curl_easy *data,
                                          সংরচনা Curl_https_rrinfo **phinfo,
                                          সংরচনা Curl_peer *peer);
#endif /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
শূন্য Curl_dns_entry_unlink(সংরচনা Curl_easy *data,
                           সংরচনা Curl_dns_entry **pdns);

সংরচনা Curl_dnscache {
  সংরচনা Curl_hash entries;
};

/* init a new dns cache */
শূন্য Curl_dnscache_init(সংরচনা Curl_dnscache *dns, size_t size);

শূন্য Curl_dnscache_destroy(সংরচনা Curl_dnscache *dns);

/* prune old entries from the DNS cache */
শূন্য Curl_dnscache_prune(সংরচনা Curl_easy *data, ধ্রুৱ সংরচনা curltime *pnow);

/* clear the DNS cache */
শূন্য Curl_dnscache_clear(সংরচনা Curl_easy *data);

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
CURLcode Curl_dnscache_get(সংরচনা Curl_easy *data,
                           uint8_t dns_queries,
                           সংরচনা Curl_peer *peer,
                           সংরচনা Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(সংরচনা Curl_easy *data,
                           সংরচনা Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(সংরচনা Curl_easy *data,
                                    uint8_t dns_queries,
                                    সংরচনা Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(সংরচনা Curl_easy *data);

#endif /* HEADER_CURL_DNSCACHE_H */
