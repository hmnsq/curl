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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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

#تعریف_کرو MAX_DNS_CACHE_ناپ 29999

/* @unittest 3217 */
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer);
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer)
{
  curlx_str_سونپو(&pid->name, peer->hostname, لڑی_لمبائی(peer->hostname));
  pid->پورٹ = peer->پورٹ;
  pid->type = type;
}

ساكن فراغ dnsc_str2id(بنية dnsc_id *pid, حرف type,
                        بنية Curl_str *name, uint16_t پورٹ)
{
  pid->name = *name;
  pid->پورٹ = پورٹ;
  pid->type = type;
}

/*
 * Create a hostcache id string for the provided host + port, to be used by
 * the DNS caching. Without alloc. Return length of the id string.
 *
 * @unittest 3217
 */
UNITTEST فراغ dnsc_id2key(بنية dnsc_کلید *کلید, بنية dnsc_id *id);
UNITTEST فراغ dnsc_id2key(بنية dnsc_کلید *کلید, بنية dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  إذا(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  کلید->انکڑے[0] = id->type;
  کلید->انکڑے[1] = (uint8_t)((id->پورٹ >> 8) & 0xff);
  کلید->انکڑے[2] = (uint8_t)(id->پورٹ & 0xff);
  Curl_strntolower((حرف *)کلید->انکڑے + 3, curlx_str(&id->name), namelen);
  کلید->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_اندراج(بنية dnsc_id *id,
                                    بنية Curl_dns_اندراج *dns);
UNITTEST bool dnsc_id_matches_اندراج(بنية dnsc_id *id,
                                    بنية Curl_dns_اندراج *dns)
{
  أرجع dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->پورٹ == dns->پورٹ) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساكن فراغ dnscache_اندراج_free(بنية Curl_dns_اندراج *dns)
{
  Curl_freeaddrinfo(dns->addr);
#جے_تعریف_شدہ USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#جے_انت
  curlx_free(dns);
}

بنية dnscache_prune_انکڑے {
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
ساكن صحيح dnscache_اندراج_is_stale(فراغ *datap, فراغ *hc)
{
  بنية dnscache_prune_انکڑے *prune = (بنية dnscache_prune_انکڑے *)datap;
  بنية Curl_dns_اندراج *dns = (بنية Curl_dns_اندراج *)hc;

  إذا(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    بدل(dns->type) {
    حالة CURL_DNST_ADDR:
      إذا(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#جے_تعریف_شدہ USE_HTTPSRR
    حالة CURL_DNST_HTTPS:
      إذا(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#جے_انت
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
  بنية dnscache_prune_انکڑے user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (فراغ *)&user,
                                 dnscache_اندراج_is_stale);

  أرجع user.oldest_ms;
}

ساكن بنية Curl_dnscache *dnscache_get(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->share && انکڑے->share->specifier & (1 << CURL_جندرا_انکڑے_DNS))
    أرجع &انکڑے->share->dnscache;
  إذا(انکڑے->multi)
    أرجع &انکڑے->multi->dnscache;
  أرجع عدم;
}

ساكن فراغ dnscache_جندرا(بنية Curl_easy *انکڑے,
                          بنية Curl_dnscache *dnscache)
{
  إذا(انکڑے->share && dnscache == &انکڑے->share->dnscache)
    Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_DNS, CURL_جندرا_ACCESS_SINGLE);
}

ساكن فراغ dnscache_unlock(بنية Curl_easy *انکڑے,
                            بنية Curl_dnscache *dnscache)
{
  إذا(انکڑے->share && dnscache == &انکڑے->share->dnscache)
    Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
فراغ Curl_dnscache_prune(بنية Curl_easy *انکڑے, ثابت بنية curltime *pnow)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = انکڑے->سیٹ.dns_cache_timeout_ms;

  إذا(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    أرجع;

  dnscache_جندرا(انکڑے, dnscache);

  افعل {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    إذا(Curl_hash_گنتی(&dnscache->entries) > MAX_DNS_CACHE_ناپ)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وإلا
      اكسر;

    /* if the cache size is still too big, use the oldest age as new prune
حد_بندی*/
  } طالما(timeout_ms);

  dnscache_unlock(انکڑے, dnscache);
}

فراغ Curl_dnscache_clear(بنية Curl_easy *انکڑے)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  إذا(dnscache) {
    dnscache_جندرا(انکڑے, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(انکڑے, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساكن CURLcode fetch_اندراج(بنية Curl_easy *انکڑے,
                            بنية Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_dns_اندراج *dns = عدم;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode نتیجہ = CURLE_OK;

  *pdns = عدم;
  إذا(!dnscache)
    أرجع CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&کلید, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, کلید.انکڑے, کلید.len);

  إذا(dns && !dnsc_id_matches_اندراج(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = عدم;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  إذا(!dns && (type == CURL_DNST_ADDR) && انکڑے->state.wildcard_resolve) {
    بنية Curl_str wildname;

    curlx_str_سونپو(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->پورٹ);
    dnsc_id2key(&کلید, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, کلید.انکڑے, کلید.len);
  }

  إذا(dns && (انکڑے->سیٹ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    بنية dnscache_prune_انکڑے user;

    user.pnow = Curl_pgrs_now(انکڑے);
    user.max_age_ms = انکڑے->سیٹ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    إذا(dnscache_اندراج_is_stale(&user, dns)) {
      infof(انکڑے, "Hostname in DNS cache was stale, zapped");
      dns = عدم; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, کلید.انکڑے, کلید.len);
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
      CURL_TRC_DNS(انکڑے, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = عدم;
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
    وإلا إذا(dns && !dns->addr) { /* negative entry */
      dns = عدم;
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  أرجع نتیجہ;
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
CURLcode Curl_dnscache_get(بنية Curl_easy *انکڑے,
                           uint8_t dns_queries,
                           بنية Curl_peer *peer,
                           بنية Curl_dns_اندراج **pentry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  بنية Curl_dns_اندراج *dns = عدم;
  CURLcode نتیجہ = CURLE_OK;

  dnscache_جندرا(انکڑے, dnscache);
  نتیجہ = fetch_اندراج(انکڑے, dnscache, dns_queries, peer, &dns);
  إذا(!نتیجہ && dns)
    dns->refcount++; /* we pass out a reference */
  وإلا إذا(نتیجہ) {
    DEBUGASSERT(!dns);
    dns = عدم;
  }
  dnscache_unlock(انکڑے, dnscache);

  CURL_TRC_DNS(انکڑے, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->پورٹ, Curl_resolv_query_str(dns_queries),
               (صحيح)نتیجہ, dns ? "" : "not ");
  *pentry = dns;
  أرجع نتیجہ;
}

#جے_بنا_تعریف CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساكن صحيح عدد_پتے(ثابت بنية Curl_addrinfo *addr)
{
  صحيح i = 0;
  طالما(addr) {
    addr = addr->ai_اگلا;
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
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *انکڑے,
                                   بنية Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *انکڑے,
                                   بنية Curl_addrinfo **addr)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت صحيح عدد_addrs = عدد_پتے(*addr);

  إذا(عدد_addrs > 1) {
    بنية Curl_addrinfo **nodes;
    CURL_TRC_DNS(انکڑے, "Shuffling %d addresses", عدد_addrs);

    nodes = curlx_malloc(عدد_addrs * حجم(*nodes));
    إذا(nodes) {
      صحيح i;
      بنانشانی صحيح *rnd;
      ثابت size_t rnd_ناپ = عدد_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      لكل(i = 1; i < عدد_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_اگلا;
      }

      rnd = curlx_malloc(rnd_ناپ);
      إذا(rnd) {
        /* Fisher-Yates shuffle */
        إذا(Curl_rand(انکڑے, (بنانشانی حرف *)rnd, rnd_ناپ) == CURLE_OK) {
          بنية Curl_addrinfo *وٹاندرا_tmp;
          لكل(i = عدد_addrs - 1; i > 0; i--) {
            وٹاندرا_tmp = nodes[rnd[i] % (بنانشانی صحيح)(i + 1)];
            nodes[rnd[i] % (بنانشانی صحيح)(i + 1)] = nodes[i];
            nodes[i] = وٹاندرا_tmp;
          }

          /* relink list in the new order */
          لكل(i = 1; i < عدد_addrs; i++) {
            nodes[i - 1]->ai_اگلا = nodes[i];
          }

          nodes[عدد_addrs - 1]->ai_اگلا = عدم;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وإلا
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
      curlx_free(nodes);
    }
    وإلا
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
  أرجع نتیجہ;
}
#جے_انت

ساكن bool dnscache_ai_has_family(بنية Curl_addrinfo *ai, صحيح ai_family)
{
  لكل(; ai; ai = ai->ai_اگلا) {
    إذا(ai->ai_family == ai_family)
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن بنية Curl_dns_اندراج *dnsc_اندراج_create(بنية dnsc_id *pid,
                                                bool permanent)
{
  بنية Curl_dns_اندراج *dns = عدم;

  إذا(curlx_strlen(&pid->name) > UINT16_MAX)
    اذهب باہرلا;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(بنية Curl_dns_اندراج) +
                     curlx_strlen(&pid->name));
  إذا(!dns)
    اذهب باہرلا;

  dns->refcount = 1; /* the cache has the first reference */
  dns->پورٹ = pid->پورٹ;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  إذا(dns->hostlen)
    حافظہ_نقل(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

باہرلا:
  أرجع dns;
}

ساكن بنية Curl_dns_اندراج *dnsc_اندراج_سونپو_addr(
  بنية Curl_easy *انکڑے,
  بنية Curl_dns_اندراج *dns,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr1,
  بنية Curl_addrinfo **paddr2)
{
  إذا(!dns)
    اذهب باہرلا;
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
      phead = &(*phead)->ai_اگلا;
    *phead = *paddr2;
    *paddr2 = عدم;
  }

  إذا((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#جے_تعریف_شدہ USE_IPV6
  إذا((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#جے_انت /* USE_IPV6 */

#جے_بنا_تعریف CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  إذا(انکڑے->سیٹ.dns_shuffle_پتے && dns->addr) {
    CURLcode نتیجہ = dns_shuffle_addr(انکڑے, &dns->addr);
    إذا(نتیجہ) {
      /* free without lock, we are the sole owner */
      dnscache_اندراج_free(dns);
      dns = عدم;
      اذهب باہرلا;
    }
  }
#وإلا
  (فراغ)انکڑے;
#جے_انت

باہرلا:
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

بنية Curl_dns_اندراج *Curl_dnsc_mk_addr(بنية Curl_easy *انکڑے,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_اندراج *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_سونپو_addr(انکڑے, dns, dns_queries, paddr, عدم);
  أرجع dns;
}

بنية Curl_dns_اندراج *Curl_dnsc_mk_addr2(بنية Curl_easy *انکڑے,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_اندراج *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_سونپو_addr(انکڑے, dns, dns_queries, paddr1, paddr2);
  أرجع dns;
}

ساكن بنية Curl_dns_اندراج *dnsc_جوڑو_اندراج(بنية Curl_easy *انکڑے,
                                             بنية Curl_dnscache *dnscache,
                                             بنية dnsc_کلید *کلید,
                                             بنية Curl_dns_اندراج *اندراج)
{
  اندراج->added = *Curl_pgrs_now(انکڑے);
  أرجع Curl_hash_جوڑو(&dnscache->entries, کلید->انکڑے, کلید->len, اندراج);
}

#جے_تعریف_شدہ USE_HTTPSRR
ساكن بنية Curl_dns_اندراج *dnsc_اندراج_سونپو_https(
  بنية Curl_dns_اندراج *dns,
  بنية Curl_https_rrinfo **phinfo)
{
  إذا(!dns)
    اذهب باہرلا;
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
باہرلا:
  إذا(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_اندراج *Curl_dnsc_mk_https(بنية Curl_easy *انکڑے,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_اندراج *dns;

  (فراغ)انکڑے;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_سونپو_https(dns, phinfo);
  أرجع dns;
}

ساكن بنية Curl_dns_اندراج *dnsc_جوڑو_https(بنية Curl_easy *انکڑے,
                                             بنية Curl_dnscache *dnscache,
                                             بنية Curl_https_rrinfo **phinfo,
                                             بنية dnsc_id *id)
{
  بنية Curl_dns_اندراج *dns, *dns2;
  بنية dnsc_کلید کلید;

  dns = dnsc_اندراج_create(id, FALSE);
  dns = dnsc_اندراج_سونپو_https(dns, phinfo);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جوڑو_اندراج(انکڑے, dnscache, &کلید, dns);
  إذا(!dns2) {
    dnscache_اندراج_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}
#جے_انت /* USE_HTTPSRR */

ساكن بنية Curl_dns_اندراج *dnsc_جوڑو_addr(بنية Curl_easy *انکڑے,
                                            بنية Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            بنية Curl_addrinfo **paddr,
                                            بنية dnsc_id *id,
                                            بنية dnsc_کلید *کلید,
                                            bool permanent)
{
  بنية Curl_dns_اندراج *dns;
  بنية Curl_dns_اندراج *dns2;

  dns = dnsc_اندراج_create(id, permanent);
  dns = dnsc_اندراج_سونپو_addr(انکڑے, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_جوڑو_اندراج(انکڑے, dnscache, کلید, dns);
  إذا(!dns2) {
    dnscache_اندراج_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

ساكن بنية Curl_dns_اندراج *dnsc_جوڑو_peer_addr(
  بنية Curl_easy *انکڑے,
  بنية Curl_dnscache *dnscache,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr,
  بنية dnsc_id *id,
  bool permanent)
{
  بنية Curl_dns_اندراج *dns;
  بنية Curl_dns_اندراج *dns2;
  بنية dnsc_کلید کلید;

  dns = dnsc_اندراج_create(id, permanent);
  dns = dnsc_اندراج_سونپو_addr(انکڑے, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جوڑو_اندراج(انکڑے, dnscache, &کلید, dns);
  إذا(!dns2) {
    dnscache_اندراج_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

CURLcode Curl_dnscache_جوڑو(بنية Curl_easy *انکڑے,
                           بنية Curl_dns_اندراج *اندراج)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  بنية Curl_str name;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;
  إذا(!اندراج || (اندراج->type == CURL_DNST_INIT))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  curlx_str_سونپو(&name, اندراج->hostname, اندراج->hostlen);
  dnsc_str2id(&id, اندراج->type, &name, اندراج->پورٹ);
  dnsc_id2key(&کلید, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_جندرا(انکڑے, dnscache);
  إذا(!dnsc_جوڑو_اندراج(انکڑے, dnscache, &کلید, اندراج)) {
    dnscache_unlock(انکڑے, dnscache);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }
  اندراج->refcount++;
  dnscache_unlock(انکڑے, dnscache);
  CURL_TRC_DNS(انکڑے, "cached entry for %s:%u queries=%s",
               اندراج->hostname, اندراج->پورٹ,
               Curl_resolv_query_str(اندراج->dns_queries));
  أرجع CURLE_OK;
}

CURLcode Curl_dnscache_جوڑو_negative(بنية Curl_easy *انکڑے,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  بنية Curl_dns_اندراج *dns = عدم;
  بنية dnsc_id id;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(dnscache);
  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  dnscache_جندرا(انکڑے, dnscache);

  إذا(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_جوڑو_peer_addr(انکڑے, dnscache, dns_queries, عدم, &id, FALSE);
    إذا(!dns)
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
#جے_تعریف_شدہ USE_HTTPSRR
  وإلا إذا(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_جوڑو_https(انکڑے, dnscache, عدم, &id);
    إذا(!dns)
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
#جے_انت
  وإلا {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  إذا(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(انکڑے, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->پورٹ,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(انکڑے, dnscache);
  أرجع نتیجہ;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
فراغ Curl_dns_اندراج_unlink(بنية Curl_easy *انکڑے,
                           بنية Curl_dns_اندراج **pdns)
{
  إذا(*pdns) {
    بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
    بنية Curl_dns_اندراج *dns = *pdns;
    *pdns = عدم;
    dnscache_جندرا(انکڑے, dnscache);
    dns->refcount--;
    إذا(dns->refcount == 0)
      dnscache_اندراج_free(dns);
    dnscache_unlock(انکڑے, dnscache);
  }
}

ساكن فراغ dnscache_اندراج_dtor(فراغ *اندراج)
{
  بنية Curl_dns_اندراج *dns = (بنية Curl_dns_اندراج *)اندراج;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  إذا(dns->refcount == 0)
    dnscache_اندراج_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t ناپ)
{
  Curl_hash_init(&dns->entries, ناپ, CURL_HASH_TYPE_بائٹاں,
                 dnscache_اندراج_dtor);
}

فراغ Curl_dnscache_destroy(بنية Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(بنية Curl_easy *انکڑے)
{
  بنية Curl_dnscache *dnscache = dnscache_get(انکڑے);
  بنية curl_slist *hostp;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  انکڑے->state.wildcard_resolve = FALSE;

  لكل(hostp = انکڑے->state.resolve; hostp; hostp = hostp->اگلا) {
    ثابت حرف *host = hostp->انکڑے;
    بنية Curl_str سروت;
    إذا(!host)
      استمر;
    إذا(*host == '-') {
      curl_off_t عدد = 0;
      host++;
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &سروت, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &سروت, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          استمر;
        }
      }

      إذا(!curlx_str_نمبر(&host, &عدد, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &سروت, (uint16_t)عدد);
        dnsc_id2key(&کلید, &id);
        dnscache_جندرا(انکڑے, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, کلید.انکڑے, کلید.len);
        dnscache_unlock(انکڑے, dnscache);
      }
    }
    وإلا {
      بنية Curl_dns_اندراج *dns;
      بنية Curl_addrinfo *سر = عدم, *پوچھ = عدم;
      حرف پتہ[64];
      curl_off_t tmpofft = 0;
      uint16_t پورٹ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *پتے = عدم);

      إذا(*host == '+') {
        host++;
        permanent = FALSE;
      }
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &سروت, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &سروت, 4096, ':'))
          استمر;
      }
      إذا(curlx_str_single(&host, ':') ||
         curlx_str_نمبر(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        اذهب err;
      پورٹ = (uint16_t)tmpofft;

      VERBOSE(پتے = host);

      /* start the address section */
      طالما(*host) {
        بنية Curl_str نشانہ;
        بنية Curl_addrinfo *ai;
        CURLcode نتیجہ;

        إذا(!curlx_str_single(&host, '[')) {
          إذا(curlx_str_until(&host, &نشانہ, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            اذهب err;
        }
        وإلا {
          إذا(curlx_str_until(&host, &نشانہ, 4096, ',')) {
            إذا(curlx_str_single(&host, ','))
              اذهب err;
            /* survive nothing but a comma */
            استمر;
          }
        }
#جے_بنا_تعریف USE_IPV6
        إذا(memchr(curlx_str(&نشانہ), ':', curlx_strlen(&نشانہ))) {
          infof(انکڑے, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحيح)curlx_strlen(&نشانہ), curlx_str(&نشانہ));
          إذا(curlx_str_single(&host, ','))
            اذهب err;
          استمر;
        }
#جے_انت

        إذا(curlx_strlen(&نشانہ) >= حجم(پتہ))
          اذهب err;

        حافظہ_نقل(پتہ, curlx_str(&نشانہ), curlx_strlen(&نشانہ));
        پتہ[curlx_strlen(&نشانہ)] = '\0';

        نتیجہ = Curl_str2addr(پتہ, پورٹ, &ai);
        إذا(نتیجہ) {
          infof(انکڑے, "Resolve IP address '%s' found is illegal", پتہ);
          اذهب err;
        }

        إذا(پوچھ) {
          پوچھ->ai_اگلا = ai;
          پوچھ = پوچھ->ai_اگلا;
        }
        وإلا {
          سر = پوچھ = ai;
        }
        إذا(curlx_str_single(&host, ','))
          اكسر;
      }

      إذا(!سر)
        اذهب err;

      error = FALSE;
err:
      إذا(error) {
        failf(انکڑے, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->انکڑے);
        Curl_freeaddrinfo(سر);
        أرجع CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &سروت, پورٹ);
      dnsc_id2key(&کلید, &id);
      dnscache_جندرا(انکڑے, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, کلید.انکڑے, کلید.len);

      إذا(dns) {
        infof(انکڑے, "RESOLVE %.*s:%u - old addresses discarded",
              (صحيح)curlx_strlen(&سروت), curlx_str(&سروت), پورٹ);
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

        Curl_hash_delete(&dnscache->entries, کلید.انکڑے, کلید.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_جوڑو_addr(انکڑے, dnscache, CURL_DNSQ_ADDR, &سر,
                          &id, &کلید, permanent);
      إذا(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(انکڑے, dnscache);

      إذا(!dns)
        أرجع CURLE_باہرلا_دا_حافظہ;

      infof(انکڑے, "[DNS] added %.*s:%u:%s to cache%s",
            (صحيح)curlx_strlen(&id.name), curlx_str(&id.name), id.پورٹ,
            پتے, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      إذا(curlx_str_casecompare(&سروت, "*")) {
        infof(انکڑے, "RESOLVE *:%u using wildcard", پورٹ);
        انکڑے->state.wildcard_resolve = TRUE;
      }
    }
  }
  انکڑے->state.resolve = عدم; /* dealt with now */

  أرجع CURLE_OK;
}
