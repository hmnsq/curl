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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#அந்தர்பாவ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#அந்தர்பாவ <signal.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "easy_lock.h"
#அந்தர்பாவ "vdns/dnscache.h"
#அந்தர்பாவ "vdns/doh.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strcopy.h"
#அந்தர்பாவ "curlx/strparse.h"

#எனில் defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#define USE_ALARM_TIMEOUT
#endif

#define RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#define IS_RESOLV_FAIL(result) \
  (((result) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((result) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#ifdef USE_IPV6
நிலையான bool ipv6works(கட்டமைப்பு Curl_easy *data);
#இல்லையெனில்
#define ipv6works(x) FALSE
#endif

/*
 * hostip.c explained
 * ==================
 *
 * The main COMPILE-TIME DEFINES to keep in mind when reading the host*.c
 * source file are these:
 *
 * CURLRES_IPV6 - this host has getaddrinfo() and family, and thus we use
 * that. The host may not be able to resolve IPv6, but we do not really have to
 * take that into account. Hosts that are not IPv6-enabled have CURLRES_IPV4
 * defined.
 *
 * USE_RESOLV_ARES - is defined if libcurl is built to use c-ares for
 * asynchronous name resolves. This can be Windows or *nix.
 *
 * USE_RESOLV_THREADED - is defined if libcurl is built to run under (native)
 * Windows, and then the name resolve will be done in a new thread, and the
 * supported API will be the same as for ares-builds.
 *
 * If any of the two previous are defined, CURLRES_ASYNCH is defined too. If
 * libcurl is not built to use an asynchronous resolver, CURLRES_SYNCH is
 * defined.
 *
 * The host*.c sources files are split up like this:
 *
 * hostip.c   - method-independent resolver functions and utility functions
 * hostip4.c  - IPv4 specific functions
 * hostip6.c  - IPv6 specific functions
 * asyn.h     - common functions for all async resolvers
 * The two asynchronous name resolver backends are implemented in:
 * asyn-ares.c - async resolver using c-ares
 * asyn-thread.c - async resolver using POSIX threads
 *
 * The hostip.h is the united header file for all this. It defines the
 * CURLRES_* defines based on the config*.h and curl_setup.h defines.
 */

uint8_t Curl_resolv_dns_queries(கட்டமைப்பு Curl_easy *data, uint8_t ip_version)
{
  (வெறுமை)data;
  தேர்வு(ip_version) {
  நிலை CURL_IPRESOLVE_V6:
    திரும்பு CURL_DNSQ_AAAA;
  நிலை CURL_IPRESOLVE_V4:
    திரும்பு CURL_DNSQ_A;
  இயல்பு:
    எனில்(ipv6works(data))
      திரும்பு (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    இல்லையெனில்
      திரும்பு CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
மாறிலி எழுத்து *Curl_resolv_query_str(uint8_t dns_queries)
{
  தேர்வு(dns_queries) {
  நிலை (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    திரும்பு "A+AAAA+HTTPS";
  நிலை (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    திரும்பு "A+AAAA";
  நிலை (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    திரும்பு "AAAA+HTTPS";
  நிலை (CURL_DNSQ_AAAA):
    திரும்பு "AAAA";
  நிலை (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    திரும்பு "A+HTTPS";
  நிலை (CURL_DNSQ_A):
    திரும்பு "A";
  நிலை (CURL_DNSQ_HTTPS):
    திரும்பு "HTTPS";
  நிலை 0:
    திரும்பு "-";
  இயல்பு:
    DEBUGASSERT(0);
    திரும்பு "???";
  }
}
#endif

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
வெறுமை Curl_printable_address(மாறிலி கட்டமைப்பு Curl_addrinfo *ai, எழுத்து *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  தேர்வு(ai->ai_family) {
  நிலை AF_INET: {
    மாறிலி கட்டமைப்பு sockaddr_in *sa4 = (மாறிலி வெறுமை *)ai->ai_addr;
    மாறிலி கட்டமைப்பு in_addr *ipaddr4 = &sa4->sin_addr;
    (வெறுமை)curlx_inet_ntop(ai->ai_family, (மாறிலி வெறுமை *)ipaddr4, buf, bufsize);
    நிறுத்து;
  }
#ifdef USE_IPV6
  நிலை AF_INET6: {
    மாறிலி கட்டமைப்பு sockaddr_in6 *sa6 = (மாறிலி வெறுமை *)ai->ai_addr;
    மாறிலி கட்டமைப்பு in6_addr *ipaddr6 = &sa6->sin6_addr;
    (வெறுமை)curlx_inet_ntop(ai->ai_family, (மாறிலி வெறுமை *)ipaddr6, buf, bufsize);
    நிறுத்து;
  }
#endif
  இயல்பு:
    நிறுத்து;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
நிலையான sigjmp_buf curl_jmpenv;
நிலையான curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
நிலையான கட்டமைப்பு Curl_addrinfo *get_localhost6(uint16_t port, மாறிலி எழுத்து *name)
{
  கட்டமைப்பு Curl_addrinfo *ca;
  மாறிலி size_t ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
  மாறிலி size_t hostlen = strlen(name);
  கட்டமைப்பு sockaddr_in6 sa6;
  unsigned எழுத்து ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size + hostlen + 1);
  எனில்(!ca)
    திரும்பு வெற்று;

  memset(&sa6, 0, அளவு(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (வெறுமை)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, அளவு(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = வெற்று;
  ca->ai_addr = (வெறுமை *)((எழுத்து *)ca + அளவு(கட்டமைப்பு Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (எழுத்து *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  திரும்பு ca;
}
#இல்லையெனில்
#define get_localhost6(x, y) வெற்று
#endif

/* return a static IPv4 127.0.0.1 for the given name */
நிலையான கட்டமைப்பு Curl_addrinfo *get_localhost(uint16_t port, மாறிலி எழுத்து *name)
{
  கட்டமைப்பு Curl_addrinfo *ca;
  கட்டமைப்பு Curl_addrinfo *ca6;
  மாறிலி size_t ss_size = அளவு(கட்டமைப்பு sockaddr_in);
  மாறிலி size_t hostlen = strlen(name);
  கட்டமைப்பு sockaddr_in sa;
  unsigned முழு ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, அளவு(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  எனில்(curlx_inet_pton(AF_INET, "127.0.0.1", (எழுத்து *)&ipv4) < 1)
    திரும்பு வெற்று;
  memcpy(&sa.sin_addr, &ipv4, அளவு(ipv4));

  ca = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size + hostlen + 1);
  எனில்(!ca)
    திரும்பு வெற்று;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (வெறுமை *)((எழுத்து *)ca + அளவு(கட்டமைப்பு Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (எழுத்து *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  எனில்(!ca6)
    திரும்பு ca;
  ca6->ai_next = ca;
  திரும்பு ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(கட்டமைப்பு Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  எனில்(s == CURL_SOCKET_BAD) {
    எனில்(SOCKERRNO == SOCKENOMEM)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  திரும்பு CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
நிலையான bool ipv6works(கட்டமைப்பு Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  திரும்பு data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(மாறிலி எழுத்து *hostname)
{
  கட்டமைப்பு in_addr in;
#ifdef USE_IPV6
  கட்டமைப்பு in6_addr in6;
#endif
  எனில்(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    திரும்பு TRUE;
  திரும்பு FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
நிலையான bool tailmatch(மாறிலி எழுத்து *full, size_t flen,
                      மாறிலி எழுத்து *part, size_t plen)
{
  எனில்(plen > flen)
    திரும்பு FALSE;
  திரும்பு curl_strnequal(part, &full[flen - plen], plen);
}

நிலையான CURLcode hostip_resolv_failed(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  திரும்பு RESOLV_FAIL(for_proxy);
}

நிலையான bool can_resolve_dns_queries(கட்டமைப்பு Curl_easy *data,
                                    uint8_t dns_queries)
{
  (வெறுமை)data;
  எனில்((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    திரும்பு FALSE;
  திரும்பு TRUE;
}

CURLcode Curl_resolv_announce_start(கட்டமைப்பு Curl_easy *data,
                                    வெறுமை *resolver)
{
  எனில்(data->set.resolver_start) {
    கட்டமைப்பு Curl_mapi_guard guard;
    முழு rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, வெற்று,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    எனில்(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      திரும்பு CURLE_ABORTED_BY_CALLBACK;
    }
  }
  திரும்பு CURLE_OK;
}

#ifdef USE_CURL_ASYNC

நிலையான கட்டமைப்பு Curl_resolv_async *hostip_async_new(கட்டமைப்பு Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  கட்டமைப்பு Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  கட்டமைப்பு Curl_resolv_async *async;

  எனில்(!data->multi) {
    DEBUGASSERT(0);
    திரும்பு வெற்று;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, அளவு(*async));
  எனில்(!async)
    திரும்பு வெற்று;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  எனில்(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  இல்லையெனில்
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  எனில்(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  திரும்பு async;
}

நிலையான CURLcode hostip_resolv_take_result(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_resolv_async *async,
                                          கட்டமைப்பு Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  எனில்(!async)
    திரும்பு CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  எனில்(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  இல்லையெனில்
#endif
  result = Curl_async_take_result(data, async, pdns);

  எனில்(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  இல்லையெனில் எனில்(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, வெற்று);
  }
  இல்லையெனில் எனில்(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (முழு)result, async->peer->hostname, async->peer->port);
  }
  இல்லையெனில் {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  திரும்பு result;
}

timediff_t Curl_resolv_elapsed_ms(கட்டமைப்பு Curl_easy *data,
                                  uint32_t resolv_id,
                                  மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  எனில்(!async)
    திரும்பு CURL_TIMEOUT_RESOLVE_MS;
  திரும்பு curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(கட்டமைப்பு Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  எனில்(!async || async->done)
    திரும்பு TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  எனில்((check_queries & async->dns_responses) != check_queries) {
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

மாறிலி கட்டமைப்பு Curl_addrinfo *Curl_resolv_get_ai(கட்டமைப்பு Curl_easy *data,
                                               uint32_t resolv_id,
                                               முழு ai_family,
                                               unsigned முழு index)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  எனில்(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    திரும்பு வெற்று;
  தேர்வு(ai_family) {
  நிலை AF_INET:
    திரும்பு Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  நிலை AF_INET6:
    திரும்பு Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  இயல்பு:
    திரும்பு வெற்று;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           கட்டமைப்பு Curl_dns_entry **pdns)
{
  திரும்பு Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

மாறிலி கட்டமைப்பு Curl_https_rrinfo *
Curl_resolv_get_https(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id)
{
  கட்டமைப்பு Curl_resolv_async *async;
  எனில்(!Curl_resolv_knows_https(data, resolv_id))
    திரும்பு வெற்று;
  async = Curl_async_get(data, resolv_id);
  திரும்பு async ? async->httpsrr : வெற்று;
}

bool Curl_resolv_knows_https(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  எனில்(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    திரும்பு ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  திரும்பு TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
நிலையான CURLcode hostip_resolv_start(கட்டமைப்பு Curl_easy *data,
                                    uint8_t dns_queries,
                                    கட்டமைப்பு Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    கட்டமைப்பு Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  கட்டமைப்பு Curl_resolv_async *async = வெற்று;
#endif
  கட்டமைப்பு Curl_addrinfo *addr = வெற்று;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (வெறுமை)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = வெற்று;

  /* Check for "known" things to resolve ourselves. */
  எனில்(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    எனில்(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, வெற்று);
      எனில்(result)
        செல் out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      செல் out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    எனில்(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, வெற்று);
      எனில்(result)
        செல் out;
      addr = get_localhost(peer->port, peer->hostname);
      எனில்(!addr)
        result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
#ifndef CURL_DISABLE_DOH
  எனில்(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, வெற்று);
    எனில்(result)
      செல் out;
    எனில்(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      எனில்(!async) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
    result = Curl_doh(data, async);
    செல் out;
  }
#இல்லையெனில்
  (வெறுமை)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  எனில்(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    செல் out;
  }

#ifdef CURLRES_ASYNCH
  (வெறுமை)addr;
  எனில்(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    எனில்(!async) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  எனில்(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    எனில்(r2)
      result = r2;
    இல்லையெனில் எனில்(*pdns)
      result = CURLE_OK;
  }
#இல்லையெனில்
  result = Curl_resolv_announce_start(data, வெற்று);
  எனில்(result)
    செல் out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  எனில்(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  எனில்(!result) {
    எனில்(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      எனில்(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    இல்லையெனில் எனில்(!*pdns)
      result = CURLE_AGAIN;
  }
  இல்லையெனில் எனில்(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  இல்லையெனில் எனில்(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  எனில்(async) {
    எனில்(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    இல்லையெனில் {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  திரும்பு result;
}

நிலையான CURLcode hostip_resolv(கட்டமைப்பு Curl_easy *data,
                              uint8_t dns_queries,
                              கட்டமைப்பு Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              கட்டமைப்பு Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (வெறுமை)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = வெற்று;

#ifdef CURL_DISABLE_DOH
  (வெறுமை)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  எனில்(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    செல் out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  எனில்((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    செல் out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  எனில்(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  இல்லையெனில் எனில்(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  இல்லையெனில் {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (முழு)result);
  }

out:
  எனில்(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    எனில்(IS_RESOLV_FAIL(result)) {
      எனில்(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      எனில்(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    இல்லையெனில் {
      failf(data, "Error %d resolving %s:%u",
            (முழு)result, peer->hostname, peer->port);
    }
  }
  இல்லையெனில் எனில்(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    எனில்(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  திரும்பு result;
}

CURLcode Curl_resolv_blocking(கட்டமைப்பு Curl_easy *data,
                              uint8_t dns_queries,
                              மாறிலி எழுத்து *hostname,
                              uint16_t port,
                              uint8_t transport,
                              கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_peer *peer = வெற்று;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = வெற்று;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  எனில்(result)
    செல் out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  தேர்வு(result) {
  நிலை CURLE_OK:
    DEBUGASSERT(*pdns);
    நிறுத்து;
#ifdef USE_CURL_ASYNC
  நிலை CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    நிறுத்து;
#endif
  இயல்பு:
    நிறுத்து;
  }

out:
  Curl_peer_unlink(&peer);
  திரும்பு result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN நிலையான வெறுமை alarmfunc(முழு sig)
{
  (வெறுமை)sig;
  siglongjmp(curl_jmpenv, 1);
}

நிலையான CURLcode resolv_alarm_timeout(கட்டமைப்பு Curl_easy *data,
                                     uint8_t dns_queries,
                                     கட்டமைப்பு Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     கட்டமைப்பு Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  கட்டமைப்பு sigaction keep_sigact; /* store the old struct here */
  நிலையற்ற bool keep_copysig = FALSE; /* whether old sigact has been saved */
  கட்டமைப்பு sigaction sigact;
#இல்லையெனில்
#ifdef HAVE_SIGNAL
  வெறுமை (*keep_sigact)(முழு);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  நிலையற்ற long timeout;
  நிலையற்ற unsigned முழு prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = வெற்று;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  எனில்(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  எனில்(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    செல் clean_up;
  }
  இல்லையெனில் {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, வெற்று, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, வெற்று);
#இல்லையெனில் /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = சமிக்ஞை(SIGALRM, alarmfunc);
#endif
#endif /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  result = hostip_resolv(data, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, entry);

clean_up:
  எனில்(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  எனில்(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, வெற்று); /* put it back */
  }
#இல்லையெனில்
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  சமிக்ஞை(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  எனில்(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    எனில்(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    இல்லையெனில்
      alarm((unsigned முழு)alarm_set);
  }

  திரும்பு result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
நிலையான CURLcode resolv_unix(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_peer *peer,
                            கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = வெற்று;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  எனில்(result) {
    எனில்(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    திரும்பு result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  திரும்பு *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}
#endif /* USE_UNIX_SOCKETS */

/*
 * Curl_resolv() is the main name resolve function within libcurl. It resolves
 * a name and returns a pointer to the entry in the 'entry' argument. This
 * function might return immediately if we are using asynch resolves. See the
 * return codes.
 *
 * The cache entry we return will get its 'inuse' counter increased when this
 * function is used. You MUST call Curl_dns_entry_unlink() later (when you are
 * done using this struct) to decrease the reference counter again.
 *
 * If built with a synchronous resolver and use of signals is not
 * disabled by the application, then a nonzero timeout will cause a
 * timeout after the specified number of milliseconds. Otherwise, timeout
 * is ignored.
 *
 * Return codes:
 * CURLE_OK = success, *pdns set to non-NULL
 * CURLE_AGAIN = resolving in progress, *pdns == NULL
 * any other CURLcode error, *pdns == NULL
 */
CURLcode Curl_resolv(கட்டமைப்பு Curl_easy *data,
                     கட்டமைப்பு Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     கட்டமைப்பு Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = வெற்று;

  எனில்(timeout_ms < 0)
    /* got an already expired timeout */
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  இல்லையெனில் எனில்(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  எனில்((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    திரும்பு resolv_unix(data, peer, pdns);
#இல்லையெனில்
  எனில்(peer->unix_socket)
    திரும்பு hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  எனில்(dns_queries & CURL_DNSQ_ADDR) {
    எனில்(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    எனில்(timeout_ms && !Curl_doh_wanted(data)) {
      திரும்பு resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  எனில்(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  திரும்பு hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

கட்டமைப்பு Curl_resolv_async *Curl_async_get(கட்டமைப்பு Curl_easy *data,
                                         uint32_t resolv_id)
{
  கட்டமைப்பு Curl_resolv_async *async = data->state.async;
  க்கு(; async; async = async->next) {
    எனில்(async->id == resolv_id)
      திரும்பு async;
  }
  திரும்பு வெற்று;
}

CURLcode Curl_resolv_take_result(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id,
                                 கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  எனில்(!async)
    திரும்பு CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  எனில்(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(result) {
    Curl_async_shutdown(data, async);
    திரும்பு Curl_async_failed(data, async, வெற்று);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  எனில்(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    எனில்(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  இல்லையெனில் எனில்(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    எனில்(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    எனில்(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  இல்லையெனில் எனில்(result) {
    failf(data, "Error %d resolving %s:%u",
          (முழு)result, async->peer->hostname, async->peer->port);
  }
  திரும்பு result;
}

CURLcode Curl_resolv_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (வெறுமை)ps;
  க்கு(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    எனில்(async->doh) /* DoH has nothing for the pollset */
      தொடர்;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  திரும்பு result;
}

வெறுமை Curl_resolv_destroy(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id)
{
  கட்டமைப்பு Curl_resolv_async **panchor = &data->state.async;

  க்கு(; *panchor; panchor = &(*panchor)->next) {
    கட்டமைப்பு Curl_resolv_async *async = *panchor;
    எனில்(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      நிறுத்து;
    }
  }
}

வெறுமை Curl_resolv_shutdown_all(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_resolv_async *async = data->state.async;
  க்கு(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

வெறுமை Curl_resolv_destroy_all(கட்டமைப்பு Curl_easy *data)
{
  போது(data->state.async) {
    கட்டமைப்பு Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
