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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "hash.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ସଂରଚନା addrinfo;
ସଂରଚନା hostent;
ସଂରଚନା Curl_easy;
ସଂରଚନା connectdata;
ସଂରଚନା easy_pollset;
ସଂରଚନା Curl_https_rrinfo;
ସଂରଚନା Curl_multi;
ସଂରଚନା Curl_peer;

#define CURL_DNST_INIT      '\0'
#define CURL_DNST_ADDR      'A'
#define CURL_DNST_HTTPS     'H'

ସଂରଚନା Curl_dns_entry {
  ସଂରଚନା Curl_addrinfo *addr;
#ifdef USE_HTTPSRR
  ସଂରଚନା Curl_https_rrinfo *hinfo;
#endif
  ସଂରଚନା curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t port; /* host port */
  ଅକ୍ଷର type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  ଅକ୍ଷର hostname[1];
};

ସଂରଚନା dnsc_id {
  ସଂରଚନା Curl_str name;
  uint16_t port;
  ଅକ୍ଷର type;
};

#define MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

ସଂରଚନା dnsc_ଚାବି {
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
ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_addr(ସଂରଚନା Curl_easy *data,
                                         uint8_t dns_queries,
                                         ସଂରଚନା Curl_addrinfo **paddr,
                                         ସଂରଚନା Curl_peer *peer);

ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_addr2(ସଂରଚନା Curl_easy *data,
                                          uint8_t dns_queries,
                                          ସଂରଚନା Curl_addrinfo **paddr1,
                                          ସଂରଚନା Curl_addrinfo **paddr2,
                                          ସଂରଚନା Curl_peer *peer);

#ifdef USE_HTTPSRR
ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_https(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_https_rrinfo **phinfo,
                                          ସଂରଚନା Curl_peer *peer);
#endif /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
ଶୂନ୍ୟ Curl_dns_entry_unlink(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_dns_entry **pdns);

ସଂରଚନା Curl_dnscache {
  ସଂରଚନା Curl_hash entries;
};

/* init a new dns cache */
ଶୂନ୍ୟ Curl_dnscache_init(ସଂରଚନା Curl_dnscache *dns, size_t size);

ଶୂନ୍ୟ Curl_dnscache_destroy(ସଂରଚନା Curl_dnscache *dns);

/* prune old entries from the DNS cache */
ଶୂନ୍ୟ Curl_dnscache_prune(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow);

/* clear the DNS cache */
ଶୂନ୍ୟ Curl_dnscache_clear(ସଂରଚନା Curl_easy *data);

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
CURLcode Curl_dnscache_get(ସଂରଚନା Curl_easy *data,
                           uint8_t dns_queries,
                           ସଂରଚନା Curl_peer *peer,
                           ସଂରଚନା Curl_dns_entry **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_add(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_dns_entry *entry);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_add_negative(ସଂରଚନା Curl_easy *data,
                                    uint8_t dns_queries,
                                    ସଂରଚନା Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(ସଂରଚନା Curl_easy *data);

#endif /* HEADER_CURL_DNSCACHE_H */
