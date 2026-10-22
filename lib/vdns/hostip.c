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

#define IS_RESOLV_FAIL(result) \
  (((result) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((result) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#ifdef USE_IPV6
স্থির bool ipv6works(সংরচনা Curl_easy *data);
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

uint8_t Curl_resolv_dns_queries(সংরচনা Curl_easy *data, uint8_t ip_version)
{
  (শূন্য)data;
  বিকল্প(ip_version) {
  পক্ষ CURL_IPRESOLVE_V6:
    প্রত্যাগম CURL_DNSQ_AAAA;
  পক্ষ CURL_IPRESOLVE_V4:
    প্রত্যাগম CURL_DNSQ_A;
  স্বভাব:
    যদি(ipv6works(data))
      প্রত্যাগম (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    অন্যথা
      প্রত্যাগম CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
ধ্রুব বর্ণ *Curl_resolv_query_str(uint8_t dns_queries)
{
  বিকল্প(dns_queries) {
  পক্ষ (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    প্রত্যাগম "A+AAAA+HTTPS";
  পক্ষ (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    প্রত্যাগম "A+AAAA";
  পক্ষ (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    প্রত্যাগম "AAAA+HTTPS";
  পক্ষ (CURL_DNSQ_AAAA):
    প্রত্যাগম "AAAA";
  পক্ষ (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    প্রত্যাগম "A+HTTPS";
  পক্ষ (CURL_DNSQ_A):
    প্রত্যাগম "A";
  পক্ষ (CURL_DNSQ_HTTPS):
    প্রত্যাগম "HTTPS";
  পক্ষ 0:
    প্রত্যাগম "-";
  স্বভাব:
    DEBUGASSERT(0);
    প্রত্যাগম "???";
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
শূন্য Curl_printable_address(ধ্রুব সংরচনা Curl_addrinfo *ai, বর্ণ *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  বিকল্প(ai->ai_family) {
  পক্ষ AF_INET: {
    ধ্রুব সংরচনা sockaddr_in *sa4 = (ধ্রুব শূন্য *)ai->ai_addr;
    ধ্রুব সংরচনা in_addr *ipaddr4 = &sa4->sin_addr;
    (শূন্য)curlx_inet_ntop(ai->ai_family, (ধ্রুব শূন্য *)ipaddr4, buf, bufsize);
    ভঙ্গ;
  }
#ifdef USE_IPV6
  পক্ষ AF_INET6: {
    ধ্রুব সংরচনা sockaddr_in6 *sa6 = (ধ্রুব শূন্য *)ai->ai_addr;
    ধ্রুব সংরচনা in6_addr *ipaddr6 = &sa6->sin6_addr;
    (শূন্য)curlx_inet_ntop(ai->ai_family, (ধ্রুব শূন্য *)ipaddr6, buf, bufsize);
    ভঙ্গ;
  }
#endif
  স্বভাব:
    ভঙ্গ;
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
স্থির সংরচনা Curl_addrinfo *get_localhost6(uint16_t port, ধ্রুব বর্ণ *name)
{
  সংরচনা Curl_addrinfo *ca;
  ধ্রুব size_t ss_size = পরিমাণ(সংরচনা sockaddr_in6);
  ধ্রুব size_t hostlen = strlen(name);
  সংরচনা sockaddr_in6 sa6;
  unsigned বর্ণ ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, পরিমাণ(সংরচনা Curl_addrinfo) + ss_size + hostlen + 1);
  যদি(!ca)
    প্রত্যাগম রিক্ত;

  memset(&sa6, 0, পরিমাণ(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (শূন্য)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, পরিমাণ(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = রিক্ত;
  ca->ai_addr = (শূন্য *)((বর্ণ *)ca + পরিমাণ(সংরচনা Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (বর্ণ *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  প্রত্যাগম ca;
}
#অন্যথা
#define get_localhost6(x, y) রিক্ত
#endif

/* return a static IPv4 127.0.0.1 for the given name */
স্থির সংরচনা Curl_addrinfo *get_localhost(uint16_t port, ধ্রুব বর্ণ *name)
{
  সংরচনা Curl_addrinfo *ca;
  সংরচনা Curl_addrinfo *ca6;
  ধ্রুব size_t ss_size = পরিমাণ(সংরচনা sockaddr_in);
  ধ্রুব size_t hostlen = strlen(name);
  সংরচনা sockaddr_in sa;
  unsigned পূর্ণাঙ্ক ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, পরিমাণ(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  যদি(curlx_inet_pton(AF_INET, "127.0.0.1", (বর্ণ *)&ipv4) < 1)
    প্রত্যাগম রিক্ত;
  memcpy(&sa.sin_addr, &ipv4, পরিমাণ(ipv4));

  ca = curlx_calloc(1, পরিমাণ(সংরচনা Curl_addrinfo) + ss_size + hostlen + 1);
  যদি(!ca)
    প্রত্যাগম রিক্ত;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (শূন্য *)((বর্ণ *)ca + পরিমাণ(সংরচনা Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (বর্ণ *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  যদি(!ca6)
    প্রত্যাগম ca;
  ca6->ai_next = ca;
  প্রত্যাগম ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(সংরচনা Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  যদি(s == CURL_SOCKET_BAD) {
    যদি(SOCKERRNO == SOCKENOMEM)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
স্থির bool ipv6works(সংরচনা Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  প্রত্যাগম data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ধ্রুব বর্ণ *hostname)
{
  সংরচনা in_addr in;
#ifdef USE_IPV6
  সংরচনা in6_addr in6;
#endif
  যদি(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
স্থির bool tailmatch(ধ্রুব বর্ণ *full, size_t flen,
                      ধ্রুব বর্ণ *part, size_t plen)
{
  যদি(plen > flen)
    প্রত্যাগম FALSE;
  প্রত্যাগম curl_strnequal(part, &full[flen - plen], plen);
}

স্থির CURLcode hostip_resolv_failed(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  প্রত্যাগম RESOLV_FAIL(for_proxy);
}

স্থির bool can_resolve_dns_queries(সংরচনা Curl_easy *data,
                                    uint8_t dns_queries)
{
  (শূন্য)data;
  যদি((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    প্রত্যাগম FALSE;
  প্রত্যাগম TRUE;
}

CURLcode Curl_resolv_announce_start(সংরচনা Curl_easy *data,
                                    শূন্য *resolver)
{
  যদি(data->set.resolver_start) {
    সংরচনা Curl_mapi_guard guard;
    পূর্ণাঙ্ক rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, রিক্ত,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    যদি(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      প্রত্যাগম CURLE_ABORTED_BY_CALLBACK;
    }
  }
  প্রত্যাগম CURLE_OK;
}

#ifdef USE_CURL_ASYNC

স্থির সংরচনা Curl_resolv_async *hostip_async_new(সংরচনা Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  সংরচনা Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  সংরচনা Curl_resolv_async *async;

  যদি(!data->multi) {
    DEBUGASSERT(0);
    প্রত্যাগম রিক্ত;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, পরিমাণ(*async));
  যদি(!async)
    প্রত্যাগম রিক্ত;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  যদি(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  অন্যথা
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  যদি(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  প্রত্যাগম async;
}

স্থির CURLcode hostip_resolv_take_result(সংরচনা Curl_easy *data,
                                          সংরচনা Curl_resolv_async *async,
                                          সংরচনা Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  যদি(!async)
    প্রত্যাগম CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  যদি(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  অন্যথা
#endif
  result = Curl_async_take_result(data, async, pdns);

  যদি(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  অন্যথা যদি(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, রিক্ত);
  }
  অন্যথা যদি(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (পূর্ণাঙ্ক)result, async->peer->hostname, async->peer->port);
  }
  অন্যথা {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  প্রত্যাগম result;
}

timediff_t Curl_resolv_elapsed_ms(সংরচনা Curl_easy *data,
                                  uint32_t resolv_id,
                                  ধ্রুব সংরচনা curltime *pnow)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  যদি(!async)
    প্রত্যাগম CURL_TIMEOUT_RESOLVE_MS;
  প্রত্যাগম curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(সংরচনা Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  যদি(!async || async->done)
    প্রত্যাগম TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  যদি((check_queries & async->dns_responses) != check_queries) {
    প্রত্যাগম FALSE;
  }
  প্রত্যাগম TRUE;
}

ধ্রুব সংরচনা Curl_addrinfo *Curl_resolv_get_ai(সংরচনা Curl_easy *data,
                                               uint32_t resolv_id,
                                               পূর্ণাঙ্ক ai_family,
                                               unsigned পূর্ণাঙ্ক index)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  যদি(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    প্রত্যাগম রিক্ত;
  বিকল্প(ai_family) {
  পক্ষ AF_INET:
    প্রত্যাগম Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  পক্ষ AF_INET6:
    প্রত্যাগম Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  স্বভাব:
    প্রত্যাগম রিক্ত;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(সংরচনা Curl_easy *data,
                           সংরচনা Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           সংরচনা Curl_dns_entry **pdns)
{
  প্রত্যাগম Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ধ্রুব সংরচনা Curl_https_rrinfo *
Curl_resolv_get_https(সংরচনা Curl_easy *data, uint32_t resolv_id)
{
  সংরচনা Curl_resolv_async *async;
  যদি(!Curl_resolv_knows_https(data, resolv_id))
    প্রত্যাগম রিক্ত;
  async = Curl_async_get(data, resolv_id);
  প্রত্যাগম async ? async->httpsrr : রিক্ত;
}

bool Curl_resolv_knows_https(সংরচনা Curl_easy *data, uint32_t resolv_id)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  যদি(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    প্রত্যাগম ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  প্রত্যাগম TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
স্থির CURLcode hostip_resolv_start(সংরচনা Curl_easy *data,
                                    uint8_t dns_queries,
                                    সংরচনা Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    সংরচনা Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  সংরচনা Curl_resolv_async *async = রিক্ত;
#endif
  সংরচনা Curl_addrinfo *addr = রিক্ত;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (শূন্য)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = রিক্ত;

  /* Check for "known" things to resolve ourselves. */
  যদি(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    যদি(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, রিক্ত);
      যদি(result)
        গচ্ছ out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      গচ্ছ out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    যদি(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, রিক্ত);
      যদি(result)
        গচ্ছ out;
      addr = get_localhost(peer->port, peer->hostname);
      যদি(!addr)
        result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }
#ifndef CURL_DISABLE_DOH
  যদি(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, রিক্ত);
    যদি(result)
      গচ্ছ out;
    যদি(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      যদি(!async) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
    result = Curl_doh(data, async);
    গচ্ছ out;
  }
#অন্যথা
  (শূন্য)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  যদি(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    গচ্ছ out;
  }

#ifdef CURLRES_ASYNCH
  (শূন্য)addr;
  যদি(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    যদি(!async) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  যদি(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    যদি(r2)
      result = r2;
    অন্যথা যদি(*pdns)
      result = CURLE_OK;
  }
#অন্যথা
  result = Curl_resolv_announce_start(data, রিক্ত);
  যদি(result)
    গচ্ছ out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  যদি(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  যদি(!result) {
    যদি(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      যদি(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    অন্যথা যদি(!*pdns)
      result = CURLE_AGAIN;
  }
  অন্যথা যদি(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  অন্যথা যদি(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  যদি(async) {
    যদি(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    অন্যথা {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  প্রত্যাগম result;
}

স্থির CURLcode hostip_resolv(সংরচনা Curl_easy *data,
                              uint8_t dns_queries,
                              সংরচনা Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              সংরচনা Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (শূন্য)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = রিক্ত;

#ifdef CURL_DISABLE_DOH
  (শূন্য)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  যদি(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    গচ্ছ out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  যদি((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    গচ্ছ out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  যদি(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  অন্যথা যদি(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  অন্যথা {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (পূর্ণাঙ্ক)result);
  }

out:
  যদি(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    যদি(IS_RESOLV_FAIL(result)) {
      যদি(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      যদি(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    অন্যথা {
      failf(data, "Error %d resolving %s:%u",
            (পূর্ণাঙ্ক)result, peer->hostname, peer->port);
    }
  }
  অন্যথা যদি(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    যদি(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  প্রত্যাগম result;
}

CURLcode Curl_resolv_blocking(সংরচনা Curl_easy *data,
                              uint8_t dns_queries,
                              ধ্রুব বর্ণ *hostname,
                              uint16_t port,
                              uint8_t transport,
                              সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_peer *peer = রিক্ত;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = রিক্ত;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  যদি(result)
    গচ্ছ out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  বিকল্প(result) {
  পক্ষ CURLE_OK:
    DEBUGASSERT(*pdns);
    ভঙ্গ;
#ifdef USE_CURL_ASYNC
  পক্ষ CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    ভঙ্গ;
#endif
  স্বভাব:
    ভঙ্গ;
  }

out:
  Curl_peer_unlink(&peer);
  প্রত্যাগম result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN স্থির শূন্য alarmfunc(পূর্ণাঙ্ক sig)
{
  (শূন্য)sig;
  siglongjmp(curl_jmpenv, 1);
}

স্থির CURLcode resolv_alarm_timeout(সংরচনা Curl_easy *data,
                                     uint8_t dns_queries,
                                     সংরচনা Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     সংরচনা Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  সংরচনা sigaction keep_sigact; /* store the old struct here */
  চঞ্চল bool keep_copysig = FALSE; /* whether old sigact has been saved */
  সংরচনা sigaction sigact;
#অন্যথা
#ifdef HAVE_SIGNAL
  শূন্য (*keep_sigact)(পূর্ণাঙ্ক);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  চঞ্চল long timeout;
  চঞ্চল unsigned পূর্ণাঙ্ক prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = রিক্ত;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  যদি(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
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
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    গচ্ছ clean_up;
  }
  অন্যথা {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, রিক্ত, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, রিক্ত);
#অন্যথা /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = সঙ্কেত(SIGALRM, alarmfunc);
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
  যদি(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  যদি(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, রিক্ত); /* put it back */
  }
#অন্যথা
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  সঙ্কেত(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  যদি(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    যদি(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    অন্যথা
      alarm((unsigned পূর্ণাঙ্ক)alarm_set);
  }

  প্রত্যাগম result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
স্থির CURLcode resolv_unix(সংরচনা Curl_easy *data,
                            সংরচনা Curl_peer *peer,
                            সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = রিক্ত;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  যদি(result) {
    যদি(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    প্রত্যাগম result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  প্রত্যাগম *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(সংরচনা Curl_easy *data,
                     সংরচনা Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     সংরচনা Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = রিক্ত;

  যদি(timeout_ms < 0)
    /* got an already expired timeout */
    প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
  অন্যথা যদি(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  যদি((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    প্রত্যাগম resolv_unix(data, peer, pdns);
#অন্যথা
  যদি(peer->unix_socket)
    প্রত্যাগম hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  যদি(dns_queries & CURL_DNSQ_ADDR) {
    যদি(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    যদি(timeout_ms && !Curl_doh_wanted(data)) {
      প্রত্যাগম resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  যদি(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  প্রত্যাগম hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

সংরচনা Curl_resolv_async *Curl_async_get(সংরচনা Curl_easy *data,
                                         uint32_t resolv_id)
{
  সংরচনা Curl_resolv_async *async = data->state.async;
  প্রতি(; async; async = async->next) {
    যদি(async->id == resolv_id)
      প্রত্যাগম async;
  }
  প্রত্যাগম রিক্ত;
}

CURLcode Curl_resolv_take_result(সংরচনা Curl_easy *data, uint32_t resolv_id,
                                 সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  যদি(!async)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  যদি(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা যদি(result) {
    Curl_async_shutdown(data, async);
    প্রত্যাগম Curl_async_failed(data, async, রিক্ত);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  যদি(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    যদি(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  অন্যথা যদি(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    যদি(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    যদি(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  অন্যথা যদি(result) {
    failf(data, "Error %d resolving %s:%u",
          (পূর্ণাঙ্ক)result, async->peer->hostname, async->peer->port);
  }
  প্রত্যাগম result;
}

CURLcode Curl_resolv_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  সংরচনা Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (শূন্য)ps;
  প্রতি(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    যদি(async->doh) /* DoH has nothing for the pollset */
      অনুবর্ত;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  প্রত্যাগম result;
}

শূন্য Curl_resolv_destroy(সংরচনা Curl_easy *data, uint32_t resolv_id)
{
  সংরচনা Curl_resolv_async **panchor = &data->state.async;

  প্রতি(; *panchor; panchor = &(*panchor)->next) {
    সংরচনা Curl_resolv_async *async = *panchor;
    যদি(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      ভঙ্গ;
    }
  }
}

শূন্য Curl_resolv_shutdown_all(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_resolv_async *async = data->state.async;
  প্রতি(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

শূন্য Curl_resolv_destroy_all(সংরচনা Curl_easy *data)
{
  যাবত্(data->state.async) {
    সংরচনা Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
