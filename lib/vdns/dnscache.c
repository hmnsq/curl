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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#અન્તર્ભાવ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "hash.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "vdns/dnscache.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strcopy.h"
#અન્તર્ભાવ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST શૂન્ય dnsc_peer2id(સંરચના dnsc_id *pid, અક્ષર type,
                           સંરચના Curl_peer *peer);
UNITTEST શૂન્ય dnsc_peer2id(સંરચના dnsc_id *pid, અક્ષર type,
                           સંરચના Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

સ્થિર શૂન્ય dnsc_str2id(સંરચના dnsc_id *pid, અક્ષર type,
                        સંરચના Curl_str *name, uint16_t port)
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
UNITTEST શૂન્ય dnsc_id2key(સંરચના dnsc_ચાવી *ચાવી, સંરચના dnsc_id *id);
UNITTEST શૂન્ય dnsc_id2key(સંરચના dnsc_ચાવી *ચાવી, સંરચના dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  જો(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ચાવી->data[0] = id->type;
  ચાવી->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  ચાવી->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((અક્ષર *)ચાવી->data + 3, curlx_str(&id->name), namelen);
  ચાવી->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(સંરચના dnsc_id *id,
                                    સંરચના Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(સંરચના dnsc_id *id,
                                    સંરચના Curl_dns_entry *dns)
{
  પાછા dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

સ્થિર શૂન્ય dnscache_entry_free(સંરચના Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

સંરચના dnscache_prune_data {
  અચલ સંરચના curltime *pnow;
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
સ્થિર પૂર્ણાંક dnscache_entry_is_stale(શૂન્ય *datap, શૂન્ય *hc)
{
  સંરચના dnscache_prune_data *prune = (સંરચના dnscache_prune_data *)datap;
  સંરચના Curl_dns_entry *dns = (સંરચના Curl_dns_entry *)hc;

  જો(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    પસંદગી(dns->type) {
    સ્થિતિ CURL_DNST_ADDR:
      જો(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      તોડો;
#ifdef USE_HTTPSRR
    સ્થિતિ CURL_DNST_HTTPS:
      જો(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      તોડો;
#endif
    મૂળભૂત:
      તોડો;
    }
    જો(age_ms >= prune->max_age_ms)
      પાછા TRUE;
    જો(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  પાછા FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
સ્થિર timediff_t dnscache_prune(સંરચના Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 અચલ સંરચના curltime *pnow)
{
  સંરચના dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (શૂન્ય *)&user,
                                 dnscache_entry_is_stale);

  પાછા user.oldest_ms;
}

સ્થિર સંરચના Curl_dnscache *dnscache_get(સંરચના Curl_easy *data)
{
  જો(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    પાછા &data->share->dnscache;
  જો(data->multi)
    પાછા &data->multi->dnscache;
  પાછા ખાલી;
}

સ્થિર શૂન્ય dnscache_lock(સંરચના Curl_easy *data,
                          સંરચના Curl_dnscache *dnscache)
{
  જો(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

સ્થિર શૂન્ય dnscache_unlock(સંરચના Curl_easy *data,
                            સંરચના Curl_dnscache *dnscache)
{
  જો(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
શૂન્ય Curl_dnscache_prune(સંરચના Curl_easy *data, અચલ સંરચના curltime *pnow)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  જો(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    પાછા;

  dnscache_lock(data, dnscache);

  કરો {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    જો(Curl_hash_ગણતરી(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    નહીંતર
      તોડો;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } જ્યારે(timeout_ms);

  dnscache_unlock(data, dnscache);
}

શૂન્ય Curl_dnscache_clear(સંરચના Curl_easy *data)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  જો(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
સ્થિર CURLcode fetch_entry(સંરચના Curl_easy *data,
                            સંરચના Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            સંરચના Curl_peer *peer,
                            સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_dns_entry *dns = ખાલી;
  સંરચના dnsc_id id;
  સંરચના dnsc_ચાવી ચાવી;
  અક્ષર type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = ખાલી;
  જો(!dnscache)
    પાછા CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ચાવી, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ચાવી.data, ચાવી.len);

  જો(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ખાલી;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  જો(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    સંરચના Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&ચાવી, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ચાવી.data, ચાવી.len);
  }

  જો(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    સંરચના dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    જો(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = ખાલી; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ચાવી.data, ચાવી.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  જો(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    જો((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ખાલી;
    }
    નહીંતર જો(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ખાલી;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    નહીંતર જો(dns && !dns->addr) { /* negative entry */
      dns = ખાલી;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  પાછા result;
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
CURLcode Curl_dnscache_get(સંરચના Curl_easy *data,
                           uint8_t dns_queries,
                           સંરચના Curl_peer *peer,
                           સંરચના Curl_dns_entry **pentry)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  સંરચના Curl_dns_entry *dns = ખાલી;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  જો(!result && dns)
    dns->refcount++; /* we pass out a reference */
  નહીંતર જો(result) {
    DEBUGASSERT(!dns);
    dns = ખાલી;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (પૂર્ણાંક)result, dns ? "" : "not ");
  *pentry = dns;
  પાછા result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
સ્થિર પૂર્ણાંક અંક_addresses(અચલ સંરચના Curl_addrinfo *addr)
{
  પૂર્ણાંક i = 0;
  જ્યારે(addr) {
    addr = addr->ai_next;
    i++;
  }
  પાછા i;
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
UNITTEST CURLcode dns_shuffle_addr(સંરચના Curl_easy *data,
                                   સંરચના Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(સંરચના Curl_easy *data,
                                   સંરચના Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  અચલ પૂર્ણાંક અંક_addrs = અંક_addresses(*addr);

  જો(અંક_addrs > 1) {
    સંરચના Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", અંક_addrs);

    nodes = curlx_malloc(અંક_addrs * કદ(*nodes));
    જો(nodes) {
      પૂર્ણાંક i;
      unsigned પૂર્ણાંક *rnd;
      અચલ size_t rnd_size = અંક_addrs * કદ(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      માટે(i = 1; i < અંક_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      જો(rnd) {
        /* Fisher-Yates shuffle */
        જો(Curl_rand(data, (unsigned અક્ષર *)rnd, rnd_size) == CURLE_OK) {
          સંરચના Curl_addrinfo *swap_tmp;
          માટે(i = અંક_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned પૂર્ણાંક)(i + 1)];
            nodes[rnd[i] % (unsigned પૂર્ણાંક)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          માટે(i = 1; i < અંક_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[અંક_addrs - 1]->ai_next = ખાલી;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      નહીંતર
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    નહીંતર
      result = CURLE_OUT_OF_MEMORY;
  }
  પાછા result;
}
#endif

સ્થિર bool dnscache_ai_has_family(સંરચના Curl_addrinfo *ai, પૂર્ણાંક ai_family)
{
  માટે(; ai; ai = ai->ai_next) {
    જો(ai->ai_family == ai_family)
      પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર સંરચના Curl_dns_entry *dnsc_entry_create(સંરચના dnsc_id *pid,
                                                bool permanent)
{
  સંરચના Curl_dns_entry *dns = ખાલી;

  જો(curlx_strlen(&pid->name) > UINT16_MAX)
    જાઓ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, કદ(સંરચના Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  જો(!dns)
    જાઓ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  જો(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  પાછા dns;
}

સ્થિર સંરચના Curl_dns_entry *dnsc_entry_assign_addr(
  સંરચના Curl_easy *data,
  સંરચના Curl_dns_entry *dns,
  uint8_t dns_queries,
  સંરચના Curl_addrinfo **paddr1,
  સંરચના Curl_addrinfo **paddr2)
{
  જો(!dns)
    જાઓ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  જો(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ખાલી;
  }
  જો(paddr2 && *paddr2) {
    સંરચના Curl_addrinfo **phead = &dns->addr;
    જ્યારે(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = ખાલી;
  }

  જો((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  જો((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  જો(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    જો(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = ખાલી;
      જાઓ out;
    }
  }
#નહીંતર
  (શૂન્ય)data;
#endif

out:
  જો(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ખાલી;
  }
  જો(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ખાલી;
  }
  પાછા dns;
}

સંરચના Curl_dns_entry *Curl_dnsc_mk_addr(સંરચના Curl_easy *data,
                                         uint8_t dns_queries,
                                         સંરચના Curl_addrinfo **paddr,
                                         સંરચના Curl_peer *peer)
{
  સંરચના dnsc_id id;
  સંરચના Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ખાલી);
  પાછા dns;
}

સંરચના Curl_dns_entry *Curl_dnsc_mk_addr2(સંરચના Curl_easy *data,
                                          uint8_t dns_queries,
                                          સંરચના Curl_addrinfo **paddr1,
                                          સંરચના Curl_addrinfo **paddr2,
                                          સંરચના Curl_peer *peer)
{
  સંરચના dnsc_id id;
  સંરચના Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  પાછા dns;
}

સ્થિર સંરચના Curl_dns_entry *dnsc_add_entry(સંરચના Curl_easy *data,
                                             સંરચના Curl_dnscache *dnscache,
                                             સંરચના dnsc_ચાવી *ચાવી,
                                             સંરચના Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  પાછા Curl_hash_add(&dnscache->entries, ચાવી->data, ચાવી->len, entry);
}

#ifdef USE_HTTPSRR
સ્થિર સંરચના Curl_dns_entry *dnsc_entry_assign_https(
  સંરચના Curl_dns_entry *dns,
  સંરચના Curl_https_rrinfo **phinfo)
{
  જો(!dns)
    જાઓ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  જો(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ખાલી;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  જો(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ખાલી;
  }
out:
  જો(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ખાલી;
  }
  પાછા dns;
}

સંરચના Curl_dns_entry *Curl_dnsc_mk_https(સંરચના Curl_easy *data,
                                          સંરચના Curl_https_rrinfo **phinfo,
                                          સંરચના Curl_peer *peer)
{
  સંરચના dnsc_id id;
  સંરચના Curl_dns_entry *dns;

  (શૂન્ય)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  પાછા dns;
}

સ્થિર સંરચના Curl_dns_entry *dnsc_add_https(સંરચના Curl_easy *data,
                                             સંરચના Curl_dnscache *dnscache,
                                             સંરચના Curl_https_rrinfo **phinfo,
                                             સંરચના dnsc_id *id)
{
  સંરચના Curl_dns_entry *dns, *dns2;
  સંરચના dnsc_ચાવી ચાવી;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  જો(!dns)
    પાછા ખાલી;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ચાવી, id);
  dns2 = dnsc_add_entry(data, dnscache, &ચાવી, dns);
  જો(!dns2) {
    dnscache_entry_free(dns);
    પાછા ખાલી;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  પાછા dns;
}
#endif /* USE_HTTPSRR */

સ્થિર સંરચના Curl_dns_entry *dnsc_add_addr(સંરચના Curl_easy *data,
                                            સંરચના Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            સંરચના Curl_addrinfo **paddr,
                                            સંરચના dnsc_id *id,
                                            સંરચના dnsc_ચાવી *ચાવી,
                                            bool permanent)
{
  સંરચના Curl_dns_entry *dns;
  સંરચના Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ખાલી);
  જો(!dns)
    પાછા ખાલી;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, ચાવી, dns);
  જો(!dns2) {
    dnscache_entry_free(dns);
    પાછા ખાલી;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  પાછા dns;
}

સ્થિર સંરચના Curl_dns_entry *dnsc_add_peer_addr(
  સંરચના Curl_easy *data,
  સંરચના Curl_dnscache *dnscache,
  uint8_t dns_queries,
  સંરચના Curl_addrinfo **paddr,
  સંરચના dnsc_id *id,
  bool permanent)
{
  સંરચના Curl_dns_entry *dns;
  સંરચના Curl_dns_entry *dns2;
  સંરચના dnsc_ચાવી ચાવી;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ખાલી);
  જો(!dns)
    પાછા ખાલી;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ચાવી, id);
  dns2 = dnsc_add_entry(data, dnscache, &ચાવી, dns);
  જો(!dns2) {
    dnscache_entry_free(dns);
    પાછા ખાલી;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  પાછા dns;
}

CURLcode Curl_dnscache_add(સંરચના Curl_easy *data,
                           સંરચના Curl_dns_entry *entry)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  સંરચના Curl_str name;
  સંરચના dnsc_id id;
  સંરચના dnsc_ચાવી ચાવી;

  જો(!dnscache)
    પાછા CURLE_FAILED_INIT;
  જો(!entry || (entry->type == CURL_DNST_INIT))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&ચાવી, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  જો(!dnsc_add_entry(data, dnscache, &ચાવી, entry)) {
    dnscache_unlock(data, dnscache);
    પાછા CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  પાછા CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(સંરચના Curl_easy *data,
                                    uint8_t dns_queries,
                                    સંરચના Curl_peer *peer)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  સંરચના Curl_dns_entry *dns = ખાલી;
  સંરચના dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  જો(!dnscache)
    પાછા CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  જો(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, ખાલી, &id, FALSE);
    જો(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  નહીંતર જો(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, ખાલી, &id);
    જો(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  નહીંતર {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  જો(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  પાછા result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
શૂન્ય Curl_dns_entry_unlink(સંરચના Curl_easy *data,
                           સંરચના Curl_dns_entry **pdns)
{
  જો(*pdns) {
    સંરચના Curl_dnscache *dnscache = dnscache_get(data);
    સંરચના Curl_dns_entry *dns = *pdns;
    *pdns = ખાલી;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    જો(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

સ્થિર શૂન્ય dnscache_entry_dtor(શૂન્ય *entry)
{
  સંરચના Curl_dns_entry *dns = (સંરચના Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  જો(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
શૂન્ય Curl_dnscache_init(સંરચના Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

શૂન્ય Curl_dnscache_destroy(સંરચના Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(સંરચના Curl_easy *data)
{
  સંરચના Curl_dnscache *dnscache = dnscache_get(data);
  સંરચના curl_slist *hostp;
  સંરચના dnsc_id id;
  સંરચના dnsc_ચાવી ચાવી;

  જો(!dnscache)
    પાછા CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  માટે(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    અચલ અક્ષર *host = hostp->data;
    સંરચના Curl_str source;
    જો(!host)
      ચાલુ;
    જો(*host == '-') {
      curl_off_t અંક = 0;
      host++;
      જો(!curlx_str_single(&host, '[')) {
        જો(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ચાલુ;
      }
      નહીંતર {
        જો(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ચાલુ;
        }
      }

      જો(!curlx_str_સંખ્યા(&host, &અંક, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)અંક);
        dnsc_id2key(&ચાવી, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ચાવી.data, ચાવી.len);
        dnscache_unlock(data, dnscache);
      }
    }
    નહીંતર {
      સંરચના Curl_dns_entry *dns;
      સંરચના Curl_addrinfo *head = ખાલી, *tail = ખાલી;
      અક્ષર address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(અચલ અક્ષર *addresses = ખાલી);

      જો(*host == '+') {
        host++;
        permanent = FALSE;
      }
      જો(!curlx_str_single(&host, '[')) {
        જો(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ચાલુ;
      }
      નહીંતર {
        જો(curlx_str_until(&host, &source, 4096, ':'))
          ચાલુ;
      }
      જો(curlx_str_single(&host, ':') ||
         curlx_str_સંખ્યા(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        જાઓ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      જ્યારે(*host) {
        સંરચના Curl_str target;
        સંરચના Curl_addrinfo *ai;
        CURLcode result;

        જો(!curlx_str_single(&host, '[')) {
          જો(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            જાઓ err;
        }
        નહીંતર {
          જો(curlx_str_until(&host, &target, 4096, ',')) {
            જો(curlx_str_single(&host, ','))
              જાઓ err;
            /* survive nothing but a comma */
            ચાલુ;
          }
        }
#ifndef USE_IPV6
        જો(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (પૂર્ણાંક)curlx_strlen(&target), curlx_str(&target));
          જો(curlx_str_single(&host, ','))
            જાઓ err;
          ચાલુ;
        }
#endif

        જો(curlx_strlen(&target) >= કદ(address))
          જાઓ err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        જો(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          જાઓ err;
        }

        જો(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        નહીંતર {
          head = tail = ai;
        }
        જો(curlx_str_single(&host, ','))
          તોડો;
      }

      જો(!head)
        જાઓ err;

      error = FALSE;
err:
      જો(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        પાછા CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&ચાવી, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ચાવી.data, ચાવી.len);

      જો(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (પૂર્ણાંક)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, ચાવી.data, ચાવી.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &ચાવી, permanent);
      જો(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      જો(!dns)
        પાછા CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (પૂર્ણાંક)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      જો(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = ખાલી; /* dealt with now */

  પાછા CURLE_OK;
}
