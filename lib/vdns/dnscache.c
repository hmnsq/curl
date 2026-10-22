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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "urldata.h"
#include "curl_addrinfo.h"
#include "curl_share.h"
#include "curl_trc.h"
#include "hash.h"
#include "progress.h"
#include "rand.h"
#include "strcase.h"
#include "vdns/dnscache.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"
#include "curlx/inet_ntop.h"
#include "curlx/inet_pton.h"
#include "curlx/strcopy.h"
#include "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST void dnsc_peer2id(sa_nracanaa dnsc_id *pid, var_na type,
                           sa_nracanaa Curl_peer *peer);
UNITTEST void dnsc_peer2id(sa_nracanaa dnsc_id *pid, var_na type,
                           sa_nracanaa Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

static void dnsc_str2id(sa_nracanaa dnsc_id *pid, var_na type,
                        sa_nracanaa Curl_str *name, uint16_t port)
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
UNITTEST void dnsc_id2key(sa_nracanaa dnsc_key *key, sa_nracanaa dnsc_id *id);
UNITTEST void dnsc_id2key(sa_nracanaa dnsc_key *key, sa_nracanaa dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  གལ་སྲིད(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  key->data[0] = id->type;
  key->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  key->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((var_na *)key->data + 3, curlx_str(&id->name), namelen);
  key->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(sa_nracanaa dnsc_id *id,
                                    sa_nracanaa Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(sa_nracanaa dnsc_id *id,
                                    sa_nracanaa Curl_dns_entry *dns)
{
  ལོག dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

static void dnscache_entry_free(sa_nracanaa Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

sa_nracanaa dnscache_prune_data {
  const sa_nracanaa curltime *pnow;
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
static ཧྲིལ dnscache_entry_is_stale(void *datap, void *hc)
{
  sa_nracanaa dnscache_prune_data *prune = (sa_nracanaa dnscache_prune_data *)datap;
  sa_nracanaa Curl_dns_entry *dns = (sa_nracanaa Curl_dns_entry *)hc;

  གལ་སྲིད(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    switch(dns->type) {
    pak_sa CURL_DNST_ADDR:
      གལ་སྲིད(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      bha_nga;
#ifdef USE_HTTPSRR
    pak_sa CURL_DNST_HTTPS:
      གལ་སྲིད(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      bha_nga;
#endif
    default:
      bha_nga;
    }
    གལ་སྲིད(age_ms >= prune->max_age_ms)
      ལོག TRUE;
    གལ་སྲིད(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ལོག FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
static timediff_t dnscache_prune(sa_nracanaa Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 const sa_nracanaa curltime *pnow)
{
  sa_nracanaa dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (void *)&user,
                                 dnscache_entry_is_stale);

  ལོག user.oldest_ms;
}

static sa_nracanaa Curl_dnscache *dnscache_get(sa_nracanaa Curl_easy *data)
{
  གལ་སྲིད(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    ལོག &data->share->dnscache;
  གལ་སྲིད(data->multi)
    ལོག &data->multi->dnscache;
  ལོག NULL;
}

static void dnscache_lock(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_dnscache *dnscache)
{
  གལ་སྲིད(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

static void dnscache_unlock(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_dnscache *dnscache)
{
  གལ་སྲིད(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
void Curl_dnscache_prune(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  གལ་སྲིད(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ལོག;

  dnscache_lock(data, dnscache);

  do {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    གལ་སྲིད(Curl_hash_count(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    དེ་མིན
      bha_nga;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } སྐབས(timeout_ms);

  dnscache_unlock(data, dnscache);
}

void Curl_dnscache_clear(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  གལ་སྲིད(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
static CURLcode fetch_entry(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            sa_nracanaa Curl_peer *peer,
                            sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa Curl_dns_entry *dns = NULL;
  sa_nracanaa dnsc_id id;
  sa_nracanaa dnsc_key key;
  var_na type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = NULL;
  གལ་སྲིད(!dnscache)
    ལོག CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&key, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);

  གལ་སྲིད(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = NULL;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  གལ་སྲིད(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    sa_nracanaa Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&key, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);
  }

  གལ་སྲིད(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    sa_nracanaa dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    གལ་སྲིད(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = NULL; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, key.data, key.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  གལ་སྲིད(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    གལ་སྲིད((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = NULL;
    }
    དེ་མིན གལ་སྲིད(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = NULL;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    དེ་མིན གལ་སྲིད(dns && !dns->addr) { /* negative entry */
      dns = NULL;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ལོག result;
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
CURLcode Curl_dnscache_get(sa_nracanaa Curl_easy *data,
                           uint8_t dns_queries,
                           sa_nracanaa Curl_peer *peer,
                           sa_nracanaa Curl_dns_entry **pentry)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  sa_nracanaa Curl_dns_entry *dns = NULL;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  གལ་སྲིད(!result && dns)
    dns->refcount++; /* we pass out a reference */
  དེ་མིན གལ་སྲིད(result) {
    DEBUGASSERT(!dns);
    dns = NULL;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (ཧྲིལ)result, dns ? "" : "not ");
  *pentry = dns;
  ལོག result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
static ཧྲིལ num_addresses(const sa_nracanaa Curl_addrinfo *addr)
{
  ཧྲིལ i = 0;
  སྐབས(addr) {
    addr = addr->ai_next;
    i++;
  }
  ལོག i;
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
UNITTEST CURLcode dns_shuffle_addr(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  const ཧྲིལ num_addrs = num_addresses(*addr);

  གལ་སྲིད(num_addrs > 1) {
    sa_nracanaa Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", num_addrs);

    nodes = curlx_malloc(num_addrs * parimaa_na(*nodes));
    གལ་སྲིད(nodes) {
      ཧྲིལ i;
      unsigned ཧྲིལ *rnd;
      const size_t rnd_size = num_addrs * parimaa_na(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      དོན་ལུ(i = 1; i < num_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      གལ་སྲིད(rnd) {
        /* Fisher-Yates shuffle */
        གལ་སྲིད(Curl_rand(data, (unsigned var_na *)rnd, rnd_size) == CURLE_OK) {
          sa_nracanaa Curl_addrinfo *swap_tmp;
          དོན་ལུ(i = num_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned ཧྲིལ)(i + 1)];
            nodes[rnd[i] % (unsigned ཧྲིལ)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          དོན་ལུ(i = 1; i < num_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[num_addrs - 1]->ai_next = NULL;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      དེ་མིན
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    དེ་མིན
      result = CURLE_OUT_OF_MEMORY;
  }
  ལོག result;
}
#endif

static bool dnscache_ai_has_family(sa_nracanaa Curl_addrinfo *ai, ཧྲིལ ai_family)
{
  དོན་ལུ(; ai; ai = ai->ai_next) {
    གལ་སྲིད(ai->ai_family == ai_family)
      ལོག TRUE;
  }
  ལོག FALSE;
}

static sa_nracanaa Curl_dns_entry *dnsc_entry_create(sa_nracanaa dnsc_id *pid,
                                                bool permanent)
{
  sa_nracanaa Curl_dns_entry *dns = NULL;

  གལ་སྲིད(curlx_strlen(&pid->name) > UINT16_MAX)
    goto out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  གལ་སྲིད(!dns)
    goto out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  གལ་སྲིད(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  ལོག dns;
}

static sa_nracanaa Curl_dns_entry *dnsc_entry_assign_addr(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa Curl_dns_entry *dns,
  uint8_t dns_queries,
  sa_nracanaa Curl_addrinfo **paddr1,
  sa_nracanaa Curl_addrinfo **paddr2)
{
  གལ་སྲིད(!dns)
    goto out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  གལ་སྲིད(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = NULL;
  }
  གལ་སྲིད(paddr2 && *paddr2) {
    sa_nracanaa Curl_addrinfo **phead = &dns->addr;
    སྐབས(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = NULL;
  }

  གལ་སྲིད((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  གལ་སྲིད((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  གལ་སྲིད(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    གལ་སྲིད(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = NULL;
      goto out;
    }
  }
#དེ་མིན
  (void)data;
#endif

out:
  གལ་སྲིད(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = NULL;
  }
  གལ་སྲིད(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = NULL;
  }
  ལོག dns;
}

sa_nracanaa Curl_dns_entry *Curl_dnsc_mk_addr(sa_nracanaa Curl_easy *data,
                                         uint8_t dns_queries,
                                         sa_nracanaa Curl_addrinfo **paddr,
                                         sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa dnsc_id id;
  sa_nracanaa Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  ལོག dns;
}

sa_nracanaa Curl_dns_entry *Curl_dnsc_mk_addr2(sa_nracanaa Curl_easy *data,
                                          uint8_t dns_queries,
                                          sa_nracanaa Curl_addrinfo **paddr1,
                                          sa_nracanaa Curl_addrinfo **paddr2,
                                          sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa dnsc_id id;
  sa_nracanaa Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  ལོག dns;
}

static sa_nracanaa Curl_dns_entry *dnsc_add_entry(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa Curl_dnscache *dnscache,
                                             sa_nracanaa dnsc_key *key,
                                             sa_nracanaa Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  ལོག Curl_hash_add(&dnscache->entries, key->data, key->len, entry);
}

#ifdef USE_HTTPSRR
static sa_nracanaa Curl_dns_entry *dnsc_entry_assign_https(
  sa_nracanaa Curl_dns_entry *dns,
  sa_nracanaa Curl_https_rrinfo **phinfo)
{
  གལ་སྲིད(!dns)
    goto out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  གལ་སྲིད(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = NULL;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  གལ་སྲིད(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = NULL;
  }
out:
  གལ་སྲིད(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = NULL;
  }
  ལོག dns;
}

sa_nracanaa Curl_dns_entry *Curl_dnsc_mk_https(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_https_rrinfo **phinfo,
                                          sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa dnsc_id id;
  sa_nracanaa Curl_dns_entry *dns;

  (void)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ལོག dns;
}

static sa_nracanaa Curl_dns_entry *dnsc_add_https(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa Curl_dnscache *dnscache,
                                             sa_nracanaa Curl_https_rrinfo **phinfo,
                                             sa_nracanaa dnsc_id *id)
{
  sa_nracanaa Curl_dns_entry *dns, *dns2;
  sa_nracanaa dnsc_key key;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  གལ་སྲིད(!dns)
    ལོག NULL;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&key, id);
  dns2 = dnsc_add_entry(data, dnscache, &key, dns);
  གལ་སྲིད(!dns2) {
    dnscache_entry_free(dns);
    ལོག NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ལོག dns;
}
#endif /* USE_HTTPSRR */

static sa_nracanaa Curl_dns_entry *dnsc_add_addr(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            sa_nracanaa Curl_addrinfo **paddr,
                                            sa_nracanaa dnsc_id *id,
                                            sa_nracanaa dnsc_key *key,
                                            bool permanent)
{
  sa_nracanaa Curl_dns_entry *dns;
  sa_nracanaa Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  གལ་སྲིད(!dns)
    ལོག NULL;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, key, dns);
  གལ་སྲིད(!dns2) {
    dnscache_entry_free(dns);
    ལོག NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ལོག dns;
}

static sa_nracanaa Curl_dns_entry *dnsc_add_peer_addr(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa Curl_dnscache *dnscache,
  uint8_t dns_queries,
  sa_nracanaa Curl_addrinfo **paddr,
  sa_nracanaa dnsc_id *id,
  bool permanent)
{
  sa_nracanaa Curl_dns_entry *dns;
  sa_nracanaa Curl_dns_entry *dns2;
  sa_nracanaa dnsc_key key;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  གལ་སྲིད(!dns)
    ལོག NULL;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&key, id);
  dns2 = dnsc_add_entry(data, dnscache, &key, dns);
  གལ་སྲིད(!dns2) {
    dnscache_entry_free(dns);
    ལོག NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ལོག dns;
}

CURLcode Curl_dnscache_add(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_dns_entry *entry)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  sa_nracanaa Curl_str name;
  sa_nracanaa dnsc_id id;
  sa_nracanaa dnsc_key key;

  གལ་སྲིད(!dnscache)
    ལོག CURLE_FAILED_INIT;
  གལ་སྲིད(!entry || (entry->type == CURL_DNST_INIT))
    ལོག CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&key, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  གལ་སྲིད(!dnsc_add_entry(data, dnscache, &key, entry)) {
    dnscache_unlock(data, dnscache);
    ལོག CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  ལོག CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(sa_nracanaa Curl_easy *data,
                                    uint8_t dns_queries,
                                    sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  sa_nracanaa Curl_dns_entry *dns = NULL;
  sa_nracanaa dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  གལ་སྲིད(!dnscache)
    ལོག CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  གལ་སྲིད(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, NULL, &id, FALSE);
    གལ་སྲིད(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  དེ་མིན གལ་སྲིད(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, NULL, &id);
    གལ་སྲིད(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  དེ་མིན {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  གལ་སྲིད(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  ལོག result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
void Curl_dns_entry_unlink(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_dns_entry **pdns)
{
  གལ་སྲིད(*pdns) {
    sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
    sa_nracanaa Curl_dns_entry *dns = *pdns;
    *pdns = NULL;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    གལ་སྲིད(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

static void dnscache_entry_dtor(void *entry)
{
  sa_nracanaa Curl_dns_entry *dns = (sa_nracanaa Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  གལ་སྲིད(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
void Curl_dnscache_init(sa_nracanaa Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

void Curl_dnscache_destroy(sa_nracanaa Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_dnscache *dnscache = dnscache_get(data);
  sa_nracanaa curl_slist *hostp;
  sa_nracanaa dnsc_id id;
  sa_nracanaa dnsc_key key;

  གལ་སྲིད(!dnscache)
    ལོག CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  དོན་ལུ(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    const var_na *host = hostp->data;
    sa_nracanaa Curl_str source;
    གལ་སྲིད(!host)
      continue;
    གལ་སྲིད(*host == '-') {
      curl_off_t num = 0;
      host++;
      གལ་སྲིད(!curlx_str_single(&host, '[')) {
        གལ་སྲིད(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          continue;
      }
      དེ་མིན {
        གལ་སྲིད(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          continue;
        }
      }

      གལ་སྲིད(!curlx_str_number(&host, &num, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)num);
        dnsc_id2key(&key, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, key.data, key.len);
        dnscache_unlock(data, dnscache);
      }
    }
    དེ་མིན {
      sa_nracanaa Curl_dns_entry *dns;
      sa_nracanaa Curl_addrinfo *head = NULL, *tail = NULL;
      var_na address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(const var_na *addresses = NULL);

      གལ་སྲིད(*host == '+') {
        host++;
        permanent = FALSE;
      }
      གལ་སྲིད(!curlx_str_single(&host, '[')) {
        གལ་སྲིད(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          continue;
      }
      དེ་མིན {
        གལ་སྲིད(curlx_str_until(&host, &source, 4096, ':'))
          continue;
      }
      གལ་སྲིད(curlx_str_single(&host, ':') ||
         curlx_str_number(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        goto err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      སྐབས(*host) {
        sa_nracanaa Curl_str target;
        sa_nracanaa Curl_addrinfo *ai;
        CURLcode result;

        གལ་སྲིད(!curlx_str_single(&host, '[')) {
          གལ་སྲིད(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            goto err;
        }
        དེ་མིན {
          གལ་སྲིད(curlx_str_until(&host, &target, 4096, ',')) {
            གལ་སྲིད(curlx_str_single(&host, ','))
              goto err;
            /* survive nothing but a comma */
            continue;
          }
        }
#ifndef USE_IPV6
        གལ་སྲིད(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (ཧྲིལ)curlx_strlen(&target), curlx_str(&target));
          གལ་སྲིད(curlx_str_single(&host, ','))
            goto err;
          continue;
        }
#endif

        གལ་སྲིད(curlx_strlen(&target) >= parimaa_na(address))
          goto err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        གལ་སྲིད(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          goto err;
        }

        གལ་སྲིད(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        དེ་མིན {
          head = tail = ai;
        }
        གལ་སྲིད(curlx_str_single(&host, ','))
          bha_nga;
      }

      གལ་སྲིད(!head)
        goto err;

      error = FALSE;
err:
      གལ་སྲིད(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        ལོག CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&key, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);

      གལ་སྲིད(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (ཧྲིལ)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, key.data, key.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &key, permanent);
      གལ་སྲིད(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      གལ་སྲིད(!dns)
        ལོག CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (ཧྲིལ)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      གལ་སྲིད(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = NULL; /* dealt with now */

  ལོག CURLE_OK;
}
