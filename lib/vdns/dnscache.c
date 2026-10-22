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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vdns/dnscache.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DNS_CACHE_ಗಾತ್ರ 29999

/* @unittest 3217 */
UNITTEST ಶೂನ್ಯ dnsc_peer2id(ರಚನೆ dnsc_id *pid, ಅಕ್ಷರ type,
                           ರಚನೆ Curl_peer *peer);
UNITTEST ಶೂನ್ಯ dnsc_peer2id(ರಚನೆ dnsc_id *pid, ಅಕ್ಷರ type,
                           ರಚನೆ Curl_peer *peer)
{
  curlx_str_ನಿಯೋಜಿಸು(&pid->name, peer->hostname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->hostname));
  pid->ಪೋರ್ಟ್ = peer->ಪೋರ್ಟ್;
  pid->type = type;
}

ಸ್ಥಿರ ಶೂನ್ಯ dnsc_str2id(ರಚನೆ dnsc_id *pid, ಅಕ್ಷರ type,
                        ರಚನೆ Curl_str *name, uint16_t ಪೋರ್ಟ್)
{
  pid->name = *name;
  pid->ಪೋರ್ಟ್ = ಪೋರ್ಟ್;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST ಶೂನ್ಯ dnsc_id2key(ರಚನೆ dnsc_ಕೀಲಿ *ಕೀಲಿ, ರಚನೆ dnsc_id *id);
UNITTEST ಶೂನ್ಯ dnsc_id2key(ರಚನೆ dnsc_ಕೀಲಿ *ಕೀಲಿ, ರಚನೆ dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  ಆದರೆ(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ಕೀಲಿ->ದತ್ತಾಂಶ[0] = id->type;
  ಕೀಲಿ->ದತ್ತಾಂಶ[1] = (uint8_t)((id->ಪೋರ್ಟ್ >> 8) & 0xff);
  ಕೀಲಿ->ದತ್ತಾಂಶ[2] = (uint8_t)(id->ಪೋರ್ಟ್ & 0xff);
  Curl_strntolower((ಅಕ್ಷರ *)ಕೀಲಿ->ದತ್ತಾಂಶ + 3, curlx_str(&id->name), namelen);
  ಕೀಲಿ->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_ನಮೂದು(ರಚನೆ dnsc_id *id,
                                    ರಚನೆ Curl_dns_ನಮೂದು *dns);
UNITTEST bool dnsc_id_matches_ನಮೂದು(ರಚನೆ dnsc_id *id,
                                    ರಚನೆ Curl_dns_ನಮೂದು *dns)
{
  ಹಿಂತಿರುಗಿ dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->ಪೋರ್ಟ್ == dns->ಪೋರ್ಟ್) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_ನಮೂದು_free(ರಚನೆ Curl_dns_ನಮೂದು *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#ಆದರೆ_ಅಂತ್ಯ
  curlx_free(dns);
}

ರಚನೆ dnscache_prune_ದತ್ತಾಂಶ {
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow;
  timediff_t oldest_ms; /* oldest time in cache not pruned. */
  timediff_t max_age_ms;
};

/*
 * This function is set as a callback to be called for every entry in the DNS
 * cache when we want to prune old unused entries.
 *
 * Returning non-zero means remove the entry, return 0 to keep it in the
 * cache.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dnscache_ನಮೂದು_is_stale(ಶೂನ್ಯ *datap, ಶೂನ್ಯ *hc)
{
  ರಚನೆ dnscache_prune_ದತ್ತಾಂಶ *prune = (ರಚನೆ dnscache_prune_ದತ್ತಾಂಶ *)datap;
  ರಚನೆ Curl_dns_ನಮೂದು *dns = (ರಚನೆ Curl_dns_ನಮೂದು *)hc;

  ಆದರೆ(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ಆಯ್ಕೆ(dns->type) {
    ಸ್ಥಿತಿ CURL_DNST_ADDR:
      ಆದರೆ(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
    ಸ್ಥಿತಿ CURL_DNST_HTTPS:
      ಆದರೆ(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(age_ms >= prune->max_age_ms)
      ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ಸ್ಥಿರ timediff_t dnscache_prune(ರಚನೆ Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ dnscache_prune_ದತ್ತಾಂಶ user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (ಶೂನ್ಯ *)&user,
                                 dnscache_ನಮೂದು_is_stale);

  ಹಿಂತಿರುಗಿ user.oldest_ms;
}

ಸ್ಥಿರ ರಚನೆ Curl_dnscache *dnscache_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->share && ದತ್ತಾಂಶ->share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_DNS))
    ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->share->dnscache;
  ಆದರೆ(ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->multi->dnscache;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_ಬೀಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_dnscache *dnscache)
{
  ಆದರೆ(ದತ್ತಾಂಶ->share && dnscache == &ದತ್ತಾಂಶ->share->dnscache)
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_DNS, CURL_ಬೀಗ_ACCESS_SINGLE);
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_unlock(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_dnscache *dnscache)
{
  ಆದರೆ(ದತ್ತಾಂಶ->share && dnscache == &ದತ್ತಾಂಶ->share->dnscache)
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
ಶೂನ್ಯ Curl_dnscache_prune(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = ದತ್ತಾಂಶ->ಗಣ.dns_cache_timeout_ms;

  ಆದರೆ(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ಹಿಂತಿರುಗಿ;

  dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);

  ಮಾಡು {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    ಆದರೆ(Curl_hash_ಎಣಿಕೆ(&dnscache->entries) > MAX_DNS_CACHE_ಗಾತ್ರ)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;

    /* if the cache size is still too big, use the oldest age as new prune
ಎಲ್ಲೆ*/
  } ಆಗ(timeout_ms);

  dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
}

ಶೂನ್ಯ Curl_dnscache_clear(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  ಆದರೆ(dnscache) {
    dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ಸ್ಥಿರ CURLcode fetch_ನಮೂದು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ರಚನೆ Curl_peer *peer,
                            ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  ರಚನೆ dnsc_id id;
  ರಚನೆ dnsc_ಕೀಲಿ ಕೀಲಿ;
  ಅಕ್ಷರ type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pdns = ಖಾಲಿ;
  ಆದರೆ(!dnscache)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ಕೀಲಿ, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);

  ಆದರೆ(dns && !dnsc_id_matches_ನಮೂದು(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ಖಾಲಿ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  ಆದರೆ(!dns && (type == CURL_DNST_ADDR) && ದತ್ತಾಂಶ->state.wildcard_resolve) {
    ರಚನೆ Curl_str wildname;

    curlx_str_ನಿಯೋಜಿಸು(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->ಪೋರ್ಟ್);
    dnsc_id2key(&ಕೀಲಿ, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);
  }

  ಆದರೆ(dns && (ದತ್ತಾಂಶ->ಗಣ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ರಚನೆ dnscache_prune_ದತ್ತಾಂಶ user;

    user.pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    user.max_age_ms = ದತ್ತಾಂಶ->ಗಣ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    ಆದರೆ(dnscache_ನಮೂದು_is_stale(&user, dns)) {
      infof(ದತ್ತಾಂಶ, "Hostname in DNS cache was stale, zapped");
      dns = ಖಾಲಿ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  ಆದರೆ(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    ಆದರೆ((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(ದತ್ತಾಂಶ, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ಖಾಲಿ;
      ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_HOST;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dns && !dns->addr) { /* negative entry */
      dns = ಖಾಲಿ;
      ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_dnscache_get() fetches a 'Curl_dns_entry' already in the DNS cache.
 *
 * Curl_resolv() checks initially and multi_runsingle() checks each time
 * it discovers the handle in the state WAITRESOLVE whether the hostname
 * has already been resolved and the address has already been stored in
 * the DNS cache. This short circuits waiting for a lot of pending
 * lookups for the same hostname requested by different handles.
 *
 * Returns the Curl_dns_entry entry pointer or NULL if not in the cache.
 *
 * The returned data *MUST* be "released" with Curl_dns_entry_unlink() after
 * use, or we will leak memory!
 */
CURLcode Curl_dnscache_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           uint8_t dns_queries,
                           ರಚನೆ Curl_peer *peer,
                           ರಚನೆ Curl_dns_ನಮೂದು **pentry)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);
  ಫಲಿತಾಂಶ = fetch_ನಮೂದು(ದತ್ತಾಂಶ, dnscache, dns_queries, peer, &dns);
  ಆದರೆ(!ಫಲಿತಾಂಶ && dns)
    dns->refcount++; /* we pass out a reference */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    DEBUGASSERT(!dns);
    dns = ಖಾಲಿ;
  }
  dnscache_unlock(ದತ್ತಾಂಶ, dnscache);

  CURL_TRC_DNS(ದತ್ತಾಂಶ, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->ಪೋರ್ಟ್, Curl_resolv_query_str(dns_queries),
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, dns ? "" : "not ");
  *pentry = dns;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ವಿಳಾಸಗಳು(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *addr)
{
  ಪೂರ್ಣಾಂಕ i = 0;
  ಆಗ(addr) {
    addr = addr->ai_ಮುಂದಿನ;
    i++;
  }
  ಹಿಂತಿರುಗಿ i;
}

/*
 * dns_shuffle_addr() shuffles the order of addresses in a 'Curl_addrinfo'
 * struct by re-linking its linked list.
 *
 * The addr argument should be the address of a pointer to the head node of a
 * `Curl_addrinfo` list and it will be modified to point to the new head after
 * shuffling.
 *
 * Not declared static only to make it easy to use in a unit test!
 *
 * @unittest 1608
 */
UNITTEST CURLcode dns_shuffle_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_addrinfo **addr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_addrs = ಅಂಕೆ_ವಿಳಾಸಗಳು(*addr);

  ಆದರೆ(ಅಂಕೆ_addrs > 1) {
    ರಚನೆ Curl_addrinfo **nodes;
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "Shuffling %d addresses", ಅಂಕೆ_addrs);

    nodes = curlx_malloc(ಅಂಕೆ_addrs * ಗಾತ್ರ(*nodes));
    ಆದರೆ(nodes) {
      ಪೂರ್ಣಾಂಕ i;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *rnd;
      ಸ್ಥಿರಾಂಕ size_t rnd_ಗಾತ್ರ = ಅಂಕೆ_addrs * ಗಾತ್ರ(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      ಗಾಗಿ(i = 1; i < ಅಂಕೆ_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_ಮುಂದಿನ;
      }

      rnd = curlx_malloc(rnd_ಗಾತ್ರ);
      ಆದರೆ(rnd) {
        /* Fisher-Yates shuffle */
        ಆದರೆ(Curl_rand(ದತ್ತಾಂಶ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)rnd, rnd_ಗಾತ್ರ) == CURLE_OK) {
          ರಚನೆ Curl_addrinfo *ಅದಲುಬದಲು_tmp;
          ಗಾಗಿ(i = ಅಂಕೆ_addrs - 1; i > 0; i--) {
            ಅದಲುಬದಲು_tmp = nodes[rnd[i] % (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(i + 1)];
            nodes[rnd[i] % (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(i + 1)] = nodes[i];
            nodes[i] = ಅದಲುಬದಲು_tmp;
          }

          /* relink list in the new order */
          ಗಾಗಿ(i = 1; i < ಅಂಕೆ_addrs; i++) {
            nodes[i - 1]->ai_ಮುಂದಿನ = nodes[i];
          }

          nodes[ಅಂಕೆ_addrs - 1]->ai_ಮುಂದಿನ = ಖಾಲಿ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      curlx_free(nodes);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ bool dnscache_ai_has_family(ರಚನೆ Curl_addrinfo *ai, ಪೂರ್ಣಾಂಕ ai_family)
{
  ಗಾಗಿ(; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಆದರೆ(ai->ai_family == ai_family)
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ನಮೂದು_create(ರಚನೆ dnsc_id *pid,
                                                bool permanent)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;

  ಆದರೆ(curlx_strlen(&pid->name) > UINT16_MAX)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_dns_ನಮೂದು) +
                     curlx_strlen(&pid->name));
  ಆದರೆ(!dns)
    ಹೋಗು ಹೊರಹೋಗು;

  dns->refcount = 1; /* the cache has the first reference */
  dns->ಪೋರ್ಟ್ = pid->ಪೋರ್ಟ್;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  ಆದರೆ(dns->hostlen)
    ಸ್ಮರಣೆ_ನಕಲು(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ dns;
}

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_addr(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ Curl_dns_ನಮೂದು *dns,
  uint8_t dns_queries,
  ರಚನೆ Curl_addrinfo **paddr1,
  ರಚನೆ Curl_addrinfo **paddr2)
{
  ಆದರೆ(!dns)
    ಹೋಗು ಹೊರಹೋಗು;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  ಆದರೆ(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ಖಾಲಿ;
  }
  ಆದರೆ(paddr2 && *paddr2) {
    ರಚನೆ Curl_addrinfo **phead = &dns->addr;
    ಆಗ(*phead)
      phead = &(*phead)->ai_ಮುಂದಿನ;
    *phead = *paddr2;
    *paddr2 = ಖಾಲಿ;
  }

  ಆದರೆ((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#ಆದರೆ_ಅಂತ್ಯ /* USE_IPV6 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.dns_shuffle_ವಿಳಾಸಗಳು && dns->addr) {
    CURLcode ಫಲಿತಾಂಶ = dns_shuffle_addr(ದತ್ತಾಂಶ, &dns->addr);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      /* free without lock, we are the sole owner */
      dnscache_ನಮೂದು_free(dns);
      dns = ಖಾಲಿ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
  ಆದರೆ(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ಖಾಲಿ;
  }
  ಆದರೆ(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ dns;
}

ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         uint8_t dns_queries,
                                         ರಚನೆ Curl_addrinfo **paddr,
                                         ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ dnsc_id id;
  ರಚನೆ Curl_dns_ನಮೂದು *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ನಮೂದು_create(&id, FALSE);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_addr(ದತ್ತಾಂಶ, dns, dns_queries, paddr, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ dns;
}

ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_addr2(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          uint8_t dns_queries,
                                          ರಚನೆ Curl_addrinfo **paddr1,
                                          ರಚನೆ Curl_addrinfo **paddr2,
                                          ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ dnsc_id id;
  ರಚನೆ Curl_dns_ನಮೂದು *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ನಮೂದು_create(&id, FALSE);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_addr(ದತ್ತಾಂಶ, dns, dns_queries, paddr1, paddr2);
  ಹಿಂತಿರುಗಿ dns;
}

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ಕೂಡಿಸು_ನಮೂದು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_dnscache *dnscache,
                                             ರಚನೆ dnsc_ಕೀಲಿ *ಕೀಲಿ,
                                             ರಚನೆ Curl_dns_ನಮೂದು *ನಮೂದು)
{
  ನಮೂದು->added = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ Curl_hash_ಕೂಡಿಸು(&dnscache->entries, ಕೀಲಿ->ದತ್ತಾಂಶ, ಕೀಲಿ->len, ನಮೂದು);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_https(
  ರಚನೆ Curl_dns_ನಮೂದು *dns,
  ರಚನೆ Curl_https_rrinfo **phinfo)
{
  ಆದರೆ(!dns)
    ಹೋಗು ಹೊರಹೋಗು;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  ಆದರೆ(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ಖಾಲಿ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  ಆದರೆ(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ಖಾಲಿ;
  }
ಹೊರಹೋಗು:
  ಆದರೆ(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ dns;
}

ರಚನೆ Curl_dns_ನಮೂದು *Curl_dnsc_mk_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_https_rrinfo **phinfo,
                                          ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ dnsc_id id;
  ರಚನೆ Curl_dns_ನಮೂದು *dns;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_ನಮೂದು_create(&id, FALSE);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_https(dns, phinfo);
  ಹಿಂತಿರುಗಿ dns;
}

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ಕೂಡಿಸು_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_dnscache *dnscache,
                                             ರಚನೆ Curl_https_rrinfo **phinfo,
                                             ರಚನೆ dnsc_id *id)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns, *dns2;
  ರಚನೆ dnsc_ಕೀಲಿ ಕೀಲಿ;

  dns = dnsc_ನಮೂದು_create(id, FALSE);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_https(dns, phinfo);
  ಆದರೆ(!dns)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ಕೀಲಿ, id);
  dns2 = dnsc_ಕೂಡಿಸು_ನಮೂದು(ದತ್ತಾಂಶ, dnscache, &ಕೀಲಿ, dns);
  ಆದರೆ(!dns2) {
    dnscache_ನಮೂದು_free(dns);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಹಿಂತಿರುಗಿ dns;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ಕೂಡಿಸು_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ರಚನೆ Curl_addrinfo **paddr,
                                            ರಚನೆ dnsc_id *id,
                                            ರಚನೆ dnsc_ಕೀಲಿ *ಕೀಲಿ,
                                            bool permanent)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns;
  ರಚನೆ Curl_dns_ನಮೂದು *dns2;

  dns = dnsc_ನಮೂದು_create(id, permanent);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_addr(ದತ್ತಾಂಶ, dns, dns_queries, paddr, ಖಾಲಿ);
  ಆದರೆ(!dns)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_ಕೂಡಿಸು_ನಮೂದು(ದತ್ತಾಂಶ, dnscache, ಕೀಲಿ, dns);
  ಆದರೆ(!dns2) {
    dnscache_ನಮೂದು_free(dns);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಹಿಂತಿರುಗಿ dns;
}

ಸ್ಥಿರ ರಚನೆ Curl_dns_ನಮೂದು *dnsc_ಕೂಡಿಸು_peer_addr(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ರಚನೆ Curl_addrinfo **paddr,
  ರಚನೆ dnsc_id *id,
  bool permanent)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns;
  ರಚನೆ Curl_dns_ನಮೂದು *dns2;
  ರಚನೆ dnsc_ಕೀಲಿ ಕೀಲಿ;

  dns = dnsc_ನಮೂದು_create(id, permanent);
  dns = dnsc_ನಮೂದು_ನಿಯೋಜಿಸು_addr(ದತ್ತಾಂಶ, dns, dns_queries, paddr, ಖಾಲಿ);
  ಆದರೆ(!dns)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ಕೀಲಿ, id);
  dns2 = dnsc_ಕೂಡಿಸು_ನಮೂದು(ದತ್ತಾಂಶ, dnscache, &ಕೀಲಿ, dns);
  ಆದರೆ(!dns2) {
    dnscache_ನಮೂದು_free(dns);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಹಿಂತಿರುಗಿ dns;
}

CURLcode Curl_dnscache_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_dns_ನಮೂದು *ನಮೂದು)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_str name;
  ರಚನೆ dnsc_id id;
  ರಚನೆ dnsc_ಕೀಲಿ ಕೀಲಿ;

  ಆದರೆ(!dnscache)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!ನಮೂದು || (ನಮೂದು->type == CURL_DNST_INIT))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  curlx_str_ನಿಯೋಜಿಸು(&name, ನಮೂದು->hostname, ನಮೂದು->hostlen);
  dnsc_str2id(&id, ನಮೂದು->type, &name, ನಮೂದು->ಪೋರ್ಟ್);
  dnsc_id2key(&ಕೀಲಿ, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);
  ಆದರೆ(!dnsc_ಕೂಡಿಸು_ನಮೂದು(ದತ್ತಾಂಶ, dnscache, &ಕೀಲಿ, ನಮೂದು)) {
    dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ನಮೂದು->refcount++;
  dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
  CURL_TRC_DNS(ದತ್ತಾಂಶ, "cached entry for %s:%u queries=%s",
               ನಮೂದು->hostname, ನಮೂದು->ಪೋರ್ಟ್,
               Curl_resolv_query_str(ನಮೂದು->dns_queries));
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_dnscache_ಕೂಡಿಸು_negative(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t dns_queries,
                                    ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  ರಚನೆ dnsc_id id;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(dnscache);
  ಆದರೆ(!dnscache)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);

  ಆದರೆ(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_ಕೂಡಿಸು_peer_addr(ದತ್ತಾಂಶ, dnscache, dns_queries, ಖಾಲಿ, &id, FALSE);
    ಆದರೆ(!dns)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_ಕೂಡಿಸು_https(ದತ್ತಾಂಶ, dnscache, ಖಾಲಿ, &id);
    ಆದರೆ(!dns)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಇಲ್ಲದಿದ್ದರೆ {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  ಆದರೆ(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->ಪೋರ್ಟ್,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
ಶೂನ್ಯ Curl_dns_ನಮೂದು_unlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ಆದರೆ(*pdns) {
    ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
    ರಚನೆ Curl_dns_ನಮೂದು *dns = *pdns;
    *pdns = ಖಾಲಿ;
    dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);
    dns->refcount--;
    ಆದರೆ(dns->refcount == 0)
      dnscache_ನಮೂದು_free(dns);
    dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_ನಮೂದು_dtor(ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ Curl_dns_ನಮೂದು *dns = (ರಚನೆ Curl_dns_ನಮೂದು *)ನಮೂದು;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  ಆದರೆ(dns->refcount == 0)
    dnscache_ನಮೂದು_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
ಶೂನ್ಯ Curl_dnscache_init(ರಚನೆ Curl_dnscache *dns, size_t ಗಾತ್ರ_2)
{
  Curl_hash_init(&dns->entries, ಗಾತ್ರ_2, CURL_HASH_TYPE_ಬೈಟುಗಳು,
                 dnscache_ನಮೂದು_dtor);
}

ಶೂನ್ಯ Curl_dnscache_destroy(ರಚನೆ Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_dnscache *dnscache = dnscache_get(ದತ್ತಾಂಶ);
  ರಚನೆ curl_slist *hostp;
  ರಚನೆ dnsc_id id;
  ರಚನೆ dnsc_ಕೀಲಿ ಕೀಲಿ;

  ಆದರೆ(!dnscache)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  ದತ್ತಾಂಶ->state.wildcard_resolve = FALSE;

  ಗಾಗಿ(hostp = ದತ್ತಾಂಶ->state.resolve; hostp; hostp = hostp->ಮುಂದಿನ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host = hostp->ದತ್ತಾಂಶ;
    ರಚನೆ Curl_str ಮೂಲ;
    ಆದರೆ(!host)
      ಮುಂದುವರಿ;
    ಆದರೆ(*host == '-') {
      curl_off_t ಅಂಕೆ = 0;
      host++;
      ಆದರೆ(!curlx_str_single(&host, '[')) {
        ಆದರೆ(curlx_str_until(&host, &ಮೂಲ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(curlx_str_until(&host, &ಮೂಲ, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ಮುಂದುವರಿ;
        }
      }

      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&host, &ಅಂಕೆ, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &ಮೂಲ, (uint16_t)ಅಂಕೆ);
        dnsc_id2key(&ಕೀಲಿ, &id);
        dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);
        dnscache_unlock(ದತ್ತಾಂಶ, dnscache);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ರಚನೆ Curl_dns_ನಮೂದು *dns;
      ರಚನೆ Curl_addrinfo *ತಲೆ = ಖಾಲಿ, *ಬಾಲ = ಖಾಲಿ;
      ಅಕ್ಷರ ವಿಳಾಸ[64];
      curl_off_t tmpofft = 0;
      uint16_t ಪೋರ್ಟ್ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಳಾಸಗಳು = ಖಾಲಿ);

      ಆದರೆ(*host == '+') {
        host++;
        permanent = FALSE;
      }
      ಆದರೆ(!curlx_str_single(&host, '[')) {
        ಆದರೆ(curlx_str_until(&host, &ಮೂಲ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(curlx_str_until(&host, &ಮೂಲ, 4096, ':'))
          ಮುಂದುವರಿ;
      }
      ಆದರೆ(curlx_str_single(&host, ':') ||
         curlx_str_ಸಂಖ್ಯೆ(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        ಹೋಗು err;
      ಪೋರ್ಟ್ = (uint16_t)tmpofft;

      VERBOSE(ವಿಳಾಸಗಳು = host);

      /* start the address section */
      ಆಗ(*host) {
        ರಚನೆ Curl_str ಗುರಿ;
        ರಚನೆ Curl_addrinfo *ai;
        CURLcode ಫಲಿತಾಂಶ;

        ಆದರೆ(!curlx_str_single(&host, '[')) {
          ಆದರೆ(curlx_str_until(&host, &ಗುರಿ, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            ಹೋಗು err;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಆದರೆ(curlx_str_until(&host, &ಗುರಿ, 4096, ',')) {
            ಆದರೆ(curlx_str_single(&host, ','))
              ಹೋಗು err;
            /* survive nothing but a comma */
            ಮುಂದುವರಿ;
          }
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IPV6
        ಆದರೆ(memchr(curlx_str(&ಗುರಿ), ':', curlx_strlen(&ಗುರಿ))) {
          infof(ದತ್ತಾಂಶ, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (ಪೂರ್ಣಾಂಕ)curlx_strlen(&ಗುರಿ), curlx_str(&ಗುರಿ));
          ಆದರೆ(curlx_str_single(&host, ','))
            ಹೋಗು err;
          ಮುಂದುವರಿ;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ(curlx_strlen(&ಗುರಿ) >= ಗಾತ್ರ(ವಿಳಾಸ))
          ಹೋಗು err;

        ಸ್ಮರಣೆ_ನಕಲು(ವಿಳಾಸ, curlx_str(&ಗುರಿ), curlx_strlen(&ಗುರಿ));
        ವಿಳಾಸ[curlx_strlen(&ಗುರಿ)] = '\0';

        ಫಲಿತಾಂಶ = Curl_str2addr(ವಿಳಾಸ, ಪೋರ್ಟ್, &ai);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          infof(ದತ್ತಾಂಶ, "Resolve IP address '%s' found is illegal", ವಿಳಾಸ);
          ಹೋಗು err;
        }

        ಆದರೆ(ಬಾಲ) {
          ಬಾಲ->ai_ಮುಂದಿನ = ai;
          ಬಾಲ = ಬಾಲ->ai_ಮುಂದಿನ;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ತಲೆ = ಬಾಲ = ai;
        }
        ಆದರೆ(curlx_str_single(&host, ','))
          ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ(!ತಲೆ)
        ಹೋಗು err;

      error = FALSE;
err:
      ಆದರೆ(error) {
        failf(ದತ್ತಾಂಶ, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->ದತ್ತಾಂಶ);
        Curl_freeaddrinfo(ತಲೆ);
        ಹಿಂತಿರುಗಿ CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &ಮೂಲ, ಪೋರ್ಟ್);
      dnsc_id2key(&ಕೀಲಿ, &id);
      dnscache_ಬೀಗ(ದತ್ತಾಂಶ, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);

      ಆದರೆ(dns) {
        infof(ದತ್ತಾಂಶ, "RESOLVE %.*s:%u - old addresses discarded",
              (ಪೂರ್ಣಾಂಕ)curlx_strlen(&ಮೂಲ), curlx_str(&ಮೂಲ), ಪೋರ್ಟ್);
        /* delete old entry, there are two reasons for this
         1. old entry may have different addresses.
         2. even if entry with correct addresses is already in the cache,
            but if it is close to expire, then by the time next http
            request is made, it can get expired and pruned because old
            entry is not necessarily marked as permanent.
         3. when adding a non-permanent entry, we want it to remove and
            replace an existing permanent entry.
         4. when adding a non-permanent entry, we want it to get a "fresh"
            timeout that starts _now_. */

        Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.ದತ್ತಾಂಶ, ಕೀಲಿ.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_ಕೂಡಿಸು_addr(ದತ್ತಾಂಶ, dnscache, CURL_DNSQ_ADDR, &ತಲೆ,
                          &id, &ಕೀಲಿ, permanent);
      ಆದರೆ(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(ದತ್ತಾಂಶ, dnscache);

      ಆದರೆ(!dns)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      infof(ದತ್ತಾಂಶ, "[DNS] added %.*s:%u:%s to cache%s",
            (ಪೂರ್ಣಾಂಕ)curlx_strlen(&id.name), curlx_str(&id.name), id.ಪೋರ್ಟ್,
            ವಿಳಾಸಗಳು, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      ಆದರೆ(curlx_str_casecompare(&ಮೂಲ, "*")) {
        infof(ದತ್ತಾಂಶ, "RESOLVE *:%u using wildcard", ಪೋರ್ಟ್);
        ದತ್ತಾಂಶ->state.wildcard_resolve = TRUE;
      }
    }
  }
  ದತ್ತಾಂಶ->state.resolve = ಖಾಲಿ; /* dealt with now */

  ಹಿಂತಿರುಗಿ CURLE_OK;
}
