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
UNITTEST void dnsc_peer2id(struct dnsc_id *pid, char type,
                           struct Curl_peer *peer);
UNITTEST void dnsc_peer2id(struct dnsc_id *pid, char type,
                           struct Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

static void dnsc_str2id(struct dnsc_id *pid, char type,
                        struct Curl_str *name, uint16_t port)
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
UNITTEST void dnsc_id2key(struct dnsc_key *key, struct dnsc_id *id);
UNITTEST void dnsc_id2key(struct dnsc_key *key, struct dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  если(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  key->data[0] = id->type;
  key->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  key->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((char *)key->data + 3, curlx_str(&id->name), namelen);
  key->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(struct dnsc_id *id,
                                    struct Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(struct dnsc_id *id,
                                    struct Curl_dns_entry *dns)
{
  вернуть dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

static void dnscache_entry_free(struct Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

struct dnscache_prune_data {
  const struct curltime *pnow;
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
static целое dnscache_entry_is_stale(void *datap, void *hc)
{
  struct dnscache_prune_data *prune = (struct dnscache_prune_data *)datap;
  struct Curl_dns_entry *dns = (struct Curl_dns_entry *)hc;

  если(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    switch(dns->type) {
    case CURL_DNST_ADDR:
      если(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      break;
#ifdef USE_HTTPSRR
    case CURL_DNST_HTTPS:
      если(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      break;
#endif
    default:
      break;
    }
    если(age_ms >= prune->max_age_ms)
      вернуть TRUE;
    если(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  вернуть FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
static timediff_t dnscache_prune(struct Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 const struct curltime *pnow)
{
  struct dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (void *)&user,
                                 dnscache_entry_is_stale);

  вернуть user.oldest_ms;
}

static struct Curl_dnscache *dnscache_get(struct Curl_easy *data)
{
  если(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    вернуть &data->share->dnscache;
  если(data->multi)
    вернуть &data->multi->dnscache;
  вернуть NULL;
}

static void dnscache_lock(struct Curl_easy *data,
                          struct Curl_dnscache *dnscache)
{
  если(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

static void dnscache_unlock(struct Curl_easy *data,
                            struct Curl_dnscache *dnscache)
{
  если(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
void Curl_dnscache_prune(struct Curl_easy *data, const struct curltime *pnow)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  если(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    вернуть;

  dnscache_lock(data, dnscache);

  do {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    если(Curl_hash_count(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    иначе
      break;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } пока(timeout_ms);

  dnscache_unlock(data, dnscache);
}

void Curl_dnscache_clear(struct Curl_easy *data)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  если(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
static CURLcode fetch_entry(struct Curl_easy *data,
                            struct Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            struct Curl_peer *peer,
                            struct Curl_dns_entry **pdns)
{
  struct Curl_dns_entry *dns = NULL;
  struct dnsc_id id;
  struct dnsc_key key;
  char type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = NULL;
  если(!dnscache)
    вернуть CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&key, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);

  если(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = NULL;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  если(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    struct Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&key, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);
  }

  если(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    struct dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    если(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = NULL; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, key.data, key.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  если(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    если((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = NULL;
    }
    иначе если(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = NULL;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    иначе если(dns && !dns->addr) { /* negative entry */
      dns = NULL;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  вернуть result;
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
CURLcode Curl_dnscache_get(struct Curl_easy *data,
                           uint8_t dns_queries,
                           struct Curl_peer *peer,
                           struct Curl_dns_entry **pentry)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  struct Curl_dns_entry *dns = NULL;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  если(!result && dns)
    dns->refcount++; /* we pass out a reference */
  иначе если(result) {
    DEBUGASSERT(!dns);
    dns = NULL;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (целое)result, dns ? "" : "not ");
  *pentry = dns;
  вернуть result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
static целое num_addresses(const struct Curl_addrinfo *addr)
{
  целое i = 0;
  пока(addr) {
    addr = addr->ai_next;
    i++;
  }
  вернуть i;
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
UNITTEST CURLcode dns_shuffle_addr(struct Curl_easy *data,
                                   struct Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(struct Curl_easy *data,
                                   struct Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  const целое num_addrs = num_addresses(*addr);

  если(num_addrs > 1) {
    struct Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", num_addrs);

    nodes = curlx_malloc(num_addrs * sizeof(*nodes));
    если(nodes) {
      целое i;
      unsigned целое *rnd;
      const size_t rnd_size = num_addrs * sizeof(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      для(i = 1; i < num_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      если(rnd) {
        /* Fisher-Yates shuffle */
        если(Curl_rand(data, (unsigned char *)rnd, rnd_size) == CURLE_OK) {
          struct Curl_addrinfo *swap_tmp;
          для(i = num_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned целое)(i + 1)];
            nodes[rnd[i] % (unsigned целое)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          для(i = 1; i < num_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[num_addrs - 1]->ai_next = NULL;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      иначе
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    иначе
      result = CURLE_OUT_OF_MEMORY;
  }
  вернуть result;
}
#endif

static bool dnscache_ai_has_family(struct Curl_addrinfo *ai, целое ai_family)
{
  для(; ai; ai = ai->ai_next) {
    если(ai->ai_family == ai_family)
      вернуть TRUE;
  }
  вернуть FALSE;
}

static struct Curl_dns_entry *dnsc_entry_create(struct dnsc_id *pid,
                                                bool permanent)
{
  struct Curl_dns_entry *dns = NULL;

  если(curlx_strlen(&pid->name) > UINT16_MAX)
    goto out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, sizeof(struct Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  если(!dns)
    goto out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  если(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  вернуть dns;
}

static struct Curl_dns_entry *dnsc_entry_assign_addr(
  struct Curl_easy *data,
  struct Curl_dns_entry *dns,
  uint8_t dns_queries,
  struct Curl_addrinfo **paddr1,
  struct Curl_addrinfo **paddr2)
{
  если(!dns)
    goto out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  если(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = NULL;
  }
  если(paddr2 && *paddr2) {
    struct Curl_addrinfo **phead = &dns->addr;
    пока(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = NULL;
  }

  если((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  если((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  если(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    если(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = NULL;
      goto out;
    }
  }
#иначе
  (void)data;
#endif

out:
  если(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = NULL;
  }
  если(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = NULL;
  }
  вернуть dns;
}

struct Curl_dns_entry *Curl_dnsc_mk_addr(struct Curl_easy *data,
                                         uint8_t dns_queries,
                                         struct Curl_addrinfo **paddr,
                                         struct Curl_peer *peer)
{
  struct dnsc_id id;
  struct Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  вернуть dns;
}

struct Curl_dns_entry *Curl_dnsc_mk_addr2(struct Curl_easy *data,
                                          uint8_t dns_queries,
                                          struct Curl_addrinfo **paddr1,
                                          struct Curl_addrinfo **paddr2,
                                          struct Curl_peer *peer)
{
  struct dnsc_id id;
  struct Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  вернуть dns;
}

static struct Curl_dns_entry *dnsc_add_entry(struct Curl_easy *data,
                                             struct Curl_dnscache *dnscache,
                                             struct dnsc_key *key,
                                             struct Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  вернуть Curl_hash_add(&dnscache->entries, key->data, key->len, entry);
}

#ifdef USE_HTTPSRR
static struct Curl_dns_entry *dnsc_entry_assign_https(
  struct Curl_dns_entry *dns,
  struct Curl_https_rrinfo **phinfo)
{
  если(!dns)
    goto out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  если(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = NULL;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  если(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = NULL;
  }
out:
  если(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = NULL;
  }
  вернуть dns;
}

struct Curl_dns_entry *Curl_dnsc_mk_https(struct Curl_easy *data,
                                          struct Curl_https_rrinfo **phinfo,
                                          struct Curl_peer *peer)
{
  struct dnsc_id id;
  struct Curl_dns_entry *dns;

  (void)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  вернуть dns;
}

static struct Curl_dns_entry *dnsc_add_https(struct Curl_easy *data,
                                             struct Curl_dnscache *dnscache,
                                             struct Curl_https_rrinfo **phinfo,
                                             struct dnsc_id *id)
{
  struct Curl_dns_entry *dns, *dns2;
  struct dnsc_key key;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  если(!dns)
    вернуть NULL;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&key, id);
  dns2 = dnsc_add_entry(data, dnscache, &key, dns);
  если(!dns2) {
    dnscache_entry_free(dns);
    вернуть NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  вернуть dns;
}
#endif /* USE_HTTPSRR */

static struct Curl_dns_entry *dnsc_add_addr(struct Curl_easy *data,
                                            struct Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            struct Curl_addrinfo **paddr,
                                            struct dnsc_id *id,
                                            struct dnsc_key *key,
                                            bool permanent)
{
  struct Curl_dns_entry *dns;
  struct Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  если(!dns)
    вернуть NULL;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, key, dns);
  если(!dns2) {
    dnscache_entry_free(dns);
    вернуть NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  вернуть dns;
}

static struct Curl_dns_entry *dnsc_add_peer_addr(
  struct Curl_easy *data,
  struct Curl_dnscache *dnscache,
  uint8_t dns_queries,
  struct Curl_addrinfo **paddr,
  struct dnsc_id *id,
  bool permanent)
{
  struct Curl_dns_entry *dns;
  struct Curl_dns_entry *dns2;
  struct dnsc_key key;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, NULL);
  если(!dns)
    вернуть NULL;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&key, id);
  dns2 = dnsc_add_entry(data, dnscache, &key, dns);
  если(!dns2) {
    dnscache_entry_free(dns);
    вернуть NULL;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  вернуть dns;
}

CURLcode Curl_dnscache_add(struct Curl_easy *data,
                           struct Curl_dns_entry *entry)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  struct Curl_str name;
  struct dnsc_id id;
  struct dnsc_key key;

  если(!dnscache)
    вернуть CURLE_FAILED_INIT;
  если(!entry || (entry->type == CURL_DNST_INIT))
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&key, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  если(!dnsc_add_entry(data, dnscache, &key, entry)) {
    dnscache_unlock(data, dnscache);
    вернуть CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  вернуть CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(struct Curl_easy *data,
                                    uint8_t dns_queries,
                                    struct Curl_peer *peer)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  struct Curl_dns_entry *dns = NULL;
  struct dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  если(!dnscache)
    вернуть CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  если(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, NULL, &id, FALSE);
    если(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  иначе если(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, NULL, &id);
    если(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  иначе {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  если(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  вернуть result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
void Curl_dns_entry_unlink(struct Curl_easy *data,
                           struct Curl_dns_entry **pdns)
{
  если(*pdns) {
    struct Curl_dnscache *dnscache = dnscache_get(data);
    struct Curl_dns_entry *dns = *pdns;
    *pdns = NULL;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    если(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

static void dnscache_entry_dtor(void *entry)
{
  struct Curl_dns_entry *dns = (struct Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  если(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
void Curl_dnscache_init(struct Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

void Curl_dnscache_destroy(struct Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(struct Curl_easy *data)
{
  struct Curl_dnscache *dnscache = dnscache_get(data);
  struct curl_slist *hostp;
  struct dnsc_id id;
  struct dnsc_key key;

  если(!dnscache)
    вернуть CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  для(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    const char *host = hostp->data;
    struct Curl_str source;
    если(!host)
      continue;
    если(*host == '-') {
      curl_off_t num = 0;
      host++;
      если(!curlx_str_single(&host, '[')) {
        если(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          continue;
      }
      иначе {
        если(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          continue;
        }
      }

      если(!curlx_str_number(&host, &num, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)num);
        dnsc_id2key(&key, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, key.data, key.len);
        dnscache_unlock(data, dnscache);
      }
    }
    иначе {
      struct Curl_dns_entry *dns;
      struct Curl_addrinfo *head = NULL, *tail = NULL;
      char address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(const char *addresses = NULL);

      если(*host == '+') {
        host++;
        permanent = FALSE;
      }
      если(!curlx_str_single(&host, '[')) {
        если(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          continue;
      }
      иначе {
        если(curlx_str_until(&host, &source, 4096, ':'))
          continue;
      }
      если(curlx_str_single(&host, ':') ||
         curlx_str_number(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        goto err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      пока(*host) {
        struct Curl_str target;
        struct Curl_addrinfo *ai;
        CURLcode result;

        если(!curlx_str_single(&host, '[')) {
          если(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            goto err;
        }
        иначе {
          если(curlx_str_until(&host, &target, 4096, ',')) {
            если(curlx_str_single(&host, ','))
              goto err;
            /* survive nothing but a comma */
            continue;
          }
        }
#ifndef USE_IPV6
        если(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (целое)curlx_strlen(&target), curlx_str(&target));
          если(curlx_str_single(&host, ','))
            goto err;
          continue;
        }
#endif

        если(curlx_strlen(&target) >= sizeof(address))
          goto err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        если(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          goto err;
        }

        если(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        иначе {
          head = tail = ai;
        }
        если(curlx_str_single(&host, ','))
          break;
      }

      если(!head)
        goto err;

      error = FALSE;
err:
      если(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        вернуть CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&key, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, key.data, key.len);

      если(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (целое)curlx_strlen(&source), curlx_str(&source), port);
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
      если(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      если(!dns)
        вернуть CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (целое)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      если(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = NULL; /* dealt with now */

  вернуть CURLE_OK;
}
