#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_DNSCACHE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_DNSCACHE_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ರಚನೆ addrinfo;
ರಚನೆ hostent;
ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ easy_pollset;
ರಚನೆ Curl_https_rrinfo;
ರಚನೆ Curl_multi;
ರಚನೆ Curl_peer;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNST_INIT      '\0'
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNST_ADDR      'A'
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNST_HTTPS     'H'

ರಚನೆ Curl_dns_ನಮೂದು {
  ರಚನೆ Curl_addrinfo *addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ರಚನೆ Curl_https_rrinfo *hinfo;
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ curltime added; /* time this entry was added to the cache */
  uint32_t refcount; /* reference counter, entry is freed on reaching 0 */
  uint16_t hostlen; /* length of hostname */
  uint16_t ಪೋರ್ಟ್; /* host port */
  ಅಕ್ಷರ type; /* CURL_DNST_ADDR or CURL_DNST_HTTPS */
  uint8_t dns_queries; /* CURL_DNSQ_* type of queries performed for this */
  uint8_t dns_responses; /* CURL_DNSQ_* type this entry has responses for */
  ಬಿಟ್(permanent); /* entry is permanent, e.g. does not time out */
  /* hostname that resolved to addr. may be NULL (Unix domain sockets). */
  ಅಕ್ಷರ hostname[1];
};

ರಚನೆ dnsc_id {
  ರಚನೆ Curl_str name;
  uint16_t ಪೋರ್ಟ್;
  ಅಕ್ಷರ type;
};

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HOSTCACHE_LEN (255 + 3) /* max FQDN + type + port */

ರಚನೆ dnsc_ಕೀಲಿ {
  uint8_t ದತ್ತಾಂಶ[MAX_HOSTCACHE_LEN];
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
ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         uint8_t dns_queries,
                                         ರಚನೆ Curl_addrinfo **paddr,
                                         ರಚನೆ Curl_peer *peer);

ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_addr2(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          uint8_t dns_queries,
                                          ರಚನೆ Curl_addrinfo **paddr1,
                                          ರಚನೆ Curl_addrinfo **paddr2,
                                          ರಚನೆ Curl_peer *peer);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_https_rrinfo **phinfo,
                                          ರಚನೆ Curl_peer *peer);
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

/* unlink a dns entry, frees all resources if it was the last reference.
 * Always clears `*pdns`` */
ಶೂನ್ಯ Curl_dns_ನಮೂದು_unlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_dns_ನಮೂದು **pdns);

ರಚನೆ Curl_dnscache {
  ರಚನೆ Curl_hash entries;
};

/* init a new dns cache */
ಶೂನ್ಯ Curl_dnscache_init(ರಚನೆ Curl_dnscache *dns, size_t ಗಾತ್ರ_2);

ಶೂನ್ಯ Curl_dnscache_destroy(ರಚನೆ Curl_dnscache *dns);

/* prune old entries from the DNS cache */
ಶೂನ್ಯ Curl_dnscache_prune(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/* clear the DNS cache */
ಶೂನ್ಯ Curl_dnscache_clear(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

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
CURLcode Curl_dnscache_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           uint8_t dns_queries,
                           ರಚನೆ Curl_peer *peer,
                           ರಚನೆ Curl_dns_ನಮೂದು **pentry);

/*
 * Curl_dnscache_addr() adds `entry` to the cache, increasing its
 * reference count on success.
 */
CURLcode Curl_dnscache_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_dns_ನಮೂದು *ನಮೂದು);

/* Store a "negative" entry for host:port, e.g. remember that
 * it could not be resolved. */
CURLcode Curl_dnscache_ಕೂಡಿಸು_negative(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t dns_queries,
                                    ರಚನೆ Curl_peer *peer);

/*
 * Populate the cache with specified entries from CURLOPT_RESOLVE.
 */
CURLcode Curl_loadhostpairs(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_DNSCACHE_H */
