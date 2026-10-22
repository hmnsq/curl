#నిర్వచితం_కాకపోతే శీర్షిక_CURL_DNSCACHE_H
#నిర్వచించు శీర్షిక_CURL_DNSCACHE_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "hash.h"
#అన్తర్భావ "curlx/timeval.h"
#అన్తర్భావ "curlx/strparse.h"

నిర్మాణం addrinfo;
నిర్మాణం hostent;
నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం easy_pollset;
నిర్మాణం Curl_https_rrinfo;
నిర్మాణం Curl_multi;
నిర్మాణం Curl_peer;

#నిర్వచించు CURL_DNST_INIT      '\0'
#నిర్వచించు CURL_DNST_ADDR      'A'
#నిర్వచించు CURL_DNST_HTTPS     'H'

నిర్మాణం Curl_dns_నమోదు {
  నిర్మాణం Curl_addrinfo *addr;
#నిర్వచితమైతే USE_HTTPSRR
  నిర్మాణం Curl_https_rrinfo *hinfo;
#అయితే_ముగింపు
  నిర్మాణం curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t పోర్టు; /* host port */
  అక్షరం type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  బిట్టు(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  అక్షరం hostname[1];
};

నిర్మాణం dnsc_id {
  నిర్మాణం Curl_str name;
  uint16_t పోర్టు;
  అక్షరం type;
};

#నిర్వచించు MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

నిర్మాణం dnsc_కీలకం {
  uint8_t దత్తాంశం[MAX_HOSTCACHE_LEN];
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
నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                                         uint8_t dns_queries,
                                         నిర్మాణం Curl_addrinfo **paddr,
                                         నిర్మాణం Curl_peer *peer);

నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_addr2(నిర్మాణం Curl_easy *దత్తాంశం,
                                          uint8_t dns_queries,
                                          నిర్మాణం Curl_addrinfo **paddr1,
                                          నిర్మాణం Curl_addrinfo **paddr2,
                                          నిర్మాణం Curl_peer *peer);

#నిర్వచితమైతే USE_HTTPSRR
నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_https_rrinfo **phinfo,
                                          నిర్మాణం Curl_peer *peer);
#అయితే_ముగింపు /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
శూన్య Curl_dns_నమోదు_unlink(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_dns_నమోదు **pdns);

నిర్మాణం Curl_dnscache {
  నిర్మాణం Curl_hash entries;
};

/* init a new dns cache */
శూన్య Curl_dnscache_init(నిర్మాణం Curl_dnscache *dns, size_t కొలత);

శూన్య Curl_dnscache_destroy(నిర్మాణం Curl_dnscache *dns);

/* prune old entries from the DNS cache */
శూన్య Curl_dnscache_prune(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow);

/* clear the DNS cache */
శూన్య Curl_dnscache_clear(నిర్మాణం Curl_easy *దత్తాంశం);

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
CURLcode Curl_dnscache_get(నిర్మాణం Curl_easy *దత్తాంశం,
                           uint8_t dns_queries,
                           నిర్మాణం Curl_peer *peer,
                           నిర్మాణం Curl_dns_నమోదు **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_dns_నమోదు *నమోదు_2);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_కూడు_negative(నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t dns_queries,
                                    నిర్మాణం Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* HEADER_CURL_DNSCACHE_H */
