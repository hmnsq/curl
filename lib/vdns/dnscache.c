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
#ضمن "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "urldata.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_share.h"
#ضمن "curl_trc.h"
#ضمن "hash.h"
#ضمن "progress.h"
#ضمن "rand.h"
#ضمن "strcase.h"
#ضمن "vdns/dnscache.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer);
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

ساكن فراغ dnsc_str2id(بنية dnsc_id *pid, حرف type,
                        بنية Curl_str *name, uint16_t port)
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
UNITTEST فراغ dnsc_id2key(بنية dnsc_مبودي *مبودي, بنية dnsc_id *id);
UNITTEST فراغ dnsc_id2key(بنية dnsc_مبودي *مبودي, بنية dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  إذا(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  مبودي->data[0] = id->type;
  مبودي->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  مبودي->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((حرف *)مبودي->data + 3, curlx_str(&id->name), namelen);
  مبودي->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(بنية dnsc_id *id,
                                    بنية Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(بنية dnsc_id *id,
                                    بنية Curl_dns_entry *dns)
{
  أرجع dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساكن فراغ dnscache_entry_free(بنية Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

بنية dnscache_prune_data {
  ثابت بنية curltime *pnow;
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
ساكن صحيح dnscache_entry_is_stale(فراغ *datap, فراغ *hc)
{
  بنية dnscache_prune_data *prune = (بنية dnscache_prune_data *)datap;
  بنية Curl_dns_entry *dns = (بنية Curl_dns_entry *)hc;

  إذا(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    بدل(dns->type) {
    حالة CURL_DNST_ADDR:
      إذا(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#ifdef USE_HTTPSRR
    حالة CURL_DNST_HTTPS:
      إذا(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#endif
    افتراضي:
      اكسر;
    }
    إذا(age_ms >= prune->max_age_ms)
      أرجع TRUE;
    إذا(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  أرجع FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ساكن timediff_t dnscache_prune(بنية Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ثابت بنية curltime *pnow)
{
  بنية dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (فراغ *)&user,
                                 dnscache_entry_is_stale);

  أرجع user.oldest_ms;
}

ساكن بنية Curl_dnscache *dnscache_get(بنية Curl_easy *data)
{
  إذا(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    أرجع &data->share->dnscache;
  إذا(data->multi)
    أرجع &data->multi->dnscache;
  أرجع عدم;
}

ساكن فراغ dnscache_lock(بنية Curl_easy *data,
                          بنية Curl_dnscache *dnscache)
{
  إذا(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

ساكن فراغ dnscache_unlock(بنية Curl_easy *data,
                            بنية Curl_dnscache *dnscache)
{
  إذا(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
فراغ Curl_dnscache_prune(بنية Curl_easy *data, ثابت بنية curltime *pnow)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  إذا(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    أرجع;

  dnscache_lock(data, dnscache);

  افعل {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    إذا(Curl_hash_كدي(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وإلا
      اكسر;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } طالما(timeout_ms);

  dnscache_unlock(data, dnscache);
}

فراغ Curl_dnscache_clear(بنية Curl_easy *data)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  إذا(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساكن CURLcode fetch_entry(بنية Curl_easy *data,
                            بنية Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_entry **pdns)
{
  بنية Curl_dns_entry *dns = عدم;
  بنية dnsc_id id;
  بنية dnsc_مبودي مبودي;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = عدم;
  إذا(!dnscache)
    أرجع CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&مبودي, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, مبودي.data, مبودي.len);

  إذا(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = عدم;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  إذا(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    بنية Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&مبودي, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, مبودي.data, مبودي.len);
  }

  إذا(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    بنية dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    إذا(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = عدم; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, مبودي.data, مبودي.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  إذا(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    إذا((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = عدم;
    }
    وإلا إذا(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = عدم;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    وإلا إذا(dns && !dns->addr) { /* negative entry */
      dns = عدم;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  أرجع result;
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
CURLcode Curl_dnscache_get(بنية Curl_easy *data,
                           uint8_t dns_queries,
                           بنية Curl_peer *peer,
                           بنية Curl_dns_entry **pentry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  بنية Curl_dns_entry *dns = عدم;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  إذا(!result && dns)
    dns->refcount++; /* we pass out a reference */
  وإلا إذا(result) {
    DEBUGASSERT(!dns);
    dns = عدم;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (صحيح)result, dns ? "" : "not ");
  *pentry = dns;
  أرجع result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساكن صحيح لمب_addresses(ثابت بنية Curl_addrinfo *addr)
{
  صحيح i = 0;
  طالما(addr) {
    addr = addr->ai_next;
    i++;
  }
  أرجع i;
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
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *data,
                                   بنية Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *data,
                                   بنية Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ثابت صحيح لمب_addrs = لمب_addresses(*addr);

  إذا(لمب_addrs > 1) {
    بنية Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", لمب_addrs);

    nodes = curlx_malloc(لمب_addrs * حجم(*nodes));
    إذا(nodes) {
      صحيح i;
      unsigned صحيح *rnd;
      ثابت size_t rnd_size = لمب_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      لكل(i = 1; i < لمب_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      إذا(rnd) {
        /* Fisher-Yates shuffle */
        إذا(Curl_rand(data, (unsigned حرف *)rnd, rnd_size) == CURLE_OK) {
          بنية Curl_addrinfo *swap_tmp;
          لكل(i = لمب_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned صحيح)(i + 1)];
            nodes[rnd[i] % (unsigned صحيح)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          لكل(i = 1; i < لمب_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[لمب_addrs - 1]->ai_next = عدم;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وإلا
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    وإلا
      result = CURLE_OUT_OF_MEMORY;
  }
  أرجع result;
}
#endif

ساكن bool dnscache_ai_has_family(بنية Curl_addrinfo *ai, صحيح ai_family)
{
  لكل(; ai; ai = ai->ai_next) {
    إذا(ai->ai_family == ai_family)
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن بنية Curl_dns_entry *dnsc_entry_create(بنية dnsc_id *pid,
                                                bool permanent)
{
  بنية Curl_dns_entry *dns = عدم;

  إذا(curlx_strlen(&pid->name) > UINT16_MAX)
    اذهب out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(بنية Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  إذا(!dns)
    اذهب out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  إذا(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  أرجع dns;
}

ساكن بنية Curl_dns_entry *dnsc_entry_assign_addr(
  بنية Curl_easy *data,
  بنية Curl_dns_entry *dns,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr1,
  بنية Curl_addrinfo **paddr2)
{
  إذا(!dns)
    اذهب out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  إذا(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = عدم;
  }
  إذا(paddr2 && *paddr2) {
    بنية Curl_addrinfo **phead = &dns->addr;
    طالما(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = عدم;
  }

  إذا((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  إذا((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  إذا(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    إذا(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = عدم;
      اذهب out;
    }
  }
#وإلا
  (فراغ)data;
#endif

out:
  إذا(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = عدم;
  }
  إذا(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_entry *Curl_dnsc_mk_addr(بنية Curl_easy *data,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, عدم);
  أرجع dns;
}

بنية Curl_dns_entry *Curl_dnsc_mk_addr2(بنية Curl_easy *data,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  أرجع dns;
}

ساكن بنية Curl_dns_entry *dnsc_add_entry(بنية Curl_easy *data,
                                             بنية Curl_dnscache *dnscache,
                                             بنية dnsc_مبودي *مبودي,
                                             بنية Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  أرجع Curl_hash_add(&dnscache->entries, مبودي->data, مبودي->len, entry);
}

#ifdef USE_HTTPSRR
ساكن بنية Curl_dns_entry *dnsc_entry_assign_https(
  بنية Curl_dns_entry *dns,
  بنية Curl_https_rrinfo **phinfo)
{
  إذا(!dns)
    اذهب out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  إذا(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = عدم;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  إذا(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = عدم;
  }
out:
  إذا(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_entry *Curl_dnsc_mk_https(بنية Curl_easy *data,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_entry *dns;

  (فراغ)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  أرجع dns;
}

ساكن بنية Curl_dns_entry *dnsc_add_https(بنية Curl_easy *data,
                                             بنية Curl_dnscache *dnscache,
                                             بنية Curl_https_rrinfo **phinfo,
                                             بنية dnsc_id *id)
{
  بنية Curl_dns_entry *dns, *dns2;
  بنية dnsc_مبودي مبودي;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&مبودي, id);
  dns2 = dnsc_add_entry(data, dnscache, &مبودي, dns);
  إذا(!dns2) {
    dnscache_entry_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}
#endif /* USE_HTTPSRR */

ساكن بنية Curl_dns_entry *dnsc_add_addr(بنية Curl_easy *data,
                                            بنية Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            بنية Curl_addrinfo **paddr,
                                            بنية dnsc_id *id,
                                            بنية dnsc_مبودي *مبودي,
                                            bool permanent)
{
  بنية Curl_dns_entry *dns;
  بنية Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, مبودي, dns);
  إذا(!dns2) {
    dnscache_entry_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

ساكن بنية Curl_dns_entry *dnsc_add_peer_addr(
  بنية Curl_easy *data,
  بنية Curl_dnscache *dnscache,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr,
  بنية dnsc_id *id,
  bool permanent)
{
  بنية Curl_dns_entry *dns;
  بنية Curl_dns_entry *dns2;
  بنية dnsc_مبودي مبودي;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&مبودي, id);
  dns2 = dnsc_add_entry(data, dnscache, &مبودي, dns);
  إذا(!dns2) {
    dnscache_entry_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

CURLcode Curl_dnscache_add(بنية Curl_easy *data,
                           بنية Curl_dns_entry *entry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  بنية Curl_str name;
  بنية dnsc_id id;
  بنية dnsc_مبودي مبودي;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;
  إذا(!entry || (entry->type == CURL_DNST_INIT))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&مبودي, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  إذا(!dnsc_add_entry(data, dnscache, &مبودي, entry)) {
    dnscache_unlock(data, dnscache);
    أرجع CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  أرجع CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(بنية Curl_easy *data,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  بنية Curl_dns_entry *dns = عدم;
  بنية dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  إذا(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, عدم, &id, FALSE);
    إذا(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  وإلا إذا(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, عدم, &id);
    إذا(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  وإلا {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  إذا(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  أرجع result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
فراغ Curl_dns_entry_unlink(بنية Curl_easy *data,
                           بنية Curl_dns_entry **pdns)
{
  إذا(*pdns) {
    بنية Curl_dnscache *dnscache = dnscache_get(data);
    بنية Curl_dns_entry *dns = *pdns;
    *pdns = عدم;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    إذا(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

ساكن فراغ dnscache_entry_dtor(فراغ *entry)
{
  بنية Curl_dns_entry *dns = (بنية Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  إذا(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

فراغ Curl_dnscache_destroy(بنية Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(بنية Curl_easy *data)
{
  بنية Curl_dnscache *dnscache = dnscache_get(data);
  بنية curl_slist *hostp;
  بنية dnsc_id id;
  بنية dnsc_مبودي مبودي;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  لكل(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ثابت حرف *host = hostp->data;
    بنية Curl_str source;
    إذا(!host)
      استمر;
    إذا(*host == '-') {
      curl_off_t لمب = 0;
      host++;
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          استمر;
        }
      }

      إذا(!curlx_str_لمب(&host, &لمب, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)لمب);
        dnsc_id2key(&مبودي, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, مبودي.data, مبودي.len);
        dnscache_unlock(data, dnscache);
      }
    }
    وإلا {
      بنية Curl_dns_entry *dns;
      بنية Curl_addrinfo *head = عدم, *tail = عدم;
      حرف address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *addresses = عدم);

      إذا(*host == '+') {
        host++;
        permanent = FALSE;
      }
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &source, 4096, ':'))
          استمر;
      }
      إذا(curlx_str_single(&host, ':') ||
         curlx_str_لمب(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        اذهب err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      طالما(*host) {
        بنية Curl_str target;
        بنية Curl_addrinfo *ai;
        CURLcode result;

        إذا(!curlx_str_single(&host, '[')) {
          إذا(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            اذهب err;
        }
        وإلا {
          إذا(curlx_str_until(&host, &target, 4096, ',')) {
            إذا(curlx_str_single(&host, ','))
              اذهب err;
            /* survive nothing but a comma */
            استمر;
          }
        }
#ifndef USE_IPV6
        إذا(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحيح)curlx_strlen(&target), curlx_str(&target));
          إذا(curlx_str_single(&host, ','))
            اذهب err;
          استمر;
        }
#endif

        إذا(curlx_strlen(&target) >= حجم(address))
          اذهب err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        إذا(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          اذهب err;
        }

        إذا(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        وإلا {
          head = tail = ai;
        }
        إذا(curlx_str_single(&host, ','))
          اكسر;
      }

      إذا(!head)
        اذهب err;

      error = FALSE;
err:
      إذا(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        أرجع CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&مبودي, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, مبودي.data, مبودي.len);

      إذا(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (صحيح)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, مبودي.data, مبودي.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &مبودي, permanent);
      إذا(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      إذا(!dns)
        أرجع CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (صحيح)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      إذا(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = عدم; /* dealt with now */

  أرجع CURLE_OK;
}
