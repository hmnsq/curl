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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

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

#وصف_ڪريو MAX_DNS_CACHE_سائيز 29999

/* @unittest 3217 */
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer);
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer)
{
  curlx_str_سونپيو(&pid->name, peer->hostname, لڙي_ڊيگهه(peer->hostname));
  pid->پورٽ = peer->پورٽ;
  pid->type = type;
}

ساكن فراغ dnsc_str2id(بنية dnsc_id *pid, حرف type,
                        بنية Curl_str *name, uint16_t پورٽ)
{
  pid->name = *name;
  pid->پورٽ = پورٽ;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST فراغ dnsc_id2key(بنية dnsc_ڪنجي *ڪنجي, بنية dnsc_id *id);
UNITTEST فراغ dnsc_id2key(بنية dnsc_ڪنجي *ڪنجي, بنية dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  إذا(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ڪنجي->معلومات[0] = id->type;
  ڪنجي->معلومات[1] = (uint8_t)((id->پورٽ >> 8) & 0xff);
  ڪنجي->معلومات[2] = (uint8_t)(id->پورٽ & 0xff);
  Curl_strntolower((حرف *)ڪنجي->معلومات + 3, curlx_str(&id->name), namelen);
  ڪنجي->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_داخلا(بنية dnsc_id *id,
                                    بنية Curl_dns_داخلا *dns);
UNITTEST bool dnsc_id_matches_داخلا(بنية dnsc_id *id,
                                    بنية Curl_dns_داخلا *dns)
{
  أرجع dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->پورٽ == dns->پورٽ) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساكن فراغ dnscache_داخلا_free(بنية Curl_dns_داخلا *dns)
{
  Curl_freeaddrinfo(dns->addr);
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#جيڪڏهن_پورو
  curlx_free(dns);
}

بنية dnscache_prune_معلومات {
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
ساكن صحيح dnscache_داخلا_is_stale(فراغ *datap, فراغ *hc)
{
  بنية dnscache_prune_معلومات *prune = (بنية dnscache_prune_معلومات *)datap;
  بنية Curl_dns_داخلا *dns = (بنية Curl_dns_داخلا *)hc;

  إذا(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    بدل(dns->type) {
    حالة CURL_DNST_ADDR:
      إذا(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
    حالة CURL_DNST_HTTPS:
      إذا(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#جيڪڏهن_پورو
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
  بنية dnscache_prune_معلومات user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (فراغ *)&user,
                                 dnscache_داخلا_is_stale);

  أرجع user.oldest_ms;
}

ساكن بنية Curl_dnscache *dnscache_get(بنية Curl_easy *معلومات)
{
  إذا(معلومات->share && معلومات->share->specifier & (1 << CURL_تالو_معلومات_DNS))
    أرجع &معلومات->share->dnscache;
  إذا(معلومات->multi)
    أرجع &معلومات->multi->dnscache;
  أرجع عدم;
}

ساكن فراغ dnscache_تالو(بنية Curl_easy *معلومات,
                          بنية Curl_dnscache *dnscache)
{
  إذا(معلومات->share && dnscache == &معلومات->share->dnscache)
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_DNS, CURL_تالو_ACCESS_SINGLE);
}

ساكن فراغ dnscache_unlock(بنية Curl_easy *معلومات,
                            بنية Curl_dnscache *dnscache)
{
  إذا(معلومات->share && dnscache == &معلومات->share->dnscache)
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
فراغ Curl_dnscache_prune(بنية Curl_easy *معلومات, ثابت بنية curltime *pnow)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = معلومات->سيٽ.dns_cache_timeout_ms;

  إذا(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    أرجع;

  dnscache_تالو(معلومات, dnscache);

  افعل {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    إذا(Curl_hash_ڳڻپ(&dnscache->entries) > MAX_DNS_CACHE_سائيز)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وإلا
      اكسر;

    /* if the cache size is still too big, use the oldest age as new prune
حد_بندي*/
  } طالما(timeout_ms);

  dnscache_unlock(معلومات, dnscache);
}

فراغ Curl_dnscache_clear(بنية Curl_easy *معلومات)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  إذا(dnscache) {
    dnscache_تالو(معلومات, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(معلومات, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساكن CURLcode fetch_داخلا(بنية Curl_easy *معلومات,
                            بنية Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_dns_داخلا *dns = عدم;
  بنية dnsc_id id;
  بنية dnsc_ڪنجي ڪنجي;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode نتيجو = CURLE_OK;

  *pdns = عدم;
  إذا(!dnscache)
    أرجع CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ڪنجي, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);

  إذا(dns && !dnsc_id_matches_داخلا(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = عدم;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  إذا(!dns && (type == CURL_DNST_ADDR) && معلومات->state.wildcard_resolve) {
    بنية Curl_str wildname;

    curlx_str_سونپيو(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->پورٽ);
    dnsc_id2key(&ڪنجي, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);
  }

  إذا(dns && (معلومات->سيٽ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    بنية dnscache_prune_معلومات user;

    user.pnow = Curl_pgrs_now(معلومات);
    user.max_age_ms = معلومات->سيٽ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    إذا(dnscache_داخلا_is_stale(&user, dns)) {
      infof(معلومات, "Hostname in DNS cache was stale, zapped");
      dns = عدم; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);
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
      CURL_TRC_DNS(معلومات, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = عدم;
      نتيجو = CURLE_COULDNT_RESOLVE_HOST;
    }
    وإلا إذا(dns && !dns->addr) { /* negative entry */
      dns = عدم;
      نتيجو = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  أرجع نتيجو;
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
CURLcode Curl_dnscache_get(بنية Curl_easy *معلومات,
                           uint8_t dns_queries,
                           بنية Curl_peer *peer,
                           بنية Curl_dns_داخلا **pentry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_dns_داخلا *dns = عدم;
  CURLcode نتيجو = CURLE_OK;

  dnscache_تالو(معلومات, dnscache);
  نتيجو = fetch_داخلا(معلومات, dnscache, dns_queries, peer, &dns);
  إذا(!نتيجو && dns)
    dns->refcount++; /* we pass out a reference */
  وإلا إذا(نتيجو) {
    DEBUGASSERT(!dns);
    dns = عدم;
  }
  dnscache_unlock(معلومات, dnscache);

  CURL_TRC_DNS(معلومات, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->پورٽ, Curl_resolv_query_str(dns_queries),
               (صحيح)نتيجو, dns ? "" : "not ");
  *pentry = dns;
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساكن صحيح انگ_پتا(ثابت بنية Curl_addrinfo *addr)
{
  صحيح i = 0;
  طالما(addr) {
    addr = addr->ai_ايندڙ;
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
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *معلومات,
                                   بنية Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *معلومات,
                                   بنية Curl_addrinfo **addr)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت صحيح انگ_addrs = انگ_پتا(*addr);

  إذا(انگ_addrs > 1) {
    بنية Curl_addrinfo **nodes;
    CURL_TRC_DNS(معلومات, "Shuffling %d addresses", انگ_addrs);

    nodes = curlx_malloc(انگ_addrs * حجم(*nodes));
    إذا(nodes) {
      صحيح i;
      بنانشان صحيح *rnd;
      ثابت size_t rnd_سائيز = انگ_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      لكل(i = 1; i < انگ_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_ايندڙ;
      }

      rnd = curlx_malloc(rnd_سائيز);
      إذا(rnd) {
        /* Fisher-Yates shuffle */
        إذا(Curl_rand(معلومات, (بنانشان حرف *)rnd, rnd_سائيز) == CURLE_OK) {
          بنية Curl_addrinfo *مٽا_سٽا_tmp;
          لكل(i = انگ_addrs - 1; i > 0; i--) {
            مٽا_سٽا_tmp = nodes[rnd[i] % (بنانشان صحيح)(i + 1)];
            nodes[rnd[i] % (بنانشان صحيح)(i + 1)] = nodes[i];
            nodes[i] = مٽا_سٽا_tmp;
          }

          /* relink list in the new order */
          لكل(i = 1; i < انگ_addrs; i++) {
            nodes[i - 1]->ai_ايندڙ = nodes[i];
          }

          nodes[انگ_addrs - 1]->ai_ايندڙ = عدم;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وإلا
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      curlx_free(nodes);
    }
    وإلا
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو

ساكن bool dnscache_ai_has_family(بنية Curl_addrinfo *ai, صحيح ai_family)
{
  لكل(; ai; ai = ai->ai_ايندڙ) {
    إذا(ai->ai_family == ai_family)
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن بنية Curl_dns_داخلا *dnsc_داخلا_create(بنية dnsc_id *pid,
                                                bool permanent)
{
  بنية Curl_dns_داخلا *dns = عدم;

  إذا(curlx_strlen(&pid->name) > UINT16_MAX)
    اذهب ٻاهريون;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(بنية Curl_dns_داخلا) +
                     curlx_strlen(&pid->name));
  إذا(!dns)
    اذهب ٻاهريون;

  dns->refcount = 1; /* the cache has the first reference */
  dns->پورٽ = pid->پورٽ;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  إذا(dns->hostlen)
    يادگيري_نقل(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

ٻاهريون:
  أرجع dns;
}

ساكن بنية Curl_dns_داخلا *dnsc_داخلا_سونپيو_addr(
  بنية Curl_easy *معلومات,
  بنية Curl_dns_داخلا *dns,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr1,
  بنية Curl_addrinfo **paddr2)
{
  إذا(!dns)
    اذهب ٻاهريون;
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
      phead = &(*phead)->ai_ايندڙ;
    *phead = *paddr2;
    *paddr2 = عدم;
  }

  إذا((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#جيڪڏهن_وصف_ٿيل USE_IPV6
  إذا((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#جيڪڏهن_پورو /* USE_IPV6 */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  إذا(معلومات->سيٽ.dns_shuffle_پتا && dns->addr) {
    CURLcode نتيجو = dns_shuffle_addr(معلومات, &dns->addr);
    إذا(نتيجو) {
      /* free without lock, we are the sole owner */
      dnscache_داخلا_free(dns);
      dns = عدم;
      اذهب ٻاهريون;
    }
  }
#وإلا
  (فراغ)معلومات;
#جيڪڏهن_پورو

ٻاهريون:
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

بنية Curl_dns_داخلا *Curl_dnsc_mk_addr(بنية Curl_easy *معلومات,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_داخلا *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_داخلا_create(&id, FALSE);
  dns = dnsc_داخلا_سونپيو_addr(معلومات, dns, dns_queries, paddr, عدم);
  أرجع dns;
}

بنية Curl_dns_داخلا *Curl_dnsc_mk_addr2(بنية Curl_easy *معلومات,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_داخلا *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_داخلا_create(&id, FALSE);
  dns = dnsc_داخلا_سونپيو_addr(معلومات, dns, dns_queries, paddr1, paddr2);
  أرجع dns;
}

ساكن بنية Curl_dns_داخلا *dnsc_جوڙيو_داخلا(بنية Curl_easy *معلومات,
                                             بنية Curl_dnscache *dnscache,
                                             بنية dnsc_ڪنجي *ڪنجي,
                                             بنية Curl_dns_داخلا *داخلا_2)
{
  داخلا_2->added = *Curl_pgrs_now(معلومات);
  أرجع Curl_hash_جوڙيو(&dnscache->entries, ڪنجي->معلومات, ڪنجي->len, داخلا_2);
}

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
ساكن بنية Curl_dns_داخلا *dnsc_داخلا_سونپيو_https(
  بنية Curl_dns_داخلا *dns,
  بنية Curl_https_rrinfo **phinfo)
{
  إذا(!dns)
    اذهب ٻاهريون;
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
ٻاهريون:
  إذا(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_داخلا *Curl_dnsc_mk_https(بنية Curl_easy *معلومات,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_داخلا *dns;

  (فراغ)معلومات;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_داخلا_create(&id, FALSE);
  dns = dnsc_داخلا_سونپيو_https(dns, phinfo);
  أرجع dns;
}

ساكن بنية Curl_dns_داخلا *dnsc_جوڙيو_https(بنية Curl_easy *معلومات,
                                             بنية Curl_dnscache *dnscache,
                                             بنية Curl_https_rrinfo **phinfo,
                                             بنية dnsc_id *id)
{
  بنية Curl_dns_داخلا *dns, *dns2;
  بنية dnsc_ڪنجي ڪنجي;

  dns = dnsc_داخلا_create(id, FALSE);
  dns = dnsc_داخلا_سونپيو_https(dns, phinfo);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ڪنجي, id);
  dns2 = dnsc_جوڙيو_داخلا(معلومات, dnscache, &ڪنجي, dns);
  إذا(!dns2) {
    dnscache_داخلا_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}
#جيڪڏهن_پورو /* USE_HTTPSRR */

ساكن بنية Curl_dns_داخلا *dnsc_جوڙيو_addr(بنية Curl_easy *معلومات,
                                            بنية Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            بنية Curl_addrinfo **paddr,
                                            بنية dnsc_id *id,
                                            بنية dnsc_ڪنجي *ڪنجي,
                                            bool permanent)
{
  بنية Curl_dns_داخلا *dns;
  بنية Curl_dns_داخلا *dns2;

  dns = dnsc_داخلا_create(id, permanent);
  dns = dnsc_داخلا_سونپيو_addr(معلومات, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_جوڙيو_داخلا(معلومات, dnscache, ڪنجي, dns);
  إذا(!dns2) {
    dnscache_داخلا_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

ساكن بنية Curl_dns_داخلا *dnsc_جوڙيو_peer_addr(
  بنية Curl_easy *معلومات,
  بنية Curl_dnscache *dnscache,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr,
  بنية dnsc_id *id,
  bool permanent)
{
  بنية Curl_dns_داخلا *dns;
  بنية Curl_dns_داخلا *dns2;
  بنية dnsc_ڪنجي ڪنجي;

  dns = dnsc_داخلا_create(id, permanent);
  dns = dnsc_داخلا_سونپيو_addr(معلومات, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ڪنجي, id);
  dns2 = dnsc_جوڙيو_داخلا(معلومات, dnscache, &ڪنجي, dns);
  إذا(!dns2) {
    dnscache_داخلا_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

CURLcode Curl_dnscache_جوڙيو(بنية Curl_easy *معلومات,
                           بنية Curl_dns_داخلا *داخلا_2)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_str name;
  بنية dnsc_id id;
  بنية dnsc_ڪنجي ڪنجي;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;
  إذا(!داخلا_2 || (داخلا_2->type == CURL_DNST_INIT))
    أرجع CURLE_BAD_FUNCTION_دليل;

  curlx_str_سونپيو(&name, داخلا_2->hostname, داخلا_2->hostlen);
  dnsc_str2id(&id, داخلا_2->type, &name, داخلا_2->پورٽ);
  dnsc_id2key(&ڪنجي, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_تالو(معلومات, dnscache);
  إذا(!dnsc_جوڙيو_داخلا(معلومات, dnscache, &ڪنجي, داخلا_2)) {
    dnscache_unlock(معلومات, dnscache);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  داخلا_2->refcount++;
  dnscache_unlock(معلومات, dnscache);
  CURL_TRC_DNS(معلومات, "cached entry for %s:%u queries=%s",
               داخلا_2->hostname, داخلا_2->پورٽ,
               Curl_resolv_query_str(داخلا_2->dns_queries));
  أرجع CURLE_OK;
}

CURLcode Curl_dnscache_جوڙيو_negative(بنية Curl_easy *معلومات,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_dns_داخلا *dns = عدم;
  بنية dnsc_id id;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(dnscache);
  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  dnscache_تالو(معلومات, dnscache);

  إذا(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_جوڙيو_peer_addr(معلومات, dnscache, dns_queries, عدم, &id, FALSE);
    إذا(!dns)
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  وإلا إذا(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_جوڙيو_https(معلومات, dnscache, عدم, &id);
    إذا(!dns)
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_پورو
  وإلا {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  إذا(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(معلومات, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->پورٽ,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(معلومات, dnscache);
  أرجع نتيجو;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
فراغ Curl_dns_داخلا_unlink(بنية Curl_easy *معلومات,
                           بنية Curl_dns_داخلا **pdns)
{
  إذا(*pdns) {
    بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
    بنية Curl_dns_داخلا *dns = *pdns;
    *pdns = عدم;
    dnscache_تالو(معلومات, dnscache);
    dns->refcount--;
    إذا(dns->refcount == 0)
      dnscache_داخلا_free(dns);
    dnscache_unlock(معلومات, dnscache);
  }
}

ساكن فراغ dnscache_داخلا_dtor(فراغ *داخلا_2)
{
  بنية Curl_dns_داخلا *dns = (بنية Curl_dns_داخلا *)داخلا_2;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  إذا(dns->refcount == 0)
    dnscache_داخلا_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t سائيز)
{
  Curl_hash_init(&dns->entries, سائيز, CURL_HASH_TYPE_بائيٽون,
                 dnscache_داخلا_dtor);
}

فراغ Curl_dnscache_destroy(بنية Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(بنية Curl_easy *معلومات)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية curl_slist *hostp;
  بنية dnsc_id id;
  بنية dnsc_ڪنجي ڪنجي;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  معلومات->state.wildcard_resolve = FALSE;

  لكل(hostp = معلومات->state.resolve; hostp; hostp = hostp->ايندڙ) {
    ثابت حرف *host = hostp->معلومات;
    بنية Curl_str ذريعو;
    إذا(!host)
      استمر;
    إذا(*host == '-') {
      curl_off_t انگ = 0;
      host++;
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &ذريعو, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &ذريعو, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          استمر;
        }
      }

      إذا(!curlx_str_عدد(&host, &انگ, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &ذريعو, (uint16_t)انگ);
        dnsc_id2key(&ڪنجي, &id);
        dnscache_تالو(معلومات, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);
        dnscache_unlock(معلومات, dnscache);
      }
    }
    وإلا {
      بنية Curl_dns_داخلا *dns;
      بنية Curl_addrinfo *سر = عدم, *پڇ = عدم;
      حرف پتو[64];
      curl_off_t tmpofft = 0;
      uint16_t پورٽ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *پتا = عدم);

      إذا(*host == '+') {
        host++;
        permanent = FALSE;
      }
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &ذريعو, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &ذريعو, 4096, ':'))
          استمر;
      }
      إذا(curlx_str_single(&host, ':') ||
         curlx_str_عدد(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        اذهب err;
      پورٽ = (uint16_t)tmpofft;

      VERBOSE(پتا = host);

      /* start the address section */
      طالما(*host) {
        بنية Curl_str هدف;
        بنية Curl_addrinfo *ai;
        CURLcode نتيجو;

        إذا(!curlx_str_single(&host, '[')) {
          إذا(curlx_str_until(&host, &هدف, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            اذهب err;
        }
        وإلا {
          إذا(curlx_str_until(&host, &هدف, 4096, ',')) {
            إذا(curlx_str_single(&host, ','))
              اذهب err;
            /* survive nothing but a comma */
            استمر;
          }
        }
#جيڪڏهن_وصف_نه_ٿيل USE_IPV6
        إذا(memchr(curlx_str(&هدف), ':', curlx_strlen(&هدف))) {
          infof(معلومات, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحيح)curlx_strlen(&هدف), curlx_str(&هدف));
          إذا(curlx_str_single(&host, ','))
            اذهب err;
          استمر;
        }
#جيڪڏهن_پورو

        إذا(curlx_strlen(&هدف) >= حجم(پتو))
          اذهب err;

        يادگيري_نقل(پتو, curlx_str(&هدف), curlx_strlen(&هدف));
        پتو[curlx_strlen(&هدف)] = '\0';

        نتيجو = Curl_str2addr(پتو, پورٽ, &ai);
        إذا(نتيجو) {
          infof(معلومات, "Resolve IP address '%s' found is illegal", پتو);
          اذهب err;
        }

        إذا(پڇ) {
          پڇ->ai_ايندڙ = ai;
          پڇ = پڇ->ai_ايندڙ;
        }
        وإلا {
          سر = پڇ = ai;
        }
        إذا(curlx_str_single(&host, ','))
          اكسر;
      }

      إذا(!سر)
        اذهب err;

      error = FALSE;
err:
      إذا(error) {
        failf(معلومات, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->معلومات);
        Curl_freeaddrinfo(سر);
        أرجع CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &ذريعو, پورٽ);
      dnsc_id2key(&ڪنجي, &id);
      dnscache_تالو(معلومات, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);

      إذا(dns) {
        infof(معلومات, "RESOLVE %.*s:%u - old addresses discarded",
              (صحيح)curlx_strlen(&ذريعو), curlx_str(&ذريعو), پورٽ);
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

        Curl_hash_delete(&dnscache->entries, ڪنجي.معلومات, ڪنجي.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_جوڙيو_addr(معلومات, dnscache, CURL_DNSQ_ADDR, &سر,
                          &id, &ڪنجي, permanent);
      إذا(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(معلومات, dnscache);

      إذا(!dns)
        أرجع CURLE_ٻاهريون_جو_يادگيري;

      infof(معلومات, "[DNS] added %.*s:%u:%s to cache%s",
            (صحيح)curlx_strlen(&id.name), curlx_str(&id.name), id.پورٽ,
            پتا, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      إذا(curlx_str_casecompare(&ذريعو, "*")) {
        infof(معلومات, "RESOLVE *:%u using wildcard", پورٽ);
        معلومات->state.wildcard_resolve = TRUE;
      }
    }
  }
  معلومات->state.resolve = عدم; /* dealt with now */

  أرجع CURLE_OK;
}
