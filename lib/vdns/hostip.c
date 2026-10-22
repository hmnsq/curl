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

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#ضمن <signal.h>

#ضمن "urldata.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "url.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "strcase.h"
#ضمن "easy_lock.h"
#ضمن "vdns/dnscache.h"
#ضمن "vdns/doh.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/strparse.h"

#ܐܢ defined(CURLRES_SYNCH) &&                   \
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
ساكن bool ipv6works(بنية Curl_easy *data);
#ܐܚܪܢܐ
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

uint8_t Curl_resolv_dns_queries(بنية Curl_easy *data, uint8_t ip_version)
{
  (فراغ)data;
  بدل(ip_version) {
  حالة CURL_IPRESOLVE_V6:
    ܦܢܐ CURL_DNSQ_AAAA;
  حالة CURL_IPRESOLVE_V4:
    ܦܢܐ CURL_DNSQ_A;
  افتراضي:
    ܐܢ(ipv6works(data))
      ܦܢܐ (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    ܐܚܪܢܐ
      ܦܢܐ CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
ثابت حرف *Curl_resolv_query_str(uint8_t dns_queries)
{
  بدل(dns_queries) {
  حالة (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ܦܢܐ "A+AAAA+HTTPS";
  حالة (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ܦܢܐ "A+AAAA";
  حالة (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ܦܢܐ "AAAA+HTTPS";
  حالة (CURL_DNSQ_AAAA):
    ܦܢܐ "AAAA";
  حالة (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ܦܢܐ "A+HTTPS";
  حالة (CURL_DNSQ_A):
    ܦܢܐ "A";
  حالة (CURL_DNSQ_HTTPS):
    ܦܢܐ "HTTPS";
  حالة 0:
    ܦܢܐ "-";
  افتراضي:
    DEBUGASSERT(0);
    ܦܢܐ "???";
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
فراغ Curl_printable_address(ثابت بنية Curl_addrinfo *ai, حرف *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  بدل(ai->ai_family) {
  حالة AF_INET: {
    ثابت بنية sockaddr_in *sa4 = (ثابت فراغ *)ai->ai_addr;
    ثابت بنية in_addr *ipaddr4 = &sa4->sin_addr;
    (فراغ)curlx_inet_ntop(ai->ai_family, (ثابت فراغ *)ipaddr4, buf, bufsize);
    اكسر;
  }
#ifdef USE_IPV6
  حالة AF_INET6: {
    ثابت بنية sockaddr_in6 *sa6 = (ثابت فراغ *)ai->ai_addr;
    ثابت بنية in6_addr *ipaddr6 = &sa6->sin6_addr;
    (فراغ)curlx_inet_ntop(ai->ai_family, (ثابت فراغ *)ipaddr6, buf, bufsize);
    اكسر;
  }
#endif
  افتراضي:
    اكسر;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ساكن sigjmp_buf curl_jmpenv;
ساكن curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
ساكن بنية Curl_addrinfo *get_localhost6(uint16_t port, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  ثابت size_t ss_size = حجم(بنية sockaddr_in6);
  ثابت size_t hostlen = strlen(name);
  بنية sockaddr_in6 sa6;
  unsigned حرف ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_size + hostlen + 1);
  ܐܢ(!ca)
    ܦܢܐ عدم;

  memset(&sa6, 0, حجم(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (فراغ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, حجم(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = عدم;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ܦܢܐ ca;
}
#ܐܚܪܢܐ
#define get_localhost6(x, y) عدم
#endif

/* return a static IPv4 127.0.0.1 for the given name */
ساكن بنية Curl_addrinfo *get_localhost(uint16_t port, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  بنية Curl_addrinfo *ca6;
  ثابت size_t ss_size = حجم(بنية sockaddr_in);
  ثابت size_t hostlen = strlen(name);
  بنية sockaddr_in sa;
  unsigned صحيح ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, حجم(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  ܐܢ(curlx_inet_pton(AF_INET, "127.0.0.1", (حرف *)&ipv4) < 1)
    ܦܢܐ عدم;
  memcpy(&sa.sin_addr, &ipv4, حجم(ipv4));

  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_size + hostlen + 1);
  ܐܢ(!ca)
    ܦܢܐ عدم;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  ܐܢ(!ca6)
    ܦܢܐ ca;
  ca6->ai_next = ca;
  ܦܢܐ ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(بنية Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  ܐܢ(s == CURL_SOCKET_BAD) {
    ܐܢ(SOCKERRNO == SOCKENOMEM)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }
  ܐܚܪܢܐ {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  ܦܢܐ CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ساكن bool ipv6works(بنية Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  ܦܢܐ data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت حرف *hostname)
{
  بنية in_addr in;
#ifdef USE_IPV6
  بنية in6_addr in6;
#endif
  ܐܢ(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    ܦܢܐ TRUE;
  ܦܢܐ FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ساكن bool tailmatch(ثابت حرف *full, size_t flen,
                      ثابت حرف *part, size_t plen)
{
  ܐܢ(plen > flen)
    ܦܢܐ FALSE;
  ܦܢܐ curl_strnequal(part, &full[flen - plen], plen);
}

ساكن CURLcode hostip_resolv_failed(بنية Curl_easy *data,
                                     بنية Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ܦܢܐ RESOLV_FAIL(for_proxy);
}

ساكن bool can_resolve_dns_queries(بنية Curl_easy *data,
                                    uint8_t dns_queries)
{
  (فراغ)data;
  ܐܢ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    ܦܢܐ FALSE;
  ܦܢܐ TRUE;
}

CURLcode Curl_resolv_announce_start(بنية Curl_easy *data,
                                    فراغ *resolver)
{
  ܐܢ(data->set.resolver_start) {
    بنية Curl_mapi_guard guard;
    صحيح rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, عدم,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    ܐܢ(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      ܦܢܐ CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ܦܢܐ CURLE_OK;
}

#ifdef USE_CURL_ASYNC

ساكن بنية Curl_resolv_async *hostip_async_new(بنية Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  بنية Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  بنية Curl_resolv_async *async;

  ܐܢ(!data->multi) {
    DEBUGASSERT(0);
    ܦܢܐ عدم;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, حجم(*async));
  ܐܢ(!async)
    ܦܢܐ عدم;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  ܐܢ(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  ܐܚܪܢܐ
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  ܐܢ(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ܦܢܐ async;
}

ساكن CURLcode hostip_resolv_take_result(بنية Curl_easy *data,
                                          بنية Curl_resolv_async *async,
                                          بنية Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  ܐܢ(!async)
    ܦܢܐ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  ܐܢ(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  ܐܚܪܢܐ
#endif
  result = Curl_async_take_result(data, async, pdns);

  ܐܢ(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  ܐܚܪܢܐ ܐܢ(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, عدم);
  }
  ܐܚܪܢܐ ܐܢ(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحيح)result, async->peer->hostname, async->peer->port);
  }
  ܐܚܪܢܐ {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  ܦܢܐ result;
}

timediff_t Curl_resolv_elapsed_ms(بنية Curl_easy *data,
                                  uint32_t resolv_id,
                                  ثابت بنية curltime *pnow)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ܐܢ(!async)
    ܦܢܐ CURL_TIMEOUT_RESOLVE_MS;
  ܦܢܐ curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(بنية Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  ܐܢ(!async || async->done)
    ܦܢܐ TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  ܐܢ((check_queries & async->dns_responses) != check_queries) {
    ܦܢܐ FALSE;
  }
  ܦܢܐ TRUE;
}

ثابت بنية Curl_addrinfo *Curl_resolv_get_ai(بنية Curl_easy *data,
                                               uint32_t resolv_id,
                                               صحيح ai_family,
                                               unsigned صحيح index)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ܐܢ(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ܦܢܐ عدم;
  بدل(ai_family) {
  حالة AF_INET:
    ܦܢܐ Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  حالة AF_INET6:
    ܦܢܐ Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  افتراضي:
    ܦܢܐ عدم;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(بنية Curl_easy *data,
                           بنية Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           بنية Curl_dns_entry **pdns)
{
  ܦܢܐ Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت بنية Curl_https_rrinfo *
Curl_resolv_get_https(بنية Curl_easy *data, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async;
  ܐܢ(!Curl_resolv_knows_https(data, resolv_id))
    ܦܢܐ عدم;
  async = Curl_async_get(data, resolv_id);
  ܦܢܐ async ? async->httpsrr : عدم;
}

bool Curl_resolv_knows_https(بنية Curl_easy *data, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ܐܢ(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ܦܢܐ ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ܦܢܐ TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ساكن CURLcode hostip_resolv_start(بنية Curl_easy *data,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    بنية Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  بنية Curl_resolv_async *async = عدم;
#endif
  بنية Curl_addrinfo *addr = عدم;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (فراغ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

  /* Check for "known" things to resolve ourselves. */
  ܐܢ(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    ܐܢ(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, عدم);
      ܐܢ(result)
        اذهب out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      اذهب out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    ܐܢ(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, عدم);
      ܐܢ(result)
        اذهب out;
      addr = get_localhost(peer->port, peer->hostname);
      ܐܢ(!addr)
        result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }
#ifndef CURL_DISABLE_DOH
  ܐܢ(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, عدم);
    ܐܢ(result)
      اذهب out;
    ܐܢ(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      ܐܢ(!async) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
    result = Curl_doh(data, async);
    اذهب out;
  }
#ܐܚܪܢܐ
  (فراغ)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  ܐܢ(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    اذهب out;
  }

#ifdef CURLRES_ASYNCH
  (فراغ)addr;
  ܐܢ(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    ܐܢ(!async) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  ܐܢ(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    ܐܢ(r2)
      result = r2;
    ܐܚܪܢܐ ܐܢ(*pdns)
      result = CURLE_OK;
  }
#ܐܚܪܢܐ
  result = Curl_resolv_announce_start(data, عدم);
  ܐܢ(result)
    اذهب out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  ܐܢ(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  ܐܢ(!result) {
    ܐܢ(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      ܐܢ(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    ܐܚܪܢܐ ܐܢ(!*pdns)
      result = CURLE_AGAIN;
  }
  ܐܚܪܢܐ ܐܢ(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  ܐܚܪܢܐ ܐܢ(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  ܐܢ(async) {
    ܐܢ(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    ܐܚܪܢܐ {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  ܦܢܐ result;
}

ساكن CURLcode hostip_resolv(بنية Curl_easy *data,
                              uint8_t dns_queries,
                              بنية Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              بنية Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (فراغ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

#ifdef CURL_DISABLE_DOH
  (فراغ)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  ܐܢ(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    اذهب out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  ܐܢ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    اذهب out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  ܐܢ(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  ܐܚܪܢܐ ܐܢ(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  ܐܚܪܢܐ {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحيح)result);
  }

out:
  ܐܢ(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    ܐܢ(IS_RESOLV_FAIL(result)) {
      ܐܢ(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      ܐܢ(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    ܐܚܪܢܐ {
      failf(data, "Error %d resolving %s:%u",
            (صحيح)result, peer->hostname, peer->port);
    }
  }
  ܐܚܪܢܐ ܐܢ(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    ܐܢ(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  ܦܢܐ result;
}

CURLcode Curl_resolv_blocking(بنية Curl_easy *data,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t port,
                              uint8_t transport,
                              بنية Curl_dns_entry **pdns)
{
  بنية Curl_peer *peer = عدم;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = عدم;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  ܐܢ(result)
    اذهب out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  بدل(result) {
  حالة CURLE_OK:
    DEBUGASSERT(*pdns);
    اكسر;
#ifdef USE_CURL_ASYNC
  حالة CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    اكسر;
#endif
  افتراضي:
    اكسر;
  }

out:
  Curl_peer_unlink(&peer);
  ܦܢܐ result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ساكن فراغ alarmfunc(صحيح sig)
{
  (فراغ)sig;
  siglongjmp(curl_jmpenv, 1);
}

ساكن CURLcode resolv_alarm_timeout(بنية Curl_easy *data,
                                     uint8_t dns_queries,
                                     بنية Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     بنية Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  بنية sigaction keep_sigact; /* store the old struct here */
  متطاير bool keep_copysig = FALSE; /* whether old sigact has been saved */
  بنية sigaction sigact;
#ܐܚܪܢܐ
#ifdef HAVE_SIGNAL
  فراغ (*keep_sigact)(صحيح);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  متطاير long timeout;
  متطاير unsigned صحيح prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = عدم;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  ܐܢ(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ܦܢܐ CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  ܐܢ(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    اذهب clean_up;
  }
  ܐܚܪܢܐ {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, عدم, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, عدم);
#ܐܚܪܢܐ /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = إشارة(SIGALRM, alarmfunc);
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
  ܐܢ(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  ܐܢ(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, عدم); /* put it back */
  }
#ܐܚܪܢܐ
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  إشارة(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  ܐܢ(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    ܐܢ(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    ܐܚܪܢܐ
      alarm((unsigned صحيح)alarm_set);
  }

  ܦܢܐ result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
ساكن CURLcode resolv_unix(بنية Curl_easy *data,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_entry **pdns)
{
  بنية Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = عدم;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  ܐܢ(result) {
    ܐܢ(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    ܦܢܐ result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  ܦܢܐ *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(بنية Curl_easy *data,
                     بنية Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     بنية Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = عدم;

  ܐܢ(timeout_ms < 0)
    /* got an already expired timeout */
    ܦܢܐ CURLE_OPERATION_TIMEDOUT;
  ܐܚܪܢܐ ܐܢ(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  ܐܢ((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ܦܢܐ resolv_unix(data, peer, pdns);
#ܐܚܪܢܐ
  ܐܢ(peer->unix_socket)
    ܦܢܐ hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  ܐܢ(dns_queries & CURL_DNSQ_ADDR) {
    ܐܢ(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    ܐܢ(timeout_ms && !Curl_doh_wanted(data)) {
      ܦܢܐ resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  ܐܢ(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  ܦܢܐ hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

بنية Curl_resolv_async *Curl_async_get(بنية Curl_easy *data,
                                         uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = data->state.async;
  لكل(; async; async = async->next) {
    ܐܢ(async->id == resolv_id)
      ܦܢܐ async;
  }
  ܦܢܐ عدم;
}

CURLcode Curl_resolv_take_result(بنية Curl_easy *data, uint32_t resolv_id,
                                 بنية Curl_dns_entry **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  ܐܢ(!async)
    ܦܢܐ CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  ܐܢ(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    ܦܢܐ CURLE_OK;
  }
  ܐܚܪܢܐ ܐܢ(result) {
    Curl_async_shutdown(data, async);
    ܦܢܐ Curl_async_failed(data, async, عدم);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  ܐܢ(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    ܐܢ(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  ܐܚܪܢܐ ܐܢ(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    ܐܢ(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    ܐܢ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  ܐܚܪܢܐ ܐܢ(result) {
    failf(data, "Error %d resolving %s:%u",
          (صحيح)result, async->peer->hostname, async->peer->port);
  }
  ܦܢܐ result;
}

CURLcode Curl_resolv_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  بنية Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (فراغ)ps;
  لكل(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    ܐܢ(async->doh) /* DoH has nothing for the pollset */
      استمر;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  ܦܢܐ result;
}

فراغ Curl_resolv_destroy(بنية Curl_easy *data, uint32_t resolv_id)
{
  بنية Curl_resolv_async **panchor = &data->state.async;

  لكل(; *panchor; panchor = &(*panchor)->next) {
    بنية Curl_resolv_async *async = *panchor;
    ܐܢ(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      اكسر;
    }
  }
}

فراغ Curl_resolv_shutdown_all(بنية Curl_easy *data)
{
  بنية Curl_resolv_async *async = data->state.async;
  لكل(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

فراغ Curl_resolv_destroy_all(بنية Curl_easy *data)
{
  ܟܡܐ(data->state.async) {
    بنية Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
