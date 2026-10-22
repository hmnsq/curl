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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ଅନ୍ତର୍ଭାଵ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "hash.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "vdns/dnscache.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST ଶୂନ୍ୟ dnsc_peer2id(ସଂରଚନା dnsc_id *pid, ଅକ୍ଷର type,
                           ସଂରଚନା Curl_peer *peer);
UNITTEST ଶୂନ୍ୟ dnsc_peer2id(ସଂରଚନା dnsc_id *pid, ଅକ୍ଷର type,
                           ସଂରଚନା Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

ସ୍ଥିର ଶୂନ୍ୟ dnsc_str2id(ସଂରଚନା dnsc_id *pid, ଅକ୍ଷର type,
                        ସଂରଚନା Curl_str *name, uint16_t port)
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
UNITTEST ଶୂନ୍ୟ dnsc_id2key(ସଂରଚନା dnsc_ଚାବି *ଚାବି, ସଂରଚନା dnsc_id *id);
UNITTEST ଶୂନ୍ୟ dnsc_id2key(ସଂରଚନା dnsc_ଚାବି *ଚାବି, ସଂରଚନା dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  ଯଦି(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ଚାବି->data[0] = id->type;
  ଚାବି->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  ଚାବି->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((ଅକ୍ଷର *)ଚାବି->data + 3, curlx_str(&id->name), namelen);
  ଚାବି->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(ସଂରଚନା dnsc_id *id,
                                    ସଂରଚନା Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(ସଂରଚନା dnsc_id *id,
                                    ସଂରଚନା Curl_dns_entry *dns)
{
  ଫେରନ୍ତୁ dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ସ୍ଥିର ଶୂନ୍ୟ dnscache_entry_free(ସଂରଚନା Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

ସଂରଚନା dnscache_prune_data {
  ଅଚଳ ସଂରଚନା curltime *pnow;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dnscache_entry_is_stale(ଶୂନ୍ୟ *datap, ଶୂନ୍ୟ *hc)
{
  ସଂରଚନା dnscache_prune_data *prune = (ସଂରଚନା dnscache_prune_data *)datap;
  ସଂରଚନା Curl_dns_entry *dns = (ସଂରଚନା Curl_dns_entry *)hc;

  ଯଦି(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ବାଛ(dns->type) {
    ସ୍ଥିତି CURL_DNST_ADDR:
      ଯଦି(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ଭାଙ୍ଗ;
#ifdef USE_HTTPSRR
    ସ୍ଥିତି CURL_DNST_HTTPS:
      ଯଦି(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
    ଯଦି(age_ms >= prune->max_age_ms)
      ଫେରନ୍ତୁ TRUE;
    ଯଦି(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ସ୍ଥିର timediff_t dnscache_prune(ସଂରଚନା Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (ଶୂନ୍ୟ *)&user,
                                 dnscache_entry_is_stale);

  ଫେରନ୍ତୁ user.oldest_ms;
}

ସ୍ଥିର ସଂରଚନା Curl_dnscache *dnscache_get(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    ଫେରନ୍ତୁ &data->share->dnscache;
  ଯଦି(data->multi)
    ଫେରନ୍ତୁ &data->multi->dnscache;
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଶୂନ୍ୟ dnscache_lock(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_dnscache *dnscache)
{
  ଯଦି(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

ସ୍ଥିର ଶୂନ୍ୟ dnscache_unlock(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_dnscache *dnscache)
{
  ଯଦି(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
ଶୂନ୍ୟ Curl_dnscache_prune(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  ଯଦି(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ଫେରନ୍ତୁ;

  dnscache_lock(data, dnscache);

  କର {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    ଯଦି(Curl_hash_ଗଣନା(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } ଯେତେବେଳେ(timeout_ms);

  dnscache_unlock(data, dnscache);
}

ଶୂନ୍ୟ Curl_dnscache_clear(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  ଯଦି(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ସ୍ଥିର CURLcode fetch_entry(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ସଂରଚନା Curl_peer *peer,
                            ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା dnsc_ଚାବି ଚାବି;
  ଅକ୍ଷର type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = ଖାଲି;
  ଯଦି(!dnscache)
    ଫେରନ୍ତୁ CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ଚାବି, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ଚାବି.data, ଚାବି.len);

  ଯଦି(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ଖାଲି;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  ଯଦି(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    ସଂରଚନା Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&ଚାବି, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ଚାବି.data, ଚାବି.len);
  }

  ଯଦି(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ସଂରଚନା dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    ଯଦି(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = ଖାଲି; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ଚାବି.data, ଚାବି.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  ଯଦି(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    ଯଦି((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ଖାଲି;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    ଅନ୍ୟଥା ଯଦି(dns && !dns->addr) { /* negative entry */
      dns = ଖାଲି;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ଫେରନ୍ତୁ result;
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
CURLcode Curl_dnscache_get(ସଂରଚନା Curl_easy *data,
                           uint8_t dns_queries,
                           ସଂରଚନା Curl_peer *peer,
                           ସଂରଚନା Curl_dns_entry **pentry)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  ଯଦି(!result && dns)
    dns->refcount++; /* we pass out a reference */
  ଅନ୍ୟଥା ଯଦି(result) {
    DEBUGASSERT(!dns);
    dns = ଖାଲି;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, dns ? "" : "not ");
  *pentry = dns;
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_addresses(ଅଚଳ ସଂରଚନା Curl_addrinfo *addr)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
  ଯେତେବେଳେ(addr) {
    addr = addr->ai_next;
    i++;
  }
  ଫେରନ୍ତୁ i;
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
UNITTEST CURLcode dns_shuffle_addr(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_addrs = ଅଙ୍କ_addresses(*addr);

  ଯଦି(ଅଙ୍କ_addrs > 1) {
    ସଂରଚନା Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", ଅଙ୍କ_addrs);

    nodes = curlx_malloc(ଅଙ୍କ_addrs * ଆକାର(*nodes));
    ଯଦି(nodes) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rnd;
      ଅଚଳ size_t rnd_size = ଅଙ୍କ_addrs * ଆକାର(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      ପାଇଁ(i = 1; i < ଅଙ୍କ_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      ଯଦି(rnd) {
        /* Fisher-Yates shuffle */
        ଯଦି(Curl_rand(data, (unsigned ଅକ୍ଷର *)rnd, rnd_size) == CURLE_OK) {
          ସଂରଚନା Curl_addrinfo *swap_tmp;
          ପାଇଁ(i = ଅଙ୍କ_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(i + 1)];
            nodes[rnd[i] % (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          ପାଇଁ(i = 1; i < ଅଙ୍କ_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[ଅଙ୍କ_addrs - 1]->ai_next = ଖାଲି;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      ଅନ୍ୟଥା
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    ଅନ୍ୟଥା
      result = CURLE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ result;
}
#endif

ସ୍ଥିର bool dnscache_ai_has_family(ସଂରଚନା Curl_addrinfo *ai, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family)
{
  ପାଇଁ(; ai; ai = ai->ai_next) {
    ଯଦି(ai->ai_family == ai_family)
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_entry_create(ସଂରଚନା dnsc_id *pid,
                                                bool permanent)
{
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;

  ଯଦି(curlx_strlen(&pid->name) > UINT16_MAX)
    ଯାଅ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  ଯଦି(!dns)
    ଯାଅ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  ଯଦି(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  ଫେରନ୍ତୁ dns;
}

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_entry_assign_addr(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା Curl_dns_entry *dns,
  uint8_t dns_queries,
  ସଂରଚନା Curl_addrinfo **paddr1,
  ସଂରଚନା Curl_addrinfo **paddr2)
{
  ଯଦି(!dns)
    ଯାଅ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  ଯଦି(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ଖାଲି;
  }
  ଯଦି(paddr2 && *paddr2) {
    ସଂରଚନା Curl_addrinfo **phead = &dns->addr;
    ଯେତେବେଳେ(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = ଖାଲି;
  }

  ଯଦି((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  ଯଦି((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  ଯଦି(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    ଯଦି(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = ଖାଲି;
      ଯାଅ out;
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
#endif

out:
  ଯଦି(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ଖାଲି;
  }
  ଯଦି(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ଖାଲି;
  }
  ଫେରନ୍ତୁ dns;
}

ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_addr(ସଂରଚନା Curl_easy *data,
                                         uint8_t dns_queries,
                                         ସଂରଚନା Curl_addrinfo **paddr,
                                         ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ଖାଲି);
  ଫେରନ୍ତୁ dns;
}

ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_addr2(ସଂରଚନା Curl_easy *data,
                                          uint8_t dns_queries,
                                          ସଂରଚନା Curl_addrinfo **paddr1,
                                          ସଂରଚନା Curl_addrinfo **paddr2,
                                          ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  ଫେରନ୍ତୁ dns;
}

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_add_entry(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_dnscache *dnscache,
                                             ସଂରଚନା dnsc_ଚାବି *ଚାବି,
                                             ସଂରଚନା Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  ଫେରନ୍ତୁ Curl_hash_add(&dnscache->entries, ଚାବି->data, ଚାବି->len, entry);
}

#ifdef USE_HTTPSRR
ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_entry_assign_https(
  ସଂରଚନା Curl_dns_entry *dns,
  ସଂରଚନା Curl_https_rrinfo **phinfo)
{
  ଯଦି(!dns)
    ଯାଅ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  ଯଦି(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ଖାଲି;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  ଯଦି(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ଖାଲି;
  }
out:
  ଯଦି(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ଖାଲି;
  }
  ଫେରନ୍ତୁ dns;
}

ସଂରଚନା Curl_dns_entry *Curl_dnsc_mk_https(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_https_rrinfo **phinfo,
                                          ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା Curl_dns_entry *dns;

  (ଶୂନ୍ୟ)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ଫେରନ୍ତୁ dns;
}

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_add_https(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_dnscache *dnscache,
                                             ସଂରଚନା Curl_https_rrinfo **phinfo,
                                             ସଂରଚନା dnsc_id *id)
{
  ସଂରଚନା Curl_dns_entry *dns, *dns2;
  ସଂରଚନା dnsc_ଚାବି ଚାବି;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ଯଦି(!dns)
    ଫେରନ୍ତୁ ଖାଲି;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ଚାବି, id);
  dns2 = dnsc_add_entry(data, dnscache, &ଚାବି, dns);
  ଯଦି(!dns2) {
    dnscache_entry_free(dns);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ଫେରନ୍ତୁ dns;
}
#endif /* USE_HTTPSRR */

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_add_addr(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ସଂରଚନା Curl_addrinfo **paddr,
                                            ସଂରଚନା dnsc_id *id,
                                            ସଂରଚନା dnsc_ଚାବି *ଚାବି,
                                            bool permanent)
{
  ସଂରଚନା Curl_dns_entry *dns;
  ସଂରଚନା Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ଖାଲି);
  ଯଦି(!dns)
    ଫେରନ୍ତୁ ଖାଲି;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, ଚାବି, dns);
  ଯଦି(!dns2) {
    dnscache_entry_free(dns);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ଫେରନ୍ତୁ dns;
}

ସ୍ଥିର ସଂରଚନା Curl_dns_entry *dnsc_add_peer_addr(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ସଂରଚନା Curl_addrinfo **paddr,
  ସଂରଚନା dnsc_id *id,
  bool permanent)
{
  ସଂରଚନା Curl_dns_entry *dns;
  ସଂରଚନା Curl_dns_entry *dns2;
  ସଂରଚନା dnsc_ଚାବି ଚାବି;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ଖାଲି);
  ଯଦି(!dns)
    ଫେରନ୍ତୁ ଖାଲି;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ଚାବି, id);
  dns2 = dnsc_add_entry(data, dnscache, &ଚାବି, dns);
  ଯଦି(!dns2) {
    dnscache_entry_free(dns);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ଫେରନ୍ତୁ dns;
}

CURLcode Curl_dnscache_add(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_dns_entry *entry)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  ସଂରଚନା Curl_str name;
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା dnsc_ଚାବି ଚାବି;

  ଯଦି(!dnscache)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!entry || (entry->type == CURL_DNST_INIT))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&ଚାବି, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  ଯଦି(!dnsc_add_entry(data, dnscache, &ଚାବି, entry)) {
    dnscache_unlock(data, dnscache);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(ସଂରଚନା Curl_easy *data,
                                    uint8_t dns_queries,
                                    ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  ସଂରଚନା dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  ଯଦି(!dnscache)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  ଯଦି(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, ଖାଲି, &id, FALSE);
    ଯଦି(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  ଅନ୍ୟଥା ଯଦି(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, ଖାଲି, &id);
    ଯଦି(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ଅନ୍ୟଥା {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  ଯଦି(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
ଶୂନ୍ୟ Curl_dns_entry_unlink(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_dns_entry **pdns)
{
  ଯଦି(*pdns) {
    ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
    ସଂରଚନା Curl_dns_entry *dns = *pdns;
    *pdns = ଖାଲି;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    ଯଦି(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ dnscache_entry_dtor(ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା Curl_dns_entry *dns = (ସଂରଚନା Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  ଯଦି(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
ଶୂନ୍ୟ Curl_dnscache_init(ସଂରଚନା Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

ଶୂନ୍ୟ Curl_dnscache_destroy(ସଂରଚନା Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_dnscache *dnscache = dnscache_get(data);
  ସଂରଚନା curl_slist *hostp;
  ସଂରଚନା dnsc_id id;
  ସଂରଚନା dnsc_ଚାବି ଚାବି;

  ଯଦି(!dnscache)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  ପାଇଁ(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ଅଚଳ ଅକ୍ଷର *host = hostp->data;
    ସଂରଚନା Curl_str source;
    ଯଦି(!host)
      ଜାରି;
    ଯଦି(*host == '-') {
      curl_off_t ଅଙ୍କ = 0;
      host++;
      ଯଦି(!curlx_str_single(&host, '[')) {
        ଯଦି(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ଜାରି;
      }
      ଅନ୍ୟଥା {
        ଯଦି(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ଜାରି;
        }
      }

      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&host, &ଅଙ୍କ, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)ଅଙ୍କ);
        dnsc_id2key(&ଚାବି, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ଚାବି.data, ଚାବି.len);
        dnscache_unlock(data, dnscache);
      }
    }
    ଅନ୍ୟଥା {
      ସଂରଚନା Curl_dns_entry *dns;
      ସଂରଚନା Curl_addrinfo *head = ଖାଲି, *tail = ଖାଲି;
      ଅକ୍ଷର address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ଅଚଳ ଅକ୍ଷର *addresses = ଖାଲି);

      ଯଦି(*host == '+') {
        host++;
        permanent = FALSE;
      }
      ଯଦି(!curlx_str_single(&host, '[')) {
        ଯଦି(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ଜାରି;
      }
      ଅନ୍ୟଥା {
        ଯଦି(curlx_str_until(&host, &source, 4096, ':'))
          ଜାରି;
      }
      ଯଦି(curlx_str_single(&host, ':') ||
         curlx_str_ସଂଖ୍ୟା(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        ଯାଅ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      ଯେତେବେଳେ(*host) {
        ସଂରଚନା Curl_str target;
        ସଂରଚନା Curl_addrinfo *ai;
        CURLcode result;

        ଯଦି(!curlx_str_single(&host, '[')) {
          ଯଦି(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            ଯାଅ err;
        }
        ଅନ୍ୟଥା {
          ଯଦି(curlx_str_until(&host, &target, 4096, ',')) {
            ଯଦି(curlx_str_single(&host, ','))
              ଯାଅ err;
            /* survive nothing but a comma */
            ଜାରି;
          }
        }
#ifndef USE_IPV6
        ଯଦି(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(&target), curlx_str(&target));
          ଯଦି(curlx_str_single(&host, ','))
            ଯାଅ err;
          ଜାରି;
        }
#endif

        ଯଦି(curlx_strlen(&target) >= ଆକାର(address))
          ଯାଅ err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        ଯଦି(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          ଯାଅ err;
        }

        ଯଦି(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        ଅନ୍ୟଥା {
          head = tail = ai;
        }
        ଯଦି(curlx_str_single(&host, ','))
          ଭାଙ୍ଗ;
      }

      ଯଦି(!head)
        ଯାଅ err;

      error = FALSE;
err:
      ଯଦି(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        ଫେରନ୍ତୁ CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&ଚାବି, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ଚାବି.data, ଚାବି.len);

      ଯଦି(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, ଚାବି.data, ଚାବି.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &ଚାବି, permanent);
      ଯଦି(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      ଯଦି(!dns)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      ଯଦି(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = ଖାଲି; /* dealt with now */

  ଫେରନ୍ତୁ CURLE_OK;
}
