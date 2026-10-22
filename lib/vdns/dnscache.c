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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vdns/dnscache.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#define MAX_DNS_CACHE_SIZE 29999

/* @unittest 3217 */
UNITTEST ಶೂನ್ಯ dnsc_peer2id(ಸಂರಚನಾ dnsc_id *pid, ವರ್ಣ type,
                           ಸಂರಚನಾ Curl_peer *peer);
UNITTEST ಶೂನ್ಯ dnsc_peer2id(ಸಂರಚನಾ dnsc_id *pid, ವರ್ಣ type,
                           ಸಂರಚನಾ Curl_peer *peer)
{
  curlx_str_assign(&pid->name, peer->hostname, strlen(peer->hostname));
  pid->port = peer->port;
  pid->type = type;
}

ಸ್ಥಿರ ಶೂನ್ಯ dnsc_str2id(ಸಂರಚನಾ dnsc_id *pid, ವರ್ಣ type,
                        ಸಂರಚನಾ Curl_str *name, uint16_t port)
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
UNITTEST ಶೂನ್ಯ dnsc_id2key(ಸಂರಚನಾ dnsc_ಕೀಲಿ *ಕೀಲಿ, ಸಂರಚನಾ dnsc_id *id);
UNITTEST ಶೂನ್ಯ dnsc_id2key(ಸಂರಚನಾ dnsc_ಕೀಲಿ *ಕೀಲಿ, ಸಂರಚನಾ dnsc_id *id)
{
  size_t namelen = curlx_strlen(&id->name);
  ಯದಿ(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  ಕೀಲಿ->data[0] = id->type;
  ಕೀಲಿ->data[1] = (uint8_t)((id->port >> 8) & 0xff);
  ಕೀಲಿ->data[2] = (uint8_t)(id->port & 0xff);
  Curl_strntolower((ವರ್ಣ *)ಕೀಲಿ->data + 3, curlx_str(&id->name), namelen);
  ಕೀಲಿ->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_entry(ಸಂರಚನಾ dnsc_id *id,
                                    ಸಂರಚನಾ Curl_dns_entry *dns);
UNITTEST bool dnsc_id_matches_entry(ಸಂರಚನಾ dnsc_id *id,
                                    ಸಂರಚನಾ Curl_dns_entry *dns)
{
  ಪ್ರತ್ಯಾಗಮ dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->port == dns->port) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!strcmp(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_entry_free(ಸಂರಚನಾ Curl_dns_entry *dns)
{
  Curl_freeaddrinfo(dns->addr);
#ifdef USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#endif
  curlx_free(dns);
}

ಸಂರಚನಾ dnscache_prune_data {
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dnscache_entry_is_stale(ಶೂನ್ಯ *datap, ಶೂನ್ಯ *hc)
{
  ಸಂರಚನಾ dnscache_prune_data *prune = (ಸಂರಚನಾ dnscache_prune_data *)datap;
  ಸಂರಚನಾ Curl_dns_entry *dns = (ಸಂರಚನಾ Curl_dns_entry *)hc;

  ಯದಿ(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    ವಿಕಲ್ಪ(dns->type) {
    ಪಕ್ಷ CURL_DNST_ADDR:
      ಯದಿ(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      ಭಙ್ಗ;
#ifdef USE_HTTPSRR
    ಪಕ್ಷ CURL_DNST_HTTPS:
      ಯದಿ(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
    ಯದಿ(age_ms >= prune->max_age_ms)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
ಸ್ಥಿರ timediff_t dnscache_prune(ಸಂರಚನಾ Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ dnscache_prune_data user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (ಶೂನ್ಯ *)&user,
                                 dnscache_entry_is_stale);

  ಪ್ರತ್ಯಾಗಮ user.oldest_ms;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dnscache *dnscache_get(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->share && data->share->specifier & (1 << CURL_LOCK_DATA_DNS))
    ಪ್ರತ್ಯಾಗಮ &data->share->dnscache;
  ಯದಿ(data->multi)
    ಪ್ರತ್ಯಾಗಮ &data->multi->dnscache;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_lock(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_dnscache *dnscache)
{
  ಯದಿ(data->share && dnscache == &data->share->dnscache)
    Curl_share_lock(data, CURL_LOCK_DATA_DNS, CURL_LOCK_ACCESS_SINGLE);
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_unlock(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_dnscache *dnscache)
{
  ಯದಿ(data->share && dnscache == &data->share->dnscache)
    Curl_share_unlock(data, CURL_LOCK_DATA_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
ಶೂನ್ಯ Curl_dnscache_prune(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = data->set.dns_cache_timeout_ms;

  ಯದಿ(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    ಪ್ರತ್ಯಾಗಮ;

  dnscache_lock(data, dnscache);

  ಕುರು {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    ಯದಿ(Curl_hash_ಎಣಿಕೆ(&dnscache->entries) > MAX_DNS_CACHE_SIZE)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;

    /* if the cache size is still too big, use the oldest age as new prune
       limit */
  } ಯಾವತ್(timeout_ms);

  dnscache_unlock(data, dnscache);
}

ಶೂನ್ಯ Curl_dnscache_clear(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  ಯದಿ(dnscache) {
    dnscache_lock(data, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(data, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
ಸ್ಥಿರ CURLcode fetch_entry(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            ಸಂರಚನಾ Curl_peer *peer,
                            ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ dnsc_ಕೀಲಿ ಕೀಲಿ;
  ವರ್ಣ type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode result = CURLE_OK;

  *pdns = ರಿಕ್ತ;
  ಯದಿ(!dnscache)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&ಕೀಲಿ, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);

  ಯದಿ(dns && !dnsc_id_matches_entry(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = ರಿಕ್ತ;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  ಯದಿ(!dns && (type == CURL_DNST_ADDR) && data->state.wildcard_resolve) {
    ಸಂರಚನಾ Curl_str wildname;

    curlx_str_assign(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->port);
    dnsc_id2key(&ಕೀಲಿ, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);
  }

  ಯದಿ(dns && (data->set.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    ಸಂರಚನಾ dnscache_prune_data user;

    user.pnow = Curl_pgrs_now(data);
    user.max_age_ms = data->set.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    ಯದಿ(dnscache_entry_is_stale(&user, dns)) {
      infof(data, "Hostname in DNS cache was stale, zapped");
      dns = ರಿಕ್ತ; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  ಯದಿ(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    ಯದಿ((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(data, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = ರಿಕ್ತ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    ಅನ್ಯಥಾ ಯದಿ(dns && !dns->addr) { /* negative entry */
      dns = ರಿಕ್ತ;
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  ಪ್ರತ್ಯಾಗಮ result;
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
CURLcode Curl_dnscache_get(ಸಂರಚನಾ Curl_easy *data,
                           uint8_t dns_queries,
                           ಸಂರಚನಾ Curl_peer *peer,
                           ಸಂರಚನಾ Curl_dns_entry **pentry)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  dnscache_lock(data, dnscache);
  result = fetch_entry(data, dnscache, dns_queries, peer, &dns);
  ಯದಿ(!result && dns)
    dns->refcount++; /* we pass out a reference */
  ಅನ್ಯಥಾ ಯದಿ(result) {
    DEBUGASSERT(!dns);
    dns = ರಿಕ್ತ;
  }
  dnscache_unlock(data, dnscache);

  CURL_TRC_DNS(data, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->port, Curl_resolv_query_str(dns_queries),
               (ಪೂರ್ಣಾಙ್ಕ)result, dns ? "" : "not ");
  *pentry = dns;
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_addresses(ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *addr)
{
  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಯಾವತ್(addr) {
    addr = addr->ai_next;
    i++;
  }
  ಪ್ರತ್ಯಾಗಮ i;
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
UNITTEST CURLcode dns_shuffle_addr(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_addrinfo **addr)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_addrs = ಅಂಕೆ_addresses(*addr);

  ಯದಿ(ಅಂಕೆ_addrs > 1) {
    ಸಂರಚನಾ Curl_addrinfo **nodes;
    CURL_TRC_DNS(data, "Shuffling %d addresses", ಅಂಕೆ_addrs);

    nodes = curlx_malloc(ಅಂಕೆ_addrs * ಪರಿಮಾಣ(*nodes));
    ಯದಿ(nodes) {
      ಪೂರ್ಣಾಙ್ಕ i;
      unsigned ಪೂರ್ಣಾಙ್ಕ *rnd;
      ಧ್ರುವ size_t rnd_size = ಅಂಕೆ_addrs * ಪರಿಮಾಣ(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      ಪ್ರತಿ(i = 1; i < ಅಂಕೆ_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_next;
      }

      rnd = curlx_malloc(rnd_size);
      ಯದಿ(rnd) {
        /* Fisher-Yates shuffle */
        ಯದಿ(Curl_rand(data, (unsigned ವರ್ಣ *)rnd, rnd_size) == CURLE_OK) {
          ಸಂರಚನಾ Curl_addrinfo *swap_tmp;
          ಪ್ರತಿ(i = ಅಂಕೆ_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (unsigned ಪೂರ್ಣಾಙ್ಕ)(i + 1)];
            nodes[rnd[i] % (unsigned ಪೂರ್ಣಾಙ್ಕ)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          ಪ್ರತಿ(i = 1; i < ಅಂಕೆ_addrs; i++) {
            nodes[i - 1]->ai_next = nodes[i];
          }

          nodes[ಅಂಕೆ_addrs - 1]->ai_next = ರಿಕ್ತ;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      ಅನ್ಯಥಾ
        result = CURLE_OUT_OF_MEMORY;
      curlx_free(nodes);
    }
    ಅನ್ಯಥಾ
      result = CURLE_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

ಸ್ಥಿರ bool dnscache_ai_has_family(ಸಂರಚನಾ Curl_addrinfo *ai, ಪೂರ್ಣಾಙ್ಕ ai_family)
{
  ಪ್ರತಿ(; ai; ai = ai->ai_next) {
    ಯದಿ(ai->ai_family == ai_family)
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_entry_create(ಸಂರಚನಾ dnsc_id *pid,
                                                bool permanent)
{
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;

  ಯದಿ(curlx_strlen(&pid->name) > UINT16_MAX)
    ಗಚ್ಛ out;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_dns_entry) +
                     curlx_strlen(&pid->name));
  ಯದಿ(!dns)
    ಗಚ್ಛ out;

  dns->refcount = 1; /* the cache has the first reference */
  dns->port = pid->port;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  ಯದಿ(dns->hostlen)
    memcpy(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

out:
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_entry_assign_addr(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ Curl_dns_entry *dns,
  uint8_t dns_queries,
  ಸಂರಚನಾ Curl_addrinfo **paddr1,
  ಸಂರಚನಾ Curl_addrinfo **paddr2)
{
  ಯದಿ(!dns)
    ಗಚ್ಛ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  ಯದಿ(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = ರಿಕ್ತ;
  }
  ಯದಿ(paddr2 && *paddr2) {
    ಸಂರಚನಾ Curl_addrinfo **phead = &dns->addr;
    ಯಾವತ್(*phead)
      phead = &(*phead)->ai_next;
    *phead = *paddr2;
    *paddr2 = ರಿಕ್ತ;
  }

  ಯದಿ((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#ifdef USE_IPV6
  ಯದಿ((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#endif /* USE_IPV6 */

#ifndef CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  ಯದಿ(data->set.dns_shuffle_addresses && dns->addr) {
    CURLcode result = dns_shuffle_addr(data, &dns->addr);
    ಯದಿ(result) {
      /* free without lock, we are the sole owner */
      dnscache_entry_free(dns);
      dns = ರಿಕ್ತ;
      ಗಚ್ಛ out;
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif

out:
  ಯದಿ(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = ರಿಕ್ತ;
  }
  ಯದಿ(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸಂರಚನಾ Curl_dns_entry *Curl_dnsc_mk_addr(ಸಂರಚನಾ Curl_easy *data,
                                         uint8_t dns_queries,
                                         ಸಂರಚನಾ Curl_addrinfo **paddr,
                                         ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸಂರಚನಾ Curl_dns_entry *Curl_dnsc_mk_addr2(ಸಂರಚನಾ Curl_easy *data,
                                          uint8_t dns_queries,
                                          ಸಂರಚನಾ Curl_addrinfo **paddr1,
                                          ಸಂರಚನಾ Curl_addrinfo **paddr2,
                                          ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ Curl_dns_entry *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr1, paddr2);
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_add_entry(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_dnscache *dnscache,
                                             ಸಂರಚನಾ dnsc_ಕೀಲಿ *ಕೀಲಿ,
                                             ಸಂರಚನಾ Curl_dns_entry *entry)
{
  entry->added = *Curl_pgrs_now(data);
  ಪ್ರತ್ಯಾಗಮ Curl_hash_add(&dnscache->entries, ಕೀಲಿ->data, ಕೀಲಿ->len, entry);
}

#ifdef USE_HTTPSRR
ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_entry_assign_https(
  ಸಂರಚನಾ Curl_dns_entry *dns,
  ಸಂರಚನಾ Curl_https_rrinfo **phinfo)
{
  ಯದಿ(!dns)
    ಗಚ್ಛ out;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  ಯದಿ(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = ರಿಕ್ತ;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  ಯದಿ(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = ರಿಕ್ತ;
  }
out:
  ಯದಿ(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸಂರಚನಾ Curl_dns_entry *Curl_dnsc_mk_https(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_https_rrinfo **phinfo,
                                          ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ Curl_dns_entry *dns;

  (ಶೂನ್ಯ)data;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_entry_create(&id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_add_https(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_dnscache *dnscache,
                                             ಸಂರಚನಾ Curl_https_rrinfo **phinfo,
                                             ಸಂರಚನಾ dnsc_id *id)
{
  ಸಂರಚನಾ Curl_dns_entry *dns, *dns2;
  ಸಂರಚನಾ dnsc_ಕೀಲಿ ಕೀಲಿ;

  dns = dnsc_entry_create(id, FALSE);
  dns = dnsc_entry_assign_https(dns, phinfo);
  ಯದಿ(!dns)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ಕೀಲಿ, id);
  dns2 = dnsc_add_entry(data, dnscache, &ಕೀಲಿ, dns);
  ಯದಿ(!dns2) {
    dnscache_entry_free(dns);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಪ್ರತ್ಯಾಗಮ dns;
}
#endif /* USE_HTTPSRR */

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_add_addr(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            ಸಂರಚನಾ Curl_addrinfo **paddr,
                                            ಸಂರಚನಾ dnsc_id *id,
                                            ಸಂರಚನಾ dnsc_ಕೀಲಿ *ಕೀಲಿ,
                                            bool permanent)
{
  ಸಂರಚನಾ Curl_dns_entry *dns;
  ಸಂರಚನಾ Curl_dns_entry *dns2;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ರಿಕ್ತ);
  ಯದಿ(!dns)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_add_entry(data, dnscache, ಕೀಲಿ, dns);
  ಯದಿ(!dns2) {
    dnscache_entry_free(dns);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಪ್ರತ್ಯಾಗಮ dns;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_dns_entry *dnsc_add_peer_addr(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ Curl_dnscache *dnscache,
  uint8_t dns_queries,
  ಸಂರಚನಾ Curl_addrinfo **paddr,
  ಸಂರಚನಾ dnsc_id *id,
  bool permanent)
{
  ಸಂರಚನಾ Curl_dns_entry *dns;
  ಸಂರಚನಾ Curl_dns_entry *dns2;
  ಸಂರಚನಾ dnsc_ಕೀಲಿ ಕೀಲಿ;

  dns = dnsc_entry_create(id, permanent);
  dns = dnsc_entry_assign_addr(data, dns, dns_queries, paddr, ರಿಕ್ತ);
  ಯದಿ(!dns)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&ಕೀಲಿ, id);
  dns2 = dnsc_add_entry(data, dnscache, &ಕೀಲಿ, dns);
  ಯದಿ(!dns2) {
    dnscache_entry_free(dns);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  ಪ್ರತ್ಯಾಗಮ dns;
}

CURLcode Curl_dnscache_add(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_dns_entry *entry)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  ಸಂರಚನಾ Curl_str name;
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ dnsc_ಕೀಲಿ ಕೀಲಿ;

  ಯದಿ(!dnscache)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!entry || (entry->type == CURL_DNST_INIT))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_str_assign(&name, entry->hostname, entry->hostlen);
  dnsc_str2id(&id, entry->type, &name, entry->port);
  dnsc_id2key(&ಕೀಲಿ, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_lock(data, dnscache);
  ಯದಿ(!dnsc_add_entry(data, dnscache, &ಕೀಲಿ, entry)) {
    dnscache_unlock(data, dnscache);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  entry->refcount++;
  dnscache_unlock(data, dnscache);
  CURL_TRC_DNS(data, "cached entry for %s:%u queries=%s",
               entry->hostname, entry->port,
               Curl_resolv_query_str(entry->dns_queries));
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_dnscache_add_negative(ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t dns_queries,
                                    ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  ಸಂರಚನಾ dnsc_id id;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dnscache);
  ಯದಿ(!dnscache)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  dnscache_lock(data, dnscache);

  ಯದಿ(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_add_peer_addr(data, dnscache, dns_queries, ರಿಕ್ತ, &id, FALSE);
    ಯದಿ(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#ifdef USE_HTTPSRR
  ಅನ್ಯಥಾ ಯದಿ(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_add_https(data, dnscache, ರಿಕ್ತ, &id);
    ಯದಿ(!dns)
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ಅನ್ಯಥಾ {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  ಯದಿ(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(data, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->port,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(data, dnscache);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
ಶೂನ್ಯ Curl_dns_entry_unlink(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಯದಿ(*pdns) {
    ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
    ಸಂರಚನಾ Curl_dns_entry *dns = *pdns;
    *pdns = ರಿಕ್ತ;
    dnscache_lock(data, dnscache);
    dns->refcount--;
    ಯದಿ(dns->refcount == 0)
      dnscache_entry_free(dns);
    dnscache_unlock(data, dnscache);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ dnscache_entry_dtor(ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ Curl_dns_entry *dns = (ಸಂರಚನಾ Curl_dns_entry *)entry;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  ಯದಿ(dns->refcount == 0)
    dnscache_entry_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
ಶೂನ್ಯ Curl_dnscache_init(ಸಂರಚನಾ Curl_dnscache *dns, size_t size)
{
  Curl_hash_init(&dns->entries, size, CURL_HASH_TYPE_BYTES,
                 dnscache_entry_dtor);
}

ಶೂನ್ಯ Curl_dnscache_destroy(ಸಂರಚನಾ Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_dnscache *dnscache = dnscache_get(data);
  ಸಂರಚನಾ curl_slist *hostp;
  ಸಂರಚನಾ dnsc_id id;
  ಸಂರಚನಾ dnsc_ಕೀಲಿ ಕೀಲಿ;

  ಯದಿ(!dnscache)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  data->state.wildcard_resolve = FALSE;

  ಪ್ರತಿ(hostp = data->state.resolve; hostp; hostp = hostp->next) {
    ಧ್ರುವ ವರ್ಣ *host = hostp->data;
    ಸಂರಚನಾ Curl_str source;
    ಯದಿ(!host)
      ಅನುವರ್ತ;
    ಯದಿ(*host == '-') {
      curl_off_t ಅಂಕೆ = 0;
      host++;
      ಯದಿ(!curlx_str_single(&host, '[')) {
        ಯದಿ(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          ಅನುವರ್ತ;
        }
      }

      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&host, &ಅಂಕೆ, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)ಅಂಕೆ);
        dnsc_id2key(&ಕೀಲಿ, &id);
        dnscache_lock(data, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);
        dnscache_unlock(data, dnscache);
      }
    }
    ಅನ್ಯಥಾ {
      ಸಂರಚನಾ Curl_dns_entry *dns;
      ಸಂರಚನಾ Curl_addrinfo *head = ರಿಕ್ತ, *tail = ರಿಕ್ತ;
      ವರ್ಣ address[64];
      curl_off_t tmpofft = 0;
      uint16_t port = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ಧ್ರುವ ವರ್ಣ *addresses = ರಿಕ್ತ);

      ಯದಿ(*host == '+') {
        host++;
        permanent = FALSE;
      }
      ಯದಿ(!curlx_str_single(&host, '[')) {
        ಯದಿ(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(curlx_str_until(&host, &source, 4096, ':'))
          ಅನುವರ್ತ;
      }
      ಯದಿ(curlx_str_single(&host, ':') ||
         curlx_str_ಸಂಖ್ಯೆ(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        ಗಚ್ಛ err;
      port = (uint16_t)tmpofft;

      VERBOSE(addresses = host);

      /* start the address section */
      ಯಾವತ್(*host) {
        ಸಂರಚನಾ Curl_str target;
        ಸಂರಚನಾ Curl_addrinfo *ai;
        CURLcode result;

        ಯದಿ(!curlx_str_single(&host, '[')) {
          ಯದಿ(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            ಗಚ್ಛ err;
        }
        ಅನ್ಯಥಾ {
          ಯದಿ(curlx_str_until(&host, &target, 4096, ',')) {
            ಯದಿ(curlx_str_single(&host, ','))
              ಗಚ್ಛ err;
            /* survive nothing but a comma */
            ಅನುವರ್ತ;
          }
        }
#ifndef USE_IPV6
        ಯದಿ(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(data, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(&target), curlx_str(&target));
          ಯದಿ(curlx_str_single(&host, ','))
            ಗಚ್ಛ err;
          ಅನುವರ್ತ;
        }
#endif

        ಯದಿ(curlx_strlen(&target) >= ಪರಿಮಾಣ(address))
          ಗಚ್ಛ err;

        memcpy(address, curlx_str(&target), curlx_strlen(&target));
        address[curlx_strlen(&target)] = '\0';

        result = Curl_str2addr(address, port, &ai);
        ಯದಿ(result) {
          infof(data, "Resolve IP address '%s' found is illegal", address);
          ಗಚ್ಛ err;
        }

        ಯದಿ(tail) {
          tail->ai_next = ai;
          tail = tail->ai_next;
        }
        ಅನ್ಯಥಾ {
          head = tail = ai;
        }
        ಯದಿ(curlx_str_single(&host, ','))
          ಭಙ್ಗ;
      }

      ಯದಿ(!head)
        ಗಚ್ಛ err;

      error = FALSE;
err:
      ಯದಿ(error) {
        failf(data, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->data);
        Curl_freeaddrinfo(head);
        ಪ್ರತ್ಯಾಗಮ CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, port);
      dnsc_id2key(&ಕೀಲಿ, &id);
      dnscache_lock(data, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);

      ಯದಿ(dns) {
        infof(data, "RESOLVE %.*s:%u - old addresses discarded",
              (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(&source), curlx_str(&source), port);
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

        Curl_hash_delete(&dnscache->entries, ಕೀಲಿ.data, ಕೀಲಿ.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_add_addr(data, dnscache, CURL_DNSQ_ADDR, &head,
                          &id, &ಕೀಲಿ, permanent);
      ಯದಿ(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(data, dnscache);

      ಯದಿ(!dns)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      infof(data, "[DNS] added %.*s:%u:%s to cache%s",
            (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(&id.name), curlx_str(&id.name), id.port,
            addresses, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      ಯದಿ(curlx_str_casecompare(&source, "*")) {
        infof(data, "RESOLVE *:%u using wildcard", port);
        data->state.wildcard_resolve = TRUE;
      }
    }
  }
  data->state.resolve = ರಿಕ್ತ; /* dealt with now */

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
