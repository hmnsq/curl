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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل "urldata.h"
#شامل "curl_addrinfo.h"
#شامل "curl_share.h"
#شامل "curl_trc.h"
#شامل "hash.h"
#شامل "progress.h"
#شامل "rand.h"
#شامل "strcase.h"
#شامل "vdns/dnscache.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strcopy.h"
#شامل "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST تهی dnsc_peer2id(ساختار dnsc_id *pid, نویسه type,
                           ساختار Curl_peer *peer);
UNITTEST تهی dnsc_peer2id(ساختار dnsc_id *pid, نویسه type,
                           ساختار Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

ایستا تهی dnsc_str2id(ساختار dnsc_id *pid, نویسه type,
                        ساختار Curl_str *name, uint16_t port)
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
UNITTEST تهی dnsc_id2key(ساختار dnsc_کلید *کلید, ساختار dnsc_id *id);
UNITTEST تهی dnsc_id2key(ساختار dnsc_کلید *کلید, ساختار dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  اگر(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  کلید->data[0] = id->type;
  کلید->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  کلید->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((نویسه *)کلید->data + 3, curlx_str(&id->name), namelen);
  کلید->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(ساختار dnsc_id *id,
                                    ساختار Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(ساختار dnsc_id *id,
                                    ساختار Curl_dns_entry *dns)
{
  بازگشت dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ایستا تهی dnscache_entry_free(ساختار Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

ساختار dnscache_prune_data {
  ثابت ساختار curltime *pnow;
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
ایستا صحیح dnscache_entry_is_stale(تهی *datap, تهی *hc)
{
  ساختار dnscache_prune_data *prune = (ساختار dnscache_prune_data *)datap;
  ساختار Curl_dns_entry *dns = (ساختار Curl_dns_entry *)hc;

  اگر(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    انتخاب(dns->type) {
    حالت CURL_DNST_ADDR:
      اگر(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      بشکن;
#ifdef USE_HTTPSRR
    حالت CURL_DNST_HTTPS:
      اگر(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      بشکن;
#endif
    پیشفرض:
      بشکن;
    }
    اگر(age_ms >= prune->max_age_ms)
      بازگشت TRUE;
    اگر(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  بازگشت FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ایستا timediff_t dnscache_prune(ساختار Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ثابت ساختار curltime *pnow)
{
  ساختار dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (تهی *)&user,
                                 dnscache_entry_is_stale);

  بازگشت user.oldest_ms;
}

ایستا ساختار Curl_dnscache *dnscache_get(ساختار Curl_easy *data)
{
  اگر(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    بازگشت &data->share->dnscache;
  اگر(data->multi)
    بازگشت &data->multi->dnscache;
  بازگشت پوچ;
}

ایستا تهی dnscache_lock(ساختار Curl_easy *data,
                          ساختار Curl_dnscache *dnscache)
{
  اگر(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

ایستا تهی dnscache_unlock(ساختار Curl_easy *data,
                            ساختار Curl_dnscache *dnscache)
{
  اگر(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
تهی Curl_dnscache_prune(ساختار Curl_easy *data, ثابت ساختار curltime *pnow)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  اگر(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    بازگشت;

  dnscache_lock(data, dnscache);

  انجامبده {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    اگر(Curl_hash_شمار(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وگرنه
      بشکن;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } وقتی(timeout_ms);

  dnscache_unlock(data, dnscache);
}

تهی Curl_dnscache_clear(ساختار Curl_easy *data)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  اگر(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ایستا CURLcode fetch_entry(ساختار Curl_easy *data,
                            ساختار Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ساختار Curl_peer *peer,
                            ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_dns_entry *dns = پوچ;
  ساختار dnsc_id id;
  ساختار dnsc_کلید کلید;
  نویسه type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = پوچ;
  اگر(!dnscache)
    بازگشت CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&کلید, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, کلید.data, کلید.len);

  اگر(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = پوچ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  اگر(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    ساختار Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&کلید, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, کلید.data, کلید.len);
  }

  اگر(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ساختار dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    اگر(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = پوچ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, کلید.data, کلید.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  اگر(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    اگر((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = پوچ;
    }
    وگرنه اگر(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = پوچ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    وگرنه اگر(dns && !dns->addr) { /* negative entry */
      dns = پوچ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  بازگشت result;
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
CURLcode Curl_dnscache_get(ساختار Curl_easy *data,
                           uint8_t dns_queries,
                           ساختار Curl_peer *peer,
                           ساختار Curl_dns_entry **pentry)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  ساختار Curl_dns_entry *dns = پوچ;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  اگر(!result && dns)
    dns->refcount++; /* we pass out a reference */
  وگرنه اگر(result) {
    DEBUGASSERT(!dns);
    dns = پوچ;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (صحیح)result, dns ? "" : "not ");
  *pentry = dns;
  بازگشت result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ایستا صحیح رقم_addresses(ثابت ساختار Curl_addrinfo *addr)
{
  صحیح i = 0;
  وقتی(addr) {
    addr = addr->ai_next;
    i++;
  }
  بازگشت i;
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
UNITTEST CURLcode dns_shuffle_addr(ساختار Curl_easy *data,
                                   ساختار Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ساختار Curl_easy *data,
                                   ساختار Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ثابت صحیح رقم_addrs = رقم_addresses(*addr);

  اگر(رقم_addrs > 1) {
    ساختار Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", رقم_addrs);

    nodes = curlx_malloc(رقم_addrs * اندازه(*nodes));
    اگر(nodes) {
      صحیح i;
      unsigned صحیح *rnd;
      ثابت size_t rnd_size = رقم_addrs * اندازه(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      برای(i = 1; i < رقم_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      اگر(rnd) {
        /* Fisher-Yates shuffle */
        اگر(Curl_rand(data, (unsigned نویسه *)rnd, rnd_size) == CURLE_OK) {
          ساختار Curl_addrinfo *swap_tmp;
          برای(i = رقم_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned صحیح)(i + 1)];
            nodes[rnd[i] % (unsigned صحیح)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          برای(i = 1; i < رقم_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[رقم_addrs - 1]->ai_next = پوچ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وگرنه
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    وگرنه
      result = CURLE_OUT_OF_MEMORY;
  }
  بازگشت result;
}
#endif

ایستا bool dnscache_ai_has_family(ساختار Curl_addrinfo *ai, صحیح ai_family)
{
  برای(; ai; ai = ai->ai_next) {
    اگر(ai->ai_family == ai_family)
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا ساختار Curl_dns_entry *dnsc_entry_create(ساختار dnsc_id *pid,
                                                bool permanent)
{
  ساختار Curl_dns_entry *dns = پوچ;

  اگر(curlx_strlen(&pid->name) > UINT16_MAX)
    برو out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, اندازه(ساختار Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  اگر(!dns)
    برو out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  اگر(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  بازگشت dns;
}

ایستا ساختار Curl_dns_entry *dnsc_entry_assign_addr(
  ساختار Curl_easy *data,
  ساختار Curl_dns_entry *dns,
  uint8_t dns_queries,
  ساختار Curl_addrinfo **paddr1,
  ساختار Curl_addrinfo **paddr2)
{
  اگر(!dns)
    برو out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  اگر(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = پوچ;
  }
  اگر(paddr2 && *paddr2) {
    ساختار Curl_addrinfo **phead = &dns->addr;
    وقتی(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = پوچ;
  }

  اگر((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  اگر((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  اگر(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    اگر(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = پوچ;
      برو out;
    }
  }
#وگرنه
  (تهی)data;
#endif

out:
  اگر(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = پوچ;
  }
  اگر(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = پوچ;
  }
  بازگشت dns;
}

ساختار Curl_dns_entry *Curl_dnsc_mk_addr(ساختار Curl_easy *data,
                                         uint8_t dns_queries,
                                         ساختار Curl_addrinfo **paddr,
                                         ساختار Curl_peer *peer)
{
  ساختار dnsc_id id;
  ساختار Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, پوچ);
  بازگشت dns;
}

ساختار Curl_dns_entry *Curl_dnsc_mk_addr2(ساختار Curl_easy *data,
                                          uint8_t dns_queries,
                                          ساختار Curl_addrinfo **paddr1,
                                          ساختار Curl_addrinfo **paddr2,
                                          ساختار Curl_peer *peer)
{
  ساختار dnsc_id id;
  ساختار Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  بازگشت dns;
}

ایستا ساختار Curl_dns_entry *dnsc_add_entry(ساختار Curl_easy *data,
                                             ساختار Curl_dnscache *dnscache,
                                             ساختار dnsc_کلید *کلید,
                                             ساختار Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  بازگشت Curl_hash_add(&dnscache->entries, کلید->data, کلید->len, entry);
}

#ifdef USE_HTTPSRR
ایستا ساختار Curl_dns_entry *dnsc_entry_assign_https(
  ساختار Curl_dns_entry *dns,
  ساختار Curl_https_rrinfo **phinfo)
{
  اگر(!dns)
    برو out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  اگر(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = پوچ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  اگر(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = پوچ;
  }
out:
  اگر(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = پوچ;
  }
  بازگشت dns;
}

ساختار Curl_dns_entry *Curl_dnsc_mk_https(ساختار Curl_easy *data,
                                          ساختار Curl_https_rrinfo **phinfo,
                                          ساختار Curl_peer *peer)
{
  ساختار dnsc_id id;
  ساختار Curl_dns_entry *dns;

  (تهی)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  بازگشت dns;
}

ایستا ساختار Curl_dns_entry *dnsc_add_https(ساختار Curl_easy *data,
                                             ساختار Curl_dnscache *dnscache,
                                             ساختار Curl_https_rrinfo **phinfo,
                                             ساختار dnsc_id *id)
{
  ساختار Curl_dns_entry *dns, *dns2;
  ساختار dnsc_کلید کلید;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  اگر(!dns)
    بازگشت پوچ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_add_entry(data, dnscache, &کلید, dns);
  اگر(!dns2) {
    dnscache_entry_free(dns);
    بازگشت پوچ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  بازگشت dns;
}
#endif /* USE_HTTPSRR */

ایستا ساختار Curl_dns_entry *dnsc_add_addr(ساختار Curl_easy *data,
                                            ساختار Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ساختار Curl_addrinfo **paddr,
                                            ساختار dnsc_id *id,
                                            ساختار dnsc_کلید *کلید,
                                            bool permanent)
{
  ساختار Curl_dns_entry *dns;
  ساختار Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, پوچ);
  اگر(!dns)
    بازگشت پوچ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, کلید, dns);
  اگر(!dns2) {
    dnscache_entry_free(dns);
    بازگشت پوچ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  بازگشت dns;
}

ایستا ساختار Curl_dns_entry *dnsc_add_peer_addr(
  ساختار Curl_easy *data,
  ساختار Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ساختار Curl_addrinfo **paddr,
  ساختار dnsc_id *id,
  bool permanent)
{
  ساختار Curl_dns_entry *dns;
  ساختار Curl_dns_entry *dns2;
  ساختار dnsc_کلید کلید;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, پوچ);
  اگر(!dns)
    بازگشت پوچ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_add_entry(data, dnscache, &کلید, dns);
  اگر(!dns2) {
    dnscache_entry_free(dns);
    بازگشت پوچ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  بازگشت dns;
}

CURLcode Curl_dnscache_add(ساختار Curl_easy *data,
                           ساختار Curl_dns_entry *entry)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  ساختار Curl_str name;
  ساختار dnsc_id id;
  ساختار dnsc_کلید کلید;

  اگر(!dnscache)
    بازگشت CURLE_FAILED_INIT;
  اگر(!entry || (entry->type == CURL_DNST_INIT))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&کلید, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  اگر(!dnsc_add_entry(data, dnscache, &کلید, entry)) {
    dnscache_unlock(data, dnscache);
    بازگشت CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  بازگشت CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(ساختار Curl_easy *data,
                                    uint8_t dns_queries,
                                    ساختار Curl_peer *peer)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  ساختار Curl_dns_entry *dns = پوچ;
  ساختار dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  اگر(!dnscache)
    بازگشت CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  اگر(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, پوچ, &id, FALSE);
    اگر(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  وگرنه اگر(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, پوچ, &id);
    اگر(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  وگرنه {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  اگر(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  بازگشت result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
تهی Curl_dns_entry_unlink(ساختار Curl_easy *data,
                           ساختار Curl_dns_entry **pdns)
{
  اگر(*pdns) {
    ساختار Curl_dnscache *dnscache = dnscache_get(data);
    ساختار Curl_dns_entry *dns = *pdns;
    *pdns = پوچ;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    اگر(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

ایستا تهی dnscache_entry_dtor(تهی *entry)
{
  ساختار Curl_dns_entry *dns = (ساختار Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  اگر(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
تهی Curl_dnscache_init(ساختار Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

تهی Curl_dnscache_destroy(ساختار Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ساختار Curl_easy *data)
{
  ساختار Curl_dnscache *dnscache = dnscache_get(data);
  ساختار curl_slist *hostp;
  ساختار dnsc_id id;
  ساختار dnsc_کلید کلید;

  اگر(!dnscache)
    بازگشت CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  برای(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ثابت نویسه *host = hostp->data;
    ساختار Curl_str source;
    اگر(!host)
      ادامه;
    اگر(*host == '-') {
      curl_off_t رقم = 0;
      host++;
      اگر(!curlx_str_single(&host, '[')) {
        اگر(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ادامه;
      }
      وگرنه {
        اگر(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ادامه;
        }
      }

      اگر(!curlx_str_عدد(&host, &رقم, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)رقم);
        dnsc_id2key(&کلید, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, کلید.data, کلید.len);
        dnscache_unlock(data, dnscache);
      }
    }
    وگرنه {
      ساختار Curl_dns_entry *dns;
      ساختار Curl_addrinfo *head = پوچ, *tail = پوچ;
      نویسه address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت نویسه *addresses = پوچ);

      اگر(*host == '+') {
        host++;
        permanent = FALSE;
      }
      اگر(!curlx_str_single(&host, '[')) {
        اگر(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ادامه;
      }
      وگرنه {
        اگر(curlx_str_until(&host, &source, 4096, ':'))
          ادامه;
      }
      اگر(curlx_str_single(&host, ':') ||
         curlx_str_عدد(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        برو err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      وقتی(*host) {
        ساختار Curl_str target;
        ساختار Curl_addrinfo *ai;
        CURLcode result;

        اگر(!curlx_str_single(&host, '[')) {
          اگر(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            برو err;
        }
        وگرنه {
          اگر(curlx_str_until(&host, &target, 4096, ',')) {
            اگر(curlx_str_single(&host, ','))
              برو err;
            /* survive nothing but a comma */
            ادامه;
          }
        }
#ifndef USE_IPV6
        اگر(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحیح)curlx_strlen(&target), curlx_str(&target));
          اگر(curlx_str_single(&host, ','))
            برو err;
          ادامه;
        }
#endif

        اگر(curlx_strlen(&target) >= اندازه(address))
          برو err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        اگر(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          برو err;
        }

        اگر(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        وگرنه {
          head = tail = ai;
        }
        اگر(curlx_str_single(&host, ','))
          بشکن;
      }

      اگر(!head)
        برو err;

      error = FALSE;
err:
      اگر(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        بازگشت CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&کلید, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, کلید.data, کلید.len);

      اگر(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (صحیح)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, کلید.data, کلید.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &کلید, permanent);
      اگر(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      اگر(!dns)
        بازگشت CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (صحیح)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      اگر(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = پوچ; /* dealt with now */

  بازگشت CURLE_OK;
}
