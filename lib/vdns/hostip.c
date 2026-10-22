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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#অন্তর্ভাব <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#অন্তর্ভাব <signal.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "easy_lock.h"
#অন্তর্ভাব "vdns/dnscache.h"
#অন্তর্ভাব "vdns/doh.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strcopy.h"
#অন্তর্ভাব "curlx/strparse.h"

#যদি defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#define USE_ALARM_TIMEOUT
#endif

#define RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#define IS_RESOLV_FAIL(ফলাফল) \
  (((ফলাফল) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((ফলাফল) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#ifdef USE_IPV6
স্থির bool ipv6works(কাঠামো Curl_easy *তথ্য);
#অন্যথা
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

uint8_t Curl_resolv_dns_queries(কাঠামো Curl_easy *তথ্য, uint8_t ip_version)
{
  (শূন্য)তথ্য;
  নির্বাচন(ip_version) {
  ক্ষেত্র CURL_IPRESOLVE_V6:
    ফেরত CURL_DNSQ_AAAA;
  ক্ষেত্র CURL_IPRESOLVE_V4:
    ফেরত CURL_DNSQ_A;
  পূর্বনির্ধারিত:
    যদি(ipv6works(তথ্য))
      ফেরত (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    অন্যথা
      ফেরত CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
ধ্রুবক অক্ষর *Curl_resolv_query_str(uint8_t dns_queries)
{
  নির্বাচন(dns_queries) {
  ক্ষেত্র (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ফেরত "A+AAAA+HTTPS";
  ক্ষেত্র (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ফেরত "A+AAAA";
  ক্ষেত্র (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ফেরত "AAAA+HTTPS";
  ক্ষেত্র (CURL_DNSQ_AAAA):
    ফেরত "AAAA";
  ক্ষেত্র (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ফেরত "A+HTTPS";
  ক্ষেত্র (CURL_DNSQ_A):
    ফেরত "A";
  ক্ষেত্র (CURL_DNSQ_HTTPS):
    ফেরত "HTTPS";
  ক্ষেত্র 0:
    ফেরত "-";
  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    ফেরত "???";
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
শূন্য Curl_printable_address(ধ্রুবক কাঠামো Curl_addrinfo *ai, অক্ষর *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  নির্বাচন(ai->ai_family) {
  ক্ষেত্র AF_INET: {
    ধ্রুবক কাঠামো sockaddr_in *sa4 = (ধ্রুবক শূন্য *)ai->ai_addr;
    ধ্রুবক কাঠামো in_addr *ipaddr4 = &sa4->sin_addr;
    (শূন্য)curlx_inet_ntop(ai->ai_family, (ধ্রুবক শূন্য *)ipaddr4, buf, bufsize);
    বিরতি;
  }
#ifdef USE_IPV6
  ক্ষেত্র AF_INET6: {
    ধ্রুবক কাঠামো sockaddr_in6 *sa6 = (ধ্রুবক শূন্য *)ai->ai_addr;
    ধ্রুবক কাঠামো in6_addr *ipaddr6 = &sa6->sin6_addr;
    (শূন্য)curlx_inet_ntop(ai->ai_family, (ধ্রুবক শূন্য *)ipaddr6, buf, bufsize);
    বিরতি;
  }
#endif
  পূর্বনির্ধারিত:
    বিরতি;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
স্থির sigjmp_buf curl_jmpenv;
স্থির curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
স্থির কাঠামো Curl_addrinfo *get_localhost6(uint16_t port, ধ্রুবক অক্ষর *name)
{
  কাঠামো Curl_addrinfo *ca;
  ধ্রুবক size_t ss_আকার = আকার(কাঠামো sockaddr_in6);
  ধ্রুবক size_t hostlen = strlen(name);
  কাঠামো sockaddr_in6 sa6;
  unsigned অক্ষর ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, আকার(কাঠামো Curl_addrinfo) + ss_আকার + hostlen + 1);
  যদি(!ca)
    ফেরত ফাঁকা;

  memset(&sa6, 0, আকার(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (শূন্য)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, আকার(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_আকার;
  ca->ai_পরবর্তী      = ফাঁকা;
  ca->ai_addr = (শূন্য *)((অক্ষর *)ca + আকার(কাঠামো Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_আকার);
  ca->ai_canonname = (অক্ষর *)ca->ai_addr + ss_আকার;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ফেরত ca;
}
#অন্যথা
#define get_localhost6(x, y) ফাঁকা
#endif

/* return a static IPv4 127.0.0.1 for the given name */
স্থির কাঠামো Curl_addrinfo *get_localhost(uint16_t port, ধ্রুবক অক্ষর *name)
{
  কাঠামো Curl_addrinfo *ca;
  কাঠামো Curl_addrinfo *ca6;
  ধ্রুবক size_t ss_আকার = আকার(কাঠামো sockaddr_in);
  ধ্রুবক size_t hostlen = strlen(name);
  কাঠামো sockaddr_in sa;
  unsigned পূর্ণসংখ্যা ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, আকার(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  যদি(curlx_inet_pton(AF_INET, "127.0.0.1", (অক্ষর *)&ipv4) < 1)
    ফেরত ফাঁকা;
  memcpy(&sa.sin_addr, &ipv4, আকার(ipv4));

  ca = curlx_calloc(1, আকার(কাঠামো Curl_addrinfo) + ss_আকার + hostlen + 1);
  যদি(!ca)
    ফেরত ফাঁকা;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_আকার;
  ca->ai_addr = (শূন্য *)((অক্ষর *)ca + আকার(কাঠামো Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_আকার);
  ca->ai_canonname = (অক্ষর *)ca->ai_addr + ss_আকার;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  যদি(!ca6)
    ফেরত ca;
  ca6->ai_পরবর্তী = ca;
  ফেরত ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(কাঠামো Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  যদি(s == CURL_SOCKET_BAD) {
    যদি(SOCKERRNO == SOCKENOMEM)
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  ফেরত CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
স্থির bool ipv6works(কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->multi);
  ফেরত তথ্য ? তথ্য->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ধ্রুবক অক্ষর *hostname)
{
  কাঠামো in_addr in;
#ifdef USE_IPV6
  কাঠামো in6_addr in6;
#endif
  যদি(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    ফেরত TRUE;
  ফেরত FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
স্থির bool tailmatch(ধ্রুবক অক্ষর *full, size_t flen,
                      ধ্রুবক অক্ষর *part, size_t plen)
{
  যদি(plen > flen)
    ফেরত FALSE;
  ফেরত curl_strnequal(part, &full[flen - plen], plen);
}

স্থির CURLcode hostip_resolv_failed(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_peer *peer,
                                     bool for_proxy)
{
  failf(তথ্য, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ফেরত RESOLV_FAIL(for_proxy);
}

স্থির bool can_resolve_dns_queries(কাঠামো Curl_easy *তথ্য,
                                    uint8_t dns_queries)
{
  (শূন্য)তথ্য;
  যদি((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(তথ্য))
    ফেরত FALSE;
  ফেরত TRUE;
}

CURLcode Curl_resolv_announce_start(কাঠামো Curl_easy *তথ্য,
                                    শূন্য *resolver)
{
  যদি(তথ্য->set.resolver_start) {
    কাঠামো Curl_mapi_guard guard;
    পূর্ণসংখ্যা rc;

    CURL_TRC_DNS(তথ্য, "announcing resolve to application");
    CURL_CBAPI_START(&guard, তথ্য, easy_resolver_start);
    rc = তথ্য->set.resolver_start(resolver, ফাঁকা,
                                  তথ্য->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    যদি(rc) {
      CURL_TRC_DNS(তথ্য, "application aborted resolve");
      ফেরত CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ফেরত CURLE_OK;
}

#ifdef USE_CURL_ASYNC

স্থির কাঠামো Curl_resolv_async *hostip_async_new(কাঠামো Curl_easy *তথ্য,
                                                  uint8_t dns_queries,
                                                  কাঠামো Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  কাঠামো Curl_resolv_async *async;

  যদি(!তথ্য->multi) {
    DEBUGASSERT(0);
    ফেরত ফাঁকা;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, আকার(*async));
  যদি(!async)
    ফেরত ফাঁকা;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  যদি(তথ্য->multi->last_resolv_id == UINT32_MAX)
    তথ্য->multi->last_resolv_id = 1; /* wrap around */
  অন্যথা
    তথ্য->multi->last_resolv_id++;
  async->id = তথ্য->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(তথ্য);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  যদি(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ফেরত async;
}

স্থির CURLcode hostip_resolv_take_ফলাফল(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_resolv_async *async,
                                          কাঠামো Curl_dns_entry **pdns)
{
  CURLcode ফলাফল;

  /* If async resolving is ongoing, this must be set */
  যদি(!async)
    ফেরত CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  যদি(async->doh)
    ফলাফল = Curl_doh_take_ফলাফল(তথ্য, async, pdns);
  অন্যথা
#endif
  ফলাফল = Curl_async_take_ফলাফল(তথ্য, async, pdns);

  যদি(ফলাফল == CURLE_AGAIN) {
    CURL_TRC_DNS(তথ্য, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    ফলাফল = CURLE_OK;
  }
  অন্যথা যদি(IS_RESOLV_FAIL(ফলাফল)) {
    ফলাফল = Curl_async_failed(তথ্য, async, ফাঁকা);
  }
  অন্যথা যদি(ফলাফল) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(তথ্য, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (পূর্ণসংখ্যা)ফলাফল, async->peer->hostname, async->peer->port);
  }
  অন্যথা {
    CURL_TRC_DNS(তথ্য, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  ফেরত ফলাফল;
}

timediff_t Curl_resolv_elapsed_ms(কাঠামো Curl_easy *তথ্য,
                                  uint32_t resolv_id,
                                  ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  যদি(!async)
    ফেরত CURL_TIMEOUT_RESOLVE_MS;
  ফেরত curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(কাঠামো Curl_easy *তথ্য,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  যদি(!async || async->done)
    ফেরত TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  যদি((check_queries & async->dns_responses) != check_queries) {
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

ধ্রুবক কাঠামো Curl_addrinfo *Curl_resolv_get_ai(কাঠামো Curl_easy *তথ্য,
                                               uint32_t resolv_id,
                                               পূর্ণসংখ্যা ai_family,
                                               unsigned পূর্ণসংখ্যা সূচক)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  যদি(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ফেরত ফাঁকা;
  নির্বাচন(ai_family) {
  ক্ষেত্র AF_INET:
    ফেরত Curl_addrinfo_get(async->ai_A, ai_family, সূচক);
#ifdef USE_IPV6
  ক্ষেত্র AF_INET6:
    ফেরত Curl_addrinfo_get(async->ai_AAAA, ai_family, সূচক);
#endif
  পূর্বনির্ধারিত:
    ফেরত ফাঁকা;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           কাঠামো Curl_dns_entry **pdns)
{
  ফেরত Curl_resolv(তথ্য, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ধ্রুবক কাঠামো Curl_https_rrinfo *
Curl_resolv_get_https(কাঠামো Curl_easy *তথ্য, uint32_t resolv_id)
{
  কাঠামো Curl_resolv_async *async;
  যদি(!Curl_resolv_knows_https(তথ্য, resolv_id))
    ফেরত ফাঁকা;
  async = Curl_async_get(তথ্য, resolv_id);
  ফেরত async ? async->httpsrr : ফাঁকা;
}

bool Curl_resolv_knows_https(কাঠামো Curl_easy *তথ্য, uint32_t resolv_id)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  যদি(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ফেরত ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ফেরত TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
স্থির CURLcode hostip_resolv_start(কাঠামো Curl_easy *তথ্য,
                                    uint8_t dns_queries,
                                    কাঠামো Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    কাঠামো Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  কাঠামো Curl_resolv_async *async = ফাঁকা;
#endif
  কাঠামো Curl_addrinfo *addr = ফাঁকা;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode ফলাফল = CURLE_OK;

  *pnegative = FALSE;

  (শূন্য)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ফাঁকা;

  /* Check for "known" things to resolve ourselves. */
  যদি(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    যদি(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      ফলাফল = Curl_resolv_announce_start(তথ্য, ফাঁকা);
      যদি(ফলাফল)
        যাও out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      ফলাফল = Curl_str2addr(peer->hostname, peer->port, &addr);
      যাও out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    যদি(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      ফলাফল = Curl_resolv_announce_start(তথ্য, ফাঁকা);
      যদি(ফলাফল)
        যাও out;
      addr = get_localhost(peer->port, peer->hostname);
      যদি(!addr)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
#ifndef CURL_DISABLE_DOH
  যদি(!Curl_is_ipaddr(peer->hostname) && allowDOH && তথ্য->set.doh) {
    ফলাফল = Curl_resolv_announce_start(তথ্য, ফাঁকা);
    যদি(ফলাফল)
      যাও out;
    যদি(!async) {
      async = hostip_async_new(তথ্য, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      যদি(!async) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
    ফলাফল = Curl_doh(তথ্য, async);
    যাও out;
  }
#অন্যথা
  (শূন্য)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  যদি(!can_resolve_dns_queries(তথ্য, dns_queries)) {
    ফলাফল = RESOLV_FAIL(for_proxy);
    যাও out;
  }

#ifdef CURLRES_ASYNCH
  (শূন্য)addr;
  যদি(!async) {
    async = hostip_async_new(তথ্য, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    যদি(!async) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
  ফলাফল = Curl_async_getaddrinfo(তথ্য, async);
  যদি(ফলাফল == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_ফলাফল(তথ্য, async, pdns);
    যদি(r2)
      ফলাফল = r2;
    অন্যথা যদি(*pdns)
      ফলাফল = CURLE_OK;
  }
#অন্যথা
  ফলাফল = Curl_resolv_announce_start(তথ্য, ফাঁকা);
  যদি(ফলাফল)
    যাও out;
  addr = Curl_sync_getaddrinfo(তথ্য, dns_queries, peer->hostname, peer->port,
                               transport);
  যদি(!addr) {
    ফলাফল = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  যদি(!ফলাফল) {
    যদি(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(তথ্য, dns_queries, &addr, peer);
      যদি(!*pdns)
        ফলাফল = CURLE_OUT_OF_MEMORY;
    }
    অন্যথা যদি(!*pdns)
      ফলাফল = CURLE_AGAIN;
  }
  অন্যথা যদি(*pdns)
    Curl_dns_entry_unlink(তথ্য, pdns);
  অন্যথা যদি(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  যদি(async) {
    যদি(ফলাফল == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->পরবর্তী = তথ্য->state.async;
      তথ্য->state.async = async;
    }
    অন্যথা {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(তথ্য, async);
    }
  }
#endif
  ফেরত ফলাফল;
}

স্থির CURLcode hostip_resolv(কাঠামো Curl_easy *তথ্য,
                              uint8_t dns_queries,
                              কাঠামো Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              কাঠামো Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode ফলাফল = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (শূন্য)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ফাঁকা;

#ifdef CURL_DISABLE_DOH
  (শূন্য)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  যদি(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(তথ্য, "Not resolving .onion address (RFC 7686)");
    যাও out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(তথ্য, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  যদি((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(তথ্য, "DEBUG fail ipv6 resolve");
    ফলাফল = hostip_resolv_failed(তথ্য, peer, for_proxy);
    যাও out;
  }
#endif
  /* Let's check our DNS cache first */
  ফলাফল = Curl_dnscache_get(তথ্য, dns_queries, peer, pdns);
  যদি(*pdns) {
    infof(তথ্য, "Hostname %s was found in DNS cache", peer->hostname);
    ফলাফল = CURLE_OK;
  }
  অন্যথা যদি(ফলাফল) {
    infof(তথ্য, "Negative DNS entry");
    ফলাফল = hostip_resolv_failed(তথ্য, peer, for_proxy);
  }
  অন্যথা {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    ফলাফল = hostip_resolv_start(তথ্য, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(তথ্য, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (পূর্ণসংখ্যা)ফলাফল);
  }

out:
  যদি(ফলাফল && (ফলাফল != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(তথ্য, pdns);
    যদি(IS_RESOLV_FAIL(ফলাফল)) {
      যদি(cache_dns && negative)
        Curl_dnscache_add_negative(তথ্য, dns_queries, peer);
      যদি(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(তথ্য, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    অন্যথা {
      failf(তথ্য, "Error %d resolving %s:%u",
            (পূর্ণসংখ্যা)ফলাফল, peer->hostname, peer->port);
    }
  }
  অন্যথা যদি(cache_dns && *pdns) {
    ফলাফল = Curl_dnscache_add(তথ্য, *pdns);
    যদি(ফলাফল)
      Curl_dns_entry_unlink(তথ্য, pdns);
  }

  ফেরত ফলাফল;
}

CURLcode Curl_resolv_blocking(কাঠামো Curl_easy *তথ্য,
                              uint8_t dns_queries,
                              ধ্রুবক অক্ষর *hostname,
                              uint16_t port,
                              uint8_t transport,
                              কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো Curl_peer *peer = ফাঁকা;
  CURLcode ফলাফল;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ফাঁকা;

  ফলাফল = Curl_peer_create(তথ্য, তথ্য->conn->scheme, hostname, port, &peer);
  যদি(ফলাফল)
    যাও out;

  /* We cannot do a blocking resolve using DoH currently */
  ফলাফল = hostip_resolv(তথ্য, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  নির্বাচন(ফলাফল) {
  ক্ষেত্র CURLE_OK:
    DEBUGASSERT(*pdns);
    বিরতি;
#ifdef USE_CURL_ASYNC
  ক্ষেত্র CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    ফলাফল = Curl_async_await(তথ্য, resolv_id, pdns);
    Curl_resolv_destroy(তথ্য, resolv_id);
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    বিরতি;
  }

out:
  Curl_peer_unlink(&peer);
  ফেরত ফলাফল;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN স্থির শূন্য alarmfunc(পূর্ণসংখ্যা sig)
{
  (শূন্য)sig;
  siglongjmp(curl_jmpenv, 1);
}

স্থির CURLcode resolv_alarm_timeout(কাঠামো Curl_easy *তথ্য,
                                     uint8_t dns_queries,
                                     কাঠামো Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     কাঠামো Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  কাঠামো sigaction keep_sigact; /* store the old struct here */
  অস্থির bool keep_copysig = FALSE; /* whether old sigact has been saved */
  কাঠামো sigaction sigact;
#অন্যথা
#ifdef HAVE_SIGNAL
  শূন্য (*keep_sigact)(পূর্ণসংখ্যা);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  অস্থির long timeout;
  অস্থির unsigned পূর্ণসংখ্যা পূর্ব_alarm = 0;
  CURLcode ফলাফল;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!তথ্য->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!তথ্য->set.doh);
#endif

  *entry = ফাঁকা;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  যদি(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(তথ্য,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  যদি(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(তথ্য, "name lookup timed out");
    ফলাফল = CURLE_OPERATION_TIMEDOUT;
    যাও clean_up;
  }
  অন্যথা {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, ফাঁকা, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ফাঁকা);
#অন্যথা /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = সংকেত(SIGALRM, alarmfunc);
#endif
#endif /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    পূর্ব_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  ফলাফল = hostip_resolv(তথ্য, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, entry);

clean_up:
  যদি(!পূর্ব_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  যদি(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ফাঁকা); /* put it back */
  }
#অন্যথা
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  সংকেত(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  যদি(পূর্ব_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(তথ্য, তথ্য->conn, Curl_pgrs_now(তথ্য)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(পূর্ব_alarm - elapsed_secs);

    যদি(!alarm_set ||
       ((alarm_set >= 0x80000000) && (পূর্ব_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      failf(তথ্য, "Previous alarm fired off");
    }
    অন্যথা
      alarm((unsigned পূর্ণসংখ্যা)alarm_set);
  }

  ফেরত ফলাফল;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
স্থির CURLcode resolv_unix(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_peer *peer,
                            কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো Curl_addrinfo *addr;
  CURLcode ফলাফল;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ফাঁকা;

  ফলাফল = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(তথ্য, "Unix socket path too long: '%s'", peer->hostname);
      ফলাফল = CURLE_COULDNT_RESOLVE_HOST;
    }
    ফেরত ফলাফল;
  }

  *pdns = Curl_dnsc_mk_addr(তথ্য, 0, &addr, peer);
  ফেরত *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(কাঠামো Curl_easy *তথ্য,
                     কাঠামো Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     কাঠামো Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = ফাঁকা;

  যদি(timeout_ms < 0)
    /* got an already expired timeout */
    ফেরত CURLE_OPERATION_TIMEDOUT;
  অন্যথা যদি(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  যদি((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ফেরত resolv_unix(তথ্য, peer, pdns);
#অন্যথা
  যদি(peer->unix_socket)
    ফেরত hostip_resolv_failed(তথ্য, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  যদি(dns_queries & CURL_DNSQ_ADDR) {
    যদি(timeout_ms && তথ্য->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    যদি(timeout_ms && !Curl_doh_wanted(তথ্য)) {
      ফেরত resolv_alarm_timeout(তথ্য, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  যদি(timeout_ms)
    infof(তথ্য, "timeout on name lookup is not supported");
#endif

  ফেরত hostip_resolv(তথ্য, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

কাঠামো Curl_resolv_async *Curl_async_get(কাঠামো Curl_easy *তথ্য,
                                         uint32_t resolv_id)
{
  কাঠামো Curl_resolv_async *async = তথ্য->state.async;
  জন্য(; async; async = async->পরবর্তী) {
    যদি(async->id == resolv_id)
      ফেরত async;
  }
  ফেরত ফাঁকা;
}

CURLcode Curl_resolv_take_ফলাফল(কাঠামো Curl_easy *তথ্য, uint32_t resolv_id,
                                 কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  CURLcode ফলাফল;

  /* If async resolving is ongoing, this must be set */
  যদি(!async)
    ফেরত CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  ফলাফল = Curl_dnscache_get(তথ্য, async->dns_queries, async->peer, pdns);
  যদি(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(তথ্য, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(তথ্য, async);
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(ফলাফল) {
    Curl_async_shutdown(তথ্য, async);
    ফেরত Curl_async_failed(তথ্য, async, ফাঁকা);
  }

  ফলাফল = hostip_resolv_take_ফলাফল(তথ্য, async, pdns);

  যদি(*pdns) {
    /* Add to cache */
    ফলাফল = Curl_dnscache_add(তথ্য, *pdns);
    যদি(ফলাফল)
      Curl_dns_entry_unlink(তথ্য, pdns);
  }
  অন্যথা যদি(IS_RESOLV_FAIL(ফলাফল)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    যদি(async->negative_answer)
      Curl_dnscache_add_negative(তথ্য, async->dns_queries, async->peer);
    যদি(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(তথ্য, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  অন্যথা যদি(ফলাফল) {
    failf(তথ্য, "Error %d resolving %s:%u",
          (পূর্ণসংখ্যা)ফলাফল, async->peer->hostname, async->peer->port);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_resolv_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  কাঠামো Curl_resolv_async *async = তথ্য->state.async;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)ps;
  জন্য(; async && !ফলাফল; async = async->পরবর্তী) {
#ifndef CURL_DISABLE_DOH
    যদি(async->doh) /* DoH has nothing for the pollset */
      চালু;
#endif
    ফলাফল = Curl_async_pollset(তথ্য, async, ps);
  }
  ফেরত ফলাফল;
}

শূন্য Curl_resolv_destroy(কাঠামো Curl_easy *তথ্য, uint32_t resolv_id)
{
  কাঠামো Curl_resolv_async **panchor = &তথ্য->state.async;

  জন্য(; *panchor; panchor = &(*panchor)->পরবর্তী) {
    কাঠামো Curl_resolv_async *async = *panchor;
    যদি(async->id == resolv_id) {
      *panchor = async->পরবর্তী;
      Curl_async_destroy(তথ্য, async);
      বিরতি;
    }
  }
}

শূন্য Curl_resolv_shutdown_all(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_resolv_async *async = তথ্য->state.async;
  জন্য(; async; async = async->পরবর্তী) {
    Curl_async_shutdown(তথ্য, async);
  }
}

শূন্য Curl_resolv_destroy_all(কাঠামো Curl_easy *তথ্য)
{
  যখন(তথ্য->state.async) {
    কাঠামো Curl_resolv_async *async = তথ্য->state.async;
    তথ্য->state.async = async->পরবর্তী;
    Curl_async_destroy(তথ্য, async);
  }
}

#endif /* USE_CURL_ASYNC */
