#अगर_अघोषित HEADER_CURL_DNSCACHE_H
#घोषणा HEADER_CURL_DNSCACHE_H
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
#समावेश "curl_setup.h"

#समावेश "hash.h"
#समावेश "curlx/timeval.h"
#समावेश "curlx/strparse.h"

काष्ठा addrinfo;
काष्ठा hostent;
काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा easy_pollset;
काष्ठा Curl_https_rrinfo;
काष्ठा Curl_multi;
काष्ठा Curl_peer;

#घोषणा CURL_DNST_INIT      '\0'
#घोषणा CURL_DNST_ADDR      'A'
#घोषणा CURL_DNST_HTTPS     'H'

काष्ठा Curl_dns_entry {
  काष्ठा Curl_addrinfo *addr;
#अगर_घोषित USE_HTTPSRR
  काष्ठा Curl_https_rrinfo *hinfo;
#पूर्ण_अगर
  काष्ठा curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  अक्षर type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  अक्षर hostname[1];
};

काष्ठा dnsc_id {
  काष्ठा Curl_str name;
  uint16_t port;
  अक्षर type;
};

#घोषणा MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

काष्ठा dnsc_कुंजी {
  uint8_t तथ्य[MAX_HOSTCACHE_LEN];
  माप_प्रकार len;
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
काष्ठा Curl_dns_entry *Curl_dnsc_mk_addr(काष्ठा Curl_easy *तथ्य,
                                         uint8_t dns_queries,
                                         काष्ठा Curl_addrinfo **paddr,
                                         काष्ठा Curl_peer *peer);

काष्ठा Curl_dns_entry *Curl_dnsc_mk_addr2(काष्ठा Curl_easy *तथ्य,
                                          uint8_t dns_queries,
                                          काष्ठा Curl_addrinfo **paddr1,
                                          काष्ठा Curl_addrinfo **paddr2,
                                          काष्ठा Curl_peer *peer);

#अगर_घोषित USE_HTTPSRR
काष्ठा Curl_dns_entry *Curl_dnsc_mk_https(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_https_rrinfo **phinfo,
                                          काष्ठा Curl_peer *peer);
#पूर्ण_अगर /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
व्योम Curl_dns_entry_unlink(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_dns_entry **pdns);

काष्ठा Curl_dnscache {
  काष्ठा Curl_hash entries;
};

/* init a new dns cache */
व्योम Curl_dnscache_init(काष्ठा Curl_dnscache *dns, माप_प्रकार आकार);

व्योम Curl_dnscache_destroy(काष्ठा Curl_dnscache *dns);

/* prune old entries from the DNS cache */
व्योम Curl_dnscache_prune(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow);

/* clear the DNS cache */
व्योम Curl_dnscache_clear(काष्ठा Curl_easy *तथ्य);

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
CURLcode Curl_dnscache_get(काष्ठा Curl_easy *तथ्य,
                           uint8_t dns_queries,
                           काष्ठा Curl_peer *peer,
                           काष्ठा Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(काष्ठा Curl_easy *तथ्य,
                                    uint8_t dns_queries,
                                    काष्ठा Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(काष्ठा Curl_easy *तथ्य);

#पूर्ण_अगर /* HEADER_CURL_DNSCACHE_H */
