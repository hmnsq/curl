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
#ضمن "curl_setup.h"

#ضمن "hash.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"

بنية addrinfo;
بنية hostent;
بنية Curl_easy;
بنية connectdata;
بنية easy_pollset;
بنية Curl_https_rrinfo;
بنية Curl_multi;
بنية Curl_peer;

#define CURL_DNST_INIT      '\0'
#define CURL_DNST_ADDR      'A'
#define CURL_DNST_HTTPS     'H'

بنية Curl_dns_entry {
  بنية Curl_addrinfo *addr;
#ifdef USE_HTTPSRR
  بنية Curl_https_rrinfo *hinfo;
#endif
  بنية curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  حرف type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  حرف hostname[1];
};

بنية dnsc_id {
  بنية Curl_str name;
  uint16_t port;
  حرف type;
};

#define MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

بنية dnsc_مبودي {
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
بنية Curl_dns_entry *Curl_dnsc_mk_addr(بنية Curl_easy *data,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer);

بنية Curl_dns_entry *Curl_dnsc_mk_addr2(بنية Curl_easy *data,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer);

#ifdef USE_HTTPSRR
بنية Curl_dns_entry *Curl_dnsc_mk_https(بنية Curl_easy *data,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer);
#endif /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
فراغ Curl_dns_entry_unlink(بنية Curl_easy *data,
                           بنية Curl_dns_entry **pdns);

بنية Curl_dnscache {
  بنية Curl_hash entries;
};

/* init a new dns cache */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t size);

فراغ Curl_dnscache_destroy(بنية Curl_dnscache *dns);

/* prune old entries from the DNS cache */
فراغ Curl_dnscache_prune(بنية Curl_easy *data, ثابت بنية curltime *pnow);

/* clear the DNS cache */
فراغ Curl_dnscache_clear(بنية Curl_easy *data);

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
CURLcode Curl_dnscache_get(بنية Curl_easy *data,
                           uint8_t dns_queries,
                           بنية Curl_peer *peer,
                           بنية Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(بنية Curl_easy *data,
                           بنية Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(بنية Curl_easy *data,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(بنية Curl_easy *data);

#endif /* HEADER_CURL_DNSCACHE_H */
