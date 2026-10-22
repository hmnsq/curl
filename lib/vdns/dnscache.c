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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "hash.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "vdns/dnscache.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST शून्य dnsc_peer2id(संरचना dnsc_id *pid, वर्ण type,
                           संरचना Curl_peer *peer);
UNITTEST शून्य dnsc_peer2id(संरचना dnsc_id *pid, वर्ण type,
                           संरचना Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

स्थिर शून्य dnsc_str2id(संरचना dnsc_id *pid, वर्ण type,
                        संरचना Curl_str *name, uint16_t port)
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
UNITTEST शून्य dnsc_id2key(संरचना dnsc_कुञ्चिका *कुञ्चिका, संरचना dnsc_id *id);
UNITTEST शून्य dnsc_id2key(संरचना dnsc_कुञ्चिका *कुञ्चिका, संरचना dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  सचे(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  कुञ्चिका->data[0] = id->type;
  कुञ्चिका->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  कुञ्चिका->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((वर्ण *)कुञ्चिका->data + 3, curlx_str(&id->name), namelen);
  कुञ्चिका->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(संरचना dnsc_id *id,
                                    संरचना Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(संरचना dnsc_id *id,
                                    संरचना Curl_dns_entry *dns)
{
  पटिनिवत्त dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

स्थिर शून्य dnscache_entry_free(संरचना Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

संरचना dnscache_prune_data {
  ध्रुव संरचना curltime *pnow;
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
स्थिर पूर्णाङ्क dnscache_entry_is_stale(शून्य *datap, शून्य *hc)
{
  संरचना dnscache_prune_data *prune = (संरचना dnscache_prune_data *)datap;
  संरचना Curl_dns_entry *dns = (संरचना Curl_dns_entry *)hc;

  सचे(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    विकल्प(dns->type) {
    पक्ष CURL_DNST_ADDR:
      सचे(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      भङ्ग;
#ifdef USE_HTTPSRR
    पक्ष CURL_DNST_HTTPS:
      सचे(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      भङ्ग;
#endif
    स्वभाव:
      भङ्ग;
    }
    सचे(age_ms >= prune->max_age_ms)
      पटिनिवत्त TRUE;
    सचे(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  पटिनिवत्त FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
स्थिर timediff_t dnscache_prune(संरचना Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ध्रुव संरचना curltime *pnow)
{
  संरचना dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (शून्य *)&user,
                                 dnscache_entry_is_stale);

  पटिनिवत्त user.oldest_ms;
}

स्थिर संरचना Curl_dnscache *dnscache_get(संरचना Curl_easy *data)
{
  सचे(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    पटिनिवत्त &data->share->dnscache;
  सचे(data->multi)
    पटिनिवत्त &data->multi->dnscache;
  पटिनिवत्त रिक्त;
}

स्थिर शून्य dnscache_lock(संरचना Curl_easy *data,
                          संरचना Curl_dnscache *dnscache)
{
  सचे(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

स्थिर शून्य dnscache_unlock(संरचना Curl_easy *data,
                            संरचना Curl_dnscache *dnscache)
{
  सचे(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
शून्य Curl_dnscache_prune(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  सचे(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    पटिनिवत्त;

  dnscache_lock(data, dnscache);

  करोहि {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    सचे(Curl_hash_गणनम्(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    अञ्ञथा
      भङ्ग;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } याव(timeout_ms);

  dnscache_unlock(data, dnscache);
}

शून्य Curl_dnscache_clear(संरचना Curl_easy *data)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  सचे(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
स्थिर CURLcode fetch_entry(संरचना Curl_easy *data,
                            संरचना Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            संरचना Curl_peer *peer,
                            संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_dns_entry *dns = रिक्त;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;
  वर्ण type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = रिक्त;
  सचे(!dnscache)
    पटिनिवत्त CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&कुञ्चिका, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);

  सचे(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = रिक्त;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  सचे(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    संरचना Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&कुञ्चिका, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);
  }

  सचे(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    संरचना dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    सचे(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = रिक्त; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  सचे(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    सचे((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = रिक्त;
    }
    अञ्ञथा सचे(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = रिक्त;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    अञ्ञथा सचे(dns && !dns->addr) { /* negative entry */
      dns = रिक्त;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  पटिनिवत्त result;
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
CURLcode Curl_dnscache_get(संरचना Curl_easy *data,
                           uint8_t dns_queries,
                           संरचना Curl_peer *peer,
                           संरचना Curl_dns_entry **pentry)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  संरचना Curl_dns_entry *dns = रिक्त;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  सचे(!result && dns)
    dns->refcount++; /* we pass out a reference */
  अञ्ञथा सचे(result) {
    DEBUGASSERT(!dns);
    dns = रिक्त;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (पूर्णाङ्क)result, dns ? "" : "not ");
  *pentry = dns;
  पटिनिवत्त result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
स्थिर पूर्णाङ्क अङ्कः_addresses(ध्रुव संरचना Curl_addrinfo *addr)
{
  पूर्णाङ्क i = 0;
  याव(addr) {
    addr = addr->ai_next;
    i++;
  }
  पटिनिवत्त i;
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
UNITTEST CURLcode dns_shuffle_addr(संरचना Curl_easy *data,
                                   संरचना Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(संरचना Curl_easy *data,
                                   संरचना Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ध्रुव पूर्णाङ्क अङ्कः_addrs = अङ्कः_addresses(*addr);

  सचे(अङ्कः_addrs > 1) {
    संरचना Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", अङ्कः_addrs);

    nodes = curlx_malloc(अङ्कः_addrs * परिमाण(*nodes));
    सचे(nodes) {
      पूर्णाङ्क i;
      unsigned पूर्णाङ्क *rnd;
      ध्रुव size_t rnd_size = अङ्कः_addrs * परिमाण(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      अत्थाय(i = 1; i < अङ्कः_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      सचे(rnd) {
        /* Fisher-Yates shuffle */
        सचे(Curl_rand(data, (unsigned वर्ण *)rnd, rnd_size) == CURLE_OK) {
          संरचना Curl_addrinfo *swap_tmp;
          अत्थाय(i = अङ्कः_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned पूर्णाङ्क)(i + 1)];
            nodes[rnd[i] % (unsigned पूर्णाङ्क)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          अत्थाय(i = 1; i < अङ्कः_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[अङ्कः_addrs - 1]->ai_next = रिक्त;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      अञ्ञथा
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    अञ्ञथा
      result = CURLE_OUT_OF_MEMORY;
  }
  पटिनिवत्त result;
}
#endif

स्थिर bool dnscache_ai_has_family(संरचना Curl_addrinfo *ai, पूर्णाङ्क ai_family)
{
  अत्थाय(; ai; ai = ai->ai_next) {
    सचे(ai->ai_family == ai_family)
      पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}

स्थिर संरचना Curl_dns_entry *dnsc_entry_create(संरचना dnsc_id *pid,
                                                bool permanent)
{
  संरचना Curl_dns_entry *dns = रिक्त;

  सचे(curlx_strlen(&pid->name) > UINT16_MAX)
    गच्छ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, परिमाण(संरचना Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  सचे(!dns)
    गच्छ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  सचे(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  पटिनिवत्त dns;
}

स्थिर संरचना Curl_dns_entry *dnsc_entry_assign_addr(
  संरचना Curl_easy *data,
  संरचना Curl_dns_entry *dns,
  uint8_t dns_queries,
  संरचना Curl_addrinfo **paddr1,
  संरचना Curl_addrinfo **paddr2)
{
  सचे(!dns)
    गच्छ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  सचे(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = रिक्त;
  }
  सचे(paddr2 && *paddr2) {
    संरचना Curl_addrinfo **phead = &dns->addr;
    याव(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = रिक्त;
  }

  सचे((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  सचे((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  सचे(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    सचे(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = रिक्त;
      गच्छ out;
    }
  }
#अञ्ञथा
  (शून्य)data;
#endif

out:
  सचे(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = रिक्त;
  }
  सचे(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = रिक्त;
  }
  पटिनिवत्त dns;
}

संरचना Curl_dns_entry *Curl_dnsc_mk_addr(संरचना Curl_easy *data,
                                         uint8_t dns_queries,
                                         संरचना Curl_addrinfo **paddr,
                                         संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, रिक्त);
  पटिनिवत्त dns;
}

संरचना Curl_dns_entry *Curl_dnsc_mk_addr2(संरचना Curl_easy *data,
                                          uint8_t dns_queries,
                                          संरचना Curl_addrinfo **paddr1,
                                          संरचना Curl_addrinfo **paddr2,
                                          संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  पटिनिवत्त dns;
}

स्थिर संरचना Curl_dns_entry *dnsc_add_entry(संरचना Curl_easy *data,
                                             संरचना Curl_dnscache *dnscache,
                                             संरचना dnsc_कुञ्चिका *कुञ्चिका,
                                             संरचना Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  पटिनिवत्त Curl_hash_add(&dnscache->entries, कुञ्चिका->data, कुञ्चिका->len, entry);
}

#ifdef USE_HTTPSRR
स्थिर संरचना Curl_dns_entry *dnsc_entry_assign_https(
  संरचना Curl_dns_entry *dns,
  संरचना Curl_https_rrinfo **phinfo)
{
  सचे(!dns)
    गच्छ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  सचे(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = रिक्त;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  सचे(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = रिक्त;
  }
out:
  सचे(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = रिक्त;
  }
  पटिनिवत्त dns;
}

संरचना Curl_dns_entry *Curl_dnsc_mk_https(संरचना Curl_easy *data,
                                          संरचना Curl_https_rrinfo **phinfo,
                                          संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_entry *dns;

  (शून्य)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  पटिनिवत्त dns;
}

स्थिर संरचना Curl_dns_entry *dnsc_add_https(संरचना Curl_easy *data,
                                             संरचना Curl_dnscache *dnscache,
                                             संरचना Curl_https_rrinfo **phinfo,
                                             संरचना dnsc_id *id)
{
  संरचना Curl_dns_entry *dns, *dns2;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  सचे(!dns)
    पटिनिवत्त रिक्त;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुञ्चिका, id);
  dns2 = dnsc_add_entry(data, dnscache, &कुञ्चिका, dns);
  सचे(!dns2) {
    dnscache_entry_free(dns);
    पटिनिवत्त रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  पटिनिवत्त dns;
}
#endif /* USE_HTTPSRR */

स्थिर संरचना Curl_dns_entry *dnsc_add_addr(संरचना Curl_easy *data,
                                            संरचना Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            संरचना Curl_addrinfo **paddr,
                                            संरचना dnsc_id *id,
                                            संरचना dnsc_कुञ्चिका *कुञ्चिका,
                                            bool permanent)
{
  संरचना Curl_dns_entry *dns;
  संरचना Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, रिक्त);
  सचे(!dns)
    पटिनिवत्त रिक्त;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, कुञ्चिका, dns);
  सचे(!dns2) {
    dnscache_entry_free(dns);
    पटिनिवत्त रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  पटिनिवत्त dns;
}

स्थिर संरचना Curl_dns_entry *dnsc_add_peer_addr(
  संरचना Curl_easy *data,
  संरचना Curl_dnscache *dnscache,
  uint8_t dns_queries,
  संरचना Curl_addrinfo **paddr,
  संरचना dnsc_id *id,
  bool permanent)
{
  संरचना Curl_dns_entry *dns;
  संरचना Curl_dns_entry *dns2;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, रिक्त);
  सचे(!dns)
    पटिनिवत्त रिक्त;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुञ्चिका, id);
  dns2 = dnsc_add_entry(data, dnscache, &कुञ्चिका, dns);
  सचे(!dns2) {
    dnscache_entry_free(dns);
    पटिनिवत्त रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  पटिनिवत्त dns;
}

CURLcode Curl_dnscache_add(संरचना Curl_easy *data,
                           संरचना Curl_dns_entry *entry)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  संरचना Curl_str name;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  सचे(!dnscache)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(!entry || (entry->type == CURL_DNST_INIT))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&कुञ्चिका, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  सचे(!dnsc_add_entry(data, dnscache, &कुञ्चिका, entry)) {
    dnscache_unlock(data, dnscache);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(संरचना Curl_easy *data,
                                    uint8_t dns_queries,
                                    संरचना Curl_peer *peer)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  संरचना Curl_dns_entry *dns = रिक्त;
  संरचना dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  सचे(!dnscache)
    पटिनिवत्त CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  सचे(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, रिक्त, &id, FALSE);
    सचे(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  अञ्ञथा सचे(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, रिक्त, &id);
    सचे(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  अञ्ञथा {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  सचे(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  पटिनिवत्त result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
शून्य Curl_dns_entry_unlink(संरचना Curl_easy *data,
                           संरचना Curl_dns_entry **pdns)
{
  सचे(*pdns) {
    संरचना Curl_dnscache *dnscache = dnscache_get(data);
    संरचना Curl_dns_entry *dns = *pdns;
    *pdns = रिक्त;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    सचे(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

स्थिर शून्य dnscache_entry_dtor(शून्य *entry)
{
  संरचना Curl_dns_entry *dns = (संरचना Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  सचे(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
शून्य Curl_dnscache_init(संरचना Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

शून्य Curl_dnscache_destroy(संरचना Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(संरचना Curl_easy *data)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(data);
  संरचना curl_slist *hostp;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  सचे(!dnscache)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  अत्थाय(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ध्रुव वर्ण *host = hostp->data;
    संरचना Curl_str source;
    सचे(!host)
      अनुवर्त;
    सचे(*host == '-') {
      curl_off_t अङ्कः = 0;
      host++;
      सचे(!curlx_str_single(&host, '[')) {
        सचे(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          अनुवर्त;
      }
      अञ्ञथा {
        सचे(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          अनुवर्त;
        }
      }

      सचे(!curlx_str_सङ्ख्या(&host, &अङ्कः, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)अङ्कः);
        dnsc_id2key(&कुञ्चिका, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);
        dnscache_unlock(data, dnscache);
      }
    }
    अञ्ञथा {
      संरचना Curl_dns_entry *dns;
      संरचना Curl_addrinfo *head = रिक्त, *tail = रिक्त;
      वर्ण address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ध्रुव वर्ण *addresses = रिक्त);

      सचे(*host == '+') {
        host++;
        permanent = FALSE;
      }
      सचे(!curlx_str_single(&host, '[')) {
        सचे(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          अनुवर्त;
      }
      अञ्ञथा {
        सचे(curlx_str_until(&host, &source, 4096, ':'))
          अनुवर्त;
      }
      सचे(curlx_str_single(&host, ':') ||
         curlx_str_सङ्ख्या(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        गच्छ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      याव(*host) {
        संरचना Curl_str target;
        संरचना Curl_addrinfo *ai;
        CURLcode result;

        सचे(!curlx_str_single(&host, '[')) {
          सचे(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            गच्छ err;
        }
        अञ्ञथा {
          सचे(curlx_str_until(&host, &target, 4096, ',')) {
            सचे(curlx_str_single(&host, ','))
              गच्छ err;
            /* survive nothing but a comma */
            अनुवर्त;
          }
        }
#ifndef USE_IPV6
        सचे(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (पूर्णाङ्क)curlx_strlen(&target), curlx_str(&target));
          सचे(curlx_str_single(&host, ','))
            गच्छ err;
          अनुवर्त;
        }
#endif

        सचे(curlx_strlen(&target) >= परिमाण(address))
          गच्छ err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        सचे(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          गच्छ err;
        }

        सचे(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        अञ्ञथा {
          head = tail = ai;
        }
        सचे(curlx_str_single(&host, ','))
          भङ्ग;
      }

      सचे(!head)
        गच्छ err;

      error = FALSE;
err:
      सचे(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        पटिनिवत्त CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&कुञ्चिका, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);

      सचे(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (पूर्णाङ्क)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, कुञ्चिका.data, कुञ्चिका.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &कुञ्चिका, permanent);
      सचे(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      सचे(!dns)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (पूर्णाङ्क)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      सचे(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = रिक्त; /* dealt with now */

  पटिनिवत्त CURLE_OK;
}
