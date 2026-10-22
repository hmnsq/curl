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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "curlx/timeval.h"
#অন্তর্ভাব "curlx/strparse.h"

কাঠামো addrinfo;
কাঠামো hostent;
কাঠামো Curl_easy;
কাঠামো connectdata;
কাঠামো easy_pollset;
কাঠামো Curl_https_rrinfo;
কাঠামো Curl_multi;
কাঠামো Curl_peer;

#define CURL_DNST_INIT      '\0'
#define CURL_DNST_ADDR      'A'
#define CURL_DNST_HTTPS     'H'

কাঠামো Curl_dns_entry {
  কাঠামো Curl_addrinfo *addr;
#ifdef USE_HTTPSRR
  কাঠামো Curl_https_rrinfo *hinfo;
#endif
  কাঠামো curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  অক্ষর type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  অক্ষর hostname[1];
};

কাঠামো dnsc_id {
  কাঠামো Curl_str name;
  uint16_t port;
  অক্ষর type;
};

#define MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

কাঠামো dnsc_চাবি {
  uint8_t তথ্য[MAX_HOSTCACHE_LEN];
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
কাঠামো Curl_dns_entry *Curl_dnsc_mk_addr(কাঠামো Curl_easy *তথ্য,
                                         uint8_t dns_queries,
                                         কাঠামো Curl_addrinfo **paddr,
                                         কাঠামো Curl_peer *peer);

কাঠামো Curl_dns_entry *Curl_dnsc_mk_addr2(কাঠামো Curl_easy *তথ্য,
                                          uint8_t dns_queries,
                                          কাঠামো Curl_addrinfo **paddr1,
                                          কাঠামো Curl_addrinfo **paddr2,
                                          কাঠামো Curl_peer *peer);

#ifdef USE_HTTPSRR
কাঠামো Curl_dns_entry *Curl_dnsc_mk_https(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_https_rrinfo **phinfo,
                                          কাঠামো Curl_peer *peer);
#endif /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
শূন্য Curl_dns_entry_unlink(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_dns_entry **pdns);

কাঠামো Curl_dnscache {
  কাঠামো Curl_hash entries;
};

/* init a new dns cache */
শূন্য Curl_dnscache_init(কাঠামো Curl_dnscache *dns, size_t আকার_2);

শূন্য Curl_dnscache_destroy(কাঠামো Curl_dnscache *dns);

/* prune old entries from the DNS cache */
শূন্য Curl_dnscache_prune(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow);

/* clear the DNS cache */
শূন্য Curl_dnscache_clear(কাঠামো Curl_easy *তথ্য);

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
CURLcode Curl_dnscache_get(কাঠামো Curl_easy *তথ্য,
                           uint8_t dns_queries,
                           কাঠামো Curl_peer *peer,
                           কাঠামো Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(কাঠামো Curl_easy *তথ্য,
                                    uint8_t dns_queries,
                                    কাঠামো Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(কাঠামো Curl_easy *তথ্য);

#endif /* HEADER_CURL_DNSCACHE_H */
