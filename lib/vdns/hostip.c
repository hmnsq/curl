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

#ಅನ್ತರ್ಭಾವ <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "easy_lock.h"
#ಅನ್ತರ್ಭಾವ "vdns/dnscache.h"
#ಅನ್ತರ್ಭಾವ "vdns/doh.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ಯದಿ defined(CURLRES_SYNCH) &&                   \
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
ಸ್ಥಿರ bool ipv6works(ಸಂರಚನಾ Curl_easy *data);
#ಅನ್ಯಥಾ
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

uint8_t Curl_resolv_dns_queries(ಸಂರಚನಾ Curl_easy *data, uint8_t ip_version)
{
  (ಶೂನ್ಯ)data;
  ವಿಕಲ್ಪ(ip_version) {
  ಪಕ್ಷ CURL_IPRESOLVE_V6:
    ಪ್ರತ್ಯಾಗಮ CURL_DNSQ_AAAA;
  ಪಕ್ಷ CURL_IPRESOLVE_V4:
    ಪ್ರತ್ಯಾಗಮ CURL_DNSQ_A;
  ಸ್ವಭಾವ:
    ಯದಿ(ipv6works(data))
      ಪ್ರತ್ಯಾಗಮ (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
ಧ್ರುವ ವರ್ಣ *Curl_resolv_query_str(uint8_t dns_queries)
{
  ವಿಕಲ್ಪ(dns_queries) {
  ಪಕ್ಷ (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ಪ್ರತ್ಯಾಗಮ "A+AAAA+HTTPS";
  ಪಕ್ಷ (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ಪ್ರತ್ಯಾಗಮ "A+AAAA";
  ಪಕ್ಷ (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ಪ್ರತ್ಯಾಗಮ "AAAA+HTTPS";
  ಪಕ್ಷ (CURL_DNSQ_AAAA):
    ಪ್ರತ್ಯಾಗಮ "AAAA";
  ಪಕ್ಷ (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ಪ್ರತ್ಯಾಗಮ "A+HTTPS";
  ಪಕ್ಷ (CURL_DNSQ_A):
    ಪ್ರತ್ಯಾಗಮ "A";
  ಪಕ್ಷ (CURL_DNSQ_HTTPS):
    ಪ್ರತ್ಯಾಗಮ "HTTPS";
  ಪಕ್ಷ 0:
    ಪ್ರತ್ಯಾಗಮ "-";
  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ "???";
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
ಶೂನ್ಯ Curl_printable_address(ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai, ವರ್ಣ *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  ವಿಕಲ್ಪ(ai->ai_family) {
  ಪಕ್ಷ AF_INET: {
    ಧ್ರುವ ಸಂರಚನಾ sockaddr_in *sa4 = (ಧ್ರುವ ಶೂನ್ಯ *)ai->ai_addr;
    ಧ್ರುವ ಸಂರಚನಾ in_addr *ipaddr4 = &sa4->sin_addr;
    (ಶೂನ್ಯ)curlx_inet_ntop(ai->ai_family, (ಧ್ರುವ ಶೂನ್ಯ *)ipaddr4, buf, bufsize);
    ಭಙ್ಗ;
  }
#ifdef USE_IPV6
  ಪಕ್ಷ AF_INET6: {
    ಧ್ರುವ ಸಂರಚನಾ sockaddr_in6 *sa6 = (ಧ್ರುವ ಶೂನ್ಯ *)ai->ai_addr;
    ಧ್ರುವ ಸಂರಚನಾ in6_addr *ipaddr6 = &sa6->sin6_addr;
    (ಶೂನ್ಯ)curlx_inet_ntop(ai->ai_family, (ಧ್ರುವ ಶೂನ್ಯ *)ipaddr6, buf, bufsize);
    ಭಙ್ಗ;
  }
#endif
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ಸ್ಥಿರ sigjmp_buf curl_jmpenv;
ಸ್ಥಿರ curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
ಸ್ಥಿರ ಸಂರಚನಾ Curl_addrinfo *get_localhost6(uint16_t port, ಧ್ರುವ ವರ್ಣ *name)
{
  ಸಂರಚನಾ Curl_addrinfo *ca;
  ಧ್ರುವ size_t ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
  ಧ್ರುವ size_t hostlen = strlen(name);
  ಸಂರಚನಾ sockaddr_in6 sa6;
  unsigned ವರ್ಣ ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size + hostlen + 1);
  ಯದಿ(!ca)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  memset(&sa6, 0, ಪರಿಮಾಣ(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (ಶೂನ್ಯ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, ಪರಿಮಾಣ(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = ರಿಕ್ತ;
  ca->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (ವರ್ಣ *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ಪ್ರತ್ಯಾಗಮ ca;
}
#ಅನ್ಯಥಾ
#define get_localhost6(x, y) ರಿಕ್ತ
#endif

/* return a static IPv4 127.0.0.1 for the given name */
ಸ್ಥಿರ ಸಂರಚನಾ Curl_addrinfo *get_localhost(uint16_t port, ಧ್ರುವ ವರ್ಣ *name)
{
  ಸಂರಚನಾ Curl_addrinfo *ca;
  ಸಂರಚನಾ Curl_addrinfo *ca6;
  ಧ್ರುವ size_t ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
  ಧ್ರುವ size_t hostlen = strlen(name);
  ಸಂರಚನಾ sockaddr_in sa;
  unsigned ಪೂರ್ಣಾಙ್ಕ ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, ಪರಿಮಾಣ(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  ಯದಿ(curlx_inet_pton(AF_INET, "127.0.0.1", (ವರ್ಣ *)&ipv4) < 1)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  memcpy(&sa.sin_addr, &ipv4, ಪರಿಮಾಣ(ipv4));

  ca = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size + hostlen + 1);
  ಯದಿ(!ca)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (ವರ್ಣ *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  ಯದಿ(!ca6)
    ಪ್ರತ್ಯಾಗಮ ca;
  ca6->ai_next = ca;
  ಪ್ರತ್ಯಾಗಮ ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(ಸಂರಚನಾ Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  ಯದಿ(s == CURL_SOCKET_BAD) {
    ಯದಿ(SOCKERRNO == SOCKENOMEM)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಅನ್ಯಥಾ {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ಸ್ಥಿರ bool ipv6works(ಸಂರಚನಾ Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  ಪ್ರತ್ಯಾಗಮ data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ಧ್ರುವ ವರ್ಣ *hostname)
{
  ಸಂರಚನಾ in_addr in;
#ifdef USE_IPV6
  ಸಂರಚನಾ in6_addr in6;
#endif
  ಯದಿ(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ಸ್ಥಿರ bool tailmatch(ಧ್ರುವ ವರ್ಣ *full, size_t flen,
                      ಧ್ರುವ ವರ್ಣ *part, size_t plen)
{
  ಯದಿ(plen > flen)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ curl_strnequal(part, &full[flen - plen], plen);
}

ಸ್ಥಿರ CURLcode hostip_resolv_failed(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ಪ್ರತ್ಯಾಗಮ RESOLV_FAIL(for_proxy);
}

ಸ್ಥಿರ bool can_resolve_dns_queries(ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t dns_queries)
{
  (ಶೂನ್ಯ)data;
  ಯದಿ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

CURLcode Curl_resolv_announce_start(ಸಂರಚನಾ Curl_easy *data,
                                    ಶೂನ್ಯ *resolver)
{
  ಯದಿ(data->set.resolver_start) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಙ್ಕ rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, ರಿಕ್ತ,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    ಯದಿ(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef USE_CURL_ASYNC

ಸ್ಥಿರ ಸಂರಚನಾ Curl_resolv_async *hostip_async_new(ಸಂರಚನಾ Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  ಸಂರಚನಾ Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  ಸಂರಚನಾ Curl_resolv_async *async;

  ಯದಿ(!data->multi) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, ಪರಿಮಾಣ(*async));
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  ಯದಿ(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  ಅನ್ಯಥಾ
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  ಯದಿ(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ಪ್ರತ್ಯಾಗಮ async;
}

ಸ್ಥಿರ CURLcode hostip_resolv_take_result(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_resolv_async *async,
                                          ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  ಯದಿ(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  ಅನ್ಯಥಾ
#endif
  result = Curl_async_take_result(data, async, pdns);

  ಯದಿ(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, ರಿಕ್ತ);
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (ಪೂರ್ಣಾಙ್ಕ)result, async->peer->hostname, async->peer->port);
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

timediff_t Curl_resolv_elapsed_ms(ಸಂರಚನಾ Curl_easy *data,
                                  uint32_t resolv_id,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ CURL_TIMEOUT_RESOLVE_MS;
  ಪ್ರತ್ಯಾಗಮ curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(ಸಂರಚನಾ Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  ಯದಿ(!async || async->done)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  ಯದಿ((check_queries & async->dns_responses) != check_queries) {
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *Curl_resolv_get_ai(ಸಂರಚನಾ Curl_easy *data,
                                               uint32_t resolv_id,
                                               ಪೂರ್ಣಾಙ್ಕ ai_family,
                                               unsigned ಪೂರ್ಣಾಙ್ಕ index)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ಯದಿ(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ವಿಕಲ್ಪ(ai_family) {
  ಪಕ್ಷ AF_INET:
    ಪ್ರತ್ಯಾಗಮ Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  ಪಕ್ಷ AF_INET6:
    ಪ್ರತ್ಯಾಗಮ Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಪ್ರತ್ಯಾಗಮ Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *
Curl_resolv_get_https(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id)
{
  ಸಂರಚನಾ Curl_resolv_async *async;
  ಯದಿ(!Curl_resolv_knows_https(data, resolv_id))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  async = Curl_async_get(data, resolv_id);
  ಪ್ರತ್ಯಾಗಮ async ? async->httpsrr : ರಿಕ್ತ;
}

bool Curl_resolv_knows_https(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ಯದಿ(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ಪ್ರತ್ಯಾಗಮ ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ಪ್ರತ್ಯಾಗಮ TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ಸ್ಥಿರ CURLcode hostip_resolv_start(ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t dns_queries,
                                    ಸಂರಚನಾ Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    ಸಂರಚನಾ Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  ಸಂರಚನಾ Curl_resolv_async *async = ರಿಕ್ತ;
#endif
  ಸಂರಚನಾ Curl_addrinfo *addr = ರಿಕ್ತ;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (ಶೂನ್ಯ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ರಿಕ್ತ;

  /* Check for "known" things to resolve ourselves. */
  ಯದಿ(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    ಯದಿ(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, ರಿಕ್ತ);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      ಗಚ್ಛ out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    ಯದಿ(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, ರಿಕ್ತ);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      addr = get_localhost(peer->port, peer->hostname);
      ಯದಿ(!addr)
        result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
#ifndef CURL_DISABLE_DOH
  ಯದಿ(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, ರಿಕ್ತ);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಯದಿ(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      ಯದಿ(!async) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
    result = Curl_doh(data, async);
    ಗಚ್ಛ out;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  ಯದಿ(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    ಗಚ್ಛ out;
  }

#ifdef CURLRES_ASYNCH
  (ಶೂನ್ಯ)addr;
  ಯದಿ(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    ಯದಿ(!async) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  ಯದಿ(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    ಯದಿ(r2)
      result = r2;
    ಅನ್ಯಥಾ ಯದಿ(*pdns)
      result = CURLE_OK;
  }
#ಅನ್ಯಥಾ
  result = Curl_resolv_announce_start(data, ರಿಕ್ತ);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  ಯದಿ(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  ಯದಿ(!result) {
    ಯದಿ(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      ಯದಿ(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ ಯದಿ(!*pdns)
      result = CURLE_AGAIN;
  }
  ಅನ್ಯಥಾ ಯದಿ(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  ಅನ್ಯಥಾ ಯದಿ(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  ಯದಿ(async) {
    ಯದಿ(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    ಅನ್ಯಥಾ {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode hostip_resolv(ಸಂರಚನಾ Curl_easy *data,
                              uint8_t dns_queries,
                              ಸಂರಚನಾ Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (ಶೂನ್ಯ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ರಿಕ್ತ;

#ifdef CURL_DISABLE_DOH
  (ಶೂನ್ಯ)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  ಯದಿ(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    ಗಚ್ಛ out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  ಯದಿ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    ಗಚ್ಛ out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  ಯದಿ(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  ಅನ್ಯಥಾ {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (ಪೂರ್ಣಾಙ್ಕ)result);
  }

out:
  ಯದಿ(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    ಯದಿ(IS_RESOLV_FAIL(result)) {
      ಯದಿ(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      ಯದಿ(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    ಅನ್ಯಥಾ {
      failf(data, "Error %d resolving %s:%u",
            (ಪೂರ್ಣಾಙ್ಕ)result, peer->hostname, peer->port);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    ಯದಿ(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_resolv_blocking(ಸಂರಚನಾ Curl_easy *data,
                              uint8_t dns_queries,
                              ಧ್ರುವ ವರ್ಣ *hostname,
                              uint16_t port,
                              uint8_t transport,
                              ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_peer *peer = ರಿಕ್ತ;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ರಿಕ್ತ;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  ವಿಕಲ್ಪ(result) {
  ಪಕ್ಷ CURLE_OK:
    DEBUGASSERT(*pdns);
    ಭಙ್ಗ;
#ifdef USE_CURL_ASYNC
  ಪಕ್ಷ CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

out:
  Curl_peer_unlink(&peer);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ಸ್ಥಿರ ಶೂನ್ಯ alarmfunc(ಪೂರ್ಣಾಙ್ಕ sig)
{
  (ಶೂನ್ಯ)sig;
  siglongjmp(curl_jmpenv, 1);
}

ಸ್ಥಿರ CURLcode resolv_alarm_timeout(ಸಂರಚನಾ Curl_easy *data,
                                     uint8_t dns_queries,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     ಸಂರಚನಾ Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  ಸಂರಚನಾ sigaction keep_sigact; /* store the old struct here */
  ಚಞ್ಚಲ bool keep_copysig = FALSE; /* whether old sigact has been saved */
  ಸಂರಚನಾ sigaction sigact;
#ಅನ್ಯಥಾ
#ifdef HAVE_SIGNAL
  ಶೂನ್ಯ (*keep_sigact)(ಪೂರ್ಣಾಙ್ಕ);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  ಚಞ್ಚಲ long timeout;
  ಚಞ್ಚಲ unsigned ಪೂರ್ಣಾಙ್ಕ prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = ರಿಕ್ತ;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  ಯದಿ(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  ಯದಿ(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    ಗಚ್ಛ clean_up;
  }
  ಅನ್ಯಥಾ {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, ರಿಕ್ತ, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ರಿಕ್ತ);
#ಅನ್ಯಥಾ /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = ಸಙ್ಕೇತ(SIGALRM, alarmfunc);
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
  ಯದಿ(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  ಯದಿ(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ರಿಕ್ತ); /* put it back */
  }
#ಅನ್ಯಥಾ
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  ಸಙ್ಕೇತ(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  ಯದಿ(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    ಯದಿ(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    ಅನ್ಯಥಾ
      alarm((unsigned ಪೂರ್ಣಾಙ್ಕ)alarm_set);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
ಸ್ಥಿರ CURLcode resolv_unix(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_peer *peer,
                            ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ರಿಕ್ತ;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  ಪ್ರತ್ಯಾಗಮ *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(ಸಂರಚನಾ Curl_easy *data,
                     ಸಂರಚನಾ Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = ರಿಕ್ತ;

  ಯದಿ(timeout_ms < 0)
    /* got an already expired timeout */
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  ಅನ್ಯಥಾ ಯದಿ(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  ಯದಿ((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ಪ್ರತ್ಯಾಗಮ resolv_unix(data, peer, pdns);
#ಅನ್ಯಥಾ
  ಯದಿ(peer->unix_socket)
    ಪ್ರತ್ಯಾಗಮ hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  ಯದಿ(dns_queries & CURL_DNSQ_ADDR) {
    ಯದಿ(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    ಯದಿ(timeout_ms && !Curl_doh_wanted(data)) {
      ಪ್ರತ್ಯಾಗಮ resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  ಯದಿ(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  ಪ್ರತ್ಯಾಗಮ hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

ಸಂರಚನಾ Curl_resolv_async *Curl_async_get(ಸಂರಚನಾ Curl_easy *data,
                                         uint32_t resolv_id)
{
  ಸಂರಚನಾ Curl_resolv_async *async = data->state.async;
  ಪ್ರತಿ(; async; async = async->next) {
    ಯದಿ(async->id == resolv_id)
      ಪ್ರತ್ಯಾಗಮ async;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURLcode Curl_resolv_take_result(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id,
                                 ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  ಯದಿ(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    Curl_async_shutdown(data, async);
    ಪ್ರತ್ಯಾಗಮ Curl_async_failed(data, async, ರಿಕ್ತ);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  ಯದಿ(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    ಯದಿ(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  ಅನ್ಯಥಾ ಯದಿ(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    ಯದಿ(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    ಯದಿ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    failf(data, "Error %d resolving %s:%u",
          (ಪೂರ್ಣಾಙ್ಕ)result, async->peer->hostname, async->peer->port);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_resolv_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)ps;
  ಪ್ರತಿ(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    ಯದಿ(async->doh) /* DoH has nothing for the pollset */
      ಅನುವರ್ತ;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_resolv_destroy(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id)
{
  ಸಂರಚನಾ Curl_resolv_async **panchor = &data->state.async;

  ಪ್ರತಿ(; *panchor; panchor = &(*panchor)->next) {
    ಸಂರಚನಾ Curl_resolv_async *async = *panchor;
    ಯದಿ(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      ಭಙ್ಗ;
    }
  }
}

ಶೂನ್ಯ Curl_resolv_shutdown_all(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_resolv_async *async = data->state.async;
  ಪ್ರತಿ(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

ಶೂನ್ಯ Curl_resolv_destroy_all(ಸಂರಚನಾ Curl_easy *data)
{
  ಯಾವತ್(data->state.async) {
    ಸಂರಚನಾ Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
