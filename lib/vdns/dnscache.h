#यदि_अपरिभाषितम् शीर्षकम्_CURL_DNSCACHE_H
#परिभाषय शीर्षकम्_CURL_DNSCACHE_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "hash.h"
#अन्तर्भाव "curlx/timeval.h"
#अन्तर्भाव "curlx/strparse.h"

संरचना addrinfo;
संरचना hostent;
संरचना Curl_easy;
संरचना connectdata;
संरचना easy_pollset;
संरचना Curl_https_rrinfo;
संरचना Curl_multi;
संरचना Curl_peer;

#परिभाषय CURL_DNST_INIT      '\0'
#परिभाषय CURL_DNST_ADDR      'A'
#परिभाषय CURL_DNST_HTTPS     'H'

संरचना Curl_dns_प्रविष्टिः {
  संरचना Curl_addrinfo *addr;
#यदि_परिभाषितम् USE_HTTPSRR
  संरचना Curl_https_rrinfo *hinfo;
#यदि_अन्तः
  संरचना curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t पत्तनम्; /* host port */
  वर्ण type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  BIT(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  वर्ण hostname[1];
};

संरचना dnsc_id {
  संरचना Curl_str name;
  uint16_t पत्तनम्;
  वर्ण type;
};

#परिभाषय MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

संरचना dnsc_कुञ्चिका {
  uint8_t दत्त[MAX_HOSTCACHE_LEN];
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
संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_addr(संरचना Curl_easy *दत्त,
                                         uint8_t dns_queries,
                                         संरचना Curl_addrinfo **paddr,
                                         संरचना Curl_peer *peer);

संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_addr2(संरचना Curl_easy *दत्त,
                                          uint8_t dns_queries,
                                          संरचना Curl_addrinfo **paddr1,
                                          संरचना Curl_addrinfo **paddr2,
                                          संरचना Curl_peer *peer);

#यदि_परिभाषितम् USE_HTTPSRR
संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_https(संरचना Curl_easy *दत्त,
                                          संरचना Curl_https_rrinfo **phinfo,
                                          संरचना Curl_peer *peer);
#यदि_अन्तः /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
शून्य Curl_dns_प्रविष्टिः_unlink(संरचना Curl_easy *दत्त,
                           संरचना Curl_dns_प्रविष्टिः **pdns);

संरचना Curl_dnscache {
  संरचना Curl_hash entries;
};

/* init a new dns cache */
शून्य Curl_dnscache_init(संरचना Curl_dnscache *dns, size_t आकार);

शून्य Curl_dnscache_destroy(संरचना Curl_dnscache *dns);

/* prune old entries from the DNS cache */
शून्य Curl_dnscache_prune(संरचना Curl_easy *दत्त, ध्रुव संरचना curltime *pnow);

/* clear the DNS cache */
शून्य Curl_dnscache_clear(संरचना Curl_easy *दत्त);

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
CURLcode Curl_dnscache_get(संरचना Curl_easy *दत्त,
                           uint8_t dns_queries,
                           संरचना Curl_peer *peer,
                           संरचना Curl_dns_प्रविष्टिः **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_सङ्कलय(संरचना Curl_easy *दत्त,
                           संरचना Curl_dns_प्रविष्टिः *प्रविष्टिः);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_सङ्कलय_negative(संरचना Curl_easy *दत्त,
                                    uint8_t dns_queries,
                                    संरचना Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(संरचना Curl_easy *दत्त);

#यदि_अन्तः /* HEADER_CURL_DNSCACHE_H */
