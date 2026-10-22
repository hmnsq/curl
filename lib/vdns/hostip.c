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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

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
#تعریف_کرو USE_ALARM_TIMEOUT
#اگر_ختم

#تعریف_کرو RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#تعریف_کرو IS_RESOLV_FAIL(نتیجہ) \
  (((نتیجہ) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((نتیجہ) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#اگر_تعریف_شدہ USE_IPV6
ساکن bool ipv6works(ساخت Curl_easy *ڈیٹا);
#ورنہ
#تعریف_کرو ipv6works(x) FALSE
#اگر_ختم

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

uint8_t Curl_resolv_dns_queries(ساخت Curl_easy *ڈیٹا, uint8_t ip_نسخہ)
{
  (خالی)ڈیٹا;
  انتخاب(ip_نسخہ) {
  حالت CURL_IPRESOLVE_V6:
    واپس CURL_DNSQ_AAAA;
  حالت CURL_IPRESOLVE_V4:
    واپس CURL_DNSQ_A;
  طےشدہ:
    اگر(ipv6works(ڈیٹا))
      واپس (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    ورنہ
      واپس CURL_DNSQ_A;
  }
}

#اگر_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_resolv_query_str(uint8_t dns_queries)
{
  انتخاب(dns_queries) {
  حالت (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    واپس "A+AAAA+HTTPS";
  حالت (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    واپس "A+AAAA";
  حالت (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    واپس "AAAA+HTTPS";
  حالت (CURL_DNSQ_AAAA):
    واپس "AAAA";
  حالت (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    واپس "A+HTTPS";
  حالت (CURL_DNSQ_A):
    واپس "A";
  حالت (CURL_DNSQ_HTTPS):
    واپس "HTTPS";
  حالت 0:
    واپس "-";
  طےشدہ:
    DEBUGASSERT(0);
    واپس "???";
  }
}
#اگر_ختم

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
خالی Curl_printable_پتہ(ثابت ساخت Curl_addrinfo *ai, حرف *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  انتخاب(ai->ai_family) {
  حالت AF_INET: {
    ثابت ساخت sockaddr_in *sa4 = (ثابت خالی *)ai->ai_addr;
    ثابت ساخت in_addr *ipaddr4 = &sa4->sin_addr;
    (خالی)curlx_inet_ntop(ai->ai_family, (ثابت خالی *)ipaddr4, buf, bufsize);
    توڑو;
  }
#اگر_تعریف_شدہ USE_IPV6
  حالت AF_INET6: {
    ثابت ساخت sockaddr_in6 *sa6 = (ثابت خالی *)ai->ai_addr;
    ثابت ساخت in6_addr *ipaddr6 = &sa6->sin6_addr;
    (خالی)curlx_inet_ntop(ai->ai_family, (ثابت خالی *)ipaddr6, buf, bufsize);
    توڑو;
  }
#اگر_ختم
  طےشدہ:
    توڑو;
  }
}

#اگر_تعریف_شدہ USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ساکن sigjmp_buf curl_jmpenv;
ساکن curl_simple_تالا curl_jmpenv_تالا = CURL_SIMPLE_تالا_INIT;
#اگر_ختم

#اگر_تعریف_شدہ USE_IPV6
/* return a static IPv6 ::1 for the name */
ساکن ساخت Curl_addrinfo *get_localhost6(uint16_t پورٹ, ثابت حرف *name)
{
  ساخت Curl_addrinfo *ca;
  ثابت size_t ss_سائز = حجم(ساخت sockaddr_in6);
  ثابت size_t hostlen = لڑی_لمبائی(name);
  ساخت sockaddr_in6 sa6;
  غیرعلامتی حرف ipv6[16];
  غیرعلامتی مختصر port16 = (غیرعلامتی مختصر)(پورٹ & 0xffff);
  ca = curlx_calloc(1, حجم(ساخت Curl_addrinfo) + ss_سائز + hostlen + 1);
  اگر(!ca)
    واپس تہی;

  memset(&sa6, 0, حجم(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (خالی)curlx_inet_pton(AF_INET6, "::1", ipv6);
  حافظہ_نقل(&sa6.sin6_addr, ipv6, حجم(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_سائز;
  ca->ai_اگلا      = تہی;
  ca->ai_addr = (خالی *)((حرف *)ca + حجم(ساخت Curl_addrinfo));
  حافظہ_نقل(ca->ai_addr, &sa6, ss_سائز);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_سائز;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  واپس ca;
}
#ورنہ
#تعریف_کرو get_localhost6(x, y) تہی
#اگر_ختم

/* return a static IPv4 127.0.0.1 for the given name */
ساکن ساخت Curl_addrinfo *get_localhost(uint16_t پورٹ, ثابت حرف *name)
{
  ساخت Curl_addrinfo *ca;
  ساخت Curl_addrinfo *ca6;
  ثابت size_t ss_سائز = حجم(ساخت sockaddr_in);
  ثابت size_t hostlen = لڑی_لمبائی(name);
  ساخت sockaddr_in sa;
  غیرعلامتی صحیح ipv4;
  غیرعلامتی مختصر port16 = (غیرعلامتی مختصر)(پورٹ & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, حجم(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  اگر(curlx_inet_pton(AF_INET, "127.0.0.1", (حرف *)&ipv4) < 1)
    واپس تہی;
  حافظہ_نقل(&sa.sin_addr, &ipv4, حجم(ipv4));

  ca = curlx_calloc(1, حجم(ساخت Curl_addrinfo) + ss_سائز + hostlen + 1);
  اگر(!ca)
    واپس تہی;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_سائز;
  ca->ai_addr = (خالی *)((حرف *)ca + حجم(ساخت Curl_addrinfo));
  حافظہ_نقل(ca->ai_addr, &sa, ss_سائز);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_سائز;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(پورٹ, name);
  اگر(!ca6)
    واپس ca;
  ca6->ai_اگلا = ca;
  واپس ca6;
}

#اگر_تعریف_شدہ USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(ساخت Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_چلتا = FALSE;
  اگر(s == CURL_SOCKET_BAD) {
    اگر(SOCKERRNO == SOCKENOMEM)
      واپس CURLE_برآمد_کا_حافظہ;
  }
  ورنہ {
    multi->ipv6_چلتا = TRUE;
    sclose(s);
  }
  واپس CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ساکن bool ipv6works(ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->multi);
  واپس ڈیٹا ? ڈیٹا->multi->ipv6_چلتا : FALSE;
}
#اگر_ختم /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت حرف *hostname)
{
  ساخت in_addr اندر;
#اگر_تعریف_شدہ USE_IPV6
  ساخت in6_addr in6;
#اگر_ختم
  اگر(curlx_inet_pton(AF_INET, hostname, &اندر) > 0
#اگر_تعریف_شدہ USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#اگر_ختم
    )
    واپس TRUE;
  واپس FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ساکن bool tailmatch(ثابت حرف *بھرا, size_t flen,
                      ثابت حرف *part, size_t plen)
{
  اگر(plen > flen)
    واپس FALSE;
  واپس curl_strnequal(part, &بھرا[flen - plen], plen);
}

ساکن CURLcode hostip_resolv_failed(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_peer *peer,
                                     bool for_proxy)
{
  failf(ڈیٹا, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  واپس RESOLV_FAIL(for_proxy);
}

ساکن bool can_resolve_dns_queries(ساخت Curl_easy *ڈیٹا,
                                    uint8_t dns_queries)
{
  (خالی)ڈیٹا;
  اگر((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(ڈیٹا))
    واپس FALSE;
  واپس TRUE;
}

CURLcode Curl_resolv_announce_آغاز(ساخت Curl_easy *ڈیٹا,
                                    خالی *resolver)
{
  اگر(ڈیٹا->سیٹ.resolver_آغاز) {
    ساخت Curl_mapi_guard guard;
    صحیح rc;

    CURL_TRC_DNS(ڈیٹا, "announcing resolve to application");
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_resolver_آغاز);
    rc = ڈیٹا->سیٹ.resolver_آغاز(resolver, تہی,
                                  ڈیٹا->سیٹ.resolver_آغاز_client);
    CURL_CBAPI_انتہا(&guard);
    اگر(rc) {
      CURL_TRC_DNS(ڈیٹا, "application aborted resolve");
      واپس CURLE_ABORTED_BY_CALLBACK;
    }
  }
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ USE_CURL_ASYNC

ساکن ساخت Curl_resolv_async *hostip_async_new(ساخت Curl_easy *ڈیٹا,
                                                  uint8_t dns_queries,
                                                  ساخت Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  ساخت Curl_resolv_async *async;

  اگر(!ڈیٹا->multi) {
    DEBUGASSERT(0);
    واپس تہی;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, حجم(*async));
  اگر(!async)
    واپس تہی;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  اگر(ڈیٹا->multi->آخری_resolv_id == UINT32_MAX)
    ڈیٹا->multi->آخری_resolv_id = 1; /* wrap around */
  ورنہ
    ڈیٹا->multi->آخری_resolv_id++;
  async->id = ڈیٹا->multi->آخری_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->آغاز = *Curl_pgrs_now(ڈیٹا);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  اگر(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  واپس async;
}

ساکن CURLcode hostip_resolv_take_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_resolv_async *async,
                                          ساخت Curl_dns_اندراج **pdns)
{
  CURLcode نتیجہ;

  /* If async resolving is ongoing, this must be set */
  اگر(!async)
    واپس CURLE_FAILED_INIT;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  اگر(async->doh)
    نتیجہ = Curl_doh_take_نتیجہ(ڈیٹا, async, pdns);
  ورنہ
#اگر_ختم
  نتیجہ = Curl_async_take_نتیجہ(ڈیٹا, async, pdns);

  اگر(نتیجہ == CURLE_AGAIN) {
    CURL_TRC_DNS(ڈیٹا, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->پورٹ);
    نتیجہ = CURLE_OK;
  }
  ورنہ اگر(IS_RESOLV_FAIL(نتیجہ)) {
    نتیجہ = Curl_async_failed(ڈیٹا, async, تہی);
  }
  ورنہ اگر(نتیجہ) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(ڈیٹا, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحیح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  ورنہ {
    CURL_TRC_DNS(ڈیٹا, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->پورٹ);
    DEBUGASSERT(*pdns);
  }

  واپس نتیجہ;
}

timediff_t Curl_resolv_elapsed_ms(ساخت Curl_easy *ڈیٹا,
                                  uint32_t resolv_id,
                                  ثابت ساخت curltime *pnow)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  اگر(!async)
    واپس CURL_TIMEOUT_RESOLVE_MS;
  واپس curlx_ptimediff_ms(pnow, &async->آغاز);
}

bool Curl_resolv_has_answers(ساخت Curl_easy *ڈیٹا,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  اگر(!async || async->مکمل)
    واپس TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  اگر((check_queries & async->dns_responses) != check_queries) {
    واپس FALSE;
  }
  واپس TRUE;
}

ثابت ساخت Curl_addrinfo *Curl_resolv_get_ai(ساخت Curl_easy *ڈیٹا,
                                               uint32_t resolv_id,
                                               صحیح ai_family,
                                               غیرعلامتی صحیح اشاریہ)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  اگر(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    واپس تہی;
  انتخاب(ai_family) {
  حالت AF_INET:
    واپس Curl_addrinfo_get(async->ai_A, ai_family, اشاریہ);
#اگر_تعریف_شدہ USE_IPV6
  حالت AF_INET6:
    واپس Curl_addrinfo_get(async->ai_AAAA, ai_family, اشاریہ);
#اگر_ختم
  طےشدہ:
    واپس تہی;
  }
}

#اگر_تعریف_شدہ USE_HTTPSRR

CURLcode Curl_resolv_https(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ساخت Curl_dns_اندراج **pdns)
{
  واپس Curl_resolv(ڈیٹا, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت ساخت Curl_https_rrinfo *
Curl_resolv_get_https(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id)
{
  ساخت Curl_resolv_async *async;
  اگر(!Curl_resolv_knows_https(ڈیٹا, resolv_id))
    واپس تہی;
  async = Curl_async_get(ڈیٹا, resolv_id);
  واپس async ? async->httpsrr : تہی;
}

bool Curl_resolv_knows_https(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  اگر(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    واپس ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  واپس TRUE; /* we know it will never come */
}

#اگر_ختم /* USE_HTTPSRR */

#اگر_ختم /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ساکن CURLcode hostip_resolv_آغاز(ساخت Curl_easy *ڈیٹا,
                                    uint8_t dns_queries,
                                    ساخت Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    ساخت Curl_dns_اندراج **pdns,
                                    bool *pnegative)
{
#اگر_تعریف_شدہ USE_CURL_ASYNC
  ساخت Curl_resolv_async *async = تہی;
#اگر_ختم
  ساخت Curl_addrinfo *addr = تہی;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode نتیجہ = CURLE_OK;

  *pnegative = FALSE;

  (خالی)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = تہی;

  /* Check for "known" things to resolve ourselves. */
  اگر(addr_queries) {
#اگر_غیر_تعریف_شدہ USE_RESOLVE_ON_IPS
    اگر(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, تہی);
      اگر(نتیجہ)
        جاؤ برآمد;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      نتیجہ = Curl_str2addr(peer->hostname, peer->پورٹ, &addr);
      جاؤ برآمد;
    }
#اگر_ختم

    hostname_len = لڑی_لمبائی(peer->hostname);
    اگر(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, تہی);
      اگر(نتیجہ)
        جاؤ برآمد;
      addr = get_localhost(peer->پورٹ, peer->hostname);
      اگر(!addr)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  اگر(!Curl_is_ipaddr(peer->hostname) && allowDOH && ڈیٹا->سیٹ.doh) {
    نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, تہی);
    اگر(نتیجہ)
      جاؤ برآمد;
    اگر(!async) {
      async = hostip_async_new(ڈیٹا, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      اگر(!async) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
    نتیجہ = Curl_doh(ڈیٹا, async);
    جاؤ برآمد;
  }
#ورنہ
  (خالی)allowDOH;
#اگر_ختم

  /* Can we provide the requested IP specifics in resolving? */
  اگر(!can_resolve_dns_queries(ڈیٹا, dns_queries)) {
    نتیجہ = RESOLV_FAIL(for_proxy);
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ CURLRES_ASYNCH
  (خالی)addr;
  اگر(!async) {
    async = hostip_async_new(ڈیٹا, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    اگر(!async) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
  نتیجہ = Curl_async_getaddrinfo(ڈیٹا, async);
  اگر(نتیجہ == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_نتیجہ(ڈیٹا, async, pdns);
    اگر(r2)
      نتیجہ = r2;
    ورنہ اگر(*pdns)
      نتیجہ = CURLE_OK;
  }
#ورنہ
  نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, تہی);
  اگر(نتیجہ)
    جاؤ برآمد;
  addr = Curl_sync_getaddrinfo(ڈیٹا, dns_queries, peer->hostname, peer->پورٹ,
                               transport);
  اگر(!addr) {
    نتیجہ = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#اگر_ختم

برآمد:
  اگر(!نتیجہ) {
    اگر(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(ڈیٹا, dns_queries, &addr, peer);
      اگر(!*pdns)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
    }
    ورنہ اگر(!*pdns)
      نتیجہ = CURLE_AGAIN;
  }
  ورنہ اگر(*pdns)
    Curl_dns_اندراج_unlink(ڈیٹا, pdns);
  ورنہ اگر(addr)
    Curl_freeaddrinfo(addr);

#اگر_تعریف_شدہ USE_CURL_ASYNC
  اگر(async) {
    اگر(نتیجہ == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->اگلا = ڈیٹا->state.async;
      ڈیٹا->state.async = async;
    }
    ورنہ {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(ڈیٹا, async);
    }
  }
#اگر_ختم
  واپس نتیجہ;
}

ساکن CURLcode hostip_resolv(ساخت Curl_easy *ڈیٹا,
                              uint8_t dns_queries,
                              ساخت Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              ساخت Curl_dns_اندراج **pdns)
{
  size_t hostname_len;
  CURLcode نتیجہ = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (خالی)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = تہی;

#اگر_تعریف_شدہ CURL_DISABLE_DOH
  (خالی)allowDOH;
#اگر_ختم

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = لڑی_لمبائی(peer->hostname);
  DEBUGASSERT(hostname_len);
  اگر(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(ڈیٹا, "Not resolving .onion address (RFC 7686)");
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ DEBUGBUILD
  CURL_TRC_DNS(ڈیٹا, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->پورٹ);
  اگر((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(ڈیٹا, "DEBUG fail ipv6 resolve");
    نتیجہ = hostip_resolv_failed(ڈیٹا, peer, for_proxy);
    جاؤ برآمد;
  }
#اگر_ختم
  /* Let's check our DNS cache first */
  نتیجہ = Curl_dnscache_get(ڈیٹا, dns_queries, peer, pdns);
  اگر(*pdns) {
    infof(ڈیٹا, "Hostname %s was found in DNS cache", peer->hostname);
    نتیجہ = CURLE_OK;
  }
  ورنہ اگر(نتیجہ) {
    infof(ڈیٹا, "Negative DNS entry");
    نتیجہ = hostip_resolv_failed(ڈیٹا, peer, for_proxy);
  }
  ورنہ {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    نتیجہ = hostip_resolv_آغاز(ڈیٹا, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(ڈیٹا, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحیح)نتیجہ);
  }

برآمد:
  اگر(نتیجہ && (نتیجہ != CURLE_AGAIN)) {
    Curl_dns_اندراج_unlink(ڈیٹا, pdns);
    اگر(IS_RESOLV_FAIL(نتیجہ)) {
      اگر(cache_dns && negative)
        Curl_dnscache_جوڑو_negative(ڈیٹا, dns_queries, peer);
      اگر(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(ڈیٹا, "Could not resolve: %s:%u", peer->hostname, peer->پورٹ);
    }
    ورنہ {
      failf(ڈیٹا, "Error %d resolving %s:%u",
            (صحیح)نتیجہ, peer->hostname, peer->پورٹ);
    }
  }
  ورنہ اگر(cache_dns && *pdns) {
    نتیجہ = Curl_dnscache_جوڑو(ڈیٹا, *pdns);
    اگر(نتیجہ)
      Curl_dns_اندراج_unlink(ڈیٹا, pdns);
  }

  واپس نتیجہ;
}

CURLcode Curl_resolv_blocking(ساخت Curl_easy *ڈیٹا,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورٹ,
                              uint8_t transport,
                              ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_peer *peer = تہی;
  CURLcode نتیجہ;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = تہی;

  نتیجہ = Curl_peer_create(ڈیٹا, ڈیٹا->conn->scheme, hostname, پورٹ, &peer);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* We cannot do a blocking resolve using DoH currently */
  نتیجہ = hostip_resolv(ڈیٹا, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  انتخاب(نتیجہ) {
  حالت CURLE_OK:
    DEBUGASSERT(*pdns);
    توڑو;
#اگر_تعریف_شدہ USE_CURL_ASYNC
  حالت CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    نتیجہ = Curl_async_await(ڈیٹا, resolv_id, pdns);
    Curl_resolv_destroy(ڈیٹا, resolv_id);
    توڑو;
#اگر_ختم
  طےشدہ:
    توڑو;
  }

برآمد:
  Curl_peer_unlink(&peer);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ساکن خالی alarmfunc(صحیح sig)
{
  (خالی)sig;
  siglongjmp(curl_jmpenv, 1);
}

ساکن CURLcode resolv_alarm_timeout(ساخت Curl_easy *ڈیٹا,
                                     uint8_t dns_queries,
                                     ساخت Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     ساخت Curl_dns_اندراج **اندراج_2)
{
#اگر_تعریف_شدہ HAVE_SIGACTION
  ساخت sigaction keep_sigact; /* store the old struct here */
  متلون bool keep_copysig = FALSE; /* whether old sigact has been saved */
  ساخت sigaction sigact;
#ورنہ
#اگر_تعریف_شدہ HAVE_SIGNAL
  خالی (*keep_sigact)(صحیح);       /* store the old handler here */
#اگر_ختم /* HAVE_SIGNAL */
#اگر_ختم /* HAVE_SIGACTION */
  متلون طویل timeout;
  متلون غیرعلامتی صحیح پچھلا_alarm = 0;
  CURLcode نتیجہ;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!ڈیٹا->سیٹ.no_signal);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  DEBUGASSERT(!ڈیٹا->سیٹ.doh);
#اگر_ختم

  *اندراج_2 = تہی;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (طویل)timeout_ms;
  اگر(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(ڈیٹا,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    واپس CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_تالا_تالا(&curl_jmpenv_تالا);

  اگر(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(ڈیٹا, "name lookup timed out");
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    جاؤ clean_اوپر;
  }
  ورنہ {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#اگر_تعریف_شدہ HAVE_SIGACTION
    sigaction(SIGALRM, تہی, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#اگر_تعریف_شدہ SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#اگر_ختم
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, تہی);
#ورنہ /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#اگر_تعریف_شدہ HAVE_SIGNAL
    keep_sigact = اشارہ(SIGALRM, alarmfunc);
#اگر_ختم
#اگر_ختم /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    پچھلا_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  نتیجہ = hostip_resolv(ڈیٹا, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, اندراج_2);

clean_اوپر:
  اگر(!پچھلا_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#اگر_تعریف_شدہ HAVE_SIGACTION
  اگر(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, تہی); /* put it back */
  }
#ورنہ
#اگر_تعریف_شدہ HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  اشارہ(SIGALRM, keep_sigact);
#اگر_ختم
#اگر_ختم /* HAVE_SIGACTION */

  curl_simple_تالا_unlock(&curl_jmpenv_تالا);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  اگر(پچھلا_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(ڈیٹا, ڈیٹا->conn, Curl_pgrs_now(ڈیٹا)) / 1000;

    /* the alarm period is counted in even number of seconds */
    غیرعلامتی طویل alarm_سیٹ = (غیرعلامتی طویل)(پچھلا_alarm - elapsed_secs);

    اگر(!alarm_سیٹ ||
       ((alarm_سیٹ >= 0x80000000) && (پچھلا_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      failf(ڈیٹا, "Previous alarm fired off");
    }
    ورنہ
      alarm((غیرعلامتی صحیح)alarm_سیٹ);
  }

  واپس نتیجہ;
}

#اگر_ختم /* USE_ALARM_TIMEOUT */

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
ساکن CURLcode resolv_unix(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *peer,
                            ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_addrinfo *addr;
  CURLcode نتیجہ;

  DEBUGASSERT(peer->unix_socket);
  *pdns = تہی;

  نتیجہ = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(ڈیٹا, "Unix socket path too long: '%s'", peer->hostname);
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
    واپس نتیجہ;
  }

  *pdns = Curl_dnsc_mk_addr(ڈیٹا, 0, &addr, peer);
  واپس *pdns ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}
#اگر_ختم /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(ساخت Curl_easy *ڈیٹا,
                     ساخت Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ساخت Curl_dns_اندراج **pdns)
{
  *presolv_id = 0;
  *pdns = تہی;

  اگر(timeout_ms < 0)
    /* got an already expired timeout */
    واپس CURLE_OPERATION_TIMEDOUT;
  ورنہ اگر(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  اگر((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    واپس resolv_unix(ڈیٹا, peer, pdns);
#ورنہ
  اگر(peer->unix_socket)
    واپس hostip_resolv_failed(ڈیٹا, peer, for_proxy);
#اگر_ختم

#اگر_تعریف_شدہ USE_ALARM_TIMEOUT
  اگر(dns_queries & CURL_DNSQ_ADDR) {
    اگر(timeout_ms && ڈیٹا->سیٹ.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    اگر(timeout_ms && !Curl_doh_wanted(ڈیٹا)) {
      واپس resolv_alarm_timeout(ڈیٹا, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#اگر_ختم /* !USE_ALARM_TIMEOUT */

#اگر_غیر_تعریف_شدہ CURLRES_ASYNCH
  اگر(timeout_ms)
    infof(ڈیٹا, "timeout on name lookup is not supported");
#اگر_ختم

  واپس hostip_resolv(ڈیٹا, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#اگر_تعریف_شدہ USE_CURL_ASYNC

ساخت Curl_resolv_async *Curl_async_get(ساخت Curl_easy *ڈیٹا,
                                         uint32_t resolv_id)
{
  ساخت Curl_resolv_async *async = ڈیٹا->state.async;
  برائے(; async; async = async->اگلا) {
    اگر(async->id == resolv_id)
      واپس async;
  }
  واپس تہی;
}

CURLcode Curl_resolv_take_نتیجہ(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id,
                                 ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  CURLcode نتیجہ;

  /* If async resolving is ongoing, this must be set */
  اگر(!async)
    واپس CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  نتیجہ = Curl_dnscache_get(ڈیٹا, async->dns_queries, async->peer, pdns);
  اگر(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(ڈیٹا, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(ڈیٹا, async);
    واپس CURLE_OK;
  }
  ورنہ اگر(نتیجہ) {
    Curl_async_shutdown(ڈیٹا, async);
    واپس Curl_async_failed(ڈیٹا, async, تہی);
  }

  نتیجہ = hostip_resolv_take_نتیجہ(ڈیٹا, async, pdns);

  اگر(*pdns) {
    /* Add to cache */
    نتیجہ = Curl_dnscache_جوڑو(ڈیٹا, *pdns);
    اگر(نتیجہ)
      Curl_dns_اندراج_unlink(ڈیٹا, pdns);
  }
  ورنہ اگر(IS_RESOLV_FAIL(نتیجہ)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    اگر(async->negative_answer)
      Curl_dnscache_جوڑو_negative(ڈیٹا, async->dns_queries, async->peer);
    اگر(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ڈیٹا, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->پورٹ);
  }
  ورنہ اگر(نتیجہ) {
    failf(ڈیٹا, "Error %d resolving %s:%u",
          (صحیح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  واپس نتیجہ;
}

CURLcode Curl_resolv_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  ساخت Curl_resolv_async *async = ڈیٹا->state.async;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)ps;
  برائے(; async && !نتیجہ; async = async->اگلا) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
    اگر(async->doh) /* DoH has nothing for the pollset */
      جاری;
#اگر_ختم
    نتیجہ = Curl_async_pollset(ڈیٹا, async, ps);
  }
  واپس نتیجہ;
}

خالی Curl_resolv_destroy(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id)
{
  ساخت Curl_resolv_async **panchor = &ڈیٹا->state.async;

  برائے(; *panchor; panchor = &(*panchor)->اگلا) {
    ساخت Curl_resolv_async *async = *panchor;
    اگر(async->id == resolv_id) {
      *panchor = async->اگلا;
      Curl_async_destroy(ڈیٹا, async);
      توڑو;
    }
  }
}

خالی Curl_resolv_shutdown_all(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_resolv_async *async = ڈیٹا->state.async;
  برائے(; async; async = async->اگلا) {
    Curl_async_shutdown(ڈیٹا, async);
  }
}

خالی Curl_resolv_destroy_all(ساخت Curl_easy *ڈیٹا)
{
  جب(ڈیٹا->state.async) {
    ساخت Curl_resolv_async *async = ڈیٹا->state.async;
    ڈیٹا->state.async = async->اگلا;
    Curl_async_destroy(ڈیٹا, async);
  }
}

#اگر_ختم /* USE_CURL_ASYNC */
