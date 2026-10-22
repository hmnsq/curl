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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#شامل <signal.h>

#شامل "urldata.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "url.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "strcase.h"
#شامل "easy_lock.h"
#شامل "vdns/dnscache.h"
#شامل "vdns/doh.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strcopy.h"
#شامل "curlx/strparse.h"

#اگر defined(CURLRES_SYNCH) &&                   \
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
ایستا bool ipv6works(ساختار Curl_easy *data);
#وگرنه
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

uint8_t Curl_resolv_dns_queries(ساختار Curl_easy *data, uint8_t ip_version)
{
  (تهی)data;
  انتخاب(ip_version) {
  حالت CURL_IPRESOLVE_V6:
    بازگشت CURL_DNSQ_AAAA;
  حالت CURL_IPRESOLVE_V4:
    بازگشت CURL_DNSQ_A;
  پیشفرض:
    اگر(ipv6works(data))
      بازگشت (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    وگرنه
      بازگشت CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
ثابت نویسه *Curl_resolv_query_str(uint8_t dns_queries)
{
  انتخاب(dns_queries) {
  حالت (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    بازگشت "A+AAAA+HTTPS";
  حالت (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    بازگشت "A+AAAA";
  حالت (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    بازگشت "AAAA+HTTPS";
  حالت (CURL_DNSQ_AAAA):
    بازگشت "AAAA";
  حالت (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    بازگشت "A+HTTPS";
  حالت (CURL_DNSQ_A):
    بازگشت "A";
  حالت (CURL_DNSQ_HTTPS):
    بازگشت "HTTPS";
  حالت 0:
    بازگشت "-";
  پیشفرض:
    DEBUGASSERT(0);
    بازگشت "???";
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
تهی Curl_printable_address(ثابت ساختار Curl_addrinfo *ai, نویسه *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  انتخاب(ai->ai_family) {
  حالت AF_INET: {
    ثابت ساختار sockaddr_in *sa4 = (ثابت تهی *)ai->ai_addr;
    ثابت ساختار in_addr *ipaddr4 = &sa4->sin_addr;
    (تهی)curlx_inet_ntop(ai->ai_family, (ثابت تهی *)ipaddr4, buf, bufsize);
    بشکن;
  }
#ifdef USE_IPV6
  حالت AF_INET6: {
    ثابت ساختار sockaddr_in6 *sa6 = (ثابت تهی *)ai->ai_addr;
    ثابت ساختار in6_addr *ipaddr6 = &sa6->sin6_addr;
    (تهی)curlx_inet_ntop(ai->ai_family, (ثابت تهی *)ipaddr6, buf, bufsize);
    بشکن;
  }
#endif
  پیشفرض:
    بشکن;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ایستا sigjmp_buf curl_jmpenv;
ایستا curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
ایستا ساختار Curl_addrinfo *get_localhost6(uint16_t port, ثابت نویسه *name)
{
  ساختار Curl_addrinfo *ca;
  ثابت size_t ss_size = اندازه(ساختار sockaddr_in6);
  ثابت size_t hostlen = strlen(name);
  ساختار sockaddr_in6 sa6;
  unsigned نویسه ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, اندازه(ساختار Curl_addrinfo) + ss_size + hostlen + 1);
  اگر(!ca)
    بازگشت پوچ;

  memset(&sa6, 0, اندازه(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (تهی)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, اندازه(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = پوچ;
  ca->ai_addr = (تهی *)((نویسه *)ca + اندازه(ساختار Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (نویسه *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  بازگشت ca;
}
#وگرنه
#define get_localhost6(x, y) پوچ
#endif

/* return a static IPv4 127.0.0.1 for the given name */
ایستا ساختار Curl_addrinfo *get_localhost(uint16_t port, ثابت نویسه *name)
{
  ساختار Curl_addrinfo *ca;
  ساختار Curl_addrinfo *ca6;
  ثابت size_t ss_size = اندازه(ساختار sockaddr_in);
  ثابت size_t hostlen = strlen(name);
  ساختار sockaddr_in sa;
  unsigned صحیح ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, اندازه(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  اگر(curlx_inet_pton(AF_INET, "127.0.0.1", (نویسه *)&ipv4) < 1)
    بازگشت پوچ;
  memcpy(&sa.sin_addr, &ipv4, اندازه(ipv4));

  ca = curlx_calloc(1, اندازه(ساختار Curl_addrinfo) + ss_size + hostlen + 1);
  اگر(!ca)
    بازگشت پوچ;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (تهی *)((نویسه *)ca + اندازه(ساختار Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (نویسه *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  اگر(!ca6)
    بازگشت ca;
  ca6->ai_next = ca;
  بازگشت ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(ساختار Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  اگر(s == CURL_SOCKET_BAD) {
    اگر(SOCKERRNO == SOCKENOMEM)
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  وگرنه {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  بازگشت CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ایستا bool ipv6works(ساختار Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  بازگشت data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت نویسه *hostname)
{
  ساختار in_addr in;
#ifdef USE_IPV6
  ساختار in6_addr in6;
#endif
  اگر(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    بازگشت TRUE;
  بازگشت FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ایستا bool tailmatch(ثابت نویسه *full, size_t flen,
                      ثابت نویسه *part, size_t plen)
{
  اگر(plen > flen)
    بازگشت FALSE;
  بازگشت curl_strnequal(part, &full[flen - plen], plen);
}

ایستا CURLcode hostip_resolv_failed(ساختار Curl_easy *data,
                                     ساختار Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  بازگشت RESOLV_FAIL(for_proxy);
}

ایستا bool can_resolve_dns_queries(ساختار Curl_easy *data,
                                    uint8_t dns_queries)
{
  (تهی)data;
  اگر((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    بازگشت FALSE;
  بازگشت TRUE;
}

CURLcode Curl_resolv_announce_start(ساختار Curl_easy *data,
                                    تهی *resolver)
{
  اگر(data->set.resolver_start) {
    ساختار Curl_mapi_guard guard;
    صحیح rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, پوچ,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    اگر(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      بازگشت CURLE_ABORTED_BY_CALLBACK;
    }
  }
  بازگشت CURLE_OK;
}

#ifdef USE_CURL_ASYNC

ایستا ساختار Curl_resolv_async *hostip_async_new(ساختار Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  ساختار Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  ساختار Curl_resolv_async *async;

  اگر(!data->multi) {
    DEBUGASSERT(0);
    بازگشت پوچ;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, اندازه(*async));
  اگر(!async)
    بازگشت پوچ;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  اگر(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  وگرنه
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  اگر(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  بازگشت async;
}

ایستا CURLcode hostip_resolv_take_result(ساختار Curl_easy *data,
                                          ساختار Curl_resolv_async *async,
                                          ساختار Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  اگر(!async)
    بازگشت CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  اگر(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  وگرنه
#endif
  result = Curl_async_take_result(data, async, pdns);

  اگر(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  وگرنه اگر(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, پوچ);
  }
  وگرنه اگر(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحیح)result, async->peer->hostname, async->peer->port);
  }
  وگرنه {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  بازگشت result;
}

timediff_t Curl_resolv_elapsed_ms(ساختار Curl_easy *data,
                                  uint32_t resolv_id,
                                  ثابت ساختار curltime *pnow)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  اگر(!async)
    بازگشت CURL_TIMEOUT_RESOLVE_MS;
  بازگشت curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(ساختار Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  اگر(!async || async->done)
    بازگشت TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  اگر((check_queries & async->dns_responses) != check_queries) {
    بازگشت FALSE;
  }
  بازگشت TRUE;
}

ثابت ساختار Curl_addrinfo *Curl_resolv_get_ai(ساختار Curl_easy *data,
                                               uint32_t resolv_id,
                                               صحیح ai_family,
                                               unsigned صحیح index)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  اگر(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    بازگشت پوچ;
  انتخاب(ai_family) {
  حالت AF_INET:
    بازگشت Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  حالت AF_INET6:
    بازگشت Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  پیشفرض:
    بازگشت پوچ;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(ساختار Curl_easy *data,
                           ساختار Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ساختار Curl_dns_entry **pdns)
{
  بازگشت Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت ساختار Curl_https_rrinfo *
Curl_resolv_get_https(ساختار Curl_easy *data, uint32_t resolv_id)
{
  ساختار Curl_resolv_async *async;
  اگر(!Curl_resolv_knows_https(data, resolv_id))
    بازگشت پوچ;
  async = Curl_async_get(data, resolv_id);
  بازگشت async ? async->httpsrr : پوچ;
}

bool Curl_resolv_knows_https(ساختار Curl_easy *data, uint32_t resolv_id)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  اگر(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    بازگشت ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  بازگشت TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ایستا CURLcode hostip_resolv_start(ساختار Curl_easy *data,
                                    uint8_t dns_queries,
                                    ساختار Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    ساختار Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  ساختار Curl_resolv_async *async = پوچ;
#endif
  ساختار Curl_addrinfo *addr = پوچ;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (تهی)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = پوچ;

  /* Check for "known" things to resolve ourselves. */
  اگر(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    اگر(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, پوچ);
      اگر(result)
        برو out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      برو out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    اگر(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, پوچ);
      اگر(result)
        برو out;
      addr = get_localhost(peer->port, peer->hostname);
      اگر(!addr)
        result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }
#ifndef CURL_DISABLE_DOH
  اگر(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, پوچ);
    اگر(result)
      برو out;
    اگر(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      اگر(!async) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
    result = Curl_doh(data, async);
    برو out;
  }
#وگرنه
  (تهی)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  اگر(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    برو out;
  }

#ifdef CURLRES_ASYNCH
  (تهی)addr;
  اگر(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    اگر(!async) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  اگر(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    اگر(r2)
      result = r2;
    وگرنه اگر(*pdns)
      result = CURLE_OK;
  }
#وگرنه
  result = Curl_resolv_announce_start(data, پوچ);
  اگر(result)
    برو out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  اگر(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  اگر(!result) {
    اگر(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      اگر(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    وگرنه اگر(!*pdns)
      result = CURLE_AGAIN;
  }
  وگرنه اگر(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  وگرنه اگر(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  اگر(async) {
    اگر(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    وگرنه {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  بازگشت result;
}

ایستا CURLcode hostip_resolv(ساختار Curl_easy *data,
                              uint8_t dns_queries,
                              ساختار Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              ساختار Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (تهی)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = پوچ;

#ifdef CURL_DISABLE_DOH
  (تهی)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  اگر(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    برو out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  اگر((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    برو out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  اگر(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  وگرنه اگر(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  وگرنه {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحیح)result);
  }

out:
  اگر(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    اگر(IS_RESOLV_FAIL(result)) {
      اگر(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      اگر(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    وگرنه {
      failf(data, "Error %d resolving %s:%u",
            (صحیح)result, peer->hostname, peer->port);
    }
  }
  وگرنه اگر(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    اگر(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  بازگشت result;
}

CURLcode Curl_resolv_blocking(ساختار Curl_easy *data,
                              uint8_t dns_queries,
                              ثابت نویسه *hostname,
                              uint16_t port,
                              uint8_t transport,
                              ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_peer *peer = پوچ;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = پوچ;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  اگر(result)
    برو out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  انتخاب(result) {
  حالت CURLE_OK:
    DEBUGASSERT(*pdns);
    بشکن;
#ifdef USE_CURL_ASYNC
  حالت CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    بشکن;
#endif
  پیشفرض:
    بشکن;
  }

out:
  Curl_peer_unlink(&peer);
  بازگشت result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ایستا تهی alarmfunc(صحیح sig)
{
  (تهی)sig;
  siglongjmp(curl_jmpenv, 1);
}

ایستا CURLcode resolv_alarm_timeout(ساختار Curl_easy *data,
                                     uint8_t dns_queries,
                                     ساختار Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     ساختار Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  ساختار sigaction keep_sigact; /* store the old struct here */
  فرار bool keep_copysig = FALSE; /* whether old sigact has been saved */
  ساختار sigaction sigact;
#وگرنه
#ifdef HAVE_SIGNAL
  تهی (*keep_sigact)(صحیح);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  فرار long timeout;
  فرار unsigned صحیح prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = پوچ;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  اگر(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  اگر(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    برو clean_up;
  }
  وگرنه {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, پوچ, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, پوچ);
#وگرنه /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = سیگنال(SIGALRM, alarmfunc);
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
  اگر(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  اگر(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, پوچ); /* put it back */
  }
#وگرنه
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  سیگنال(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  اگر(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    اگر(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    وگرنه
      alarm((unsigned صحیح)alarm_set);
  }

  بازگشت result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
ایستا CURLcode resolv_unix(ساختار Curl_easy *data,
                            ساختار Curl_peer *peer,
                            ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = پوچ;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  اگر(result) {
    اگر(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    بازگشت result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  بازگشت *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(ساختار Curl_easy *data,
                     ساختار Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ساختار Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = پوچ;

  اگر(timeout_ms < 0)
    /* got an already expired timeout */
    بازگشت CURLE_OPERATION_TIMEDOUT;
  وگرنه اگر(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  اگر((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    بازگشت resolv_unix(data, peer, pdns);
#وگرنه
  اگر(peer->unix_socket)
    بازگشت hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  اگر(dns_queries & CURL_DNSQ_ADDR) {
    اگر(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    اگر(timeout_ms && !Curl_doh_wanted(data)) {
      بازگشت resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  اگر(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  بازگشت hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

ساختار Curl_resolv_async *Curl_async_get(ساختار Curl_easy *data,
                                         uint32_t resolv_id)
{
  ساختار Curl_resolv_async *async = data->state.async;
  برای(; async; async = async->next) {
    اگر(async->id == resolv_id)
      بازگشت async;
  }
  بازگشت پوچ;
}

CURLcode Curl_resolv_take_result(ساختار Curl_easy *data, uint32_t resolv_id,
                                 ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  اگر(!async)
    بازگشت CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  اگر(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    بازگشت CURLE_OK;
  }
  وگرنه اگر(result) {
    Curl_async_shutdown(data, async);
    بازگشت Curl_async_failed(data, async, پوچ);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  اگر(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    اگر(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  وگرنه اگر(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    اگر(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    اگر(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  وگرنه اگر(result) {
    failf(data, "Error %d resolving %s:%u",
          (صحیح)result, async->peer->hostname, async->peer->port);
  }
  بازگشت result;
}

CURLcode Curl_resolv_pollset(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps)
{
  ساختار Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (تهی)ps;
  برای(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    اگر(async->doh) /* DoH has nothing for the pollset */
      ادامه;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  بازگشت result;
}

تهی Curl_resolv_destroy(ساختار Curl_easy *data, uint32_t resolv_id)
{
  ساختار Curl_resolv_async **panchor = &data->state.async;

  برای(; *panchor; panchor = &(*panchor)->next) {
    ساختار Curl_resolv_async *async = *panchor;
    اگر(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      بشکن;
    }
  }
}

تهی Curl_resolv_shutdown_all(ساختار Curl_easy *data)
{
  ساختار Curl_resolv_async *async = data->state.async;
  برای(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

تهی Curl_resolv_destroy_all(ساختار Curl_easy *data)
{
  وقتی(data->state.async) {
    ساختار Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
