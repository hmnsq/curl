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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#অন্তর্ভাৱ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_share.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "hash.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "strcase.h"
#অন্তর্ভাৱ "vdns/dnscache.h"
#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"
#অন্তর্ভাৱ "curlx/inet_ntop.h"
#অন্তর্ভাৱ "curlx/inet_pton.h"
#অন্তর্ভাৱ "curlx/strcopy.h"
#অন্তর্ভাৱ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST শূন্য dnsc_peer2id(সংরচনা dnsc_id *pid, ৱর্ণ type,
                           সংরচনা Curl_peer *peer);
UNITTEST শূন্য dnsc_peer2id(সংরচনা dnsc_id *pid, ৱর্ণ type,
                           সংরচনা Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

স্থির শূন্য dnsc_str2id(সংরচনা dnsc_id *pid, ৱর্ণ type,
                        সংরচনা Curl_str *name, uint16_t port)
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
UNITTEST শূন্য dnsc_id2key(সংরচনা dnsc_চাবি *চাবি, সংরচনা dnsc_id *id);
UNITTEST শূন্য dnsc_id2key(সংরচনা dnsc_চাবি *চাবি, সংরচনা dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  যদি(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  চাবি->data[0] = id->type;
  চাবি->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  চাবি->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((ৱর্ণ *)চাবি->data + 3, curlx_str(&id->name), namelen);
  চাবি->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(সংরচনা dnsc_id *id,
                                    সংরচনা Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(সংরচনা dnsc_id *id,
                                    সংরচনা Curl_dns_entry *dns)
{
  ঘূৰাই dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

স্থির শূন্য dnscache_entry_free(সংরচনা Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

সংরচনা dnscache_prune_data {
  ধ্রুৱ সংরচনা curltime *pnow;
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
স্থির পূর্ণাঙ্ক dnscache_entry_is_stale(শূন্য *datap, শূন্য *hc)
{
  সংরচনা dnscache_prune_data *prune = (সংরচনা dnscache_prune_data *)datap;
  সংরচনা Curl_dns_entry *dns = (সংরচনা Curl_dns_entry *)hc;

  যদি(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ৱিকল্প(dns->type) {
    পক্ষ CURL_DNST_ADDR:
      যদি(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ভঙ্গ;
#ifdef USE_HTTPSRR
    পক্ষ CURL_DNST_HTTPS:
      যদি(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ভঙ্গ;
#endif
    স্ৱভাৱ:
      ভঙ্গ;
    }
    যদি(age_ms >= prune->max_age_ms)
      ঘূৰাই TRUE;
    যদি(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ঘূৰাই FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
স্থির timediff_t dnscache_prune(সংরচনা Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (শূন্য *)&user,
                                 dnscache_entry_is_stale);

  ঘূৰাই user.oldest_ms;
}

স্থির সংরচনা Curl_dnscache *dnscache_get(সংরচনা Curl_easy *data)
{
  যদি(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    ঘূৰাই &data->share->dnscache;
  যদি(data->multi)
    ঘূৰাই &data->multi->dnscache;
  ঘূৰাই রিক্ত;
}

স্থির শূন্য dnscache_lock(সংরচনা Curl_easy *data,
                          সংরচনা Curl_dnscache *dnscache)
{
  যদি(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

স্থির শূন্য dnscache_unlock(সংরচনা Curl_easy *data,
                            সংরচনা Curl_dnscache *dnscache)
{
  যদি(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
শূন্য Curl_dnscache_prune(সংরচনা Curl_easy *data, ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  যদি(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ঘূৰাই;

  dnscache_lock(data, dnscache);

  কৰা {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    যদি(Curl_hash_গুনতি(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    অন্যথা
      ভঙ্গ;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } যেতিয়ালৈ(timeout_ms);

  dnscache_unlock(data, dnscache);
}

শূন্য Curl_dnscache_clear(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  যদি(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
স্থির CURLcode fetch_entry(সংরচনা Curl_easy *data,
                            সংরচনা Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            সংরচনা Curl_peer *peer,
                            সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  সংরচনা dnsc_id id;
  সংরচনা dnsc_চাবি চাবি;
  ৱর্ণ type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = রিক্ত;
  যদি(!dnscache)
    ঘূৰাই CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&চাবি, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, চাবি.data, চাবি.len);

  যদি(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = রিক্ত;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  যদি(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    সংরচনা Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&চাবি, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, চাবি.data, চাবি.len);
  }

  যদি(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    সংরচনা dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    যদি(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = রিক্ত; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, চাবি.data, চাবি.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  যদি(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    যদি((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = রিক্ত;
    }
    অন্যথা যদি(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = রিক্ত;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    অন্যথা যদি(dns && !dns->addr) { /* negative entry */
      dns = রিক্ত;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ঘূৰাই result;
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
CURLcode Curl_dnscache_get(সংরচনা Curl_easy *data,
                           uint8_t dns_queries,
                           সংরচনা Curl_peer *peer,
                           সংরচনা Curl_dns_entry **pentry)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  যদি(!result && dns)
    dns->refcount++; /* we pass out a reference */
  অন্যথা যদি(result) {
    DEBUGASSERT(!dns);
    dns = রিক্ত;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (পূর্ণাঙ্ক)result, dns ? "" : "not ");
  *pentry = dns;
  ঘূৰাই result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
স্থির পূর্ণাঙ্ক অঙ্ক_addresses(ধ্রুৱ সংরচনা Curl_addrinfo *addr)
{
  পূর্ণাঙ্ক i = 0;
  যেতিয়ালৈ(addr) {
    addr = addr->ai_next;
    i++;
  }
  ঘূৰাই i;
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
UNITTEST CURLcode dns_shuffle_addr(সংরচনা Curl_easy *data,
                                   সংরচনা Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(সংরচনা Curl_easy *data,
                                   সংরচনা Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ পূর্ণাঙ্ক অঙ্ক_addrs = অঙ্ক_addresses(*addr);

  যদি(অঙ্ক_addrs > 1) {
    সংরচনা Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", অঙ্ক_addrs);

    nodes = curlx_malloc(অঙ্ক_addrs * পরিমাণ(*nodes));
    যদি(nodes) {
      পূর্ণাঙ্ক i;
      unsigned পূর্ণাঙ্ক *rnd;
      ধ্রুৱ size_t rnd_size = অঙ্ক_addrs * পরিমাণ(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      বাবে(i = 1; i < অঙ্ক_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      যদি(rnd) {
        /* Fisher-Yates shuffle */
        যদি(Curl_rand(data, (unsigned ৱর্ণ *)rnd, rnd_size) == CURLE_OK) {
          সংরচনা Curl_addrinfo *swap_tmp;
          বাবে(i = অঙ্ক_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned পূর্ণাঙ্ক)(i + 1)];
            nodes[rnd[i] % (unsigned পূর্ণাঙ্ক)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          বাবে(i = 1; i < অঙ্ক_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[অঙ্ক_addrs - 1]->ai_next = রিক্ত;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      অন্যথা
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    অন্যথা
      result = CURLE_OUT_OF_MEMORY;
  }
  ঘূৰাই result;
}
#endif

স্থির bool dnscache_ai_has_family(সংরচনা Curl_addrinfo *ai, পূর্ণাঙ্ক ai_family)
{
  বাবে(; ai; ai = ai->ai_next) {
    যদি(ai->ai_family == ai_family)
      ঘূৰাই TRUE;
  }
  ঘূৰাই FALSE;
}

স্থির সংরচনা Curl_dns_entry *dnsc_entry_create(সংরচনা dnsc_id *pid,
                                                bool permanent)
{
  সংরচনা Curl_dns_entry *dns = রিক্ত;

  যদি(curlx_strlen(&pid->name) > UINT16_MAX)
    গচ্ছ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, পরিমাণ(সংরচনা Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  যদি(!dns)
    গচ্ছ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  যদি(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  ঘূৰাই dns;
}

স্থির সংরচনা Curl_dns_entry *dnsc_entry_assign_addr(
  সংরচনা Curl_easy *data,
  সংরচনা Curl_dns_entry *dns,
  uint8_t dns_queries,
  সংরচনা Curl_addrinfo **paddr1,
  সংরচনা Curl_addrinfo **paddr2)
{
  যদি(!dns)
    গচ্ছ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  যদি(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = রিক্ত;
  }
  যদি(paddr2 && *paddr2) {
    সংরচনা Curl_addrinfo **phead = &dns->addr;
    যেতিয়ালৈ(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = রিক্ত;
  }

  যদি((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  যদি((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  যদি(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    যদি(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = রিক্ত;
      গচ্ছ out;
    }
  }
#অন্যথা
  (শূন্য)data;
#endif

out:
  যদি(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = রিক্ত;
  }
  যদি(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = রিক্ত;
  }
  ঘূৰাই dns;
}

সংরচনা Curl_dns_entry *Curl_dnsc_mk_addr(সংরচনা Curl_easy *data,
                                         uint8_t dns_queries,
                                         সংরচনা Curl_addrinfo **paddr,
                                         সংরচনা Curl_peer *peer)
{
  সংরচনা dnsc_id id;
  সংরচনা Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, রিক্ত);
  ঘূৰাই dns;
}

সংরচনা Curl_dns_entry *Curl_dnsc_mk_addr2(সংরচনা Curl_easy *data,
                                          uint8_t dns_queries,
                                          সংরচনা Curl_addrinfo **paddr1,
                                          সংরচনা Curl_addrinfo **paddr2,
                                          সংরচনা Curl_peer *peer)
{
  সংরচনা dnsc_id id;
  সংরচনা Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  ঘূৰাই dns;
}

স্থির সংরচনা Curl_dns_entry *dnsc_add_entry(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_dnscache *dnscache,
                                             সংরচনা dnsc_চাবি *চাবি,
                                             সংরচনা Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  ঘূৰাই Curl_hash_add(&dnscache->entries, চাবি->data, চাবি->len, entry);
}

#ifdef USE_HTTPSRR
স্থির সংরচনা Curl_dns_entry *dnsc_entry_assign_https(
  সংরচনা Curl_dns_entry *dns,
  সংরচনা Curl_https_rrinfo **phinfo)
{
  যদি(!dns)
    গচ্ছ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  যদি(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = রিক্ত;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  যদি(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = রিক্ত;
  }
out:
  যদি(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = রিক্ত;
  }
  ঘূৰাই dns;
}

সংরচনা Curl_dns_entry *Curl_dnsc_mk_https(সংরচনা Curl_easy *data,
                                          সংরচনা Curl_https_rrinfo **phinfo,
                                          সংরচনা Curl_peer *peer)
{
  সংরচনা dnsc_id id;
  সংরচনা Curl_dns_entry *dns;

  (শূন্য)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ঘূৰাই dns;
}

স্থির সংরচনা Curl_dns_entry *dnsc_add_https(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_dnscache *dnscache,
                                             সংরচনা Curl_https_rrinfo **phinfo,
                                             সংরচনা dnsc_id *id)
{
  সংরচনা Curl_dns_entry *dns, *dns2;
  সংরচনা dnsc_চাবি চাবি;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  যদি(!dns)
    ঘূৰাই রিক্ত;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&চাবি, id);
  dns2 = dnsc_add_entry(data, dnscache, &চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ঘূৰাই রিক্ত;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ঘূৰাই dns;
}
#endif /* USE_HTTPSRR */

স্থির সংরচনা Curl_dns_entry *dnsc_add_addr(সংরচনা Curl_easy *data,
                                            সংরচনা Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            সংরচনা Curl_addrinfo **paddr,
                                            সংরচনা dnsc_id *id,
                                            সংরচনা dnsc_চাবি *চাবি,
                                            bool permanent)
{
  সংরচনা Curl_dns_entry *dns;
  সংরচনা Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, রিক্ত);
  যদি(!dns)
    ঘূৰাই রিক্ত;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ঘূৰাই রিক্ত;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ঘূৰাই dns;
}

স্থির সংরচনা Curl_dns_entry *dnsc_add_peer_addr(
  সংরচনা Curl_easy *data,
  সংরচনা Curl_dnscache *dnscache,
  uint8_t dns_queries,
  সংরচনা Curl_addrinfo **paddr,
  সংরচনা dnsc_id *id,
  bool permanent)
{
  সংরচনা Curl_dns_entry *dns;
  সংরচনা Curl_dns_entry *dns2;
  সংরচনা dnsc_চাবি চাবি;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, রিক্ত);
  যদি(!dns)
    ঘূৰাই রিক্ত;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&চাবি, id);
  dns2 = dnsc_add_entry(data, dnscache, &চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ঘূৰাই রিক্ত;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ঘূৰাই dns;
}

CURLcode Curl_dnscache_add(সংরচনা Curl_easy *data,
                           সংরচনা Curl_dns_entry *entry)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  সংরচনা Curl_str name;
  সংরচনা dnsc_id id;
  সংরচনা dnsc_চাবি চাবি;

  যদি(!dnscache)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!entry || (entry->type == CURL_DNST_INIT))
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&চাবি, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  যদি(!dnsc_add_entry(data, dnscache, &চাবি, entry)) {
    dnscache_unlock(data, dnscache);
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(সংরচনা Curl_easy *data,
                                    uint8_t dns_queries,
                                    সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  সংরচনা dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  যদি(!dnscache)
    ঘূৰাই CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  যদি(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, রিক্ত, &id, FALSE);
    যদি(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, রিক্ত, &id);
    যদি(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  অন্যথা {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  যদি(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  ঘূৰাই result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
শূন্য Curl_dns_entry_unlink(সংরচনা Curl_easy *data,
                           সংরচনা Curl_dns_entry **pdns)
{
  যদি(*pdns) {
    সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
    সংরচনা Curl_dns_entry *dns = *pdns;
    *pdns = রিক্ত;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    যদি(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

স্থির শূন্য dnscache_entry_dtor(শূন্য *entry)
{
  সংরচনা Curl_dns_entry *dns = (সংরচনা Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  যদি(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
শূন্য Curl_dnscache_init(সংরচনা Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

শূন্য Curl_dnscache_destroy(সংরচনা Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_dnscache *dnscache = dnscache_get(data);
  সংরচনা curl_slist *hostp;
  সংরচনা dnsc_id id;
  সংরচনা dnsc_চাবি চাবি;

  যদি(!dnscache)
    ঘূৰাই CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  বাবে(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ধ্রুৱ ৱর্ণ *host = hostp->data;
    সংরচনা Curl_str source;
    যদি(!host)
      অনুৱর্ত;
    যদি(*host == '-') {
      curl_off_t অঙ্ক = 0;
      host++;
      যদি(!curlx_str_single(&host, '[')) {
        যদি(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          অনুৱর্ত;
      }
      অন্যথা {
        যদি(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          অনুৱর্ত;
        }
      }

      যদি(!curlx_str_সংখ্যা(&host, &অঙ্ক, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)অঙ্ক);
        dnsc_id2key(&চাবি, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, চাবি.data, চাবি.len);
        dnscache_unlock(data, dnscache);
      }
    }
    অন্যথা {
      সংরচনা Curl_dns_entry *dns;
      সংরচনা Curl_addrinfo *head = রিক্ত, *tail = রিক্ত;
      ৱর্ণ address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ধ্রুৱ ৱর্ণ *addresses = রিক্ত);

      যদি(*host == '+') {
        host++;
        permanent = FALSE;
      }
      যদি(!curlx_str_single(&host, '[')) {
        যদি(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          অনুৱর্ত;
      }
      অন্যথা {
        যদি(curlx_str_until(&host, &source, 4096, ':'))
          অনুৱর্ত;
      }
      যদি(curlx_str_single(&host, ':') ||
         curlx_str_সংখ্যা(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        গচ্ছ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      যেতিয়ালৈ(*host) {
        সংরচনা Curl_str target;
        সংরচনা Curl_addrinfo *ai;
        CURLcode result;

        যদি(!curlx_str_single(&host, '[')) {
          যদি(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            গচ্ছ err;
        }
        অন্যথা {
          যদি(curlx_str_until(&host, &target, 4096, ',')) {
            যদি(curlx_str_single(&host, ','))
              গচ্ছ err;
            /* survive nothing but a comma */
            অনুৱর্ত;
          }
        }
#ifndef USE_IPV6
        যদি(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (পূর্ণাঙ্ক)curlx_strlen(&target), curlx_str(&target));
          যদি(curlx_str_single(&host, ','))
            গচ্ছ err;
          অনুৱর্ত;
        }
#endif

        যদি(curlx_strlen(&target) >= পরিমাণ(address))
          গচ্ছ err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        যদি(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          গচ্ছ err;
        }

        যদি(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        অন্যথা {
          head = tail = ai;
        }
        যদি(curlx_str_single(&host, ','))
          ভঙ্গ;
      }

      যদি(!head)
        গচ্ছ err;

      error = FALSE;
err:
      যদি(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        ঘূৰাই CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&চাবি, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, চাবি.data, চাবি.len);

      যদি(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (পূর্ণাঙ্ক)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, চাবি.data, চাবি.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &চাবি, permanent);
      যদি(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      যদি(!dns)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (পূর্ণাঙ্ক)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      যদি(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = রিক্ত; /* dealt with now */

  ঘূৰাই CURLE_OK;
}
