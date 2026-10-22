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

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#அந்தர்பாவ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "vdns/dnscache.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strcopy.h"
#அந்தர்பாவ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST வெறுமை dnsc_peer2id(கட்டமைப்பு dnsc_id *pid, எழுத்து type,
                           கட்டமைப்பு Curl_peer *peer);
UNITTEST வெறுமை dnsc_peer2id(கட்டமைப்பு dnsc_id *pid, எழுத்து type,
                           கட்டமைப்பு Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

நிலையான வெறுமை dnsc_str2id(கட்டமைப்பு dnsc_id *pid, எழுத்து type,
                        கட்டமைப்பு Curl_str *name, uint16_t port)
{
  pid->name = *name;
  pid->port = port;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST வெறுமை dnsc_id2key(கட்டமைப்பு dnsc_சாவி *சாவி, கட்டமைப்பு dnsc_id *id);
UNITTEST வெறுமை dnsc_id2key(கட்டமைப்பு dnsc_சாவி *சாவி, கட்டமைப்பு dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  எனில்(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  சாவி->data[0] = id->type;
  சாவி->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  சாவி->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((எழுத்து *)சாவி->data + 3, curlx_str(&id->name), namelen);
  சாவி->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(கட்டமைப்பு dnsc_id *id,
                                    கட்டமைப்பு Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(கட்டமைப்பு dnsc_id *id,
                                    கட்டமைப்பு Curl_dns_entry *dns)
{
  திரும்பு dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

நிலையான வெறுமை dnscache_entry_free(கட்டமைப்பு Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

கட்டமைப்பு dnscache_prune_data {
  மாறிலி கட்டமைப்பு curltime *pnow;
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
நிலையான முழு dnscache_entry_is_stale(வெறுமை *datap, வெறுமை *hc)
{
  கட்டமைப்பு dnscache_prune_data *prune = (கட்டமைப்பு dnscache_prune_data *)datap;
  கட்டமைப்பு Curl_dns_entry *dns = (கட்டமைப்பு Curl_dns_entry *)hc;

  எனில்(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    தேர்வு(dns->type) {
    நிலை CURL_DNST_ADDR:
      எனில்(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      நிறுத்து;
#ifdef USE_HTTPSRR
    நிலை CURL_DNST_HTTPS:
      எனில்(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      நிறுத்து;
#endif
    இயல்பு:
      நிறுத்து;
    }
    எனில்(age_ms >= prune->max_age_ms)
      திரும்பு TRUE;
    எனில்(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  திரும்பு FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
நிலையான timediff_t dnscache_prune(கட்டமைப்பு Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (வெறுமை *)&user,
                                 dnscache_entry_is_stale);

  திரும்பு user.oldest_ms;
}

நிலையான கட்டமைப்பு Curl_dnscache *dnscache_get(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    திரும்பு &data->share->dnscache;
  எனில்(data->multi)
    திரும்பு &data->multi->dnscache;
  திரும்பு வெற்று;
}

நிலையான வெறுமை dnscache_lock(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_dnscache *dnscache)
{
  எனில்(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

நிலையான வெறுமை dnscache_unlock(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_dnscache *dnscache)
{
  எனில்(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
வெறுமை Curl_dnscache_prune(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  எனில்(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    திரும்பு;

  dnscache_lock(data, dnscache);

  செய் {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    எனில்(Curl_hash_மொத்தம்(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    இல்லையெனில்
      நிறுத்து;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } போது(timeout_ms);

  dnscache_unlock(data, dnscache);
}

வெறுமை Curl_dnscache_clear(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  எனில்(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
நிலையான CURLcode fetch_entry(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            கட்டமைப்பு Curl_peer *peer,
                            கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு dnsc_சாவி சாவி;
  எழுத்து type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = வெற்று;
  எனில்(!dnscache)
    திரும்பு CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&சாவி, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, சாவி.data, சாவி.len);

  எனில்(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = வெற்று;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  எனில்(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    கட்டமைப்பு Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&சாவி, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, சாவி.data, சாவி.len);
  }

  எனில்(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    கட்டமைப்பு dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    எனில்(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = வெற்று; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, சாவி.data, சாவி.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  எனில்(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    எனில்((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = வெற்று;
    }
    இல்லையெனில் எனில்(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = வெற்று;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    இல்லையெனில் எனில்(dns && !dns->addr) { /* negative entry */
      dns = வெற்று;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  திரும்பு result;
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
CURLcode Curl_dnscache_get(கட்டமைப்பு Curl_easy *data,
                           uint8_t dns_queries,
                           கட்டமைப்பு Curl_peer *peer,
                           கட்டமைப்பு Curl_dns_entry **pentry)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  எனில்(!result && dns)
    dns->refcount++; /* we pass out a reference */
  இல்லையெனில் எனில்(result) {
    DEBUGASSERT(!dns);
    dns = வெற்று;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (முழு)result, dns ? "" : "not ");
  *pentry = dns;
  திரும்பு result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
நிலையான முழு இலக்கம்_addresses(மாறிலி கட்டமைப்பு Curl_addrinfo *addr)
{
  முழு i = 0;
  போது(addr) {
    addr = addr->ai_next;
    i++;
  }
  திரும்பு i;
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
UNITTEST CURLcode dns_shuffle_addr(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  மாறிலி முழு இலக்கம்_addrs = இலக்கம்_addresses(*addr);

  எனில்(இலக்கம்_addrs > 1) {
    கட்டமைப்பு Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", இலக்கம்_addrs);

    nodes = curlx_malloc(இலக்கம்_addrs * அளவு(*nodes));
    எனில்(nodes) {
      முழு i;
      unsigned முழு *rnd;
      மாறிலி size_t rnd_size = இலக்கம்_addrs * அளவு(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      க்கு(i = 1; i < இலக்கம்_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      எனில்(rnd) {
        /* Fisher-Yates shuffle */
        எனில்(Curl_rand(data, (unsigned எழுத்து *)rnd, rnd_size) == CURLE_OK) {
          கட்டமைப்பு Curl_addrinfo *swap_tmp;
          க்கு(i = இலக்கம்_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned முழு)(i + 1)];
            nodes[rnd[i] % (unsigned முழு)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          க்கு(i = 1; i < இலக்கம்_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[இலக்கம்_addrs - 1]->ai_next = வெற்று;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      இல்லையெனில்
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    இல்லையெனில்
      result = CURLE_OUT_OF_MEMORY;
  }
  திரும்பு result;
}
#endif

நிலையான bool dnscache_ai_has_family(கட்டமைப்பு Curl_addrinfo *ai, முழு ai_family)
{
  க்கு(; ai; ai = ai->ai_next) {
    எனில்(ai->ai_family == ai_family)
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_entry_create(கட்டமைப்பு dnsc_id *pid,
                                                bool permanent)
{
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;

  எனில்(curlx_strlen(&pid->name) > UINT16_MAX)
    செல் out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  எனில்(!dns)
    செல் out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  எனில்(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  திரும்பு dns;
}

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_entry_assign_addr(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு Curl_dns_entry *dns,
  uint8_t dns_queries,
  கட்டமைப்பு Curl_addrinfo **paddr1,
  கட்டமைப்பு Curl_addrinfo **paddr2)
{
  எனில்(!dns)
    செல் out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  எனில்(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = வெற்று;
  }
  எனில்(paddr2 && *paddr2) {
    கட்டமைப்பு Curl_addrinfo **phead = &dns->addr;
    போது(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = வெற்று;
  }

  எனில்((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  எனில்((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  எனில்(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    எனில்(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = வெற்று;
      செல் out;
    }
  }
#இல்லையெனில்
  (வெறுமை)data;
#endif

out:
  எனில்(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = வெற்று;
  }
  எனில்(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = வெற்று;
  }
  திரும்பு dns;
}

கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_addr(கட்டமைப்பு Curl_easy *data,
                                         uint8_t dns_queries,
                                         கட்டமைப்பு Curl_addrinfo **paddr,
                                         கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, வெற்று);
  திரும்பு dns;
}

கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_addr2(கட்டமைப்பு Curl_easy *data,
                                          uint8_t dns_queries,
                                          கட்டமைப்பு Curl_addrinfo **paddr1,
                                          கட்டமைப்பு Curl_addrinfo **paddr2,
                                          கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  திரும்பு dns;
}

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_add_entry(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_dnscache *dnscache,
                                             கட்டமைப்பு dnsc_சாவி *சாவி,
                                             கட்டமைப்பு Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  திரும்பு Curl_hash_add(&dnscache->entries, சாவி->data, சாவி->len, entry);
}

#ifdef USE_HTTPSRR
நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_entry_assign_https(
  கட்டமைப்பு Curl_dns_entry *dns,
  கட்டமைப்பு Curl_https_rrinfo **phinfo)
{
  எனில்(!dns)
    செல் out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  எனில்(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = வெற்று;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  எனில்(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = வெற்று;
  }
out:
  எனில்(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = வெற்று;
  }
  திரும்பு dns;
}

கட்டமைப்பு Curl_dns_entry *Curl_dnsc_mk_https(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_https_rrinfo **phinfo,
                                          கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு Curl_dns_entry *dns;

  (வெறுமை)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  திரும்பு dns;
}

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_add_https(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_dnscache *dnscache,
                                             கட்டமைப்பு Curl_https_rrinfo **phinfo,
                                             கட்டமைப்பு dnsc_id *id)
{
  கட்டமைப்பு Curl_dns_entry *dns, *dns2;
  கட்டமைப்பு dnsc_சாவி சாவி;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  எனில்(!dns)
    திரும்பு வெற்று;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&சாவி, id);
  dns2 = dnsc_add_entry(data, dnscache, &சாவி, dns);
  எனில்(!dns2) {
    dnscache_entry_free(dns);
    திரும்பு வெற்று;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  திரும்பு dns;
}
#endif /* USE_HTTPSRR */

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_add_addr(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            கட்டமைப்பு Curl_addrinfo **paddr,
                                            கட்டமைப்பு dnsc_id *id,
                                            கட்டமைப்பு dnsc_சாவி *சாவி,
                                            bool permanent)
{
  கட்டமைப்பு Curl_dns_entry *dns;
  கட்டமைப்பு Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, வெற்று);
  எனில்(!dns)
    திரும்பு வெற்று;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, சாவி, dns);
  எனில்(!dns2) {
    dnscache_entry_free(dns);
    திரும்பு வெற்று;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  திரும்பு dns;
}

நிலையான கட்டமைப்பு Curl_dns_entry *dnsc_add_peer_addr(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு Curl_dnscache *dnscache,
  uint8_t dns_queries,
  கட்டமைப்பு Curl_addrinfo **paddr,
  கட்டமைப்பு dnsc_id *id,
  bool permanent)
{
  கட்டமைப்பு Curl_dns_entry *dns;
  கட்டமைப்பு Curl_dns_entry *dns2;
  கட்டமைப்பு dnsc_சாவி சாவி;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, வெற்று);
  எனில்(!dns)
    திரும்பு வெற்று;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&சாவி, id);
  dns2 = dnsc_add_entry(data, dnscache, &சாவி, dns);
  எனில்(!dns2) {
    dnscache_entry_free(dns);
    திரும்பு வெற்று;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  திரும்பு dns;
}

CURLcode Curl_dnscache_add(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_dns_entry *entry)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  கட்டமைப்பு Curl_str name;
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு dnsc_சாவி சாவி;

  எனில்(!dnscache)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!entry || (entry->type == CURL_DNST_INIT))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&சாவி, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  எனில்(!dnsc_add_entry(data, dnscache, &சாவி, entry)) {
    dnscache_unlock(data, dnscache);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  திரும்பு CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(கட்டமைப்பு Curl_easy *data,
                                    uint8_t dns_queries,
                                    கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  கட்டமைப்பு dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  எனில்(!dnscache)
    திரும்பு CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  எனில்(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, வெற்று, &id, FALSE);
    எனில்(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  இல்லையெனில் எனில்(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, வெற்று, &id);
    எனில்(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  இல்லையெனில் {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  எனில்(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  திரும்பு result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
வெறுமை Curl_dns_entry_unlink(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_dns_entry **pdns)
{
  எனில்(*pdns) {
    கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
    கட்டமைப்பு Curl_dns_entry *dns = *pdns;
    *pdns = வெற்று;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    எனில்(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

நிலையான வெறுமை dnscache_entry_dtor(வெறுமை *entry)
{
  கட்டமைப்பு Curl_dns_entry *dns = (கட்டமைப்பு Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  எனில்(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
வெறுமை Curl_dnscache_init(கட்டமைப்பு Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

வெறுமை Curl_dnscache_destroy(கட்டமைப்பு Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_dnscache *dnscache = dnscache_get(data);
  கட்டமைப்பு curl_slist *hostp;
  கட்டமைப்பு dnsc_id id;
  கட்டமைப்பு dnsc_சாவி சாவி;

  எனில்(!dnscache)
    திரும்பு CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  க்கு(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    மாறிலி எழுத்து *host = hostp->data;
    கட்டமைப்பு Curl_str source;
    எனில்(!host)
      தொடர்;
    எனில்(*host == '-') {
      curl_off_t இலக்கம் = 0;
      host++;
      எனில்(!curlx_str_single(&host, '[')) {
        எனில்(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          தொடர்;
      }
      இல்லையெனில் {
        எனில்(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          தொடர்;
        }
      }

      எனில்(!curlx_str_எண்(&host, &இலக்கம், 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)இலக்கம்);
        dnsc_id2key(&சாவி, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, சாவி.data, சாவி.len);
        dnscache_unlock(data, dnscache);
      }
    }
    இல்லையெனில் {
      கட்டமைப்பு Curl_dns_entry *dns;
      கட்டமைப்பு Curl_addrinfo *head = வெற்று, *tail = வெற்று;
      எழுத்து address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(மாறிலி எழுத்து *addresses = வெற்று);

      எனில்(*host == '+') {
        host++;
        permanent = FALSE;
      }
      எனில்(!curlx_str_single(&host, '[')) {
        எனில்(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          தொடர்;
      }
      இல்லையெனில் {
        எனில்(curlx_str_until(&host, &source, 4096, ':'))
          தொடர்;
      }
      எனில்(curlx_str_single(&host, ':') ||
         curlx_str_எண்(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        செல் err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      போது(*host) {
        கட்டமைப்பு Curl_str target;
        கட்டமைப்பு Curl_addrinfo *ai;
        CURLcode result;

        எனில்(!curlx_str_single(&host, '[')) {
          எனில்(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            செல் err;
        }
        இல்லையெனில் {
          எனில்(curlx_str_until(&host, &target, 4096, ',')) {
            எனில்(curlx_str_single(&host, ','))
              செல் err;
            /* survive nothing but a comma */
            தொடர்;
          }
        }
#ifndef USE_IPV6
        எனில்(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (முழு)curlx_strlen(&target), curlx_str(&target));
          எனில்(curlx_str_single(&host, ','))
            செல் err;
          தொடர்;
        }
#endif

        எனில்(curlx_strlen(&target) >= அளவு(address))
          செல் err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        எனில்(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          செல் err;
        }

        எனில்(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        இல்லையெனில் {
          head = tail = ai;
        }
        எனில்(curlx_str_single(&host, ','))
          நிறுத்து;
      }

      எனில்(!head)
        செல் err;

      error = FALSE;
err:
      எனில்(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        திரும்பு CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&சாவி, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, சாவி.data, சாவி.len);

      எனில்(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (முழு)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, சாவி.data, சாவி.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &சாவி, permanent);
      எனில்(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      எனில்(!dns)
        திரும்பு CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (முழு)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      எனில்(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = வெற்று; /* dealt with now */

  திரும்பு CURLE_OK;
}
