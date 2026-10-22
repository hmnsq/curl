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

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IN6_H
#అన్తర్భావ <netinet/in6.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "hash.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "vdns/dnscache.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strcopy.h"
#అన్తర్భావ "curlx/strparse.h"

#నిర్వచించు MAX_DNS_CACHE_కొలత 29999

/* @unittest 3217 */
UNITTEST శూన్య dnsc_peer2id(నిర్మాణం dnsc_id *pid, అక్షరం type,
                           నిర్మాణం Curl_peer *peer);
UNITTEST శూన్య dnsc_peer2id(నిర్మాణం dnsc_id *pid, అక్షరం type,
                           నిర్మాణం Curl_peer *peer)
{
  curlx_str_కేటాయించు(&pid->name, peer->hostname, అక్షరశ్రేణి_పొడవు(peer->hostname));
  pid->పోర్టు = peer->పోర్టు;
  pid->type = type;
}

స్థిర శూన్య dnsc_str2id(నిర్మాణం dnsc_id *pid, అక్షరం type,
                        నిర్మాణం Curl_str *name, uint16_t పోర్టు)
{
  pid->name = *name;
  pid->పోర్టు = పోర్టు;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST శూన్య dnsc_id2key(నిర్మాణం dnsc_కీలకం *కీలకం, నిర్మాణం dnsc_id *id);
UNITTEST శూన్య dnsc_id2key(నిర్మాణం dnsc_కీలకం *కీలకం, నిర్మాణం dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  ఒకవేళ(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  కీలకం->దత్తాంశం[0] = id->type;
  కీలకం->దత్తాంశం[1] = (uint8_t)((id->పోర్టు >> 8) & 0xff);
  కీలకం->దత్తాంశం[2] = (uint8_t)(id->పోర్టు & 0xff);
  Curl_strntolower((అక్షరం *)కీలకం->దత్తాంశం + 3, curlx_str(&id->name), namelen);
  కీలకం->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_నమోదు(నిర్మాణం dnsc_id *id,
                                    నిర్మాణం Curl_dns_నమోదు *dns);
UNITTEST bool dnsc_id_matches_నమోదు(నిర్మాణం dnsc_id *id,
                                    నిర్మాణం Curl_dns_నమోదు *dns)
{
  తిరిగి dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->పోర్టు == dns->పోర్టు) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

స్థిర శూన్య dnscache_నమోదు_free(నిర్మాణం Curl_dns_నమోదు *dns)
{
  Curl_freeaddrinfo(dns->addr);
#నిర్వచితమైతే USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#అయితే_ముగింపు
  curlx_free(dns);
}

నిర్మాణం dnscache_prune_దత్తాంశం {
  స్థిరరాశి నిర్మాణం curltime *pnow;
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
స్థిర పూర్ణాంకం dnscache_నమోదు_is_stale(శూన్య *datap, శూన్య *hc)
{
  నిర్మాణం dnscache_prune_దత్తాంశం *prune = (నిర్మాణం dnscache_prune_దత్తాంశం *)datap;
  నిర్మాణం Curl_dns_నమోదు *dns = (నిర్మాణం Curl_dns_నమోదు *)hc;

  ఒకవేళ(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ఎంపిక(dns->type) {
    స్థితి CURL_DNST_ADDR:
      ఒకవేళ(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ఆపు;
#నిర్వచితమైతే USE_HTTPSRR
    స్థితి CURL_DNST_HTTPS:
      ఒకవేళ(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
      ఆపు;
    }
    ఒకవేళ(age_ms >= prune->max_age_ms)
      తిరిగి TRUE;
    ఒకవేళ(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  తిరిగి FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
స్థిర timediff_t dnscache_prune(నిర్మాణం Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం dnscache_prune_దత్తాంశం user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (శూన్య *)&user,
                                 dnscache_నమోదు_is_stale);

  తిరిగి user.oldest_ms;
}

స్థిర నిర్మాణం Curl_dnscache *dnscache_get(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->share && దత్తాంశం->share->specifier & (1 << CURL_తాళం_దత్తాంశం_DNS))
    తిరిగి &దత్తాంశం->share->dnscache;
  ఒకవేళ(దత్తాంశం->multi)
    తిరిగి &దత్తాంశం->multi->dnscache;
  తిరిగి ఖాళీ;
}

స్థిర శూన్య dnscache_తాళం(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_dnscache *dnscache)
{
  ఒకవేళ(దత్తాంశం->share && dnscache == &దత్తాంశం->share->dnscache)
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_DNS, CURL_తాళం_ACCESS_SINGLE);
}

స్థిర శూన్య dnscache_unlock(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_dnscache *dnscache)
{
  ఒకవేళ(దత్తాంశం->share && dnscache == &దత్తాంశం->share->dnscache)
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
శూన్య Curl_dnscache_prune(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = దత్తాంశం->సమితి.dns_cache_timeout_ms;

  ఒకవేళ(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    తిరిగి;

  dnscache_తాళం(దత్తాంశం, dnscache);

  చేయి {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    ఒకవేళ(Curl_hash_లెక్క(&dnscache->entries) > MAX_DNS_CACHE_కొలత)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    లేకపోతే
      ఆపు;

    /* if the cache size is still too big, use the oldest age as new prune
హద్దు*/
  } అయితే(timeout_ms);

  dnscache_unlock(దత్తాంశం, dnscache);
}

శూన్య Curl_dnscache_clear(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  ఒకవేళ(dnscache) {
    dnscache_తాళం(దత్తాంశం, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(దత్తాంశం, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
స్థిర CURLcode fetch_నమోదు(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            నిర్మాణం Curl_peer *peer,
                            నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  నిర్మాణం dnsc_id id;
  నిర్మాణం dnsc_కీలకం కీలకం;
  అక్షరం type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode ఫలితం = CURLE_OK;

  *pdns = ఖాళీ;
  ఒకవేళ(!dnscache)
    తిరిగి CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&కీలకం, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);

  ఒకవేళ(dns && !dnsc_id_matches_నమోదు(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ఖాళీ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  ఒకవేళ(!dns && (type == CURL_DNST_ADDR) && దత్తాంశం->state.wildcard_resolve) {
    నిర్మాణం Curl_str wildname;

    curlx_str_కేటాయించు(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->పోర్టు);
    dnsc_id2key(&కీలకం, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);
  }

  ఒకవేళ(dns && (దత్తాంశం->సమితి.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    నిర్మాణం dnscache_prune_దత్తాంశం user;

    user.pnow = Curl_pgrs_now(దత్తాంశం);
    user.max_age_ms = దత్తాంశం->సమితి.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    ఒకవేళ(dnscache_నమోదు_is_stale(&user, dns)) {
      infof(దత్తాంశం, "Hostname in DNS cache was stale, zapped");
      dns = ఖాళీ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  ఒకవేళ(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    ఒకవేళ((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ఖాళీ;
    }
    లేకపోతే ఒకవేళ(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(దత్తాంశం, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ఖాళీ;
      ఫలితం = CURLE_COULDNT_RESOLVE_HOST;
    }
    లేకపోతే ఒకవేళ(dns && !dns->addr) { /* negative entry */
      dns = ఖాళీ;
      ఫలితం = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  తిరిగి ఫలితం;
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
CURLcode Curl_dnscache_get(నిర్మాణం Curl_easy *దత్తాంశం,
                           uint8_t dns_queries,
                           నిర్మాణం Curl_peer *peer,
                           నిర్మాణం Curl_dns_నమోదు **pentry)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  dnscache_తాళం(దత్తాంశం, dnscache);
  ఫలితం = fetch_నమోదు(దత్తాంశం, dnscache, dns_queries, peer, &dns);
  ఒకవేళ(!ఫలితం && dns)
    dns->refcount++; /* we pass out a reference */
  లేకపోతే ఒకవేళ(ఫలితం) {
    DEBUGASSERT(!dns);
    dns = ఖాళీ;
  }
  dnscache_unlock(దత్తాంశం, dnscache);

  CURL_TRC_DNS(దత్తాంశం, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->పోర్టు, Curl_resolv_query_str(dns_queries),
               (పూర్ణాంకం)ఫలితం, dns ? "" : "not ");
  *pentry = dns;
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
స్థిర పూర్ణాంకం అంకె_చిరునామాలు(స్థిరరాశి నిర్మాణం Curl_addrinfo *addr)
{
  పూర్ణాంకం i = 0;
  అయితే(addr) {
    addr = addr->ai_తదుపరి;
    i++;
  }
  తిరిగి i;
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
UNITTEST CURLcode dns_shuffle_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_addrinfo **addr)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి పూర్ణాంకం అంకె_addrs = అంకె_చిరునామాలు(*addr);

  ఒకవేళ(అంకె_addrs > 1) {
    నిర్మాణం Curl_addrinfo **nodes;
    CURL_TRC_DNS(దత్తాంశం, "Shuffling %d addresses", అంకె_addrs);

    nodes = curlx_malloc(అంకె_addrs * పరిమాణం(*nodes));
    ఒకవేళ(nodes) {
      పూర్ణాంకం i;
      గుర్తులేని పూర్ణాంకం *rnd;
      స్థిరరాశి size_t rnd_కొలత = అంకె_addrs * పరిమాణం(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      కొరకు(i = 1; i < అంకె_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_తదుపరి;
      }

      rnd = curlx_malloc(rnd_కొలత);
      ఒకవేళ(rnd) {
        /* Fisher-Yates shuffle */
        ఒకవేళ(Curl_rand(దత్తాంశం, (గుర్తులేని అక్షరం *)rnd, rnd_కొలత) == CURLE_OK) {
          నిర్మాణం Curl_addrinfo *మార్పిడి_tmp;
          కొరకు(i = అంకె_addrs - 1; i > 0; i--) {
            మార్పిడి_tmp = nodes[rnd[i] % (గుర్తులేని పూర్ణాంకం)(i + 1)];
            nodes[rnd[i] % (గుర్తులేని పూర్ణాంకం)(i + 1)] = nodes[i];
            nodes[i] = మార్పిడి_tmp;
          }

          /* relink list in the new order */
          కొరకు(i = 1; i < అంకె_addrs; i++) {
            nodes[i - 1]->ai_తదుపరి = nodes[i];
          }

          nodes[అంకె_addrs - 1]->ai_తదుపరి = ఖాళీ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      లేకపోతే
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      curlx_free(nodes);
    }
    లేకపోతే
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

స్థిర bool dnscache_ai_has_family(నిర్మాణం Curl_addrinfo *ai, పూర్ణాంకం ai_family)
{
  కొరకు(; ai; ai = ai->ai_తదుపరి) {
    ఒకవేళ(ai->ai_family == ai_family)
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_నమోదు_create(నిర్మాణం dnsc_id *pid,
                                                bool permanent)
{
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;

  ఒకవేళ(curlx_strlen(&pid->name) > UINT16_MAX)
    వెళ్ళు బహిర్గతం;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_dns_నమోదు) +
                     curlx_strlen(&pid->name));
  ఒకవేళ(!dns)
    వెళ్ళు బహిర్గతం;

  dns->refcount = 1; /* the cache has the first reference */
  dns->పోర్టు = pid->పోర్టు;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  ఒకవేళ(dns->hostlen)
    జ్ఞాపకం_నకలు(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

బహిర్గతం:
  తిరిగి dns;
}

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_నమోదు_కేటాయించు_addr(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం Curl_dns_నమోదు *dns,
  uint8_t dns_queries,
  నిర్మాణం Curl_addrinfo **paddr1,
  నిర్మాణం Curl_addrinfo **paddr2)
{
  ఒకవేళ(!dns)
    వెళ్ళు బహిర్గతం;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  ఒకవేళ(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ఖాళీ;
  }
  ఒకవేళ(paddr2 && *paddr2) {
    నిర్మాణం Curl_addrinfo **phead = &dns->addr;
    అయితే(*phead)
      phead = &(*phead)->ai_తదుపరి;
    *phead = *paddr2;
    *paddr2 = ఖాళీ;
  }

  ఒకవేళ((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#అయితే_ముగింపు /* USE_IPV6 */

#నిర్వచితం_కాకపోతే CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  ఒకవేళ(దత్తాంశం->సమితి.dns_shuffle_చిరునామాలు && dns->addr) {
    CURLcode ఫలితం = dns_shuffle_addr(దత్తాంశం, &dns->addr);
    ఒకవేళ(ఫలితం) {
      /* free without lock, we are the sole owner */
      dnscache_నమోదు_free(dns);
      dns = ఖాళీ;
      వెళ్ళు బహిర్గతం;
    }
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు

బహిర్గతం:
  ఒకవేళ(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ఖాళీ;
  }
  ఒకవేళ(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ఖాళీ;
  }
  తిరిగి dns;
}

నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                                         uint8_t dns_queries,
                                         నిర్మాణం Curl_addrinfo **paddr,
                                         నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం dnsc_id id;
  నిర్మాణం Curl_dns_నమోదు *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_నమోదు_create(&id, FALSE);
  dns = dnsc_నమోదు_కేటాయించు_addr(దత్తాంశం, dns, dns_queries, paddr, ఖాళీ);
  తిరిగి dns;
}

నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_addr2(నిర్మాణం Curl_easy *దత్తాంశం,
                                          uint8_t dns_queries,
                                          నిర్మాణం Curl_addrinfo **paddr1,
                                          నిర్మాణం Curl_addrinfo **paddr2,
                                          నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం dnsc_id id;
  నిర్మాణం Curl_dns_నమోదు *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_నమోదు_create(&id, FALSE);
  dns = dnsc_నమోదు_కేటాయించు_addr(దత్తాంశం, dns, dns_queries, paddr1, paddr2);
  తిరిగి dns;
}

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_కూడు_నమోదు(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_dnscache *dnscache,
                                             నిర్మాణం dnsc_కీలకం *కీలకం,
                                             నిర్మాణం Curl_dns_నమోదు *నమోదు_2)
{
  నమోదు_2->added = *Curl_pgrs_now(దత్తాంశం);
  తిరిగి Curl_hash_కూడు(&dnscache->entries, కీలకం->దత్తాంశం, కీలకం->len, నమోదు_2);
}

#నిర్వచితమైతే USE_HTTPSRR
స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_నమోదు_కేటాయించు_https(
  నిర్మాణం Curl_dns_నమోదు *dns,
  నిర్మాణం Curl_https_rrinfo **phinfo)
{
  ఒకవేళ(!dns)
    వెళ్ళు బహిర్గతం;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  ఒకవేళ(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ఖాళీ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  ఒకవేళ(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ఖాళీ;
  }
బహిర్గతం:
  ఒకవేళ(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ఖాళీ;
  }
  తిరిగి dns;
}

నిర్మాణం Curl_dns_నమోదు *Curl_dnsc_mk_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_https_rrinfo **phinfo,
                                          నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం dnsc_id id;
  నిర్మాణం Curl_dns_నమోదు *dns;

  (శూన్య)దత్తాంశం;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_నమోదు_create(&id, FALSE);
  dns = dnsc_నమోదు_కేటాయించు_https(dns, phinfo);
  తిరిగి dns;
}

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_కూడు_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_dnscache *dnscache,
                                             నిర్మాణం Curl_https_rrinfo **phinfo,
                                             నిర్మాణం dnsc_id *id)
{
  నిర్మాణం Curl_dns_నమోదు *dns, *dns2;
  నిర్మాణం dnsc_కీలకం కీలకం;

  dns = dnsc_నమోదు_create(id, FALSE);
  dns = dnsc_నమోదు_కేటాయించు_https(dns, phinfo);
  ఒకవేళ(!dns)
    తిరిగి ఖాళీ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&కీలకం, id);
  dns2 = dnsc_కూడు_నమోదు(దత్తాంశం, dnscache, &కీలకం, dns);
  ఒకవేళ(!dns2) {
    dnscache_నమోదు_free(dns);
    తిరిగి ఖాళీ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  తిరిగి dns;
}
#అయితే_ముగింపు /* USE_HTTPSRR */

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_కూడు_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            నిర్మాణం Curl_addrinfo **paddr,
                                            నిర్మాణం dnsc_id *id,
                                            నిర్మాణం dnsc_కీలకం *కీలకం,
                                            bool permanent)
{
  నిర్మాణం Curl_dns_నమోదు *dns;
  నిర్మాణం Curl_dns_నమోదు *dns2;

  dns = dnsc_నమోదు_create(id, permanent);
  dns = dnsc_నమోదు_కేటాయించు_addr(దత్తాంశం, dns, dns_queries, paddr, ఖాళీ);
  ఒకవేళ(!dns)
    తిరిగి ఖాళీ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_కూడు_నమోదు(దత్తాంశం, dnscache, కీలకం, dns);
  ఒకవేళ(!dns2) {
    dnscache_నమోదు_free(dns);
    తిరిగి ఖాళీ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  తిరిగి dns;
}

స్థిర నిర్మాణం Curl_dns_నమోదు *dnsc_కూడు_peer_addr(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం Curl_dnscache *dnscache,
  uint8_t dns_queries,
  నిర్మాణం Curl_addrinfo **paddr,
  నిర్మాణం dnsc_id *id,
  bool permanent)
{
  నిర్మాణం Curl_dns_నమోదు *dns;
  నిర్మాణం Curl_dns_నమోదు *dns2;
  నిర్మాణం dnsc_కీలకం కీలకం;

  dns = dnsc_నమోదు_create(id, permanent);
  dns = dnsc_నమోదు_కేటాయించు_addr(దత్తాంశం, dns, dns_queries, paddr, ఖాళీ);
  ఒకవేళ(!dns)
    తిరిగి ఖాళీ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&కీలకం, id);
  dns2 = dnsc_కూడు_నమోదు(దత్తాంశం, dnscache, &కీలకం, dns);
  ఒకవేళ(!dns2) {
    dnscache_నమోదు_free(dns);
    తిరిగి ఖాళీ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  తిరిగి dns;
}

CURLcode Curl_dnscache_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_dns_నమోదు *నమోదు_2)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  నిర్మాణం Curl_str name;
  నిర్మాణం dnsc_id id;
  నిర్మాణం dnsc_కీలకం కీలకం;

  ఒకవేళ(!dnscache)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!నమోదు_2 || (నమోదు_2->type == CURL_DNST_INIT))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  curlx_str_కేటాయించు(&name, నమోదు_2->hostname, నమోదు_2->hostlen);
  dnsc_str2id(&id, నమోదు_2->type, &name, నమోదు_2->పోర్టు);
  dnsc_id2key(&కీలకం, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_తాళం(దత్తాంశం, dnscache);
  ఒకవేళ(!dnsc_కూడు_నమోదు(దత్తాంశం, dnscache, &కీలకం, నమోదు_2)) {
    dnscache_unlock(దత్తాంశం, dnscache);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  నమోదు_2->refcount++;
  dnscache_unlock(దత్తాంశం, dnscache);
  CURL_TRC_DNS(దత్తాంశం, "cached entry for %s:%u queries=%s",
               నమోదు_2->hostname, నమోదు_2->పోర్టు,
               Curl_resolv_query_str(నమోదు_2->dns_queries));
  తిరిగి CURLE_OK;
}

CURLcode Curl_dnscache_కూడు_negative(నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t dns_queries,
                                    నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  నిర్మాణం dnsc_id id;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(dnscache);
  ఒకవేళ(!dnscache)
    తిరిగి CURLE_FAILED_INIT;

  dnscache_తాళం(దత్తాంశం, dnscache);

  ఒకవేళ(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_కూడు_peer_addr(దత్తాంశం, dnscache, dns_queries, ఖాళీ, &id, FALSE);
    ఒకవేళ(!dns)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#నిర్వచితమైతే USE_HTTPSRR
  లేకపోతే ఒకవేళ(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_కూడు_https(దత్తాంశం, dnscache, ఖాళీ, &id);
    ఒకవేళ(!dns)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#అయితే_ముగింపు
  లేకపోతే {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  ఒకవేళ(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(దత్తాంశం, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->పోర్టు,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(దత్తాంశం, dnscache);
  తిరిగి ఫలితం;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
శూన్య Curl_dns_నమోదు_unlink(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_dns_నమోదు **pdns)
{
  ఒకవేళ(*pdns) {
    నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
    నిర్మాణం Curl_dns_నమోదు *dns = *pdns;
    *pdns = ఖాళీ;
    dnscache_తాళం(దత్తాంశం, dnscache);
    dns->refcount--;
    ఒకవేళ(dns->refcount == 0)
      dnscache_నమోదు_free(dns);
    dnscache_unlock(దత్తాంశం, dnscache);
  }
}

స్థిర శూన్య dnscache_నమోదు_dtor(శూన్య *నమోదు_2)
{
  నిర్మాణం Curl_dns_నమోదు *dns = (నిర్మాణం Curl_dns_నమోదు *)నమోదు_2;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  ఒకవేళ(dns->refcount == 0)
    dnscache_నమోదు_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
శూన్య Curl_dnscache_init(నిర్మాణం Curl_dnscache *dns, size_t కొలత)
{
  Curl_hash_init(&dns->entries, కొలత, CURL_HASH_TYPE_బైట్లు,
                 dnscache_నమోదు_dtor);
}

శూన్య Curl_dnscache_destroy(నిర్మాణం Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_dnscache *dnscache = dnscache_get(దత్తాంశం);
  నిర్మాణం curl_slist *hostp;
  నిర్మాణం dnsc_id id;
  నిర్మాణం dnsc_కీలకం కీలకం;

  ఒకవేళ(!dnscache)
    తిరిగి CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  దత్తాంశం->state.wildcard_resolve = FALSE;

  కొరకు(hostp = దత్తాంశం->state.resolve; hostp; hostp = hostp->తదుపరి) {
    స్థిరరాశి అక్షరం *host = hostp->దత్తాంశం;
    నిర్మాణం Curl_str ఆధారం;
    ఒకవేళ(!host)
      కొనసాగు;
    ఒకవేళ(*host == '-') {
      curl_off_t అంకె = 0;
      host++;
      ఒకవేళ(!curlx_str_single(&host, '[')) {
        ఒకవేళ(curlx_str_until(&host, &ఆధారం, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          కొనసాగు;
      }
      లేకపోతే {
        ఒకవేళ(curlx_str_until(&host, &ఆధారం, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          కొనసాగు;
        }
      }

      ఒకవేళ(!curlx_str_సంఖ్య(&host, &అంకె, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &ఆధారం, (uint16_t)అంకె);
        dnsc_id2key(&కీలకం, &id);
        dnscache_తాళం(దత్తాంశం, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);
        dnscache_unlock(దత్తాంశం, dnscache);
      }
    }
    లేకపోతే {
      నిర్మాణం Curl_dns_నమోదు *dns;
      నిర్మాణం Curl_addrinfo *తల = ఖాళీ, *తోక = ఖాళీ;
      అక్షరం చిరునామా[64];
      curl_off_t tmpofft = 0;
      uint16_t పోర్టు = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(స్థిరరాశి అక్షరం *చిరునామాలు = ఖాళీ);

      ఒకవేళ(*host == '+') {
        host++;
        permanent = FALSE;
      }
      ఒకవేళ(!curlx_str_single(&host, '[')) {
        ఒకవేళ(curlx_str_until(&host, &ఆధారం, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          కొనసాగు;
      }
      లేకపోతే {
        ఒకవేళ(curlx_str_until(&host, &ఆధారం, 4096, ':'))
          కొనసాగు;
      }
      ఒకవేళ(curlx_str_single(&host, ':') ||
         curlx_str_సంఖ్య(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        వెళ్ళు err;
      పోర్టు = (uint16_t)tmpofft;

      VERBOSE(చిరునామాలు = host);

      /* start the address section */
      అయితే(*host) {
        నిర్మాణం Curl_str లక్ష్యం;
        నిర్మాణం Curl_addrinfo *ai;
        CURLcode ఫలితం;

        ఒకవేళ(!curlx_str_single(&host, '[')) {
          ఒకవేళ(curlx_str_until(&host, &లక్ష్యం, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            వెళ్ళు err;
        }
        లేకపోతే {
          ఒకవేళ(curlx_str_until(&host, &లక్ష్యం, 4096, ',')) {
            ఒకవేళ(curlx_str_single(&host, ','))
              వెళ్ళు err;
            /* survive nothing but a comma */
            కొనసాగు;
          }
        }
#నిర్వచితం_కాకపోతే USE_IPV6
        ఒకవేళ(memchr(curlx_str(&లక్ష్యం), ':', curlx_strlen(&లక్ష్యం))) {
          infof(దత్తాంశం, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (పూర్ణాంకం)curlx_strlen(&లక్ష్యం), curlx_str(&లక్ష్యం));
          ఒకవేళ(curlx_str_single(&host, ','))
            వెళ్ళు err;
          కొనసాగు;
        }
#అయితే_ముగింపు

        ఒకవేళ(curlx_strlen(&లక్ష్యం) >= పరిమాణం(చిరునామా))
          వెళ్ళు err;

        జ్ఞాపకం_నకలు(చిరునామా, curlx_str(&లక్ష్యం), curlx_strlen(&లక్ష్యం));
        చిరునామా[curlx_strlen(&లక్ష్యం)] = '\0';

        ఫలితం = Curl_str2addr(చిరునామా, పోర్టు, &ai);
        ఒకవేళ(ఫలితం) {
          infof(దత్తాంశం, "Resolve IP address '%s' found is illegal", చిరునామా);
          వెళ్ళు err;
        }

        ఒకవేళ(తోక) {
          తోక->ai_తదుపరి = ai;
          తోక = తోక->ai_తదుపరి;
        }
        లేకపోతే {
          తల = తోక = ai;
        }
        ఒకవేళ(curlx_str_single(&host, ','))
          ఆపు;
      }

      ఒకవేళ(!తల)
        వెళ్ళు err;

      error = FALSE;
err:
      ఒకవేళ(error) {
        failf(దత్తాంశం, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->దత్తాంశం);
        Curl_freeaddrinfo(తల);
        తిరిగి CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &ఆధారం, పోర్టు);
      dnsc_id2key(&కీలకం, &id);
      dnscache_తాళం(దత్తాంశం, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);

      ఒకవేళ(dns) {
        infof(దత్తాంశం, "RESOLVE %.*s:%u - old addresses discarded",
              (పూర్ణాంకం)curlx_strlen(&ఆధారం), curlx_str(&ఆధారం), పోర్టు);
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

        Curl_hash_delete(&dnscache->entries, కీలకం.దత్తాంశం, కీలకం.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_కూడు_addr(దత్తాంశం, dnscache, CURL_DNSQ_ADDR, &తల,
                          &id, &కీలకం, permanent);
      ఒకవేళ(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(దత్తాంశం, dnscache);

      ఒకవేళ(!dns)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      infof(దత్తాంశం, "[DNS] added %.*s:%u:%s to cache%s",
            (పూర్ణాంకం)curlx_strlen(&id.name), curlx_str(&id.name), id.పోర్టు,
            చిరునామాలు, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      ఒకవేళ(curlx_str_casecompare(&ఆధారం, "*")) {
        infof(దత్తాంశం, "RESOLVE *:%u using wildcard", పోర్టు);
        దత్తాంశం->state.wildcard_resolve = TRUE;
      }
    }
  }
  దత్తాంశం->state.resolve = ఖాళీ; /* dealt with now */

  తిరిగి CURLE_OK;
}
