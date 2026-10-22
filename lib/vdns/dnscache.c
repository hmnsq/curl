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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

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

#تعریف_کرو MAX_DNS_CACHE_سائز 29999

/* @unittest 3217 */
UNITTEST خالی dnsc_peer2id(ساخت dnsc_id *pid, حرف type,
                           ساخت Curl_peer *peer);
UNITTEST خالی dnsc_peer2id(ساخت dnsc_id *pid, حرف type,
                           ساخت Curl_peer *peer)
{
  curlx_str_تفویض(&pid->name, peer->hostname, لڑی_لمبائی(peer->hostname));
  pid->پورٹ = peer->پورٹ;
  pid->type = type;
}

ساکن خالی dnsc_str2id(ساخت dnsc_id *pid, حرف type,
                        ساخت Curl_str *name, uint16_t پورٹ)
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
UNITTEST خالی dnsc_id2key(ساخت dnsc_کلید *کلید, ساخت dnsc_id *id);
UNITTEST خالی dnsc_id2key(ساخت dnsc_کلید *کلید, ساخت dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  اگر(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  کلید->ڈیٹا[0] = id->type;
  کلید->ڈیٹا[1] = (uint8_t)((id->پورٹ >> 8) & 0xff);
  کلید->ڈیٹا[2] = (uint8_t)(id->پورٹ & 0xff);
  Curl_strntolower((حرف *)کلید->ڈیٹا + 3, curlx_str(&id->name), namelen);
  کلید->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_اندراج(ساخت dnsc_id *id,
                                    ساخت Curl_dns_اندراج *dns);
UNITTEST bool dnsc_id_matches_اندراج(ساخت dnsc_id *id,
                                    ساخت Curl_dns_اندراج *dns)
{
  واپس dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->پورٹ == dns->پورٹ) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ساکن خالی dnscache_اندراج_free(ساخت Curl_dns_اندراج *dns)
{
  Curl_freeaddrinfo(dns->addr);
#اگر_تعریف_شدہ USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#اگر_ختم
  curlx_free(dns);
}

ساخت dnscache_prune_ڈیٹا {
  ثابت ساخت curltime *pnow;
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
ساکن صحیح dnscache_اندراج_is_stale(خالی *datap, خالی *hc)
{
  ساخت dnscache_prune_ڈیٹا *prune = (ساخت dnscache_prune_ڈیٹا *)datap;
  ساخت Curl_dns_اندراج *dns = (ساخت Curl_dns_اندراج *)hc;

  اگر(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    انتخاب(dns->type) {
    حالت CURL_DNST_ADDR:
      اگر(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      توڑو;
#اگر_تعریف_شدہ USE_HTTPSRR
    حالت CURL_DNST_HTTPS:
      اگر(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      توڑو;
#اگر_ختم
    طےشدہ:
      توڑو;
    }
    اگر(age_ms >= prune->max_age_ms)
      واپس TRUE;
    اگر(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  واپس FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ساکن timediff_t dnscache_prune(ساخت Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ثابت ساخت curltime *pnow)
{
  ساخت dnscache_prune_ڈیٹا user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (خالی *)&user,
                                 dnscache_اندراج_is_stale);

  واپس user.oldest_ms;
}

ساکن ساخت Curl_dnscache *dnscache_get(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->share && ڈیٹا->share->specifier & (1 << CURL_تالا_ڈیٹا_DNS))
    واپس &ڈیٹا->share->dnscache;
  اگر(ڈیٹا->multi)
    واپس &ڈیٹا->multi->dnscache;
  واپس تہی;
}

ساکن خالی dnscache_تالا(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_dnscache *dnscache)
{
  اگر(ڈیٹا->share && dnscache == &ڈیٹا->share->dnscache)
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_DNS, CURL_تالا_ACCESS_SINGLE);
}

ساکن خالی dnscache_unlock(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_dnscache *dnscache)
{
  اگر(ڈیٹا->share && dnscache == &ڈیٹا->share->dnscache)
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
خالی Curl_dnscache_prune(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = ڈیٹا->سیٹ.dns_cache_timeout_ms;

  اگر(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    واپس;

  dnscache_تالا(ڈیٹا, dnscache);

  کرو {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    اگر(Curl_hash_گنتی(&dnscache->entries) > MAX_DNS_CACHE_سائز)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    ورنہ
      توڑو;

    /* if the cache size is still too big, use the oldest age as new prune
حد_بندی*/
  } جب(timeout_ms);

  dnscache_unlock(ڈیٹا, dnscache);
}

خالی Curl_dnscache_clear(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  اگر(dnscache) {
    dnscache_تالا(ڈیٹا, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(ڈیٹا, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ساکن CURLcode fetch_اندراج(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ساخت Curl_peer *peer,
                            ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_dns_اندراج *dns = تہی;
  ساخت dnsc_id id;
  ساخت dnsc_کلید کلید;
  حرف type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode نتیجہ = CURLE_OK;

  *pdns = تہی;
  اگر(!dnscache)
    واپس CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&کلید, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, کلید.ڈیٹا, کلید.len);

  اگر(dns && !dnsc_id_matches_اندراج(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = تہی;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  اگر(!dns && (type == CURL_DNST_ADDR) && ڈیٹا->state.wildcard_resolve) {
    ساخت Curl_str wildname;

    curlx_str_تفویض(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->پورٹ);
    dnsc_id2key(&کلید, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, کلید.ڈیٹا, کلید.len);
  }

  اگر(dns && (ڈیٹا->سیٹ.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ساخت dnscache_prune_ڈیٹا user;

    user.pnow = Curl_pgrs_now(ڈیٹا);
    user.max_age_ms = ڈیٹا->سیٹ.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    اگر(dnscache_اندراج_is_stale(&user, dns)) {
      infof(ڈیٹا, "Hostname in DNS cache was stale, zapped");
      dns = تہی; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, کلید.ڈیٹا, کلید.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  اگر(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    اگر((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = تہی;
    }
    ورنہ اگر(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(ڈیٹا, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = تہی;
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
    ورنہ اگر(dns && !dns->addr) { /* negative entry */
      dns = تہی;
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  واپس نتیجہ;
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
CURLcode Curl_dnscache_get(ساخت Curl_easy *ڈیٹا,
                           uint8_t dns_queries,
                           ساخت Curl_peer *peer,
                           ساخت Curl_dns_اندراج **pentry)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  ساخت Curl_dns_اندراج *dns = تہی;
  CURLcode نتیجہ = CURLE_OK;

  dnscache_تالا(ڈیٹا, dnscache);
  نتیجہ = fetch_اندراج(ڈیٹا, dnscache, dns_queries, peer, &dns);
  اگر(!نتیجہ && dns)
    dns->refcount++; /* we pass out a reference */
  ورنہ اگر(نتیجہ) {
    DEBUGASSERT(!dns);
    dns = تہی;
  }
  dnscache_unlock(ڈیٹا, dnscache);

  CURL_TRC_DNS(ڈیٹا, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->پورٹ, Curl_resolv_query_str(dns_queries),
               (صحیح)نتیجہ, dns ? "" : "not ");
  *pentry = dns;
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ساکن صحیح عدد_پتے(ثابت ساخت Curl_addrinfo *addr)
{
  صحیح i = 0;
  جب(addr) {
    addr = addr->ai_اگلا;
    i++;
  }
  واپس i;
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
UNITTEST CURLcode dns_shuffle_addr(ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_addrinfo **addr)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت صحیح عدد_addrs = عدد_پتے(*addr);

  اگر(عدد_addrs > 1) {
    ساخت Curl_addrinfo **nodes;
    CURL_TRC_DNS(ڈیٹا, "Shuffling %d addresses", عدد_addrs);

    nodes = curlx_malloc(عدد_addrs * حجم(*nodes));
    اگر(nodes) {
      صحیح i;
      غیرعلامتی صحیح *rnd;
      ثابت size_t rnd_سائز = عدد_addrs * حجم(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      برائے(i = 1; i < عدد_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_اگلا;
      }

      rnd = curlx_malloc(rnd_سائز);
      اگر(rnd) {
        /* Fisher-Yates shuffle */
        اگر(Curl_rand(ڈیٹا, (غیرعلامتی حرف *)rnd, rnd_سائز) == CURLE_OK) {
          ساخت Curl_addrinfo *تبادلہ_tmp;
          برائے(i = عدد_addrs - 1; i > 0; i--) {
            تبادلہ_tmp = nodes[rnd[i] % (غیرعلامتی صحیح)(i + 1)];
            nodes[rnd[i] % (غیرعلامتی صحیح)(i + 1)] = nodes[i];
            nodes[i] = تبادلہ_tmp;
          }

          /* relink list in the new order */
          برائے(i = 1; i < عدد_addrs; i++) {
            nodes[i - 1]->ai_اگلا = nodes[i];
          }

          nodes[عدد_addrs - 1]->ai_اگلا = تہی;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      ورنہ
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      curlx_free(nodes);
    }
    ورنہ
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
  واپس نتیجہ;
}
#اگر_ختم

ساکن bool dnscache_ai_has_family(ساخت Curl_addrinfo *ai, صحیح ai_family)
{
  برائے(; ai; ai = ai->ai_اگلا) {
    اگر(ai->ai_family == ai_family)
      واپس TRUE;
  }
  واپس FALSE;
}

ساکن ساخت Curl_dns_اندراج *dnsc_اندراج_create(ساخت dnsc_id *pid,
                                                bool permanent)
{
  ساخت Curl_dns_اندراج *dns = تہی;

  اگر(curlx_strlen(&pid->name) > UINT16_MAX)
    جاؤ برآمد;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, حجم(ساخت Curl_dns_اندراج) +
                     curlx_strlen(&pid->name));
  اگر(!dns)
    جاؤ برآمد;

  dns->refcount = 1; /* the cache has the first reference */
  dns->پورٹ = pid->پورٹ;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  اگر(dns->hostlen)
    حافظہ_نقل(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

برآمد:
  واپس dns;
}

ساکن ساخت Curl_dns_اندراج *dnsc_اندراج_تفویض_addr(
  ساخت Curl_easy *ڈیٹا,
  ساخت Curl_dns_اندراج *dns,
  uint8_t dns_queries,
  ساخت Curl_addrinfo **paddr1,
  ساخت Curl_addrinfo **paddr2)
{
  اگر(!dns)
    جاؤ برآمد;
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
    *paddr1 = تہی;
  }
  اگر(paddr2 && *paddr2) {
    ساخت Curl_addrinfo **phead = &dns->addr;
    جب(*phead)
      phead = &(*phead)->ai_اگلا;
    *phead = *paddr2;
    *paddr2 = تہی;
  }

  اگر((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#اگر_تعریف_شدہ USE_IPV6
  اگر((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#اگر_ختم /* USE_IPV6 */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  اگر(ڈیٹا->سیٹ.dns_shuffle_پتے && dns->addr) {
    CURLcode نتیجہ = dns_shuffle_addr(ڈیٹا, &dns->addr);
    اگر(نتیجہ) {
      /* free without lock, we are the sole owner */
      dnscache_اندراج_free(dns);
      dns = تہی;
      جاؤ برآمد;
    }
  }
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم

برآمد:
  اگر(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = تہی;
  }
  اگر(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = تہی;
  }
  واپس dns;
}

ساخت Curl_dns_اندراج *Curl_dnsc_mk_addr(ساخت Curl_easy *ڈیٹا,
                                         uint8_t dns_queries,
                                         ساخت Curl_addrinfo **paddr,
                                         ساخت Curl_peer *peer)
{
  ساخت dnsc_id id;
  ساخت Curl_dns_اندراج *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_تفویض_addr(ڈیٹا, dns, dns_queries, paddr, تہی);
  واپس dns;
}

ساخت Curl_dns_اندراج *Curl_dnsc_mk_addr2(ساخت Curl_easy *ڈیٹا,
                                          uint8_t dns_queries,
                                          ساخت Curl_addrinfo **paddr1,
                                          ساخت Curl_addrinfo **paddr2,
                                          ساخت Curl_peer *peer)
{
  ساخت dnsc_id id;
  ساخت Curl_dns_اندراج *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_تفویض_addr(ڈیٹا, dns, dns_queries, paddr1, paddr2);
  واپس dns;
}

ساکن ساخت Curl_dns_اندراج *dnsc_جوڑو_اندراج(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_dnscache *dnscache,
                                             ساخت dnsc_کلید *کلید,
                                             ساخت Curl_dns_اندراج *اندراج_2)
{
  اندراج_2->added = *Curl_pgrs_now(ڈیٹا);
  واپس Curl_hash_جوڑو(&dnscache->entries, کلید->ڈیٹا, کلید->len, اندراج_2);
}

#اگر_تعریف_شدہ USE_HTTPSRR
ساکن ساخت Curl_dns_اندراج *dnsc_اندراج_تفویض_https(
  ساخت Curl_dns_اندراج *dns,
  ساخت Curl_https_rrinfo **phinfo)
{
  اگر(!dns)
    جاؤ برآمد;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  اگر(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = تہی;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  اگر(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = تہی;
  }
برآمد:
  اگر(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = تہی;
  }
  واپس dns;
}

ساخت Curl_dns_اندراج *Curl_dnsc_mk_https(ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_https_rrinfo **phinfo,
                                          ساخت Curl_peer *peer)
{
  ساخت dnsc_id id;
  ساخت Curl_dns_اندراج *dns;

  (خالی)ڈیٹا;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_اندراج_create(&id, FALSE);
  dns = dnsc_اندراج_تفویض_https(dns, phinfo);
  واپس dns;
}

ساکن ساخت Curl_dns_اندراج *dnsc_جوڑو_https(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_dnscache *dnscache,
                                             ساخت Curl_https_rrinfo **phinfo,
                                             ساخت dnsc_id *id)
{
  ساخت Curl_dns_اندراج *dns, *dns2;
  ساخت dnsc_کلید کلید;

  dns = dnsc_اندراج_create(id, FALSE);
  dns = dnsc_اندراج_تفویض_https(dns, phinfo);
  اگر(!dns)
    واپس تہی;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جوڑو_اندراج(ڈیٹا, dnscache, &کلید, dns);
  اگر(!dns2) {
    dnscache_اندراج_free(dns);
    واپس تہی;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  واپس dns;
}
#اگر_ختم /* USE_HTTPSRR */

ساکن ساخت Curl_dns_اندراج *dnsc_جوڑو_addr(ساخت Curl_easy *ڈیٹا,
                                            ساخت Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ساخت Curl_addrinfo **paddr,
                                            ساخت dnsc_id *id,
                                            ساخت dnsc_کلید *کلید,
                                            bool permanent)
{
  ساخت Curl_dns_اندراج *dns;
  ساخت Curl_dns_اندراج *dns2;

  dns = dnsc_اندراج_create(id, permanent);
  dns = dnsc_اندراج_تفویض_addr(ڈیٹا, dns, dns_queries, paddr, تہی);
  اگر(!dns)
    واپس تہی;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_جوڑو_اندراج(ڈیٹا, dnscache, کلید, dns);
  اگر(!dns2) {
    dnscache_اندراج_free(dns);
    واپس تہی;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  واپس dns;
}

ساکن ساخت Curl_dns_اندراج *dnsc_جوڑو_peer_addr(
  ساخت Curl_easy *ڈیٹا,
  ساخت Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ساخت Curl_addrinfo **paddr,
  ساخت dnsc_id *id,
  bool permanent)
{
  ساخت Curl_dns_اندراج *dns;
  ساخت Curl_dns_اندراج *dns2;
  ساخت dnsc_کلید کلید;

  dns = dnsc_اندراج_create(id, permanent);
  dns = dnsc_اندراج_تفویض_addr(ڈیٹا, dns, dns_queries, paddr, تہی);
  اگر(!dns)
    واپس تہی;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&کلید, id);
  dns2 = dnsc_جوڑو_اندراج(ڈیٹا, dnscache, &کلید, dns);
  اگر(!dns2) {
    dnscache_اندراج_free(dns);
    واپس تہی;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  واپس dns;
}

CURLcode Curl_dnscache_جوڑو(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_dns_اندراج *اندراج_2)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  ساخت Curl_str name;
  ساخت dnsc_id id;
  ساخت dnsc_کلید کلید;

  اگر(!dnscache)
    واپس CURLE_FAILED_INIT;
  اگر(!اندراج_2 || (اندراج_2->type == CURL_DNST_INIT))
    واپس CURLE_BAD_FUNCTION_دلیل;

  curlx_str_تفویض(&name, اندراج_2->hostname, اندراج_2->hostlen);
  dnsc_str2id(&id, اندراج_2->type, &name, اندراج_2->پورٹ);
  dnsc_id2key(&کلید, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_تالا(ڈیٹا, dnscache);
  اگر(!dnsc_جوڑو_اندراج(ڈیٹا, dnscache, &کلید, اندراج_2)) {
    dnscache_unlock(ڈیٹا, dnscache);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  اندراج_2->refcount++;
  dnscache_unlock(ڈیٹا, dnscache);
  CURL_TRC_DNS(ڈیٹا, "cached entry for %s:%u queries=%s",
               اندراج_2->hostname, اندراج_2->پورٹ,
               Curl_resolv_query_str(اندراج_2->dns_queries));
  واپس CURLE_OK;
}

CURLcode Curl_dnscache_جوڑو_negative(ساخت Curl_easy *ڈیٹا,
                                    uint8_t dns_queries,
                                    ساخت Curl_peer *peer)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  ساخت Curl_dns_اندراج *dns = تہی;
  ساخت dnsc_id id;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(dnscache);
  اگر(!dnscache)
    واپس CURLE_FAILED_INIT;

  dnscache_تالا(ڈیٹا, dnscache);

  اگر(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_جوڑو_peer_addr(ڈیٹا, dnscache, dns_queries, تہی, &id, FALSE);
    اگر(!dns)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
#اگر_تعریف_شدہ USE_HTTPSRR
  ورنہ اگر(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_جوڑو_https(ڈیٹا, dnscache, تہی, &id);
    اگر(!dns)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
#اگر_ختم
  ورنہ {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  اگر(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(ڈیٹا, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->پورٹ,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(ڈیٹا, dnscache);
  واپس نتیجہ;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
خالی Curl_dns_اندراج_unlink(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_dns_اندراج **pdns)
{
  اگر(*pdns) {
    ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
    ساخت Curl_dns_اندراج *dns = *pdns;
    *pdns = تہی;
    dnscache_تالا(ڈیٹا, dnscache);
    dns->refcount--;
    اگر(dns->refcount == 0)
      dnscache_اندراج_free(dns);
    dnscache_unlock(ڈیٹا, dnscache);
  }
}

ساکن خالی dnscache_اندراج_dtor(خالی *اندراج_2)
{
  ساخت Curl_dns_اندراج *dns = (ساخت Curl_dns_اندراج *)اندراج_2;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  اگر(dns->refcount == 0)
    dnscache_اندراج_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
خالی Curl_dnscache_init(ساخت Curl_dnscache *dns, size_t سائز)
{
  Curl_hash_init(&dns->entries, سائز, CURL_HASH_TYPE_بائٹیں,
                 dnscache_اندراج_dtor);
}

خالی Curl_dnscache_destroy(ساخت Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_dnscache *dnscache = dnscache_get(ڈیٹا);
  ساخت curl_slist *hostp;
  ساخت dnsc_id id;
  ساخت dnsc_کلید کلید;

  اگر(!dnscache)
    واپس CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  ڈیٹا->state.wildcard_resolve = FALSE;

  برائے(hostp = ڈیٹا->state.resolve; hostp; hostp = hostp->اگلا) {
    ثابت حرف *host = hostp->ڈیٹا;
    ساخت Curl_str ماخذ;
    اگر(!host)
      جاری;
    اگر(*host == '-') {
      curl_off_t عدد = 0;
      host++;
      اگر(!curlx_str_single(&host, '[')) {
        اگر(curlx_str_until(&host, &ماخذ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          جاری;
      }
      ورنہ {
        اگر(curlx_str_until(&host, &ماخذ, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          جاری;
        }
      }

      اگر(!curlx_str_نمبر(&host, &عدد, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &ماخذ, (uint16_t)عدد);
        dnsc_id2key(&کلید, &id);
        dnscache_تالا(ڈیٹا, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, کلید.ڈیٹا, کلید.len);
        dnscache_unlock(ڈیٹا, dnscache);
      }
    }
    ورنہ {
      ساخت Curl_dns_اندراج *dns;
      ساخت Curl_addrinfo *سر = تہی, *دم = تہی;
      حرف پتہ[64];
      curl_off_t tmpofft = 0;
      uint16_t پورٹ = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ثابت حرف *پتے = تہی);

      اگر(*host == '+') {
        host++;
        permanent = FALSE;
      }
      اگر(!curlx_str_single(&host, '[')) {
        اگر(curlx_str_until(&host, &ماخذ, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          جاری;
      }
      ورنہ {
        اگر(curlx_str_until(&host, &ماخذ, 4096, ':'))
          جاری;
      }
      اگر(curlx_str_single(&host, ':') ||
         curlx_str_نمبر(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        جاؤ err;
      پورٹ = (uint16_t)tmpofft;

      VERBOSE(پتے = host);

      /* start the address section */
      جب(*host) {
        ساخت Curl_str ہدف;
        ساخت Curl_addrinfo *ai;
        CURLcode نتیجہ;

        اگر(!curlx_str_single(&host, '[')) {
          اگر(curlx_str_until(&host, &ہدف, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            جاؤ err;
        }
        ورنہ {
          اگر(curlx_str_until(&host, &ہدف, 4096, ',')) {
            اگر(curlx_str_single(&host, ','))
              جاؤ err;
            /* survive nothing but a comma */
            جاری;
          }
        }
#اگر_غیر_تعریف_شدہ USE_IPV6
        اگر(memchr(curlx_str(&ہدف), ':', curlx_strlen(&ہدف))) {
          infof(ڈیٹا, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (صحیح)curlx_strlen(&ہدف), curlx_str(&ہدف));
          اگر(curlx_str_single(&host, ','))
            جاؤ err;
          جاری;
        }
#اگر_ختم

        اگر(curlx_strlen(&ہدف) >= حجم(پتہ))
          جاؤ err;

        حافظہ_نقل(پتہ, curlx_str(&ہدف), curlx_strlen(&ہدف));
        پتہ[curlx_strlen(&ہدف)] = '\0';

        نتیجہ = Curl_str2addr(پتہ, پورٹ, &ai);
        اگر(نتیجہ) {
          infof(ڈیٹا, "Resolve IP address '%s' found is illegal", پتہ);
          جاؤ err;
        }

        اگر(دم) {
          دم->ai_اگلا = ai;
          دم = دم->ai_اگلا;
        }
        ورنہ {
          سر = دم = ai;
        }
        اگر(curlx_str_single(&host, ','))
          توڑو;
      }

      اگر(!سر)
        جاؤ err;

      error = FALSE;
err:
      اگر(error) {
        failf(ڈیٹا, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->ڈیٹا);
        Curl_freeaddrinfo(سر);
        واپس CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &ماخذ, پورٹ);
      dnsc_id2key(&کلید, &id);
      dnscache_تالا(ڈیٹا, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, کلید.ڈیٹا, کلید.len);

      اگر(dns) {
        infof(ڈیٹا, "RESOLVE %.*s:%u - old addresses discarded",
              (صحیح)curlx_strlen(&ماخذ), curlx_str(&ماخذ), پورٹ);
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

        Curl_hash_delete(&dnscache->entries, کلید.ڈیٹا, کلید.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_جوڑو_addr(ڈیٹا, dnscache, CURL_DNSQ_ADDR, &سر,
                          &id, &کلید, permanent);
      اگر(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(ڈیٹا, dnscache);

      اگر(!dns)
        واپس CURLE_برآمد_کا_حافظہ;

      infof(ڈیٹا, "[DNS] added %.*s:%u:%s to cache%s",
            (صحیح)curlx_strlen(&id.name), curlx_str(&id.name), id.پورٹ,
            پتے, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      اگر(curlx_str_casecompare(&ماخذ, "*")) {
        infof(ڈیٹا, "RESOLVE *:%u using wildcard", پورٹ);
        ڈیٹا->state.wildcard_resolve = TRUE;
      }
    }
  }
  ڈیٹا->state.resolve = تہی; /* dealt with now */

  واپس CURLE_OK;
}
