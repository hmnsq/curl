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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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

#परिभाषय MAX_DNS_CACHE_आकार 29999

/* @unittest 3217 */
UNITTEST शून्य dnsc_peer2id(संरचना dnsc_id *pid, वर्ण type,
                           संरचना Curl_peer *peer);
UNITTEST शून्य dnsc_peer2id(संरचना dnsc_id *pid, वर्ण type,
                           संरचना Curl_peer *peer)
{
  curlx_str_नियोजय(&pid->name, peer->hostname, वाक्यदैर्घ्यम्(peer->hostname));
  pid->पत्तनम् = peer->पत्तनम्;
  pid->type = type;
}

स्थिर शून्य dnsc_str2id(संरचना dnsc_id *pid, वर्ण type,
                        संरचना Curl_str *name, uint16_t पत्तनम्)
{
  pid->name = *name;
  pid->पत्तनम् = पत्तनम्;
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
  यदि(namelen > (MAX_HOSTCACHE_LEN - 3))
    namelen = MAX_HOSTCACHE_LEN - 3;
  /* store and lower case the name */
  कुञ्चिका->दत्त[0] = id->type;
  कुञ्चिका->दत्त[1] = (uint8_t)((id->पत्तनम् >> 8) & 0xff);
  कुञ्चिका->दत्त[2] = (uint8_t)(id->पत्तनम् & 0xff);
  Curl_strntolower((वर्ण *)कुञ्चिका->दत्त + 3, curlx_str(&id->name), namelen);
  कुञ्चिका->len = namelen + 3;
}

/* @unittest 3271 */
UNITTEST bool dnsc_id_matches_प्रविष्टिः(संरचना dnsc_id *id,
                                    संरचना Curl_dns_प्रविष्टिः *dns);
UNITTEST bool dnsc_id_matches_प्रविष्टिः(संरचना dnsc_id *id,
                                    संरचना Curl_dns_प्रविष्टिः *dns)
{
  प्रत्यागम dns &&
         /* Neither the requested nor the cached key was truncated. */
         (((dns->hostlen <= (MAX_HOSTCACHE_LEN - 3)) &&
           (id->name.len <= (MAX_HOSTCACHE_LEN - 3))) ||
          /* port and host string are the same, case insensitive */
          ((id->पत्तनम् == dns->पत्तनम्) && (id->name.len == dns->hostlen) &&
           /* for most lookups, case is the same, try that first */
           (!वाक्यं_तुलय(id->name.str, dns->hostname) ||
            curl_strequal(id->name.str, dns->hostname))));
}

स्थिर शून्य dnscache_प्रविष्टिः_free(संरचना Curl_dns_प्रविष्टिः *dns)
{
  Curl_freeaddrinfo(dns->addr);
#यदि_परिभाषितम् USE_HTTPSRR
  Curl_httpsrr_destroy(dns->hinfo);
#यदि_अन्तः
  curlx_free(dns);
}

संरचना dnscache_prune_दत्त {
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
स्थिर पूर्णाङ्क dnscache_प्रविष्टिः_is_stale(शून्य *datap, शून्य *hc)
{
  संरचना dnscache_prune_दत्त *prune = (संरचना dnscache_prune_दत्त *)datap;
  संरचना Curl_dns_प्रविष्टिः *dns = (संरचना Curl_dns_प्रविष्टिः *)hc;

  यदि(!dns->permanent) {
    /* get age in milliseconds */
    timediff_t age_ms = curlx_ptimediff_ms(prune->pnow, &dns->added);
    विकल्प(dns->type) {
    पक्ष CURL_DNST_ADDR:
      यदि(!dns->addr)
        age_ms *= 2; /* negative entries age twice as fast */
      भङ्ग;
#यदि_परिभाषितम् USE_HTTPSRR
    पक्ष CURL_DNST_HTTPS:
      यदि(!dns->hinfo)
        age_ms *= 2; /* negative entries age twice as fast */
      भङ्ग;
#यदि_अन्तः
    स्वभाव:
      भङ्ग;
    }
    यदि(age_ms >= prune->max_age_ms)
      प्रत्यागम TRUE;
    यदि(age_ms > prune->oldest_ms)
      prune->oldest_ms = age_ms;
  }
  प्रत्यागम FALSE;
}

/*
 * Prune the DNS cache. This assumes that a lock has already been taken.
 * Returns the 'age' of the oldest still kept entry - in milliseconds.
 */
स्थिर timediff_t dnscache_prune(संरचना Curl_hash *hostcache,
                                 timediff_t cache_timeout_ms,
                                 ध्रुव संरचना curltime *pnow)
{
  संरचना dnscache_prune_दत्त user;

  user.max_age_ms = cache_timeout_ms;
  user.pnow = pnow;
  user.oldest_ms = 0;

  Curl_hash_clean_with_criterium(hostcache,
                                 (शून्य *)&user,
                                 dnscache_प्रविष्टिः_is_stale);

  प्रत्यागम user.oldest_ms;
}

स्थिर संरचना Curl_dnscache *dnscache_get(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->share && दत्त->share->specifier & (1 << CURL_तालकम्_दत्त_DNS))
    प्रत्यागम &दत्त->share->dnscache;
  यदि(दत्त->multi)
    प्रत्यागम &दत्त->multi->dnscache;
  प्रत्यागम रिक्त;
}

स्थिर शून्य dnscache_तालकम्(संरचना Curl_easy *दत्त,
                          संरचना Curl_dnscache *dnscache)
{
  यदि(दत्त->share && dnscache == &दत्त->share->dnscache)
    Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_DNS, CURL_तालकम्_ACCESS_SINGLE);
}

स्थिर शून्य dnscache_unlock(संरचना Curl_easy *दत्त,
                            संरचना Curl_dnscache *dnscache)
{
  यदि(दत्त->share && dnscache == &दत्त->share->dnscache)
    Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_DNS);
}

/*
 * Library-wide function for pruning the DNS cache. This function takes and
 * returns the appropriate locks.
 */
शून्य Curl_dnscache_prune(संरचना Curl_easy *दत्त, ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  /* the timeout may be set -1 (forever) */
  timediff_t timeout_ms = दत्त->समुच्चयः.dns_cache_timeout_ms;

  यदि(!dnscache || (timeout_ms == -1))
    /* NULL hostcache means we cannot do it */
    प्रत्यागम;

  dnscache_तालकम्(दत्त, dnscache);

  कुरु {
    /* Remove outdated and unused entries from the hostcache */
    timediff_t oldest_ms =
      dnscache_prune(&dnscache->entries, timeout_ms, pnow);

    यदि(Curl_hash_गणनम्(&dnscache->entries) > MAX_DNS_CACHE_आकार)
      /* prune the ones over half this age */
      timeout_ms = oldest_ms / 2;
    अन्यथा
      भङ्ग;

    /* if the cache size is still too big, use the oldest age as new prune
सीमा*/
  } यावत्(timeout_ms);

  dnscache_unlock(दत्त, dnscache);
}

शून्य Curl_dnscache_clear(संरचना Curl_easy *दत्त)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  यदि(dnscache) {
    dnscache_तालकम्(दत्त, dnscache);
    Curl_hash_clean(&dnscache->entries);
    dnscache_unlock(दत्त, dnscache);
  }
}

/* lookup address, returns entry if found and not stale */
स्थिर CURLcode fetch_प्रविष्टिः(संरचना Curl_easy *दत्त,
                            संरचना Curl_dnscache *dnscache,
                            uint8_t dns_queries,
                            संरचना Curl_peer *peer,
                            संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;
  वर्ण type = CURL_DNSQ_IS_ADDR(dns_queries) ?
              CURL_DNST_ADDR : CURL_DNST_HTTPS;
  CURLcode परिणाम = CURLE_OK;

  *pdns = रिक्त;
  यदि(!dnscache)
    प्रत्यागम CURLE_OK;

  dnsc_peer2id(&id, type, peer);
  dnsc_id2key(&कुञ्चिका, &id);

  /* See if it is already in our dns cache */
  dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);

  यदि(dns && !dnsc_id_matches_प्रविष्टिः(&id, dns)) {
    /* May happen on hostnames longer than MAX_HOSTCACHE_LEN */
    dns = रिक्त;
  }

  /* No entry found in cache, check if we might have a wildcard entry */
  यदि(!dns && (type == CURL_DNST_ADDR) && दत्त->state.wildcard_resolve) {
    संरचना Curl_str wildname;

    curlx_str_नियोजय(&wildname, "*", 1);
    dnsc_str2id(&id, CURL_DNST_ADDR, &wildname, peer->पत्तनम्);
    dnsc_id2key(&कुञ्चिका, &id);

    /* See if it is already in our dns cache */
    dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);
  }

  यदि(dns && (दत्त->समुच्चयः.dns_cache_timeout_ms != -1)) {
    /* See whether the returned entry is stale. Done before we release lock */
    संरचना dnscache_prune_दत्त user;

    user.pnow = Curl_pgrs_now(दत्त);
    user.max_age_ms = दत्त->समुच्चयः.dns_cache_timeout_ms;
    user.oldest_ms = 0;

    यदि(dnscache_प्रविष्टिः_is_stale(&user, dns)) {
      infof(दत्त, "Hostname in DNS cache was stale, zapped");
      dns = रिक्त; /* the memory deallocation is being handled by the hash */
      Curl_hash_delete(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);
    }
  }

  /* We need to cache address information and HTTPS-RR separately. */
  यदि(dns && CURL_DNSQ_IS_ADDR(dns_queries)) {
    यदि((uint8_t)(dns->dns_queries & dns_queries) !=
       (uint8_t)(dns_queries & CURL_DNSQ_ADDR)) {
      /* The entry does not cover all wanted address queries, a miss. */
      dns = रिक्त;
    }
    अन्यथा यदि(!(dns->dns_responses & dns_queries)) {
      /* The entry has no responses for the wanted DNS queries. */
      CURL_TRC_DNS(दत्त, "cache entry does not have type=%s addresses",
                   Curl_resolv_query_str(dns_queries));
      dns = रिक्त;
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
    अन्यथा यदि(dns && !dns->addr) { /* negative entry */
      dns = रिक्त;
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  *pdns = dns;
  प्रत्यागम परिणाम;
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
CURLcode Curl_dnscache_get(संरचना Curl_easy *दत्त,
                           uint8_t dns_queries,
                           संरचना Curl_peer *peer,
                           संरचना Curl_dns_प्रविष्टिः **pentry)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  dnscache_तालकम्(दत्त, dnscache);
  परिणाम = fetch_प्रविष्टिः(दत्त, dnscache, dns_queries, peer, &dns);
  यदि(!परिणाम && dns)
    dns->refcount++; /* we pass out a reference */
  अन्यथा यदि(परिणाम) {
    DEBUGASSERT(!dns);
    dns = रिक्त;
  }
  dnscache_unlock(दत्त, dnscache);

  CURL_TRC_DNS(दत्त, "cache lookup %s:%u queries=%s -> %d %sfound",
               peer->hostname, peer->पत्तनम्, Curl_resolv_query_str(dns_queries),
               (पूर्णाङ्क)परिणाम, dns ? "" : "not ");
  *pentry = dns;
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_SHUFFLE_DNS
/*
 * Return # of addresses in a Curl_addrinfo struct
 */
स्थिर पूर्णाङ्क अङ्कः_स्थानाङ्काः(ध्रुव संरचना Curl_addrinfo *addr)
{
  पूर्णाङ्क i = 0;
  यावत्(addr) {
    addr = addr->ai_अग्र;
    i++;
  }
  प्रत्यागम i;
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
UNITTEST CURLcode dns_shuffle_addr(संरचना Curl_easy *दत्त,
                                   संरचना Curl_addrinfo **addr);
UNITTEST CURLcode dns_shuffle_addr(संरचना Curl_easy *दत्त,
                                   संरचना Curl_addrinfo **addr)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव पूर्णाङ्क अङ्कः_addrs = अङ्कः_स्थानाङ्काः(*addr);

  यदि(अङ्कः_addrs > 1) {
    संरचना Curl_addrinfo **nodes;
    CURL_TRC_DNS(दत्त, "Shuffling %d addresses", अङ्कः_addrs);

    nodes = curlx_malloc(अङ्कः_addrs * परिमाण(*nodes));
    यदि(nodes) {
      पूर्णाङ्क i;
      अचिह्नित पूर्णाङ्क *rnd;
      ध्रुव size_t rnd_आकार = अङ्कः_addrs * परिमाण(*rnd);

      /* build a plain array of Curl_addrinfo pointers */
      nodes[0] = *addr;
      प्रति(i = 1; i < अङ्कः_addrs; i++) {
        nodes[i] = nodes[i - 1]->ai_अग्र;
      }

      rnd = curlx_malloc(rnd_आकार);
      यदि(rnd) {
        /* Fisher-Yates shuffle */
        यदि(Curl_rand(दत्त, (अचिह्नित वर्ण *)rnd, rnd_आकार) == CURLE_OK) {
          संरचना Curl_addrinfo *swap_tmp;
          प्रति(i = अङ्कः_addrs - 1; i > 0; i--) {
            swap_tmp = nodes[rnd[i] % (अचिह्नित पूर्णाङ्क)(i + 1)];
            nodes[rnd[i] % (अचिह्नित पूर्णाङ्क)(i + 1)] = nodes[i];
            nodes[i] = swap_tmp;
          }

          /* relink list in the new order */
          प्रति(i = 1; i < अङ्कः_addrs; i++) {
            nodes[i - 1]->ai_अग्र = nodes[i];
          }

          nodes[अङ्कः_addrs - 1]->ai_अग्र = रिक्त;
          *addr = nodes[0];
        }
        curlx_free(rnd);
      }
      अन्यथा
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      curlx_free(nodes);
    }
    अन्यथा
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

स्थिर bool dnscache_ai_has_family(संरचना Curl_addrinfo *ai, पूर्णाङ्क ai_family)
{
  प्रति(; ai; ai = ai->ai_अग्र) {
    यदि(ai->ai_family == ai_family)
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_प्रविष्टिः_create(संरचना dnsc_id *pid,
                                                bool permanent)
{
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;

  यदि(curlx_strlen(&pid->name) > UINT16_MAX)
    गच्छ बहिर्गमनम्;

  /* Create a new cache entry,
     struct already has the hostname null-terminator */
  dns = curlx_calloc(1, परिमाण(संरचना Curl_dns_प्रविष्टिः) +
                     curlx_strlen(&pid->name));
  यदि(!dns)
    गच्छ बहिर्गमनम्;

  dns->refcount = 1; /* the cache has the first reference */
  dns->पत्तनम् = pid->पत्तनम्;
  dns->hostlen = (uint16_t)curlx_strlen(&pid->name);
  यदि(dns->hostlen)
    स्मृतिं_प्रतिलिख(dns->hostname, curlx_str(&pid->name), dns->hostlen);
  dns->permanent = permanent;

बहिर्गमनम्:
  प्रत्यागम dns;
}

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_प्रविष्टिः_नियोजय_addr(
  संरचना Curl_easy *दत्त,
  संरचना Curl_dns_प्रविष्टिः *dns,
  uint8_t dns_queries,
  संरचना Curl_addrinfo **paddr1,
  संरचना Curl_addrinfo **paddr2)
{
  यदि(!dns)
    गच्छ बहिर्गमनम्;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  dns->type = CURL_DNST_ADDR;
  /* queries should only be about addresses */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  dns->dns_queries = (dns_queries & CURL_DNSQ_ADDR);

  /* Take the given address lists into the entry */
  यदि(paddr1 && *paddr1) {
    dns->addr = *paddr1;
    *paddr1 = रिक्त;
  }
  यदि(paddr2 && *paddr2) {
    संरचना Curl_addrinfo **phead = &dns->addr;
    यावत्(*phead)
      phead = &(*phead)->ai_अग्र;
    *phead = *paddr2;
    *paddr2 = रिक्त;
  }

  यदि((dns_queries & CURL_DNSQ_A) &&
     dnscache_ai_has_family(dns->addr, PF_INET))
    dns->dns_responses |= CURL_DNSQ_A;

#यदि_परिभाषितम् USE_IPV6
  यदि((dns_queries & CURL_DNSQ_AAAA) &&
     dnscache_ai_has_family(dns->addr, PF_INET6))
    dns->dns_responses |= CURL_DNSQ_AAAA;
#यदि_अन्तः /* USE_IPV6 */

#यदि_अपरिभाषितम् CURL_DISABLE_SHUFFLE_DNS
  /* shuffle addresses if requested */
  यदि(दत्त->समुच्चयः.dns_shuffle_स्थानाङ्काः && dns->addr) {
    CURLcode परिणाम = dns_shuffle_addr(दत्त, &dns->addr);
    यदि(परिणाम) {
      /* free without lock, we are the sole owner */
      dnscache_प्रविष्टिः_free(dns);
      dns = रिक्त;
      गच्छ बहिर्गमनम्;
    }
  }
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः

बहिर्गमनम्:
  यदि(paddr1 && *paddr1) {
    Curl_freeaddrinfo(*paddr1);
    *paddr1 = रिक्त;
  }
  यदि(paddr2 && *paddr2) {
    Curl_freeaddrinfo(*paddr2);
    *paddr2 = रिक्त;
  }
  प्रत्यागम dns;
}

संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_addr(संरचना Curl_easy *दत्त,
                                         uint8_t dns_queries,
                                         संरचना Curl_addrinfo **paddr,
                                         संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_प्रविष्टिः *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_प्रविष्टिः_create(&id, FALSE);
  dns = dnsc_प्रविष्टिः_नियोजय_addr(दत्त, dns, dns_queries, paddr, रिक्त);
  प्रत्यागम dns;
}

संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_addr2(संरचना Curl_easy *दत्त,
                                          uint8_t dns_queries,
                                          संरचना Curl_addrinfo **paddr1,
                                          संरचना Curl_addrinfo **paddr2,
                                          संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_प्रविष्टिः *dns;

  dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
  dns = dnsc_प्रविष्टिः_create(&id, FALSE);
  dns = dnsc_प्रविष्टिः_नियोजय_addr(दत्त, dns, dns_queries, paddr1, paddr2);
  प्रत्यागम dns;
}

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_सङ्कलय_प्रविष्टिः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_dnscache *dnscache,
                                             संरचना dnsc_कुञ्चिका *कुञ्चिका,
                                             संरचना Curl_dns_प्रविष्टिः *प्रविष्टिः)
{
  प्रविष्टिः->added = *Curl_pgrs_now(दत्त);
  प्रत्यागम Curl_hash_सङ्कलय(&dnscache->entries, कुञ्चिका->दत्त, कुञ्चिका->len, प्रविष्टिः);
}

#यदि_परिभाषितम् USE_HTTPSRR
स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_प्रविष्टिः_नियोजय_https(
  संरचना Curl_dns_प्रविष्टिः *dns,
  संरचना Curl_https_rrinfo **phinfo)
{
  यदि(!dns)
    गच्छ बहिर्गमनम्;
  /* only do this when this is the only reference */
  DEBUGASSERT(dns->refcount == 1);
  DEBUGASSERT(dns->type == CURL_DNST_INIT);

  यदि(dns->hinfo) {
    Curl_httpsrr_destroy(dns->hinfo);
    dns->hinfo = रिक्त;
  }
  dns->type = CURL_DNST_HTTPS;
  dns->dns_responses = dns->dns_queries = CURL_DNSQ_HTTPS;
  यदि(phinfo) {
    dns->hinfo = *phinfo;
    *phinfo = रिक्त;
  }
बहिर्गमनम्:
  यदि(phinfo && *phinfo) {
    Curl_httpsrr_destroy(*phinfo);
    *phinfo = रिक्त;
  }
  प्रत्यागम dns;
}

संरचना Curl_dns_प्रविष्टिः *Curl_dnsc_mk_https(संरचना Curl_easy *दत्त,
                                          संरचना Curl_https_rrinfo **phinfo,
                                          संरचना Curl_peer *peer)
{
  संरचना dnsc_id id;
  संरचना Curl_dns_प्रविष्टिः *dns;

  (शून्य)दत्त;
  dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
  dns = dnsc_प्रविष्टिः_create(&id, FALSE);
  dns = dnsc_प्रविष्टिः_नियोजय_https(dns, phinfo);
  प्रत्यागम dns;
}

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_सङ्कलय_https(संरचना Curl_easy *दत्त,
                                             संरचना Curl_dnscache *dnscache,
                                             संरचना Curl_https_rrinfo **phinfo,
                                             संरचना dnsc_id *id)
{
  संरचना Curl_dns_प्रविष्टिः *dns, *dns2;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  dns = dnsc_प्रविष्टिः_create(id, FALSE);
  dns = dnsc_प्रविष्टिः_नियोजय_https(dns, phinfo);
  यदि(!dns)
    प्रत्यागम रिक्त;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुञ्चिका, id);
  dns2 = dnsc_सङ्कलय_प्रविष्टिः(दत्त, dnscache, &कुञ्चिका, dns);
  यदि(!dns2) {
    dnscache_प्रविष्टिः_free(dns);
    प्रत्यागम रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  प्रत्यागम dns;
}
#यदि_अन्तः /* USE_HTTPSRR */

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_सङ्कलय_addr(संरचना Curl_easy *दत्त,
                                            संरचना Curl_dnscache *dnscache,
                                            uint8_t dns_queries,
                                            संरचना Curl_addrinfo **paddr,
                                            संरचना dnsc_id *id,
                                            संरचना dnsc_कुञ्चिका *कुञ्चिका,
                                            bool permanent)
{
  संरचना Curl_dns_प्रविष्टिः *dns;
  संरचना Curl_dns_प्रविष्टिः *dns2;

  dns = dnsc_प्रविष्टिः_create(id, permanent);
  dns = dnsc_प्रविष्टिः_नियोजय_addr(दत्त, dns, dns_queries, paddr, रिक्त);
  यदि(!dns)
    प्रत्यागम रिक्त;

  /* Store the resolved data in our DNS cache. */
  dns2 = dnsc_सङ्कलय_प्रविष्टिः(दत्त, dnscache, कुञ्चिका, dns);
  यदि(!dns2) {
    dnscache_प्रविष्टिः_free(dns);
    प्रत्यागम रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  प्रत्यागम dns;
}

स्थिर संरचना Curl_dns_प्रविष्टिः *dnsc_सङ्कलय_peer_addr(
  संरचना Curl_easy *दत्त,
  संरचना Curl_dnscache *dnscache,
  uint8_t dns_queries,
  संरचना Curl_addrinfo **paddr,
  संरचना dnsc_id *id,
  bool permanent)
{
  संरचना Curl_dns_प्रविष्टिः *dns;
  संरचना Curl_dns_प्रविष्टिः *dns2;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  dns = dnsc_प्रविष्टिः_create(id, permanent);
  dns = dnsc_प्रविष्टिः_नियोजय_addr(दत्त, dns, dns_queries, paddr, रिक्त);
  यदि(!dns)
    प्रत्यागम रिक्त;

  /* Store the resolved data in our DNS cache. */
  dnsc_id2key(&कुञ्चिका, id);
  dns2 = dnsc_सङ्कलय_प्रविष्टिः(दत्त, dnscache, &कुञ्चिका, dns);
  यदि(!dns2) {
    dnscache_प्रविष्टिः_free(dns);
    प्रत्यागम रिक्त;
  }

  dns = dns2;
  dns->refcount++; /* mark entry as in-use */
  प्रत्यागम dns;
}

CURLcode Curl_dnscache_सङ्कलय(संरचना Curl_easy *दत्त,
                           संरचना Curl_dns_प्रविष्टिः *प्रविष्टिः)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  संरचना Curl_str name;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  यदि(!dnscache)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!प्रविष्टिः || (प्रविष्टिः->type == CURL_DNST_INIT))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  curlx_str_नियोजय(&name, प्रविष्टिः->hostname, प्रविष्टिः->hostlen);
  dnsc_str2id(&id, प्रविष्टिः->type, &name, प्रविष्टिः->पत्तनम्);
  dnsc_id2key(&कुञ्चिका, &id);

  /* Store the resolved data in our DNS cache and up ref count */
  dnscache_तालकम्(दत्त, dnscache);
  यदि(!dnsc_सङ्कलय_प्रविष्टिः(दत्त, dnscache, &कुञ्चिका, प्रविष्टिः)) {
    dnscache_unlock(दत्त, dnscache);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रविष्टिः->refcount++;
  dnscache_unlock(दत्त, dnscache);
  CURL_TRC_DNS(दत्त, "cached entry for %s:%u queries=%s",
               प्रविष्टिः->hostname, प्रविष्टिः->पत्तनम्,
               Curl_resolv_query_str(प्रविष्टिः->dns_queries));
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_dnscache_सङ्कलय_negative(संरचना Curl_easy *दत्त,
                                    uint8_t dns_queries,
                                    संरचना Curl_peer *peer)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  संरचना dnsc_id id;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(dnscache);
  यदि(!dnscache)
    प्रत्यागम CURLE_FAILED_INIT;

  dnscache_तालकम्(दत्त, dnscache);

  यदि(dns_queries & CURL_DNSQ_ADDR) {
    /* put this new host in the cache */
    dnsc_peer2id(&id, CURL_DNST_ADDR, peer);
    dns = dnsc_सङ्कलय_peer_addr(दत्त, dnscache, dns_queries, रिक्त, &id, FALSE);
    यदि(!dns)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_परिभाषितम् USE_HTTPSRR
  अन्यथा यदि(dns_queries == CURL_DNSQ_HTTPS) {
    dnsc_peer2id(&id, CURL_DNST_HTTPS, peer);
    dns = dnsc_सङ्कलय_https(दत्त, dnscache, रिक्त, &id);
    यदि(!dns)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_अन्तः
  अन्यथा {
    /* a query we do not know, just cache nothing */
    DEBUGASSERT(0);
  }

  यदि(dns) {
    /* release the returned reference; the cache itself will keep the
     * entry alive: */
    dns->refcount--;
    CURL_TRC_DNS(दत्त, "cache negative name resolve for %s:%d type=%s",
                 peer->hostname, peer->पत्तनम्,
                 Curl_resolv_query_str(dns_queries));
  }
  dnscache_unlock(दत्त, dnscache);
  प्रत्यागम परिणाम;
}

/*
 * Curl_dns_entry_unlink() releases a reference to the given cached DNS entry.
 * When the reference count reaches 0, the entry is destroyed. It is important
 * that only one unlink is made for each Curl_resolv() call.
 *
 * May be called with 'data' == NULL for global cache.
 */
शून्य Curl_dns_प्रविष्टिः_unlink(संरचना Curl_easy *दत्त,
                           संरचना Curl_dns_प्रविष्टिः **pdns)
{
  यदि(*pdns) {
    संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
    संरचना Curl_dns_प्रविष्टिः *dns = *pdns;
    *pdns = रिक्त;
    dnscache_तालकम्(दत्त, dnscache);
    dns->refcount--;
    यदि(dns->refcount == 0)
      dnscache_प्रविष्टिः_free(dns);
    dnscache_unlock(दत्त, dnscache);
  }
}

स्थिर शून्य dnscache_प्रविष्टिः_dtor(शून्य *प्रविष्टिः)
{
  संरचना Curl_dns_प्रविष्टिः *dns = (संरचना Curl_dns_प्रविष्टिः *)प्रविष्टिः;
  DEBUGASSERT(dns && (dns->refcount > 0));
  dns->refcount--;
  यदि(dns->refcount == 0)
    dnscache_प्रविष्टिः_free(dns);
}

/*
 * Curl_dnscache_init() inits a new DNS cache.
 */
शून्य Curl_dnscache_init(संरचना Curl_dnscache *dns, size_t आकार)
{
  Curl_hash_init(&dns->entries, आकार, CURL_HASH_TYPE_अष्टकानि,
                 dnscache_प्रविष्टिः_dtor);
}

शून्य Curl_dnscache_destroy(संरचना Curl_dnscache *dns)
{
  Curl_hash_destroy(&dns->entries);
}

CURLcode Curl_loadhostpairs(संरचना Curl_easy *दत्त)
{
  संरचना Curl_dnscache *dnscache = dnscache_get(दत्त);
  संरचना curl_slist *hostp;
  संरचना dnsc_id id;
  संरचना dnsc_कुञ्चिका कुञ्चिका;

  यदि(!dnscache)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Default is no wildcard found */
  दत्त->state.wildcard_resolve = FALSE;

  प्रति(hostp = दत्त->state.resolve; hostp; hostp = hostp->अग्र) {
    ध्रुव वर्ण *host = hostp->दत्त;
    संरचना Curl_str source;
    यदि(!host)
      अनुवर्त;
    यदि(*host == '-') {
      curl_off_t अङ्कः = 0;
      host++;
      यदि(!curlx_str_single(&host, '[')) {
        यदि(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']') ||
           curlx_str_single(&host, ':'))
          अनुवर्त;
      }
      अन्यथा {
        यदि(curlx_str_until(&host, &source, 4096, ':') ||
           curlx_str_single(&host, ':')) {
          अनुवर्त;
        }
      }

      यदि(!curlx_str_सङ्ख्या(&host, &अङ्कः, 0xffff)) {
        /* Create an entry id, based upon the hostname and port */
        dnsc_str2id(&id, CURL_DNST_ADDR, &source, (uint16_t)अङ्कः);
        dnsc_id2key(&कुञ्चिका, &id);
        dnscache_तालकम्(दत्त, dnscache);
        /* delete entry, ignore if it did not exist */
        Curl_hash_delete(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);
        dnscache_unlock(दत्त, dnscache);
      }
    }
    अन्यथा {
      संरचना Curl_dns_प्रविष्टिः *dns;
      संरचना Curl_addrinfo *शीर्ष = रिक्त, *पुच्छ = रिक्त;
      वर्ण स्थानाङ्कः[64];
      curl_off_t tmpofft = 0;
      uint16_t पत्तनम् = 0;
      bool permanent = TRUE;
      bool error = TRUE;
      VERBOSE(ध्रुव वर्ण *स्थानाङ्काः = रिक्त);

      यदि(*host == '+') {
        host++;
        permanent = FALSE;
      }
      यदि(!curlx_str_single(&host, '[')) {
        यदि(curlx_str_until(&host, &source, MAX_IPADR_LEN, ']') ||
           curlx_str_single(&host, ']'))
          अनुवर्त;
      }
      अन्यथा {
        यदि(curlx_str_until(&host, &source, 4096, ':'))
          अनुवर्त;
      }
      यदि(curlx_str_single(&host, ':') ||
         curlx_str_सङ्ख्या(&host, &tmpofft, 0xffff) ||
         curlx_str_single(&host, ':'))
        गच्छ err;
      पत्तनम् = (uint16_t)tmpofft;

      VERBOSE(स्थानाङ्काः = host);

      /* start the address section */
      यावत्(*host) {
        संरचना Curl_str target;
        संरचना Curl_addrinfo *ai;
        CURLcode परिणाम;

        यदि(!curlx_str_single(&host, '[')) {
          यदि(curlx_str_until(&host, &target, MAX_IPADR_LEN, ']') ||
             curlx_str_single(&host, ']'))
            गच्छ err;
        }
        अन्यथा {
          यदि(curlx_str_until(&host, &target, 4096, ',')) {
            यदि(curlx_str_single(&host, ','))
              गच्छ err;
            /* survive nothing but a comma */
            अनुवर्त;
          }
        }
#यदि_अपरिभाषितम् USE_IPV6
        यदि(memchr(curlx_str(&target), ':', curlx_strlen(&target))) {
          infof(दत्त, "Ignoring resolve address '%.*s', missing IPv6 support.",
                (पूर्णाङ्क)curlx_strlen(&target), curlx_str(&target));
          यदि(curlx_str_single(&host, ','))
            गच्छ err;
          अनुवर्त;
        }
#यदि_अन्तः

        यदि(curlx_strlen(&target) >= परिमाण(स्थानाङ्कः))
          गच्छ err;

        स्मृतिं_प्रतिलिख(स्थानाङ्कः, curlx_str(&target), curlx_strlen(&target));
        स्थानाङ्कः[curlx_strlen(&target)] = '\0';

        परिणाम = Curl_str2addr(स्थानाङ्कः, पत्तनम्, &ai);
        यदि(परिणाम) {
          infof(दत्त, "Resolve IP address '%s' found is illegal", स्थानाङ्कः);
          गच्छ err;
        }

        यदि(पुच्छ) {
          पुच्छ->ai_अग्र = ai;
          पुच्छ = पुच्छ->ai_अग्र;
        }
        अन्यथा {
          शीर्ष = पुच्छ = ai;
        }
        यदि(curlx_str_single(&host, ','))
          भङ्ग;
      }

      यदि(!शीर्ष)
        गच्छ err;

      error = FALSE;
err:
      यदि(error) {
        failf(दत्त, "Could not parse CURLOPT_RESOLVE entry '%s'", hostp->दत्त);
        Curl_freeaddrinfo(शीर्ष);
        प्रत्यागम CURLE_SETOPT_OPTION_SYNTAX;
      }

      dnsc_str2id(&id, CURL_DNST_ADDR, &source, पत्तनम्);
      dnsc_id2key(&कुञ्चिका, &id);
      dnscache_तालकम्(दत्त, dnscache);

      /* See if it is already in our dns cache */
      dns = Curl_hash_pick(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);

      यदि(dns) {
        infof(दत्त, "RESOLVE %.*s:%u - old addresses discarded",
              (पूर्णाङ्क)curlx_strlen(&source), curlx_str(&source), पत्तनम्);
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

        Curl_hash_delete(&dnscache->entries, कुञ्चिका.दत्त, कुञ्चिका.len);
      }

      /* put this new host in the cache, override all address queries */
      dns = dnsc_सङ्कलय_addr(दत्त, dnscache, CURL_DNSQ_ADDR, &शीर्ष,
                          &id, &कुञ्चिका, permanent);
      यदि(dns)
        /* release the returned reference; the cache itself will keep the
         * entry alive: */
        dns->refcount--;

      dnscache_unlock(दत्त, dnscache);

      यदि(!dns)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      infof(दत्त, "[DNS] added %.*s:%u:%s to cache%s",
            (पूर्णाङ्क)curlx_strlen(&id.name), curlx_str(&id.name), id.पत्तनम्,
            स्थानाङ्काः, permanent ? "" : " (non-permanent)");

      /* Wildcard hostname */
      यदि(curlx_str_casecompare(&source, "*")) {
        infof(दत्त, "RESOLVE *:%u using wildcard", पत्तनम्);
        दत्त->state.wildcard_resolve = TRUE;
      }
    }
  }
  दत्त->state.resolve = रिक्त; /* dealt with now */

  प्रत्यागम CURLE_OK;
}
