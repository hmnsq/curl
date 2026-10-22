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

#अन्तर्भाव <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#अन्तर्भाव <signal.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "easy_lock.h"
#अन्तर्भाव "vdns/dnscache.h"
#अन्तर्भाव "vdns/doh.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/strparse.h"

#जर defined(CURLRES_SYNCH) &&                   \
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
स्थिर bool ipv6works(संरचना Curl_easy *data);
#अन्यथा
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

uint8_t Curl_resolv_dns_queries(संरचना Curl_easy *data, uint8_t ip_version)
{
  (शून्य)data;
  निवड(ip_version) {
  स्थिती CURL_IPRESOLVE_V6:
    परत CURL_DNSQ_AAAA;
  स्थिती CURL_IPRESOLVE_V4:
    परत CURL_DNSQ_A;
  पूर्वनिर्धारित:
    जर(ipv6works(data))
      परत (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    अन्यथा
      परत CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
स्थिरांक अक्षर *Curl_resolv_query_str(uint8_t dns_queries)
{
  निवड(dns_queries) {
  स्थिती (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    परत "A+AAAA+HTTPS";
  स्थिती (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    परत "A+AAAA";
  स्थिती (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    परत "AAAA+HTTPS";
  स्थिती (CURL_DNSQ_AAAA):
    परत "AAAA";
  स्थिती (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    परत "A+HTTPS";
  स्थिती (CURL_DNSQ_A):
    परत "A";
  स्थिती (CURL_DNSQ_HTTPS):
    परत "HTTPS";
  स्थिती 0:
    परत "-";
  पूर्वनिर्धारित:
    DEBUGASSERT(0);
    परत "???";
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
शून्य Curl_printable_address(स्थिरांक संरचना Curl_addrinfo *ai, अक्षर *buf,
                            माप_प्रकार bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  निवड(ai->ai_family) {
  स्थिती AF_INET: {
    स्थिरांक संरचना sockaddr_in *sa4 = (स्थिरांक शून्य *)ai->ai_addr;
    स्थिरांक संरचना in_addr *ipaddr4 = &sa4->sin_addr;
    (शून्य)curlx_inet_ntop(ai->ai_family, (स्थिरांक शून्य *)ipaddr4, buf, bufsize);
    थांबा;
  }
#ifdef USE_IPV6
  स्थिती AF_INET6: {
    स्थिरांक संरचना sockaddr_in6 *sa6 = (स्थिरांक शून्य *)ai->ai_addr;
    स्थिरांक संरचना in6_addr *ipaddr6 = &sa6->sin6_addr;
    (शून्य)curlx_inet_ntop(ai->ai_family, (स्थिरांक शून्य *)ipaddr6, buf, bufsize);
    थांबा;
  }
#endif
  पूर्वनिर्धारित:
    थांबा;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
स्थिर sigjmp_buf curl_jmpenv;
स्थिर curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
स्थिर संरचना Curl_addrinfo *get_localhost6(uint16_t port, स्थिरांक अक्षर *name)
{
  संरचना Curl_addrinfo *ca;
  स्थिरांक माप_प्रकार ss_size = आकार(संरचना sockaddr_in6);
  स्थिरांक माप_प्रकार hostlen = म_माप(name);
  संरचना sockaddr_in6 sa6;
  अचिन्हित अक्षर ipv6[16];
  अचिन्हित लघु port16 = (अचिन्हित लघु)(port & 0xffff);
  ca = curlx_calloc(1, आकार(संरचना Curl_addrinfo) + ss_size + hostlen + 1);
  जर(!ca)
    परत रिक्त;

  स_रखो(&sa6, 0, आकार(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (शून्य)curlx_inet_pton(AF_INET6, "::1", ipv6);
  स_नकल(&sa6.sin6_addr, ipv6, आकार(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = रिक्त;
  ca->ai_addr = (शून्य *)((अक्षर *)ca + आकार(संरचना Curl_addrinfo));
  स_नकल(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (अक्षर *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  परत ca;
}
#अन्यथा
#define get_localhost6(x, y) रिक्त
#endif

/* return a static IPv4 127.0.0.1 for the given name */
स्थिर संरचना Curl_addrinfo *get_localhost(uint16_t port, स्थिरांक अक्षर *name)
{
  संरचना Curl_addrinfo *ca;
  संरचना Curl_addrinfo *ca6;
  स्थिरांक माप_प्रकार ss_size = आकार(संरचना sockaddr_in);
  स्थिरांक माप_प्रकार hostlen = म_माप(name);
  संरचना sockaddr_in sa;
  अचिन्हित पूर्णांक ipv4;
  अचिन्हित लघु port16 = (अचिन्हित लघु)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  स_रखो(&sa, 0, आकार(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  जर(curlx_inet_pton(AF_INET, "127.0.0.1", (अक्षर *)&ipv4) < 1)
    परत रिक्त;
  स_नकल(&sa.sin_addr, &ipv4, आकार(ipv4));

  ca = curlx_calloc(1, आकार(संरचना Curl_addrinfo) + ss_size + hostlen + 1);
  जर(!ca)
    परत रिक्त;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (शून्य *)((अक्षर *)ca + आकार(संरचना Curl_addrinfo));
  स_नकल(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (अक्षर *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  जर(!ca6)
    परत ca;
  ca6->ai_next = ca;
  परत ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(संरचना Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  जर(s == CURL_SOCKET_BAD) {
    जर(SOCKERRNO == SOCKENOMEM)
      परत CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  परत CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
स्थिर bool ipv6works(संरचना Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  परत data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(स्थिरांक अक्षर *hostname)
{
  संरचना in_addr in;
#ifdef USE_IPV6
  संरचना in6_addr in6;
#endif
  जर(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    परत TRUE;
  परत FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
स्थिर bool tailmatch(स्थिरांक अक्षर *full, माप_प्रकार flen,
                      स्थिरांक अक्षर *part, माप_प्रकार plen)
{
  जर(plen > flen)
    परत FALSE;
  परत curl_strnequal(part, &full[flen - plen], plen);
}

स्थिर CURLcode hostip_resolv_failed(संरचना Curl_easy *data,
                                     संरचना Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  परत RESOLV_FAIL(for_proxy);
}

स्थिर bool can_resolve_dns_queries(संरचना Curl_easy *data,
                                    uint8_t dns_queries)
{
  (शून्य)data;
  जर((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    परत FALSE;
  परत TRUE;
}

CURLcode Curl_resolv_announce_start(संरचना Curl_easy *data,
                                    शून्य *resolver)
{
  जर(data->set.resolver_start) {
    संरचना Curl_mapi_guard guard;
    पूर्णांक rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, रिक्त,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    जर(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      परत CURLE_ABORTED_BY_CALLBACK;
    }
  }
  परत CURLE_OK;
}

#ifdef USE_CURL_ASYNC

स्थिर संरचना Curl_resolv_async *hostip_async_new(संरचना Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  संरचना Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  संरचना Curl_resolv_async *async;

  जर(!data->multi) {
    DEBUGASSERT(0);
    परत रिक्त;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, आकार(*async));
  जर(!async)
    परत रिक्त;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  जर(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  अन्यथा
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  जर(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  परत async;
}

स्थिर CURLcode hostip_resolv_take_result(संरचना Curl_easy *data,
                                          संरचना Curl_resolv_async *async,
                                          संरचना Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  जर(!async)
    परत CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  जर(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  अन्यथा
#endif
  result = Curl_async_take_result(data, async, pdns);

  जर(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  अन्यथा जर(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, रिक्त);
  }
  अन्यथा जर(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (पूर्णांक)result, async->peer->hostname, async->peer->port);
  }
  अन्यथा {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  परत result;
}

timediff_t Curl_resolv_elapsed_ms(संरचना Curl_easy *data,
                                  uint32_t resolv_id,
                                  स्थिरांक संरचना curltime *pnow)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  जर(!async)
    परत CURL_TIMEOUT_RESOLVE_MS;
  परत curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(संरचना Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  जर(!async || async->done)
    परत TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  जर((check_queries & async->dns_responses) != check_queries) {
    परत FALSE;
  }
  परत TRUE;
}

स्थिरांक संरचना Curl_addrinfo *Curl_resolv_get_ai(संरचना Curl_easy *data,
                                               uint32_t resolv_id,
                                               पूर्णांक ai_family,
                                               अचिन्हित पूर्णांक index)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  जर(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    परत रिक्त;
  निवड(ai_family) {
  स्थिती AF_INET:
    परत Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  स्थिती AF_INET6:
    परत Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  पूर्वनिर्धारित:
    परत रिक्त;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(संरचना Curl_easy *data,
                           संरचना Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           संरचना Curl_dns_entry **pdns)
{
  परत Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

स्थिरांक संरचना Curl_https_rrinfo *
Curl_resolv_get_https(संरचना Curl_easy *data, uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async;
  जर(!Curl_resolv_knows_https(data, resolv_id))
    परत रिक्त;
  async = Curl_async_get(data, resolv_id);
  परत async ? async->httpsrr : रिक्त;
}

bool Curl_resolv_knows_https(संरचना Curl_easy *data, uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  जर(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    परत ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  परत TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
स्थिर CURLcode hostip_resolv_start(संरचना Curl_easy *data,
                                    uint8_t dns_queries,
                                    संरचना Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    संरचना Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  संरचना Curl_resolv_async *async = रिक्त;
#endif
  संरचना Curl_addrinfo *addr = रिक्त;
  माप_प्रकार hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (शून्य)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = रिक्त;

  /* Check for "known" things to resolve ourselves. */
  जर(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    जर(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, रिक्त);
      जर(result)
        जा out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      जा out;
    }
#endif

    hostname_len = म_माप(peer->hostname);
    जर(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, रिक्त);
      जर(result)
        जा out;
      addr = get_localhost(peer->port, peer->hostname);
      जर(!addr)
        result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
  }
#ifndef CURL_DISABLE_DOH
  जर(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, रिक्त);
    जर(result)
      जा out;
    जर(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      जर(!async) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
    result = Curl_doh(data, async);
    जा out;
  }
#अन्यथा
  (शून्य)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  जर(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    जा out;
  }

#ifdef CURLRES_ASYNCH
  (शून्य)addr;
  जर(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    जर(!async) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  जर(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    जर(r2)
      result = r2;
    अन्यथा जर(*pdns)
      result = CURLE_OK;
  }
#अन्यथा
  result = Curl_resolv_announce_start(data, रिक्त);
  जर(result)
    जा out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  जर(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  जर(!result) {
    जर(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      जर(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    अन्यथा जर(!*pdns)
      result = CURLE_AGAIN;
  }
  अन्यथा जर(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  अन्यथा जर(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  जर(async) {
    जर(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    अन्यथा {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  परत result;
}

स्थिर CURLcode hostip_resolv(संरचना Curl_easy *data,
                              uint8_t dns_queries,
                              संरचना Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              संरचना Curl_dns_entry **pdns)
{
  माप_प्रकार hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (शून्य)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = रिक्त;

#ifdef CURL_DISABLE_DOH
  (शून्य)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = म_माप(peer->hostname);
  DEBUGASSERT(hostname_len);
  जर(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    जा out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  जर((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     दो_पर्या("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    जा out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  जर(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  अन्यथा जर(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  अन्यथा {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (पूर्णांक)result);
  }

out:
  जर(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    जर(IS_RESOLV_FAIL(result)) {
      जर(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      जर(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    अन्यथा {
      failf(data, "Error %d resolving %s:%u",
            (पूर्णांक)result, peer->hostname, peer->port);
    }
  }
  अन्यथा जर(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    जर(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  परत result;
}

CURLcode Curl_resolv_blocking(संरचना Curl_easy *data,
                              uint8_t dns_queries,
                              स्थिरांक अक्षर *hostname,
                              uint16_t port,
                              uint8_t transport,
                              संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = रिक्त;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  जर(result)
    जा out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  निवड(result) {
  स्थिती CURLE_OK:
    DEBUGASSERT(*pdns);
    थांबा;
#ifdef USE_CURL_ASYNC
  स्थिती CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    थांबा;
#endif
  पूर्वनिर्धारित:
    थांबा;
  }

out:
  Curl_peer_unlink(&peer);
  परत result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN स्थिर शून्य alarmfunc(पूर्णांक sig)
{
  (शून्य)sig;
  siglongjmp(curl_jmpenv, 1);
}

स्थिर CURLcode resolv_alarm_timeout(संरचना Curl_easy *data,
                                     uint8_t dns_queries,
                                     संरचना Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     संरचना Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  संरचना sigaction keep_sigact; /* store the old struct here */
  अस्थिर bool keep_copysig = FALSE; /* whether old sigact has been saved */
  संरचना sigaction sigact;
#अन्यथा
#ifdef HAVE_SIGNAL
  शून्य (*keep_sigact)(पूर्णांक);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  अस्थिर दीर्घ timeout;
  अस्थिर अचिन्हित पूर्णांक prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = रिक्त;
  timeout = (timeout_ms > दीर्घ_उच्च) ? दीर्घ_उच्च : (दीर्घ)timeout_ms;
  जर(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    परत CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  जर(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    जा clean_up;
  }
  अन्यथा {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, रिक्त, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, रिक्त);
#अन्यथा /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = संकेत(SIGALRM, alarmfunc);
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
  जर(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  जर(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, रिक्त); /* put it back */
  }
#अन्यथा
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  संकेत(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  जर(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    अचिन्हित दीर्घ alarm_set = (अचिन्हित दीर्घ)(prev_alarm - elapsed_secs);

    जर(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    अन्यथा
      alarm((अचिन्हित पूर्णांक)alarm_set);
  }

  परत result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
स्थिर CURLcode resolv_unix(संरचना Curl_easy *data,
                            संरचना Curl_peer *peer,
                            संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = रिक्त;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  जर(result) {
    जर(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    परत result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  परत *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(संरचना Curl_easy *data,
                     संरचना Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     संरचना Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = रिक्त;

  जर(timeout_ms < 0)
    /* got an already expired timeout */
    परत CURLE_OPERATION_TIMEDOUT;
  अन्यथा जर(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  जर((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    परत resolv_unix(data, peer, pdns);
#अन्यथा
  जर(peer->unix_socket)
    परत hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  जर(dns_queries & CURL_DNSQ_ADDR) {
    जर(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    जर(timeout_ms && !Curl_doh_wanted(data)) {
      परत resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  जर(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  परत hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

संरचना Curl_resolv_async *Curl_async_get(संरचना Curl_easy *data,
                                         uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async = data->state.async;
  साठी(; async; async = async->next) {
    जर(async->id == resolv_id)
      परत async;
  }
  परत रिक्त;
}

CURLcode Curl_resolv_take_result(संरचना Curl_easy *data, uint32_t resolv_id,
                                 संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  जर(!async)
    परत CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  जर(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    परत CURLE_OK;
  }
  अन्यथा जर(result) {
    Curl_async_shutdown(data, async);
    परत Curl_async_failed(data, async, रिक्त);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  जर(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    जर(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  अन्यथा जर(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    जर(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    जर(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  अन्यथा जर(result) {
    failf(data, "Error %d resolving %s:%u",
          (पूर्णांक)result, async->peer->hostname, async->peer->port);
  }
  परत result;
}

CURLcode Curl_resolv_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (शून्य)ps;
  साठी(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    जर(async->doh) /* DoH has nothing for the pollset */
      सुरूठेवा;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  परत result;
}

शून्य Curl_resolv_destroy(संरचना Curl_easy *data, uint32_t resolv_id)
{
  संरचना Curl_resolv_async **panchor = &data->state.async;

  साठी(; *panchor; panchor = &(*panchor)->next) {
    संरचना Curl_resolv_async *async = *panchor;
    जर(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      थांबा;
    }
  }
}

शून्य Curl_resolv_shutdown_all(संरचना Curl_easy *data)
{
  संरचना Curl_resolv_async *async = data->state.async;
  साठी(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

शून्य Curl_resolv_destroy_all(संरचना Curl_easy *data)
{
  पर्यंत(data->state.async) {
    संरचना Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
