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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

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

#تعریف_کړه MAX_DNS_CACHE_اندازه 29999

/* @unittest 3217 */
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer);
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer)
{
  curlx_str_ټاکل(&pid->name, peer->hostname, مزي_اوږدوالی(peer->hostname));
  pid->پورټ = peer->پورټ;
  pid->type = type;
}

ساكن فراغ dnsc_str2id(بنية dnsc_id *pid, حرف type,
                        بنية Curl_str *name, uint16_t پورټ)
{
  pid->name = *name;
  pid->پورټ = پورټ;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST فراغ dnsc_id2key(بنية dnsc_کلی *کلی, بنية dnsc_id *id);
UNITTEST فراغ dnsc_id2key(بنية dnsc_کلی *کلی, بنية dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  إذا(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  کلی->معلومات[0] = id->type;
  کلی->معلومات[1] = (uint8_t)((id->پورټ >> 8) & 0xff);
  کلی->معلومات[2] = (uint8_t)(id->پورټ & 0xff);
  Curl_strntolower((حرف *)کلی->معلومات + 3, curlx_str(&id->name), namelen);
  کلی->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_ثبت(بنية dnsc_id *id,
                                    بنية Curl_dns_ثبت *dns);
UNITTEST bool dnsc_id_matches_ثبت(بنية dnsc_id *id,
                                    بنية Curl_dns_ثبت *dns)
{
  أرجع dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->پورټ == dns->پورټ) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساكن فراغ dnscache_ثبت_free(بنية Curl_dns_ثبت *dns)
{
  Curl_freeaddrinfo(dns->addr);
#که_تعریف_شوی USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#د_که_پای
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
ساكن صحيح dnscache_ثبت_is_stale(فراغ *datap, فراغ *hc)
{
  بنية dnscache_prune_معلومات *prune = (بنية dnscache_prune_معلومات *)datap;
  بنية Curl_dns_ثبت *dns = (بنية Curl_dns_ثبت *)hc;

  إذا(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    بدل(dns->type) {
    حالة CURL_DNST_ADDR:
      إذا(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#که_تعریف_شوی USE_HTTPSRR
    حالة CURL_DNST_HTTPS:
      إذا(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#د_که_پای
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
                                 dnscache_ثبت_is_stale);

  أرجع user.oldest_ms;
}

ساكن بنية Curl_dnscache *dnscache_get(بنية Curl_easy *معلومات)
{
  إذا(معلومات->share && معلومات->share->specifier & (1 << CURL_قلف_معلومات_DNS))
    أرجع &معلومات->share->dnscache;
  إذا(معلومات->multi)
    أرجع &معلومات->multi->dnscache;
  أرجع عدم;
}

ساكن فراغ dnscache_قلف(بنية Curl_easy *معلومات,
                          بنية Curl_dnscache *dnscache)
{
  إذا(معلومات->share && dnscache == &معلومات->share->dnscache)
    Curl_share_قلف(معلومات, CURL_قلف_معلومات_DNS, CURL_قلف_ACCESS_SINGLE);
}

ساكن فراغ dnscache_unlock(بنية Curl_easy *معلومات,
                            بنية Curl_dnscache *dnscache)
{
  إذا(معلومات->share && dnscache == &معلومات->share->dnscache)
    Curl_share_unlock(معلومات, CURL_قلف_معلومات_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
فراغ Curl_dnscache_prune(بنية Curl_easy *معلومات, ثابت بنية curltime *pnow)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = معلومات->سېټ.dns_cache_timeout_ms;

  إذا(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    أرجع;

  dnscache_قلف(معلومات, dnscache);

  افعل {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    إذا(Curl_hash_شمېر(&dnscache->entries) > MAX_DNS_CACHE_اندازه)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وإلا
      اكسر;

    /* if the cache size is still too big, use the oldest age as new prune
حد*/
  } طالما(timeout_ms);

  dnscache_unlock(معلومات, dnscache);
}

فراغ Curl_dnscache_clear(بنية Curl_easy *معلومات)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  إذا(dnscache) {
    dnscache_قلف(معلومات, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(معلومات, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساكن CURLcode fetch_ثبت(بنية Curl_easy *معلومات,
                            بنية Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_ثبت **pdns)
{
  بنية Curl_dns_ثبت *dns = عدم;
  بنية dnsc_id id;
  بنية dnsc_کلی کلی;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode پایله = CURLE_OK;

  *pdns = عدم;
  إذا(!dnscache)
    أرجع CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&کلی, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, کلی.معلومات, کلی.len);

  إذا(dns && !dnsc_id_matches_ثبت(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = عدم;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  إذا(!dns && (type == CURL_DNST_ADDR) && معلومات->state.wildcard_resolve) {
    بنية Curl_str wildname;

    curlx_str_ټاکل(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->پورټ);
    dnsc_id2key(&کلی, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, کلی.معلومات, کلی.len);
  }

  إذا(dns && (معلومات->سېټ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    بنية dnscache_prune_معلومات user;

    user.pnow = Curl_pgrs_now(معلومات);
    user.max_age_ms = معلومات->سېټ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    إذا(dnscache_ثبت_is_stale(&user, dns)) {
      infof(معلومات, "Hostname in DNS cache was stale, zapped");
      dns = عدم; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, کلی.معلومات, کلی.len);
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
      پایله = CURLE_COULDNT_RESOLVE_HOST;
    }
    وإلا إذا(dns && !dns->addr) { /* negative entry */
      dns = عدم;
      پایله = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  أرجع پایله;
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
                           بنية Curl_dns_ثبت **pentry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_dns_ثبت *dns = عدم;
  CURLcode پایله = CURLE_OK;

  dnscache_قلف(معلومات, dnscache);
  پایله = fetch_ثبت(معلومات, dnscache, dns_queries, peer, &dns);
  إذا(!پایله && dns)
    dns->refcount++; /* we pass out a reference */
  وإلا إذا(پایله) {
    DEBUGASSERT(!dns);
    dns = عدم;
  }
  dnscache_unlock(معلومات, dnscache);

  CURL_TRC_DNS(معلومات, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->پورټ, Curl_resolv_query_str(dns_queries),
               (صحيح)پایله, dns ? "" : "not ");
  *pentry = dns;
  أرجع پایله;
}

#که_نه_تعریف_شوی CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساكن صحيح عدد_پتې(ثابت بنية Curl_addrinfo *addr)
{
  صحيح i = 0;
  طالما(addr) {
    addr = addr->ai_بل;
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
  CURLcode پایله = CURLE_OK;
  ثابت صحيح عدد_addrs = عدد_پتې(*addr);

  إذا(عدد_addrs > 1) {
    بنية Curl_addrinfo **nodes;
    CURL_TRC_DNS(معلومات, "Shuffling %d addresses", عدد_addrs);

    nodes = curlx_malloc(عدد_addrs * حجم(*nodes));
    إذا(nodes) {
      صحيح i;
      بینښه صحيح *rnd;
      ثابت size_t rnd_اندازه = عدد_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      لكل(i = 1; i < عدد_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_بل;
      }

      rnd = curlx_malloc(rnd_اندازه);
      إذا(rnd) {
        /* Fisher-Yates shuffle */
        إذا(Curl_rand(معلومات, (بینښه حرف *)rnd, rnd_اندازه) == CURLE_OK) {
          بنية Curl_addrinfo *بدلون_tmp;
          لكل(i = عدد_addrs - 1; i > 0; i--) {
            بدلون_tmp = nodes[rnd[i] % (بینښه صحيح)(i + 1)];
            nodes[rnd[i] % (بینښه صحيح)(i + 1)] = nodes[i];
            nodes[i] = بدلون_tmp;
          }

          /* relink list in the new order */
          لكل(i = 1; i < عدد_addrs; i++) {
            nodes[i - 1]->ai_بل = nodes[i];
          }

          nodes[عدد_addrs - 1]->ai_بل = عدم;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وإلا
        پایله = CURLE_بهر_د_حافظه;
      curlx_free(nodes);
    }
    وإلا
      پایله = CURLE_بهر_د_حافظه;
  }
  أرجع پایله;
}
#د_که_پای

ساكن bool dnscache_ai_has_family(بنية Curl_addrinfo *ai, صحيح ai_family)
{
  لكل(; ai; ai = ai->ai_بل) {
    إذا(ai->ai_family == ai_family)
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن بنية Curl_dns_ثبت *dnsc_ثبت_create(بنية dnsc_id *pid,
                                                bool permanent)
{
  بنية Curl_dns_ثبت *dns = عدم;

  إذا(curlx_strlen(&pid->name) > UINT16_MAX)
    اذهب بهر;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(بنية Curl_dns_ثبت) +
                     curlx_strlen(&pid->name));
  إذا(!dns)
    اذهب بهر;

  dns->refcount = 1; /* the cache has the first reference */
  dns->پورټ = pid->پورټ;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  إذا(dns->hostlen)
    حافظه_نقل(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

بهر:
  أرجع dns;
}

ساكن بنية Curl_dns_ثبت *dnsc_ثبت_ټاکل_addr(
  بنية Curl_easy *معلومات,
  بنية Curl_dns_ثبت *dns,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr1,
  بنية Curl_addrinfo **paddr2)
{
  إذا(!dns)
    اذهب بهر;
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
      phead = &(*phead)->ai_بل;
    *phead = *paddr2;
    *paddr2 = عدم;
  }

  إذا((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#که_تعریف_شوی USE_IPV6
  إذا((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#د_که_پای /* USE_IPV6 */

#که_نه_تعریف_شوی CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  إذا(معلومات->سېټ.dns_shuffle_پتې && dns->addr) {
    CURLcode پایله = dns_shuffle_addr(معلومات, &dns->addr);
    إذا(پایله) {
      /* free without lock, we are the sole owner */
      dnscache_ثبت_free(dns);
      dns = عدم;
      اذهب بهر;
    }
  }
#وإلا
  (فراغ)معلومات;
#د_که_پای

بهر:
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

بنية Curl_dns_ثبت *Curl_dnsc_mk_addr(بنية Curl_easy *معلومات,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_ثبت *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ثبت_create(&id, FALSE);
  dns = dnsc_ثبت_ټاکل_addr(معلومات, dns, dns_queries, paddr, عدم);
  أرجع dns;
}

بنية Curl_dns_ثبت *Curl_dnsc_mk_addr2(بنية Curl_easy *معلومات,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_ثبت *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_ثبت_create(&id, FALSE);
  dns = dnsc_ثبت_ټاکل_addr(معلومات, dns, dns_queries, paddr1, paddr2);
  أرجع dns;
}

ساكن بنية Curl_dns_ثبت *dnsc_جمع_کړه_ثبت(بنية Curl_easy *معلومات,
                                             بنية Curl_dnscache *dnscache,
                                             بنية dnsc_کلی *کلی,
                                             بنية Curl_dns_ثبت *ثبت)
{
  ثبت->added = *Curl_pgrs_now(معلومات);
  أرجع Curl_hash_جمع_کړه(&dnscache->entries, کلی->معلومات, کلی->len, ثبت);
}

#که_تعریف_شوی USE_HTTPSRR
ساكن بنية Curl_dns_ثبت *dnsc_ثبت_ټاکل_https(
  بنية Curl_dns_ثبت *dns,
  بنية Curl_https_rrinfo **phinfo)
{
  إذا(!dns)
    اذهب بهر;
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
بهر:
  إذا(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_ثبت *Curl_dnsc_mk_https(بنية Curl_easy *معلومات,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_ثبت *dns;

  (فراغ)معلومات;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_ثبت_create(&id, FALSE);
  dns = dnsc_ثبت_ټاکل_https(dns, phinfo);
  أرجع dns;
}

ساكن بنية Curl_dns_ثبت *dnsc_جمع_کړه_https(بنية Curl_easy *معلومات,
                                             بنية Curl_dnscache *dnscache,
                                             بنية Curl_https_rrinfo **phinfo,
                                             بنية dnsc_id *id)
{
  بنية Curl_dns_ثبت *dns, *dns2;
  بنية dnsc_کلی کلی;

  dns = dnsc_ثبت_create(id, FALSE);
  dns = dnsc_ثبت_ټاکل_https(dns, phinfo);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلی, id);
  dns2 = dnsc_جمع_کړه_ثبت(معلومات, dnscache, &کلی, dns);
  إذا(!dns2) {
    dnscache_ثبت_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}
#د_که_پای /* USE_HTTPSRR */

ساكن بنية Curl_dns_ثبت *dnsc_جمع_کړه_addr(بنية Curl_easy *معلومات,
                                            بنية Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            بنية Curl_addrinfo **paddr,
                                            بنية dnsc_id *id,
                                            بنية dnsc_کلی *کلی,
                                            bool permanent)
{
  بنية Curl_dns_ثبت *dns;
  بنية Curl_dns_ثبت *dns2;

  dns = dnsc_ثبت_create(id, permanent);
  dns = dnsc_ثبت_ټاکل_addr(معلومات, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_جمع_کړه_ثبت(معلومات, dnscache, کلی, dns);
  إذا(!dns2) {
    dnscache_ثبت_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

ساكن بنية Curl_dns_ثبت *dnsc_جمع_کړه_peer_addr(
  بنية Curl_easy *معلومات,
  بنية Curl_dnscache *dnscache,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr,
  بنية dnsc_id *id,
  bool permanent)
{
  بنية Curl_dns_ثبت *dns;
  بنية Curl_dns_ثبت *dns2;
  بنية dnsc_کلی کلی;

  dns = dnsc_ثبت_create(id, permanent);
  dns = dnsc_ثبت_ټاکل_addr(معلومات, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلی, id);
  dns2 = dnsc_جمع_کړه_ثبت(معلومات, dnscache, &کلی, dns);
  إذا(!dns2) {
    dnscache_ثبت_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

CURLcode Curl_dnscache_جمع_کړه(بنية Curl_easy *معلومات,
                           بنية Curl_dns_ثبت *ثبت)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_str name;
  بنية dnsc_id id;
  بنية dnsc_کلی کلی;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;
  إذا(!ثبت || (ثبت->type == CURL_DNST_INIT))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  curlx_str_ټاکل(&name, ثبت->hostname, ثبت->hostlen);
  dnsc_str2id(&id, ثبت->type, &name, ثبت->پورټ);
  dnsc_id2key(&کلی, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_قلف(معلومات, dnscache);
  إذا(!dnsc_جمع_کړه_ثبت(معلومات, dnscache, &کلی, ثبت)) {
    dnscache_unlock(معلومات, dnscache);
    أرجع CURLE_بهر_د_حافظه;
  }
  ثبت->refcount++;
  dnscache_unlock(معلومات, dnscache);
  CURL_TRC_DNS(معلومات, "cached entry for %s:%u queries=%s",
               ثبت->hostname, ثبت->پورټ,
               Curl_resolv_query_str(ثبت->dns_queries));
  أرجع CURLE_OK;
}

CURLcode Curl_dnscache_جمع_کړه_negative(بنية Curl_easy *معلومات,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer)
{
  بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
  بنية Curl_dns_ثبت *dns = عدم;
  بنية dnsc_id id;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(dnscache);
  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  dnscache_قلف(معلومات, dnscache);

  إذا(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_جمع_کړه_peer_addr(معلومات, dnscache, dns_queries, عدم, &id, FALSE);
    إذا(!dns)
      پایله = CURLE_بهر_د_حافظه;
  }
#که_تعریف_شوی USE_HTTPSRR
  وإلا إذا(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_جمع_کړه_https(معلومات, dnscache, عدم, &id);
    إذا(!dns)
      پایله = CURLE_بهر_د_حافظه;
  }
#د_که_پای
  وإلا {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  إذا(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(معلومات, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->پورټ,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(معلومات, dnscache);
  أرجع پایله;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
فراغ Curl_dns_ثبت_unlink(بنية Curl_easy *معلومات,
                           بنية Curl_dns_ثبت **pdns)
{
  إذا(*pdns) {
    بنية Curl_dnscache *dnscache = dnscache_get(معلومات);
    بنية Curl_dns_ثبت *dns = *pdns;
    *pdns = عدم;
    dnscache_قلف(معلومات, dnscache);
    dns->refcount--;
    إذا(dns->refcount == 0)
      dnscache_ثبت_free(dns);
    dnscache_unlock(معلومات, dnscache);
  }
}

ساكن فراغ dnscache_ثبت_dtor(فراغ *ثبت)
{
  بنية Curl_dns_ثبت *dns = (بنية Curl_dns_ثبت *)ثبت;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  إذا(dns->refcount == 0)
    dnscache_ثبت_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t اندازه)
{
  Curl_hash_init(&dns->entries, اندازه, CURL_HASH_TYPE_بایټونه,
                 dnscache_ثبت_dtor);
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
  بنية dnsc_کلی کلی;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  معلومات->state.wildcard_resolve = FALSE;

  لكل(hostp = معلومات->state.resolve; hostp; hostp = hostp->بل) {
    ثابت حرف *host = hostp->معلومات;
    بنية Curl_str سرچینه;
    إذا(!host)
      استمر;
    إذا(*host == '-') {
      curl_off_t عدد = 0;
      host++;
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &سرچینه, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &سرچینه, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          استمر;
        }
      }

      إذا(!curlx_str_شمېره(&host, &عدد, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &سرچینه, (uint16_t)عدد);
        dnsc_id2key(&کلی, &id);
        dnscache_قلف(معلومات, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, کلی.معلومات, کلی.len);
        dnscache_unlock(معلومات, dnscache);
      }
    }
    وإلا {
      بنية Curl_dns_ثبت *dns;
      بنية Curl_addrinfo *سر = عدم, *لکۍ = عدم;
      حرف پته[64];
      curl_off_t tmpofft = 0;
      uint16_t پورټ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *پتې = عدم);

      إذا(*host == '+') {
        host++;
        permanent = FALSE;
      }
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &سرچینه, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &سرچینه, 4096, ':'))
          استمر;
      }
      إذا(curlx_str_single(&host, ':') ||
         curlx_str_شمېره(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        اذهب err;
      پورټ = (uint16_t)tmpofft;

      VERBOSE(پتې = host);

      /* start the address section */
      طالما(*host) {
        بنية Curl_str موخه;
        بنية Curl_addrinfo *ai;
        CURLcode پایله;

        إذا(!curlx_str_single(&host, '[')) {
          إذا(curlx_str_until(&host, &موخه, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            اذهب err;
        }
        وإلا {
          إذا(curlx_str_until(&host, &موخه, 4096, ',')) {
            إذا(curlx_str_single(&host, ','))
              اذهب err;
            /* survive nothing but a comma */
            استمر;
          }
        }
#که_نه_تعریف_شوی USE_IPV6
        إذا(memchr(curlx_str(&موخه), ':', curlx_strlen(&موخه))) {
          infof(معلومات, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحيح)curlx_strlen(&موخه), curlx_str(&موخه));
          إذا(curlx_str_single(&host, ','))
            اذهب err;
          استمر;
        }
#د_که_پای

        إذا(curlx_strlen(&موخه) >= حجم(پته))
          اذهب err;

        حافظه_نقل(پته, curlx_str(&موخه), curlx_strlen(&موخه));
        پته[curlx_strlen(&موخه)] = '\0';

        پایله = Curl_str2addr(پته, پورټ, &ai);
        إذا(پایله) {
          infof(معلومات, "Resolve IP address '%s' found is illegal", پته);
          اذهب err;
        }

        إذا(لکۍ) {
          لکۍ->ai_بل = ai;
          لکۍ = لکۍ->ai_بل;
        }
        وإلا {
          سر = لکۍ = ai;
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

      dnsc_str2id(&id, CURL_DNST_ADDR, &سرچینه, پورټ);
      dnsc_id2key(&کلی, &id);
      dnscache_قلف(معلومات, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, کلی.معلومات, کلی.len);

      إذا(dns) {
        infof(معلومات, "RESOLVE %.*s:%u - old addresses discarded",
              (صحيح)curlx_strlen(&سرچینه), curlx_str(&سرچینه), پورټ);
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

        Curl_hash_delete(&dnscache->entries, کلی.معلومات, کلی.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_جمع_کړه_addr(معلومات, dnscache, CURL_DNSQ_ADDR, &سر,
                          &id, &کلی, permanent);
      إذا(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(معلومات, dnscache);

      إذا(!dns)
        أرجع CURLE_بهر_د_حافظه;

      infof(معلومات, "[DNS] added %.*s:%u:%s to cache%s",
            (صحيح)curlx_strlen(&id.name), curlx_str(&id.name), id.پورټ,
            پتې, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      إذا(curlx_str_casecompare(&سرچینه, "*")) {
        infof(معلومات, "RESOLVE *:%u using wildcard", پورټ);
        معلومات->state.wildcard_resolve = TRUE;
      }
    }
  }
  معلومات->state.resolve = عدم; /* dealt with now */

  أرجع CURLE_OK;
}
