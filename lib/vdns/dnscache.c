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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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

#تعریف_کن MAX_DNS_CACHE_اندازه 29999

/* @unittest 3217 */
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer);
UNITTEST فراغ dnsc_peer2id(بنية dnsc_id *pid, حرف type,
                           بنية Curl_peer *peer)
{
  curlx_str_تخصیص_بده(&pid->name, peer->hostname, طول_رشته(peer->hostname));
  pid->پورت = peer->پورت;
  pid->type = type;
}

ساكن فراغ dnsc_str2id(بنية dnsc_id *pid, حرف type,
                        بنية Curl_str *name, uint16_t پورت)
{
  pid->name = *name;
  pid->پورت = پورت;
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
  کلید->داده[0] = id->type;
  کلید->داده[1] = (uint8_t)((id->پورت >> 8) & 0xff);
  کلید->داده[2] = (uint8_t)(id->پورت & 0xff);
  Curl_strntolower((حرف *)کلید->داده + 3, curlx_str(&id->name), namelen);
  کلید->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_مدخل(بنية dnsc_id *id,
                                    بنية Curl_dns_مدخل *dns);
UNITTEST bool dnsc_id_matches_مدخل(بنية dnsc_id *id,
                                    بنية Curl_dns_مدخل *dns)
{
  أرجع dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->پورت == dns->پورت) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساكن فراغ dnscache_مدخل_free(بنية Curl_dns_مدخل *dns)
{
  Curl_freeaddrinfo(dns->addr);
#اگر_تعریف_شده USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#پایان_اگر
  curlx_free(dns);
}

بنية dnscache_prune_داده {
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
ساكن صحيح dnscache_مدخل_is_stale(فراغ *datap, فراغ *hc)
{
  بنية dnscache_prune_داده *prune = (بنية dnscache_prune_داده *)datap;
  بنية Curl_dns_مدخل *dns = (بنية Curl_dns_مدخل *)hc;

  إذا(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    بدل(dns->type) {
    حالة CURL_DNST_ADDR:
      إذا(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#اگر_تعریف_شده USE_HTTPSRR
    حالة CURL_DNST_HTTPS:
      إذا(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      اكسر;
#پایان_اگر
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
  بنية dnscache_prune_داده user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (فراغ *)&user,
                                 dnscache_مدخل_is_stale);

  أرجع user.oldest_ms;
}

ساكن بنية Curl_dnscache *dnscache_get(بنية Curl_easy *داده)
{
  إذا(داده->share && داده->share->specifier & (1 << CURL_قفل_داده_DNS))
    أرجع &داده->share->dnscache;
  إذا(داده->multi)
    أرجع &داده->multi->dnscache;
  أرجع عدم;
}

ساكن فراغ dnscache_قفل(بنية Curl_easy *داده,
                          بنية Curl_dnscache *dnscache)
{
  إذا(داده->share && dnscache == &داده->share->dnscache)
    Curl_share_قفل(داده, CURL_قفل_داده_DNS, CURL_قفل_ACCESS_SINGLE);
}

ساكن فراغ dnscache_unlock(بنية Curl_easy *داده,
                            بنية Curl_dnscache *dnscache)
{
  إذا(داده->share && dnscache == &داده->share->dnscache)
    Curl_share_unlock(داده, CURL_قفل_داده_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
فراغ Curl_dnscache_prune(بنية Curl_easy *داده, ثابت بنية curltime *pnow)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = داده->مجموعه.dns_cache_timeout_ms;

  إذا(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    أرجع;

  dnscache_قفل(داده, dnscache);

  افعل {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    إذا(Curl_hash_شمار(&dnscache->entries) > MAX_DNS_CACHE_اندازه)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    وإلا
      اكسر;

    /* if the cache size is still too big, use the oldest age as new prune
حد*/
  } طالما(timeout_ms);

  dnscache_unlock(داده, dnscache);
}

فراغ Curl_dnscache_clear(بنية Curl_easy *داده)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  إذا(dnscache) {
    dnscache_قفل(داده, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(داده, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساكن CURLcode fetch_مدخل(بنية Curl_easy *داده,
                            بنية Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_مدخل **pdns)
{
  بنية Curl_dns_مدخل *dns = عدم;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode نتیجه = CURLE_OK;

  *pdns = عدم;
  إذا(!dnscache)
    أرجع CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&کلید, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, کلید.داده, کلید.len);

  إذا(dns && !dnsc_id_matches_مدخل(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = عدم;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  إذا(!dns && (type == CURL_DNST_ADDR) && داده->state.wildcard_resolve) {
    بنية Curl_str wildname;

    curlx_str_تخصیص_بده(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->پورت);
    dnsc_id2key(&کلید, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, کلید.داده, کلید.len);
  }

  إذا(dns && (داده->مجموعه.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    بنية dnscache_prune_داده user;

    user.pnow = Curl_pgrs_now(داده);
    user.max_age_ms = داده->مجموعه.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    إذا(dnscache_مدخل_is_stale(&user, dns)) {
      infof(داده, "Hostname in DNS cache was stale, zapped");
      dns = عدم; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, کلید.داده, کلید.len);
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
      CURL_TRC_DNS(داده, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = عدم;
      نتیجه = CURLE_COULDNT_RESOLVE_HOST;
    }
    وإلا إذا(dns && !dns->addr) { /* negative entry */
      dns = عدم;
      نتیجه = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  أرجع نتیجه;
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
CURLcode Curl_dnscache_get(بنية Curl_easy *داده,
                           uint8_t dns_queries,
                           بنية Curl_peer *peer,
                           بنية Curl_dns_مدخل **pentry)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  بنية Curl_dns_مدخل *dns = عدم;
  CURLcode نتیجه = CURLE_OK;

  dnscache_قفل(داده, dnscache);
  نتیجه = fetch_مدخل(داده, dnscache, dns_queries, peer, &dns);
  إذا(!نتیجه && dns)
    dns->refcount++; /* we pass out a reference */
  وإلا إذا(نتیجه) {
    DEBUGASSERT(!dns);
    dns = عدم;
  }
  dnscache_unlock(داده, dnscache);

  CURL_TRC_DNS(داده, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->پورت, Curl_resolv_query_str(dns_queries),
               (صحيح)نتیجه, dns ? "" : "not ");
  *pentry = dns;
  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساكن صحيح رقم_آدرسها(ثابت بنية Curl_addrinfo *addr)
{
  صحيح i = 0;
  طالما(addr) {
    addr = addr->ai_بعدی;
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
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *داده,
                                   بنية Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(بنية Curl_easy *داده,
                                   بنية Curl_addrinfo **addr)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت صحيح رقم_addrs = رقم_آدرسها(*addr);

  إذا(رقم_addrs > 1) {
    بنية Curl_addrinfo **nodes;
    CURL_TRC_DNS(داده, "Shuffling %d addresses", رقم_addrs);

    nodes = curlx_malloc(رقم_addrs * حجم(*nodes));
    إذا(nodes) {
      صحيح i;
      بیعلامت صحيح *rnd;
      ثابت size_t rnd_اندازه = رقم_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      لكل(i = 1; i < رقم_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_بعدی;
      }

      rnd = curlx_malloc(rnd_اندازه);
      إذا(rnd) {
        /* Fisher-Yates shuffle */
        إذا(Curl_rand(داده, (بیعلامت حرف *)rnd, rnd_اندازه) == CURLE_OK) {
          بنية Curl_addrinfo *جابجا_tmp;
          لكل(i = رقم_addrs - 1; i > 0; i--) {
            جابجا_tmp = nodes[rnd[i] % (بیعلامت صحيح)(i + 1)];
            nodes[rnd[i] % (بیعلامت صحيح)(i + 1)] = nodes[i];
            nodes[i] = جابجا_tmp;
          }

          /* relink list in the new order */
          لكل(i = 1; i < رقم_addrs; i++) {
            nodes[i - 1]->ai_بعدی = nodes[i];
          }

          nodes[رقم_addrs - 1]->ai_بعدی = عدم;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      وإلا
        نتیجه = CURLE_بیرون_از_حافظه;
      curlx_free(nodes);
    }
    وإلا
      نتیجه = CURLE_بیرون_از_حافظه;
  }
  أرجع نتیجه;
}
#پایان_اگر

ساكن bool dnscache_ai_has_family(بنية Curl_addrinfo *ai, صحيح ai_family)
{
  لكل(; ai; ai = ai->ai_بعدی) {
    إذا(ai->ai_family == ai_family)
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن بنية Curl_dns_مدخل *dnsc_مدخل_create(بنية dnsc_id *pid,
                                                bool permanent)
{
  بنية Curl_dns_مدخل *dns = عدم;

  إذا(curlx_strlen(&pid->name) > UINT16_MAX)
    اذهب بیرون;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(بنية Curl_dns_مدخل) +
                     curlx_strlen(&pid->name));
  إذا(!dns)
    اذهب بیرون;

  dns->refcount = 1; /* the cache has the first reference */
  dns->پورت = pid->پورت;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  إذا(dns->hostlen)
    کپی_حافظه(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

بیرون:
  أرجع dns;
}

ساكن بنية Curl_dns_مدخل *dnsc_مدخل_تخصیص_بده_addr(
  بنية Curl_easy *داده,
  بنية Curl_dns_مدخل *dns,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr1,
  بنية Curl_addrinfo **paddr2)
{
  إذا(!dns)
    اذهب بیرون;
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
      phead = &(*phead)->ai_بعدی;
    *phead = *paddr2;
    *paddr2 = عدم;
  }

  إذا((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#اگر_تعریف_شده USE_IPV6
  إذا((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#پایان_اگر /* USE_IPV6 */

#اگر_تعریف_نشده CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  إذا(داده->مجموعه.dns_shuffle_آدرسها && dns->addr) {
    CURLcode نتیجه = dns_shuffle_addr(داده, &dns->addr);
    إذا(نتیجه) {
      /* free without lock, we are the sole owner */
      dnscache_مدخل_free(dns);
      dns = عدم;
      اذهب بیرون;
    }
  }
#وإلا
  (فراغ)داده;
#پایان_اگر

بیرون:
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

بنية Curl_dns_مدخل *Curl_dnsc_mk_addr(بنية Curl_easy *داده,
                                         uint8_t dns_queries,
                                         بنية Curl_addrinfo **paddr,
                                         بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_مدخل *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_مدخل_create(&id, FALSE);
  dns = dnsc_مدخل_تخصیص_بده_addr(داده, dns, dns_queries, paddr, عدم);
  أرجع dns;
}

بنية Curl_dns_مدخل *Curl_dnsc_mk_addr2(بنية Curl_easy *داده,
                                          uint8_t dns_queries,
                                          بنية Curl_addrinfo **paddr1,
                                          بنية Curl_addrinfo **paddr2,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_مدخل *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_مدخل_create(&id, FALSE);
  dns = dnsc_مدخل_تخصیص_بده_addr(داده, dns, dns_queries, paddr1, paddr2);
  أرجع dns;
}

ساكن بنية Curl_dns_مدخل *dnsc_جمع_کن_مدخل(بنية Curl_easy *داده,
                                             بنية Curl_dnscache *dnscache,
                                             بنية dnsc_کلید *کلید,
                                             بنية Curl_dns_مدخل *مدخل)
{
  مدخل->added = *Curl_pgrs_now(داده);
  أرجع Curl_hash_جمع_کن(&dnscache->entries, کلید->داده, کلید->len, مدخل);
}

#اگر_تعریف_شده USE_HTTPSRR
ساكن بنية Curl_dns_مدخل *dnsc_مدخل_تخصیص_بده_https(
  بنية Curl_dns_مدخل *dns,
  بنية Curl_https_rrinfo **phinfo)
{
  إذا(!dns)
    اذهب بیرون;
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
بیرون:
  إذا(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = عدم;
  }
  أرجع dns;
}

بنية Curl_dns_مدخل *Curl_dnsc_mk_https(بنية Curl_easy *داده,
                                          بنية Curl_https_rrinfo **phinfo,
                                          بنية Curl_peer *peer)
{
  بنية dnsc_id id;
  بنية Curl_dns_مدخل *dns;

  (فراغ)داده;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_مدخل_create(&id, FALSE);
  dns = dnsc_مدخل_تخصیص_بده_https(dns, phinfo);
  أرجع dns;
}

ساكن بنية Curl_dns_مدخل *dnsc_جمع_کن_https(بنية Curl_easy *داده,
                                             بنية Curl_dnscache *dnscache,
                                             بنية Curl_https_rrinfo **phinfo,
                                             بنية dnsc_id *id)
{
  بنية Curl_dns_مدخل *dns, *dns2;
  بنية dnsc_کلید کلید;

  dns = dnsc_مدخل_create(id, FALSE);
  dns = dnsc_مدخل_تخصیص_بده_https(dns, phinfo);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جمع_کن_مدخل(داده, dnscache, &کلید, dns);
  إذا(!dns2) {
    dnscache_مدخل_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}
#پایان_اگر /* USE_HTTPSRR */

ساكن بنية Curl_dns_مدخل *dnsc_جمع_کن_addr(بنية Curl_easy *داده,
                                            بنية Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            بنية Curl_addrinfo **paddr,
                                            بنية dnsc_id *id,
                                            بنية dnsc_کلید *کلید,
                                            bool permanent)
{
  بنية Curl_dns_مدخل *dns;
  بنية Curl_dns_مدخل *dns2;

  dns = dnsc_مدخل_create(id, permanent);
  dns = dnsc_مدخل_تخصیص_بده_addr(داده, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_جمع_کن_مدخل(داده, dnscache, کلید, dns);
  إذا(!dns2) {
    dnscache_مدخل_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

ساكن بنية Curl_dns_مدخل *dnsc_جمع_کن_peer_addr(
  بنية Curl_easy *داده,
  بنية Curl_dnscache *dnscache,
  uint8_t dns_queries,
  بنية Curl_addrinfo **paddr,
  بنية dnsc_id *id,
  bool permanent)
{
  بنية Curl_dns_مدخل *dns;
  بنية Curl_dns_مدخل *dns2;
  بنية dnsc_کلید کلید;

  dns = dnsc_مدخل_create(id, permanent);
  dns = dnsc_مدخل_تخصیص_بده_addr(داده, dns, dns_queries, paddr, عدم);
  إذا(!dns)
    أرجع عدم;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جمع_کن_مدخل(داده, dnscache, &کلید, dns);
  إذا(!dns2) {
    dnscache_مدخل_free(dns);
    أرجع عدم;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  أرجع dns;
}

CURLcode Curl_dnscache_جمع_کن(بنية Curl_easy *داده,
                           بنية Curl_dns_مدخل *مدخل)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  بنية Curl_str name;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;
  إذا(!مدخل || (مدخل->type == CURL_DNST_INIT))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  curlx_str_تخصیص_بده(&name, مدخل->hostname, مدخل->hostlen);
  dnsc_str2id(&id, مدخل->type, &name, مدخل->پورت);
  dnsc_id2key(&کلید, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_قفل(داده, dnscache);
  إذا(!dnsc_جمع_کن_مدخل(داده, dnscache, &کلید, مدخل)) {
    dnscache_unlock(داده, dnscache);
    أرجع CURLE_بیرون_از_حافظه;
  }
  مدخل->refcount++;
  dnscache_unlock(داده, dnscache);
  CURL_TRC_DNS(داده, "cached entry for %s:%u queries=%s",
               مدخل->hostname, مدخل->پورت,
               Curl_resolv_query_str(مدخل->dns_queries));
  أرجع CURLE_OK;
}

CURLcode Curl_dnscache_جمع_کن_negative(بنية Curl_easy *داده,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  بنية Curl_dns_مدخل *dns = عدم;
  بنية dnsc_id id;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(dnscache);
  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  dnscache_قفل(داده, dnscache);

  إذا(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_جمع_کن_peer_addr(داده, dnscache, dns_queries, عدم, &id, FALSE);
    إذا(!dns)
      نتیجه = CURLE_بیرون_از_حافظه;
  }
#اگر_تعریف_شده USE_HTTPSRR
  وإلا إذا(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_جمع_کن_https(داده, dnscache, عدم, &id);
    إذا(!dns)
      نتیجه = CURLE_بیرون_از_حافظه;
  }
#پایان_اگر
  وإلا {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  إذا(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(داده, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->پورت,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(داده, dnscache);
  أرجع نتیجه;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
فراغ Curl_dns_مدخل_unlink(بنية Curl_easy *داده,
                           بنية Curl_dns_مدخل **pdns)
{
  إذا(*pdns) {
    بنية Curl_dnscache *dnscache = dnscache_get(داده);
    بنية Curl_dns_مدخل *dns = *pdns;
    *pdns = عدم;
    dnscache_قفل(داده, dnscache);
    dns->refcount--;
    إذا(dns->refcount == 0)
      dnscache_مدخل_free(dns);
    dnscache_unlock(داده, dnscache);
  }
}

ساكن فراغ dnscache_مدخل_dtor(فراغ *مدخل)
{
  بنية Curl_dns_مدخل *dns = (بنية Curl_dns_مدخل *)مدخل;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  إذا(dns->refcount == 0)
    dnscache_مدخل_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
فراغ Curl_dnscache_init(بنية Curl_dnscache *dns, size_t اندازه)
{
  Curl_hash_init(&dns->entries, اندازه, CURL_HASH_TYPE_بایتها,
                 dnscache_مدخل_dtor);
}

فراغ Curl_dnscache_destroy(بنية Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(بنية Curl_easy *داده)
{
  بنية Curl_dnscache *dnscache = dnscache_get(داده);
  بنية curl_slist *hostp;
  بنية dnsc_id id;
  بنية dnsc_کلید کلید;

  إذا(!dnscache)
    أرجع CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  داده->state.wildcard_resolve = FALSE;

  لكل(hostp = داده->state.resolve; hostp; hostp = hostp->بعدی) {
    ثابت حرف *host = hostp->داده;
    بنية Curl_str منبع;
    إذا(!host)
      استمر;
    إذا(*host == '-') {
      curl_off_t رقم = 0;
      host++;
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &منبع, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &منبع, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          استمر;
        }
      }

      إذا(!curlx_str_عدد(&host, &رقم, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &منبع, (uint16_t)رقم);
        dnsc_id2key(&کلید, &id);
        dnscache_قفل(داده, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, کلید.داده, کلید.len);
        dnscache_unlock(داده, dnscache);
      }
    }
    وإلا {
      بنية Curl_dns_مدخل *dns;
      بنية Curl_addrinfo *سر = عدم, *دم = عدم;
      حرف آدرس[64];
      curl_off_t tmpofft = 0;
      uint16_t پورت = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *آدرسها = عدم);

      إذا(*host == '+') {
        host++;
        permanent = FALSE;
      }
      إذا(!curlx_str_single(&host, '[')) {
        إذا(curlx_str_until(&host, &منبع, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          استمر;
      }
      وإلا {
        إذا(curlx_str_until(&host, &منبع, 4096, ':'))
          استمر;
      }
      إذا(curlx_str_single(&host, ':') ||
         curlx_str_عدد(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        اذهب err;
      پورت = (uint16_t)tmpofft;

      VERBOSE(آدرسها = host);

      /* start the address section */
      طالما(*host) {
        بنية Curl_str هدف;
        بنية Curl_addrinfo *ai;
        CURLcode نتیجه;

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
#اگر_تعریف_نشده USE_IPV6
        إذا(memchr(curlx_str(&هدف), ':', curlx_strlen(&هدف))) {
          infof(داده, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحيح)curlx_strlen(&هدف), curlx_str(&هدف));
          إذا(curlx_str_single(&host, ','))
            اذهب err;
          استمر;
        }
#پایان_اگر

        إذا(curlx_strlen(&هدف) >= حجم(آدرس))
          اذهب err;

        کپی_حافظه(آدرس, curlx_str(&هدف), curlx_strlen(&هدف));
        آدرس[curlx_strlen(&هدف)] = '\0';

        نتیجه = Curl_str2addr(آدرس, پورت, &ai);
        إذا(نتیجه) {
          infof(داده, "Resolve IP address '%s' found is illegal", آدرس);
          اذهب err;
        }

        إذا(دم) {
          دم->ai_بعدی = ai;
          دم = دم->ai_بعدی;
        }
        وإلا {
          سر = دم = ai;
        }
        إذا(curlx_str_single(&host, ','))
          اكسر;
      }

      إذا(!سر)
        اذهب err;

      error = FALSE;
err:
      إذا(error) {
        failf(داده, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->داده);
        Curl_freeaddrinfo(سر);
        أرجع CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &منبع, پورت);
      dnsc_id2key(&کلید, &id);
      dnscache_قفل(داده, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, کلید.داده, کلید.len);

      إذا(dns) {
        infof(داده, "RESOLVE %.*s:%u - old addresses discarded",
              (صحيح)curlx_strlen(&منبع), curlx_str(&منبع), پورت);
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

        Curl_hash_delete(&dnscache->entries, کلید.داده, کلید.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_جمع_کن_addr(داده, dnscache, CURL_DNSQ_ADDR, &سر,
                          &id, &کلید, permanent);
      إذا(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(داده, dnscache);

      إذا(!dns)
        أرجع CURLE_بیرون_از_حافظه;

      infof(داده, "[DNS] added %.*s:%u:%s to cache%s",
            (صحيح)curlx_strlen(&id.name), curlx_str(&id.name), id.پورت,
            آدرسها, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      إذا(curlx_str_casecompare(&منبع, "*")) {
        infof(داده, "RESOLVE *:%u using wildcard", پورت);
        داده->state.wildcard_resolve = TRUE;
      }
    }
  }
  داده->state.resolve = عدم; /* dealt with now */

  أرجع CURLE_OK;
}
