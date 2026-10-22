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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#অন্তর্ভাব <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "vdns/dnscache.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strcopy.h"
#অন্তর্ভাব "curlx/strparse.h"

#define MAX_DNS_CACHE_আকার 29999

/* @unittest 3217 */
UNITTEST শূন্য dnsc_peer2id(কাঠামো dnsc_id *pid, অক্ষর type,
                           কাঠামো Curl_peer *peer);
UNITTEST শূন্য dnsc_peer2id(কাঠামো dnsc_id *pid, অক্ষর type,
                           কাঠামো Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

স্থির শূন্য dnsc_str2id(কাঠামো dnsc_id *pid, অক্ষর type,
                        কাঠামো Curl_str *name, uint16_t port)
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
UNITTEST শূন্য dnsc_id2key(কাঠামো dnsc_চাবি *চাবি, কাঠামো dnsc_id *id);
UNITTEST শূন্য dnsc_id2key(কাঠামো dnsc_চাবি *চাবি, কাঠামো dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  যদি(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  চাবি->তথ্য[0] = id->type;
  চাবি->তথ্য[1] = (uint8_t)((id->port >> 8) & 0xff);
  চাবি->তথ্য[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((অক্ষর *)চাবি->তথ্য + 3, curlx_str(&id->name), namelen);
  চাবি->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(কাঠামো dnsc_id *id,
                                    কাঠামো Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(কাঠামো dnsc_id *id,
                                    কাঠামো Curl_dns_entry *dns)
{
  ফেরত dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

স্থির শূন্য dnscache_entry_free(কাঠামো Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

কাঠামো dnscache_prune_তথ্য {
  ধ্রুবক কাঠামো curltime *pnow;
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
স্থির পূর্ণসংখ্যা dnscache_entry_is_stale(শূন্য *datap, শূন্য *hc)
{
  কাঠামো dnscache_prune_তথ্য *prune = (কাঠামো dnscache_prune_তথ্য *)datap;
  কাঠামো Curl_dns_entry *dns = (কাঠামো Curl_dns_entry *)hc;

  যদি(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    নির্বাচন(dns->type) {
    ক্ষেত্র CURL_DNST_ADDR:
      যদি(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      বিরতি;
#ifdef USE_HTTPSRR
    ক্ষেত্র CURL_DNST_HTTPS:
      যদি(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      বিরতি;
#endif
    পূর্বনির্ধারিত:
      বিরতি;
    }
    যদি(age_ms >= prune->max_age_ms)
      ফেরত TRUE;
    যদি(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ফেরত FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
স্থির timediff_t dnscache_prune(কাঠামো Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো dnscache_prune_তথ্য user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (শূন্য *)&user,
                                 dnscache_entry_is_stale);

  ফেরত user.oldest_ms;
}

স্থির কাঠামো Curl_dnscache *dnscache_get(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->share && তথ্য->share->specifier & (1 << CURL_LOCK_তথ্য_DNS))
    ফেরত &তথ্য->share->dnscache;
  যদি(তথ্য->multi)
    ফেরত &তথ্য->multi->dnscache;
  ফেরত ফাঁকা;
}

স্থির শূন্য dnscache_lock(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_dnscache *dnscache)
{
  যদি(তথ্য->share && dnscache == &তথ্য->share->dnscache)
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_DNS, CURL_LOCK_ACCESS_SINGLE);
}

স্থির শূন্য dnscache_unlock(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_dnscache *dnscache)
{
  যদি(তথ্য->share && dnscache == &তথ্য->share->dnscache)
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
শূন্য Curl_dnscache_prune(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = তথ্য->set.dns_cache_timeout_ms;

  যদি(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ফেরত;

  dnscache_lock(তথ্য, dnscache);

  করো {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    যদি(Curl_hash_গুনতি(&dnscache->entries) > MAX_DNS_CACHE_আকার)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    অন্যথা
      বিরতি;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } যখন(timeout_ms);

  dnscache_unlock(তথ্য, dnscache);
}

শূন্য Curl_dnscache_clear(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  যদি(dnscache) {
    dnscache_lock(তথ্য, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(তথ্য, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
স্থির CURLcode fetch_entry(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            কাঠামো Curl_peer *peer,
                            কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো Curl_dns_entry *dns = ফাঁকা;
  কাঠামো dnsc_id id;
  কাঠামো dnsc_চাবি চাবি;
  অক্ষর type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode ফলাফল = CURLE_OK;

  *pdns = ফাঁকা;
  যদি(!dnscache)
    ফেরত CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&চাবি, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, চাবি.তথ্য, চাবি.len);

  যদি(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ফাঁকা;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  যদি(!dns && (type == CURL_DNST_ADDR) && তথ্য->state.wildcard_resolve) {
    কাঠামো Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&চাবি, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, চাবি.তথ্য, চাবি.len);
  }

  যদি(dns && (তথ্য->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    কাঠামো dnscache_prune_তথ্য user;

    user.pnow = Curl_pgrs_now(তথ্য);
    user.max_age_ms = তথ্য->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    যদি(dnscache_entry_is_stale(&user, dns)) {
      infof(তথ্য, "Hostname in DNS cache was stale, zapped");
      dns = ফাঁকা; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, চাবি.তথ্য, চাবি.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  যদি(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    যদি((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ফাঁকা;
    }
    অন্যথা যদি(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(তথ্য, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ফাঁকা;
      ফলাফল = CURLE_COULDNT_RESOLVE_HOST;
    }
    অন্যথা যদি(dns && !dns->addr) { /* negative entry */
      dns = ফাঁকা;
      ফলাফল = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ফেরত ফলাফল;
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
CURLcode Curl_dnscache_get(কাঠামো Curl_easy *তথ্য,
                           uint8_t dns_queries,
                           কাঠামো Curl_peer *peer,
                           কাঠামো Curl_dns_entry **pentry)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  কাঠামো Curl_dns_entry *dns = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  dnscache_lock(তথ্য, dnscache);
  ফলাফল = fetch_entry(তথ্য, dnscache, dns_queries, peer, &dns);
  যদি(!ফলাফল && dns)
    dns->refcount++; /* we pass out a reference */
  অন্যথা যদি(ফলাফল) {
    DEBUGASSERT(!dns);
    dns = ফাঁকা;
  }
  dnscache_unlock(তথ্য, dnscache);

  CURL_TRC_DNS(তথ্য, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (পূর্ণসংখ্যা)ফলাফল, dns ? "" : "not ");
  *pentry = dns;
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
স্থির পূর্ণসংখ্যা অঙ্ক_addresses(ধ্রুবক কাঠামো Curl_addrinfo *addr)
{
  পূর্ণসংখ্যা i = 0;
  যখন(addr) {
    addr = addr->ai_পরবর্তী;
    i++;
  }
  ফেরত i;
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
UNITTEST CURLcode dns_shuffle_addr(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_addrinfo **addr)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক পূর্ণসংখ্যা অঙ্ক_addrs = অঙ্ক_addresses(*addr);

  যদি(অঙ্ক_addrs > 1) {
    কাঠামো Curl_addrinfo **nodes;
    CURL_TRC_DNS(তথ্য, "Shuffling %d addresses", অঙ্ক_addrs);

    nodes = curlx_malloc(অঙ্ক_addrs * আকার(*nodes));
    যদি(nodes) {
      পূর্ণসংখ্যা i;
      unsigned পূর্ণসংখ্যা *rnd;
      ধ্রুবক size_t rnd_আকার = অঙ্ক_addrs * আকার(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      জন্য(i = 1; i < অঙ্ক_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_পরবর্তী;
      }

      rnd = curlx_malloc(rnd_আকার);
      যদি(rnd) {
        /* Fisher-Yates shuffle */
        যদি(Curl_rand(তথ্য, (unsigned অক্ষর *)rnd, rnd_আকার) == CURLE_OK) {
          কাঠামো Curl_addrinfo *swap_tmp;
          জন্য(i = অঙ্ক_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned পূর্ণসংখ্যা)(i + 1)];
            nodes[rnd[i] % (unsigned পূর্ণসংখ্যা)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          জন্য(i = 1; i < অঙ্ক_addrs; i++) {
            nodes[i - 1]->ai_পরবর্তী = nodes[i];
          }

          nodes[অঙ্ক_addrs - 1]->ai_পরবর্তী = ফাঁকা;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      অন্যথা
        ফলাফল = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    অন্যথা
      ফলাফল = CURLE_OUT_OF_MEMORY;
  }
  ফেরত ফলাফল;
}
#endif

স্থির bool dnscache_ai_has_family(কাঠামো Curl_addrinfo *ai, পূর্ণসংখ্যা ai_family)
{
  জন্য(; ai; ai = ai->ai_পরবর্তী) {
    যদি(ai->ai_family == ai_family)
      ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির কাঠামো Curl_dns_entry *dnsc_entry_create(কাঠামো dnsc_id *pid,
                                                bool permanent)
{
  কাঠামো Curl_dns_entry *dns = ফাঁকা;

  যদি(curlx_strlen(&pid->name) > UINT16_MAX)
    যাও out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, আকার(কাঠামো Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  যদি(!dns)
    যাও out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  যদি(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  ফেরত dns;
}

স্থির কাঠামো Curl_dns_entry *dnsc_entry_assign_addr(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো Curl_dns_entry *dns,
  uint8_t dns_queries,
  কাঠামো Curl_addrinfo **paddr1,
  কাঠামো Curl_addrinfo **paddr2)
{
  যদি(!dns)
    যাও out;
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
    *paddr1 = ফাঁকা;
  }
  যদি(paddr2 && *paddr2) {
    কাঠামো Curl_addrinfo **phead = &dns->addr;
    যখন(*phead)
      phead = &(*phead)->ai_পরবর্তী;
    *phead = *paddr2;
    *paddr2 = ফাঁকা;
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
  যদি(তথ্য->set.dns_shuffle_addresses && dns->addr) {
    CURLcode ফলাফল = dns_shuffle_addr(তথ্য, &dns->addr);
    যদি(ফলাফল) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = ফাঁকা;
      যাও out;
    }
  }
#অন্যথা
  (শূন্য)তথ্য;
#endif

out:
  যদি(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ফাঁকা;
  }
  যদি(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ফাঁকা;
  }
  ফেরত dns;
}

কাঠামো Curl_dns_entry *Curl_dnsc_mk_addr(কাঠামো Curl_easy *তথ্য,
                                         uint8_t dns_queries,
                                         কাঠামো Curl_addrinfo **paddr,
                                         কাঠামো Curl_peer *peer)
{
  কাঠামো dnsc_id id;
  কাঠামো Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(তথ্য, dns, dns_queries, paddr, ফাঁকা);
  ফেরত dns;
}

কাঠামো Curl_dns_entry *Curl_dnsc_mk_addr2(কাঠামো Curl_easy *তথ্য,
                                          uint8_t dns_queries,
                                          কাঠামো Curl_addrinfo **paddr1,
                                          কাঠামো Curl_addrinfo **paddr2,
                                          কাঠামো Curl_peer *peer)
{
  কাঠামো dnsc_id id;
  কাঠামো Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(তথ্য, dns, dns_queries, paddr1, paddr2);
  ফেরত dns;
}

স্থির কাঠামো Curl_dns_entry *dnsc_add_entry(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো Curl_dnscache *dnscache,
                                             কাঠামো dnsc_চাবি *চাবি,
                                             কাঠামো Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(তথ্য);
  ফেরত Curl_hash_add(&dnscache->entries, চাবি->তথ্য, চাবি->len, entry);
}

#ifdef USE_HTTPSRR
স্থির কাঠামো Curl_dns_entry *dnsc_entry_assign_https(
  কাঠামো Curl_dns_entry *dns,
  কাঠামো Curl_https_rrinfo **phinfo)
{
  যদি(!dns)
    যাও out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  যদি(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ফাঁকা;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  যদি(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ফাঁকা;
  }
out:
  যদি(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ফাঁকা;
  }
  ফেরত dns;
}

কাঠামো Curl_dns_entry *Curl_dnsc_mk_https(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_https_rrinfo **phinfo,
                                          কাঠামো Curl_peer *peer)
{
  কাঠামো dnsc_id id;
  কাঠামো Curl_dns_entry *dns;

  (শূন্য)তথ্য;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ফেরত dns;
}

স্থির কাঠামো Curl_dns_entry *dnsc_add_https(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো Curl_dnscache *dnscache,
                                             কাঠামো Curl_https_rrinfo **phinfo,
                                             কাঠামো dnsc_id *id)
{
  কাঠামো Curl_dns_entry *dns, *dns2;
  কাঠামো dnsc_চাবি চাবি;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  যদি(!dns)
    ফেরত ফাঁকা;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&চাবি, id);
  dns2 = dnsc_add_entry(তথ্য, dnscache, &চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ফেরত ফাঁকা;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ফেরত dns;
}
#endif /* USE_HTTPSRR */

স্থির কাঠামো Curl_dns_entry *dnsc_add_addr(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            কাঠামো Curl_addrinfo **paddr,
                                            কাঠামো dnsc_id *id,
                                            কাঠামো dnsc_চাবি *চাবি,
                                            bool permanent)
{
  কাঠামো Curl_dns_entry *dns;
  কাঠামো Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(তথ্য, dns, dns_queries, paddr, ফাঁকা);
  যদি(!dns)
    ফেরত ফাঁকা;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(তথ্য, dnscache, চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ফেরত ফাঁকা;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ফেরত dns;
}

স্থির কাঠামো Curl_dns_entry *dnsc_add_peer_addr(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো Curl_dnscache *dnscache,
  uint8_t dns_queries,
  কাঠামো Curl_addrinfo **paddr,
  কাঠামো dnsc_id *id,
  bool permanent)
{
  কাঠামো Curl_dns_entry *dns;
  কাঠামো Curl_dns_entry *dns2;
  কাঠামো dnsc_চাবি চাবি;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(তথ্য, dns, dns_queries, paddr, ফাঁকা);
  যদি(!dns)
    ফেরত ফাঁকা;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&চাবি, id);
  dns2 = dnsc_add_entry(তথ্য, dnscache, &চাবি, dns);
  যদি(!dns2) {
    dnscache_entry_free(dns);
    ফেরত ফাঁকা;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ফেরত dns;
}

CURLcode Curl_dnscache_add(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_dns_entry *entry)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  কাঠামো Curl_str name;
  কাঠামো dnsc_id id;
  কাঠামো dnsc_চাবি চাবি;

  যদি(!dnscache)
    ফেরত CURLE_FAILED_INIT;
  যদি(!entry || (entry->type == CURL_DNST_INIT))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&চাবি, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(তথ্য, dnscache);
  যদি(!dnsc_add_entry(তথ্য, dnscache, &চাবি, entry)) {
    dnscache_unlock(তথ্য, dnscache);
    ফেরত CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(তথ্য, dnscache);
  CURL_TRC_DNS(তথ্য, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  ফেরত CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(কাঠামো Curl_easy *তথ্য,
                                    uint8_t dns_queries,
                                    কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  কাঠামো Curl_dns_entry *dns = ফাঁকা;
  কাঠামো dnsc_id id;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(dnscache);
  যদি(!dnscache)
    ফেরত CURLE_FAILED_INIT;

  dnscache_lock(তথ্য, dnscache);

  যদি(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(তথ্য, dnscache, dns_queries, ফাঁকা, &id, FALSE);
    যদি(!dns)
      ফলাফল = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(তথ্য, dnscache, ফাঁকা, &id);
    যদি(!dns)
      ফলাফল = CURLE_OUT_OF_MEMORY;
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
    CURL_TRC_DNS(তথ্য, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(তথ্য, dnscache);
  ফেরত ফলাফল;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
শূন্য Curl_dns_entry_unlink(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_dns_entry **pdns)
{
  যদি(*pdns) {
    কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
    কাঠামো Curl_dns_entry *dns = *pdns;
    *pdns = ফাঁকা;
    dnscache_lock(তথ্য, dnscache);
    dns->refcount--;
    যদি(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(তথ্য, dnscache);
  }
}

স্থির শূন্য dnscache_entry_dtor(শূন্য *entry)
{
  কাঠামো Curl_dns_entry *dns = (কাঠামো Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  যদি(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
শূন্য Curl_dnscache_init(কাঠামো Curl_dnscache *dns, size_t আকার_2)
{
  Curl_hash_init(&dns->entries, আকার_2, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

শূন্য Curl_dnscache_destroy(কাঠামো Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_dnscache *dnscache = dnscache_get(তথ্য);
  কাঠামো curl_slist *hostp;
  কাঠামো dnsc_id id;
  কাঠামো dnsc_চাবি চাবি;

  যদি(!dnscache)
    ফেরত CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  তথ্য->state.wildcard_resolve = FALSE;

  জন্য(hostp = তথ্য->state.resolve; hostp; hostp = hostp->পরবর্তী) {
    ধ্রুবক অক্ষর *host = hostp->তথ্য;
    কাঠামো Curl_str source;
    যদি(!host)
      চালু;
    যদি(*host == '-') {
      curl_off_t অঙ্ক = 0;
      host++;
      যদি(!curlx_str_single(&host, '[')) {
        যদি(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          চালু;
      }
      অন্যথা {
        যদি(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          চালু;
        }
      }

      যদি(!curlx_str_সংখ্যা(&host, &অঙ্ক, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)অঙ্ক);
        dnsc_id2key(&চাবি, &id);
        dnscache_lock(তথ্য, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, চাবি.তথ্য, চাবি.len);
        dnscache_unlock(তথ্য, dnscache);
      }
    }
    অন্যথা {
      কাঠামো Curl_dns_entry *dns;
      কাঠামো Curl_addrinfo *শীর্ষ = ফাঁকা, *লেজ = ফাঁকা;
      অক্ষর address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ধ্রুবক অক্ষর *addresses = ফাঁকা);

      যদি(*host == '+') {
        host++;
        permanent = FALSE;
      }
      যদি(!curlx_str_single(&host, '[')) {
        যদি(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          চালু;
      }
      অন্যথা {
        যদি(curlx_str_until(&host, &source, 4096, ':'))
          চালু;
      }
      যদি(curlx_str_single(&host, ':') ||
         curlx_str_সংখ্যা(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        যাও err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      যখন(*host) {
        কাঠামো Curl_str target;
        কাঠামো Curl_addrinfo *ai;
        CURLcode ফলাফল;

        যদি(!curlx_str_single(&host, '[')) {
          যদি(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            যাও err;
        }
        অন্যথা {
          যদি(curlx_str_until(&host, &target, 4096, ',')) {
            যদি(curlx_str_single(&host, ','))
              যাও err;
            /* survive nothing but a comma */
            চালু;
          }
        }
#ifndef USE_IPV6
        যদি(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(তথ্য, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (পূর্ণসংখ্যা)curlx_strlen(&target), curlx_str(&target));
          যদি(curlx_str_single(&host, ','))
            যাও err;
          চালু;
        }
#endif

        যদি(curlx_strlen(&target) >= আকার(address))
          যাও err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        ফলাফল = Curl_str2addr(address, port, &ai);
        যদি(ফলাফল) {
          infof(তথ্য, "Resolve IP address '%s' found is illegal", address);
          যাও err;
        }

        যদি(লেজ) {
          লেজ->ai_পরবর্তী = ai;
          লেজ = লেজ->ai_পরবর্তী;
        }
        অন্যথা {
          শীর্ষ = লেজ = ai;
        }
        যদি(curlx_str_single(&host, ','))
          বিরতি;
      }

      যদি(!শীর্ষ)
        যাও err;

      error = FALSE;
err:
      যদি(error) {
        failf(তথ্য, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->তথ্য);
        Curl_freeaddrinfo(শীর্ষ);
        ফেরত CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&চাবি, &id);
      dnscache_lock(তথ্য, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, চাবি.তথ্য, চাবি.len);

      যদি(dns) {
        infof(তথ্য, "RESOLVE %.*s:%u - old addresses discarded",
              (পূর্ণসংখ্যা)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, চাবি.তথ্য, চাবি.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(তথ্য, dnscache, CURL_DNSQ_ADDR, &শীর্ষ,
                          &id, &চাবি, permanent);
      যদি(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(তথ্য, dnscache);

      যদি(!dns)
        ফেরত CURLE_OUT_OF_MEMORY;

      infof(তথ্য, "[DNS] added %.*s:%u:%s to cache%s",
            (পূর্ণসংখ্যা)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      যদি(curlx_str_casecompare(&source, "*")) {
        infof(তথ্য, "RESOLVE *:%u using wildcard", port);
        তথ্য->state.wildcard_resolve = TRUE;
      }
    }
  }
  তথ্য->state.resolve = ফাঁকা; /* dealt with now */

  ফেরত CURLE_OK;
}
