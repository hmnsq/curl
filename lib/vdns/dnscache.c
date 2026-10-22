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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#അന്തര്ഭാവ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "hash.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "vdns/dnscache.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/strcopy.h"
#അന്തര്ഭാവ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST ശൂന്യം dnsc_peer2id(ഘടന dnsc_id *pid, അക്ഷരം type,
                           ഘടന Curl_peer *peer);
UNITTEST ശൂന്യം dnsc_peer2id(ഘടന dnsc_id *pid, അക്ഷരം type,
                           ഘടന Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

സ്ഥിതം ശൂന്യം dnsc_str2id(ഘടന dnsc_id *pid, അക്ഷരം type,
                        ഘടന Curl_str *name, uint16_t port)
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
UNITTEST ശൂന്യം dnsc_id2key(ഘടന dnsc_താക്കോൽ *താക്കോൽ, ഘടന dnsc_id *id);
UNITTEST ശൂന്യം dnsc_id2key(ഘടന dnsc_താക്കോൽ *താക്കോൽ, ഘടന dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  എങ്കിൽ(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  താക്കോൽ->data[0] = id->type;
  താക്കോൽ->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  താക്കോൽ->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((അക്ഷരം *)താക്കോൽ->data + 3, curlx_str(&id->name), namelen);
  താക്കോൽ->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(ഘടന dnsc_id *id,
                                    ഘടന Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(ഘടന dnsc_id *id,
                                    ഘടന Curl_dns_entry *dns)
{
  തിരികെ dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

സ്ഥിതം ശൂന്യം dnscache_entry_free(ഘടന Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

ഘടന dnscache_prune_data {
  സ്ഥിരം ഘടന curltime *pnow;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ dnscache_entry_is_stale(ശൂന്യം *datap, ശൂന്യം *hc)
{
  ഘടന dnscache_prune_data *prune = (ഘടന dnscache_prune_data *)datap;
  ഘടന Curl_dns_entry *dns = (ഘടന Curl_dns_entry *)hc;

  എങ്കിൽ(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    തിരഞ്ഞെടുപ്പ്(dns->type) {
    അവസ്ഥ CURL_DNST_ADDR:
      എങ്കിൽ(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      നിർത്തുക;
#ifdef USE_HTTPSRR
    അവസ്ഥ CURL_DNST_HTTPS:
      എങ്കിൽ(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
    എങ്കിൽ(age_ms >= prune->max_age_ms)
      തിരികെ TRUE;
    എങ്കിൽ(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  തിരികെ FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
സ്ഥിതം timediff_t dnscache_prune(ഘടന Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 സ്ഥിരം ഘടന curltime *pnow)
{
  ഘടന dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (ശൂന്യം *)&user,
                                 dnscache_entry_is_stale);

  തിരികെ user.oldest_ms;
}

സ്ഥിതം ഘടന Curl_dnscache *dnscache_get(ഘടന Curl_easy *data)
{
  എങ്കിൽ(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    തിരികെ &data->share->dnscache;
  എങ്കിൽ(data->multi)
    തിരികെ &data->multi->dnscache;
  തിരികെ ശൂന്യ;
}

സ്ഥിതം ശൂന്യം dnscache_lock(ഘടന Curl_easy *data,
                          ഘടന Curl_dnscache *dnscache)
{
  എങ്കിൽ(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

സ്ഥിതം ശൂന്യം dnscache_unlock(ഘടന Curl_easy *data,
                            ഘടന Curl_dnscache *dnscache)
{
  എങ്കിൽ(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
ശൂന്യം Curl_dnscache_prune(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  എങ്കിൽ(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    തിരികെ;

  dnscache_lock(data, dnscache);

  ചെയ്യുക {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    എങ്കിൽ(Curl_hash_എണ്ണം(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    അല്ലെങ്കിൽ
      നിർത്തുക;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } ആകുമ്പോൾ(timeout_ms);

  dnscache_unlock(data, dnscache);
}

ശൂന്യം Curl_dnscache_clear(ഘടന Curl_easy *data)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  എങ്കിൽ(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
സ്ഥിതം CURLcode fetch_entry(ഘടന Curl_easy *data,
                            ഘടന Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ഘടന Curl_peer *peer,
                            ഘടന Curl_dns_entry **pdns)
{
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  ഘടന dnsc_id id;
  ഘടന dnsc_താക്കോൽ താക്കോൽ;
  അക്ഷരം type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = ശൂന്യ;
  എങ്കിൽ(!dnscache)
    തിരികെ CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&താക്കോൽ, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);

  എങ്കിൽ(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ശൂന്യ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  എങ്കിൽ(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    ഘടന Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&താക്കോൽ, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);
  }

  എങ്കിൽ(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ഘടന dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    എങ്കിൽ(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = ശൂന്യ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  എങ്കിൽ(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    എങ്കിൽ((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ശൂന്യ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(dns && !dns->addr) { /* negative entry */
      dns = ശൂന്യ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  തിരികെ result;
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
CURLcode Curl_dnscache_get(ഘടന Curl_easy *data,
                           uint8_t dns_queries,
                           ഘടന Curl_peer *peer,
                           ഘടന Curl_dns_entry **pentry)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  എങ്കിൽ(!result && dns)
    dns->refcount++; /* we pass out a reference */
  അല്ലെങ്കിൽ എങ്കിൽ(result) {
    DEBUGASSERT(!dns);
    dns = ശൂന്യ;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (പൂർണ്ണസംഖ്യ)result, dns ? "" : "not ");
  *pentry = dns;
  തിരികെ result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ അക്കം_addresses(സ്ഥിരം ഘടന Curl_addrinfo *addr)
{
  പൂർണ്ണസംഖ്യ i = 0;
  ആകുമ്പോൾ(addr) {
    addr = addr->ai_next;
    i++;
  }
  തിരികെ i;
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
UNITTEST CURLcode dns_shuffle_addr(ഘടന Curl_easy *data,
                                   ഘടന Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ഘടന Curl_easy *data,
                                   ഘടന Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_addrs = അക്കം_addresses(*addr);

  എങ്കിൽ(അക്കം_addrs > 1) {
    ഘടന Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", അക്കം_addrs);

    nodes = curlx_malloc(അക്കം_addrs * വലുപ്പം(*nodes));
    എങ്കിൽ(nodes) {
      പൂർണ്ണസംഖ്യ i;
      unsigned പൂർണ്ണസംഖ്യ *rnd;
      സ്ഥിരം size_t rnd_size = അക്കം_addrs * വലുപ്പം(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      വേണ്ടി(i = 1; i < അക്കം_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      എങ്കിൽ(rnd) {
        /* Fisher-Yates shuffle */
        എങ്കിൽ(Curl_rand(data, (unsigned അക്ഷരം *)rnd, rnd_size) == CURLE_OK) {
          ഘടന Curl_addrinfo *swap_tmp;
          വേണ്ടി(i = അക്കം_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned പൂർണ്ണസംഖ്യ)(i + 1)];
            nodes[rnd[i] % (unsigned പൂർണ്ണസംഖ്യ)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          വേണ്ടി(i = 1; i < അക്കം_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[അക്കം_addrs - 1]->ai_next = ശൂന്യ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      അല്ലെങ്കിൽ
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    അല്ലെങ്കിൽ
      result = CURLE_OUT_OF_MEMORY;
  }
  തിരികെ result;
}
#endif

സ്ഥിതം bool dnscache_ai_has_family(ഘടന Curl_addrinfo *ai, പൂർണ്ണസംഖ്യ ai_family)
{
  വേണ്ടി(; ai; ai = ai->ai_next) {
    എങ്കിൽ(ai->ai_family == ai_family)
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_entry_create(ഘടന dnsc_id *pid,
                                                bool permanent)
{
  ഘടന Curl_dns_entry *dns = ശൂന്യ;

  എങ്കിൽ(curlx_strlen(&pid->name) > UINT16_MAX)
    പോകുക out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, വലുപ്പം(ഘടന Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  എങ്കിൽ(!dns)
    പോകുക out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  എങ്കിൽ(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  തിരികെ dns;
}

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_entry_assign_addr(
  ഘടന Curl_easy *data,
  ഘടന Curl_dns_entry *dns,
  uint8_t dns_queries,
  ഘടന Curl_addrinfo **paddr1,
  ഘടന Curl_addrinfo **paddr2)
{
  എങ്കിൽ(!dns)
    പോകുക out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  എങ്കിൽ(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ശൂന്യ;
  }
  എങ്കിൽ(paddr2 && *paddr2) {
    ഘടന Curl_addrinfo **phead = &dns->addr;
    ആകുമ്പോൾ(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = ശൂന്യ;
  }

  എങ്കിൽ((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  എങ്കിൽ((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  എങ്കിൽ(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    എങ്കിൽ(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = ശൂന്യ;
      പോകുക out;
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
#endif

out:
  എങ്കിൽ(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ശൂന്യ;
  }
  എങ്കിൽ(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ശൂന്യ;
  }
  തിരികെ dns;
}

ഘടന Curl_dns_entry *Curl_dnsc_mk_addr(ഘടന Curl_easy *data,
                                         uint8_t dns_queries,
                                         ഘടന Curl_addrinfo **paddr,
                                         ഘടന Curl_peer *peer)
{
  ഘടന dnsc_id id;
  ഘടന Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ശൂന്യ);
  തിരികെ dns;
}

ഘടന Curl_dns_entry *Curl_dnsc_mk_addr2(ഘടന Curl_easy *data,
                                          uint8_t dns_queries,
                                          ഘടന Curl_addrinfo **paddr1,
                                          ഘടന Curl_addrinfo **paddr2,
                                          ഘടന Curl_peer *peer)
{
  ഘടന dnsc_id id;
  ഘടന Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  തിരികെ dns;
}

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_add_entry(ഘടന Curl_easy *data,
                                             ഘടന Curl_dnscache *dnscache,
                                             ഘടന dnsc_താക്കോൽ *താക്കോൽ,
                                             ഘടന Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  തിരികെ Curl_hash_add(&dnscache->entries, താക്കോൽ->data, താക്കോൽ->len, entry);
}

#ifdef USE_HTTPSRR
സ്ഥിതം ഘടന Curl_dns_entry *dnsc_entry_assign_https(
  ഘടന Curl_dns_entry *dns,
  ഘടന Curl_https_rrinfo **phinfo)
{
  എങ്കിൽ(!dns)
    പോകുക out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  എങ്കിൽ(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ശൂന്യ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  എങ്കിൽ(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ശൂന്യ;
  }
out:
  എങ്കിൽ(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ശൂന്യ;
  }
  തിരികെ dns;
}

ഘടന Curl_dns_entry *Curl_dnsc_mk_https(ഘടന Curl_easy *data,
                                          ഘടന Curl_https_rrinfo **phinfo,
                                          ഘടന Curl_peer *peer)
{
  ഘടന dnsc_id id;
  ഘടന Curl_dns_entry *dns;

  (ശൂന്യം)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  തിരികെ dns;
}

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_add_https(ഘടന Curl_easy *data,
                                             ഘടന Curl_dnscache *dnscache,
                                             ഘടന Curl_https_rrinfo **phinfo,
                                             ഘടന dnsc_id *id)
{
  ഘടന Curl_dns_entry *dns, *dns2;
  ഘടന dnsc_താക്കോൽ താക്കോൽ;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  എങ്കിൽ(!dns)
    തിരികെ ശൂന്യ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&താക്കോൽ, id);
  dns2 = dnsc_add_entry(data, dnscache, &താക്കോൽ, dns);
  എങ്കിൽ(!dns2) {
    dnscache_entry_free(dns);
    തിരികെ ശൂന്യ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  തിരികെ dns;
}
#endif /* USE_HTTPSRR */

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_add_addr(ഘടന Curl_easy *data,
                                            ഘടന Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ഘടന Curl_addrinfo **paddr,
                                            ഘടന dnsc_id *id,
                                            ഘടന dnsc_താക്കോൽ *താക്കോൽ,
                                            bool permanent)
{
  ഘടന Curl_dns_entry *dns;
  ഘടന Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ശൂന്യ);
  എങ്കിൽ(!dns)
    തിരികെ ശൂന്യ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, താക്കോൽ, dns);
  എങ്കിൽ(!dns2) {
    dnscache_entry_free(dns);
    തിരികെ ശൂന്യ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  തിരികെ dns;
}

സ്ഥിതം ഘടന Curl_dns_entry *dnsc_add_peer_addr(
  ഘടന Curl_easy *data,
  ഘടന Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ഘടന Curl_addrinfo **paddr,
  ഘടന dnsc_id *id,
  bool permanent)
{
  ഘടന Curl_dns_entry *dns;
  ഘടന Curl_dns_entry *dns2;
  ഘടന dnsc_താക്കോൽ താക്കോൽ;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ശൂന്യ);
  എങ്കിൽ(!dns)
    തിരികെ ശൂന്യ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&താക്കോൽ, id);
  dns2 = dnsc_add_entry(data, dnscache, &താക്കോൽ, dns);
  എങ്കിൽ(!dns2) {
    dnscache_entry_free(dns);
    തിരികെ ശൂന്യ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  തിരികെ dns;
}

CURLcode Curl_dnscache_add(ഘടന Curl_easy *data,
                           ഘടന Curl_dns_entry *entry)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  ഘടന Curl_str name;
  ഘടന dnsc_id id;
  ഘടന dnsc_താക്കോൽ താക്കോൽ;

  എങ്കിൽ(!dnscache)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!entry || (entry->type == CURL_DNST_INIT))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&താക്കോൽ, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  എങ്കിൽ(!dnsc_add_entry(data, dnscache, &താക്കോൽ, entry)) {
    dnscache_unlock(data, dnscache);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  തിരികെ CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(ഘടന Curl_easy *data,
                                    uint8_t dns_queries,
                                    ഘടന Curl_peer *peer)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  ഘടന dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  എങ്കിൽ(!dnscache)
    തിരികെ CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  എങ്കിൽ(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, ശൂന്യ, &id, FALSE);
    എങ്കിൽ(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  അല്ലെങ്കിൽ എങ്കിൽ(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, ശൂന്യ, &id);
    എങ്കിൽ(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  അല്ലെങ്കിൽ {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  എങ്കിൽ(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  തിരികെ result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
ശൂന്യം Curl_dns_entry_unlink(ഘടന Curl_easy *data,
                           ഘടന Curl_dns_entry **pdns)
{
  എങ്കിൽ(*pdns) {
    ഘടന Curl_dnscache *dnscache = dnscache_get(data);
    ഘടന Curl_dns_entry *dns = *pdns;
    *pdns = ശൂന്യ;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    എങ്കിൽ(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

സ്ഥിതം ശൂന്യം dnscache_entry_dtor(ശൂന്യം *entry)
{
  ഘടന Curl_dns_entry *dns = (ഘടന Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  എങ്കിൽ(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
ശൂന്യം Curl_dnscache_init(ഘടന Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

ശൂന്യം Curl_dnscache_destroy(ഘടന Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ഘടന Curl_easy *data)
{
  ഘടന Curl_dnscache *dnscache = dnscache_get(data);
  ഘടന curl_slist *hostp;
  ഘടന dnsc_id id;
  ഘടന dnsc_താക്കോൽ താക്കോൽ;

  എങ്കിൽ(!dnscache)
    തിരികെ CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  വേണ്ടി(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    സ്ഥിരം അക്ഷരം *host = hostp->data;
    ഘടന Curl_str source;
    എങ്കിൽ(!host)
      തുടരുക;
    എങ്കിൽ(*host == '-') {
      curl_off_t അക്കം = 0;
      host++;
      എങ്കിൽ(!curlx_str_single(&host, '[')) {
        എങ്കിൽ(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          തുടരുക;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          തുടരുക;
        }
      }

      എങ്കിൽ(!curlx_str_സംഖ്യ(&host, &അക്കം, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)അക്കം);
        dnsc_id2key(&താക്കോൽ, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);
        dnscache_unlock(data, dnscache);
      }
    }
    അല്ലെങ്കിൽ {
      ഘടന Curl_dns_entry *dns;
      ഘടന Curl_addrinfo *head = ശൂന്യ, *tail = ശൂന്യ;
      അക്ഷരം address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(സ്ഥിരം അക്ഷരം *addresses = ശൂന്യ);

      എങ്കിൽ(*host == '+') {
        host++;
        permanent = FALSE;
      }
      എങ്കിൽ(!curlx_str_single(&host, '[')) {
        എങ്കിൽ(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          തുടരുക;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(curlx_str_until(&host, &source, 4096, ':'))
          തുടരുക;
      }
      എങ്കിൽ(curlx_str_single(&host, ':') ||
         curlx_str_സംഖ്യ(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        പോകുക err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      ആകുമ്പോൾ(*host) {
        ഘടന Curl_str target;
        ഘടന Curl_addrinfo *ai;
        CURLcode result;

        എങ്കിൽ(!curlx_str_single(&host, '[')) {
          എങ്കിൽ(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            പോകുക err;
        }
        അല്ലെങ്കിൽ {
          എങ്കിൽ(curlx_str_until(&host, &target, 4096, ',')) {
            എങ്കിൽ(curlx_str_single(&host, ','))
              പോകുക err;
            /* survive nothing but a comma */
            തുടരുക;
          }
        }
#ifndef USE_IPV6
        എങ്കിൽ(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (പൂർണ്ണസംഖ്യ)curlx_strlen(&target), curlx_str(&target));
          എങ്കിൽ(curlx_str_single(&host, ','))
            പോകുക err;
          തുടരുക;
        }
#endif

        എങ്കിൽ(curlx_strlen(&target) >= വലുപ്പം(address))
          പോകുക err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        എങ്കിൽ(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          പോകുക err;
        }

        എങ്കിൽ(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        അല്ലെങ്കിൽ {
          head = tail = ai;
        }
        എങ്കിൽ(curlx_str_single(&host, ','))
          നിർത്തുക;
      }

      എങ്കിൽ(!head)
        പോകുക err;

      error = FALSE;
err:
      എങ്കിൽ(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        തിരികെ CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&താക്കോൽ, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);

      എങ്കിൽ(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (പൂർണ്ണസംഖ്യ)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, താക്കോൽ.data, താക്കോൽ.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &താക്കോൽ, permanent);
      എങ്കിൽ(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      എങ്കിൽ(!dns)
        തിരികെ CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (പൂർണ്ണസംഖ്യ)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      എങ്കിൽ(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = ശൂന്യ; /* dealt with now */

  തിരികെ CURLE_OK;
}
