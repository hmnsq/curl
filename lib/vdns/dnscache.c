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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IN6_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in6.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/dnscache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DNS_CACHE_ਮਾਪ 29999

/* @unittest 3217 */
UNITTEST ਖਾਲੀ dnsc_peer2id(ਬਣਤਰ dnsc_id *pid, ਅੱਖਰ type,
                           ਬਣਤਰ Curl_peer *peer);
UNITTEST ਖਾਲੀ dnsc_peer2id(ਬਣਤਰ dnsc_id *pid, ਅੱਖਰ type,
                           ਬਣਤਰ Curl_peer *peer)
{
  curlx_str_ਸੌਂਪੋ(&pid->name, peer->hostname, ਲੜੀ_ਲੰਬਾਈ(peer->hostname));
  pid->ਪੋਰਟ = peer->ਪੋਰਟ;
  pid->type = type;
}

ਸਥਿਰ ਖਾਲੀ dnsc_str2id(ਬਣਤਰ dnsc_id *pid, ਅੱਖਰ type,
                        ਬਣਤਰ Curl_str *name, uint16_t ਪੋਰਟ)
{
  pid->name = *name;
  pid->ਪੋਰਟ = ਪੋਰਟ;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST ਖਾਲੀ dnsc_id2key(ਬਣਤਰ dnsc_ਕੁੰਜੀ *ਕੁੰਜੀ, ਬਣਤਰ dnsc_id *id);
UNITTEST ਖਾਲੀ dnsc_id2key(ਬਣਤਰ dnsc_ਕੁੰਜੀ *ਕੁੰਜੀ, ਬਣਤਰ dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  ਜੇ(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ਕੁੰਜੀ->ਅੰਕੜੇ[0] = id->type;
  ਕੁੰਜੀ->ਅੰਕੜੇ[1] = (uint8_t)((id->ਪੋਰਟ >> 8) & 0xff);
  ਕੁੰਜੀ->ਅੰਕੜੇ[2] = (uint8_t)(id->ਪੋਰਟ & 0xff);
  Curl_strntolower((ਅੱਖਰ *)ਕੁੰਜੀ->ਅੰਕੜੇ + 3, curlx_str(&id->name), namelen);
  ਕੁੰਜੀ->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_ਇੰਦਰਾਜ(ਬਣਤਰ dnsc_id *id,
                                    ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns);
UNITTEST bool dnsc_id_matches_ਇੰਦਰਾਜ(ਬਣਤਰ dnsc_id *id,
                                    ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns)
{
  ਵਾਪਸ dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->ਪੋਰਟ == dns->ਪੋਰਟ) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ਸਥਿਰ ਖਾਲੀ dnscache_ਇੰਦਰਾਜ_free(ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#ਜੇ_ਅੰਤ
  curlx_free(dns);
}

ਬਣਤਰ dnscache_prune_ਅੰਕੜੇ {
  ਅਚਲ ਬਣਤਰ curltime *pnow;
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
ਸਥਿਰ ਪੂਰਨ dnscache_ਇੰਦਰਾਜ_is_stale(ਖਾਲੀ *datap, ਖਾਲੀ *hc)
{
  ਬਣਤਰ dnscache_prune_ਅੰਕੜੇ *prune = (ਬਣਤਰ dnscache_prune_ਅੰਕੜੇ *)datap;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = (ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *)hc;

  ਜੇ(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ਚੋਣ(dns->type) {
    ਹਾਲਤ CURL_DNST_ADDR:
      ਜੇ(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
    ਹਾਲਤ CURL_DNST_HTTPS:
      ਜੇ(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
      ਰੋਕੋ;
    }
    ਜੇ(age_ms >= prune->max_age_ms)
      ਵਾਪਸ TRUE;
    ਜੇ(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ਵਾਪਸ FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ਸਥਿਰ timediff_t dnscache_prune(ਬਣਤਰ Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ dnscache_prune_ਅੰਕੜੇ user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (ਖਾਲੀ *)&user,
                                 dnscache_ਇੰਦਰਾਜ_is_stale);

  ਵਾਪਸ user.oldest_ms;
}

ਸਥਿਰ ਬਣਤਰ Curl_dnscache *dnscache_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->share && ਅੰਕੜੇ->share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_DNS))
    ਵਾਪਸ &ਅੰਕੜੇ->share->dnscache;
  ਜੇ(ਅੰਕੜੇ->multi)
    ਵਾਪਸ &ਅੰਕੜੇ->multi->dnscache;
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ dnscache_ਤਾਲਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_dnscache *dnscache)
{
  ਜੇ(ਅੰਕੜੇ->share && dnscache == &ਅੰਕੜੇ->share->dnscache)
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_DNS, CURL_ਤਾਲਾ_ACCESS_SINGLE);
}

ਸਥਿਰ ਖਾਲੀ dnscache_unlock(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_dnscache *dnscache)
{
  ਜੇ(ਅੰਕੜੇ->share && dnscache == &ਅੰਕੜੇ->share->dnscache)
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
ਖਾਲੀ Curl_dnscache_prune(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.dns_cache_timeout_ms;

  ਜੇ(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ਵਾਪਸ;

  dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);

  ਕਰੋ {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    ਜੇ(Curl_hash_ਗਿਣਤੀ(&dnscache->entries) > MAX_DNS_CACHE_ਮਾਪ)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;

    /* if the cache size is still too big, use the oldest age as new prune
ਹੱਦਬੰਦੀ*/
  } ਜਦੋਂ(timeout_ms);

  dnscache_unlock(ਅੰਕੜੇ, dnscache);
}

ਖਾਲੀ Curl_dnscache_clear(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  ਜੇ(dnscache) {
    dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(ਅੰਕੜੇ, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ਸਥਿਰ CURLcode fetch_ਇੰਦਰਾਜ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ਬਣਤਰ Curl_peer *peer,
                            ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ dnsc_ਕੁੰਜੀ ਕੁੰਜੀ;
  ਅੱਖਰ type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pdns = ਨਿਰਾ;
  ਜੇ(!dnscache)
    ਵਾਪਸ CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ਕੁੰਜੀ, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);

  ਜੇ(dns && !dnsc_id_matches_ਇੰਦਰਾਜ(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ਨਿਰਾ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  ਜੇ(!dns && (type == CURL_DNST_ADDR) && ਅੰਕੜੇ->state.wildcard_resolve) {
    ਬਣਤਰ Curl_str wildname;

    curlx_str_ਸੌਂਪੋ(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->ਪੋਰਟ);
    dnsc_id2key(&ਕੁੰਜੀ, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);
  }

  ਜੇ(dns && (ਅੰਕੜੇ->ਸੈੱਟ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ਬਣਤਰ dnscache_prune_ਅੰਕੜੇ user;

    user.pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    user.max_age_ms = ਅੰਕੜੇ->ਸੈੱਟ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    ਜੇ(dnscache_ਇੰਦਰਾਜ_is_stale(&user, dns)) {
      infof(ਅੰਕੜੇ, "Hostname in DNS cache was stale, zapped");
      dns = ਨਿਰਾ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  ਜੇ(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    ਜੇ((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(ਅੰਕੜੇ, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ਨਿਰਾ;
      ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_HOST;
    }
    ਨਹੀਂਤਾਂ ਜੇ(dns && !dns->addr) { /* negative entry */
      dns = ਨਿਰਾ;
      ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_dnscache_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           uint8_t dns_queries,
                           ਬਣਤਰ Curl_peer *peer,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pentry)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);
  ਨਤੀਜਾ = fetch_ਇੰਦਰਾਜ(ਅੰਕੜੇ, dnscache, dns_queries, peer, &dns);
  ਜੇ(!ਨਤੀਜਾ && dns)
    dns->refcount++; /* we pass out a reference */
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    DEBUGASSERT(!dns);
    dns = ਨਿਰਾ;
  }
  dnscache_unlock(ਅੰਕੜੇ, dnscache);

  CURL_TRC_DNS(ਅੰਕੜੇ, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->ਪੋਰਟ, Curl_resolv_query_str(dns_queries),
               (ਪੂਰਨ)ਨਤੀਜਾ, dns ? "" : "not ");
  *pentry = dns;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ਸਥਿਰ ਪੂਰਨ ਅੰਕ_ਪਤੇ(ਅਚਲ ਬਣਤਰ Curl_addrinfo *addr)
{
  ਪੂਰਨ i = 0;
  ਜਦੋਂ(addr) {
    addr = addr->ai_ਅਗਲਾ;
    i++;
  }
  ਵਾਪਸ i;
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
UNITTEST CURLcode dns_shuffle_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_addrinfo **addr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਪੂਰਨ ਅੰਕ_addrs = ਅੰਕ_ਪਤੇ(*addr);

  ਜੇ(ਅੰਕ_addrs > 1) {
    ਬਣਤਰ Curl_addrinfo **nodes;
    CURL_TRC_DNS(ਅੰਕੜੇ, "Shuffling %d addresses", ਅੰਕ_addrs);

    nodes = curlx_malloc(ਅੰਕ_addrs * ਆਕਾਰ(*nodes));
    ਜੇ(nodes) {
      ਪੂਰਨ i;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *rnd;
      ਅਚਲ size_t rnd_ਮਾਪ = ਅੰਕ_addrs * ਆਕਾਰ(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      ਲਈ(i = 1; i < ਅੰਕ_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_ਅਗਲਾ;
      }

      rnd = curlx_malloc(rnd_ਮਾਪ);
      ਜੇ(rnd) {
        /* Fisher-Yates shuffle */
        ਜੇ(Curl_rand(ਅੰਕੜੇ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)rnd, rnd_ਮਾਪ) == CURLE_OK) {
          ਬਣਤਰ Curl_addrinfo *ਅਦਲਾ_ਬਦਲੀ_tmp;
          ਲਈ(i = ਅੰਕ_addrs - 1; i > 0; i--) {
            ਅਦਲਾ_ਬਦਲੀ_tmp = nodes[rnd[i] % (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(i + 1)];
            nodes[rnd[i] % (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(i + 1)] = nodes[i];
            nodes[i] = ਅਦਲਾ_ਬਦਲੀ_tmp;
          }

          /* relink list in the new order */
          ਲਈ(i = 1; i < ਅੰਕ_addrs; i++) {
            nodes[i - 1]->ai_ਅਗਲਾ = nodes[i];
          }

          nodes[ਅੰਕ_addrs - 1]->ai_ਅਗਲਾ = ਨਿਰਾ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      curlx_free(nodes);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool dnscache_ai_has_family(ਬਣਤਰ Curl_addrinfo *ai, ਪੂਰਨ ai_family)
{
  ਲਈ(; ai; ai = ai->ai_ਅਗਲਾ) {
    ਜੇ(ai->ai_family == ai_family)
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਇੰਦਰਾਜ_create(ਬਣਤਰ dnsc_id *pid,
                                                bool permanent)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;

  ਜੇ(curlx_strlen(&pid->name) > UINT16_MAX)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ) +
                     curlx_strlen(&pid->name));
  ਜੇ(!dns)
    ਜਾਓ ਬਾਹਰਲਾ;

  dns->refcount = 1; /* the cache has the first reference */
  dns->ਪੋਰਟ = pid->ਪੋਰਟ;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  ਜੇ(dns->hostlen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

ਬਾਹਰਲਾ:
  ਵਾਪਸ dns;
}

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_addr(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns,
  uint8_t dns_queries,
  ਬਣਤਰ Curl_addrinfo **paddr1,
  ਬਣਤਰ Curl_addrinfo **paddr2)
{
  ਜੇ(!dns)
    ਜਾਓ ਬਾਹਰਲਾ;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  ਜੇ(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ਨਿਰਾ;
  }
  ਜੇ(paddr2 && *paddr2) {
    ਬਣਤਰ Curl_addrinfo **phead = &dns->addr;
    ਜਦੋਂ(*phead)
      phead = &(*phead)->ai_ਅਗਲਾ;
    *phead = *paddr2;
    *paddr2 = ਨਿਰਾ;
  }

  ਜੇ((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#ਜੇ_ਅੰਤ /* USE_IPV6 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.dns_shuffle_ਪਤੇ && dns->addr) {
    CURLcode ਨਤੀਜਾ = dns_shuffle_addr(ਅੰਕੜੇ, &dns->addr);
    ਜੇ(ਨਤੀਜਾ) {
      /* free without lock, we are the sole owner */
      dnscache_ਇੰਦਰਾਜ_free(dns);
      dns = ਨਿਰਾ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
  ਜੇ(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ਨਿਰਾ;
  }
  ਜੇ(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ਨਿਰਾ;
  }
  ਵਾਪਸ dns;
}

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         uint8_t dns_queries,
                                         ਬਣਤਰ Curl_addrinfo **paddr,
                                         ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ਇੰਦਰਾਜ_create(&id, FALSE);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_addr(ਅੰਕੜੇ, dns, dns_queries, paddr, ਨਿਰਾ);
  ਵਾਪਸ dns;
}

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_addr2(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          uint8_t dns_queries,
                                          ਬਣਤਰ Curl_addrinfo **paddr1,
                                          ਬਣਤਰ Curl_addrinfo **paddr2,
                                          ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ਇੰਦਰਾਜ_create(&id, FALSE);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_addr(ਅੰਕੜੇ, dns, dns_queries, paddr1, paddr2);
  ਵਾਪਸ dns;
}

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਜੋੜੋ_ਇੰਦਰਾਜ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_dnscache *dnscache,
                                             ਬਣਤਰ dnsc_ਕੁੰਜੀ *ਕੁੰਜੀ,
                                             ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ)
{
  ਇੰਦਰਾਜ->added = *Curl_pgrs_now(ਅੰਕੜੇ);
  ਵਾਪਸ Curl_hash_ਜੋੜੋ(&dnscache->entries, ਕੁੰਜੀ->ਅੰਕੜੇ, ਕੁੰਜੀ->len, ਇੰਦਰਾਜ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_https(
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns,
  ਬਣਤਰ Curl_https_rrinfo **phinfo)
{
  ਜੇ(!dns)
    ਜਾਓ ਬਾਹਰਲਾ;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  ਜੇ(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ਨਿਰਾ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  ਜੇ(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ਨਿਰਾ;
  }
ਬਾਹਰਲਾ:
  ਜੇ(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ਨਿਰਾ;
  }
  ਵਾਪਸ dns;
}

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *Curl_dnsc_mk_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_https_rrinfo **phinfo,
                                          ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_ਇੰਦਰਾਜ_create(&id, FALSE);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_https(dns, phinfo);
  ਵਾਪਸ dns;
}

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਜੋੜੋ_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_dnscache *dnscache,
                                             ਬਣਤਰ Curl_https_rrinfo **phinfo,
                                             ਬਣਤਰ dnsc_id *id)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns, *dns2;
  ਬਣਤਰ dnsc_ਕੁੰਜੀ ਕੁੰਜੀ;

  dns = dnsc_ਇੰਦਰਾਜ_create(id, FALSE);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_https(dns, phinfo);
  ਜੇ(!dns)
    ਵਾਪਸ ਨਿਰਾ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ਕੁੰਜੀ, id);
  dns2 = dnsc_ਜੋੜੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, dnscache, &ਕੁੰਜੀ, dns);
  ਜੇ(!dns2) {
    dnscache_ਇੰਦਰਾਜ_free(dns);
    ਵਾਪਸ ਨਿਰਾ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ਵਾਪਸ dns;
}
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਜੋੜੋ_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ਬਣਤਰ Curl_addrinfo **paddr,
                                            ਬਣਤਰ dnsc_id *id,
                                            ਬਣਤਰ dnsc_ਕੁੰਜੀ *ਕੁੰਜੀ,
                                            bool permanent)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns2;

  dns = dnsc_ਇੰਦਰਾਜ_create(id, permanent);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_addr(ਅੰਕੜੇ, dns, dns_queries, paddr, ਨਿਰਾ);
  ਜੇ(!dns)
    ਵਾਪਸ ਨਿਰਾ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_ਜੋੜੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, dnscache, ਕੁੰਜੀ, dns);
  ਜੇ(!dns2) {
    dnscache_ਇੰਦਰਾਜ_free(dns);
    ਵਾਪਸ ਨਿਰਾ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ਵਾਪਸ dns;
}

ਸਥਿਰ ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dnsc_ਜੋੜੋ_peer_addr(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ਬਣਤਰ Curl_addrinfo **paddr,
  ਬਣਤਰ dnsc_id *id,
  bool permanent)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns2;
  ਬਣਤਰ dnsc_ਕੁੰਜੀ ਕੁੰਜੀ;

  dns = dnsc_ਇੰਦਰਾਜ_create(id, permanent);
  dns = dnsc_ਇੰਦਰਾਜ_ਸੌਂਪੋ_addr(ਅੰਕੜੇ, dns, dns_queries, paddr, ਨਿਰਾ);
  ਜੇ(!dns)
    ਵਾਪਸ ਨਿਰਾ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ਕੁੰਜੀ, id);
  dns2 = dnsc_ਜੋੜੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, dnscache, &ਕੁੰਜੀ, dns);
  ਜੇ(!dns2) {
    dnscache_ਇੰਦਰਾਜ_free(dns);
    ਵਾਪਸ ਨਿਰਾ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ਵਾਪਸ dns;
}

CURLcode Curl_dnscache_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_str name;
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ dnsc_ਕੁੰਜੀ ਕੁੰਜੀ;

  ਜੇ(!dnscache)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!ਇੰਦਰਾਜ || (ਇੰਦਰਾਜ->type == CURL_DNST_INIT))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  curlx_str_ਸੌਂਪੋ(&name, ਇੰਦਰਾਜ->hostname, ਇੰਦਰਾਜ->hostlen);
  dnsc_str2id(&id, ਇੰਦਰਾਜ->type, &name, ਇੰਦਰਾਜ->ਪੋਰਟ);
  dnsc_id2key(&ਕੁੰਜੀ, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);
  ਜੇ(!dnsc_ਜੋੜੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, dnscache, &ਕੁੰਜੀ, ਇੰਦਰਾਜ)) {
    dnscache_unlock(ਅੰਕੜੇ, dnscache);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਇੰਦਰਾਜ->refcount++;
  dnscache_unlock(ਅੰਕੜੇ, dnscache);
  CURL_TRC_DNS(ਅੰਕੜੇ, "cached entry for %s:%u queries=%s",
               ਇੰਦਰਾਜ->hostname, ਇੰਦਰਾਜ->ਪੋਰਟ,
               Curl_resolv_query_str(ਇੰਦਰਾਜ->dns_queries));
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_dnscache_ਜੋੜੋ_negative(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t dns_queries,
                                    ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  ਬਣਤਰ dnsc_id id;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(dnscache);
  ਜੇ(!dnscache)
    ਵਾਪਸ CURLE_FAILED_INIT;

  dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);

  ਜੇ(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_ਜੋੜੋ_peer_addr(ਅੰਕੜੇ, dnscache, dns_queries, ਨਿਰਾ, &id, FALSE);
    ਜੇ(!dns)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਨਹੀਂਤਾਂ ਜੇ(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_ਜੋੜੋ_https(ਅੰਕੜੇ, dnscache, ਨਿਰਾ, &id);
    ਜੇ(!dns)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਅੰਤ
  ਨਹੀਂਤਾਂ {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  ਜੇ(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(ਅੰਕੜੇ, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->ਪੋਰਟ,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(ਅੰਕੜੇ, dnscache);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
ਖਾਲੀ Curl_dns_ਇੰਦਰਾਜ_unlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਜੇ(*pdns) {
    ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
    ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = *pdns;
    *pdns = ਨਿਰਾ;
    dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);
    dns->refcount--;
    ਜੇ(dns->refcount == 0)
      dnscache_ਇੰਦਰਾਜ_free(dns);
    dnscache_unlock(ਅੰਕੜੇ, dnscache);
  }
}

ਸਥਿਰ ਖਾਲੀ dnscache_ਇੰਦਰਾਜ_dtor(ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = (ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *)ਇੰਦਰਾਜ;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  ਜੇ(dns->refcount == 0)
    dnscache_ਇੰਦਰਾਜ_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
ਖਾਲੀ Curl_dnscache_init(ਬਣਤਰ Curl_dnscache *dns, size_t ਮਾਪ)
{
  Curl_hash_init(&dns->entries, ਮਾਪ, CURL_HASH_TYPE_ਬਾਈਟਾਂ,
                 dnscache_ਇੰਦਰਾਜ_dtor);
}

ਖਾਲੀ Curl_dnscache_destroy(ਬਣਤਰ Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_dnscache *dnscache = dnscache_get(ਅੰਕੜੇ);
  ਬਣਤਰ curl_slist *hostp;
  ਬਣਤਰ dnsc_id id;
  ਬਣਤਰ dnsc_ਕੁੰਜੀ ਕੁੰਜੀ;

  ਜੇ(!dnscache)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  ਅੰਕੜੇ->state.wildcard_resolve = FALSE;

  ਲਈ(hostp = ਅੰਕੜੇ->state.resolve; hostp; hostp = hostp->ਅਗਲਾ) {
    ਅਚਲ ਅੱਖਰ *host = hostp->ਅੰਕੜੇ;
    ਬਣਤਰ Curl_str ਸਰੋਤ;
    ਜੇ(!host)
      ਜਾਰੀ;
    ਜੇ(*host == '-') {
      curl_off_t ਅੰਕ = 0;
      host++;
      ਜੇ(!curlx_str_single(&host, '[')) {
        ਜੇ(curlx_str_until(&host, &ਸਰੋਤ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(curlx_str_until(&host, &ਸਰੋਤ, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ਜਾਰੀ;
        }
      }

      ਜੇ(!curlx_str_ਸੰਖਿਆ(&host, &ਅੰਕ, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &ਸਰੋਤ, (uint16_t)ਅੰਕ);
        dnsc_id2key(&ਕੁੰਜੀ, &id);
        dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);
        dnscache_unlock(ਅੰਕੜੇ, dnscache);
      }
    }
    ਨਹੀਂਤਾਂ {
      ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;
      ਬਣਤਰ Curl_addrinfo *ਸਿਰ = ਨਿਰਾ, *ਪੂਛ = ਨਿਰਾ;
      ਅੱਖਰ ਪਤਾ[64];
      curl_off_t tmpofft = 0;
      uint16_t ਪੋਰਟ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ਅਚਲ ਅੱਖਰ *ਪਤੇ = ਨਿਰਾ);

      ਜੇ(*host == '+') {
        host++;
        permanent = FALSE;
      }
      ਜੇ(!curlx_str_single(&host, '[')) {
        ਜੇ(curlx_str_until(&host, &ਸਰੋਤ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(curlx_str_until(&host, &ਸਰੋਤ, 4096, ':'))
          ਜਾਰੀ;
      }
      ਜੇ(curlx_str_single(&host, ':') ||
         curlx_str_ਸੰਖਿਆ(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        ਜਾਓ err;
      ਪੋਰਟ = (uint16_t)tmpofft;

      VERBOSE(ਪਤੇ = host);

      /* start the address section */
      ਜਦੋਂ(*host) {
        ਬਣਤਰ Curl_str ਨਿਸ਼ਾਨਾ;
        ਬਣਤਰ Curl_addrinfo *ai;
        CURLcode ਨਤੀਜਾ;

        ਜੇ(!curlx_str_single(&host, '[')) {
          ਜੇ(curlx_str_until(&host, &ਨਿਸ਼ਾਨਾ, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            ਜਾਓ err;
        }
        ਨਹੀਂਤਾਂ {
          ਜੇ(curlx_str_until(&host, &ਨਿਸ਼ਾਨਾ, 4096, ',')) {
            ਜੇ(curlx_str_single(&host, ','))
              ਜਾਓ err;
            /* survive nothing but a comma */
            ਜਾਰੀ;
          }
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IPV6
        ਜੇ(memchr(curlx_str(&ਨਿਸ਼ਾਨਾ), ':', curlx_strlen(&ਨਿਸ਼ਾਨਾ))) {
          infof(ਅੰਕੜੇ, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (ਪੂਰਨ)curlx_strlen(&ਨਿਸ਼ਾਨਾ), curlx_str(&ਨਿਸ਼ਾਨਾ));
          ਜੇ(curlx_str_single(&host, ','))
            ਜਾਓ err;
          ਜਾਰੀ;
        }
#ਜੇ_ਅੰਤ

        ਜੇ(curlx_strlen(&ਨਿਸ਼ਾਨਾ) >= ਆਕਾਰ(ਪਤਾ))
          ਜਾਓ err;

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਪਤਾ, curlx_str(&ਨਿਸ਼ਾਨਾ), curlx_strlen(&ਨਿਸ਼ਾਨਾ));
        ਪਤਾ[curlx_strlen(&ਨਿਸ਼ਾਨਾ)] = '\0';

        ਨਤੀਜਾ = Curl_str2addr(ਪਤਾ, ਪੋਰਟ, &ai);
        ਜੇ(ਨਤੀਜਾ) {
          infof(ਅੰਕੜੇ, "Resolve IP address '%s' found is illegal", ਪਤਾ);
          ਜਾਓ err;
        }

        ਜੇ(ਪੂਛ) {
          ਪੂਛ->ai_ਅਗਲਾ = ai;
          ਪੂਛ = ਪੂਛ->ai_ਅਗਲਾ;
        }
        ਨਹੀਂਤਾਂ {
          ਸਿਰ = ਪੂਛ = ai;
        }
        ਜੇ(curlx_str_single(&host, ','))
          ਰੋਕੋ;
      }

      ਜੇ(!ਸਿਰ)
        ਜਾਓ err;

      error = FALSE;
err:
      ਜੇ(error) {
        failf(ਅੰਕੜੇ, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->ਅੰਕੜੇ);
        Curl_freeaddrinfo(ਸਿਰ);
        ਵਾਪਸ CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &ਸਰੋਤ, ਪੋਰਟ);
      dnsc_id2key(&ਕੁੰਜੀ, &id);
      dnscache_ਤਾਲਾ(ਅੰਕੜੇ, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);

      ਜੇ(dns) {
        infof(ਅੰਕੜੇ, "RESOLVE %.*s:%u - old addresses discarded",
              (ਪੂਰਨ)curlx_strlen(&ਸਰੋਤ), curlx_str(&ਸਰੋਤ), ਪੋਰਟ);
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

        Curl_hash_delete(&dnscache->entries, ਕੁੰਜੀ.ਅੰਕੜੇ, ਕੁੰਜੀ.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_ਜੋੜੋ_addr(ਅੰਕੜੇ, dnscache, CURL_DNSQ_ADDR, &ਸਿਰ,
                          &id, &ਕੁੰਜੀ, permanent);
      ਜੇ(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(ਅੰਕੜੇ, dnscache);

      ਜੇ(!dns)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      infof(ਅੰਕੜੇ, "[DNS] added %.*s:%u:%s to cache%s",
            (ਪੂਰਨ)curlx_strlen(&id.name), curlx_str(&id.name), id.ਪੋਰਟ,
            ਪਤੇ, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      ਜੇ(curlx_str_casecompare(&ਸਰੋਤ, "*")) {
        infof(ਅੰਕੜੇ, "RESOLVE *:%u using wildcard", ਪੋਰਟ);
        ਅੰਕੜੇ->state.wildcard_resolve = TRUE;
      }
    }
  }
  ਅੰਕੜੇ->state.resolve = ਨਿਰਾ; /* dealt with now */

  ਵਾਪਸ CURLE_OK;
}
