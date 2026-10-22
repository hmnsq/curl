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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "curlx/timeval.h"
#அந்தர்பாவ "curlx/strparse.h"

கட்டமைப்பு addrinfo;
கட்டமைப்பு hostent;
கட்டமைப்பு Curl_easy;
கட்டமைப்பு connectdata;
கட்டமைப்பு easy_pollset;
கட்டமைப்பு Curl_https_rrinfo;
கட்டமைப்பு Curl_multi;
கட்டமைப்பு Curl_peer;

#define CURL_DNST_INIT      '\0'
#define CURL_DNST_ADDR      'A'
#define CURL_DNST_HTTPS     'H'

கட்டமைப்பு Curl_dns_entry {
  கட்டமைப்பு Curl_addrinfo *addr;
#ifdef USE_HTTPSRR
  கட்டமைப்பு Curl_https_rrinfo *hinfo;
#endif
  கட்டமைப்பு curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  எழுத்து type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  எழுத்து hostname[1];
};

கட்டமைப்பு dnsc_id {
  கட்டமைப்பு Curl_str name;
  uint16_t port;
  எழுத்து type;
};

#define MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

கட்டமைப்பு dnsc_சாவி {
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
கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_addr(கட்டமைப்பு Curl_easy *data,
                                         uint8_t dns_queries,
                                         கட்டமைப்பு Curl_addrinfo **paddr,
                                         கட்டமைப்பு Curl_peer *peer);

கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_addr2(கட்டமைப்பு Curl_easy *data,
                                          uint8_t dns_queries,
                                          கட்டமைப்பு Curl_addrinfo **paddr1,
                                          கட்டமைப்பு Curl_addrinfo **paddr2,
                                          கட்டமைப்பு Curl_peer *peer);

#ifdef USE_HTTPSRR
கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_https(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_https_rrinfo **phinfo,
                                          கட்டமைப்பு Curl_peer *peer);
#endif /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
வெறுமை Curl_dns_entry_unlink(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_dns_entry **pdns);

கட்டமைப்பு Curl_dnscache {
  கட்டமைப்பு Curl_hash entries;
};

/* init a new dns cache */
வெறுமை Curl_dnscache_init(கட்டமைப்பு Curl_dnscache *dns, size_t size);

வெறுமை Curl_dnscache_destroy(கட்டமைப்பு Curl_dnscache *dns);

/* prune old entries from the DNS cache */
வெறுமை Curl_dnscache_prune(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow);

/* clear the DNS cache */
வெறுமை Curl_dnscache_clear(கட்டமைப்பு Curl_easy *data);

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
CURLcode Curl_dnscache_get(கட்டமைப்பு Curl_easy *data,
                           uint8_t dns_queries,
                           கட்டமைப்பு Curl_peer *peer,
                           கட்டமைப்பு Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(கட்டமைப்பு Curl_easy *data,
                                    uint8_t dns_queries,
                                    கட்டமைப்பு Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(கட்டமைப்பு Curl_easy *data);

#endif /* HEADER_CURL_DNSCACHE_H */
