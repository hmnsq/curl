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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN6_H
#समावेश <netinet/in6.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_share.h"
#समावेश "curl_trc.h"
#समावेश "hash.h"
#समावेश "progress.h"
#समावेश "rand.h"
#समावेश "strcase.h"
#समावेश "vdns/dnscache.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/strcopy.h"
#समावेश "curlx/strparse.h"

#घोषणा MAX_DNS_CACHE_आकार 29999

/* @unittest 3217 */
UNITTEST व्योम dnsc_peer2id(काष्ठा dnsc_id *pid, अक्षर type,
                           काष्ठा Curl_peer *peer);
UNITTEST व्योम dnsc_peer2id(काष्ठा dnsc_id *pid, अक्षर type,
                           काष्ठा Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, म_माप(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

अटल व्योम dnsc_str2id(काष्ठा dnsc_id *pid, अक्षर type,
                        काष्ठा Curl_str *name, uint16_t port)
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
UNITTEST व्योम dnsc_id2key(काष्ठा dnsc_कुंजी *कुंजी, काष्ठा dnsc_id *id);
UNITTEST व्योम dnsc_id2key(काष्ठा dnsc_कुंजी *कुंजी, काष्ठा dnsc_id *id)
{
  माप_प्रकार namelen = curlx_strlen(&id->name);
  अगर(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  कुंजी->तथ्य[0] = id->type;
  कुंजी->तथ्य[1] = (uint8_t)((id->port >> 8) & 0xff);
  कुंजी->तथ्य[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((अक्षर *)कुंजी->तथ्य + 3, curlx_str(&id->name), namelen);
  कुंजी->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(काष्ठा dnsc_id *id,
                                    काष्ठा Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(काष्ठा dnsc_id *id,
                                    काष्ठा Curl_dns_entry *dns)
{
  वापस dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!म_भेद(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

अटल व्योम dnscache_entry_free(काष्ठा Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#अगर_घोषित USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#पूर्ण_अगर
  curlx_free(dns);
}

काष्ठा dnscache_prune_तथ्य {
  स्थिर काष्ठा curltime *pnow;
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
अटल पूर्णांक dnscache_entry_is_stale(व्योम *datap, व्योम *hc)
{
  काष्ठा dnscache_prune_तथ्य *prune = (काष्ठा dnscache_prune_तथ्य *)datap;
  काष्ठा Curl_dns_entry *dns = (काष्ठा Curl_dns_entry *)hc;

  अगर(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    चयन(dns->type) {
    हाल CURL_DNST_ADDR:
      अगर(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      अवरोध;
#अगर_घोषित USE_HTTPSRR
    हाल CURL_DNST_HTTPS:
      अगर(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      अवरोध;
#पूर्ण_अगर
    शेष:
      अवरोध;
    }
    अगर(age_ms >= prune->max_age_ms)
      वापस TRUE;
    अगर(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  वापस FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
अटल timediff_t dnscache_prune(काष्ठा Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 स्थिर काष्ठा curltime *pnow)
{
  काष्ठा dnscache_prune_तथ्य user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (व्योम *)&user,
                                 dnscache_entry_is_stale);

  वापस user.oldest_ms;
}

अटल काष्ठा Curl_dnscache *dnscache_get(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->share && तथ्य->share->specifier & (1 << CURL_LOCK_तथ्य_DNS))
    वापस &तथ्य->share->dnscache;
  अगर(तथ्य->multi)
    वापस &तथ्य->multi->dnscache;
  वापस शून्य;
}

अटल व्योम dnscache_lock(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_dnscache *dnscache)
{
  अगर(तथ्य->share && dnscache == &तथ्य->share->dnscache)
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_DNS, CURL_LOCK_ACCESS_SINGLE);
}

अटल व्योम dnscache_unlock(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_dnscache *dnscache)
{
  अगर(तथ्य->share && dnscache == &तथ्य->share->dnscache)
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
व्योम Curl_dnscache_prune(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = तथ्य->set.dns_cache_timeout_ms;

  अगर(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    वापस;

  dnscache_lock(तथ्य, dnscache);

  करो {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    अगर(Curl_hash_गिनती(&dnscache->entries) > MAX_DNS_CACHE_आकार)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    अन्यथा
      अवरोध;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } जबतक(timeout_ms);

  dnscache_unlock(तथ्य, dnscache);
}

व्योम Curl_dnscache_clear(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  अगर(dnscache) {
    dnscache_lock(तथ्य, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(तथ्य, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
अटल CURLcode fetch_entry(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            काष्ठा Curl_peer *peer,
                            काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_dns_entry *dns = शून्य;
  काष्ठा dnsc_id id;
  काष्ठा dnsc_कुंजी कुंजी;
  अक्षर type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode परिणाम = CURLE_OK;

  *pdns = शून्य;
  अगर(!dnscache)
    वापस CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&कुंजी, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);

  अगर(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = शून्य;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  अगर(!dns && (type == CURL_DNST_ADDR) && तथ्य->state.wildcard_resolve) {
    काष्ठा Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&कुंजी, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);
  }

  अगर(dns && (तथ्य->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    काष्ठा dnscache_prune_तथ्य user;

    user.pnow = Curl_pgrs_now(तथ्य);
    user.max_age_ms = तथ्य->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    अगर(dnscache_entry_is_stale(&user, dns)) {
      infof(तथ्य, "Hostname in DNS cache was stale, zapped");
      dns = शून्य; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  अगर(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    अगर((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = शून्य;
    }
    अन्यथा अगर(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(तथ्य, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = शून्य;
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
    अन्यथा अगर(dns && !dns->addr) { /* negative entry */
      dns = शून्य;
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  वापस परिणाम;
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
CURLcode Curl_dnscache_get(काष्ठा Curl_easy *तथ्य,
                           uint8_t dns_queries,
                           काष्ठा Curl_peer *peer,
                           काष्ठा Curl_dns_entry **pentry)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  काष्ठा Curl_dns_entry *dns = शून्य;
  CURLcode परिणाम = CURLE_OK;

  dnscache_lock(तथ्य, dnscache);
  परिणाम = fetch_entry(तथ्य, dnscache, dns_queries, peer, &dns);
  अगर(!परिणाम && dns)
    dns->refcount++; /* we pass out a reference */
  अन्यथा अगर(परिणाम) {
    DEBUGASSERT(!dns);
    dns = शून्य;
  }
  dnscache_unlock(तथ्य, dnscache);

  CURL_TRC_DNS(तथ्य, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (पूर्णांक)परिणाम, dns ? "" : "not ");
  *pentry = dns;
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
अटल पूर्णांक अंक_addresses(स्थिर काष्ठा Curl_addrinfo *addr)
{
  पूर्णांक i = 0;
  जबतक(addr) {
    addr = addr->ai_अग्र;
    i++;
  }
  वापस i;
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
UNITTEST CURLcode dns_shuffle_addr(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_addrinfo **addr)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर पूर्णांक अंक_addrs = अंक_addresses(*addr);

  अगर(अंक_addrs > 1) {
    काष्ठा Curl_addrinfo **nodes;
    CURL_TRC_DNS(तथ्य, "Shuffling %d addresses", अंक_addrs);

    nodes = curlx_malloc(अंक_addrs * माप(*nodes));
    अगर(nodes) {
      पूर्णांक i;
      अचिन्हित पूर्णांक *rnd;
      स्थिर माप_प्रकार rnd_आकार = अंक_addrs * माप(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      क्रम(i = 1; i < अंक_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_अग्र;
      }

      rnd = curlx_malloc(rnd_आकार);
      अगर(rnd) {
        /* Fisher-Yates shuffle */
        अगर(Curl_rand(तथ्य, (अचिन्हित अक्षर *)rnd, rnd_आकार) == CURLE_OK) {
          काष्ठा Curl_addrinfo *swap_tmp;
          क्रम(i = अंक_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (अचिन्हित पूर्णांक)(i + 1)];
            nodes[rnd[i] % (अचिन्हित पूर्णांक)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          क्रम(i = 1; i < अंक_addrs; i++) {
            nodes[i - 1]->ai_अग्र = nodes[i];
          }

          nodes[अंक_addrs - 1]->ai_अग्र = शून्य;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      अन्यथा
        परिणाम = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    अन्यथा
      परिणाम = CURLE_OUT_OF_MEMORY;
  }
  वापस परिणाम;
}
#पूर्ण_अगर

अटल bool dnscache_ai_has_family(काष्ठा Curl_addrinfo *ai, पूर्णांक ai_family)
{
  क्रम(; ai; ai = ai->ai_अग्र) {
    अगर(ai->ai_family == ai_family)
      वापस TRUE;
  }
  वापस FALSE;
}

अटल काष्ठा Curl_dns_entry *dnsc_entry_create(काष्ठा dnsc_id *pid,
                                                bool permanent)
{
  काष्ठा Curl_dns_entry *dns = शून्य;

  अगर(curlx_strlen(&pid->name) > UINT16_MAX)
    जाओ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, माप(काष्ठा Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  अगर(!dns)
    जाओ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  अगर(dns->hostlen)
    स_नकल(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  वापस dns;
}

अटल काष्ठा Curl_dns_entry *dnsc_entry_assign_addr(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा Curl_dns_entry *dns,
  uint8_t dns_queries,
  काष्ठा Curl_addrinfo **paddr1,
  काष्ठा Curl_addrinfo **paddr2)
{
  अगर(!dns)
    जाओ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  अगर(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = शून्य;
  }
  अगर(paddr2 && *paddr2) {
    काष्ठा Curl_addrinfo **phead = &dns->addr;
    जबतक(*phead)
      phead = &(*phead)->ai_अग्र;
    *phead = *paddr2;
    *paddr2 = शून्य;
  }

  अगर((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#अगर_घोषित USE_IPV6
  अगर((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#पूर्ण_अगर /* USE_IPV6 */

#अगर_अघोषित CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  अगर(तथ्य->set.dns_shuffle_addresses && dns->addr) {
    CURLcode परिणाम = dns_shuffle_addr(तथ्य, &dns->addr);
    अगर(परिणाम) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = शून्य;
      जाओ out;
    }
  }
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर

out:
  अगर(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = शून्य;
  }
  अगर(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = शून्य;
  }
  वापस dns;
}

काष्ठा Curl_dns_entry *Curl_dnsc_mk_addr(काष्ठा Curl_easy *तथ्य,
                                         uint8_t dns_queries,
                                         काष्ठा Curl_addrinfo **paddr,
                                         काष्ठा Curl_peer *peer)
{
  काष्ठा dnsc_id id;
  काष्ठा Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(तथ्य, dns, dns_queries, paddr, शून्य);
  वापस dns;
}

काष्ठा Curl_dns_entry *Curl_dnsc_mk_addr2(काष्ठा Curl_easy *तथ्य,
                                          uint8_t dns_queries,
                                          काष्ठा Curl_addrinfo **paddr1,
                                          काष्ठा Curl_addrinfo **paddr2,
                                          काष्ठा Curl_peer *peer)
{
  काष्ठा dnsc_id id;
  काष्ठा Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(तथ्य, dns, dns_queries, paddr1, paddr2);
  वापस dns;
}

अटल काष्ठा Curl_dns_entry *dnsc_add_entry(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_dnscache *dnscache,
                                             काष्ठा dnsc_कुंजी *कुंजी,
                                             काष्ठा Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(तथ्य);
  वापस Curl_hash_add(&dnscache->entries, कुंजी->तथ्य, कुंजी->len, entry);
}

#अगर_घोषित USE_HTTPSRR
अटल काष्ठा Curl_dns_entry *dnsc_entry_assign_https(
  काष्ठा Curl_dns_entry *dns,
  काष्ठा Curl_https_rrinfo **phinfo)
{
  अगर(!dns)
    जाओ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  अगर(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = शून्य;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  अगर(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = शून्य;
  }
out:
  अगर(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = शून्य;
  }
  वापस dns;
}

काष्ठा Curl_dns_entry *Curl_dnsc_mk_https(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_https_rrinfo **phinfo,
                                          काष्ठा Curl_peer *peer)
{
  काष्ठा dnsc_id id;
  काष्ठा Curl_dns_entry *dns;

  (व्योम)तथ्य;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  वापस dns;
}

अटल काष्ठा Curl_dns_entry *dnsc_add_https(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_dnscache *dnscache,
                                             काष्ठा Curl_https_rrinfo **phinfo,
                                             काष्ठा dnsc_id *id)
{
  काष्ठा Curl_dns_entry *dns, *dns2;
  काष्ठा dnsc_कुंजी कुंजी;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  अगर(!dns)
    वापस शून्य;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुंजी, id);
  dns2 = dnsc_add_entry(तथ्य, dnscache, &कुंजी, dns);
  अगर(!dns2) {
    dnscache_entry_free(dns);
    वापस शून्य;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  वापस dns;
}
#पूर्ण_अगर /* USE_HTTPSRR */

अटल काष्ठा Curl_dns_entry *dnsc_add_addr(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            काष्ठा Curl_addrinfo **paddr,
                                            काष्ठा dnsc_id *id,
                                            काष्ठा dnsc_कुंजी *कुंजी,
                                            bool permanent)
{
  काष्ठा Curl_dns_entry *dns;
  काष्ठा Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(तथ्य, dns, dns_queries, paddr, शून्य);
  अगर(!dns)
    वापस शून्य;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(तथ्य, dnscache, कुंजी, dns);
  अगर(!dns2) {
    dnscache_entry_free(dns);
    वापस शून्य;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  वापस dns;
}

अटल काष्ठा Curl_dns_entry *dnsc_add_peer_addr(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा Curl_dnscache *dnscache,
  uint8_t dns_queries,
  काष्ठा Curl_addrinfo **paddr,
  काष्ठा dnsc_id *id,
  bool permanent)
{
  काष्ठा Curl_dns_entry *dns;
  काष्ठा Curl_dns_entry *dns2;
  काष्ठा dnsc_कुंजी कुंजी;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(तथ्य, dns, dns_queries, paddr, शून्य);
  अगर(!dns)
    वापस शून्य;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुंजी, id);
  dns2 = dnsc_add_entry(तथ्य, dnscache, &कुंजी, dns);
  अगर(!dns2) {
    dnscache_entry_free(dns);
    वापस शून्य;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  वापस dns;
}

CURLcode Curl_dnscache_add(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_dns_entry *entry)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  काष्ठा Curl_str name;
  काष्ठा dnsc_id id;
  काष्ठा dnsc_कुंजी कुंजी;

  अगर(!dnscache)
    वापस CURLE_FAILED_INIT;
  अगर(!entry || (entry->type == CURL_DNST_INIT))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&कुंजी, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(तथ्य, dnscache);
  अगर(!dnsc_add_entry(तथ्य, dnscache, &कुंजी, entry)) {
    dnscache_unlock(तथ्य, dnscache);
    वापस CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(तथ्य, dnscache);
  CURL_TRC_DNS(तथ्य, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  वापस CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(काष्ठा Curl_easy *तथ्य,
                                    uint8_t dns_queries,
                                    काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  काष्ठा Curl_dns_entry *dns = शून्य;
  काष्ठा dnsc_id id;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(dnscache);
  अगर(!dnscache)
    वापस CURLE_FAILED_INIT;

  dnscache_lock(तथ्य, dnscache);

  अगर(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(तथ्य, dnscache, dns_queries, शून्य, &id, FALSE);
    अगर(!dns)
      परिणाम = CURLE_OUT_OF_MEMORY;
  }
#अगर_घोषित USE_HTTPSRR
  अन्यथा अगर(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(तथ्य, dnscache, शून्य, &id);
    अगर(!dns)
      परिणाम = CURLE_OUT_OF_MEMORY;
  }
#पूर्ण_अगर
  अन्यथा {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  अगर(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(तथ्य, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(तथ्य, dnscache);
  वापस परिणाम;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
व्योम Curl_dns_entry_unlink(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_dns_entry **pdns)
{
  अगर(*pdns) {
    काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
    काष्ठा Curl_dns_entry *dns = *pdns;
    *pdns = शून्य;
    dnscache_lock(तथ्य, dnscache);
    dns->refcount--;
    अगर(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(तथ्य, dnscache);
  }
}

अटल व्योम dnscache_entry_dtor(व्योम *entry)
{
  काष्ठा Curl_dns_entry *dns = (काष्ठा Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  अगर(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
व्योम Curl_dnscache_init(काष्ठा Curl_dnscache *dns, माप_प्रकार आकार)
{
  Curl_hash_init(&dns->entries, आकार, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

व्योम Curl_dnscache_destroy(काष्ठा Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_dnscache *dnscache = dnscache_get(तथ्य);
  काष्ठा curl_slist *hostp;
  काष्ठा dnsc_id id;
  काष्ठा dnsc_कुंजी कुंजी;

  अगर(!dnscache)
    वापस CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  तथ्य->state.wildcard_resolve = FALSE;

  क्रम(hostp = तथ्य->state.resolve; hostp; hostp = hostp->अग्र) {
    स्थिर अक्षर *host = hostp->तथ्य;
    काष्ठा Curl_str source;
    अगर(!host)
      जारी;
    अगर(*host == '-') {
      curl_off_t अंक = 0;
      host++;
      अगर(!curlx_str_single(&host, '[')) {
        अगर(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          जारी;
      }
      अन्यथा {
        अगर(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          जारी;
        }
      }

      अगर(!curlx_str_संख्या(&host, &अंक, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)अंक);
        dnsc_id2key(&कुंजी, &id);
        dnscache_lock(तथ्य, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);
        dnscache_unlock(तथ्य, dnscache);
      }
    }
    अन्यथा {
      काष्ठा Curl_dns_entry *dns;
      काष्ठा Curl_addrinfo *शीर्ष = शून्य, *पुच्छ = शून्य;
      अक्षर address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(स्थिर अक्षर *addresses = शून्य);

      अगर(*host == '+') {
        host++;
        permanent = FALSE;
      }
      अगर(!curlx_str_single(&host, '[')) {
        अगर(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          जारी;
      }
      अन्यथा {
        अगर(curlx_str_until(&host, &source, 4096, ':'))
          जारी;
      }
      अगर(curlx_str_single(&host, ':') ||
         curlx_str_संख्या(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        जाओ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      जबतक(*host) {
        काष्ठा Curl_str target;
        काष्ठा Curl_addrinfo *ai;
        CURLcode परिणाम;

        अगर(!curlx_str_single(&host, '[')) {
          अगर(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            जाओ err;
        }
        अन्यथा {
          अगर(curlx_str_until(&host, &target, 4096, ',')) {
            अगर(curlx_str_single(&host, ','))
              जाओ err;
            /* survive nothing but a comma */
            जारी;
          }
        }
#अगर_अघोषित USE_IPV6
        अगर(स_प्रथम(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(तथ्य, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (पूर्णांक)curlx_strlen(&target), curlx_str(&target));
          अगर(curlx_str_single(&host, ','))
            जाओ err;
          जारी;
        }
#पूर्ण_अगर

        अगर(curlx_strlen(&target) >= माप(address))
          जाओ err;

        स_नकल(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        परिणाम = Curl_str2addr(address, port, &ai);
        अगर(परिणाम) {
          infof(तथ्य, "Resolve IP address '%s' found is illegal", address);
          जाओ err;
        }

        अगर(पुच्छ) {
          पुच्छ->ai_अग्र = ai;
          पुच्छ = पुच्छ->ai_अग्र;
        }
        अन्यथा {
          शीर्ष = पुच्छ = ai;
        }
        अगर(curlx_str_single(&host, ','))
          अवरोध;
      }

      अगर(!शीर्ष)
        जाओ err;

      error = FALSE;
err:
      अगर(error) {
        failf(तथ्य, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->तथ्य);
        Curl_freeaddrinfo(शीर्ष);
        वापस CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&कुंजी, &id);
      dnscache_lock(तथ्य, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);

      अगर(dns) {
        infof(तथ्य, "RESOLVE %.*s:%u - old addresses discarded",
              (पूर्णांक)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, कुंजी.तथ्य, कुंजी.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(तथ्य, dnscache, CURL_DNSQ_ADDR, &शीर्ष,
                          &id, &कुंजी, permanent);
      अगर(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(तथ्य, dnscache);

      अगर(!dns)
        वापस CURLE_OUT_OF_MEMORY;

      infof(तथ्य, "[DNS] added %.*s:%u:%s to cache%s",
            (पूर्णांक)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      अगर(curlx_str_casecompare(&source, "*")) {
        infof(तथ्य, "RESOLVE *:%u using wildcard", port);
        तथ्य->state.wildcard_resolve = TRUE;
      }
    }
  }
  तथ्य->state.resolve = शून्य; /* dealt with now */

  वापस CURLE_OK;
}
