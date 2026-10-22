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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

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

#إذا defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#وصف_ڪريو USE_ALARM_TIMEOUT
#جيڪڏهن_پورو

#وصف_ڪريو RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#وصف_ڪريو IS_RESOLV_FAIL(نتيجو) \
  (((نتيجو) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((نتيجو) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#جيڪڏهن_وصف_ٿيل USE_IPV6
ساكن bool ipv6works(بنية Curl_easy *معلومات);
#وإلا
#وصف_ڪريو ipv6works(x) FALSE
#جيڪڏهن_پورو

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

uint8_t Curl_resolv_dns_queries(بنية Curl_easy *معلومات, uint8_t ip_نسخو)
{
  (فراغ)معلومات;
  بدل(ip_نسخو) {
  حالة CURL_IPRESOLVE_V6:
    أرجع CURL_DNSQ_AAAA;
  حالة CURL_IPRESOLVE_V4:
    أرجع CURL_DNSQ_A;
  افتراضي:
    إذا(ipv6works(معلومات))
      أرجع (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    وإلا
      أرجع CURL_DNSQ_A;
  }
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ثابت حرف *Curl_resolv_query_str(uint8_t dns_queries)
{
  بدل(dns_queries) {
  حالة (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    أرجع "A+AAAA+HTTPS";
  حالة (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    أرجع "A+AAAA";
  حالة (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    أرجع "AAAA+HTTPS";
  حالة (CURL_DNSQ_AAAA):
    أرجع "AAAA";
  حالة (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    أرجع "A+HTTPS";
  حالة (CURL_DNSQ_A):
    أرجع "A";
  حالة (CURL_DNSQ_HTTPS):
    أرجع "HTTPS";
  حالة 0:
    أرجع "-";
  افتراضي:
    DEBUGASSERT(0);
    أرجع "???";
  }
}
#جيڪڏهن_پورو

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
فراغ Curl_printable_پتو(ثابت بنية Curl_addrinfo *ai, حرف *buf,
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
#جيڪڏهن_وصف_ٿيل USE_IPV6
  حالة AF_INET6: {
    ثابت بنية sockaddr_in6 *sa6 = (ثابت فراغ *)ai->ai_addr;
    ثابت بنية in6_addr *ipaddr6 = &sa6->sin6_addr;
    (فراغ)curlx_inet_ntop(ai->ai_family, (ثابت فراغ *)ipaddr6, buf, bufsize);
    اكسر;
  }
#جيڪڏهن_پورو
  افتراضي:
    اكسر;
  }
}

#جيڪڏهن_وصف_ٿيل USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ساكن sigjmp_buf curl_jmpenv;
ساكن curl_simple_تالو curl_jmpenv_تالو = CURL_SIMPLE_تالو_INIT;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_IPV6
/* return a static IPv6 ::1 for the name */
ساكن بنية Curl_addrinfo *get_localhost6(uint16_t پورٽ, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  ثابت size_t ss_سائيز = حجم(بنية sockaddr_in6);
  ثابت size_t hostlen = لڙي_ڊيگهه(name);
  بنية sockaddr_in6 sa6;
  بنانشان حرف ipv6[16];
  بنانشان ننڍو port16 = (بنانشان ننڍو)(پورٽ & 0xffff);
  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_سائيز + hostlen + 1);
  إذا(!ca)
    أرجع عدم;

  memset(&sa6, 0, حجم(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (فراغ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  يادگيري_نقل(&sa6.sin6_addr, ipv6, حجم(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_سائيز;
  ca->ai_ايندڙ      = عدم;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  يادگيري_نقل(ca->ai_addr, &sa6, ss_سائيز);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_سائيز;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  أرجع ca;
}
#وإلا
#وصف_ڪريو get_localhost6(x, y) عدم
#جيڪڏهن_پورو

/* return a static IPv4 127.0.0.1 for the given name */
ساكن بنية Curl_addrinfo *get_localhost(uint16_t پورٽ, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  بنية Curl_addrinfo *ca6;
  ثابت size_t ss_سائيز = حجم(بنية sockaddr_in);
  ثابت size_t hostlen = لڙي_ڊيگهه(name);
  بنية sockaddr_in sa;
  بنانشان صحيح ipv4;
  بنانشان ننڍو port16 = (بنانشان ننڍو)(پورٽ & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, حجم(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  إذا(curlx_inet_pton(AF_INET, "127.0.0.1", (حرف *)&ipv4) < 1)
    أرجع عدم;
  يادگيري_نقل(&sa.sin_addr, &ipv4, حجم(ipv4));

  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_سائيز + hostlen + 1);
  إذا(!ca)
    أرجع عدم;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_سائيز;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  يادگيري_نقل(ca->ai_addr, &sa, ss_سائيز);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_سائيز;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(پورٽ, name);
  إذا(!ca6)
    أرجع ca;
  ca6->ai_ايندڙ = ca;
  أرجع ca6;
}

#جيڪڏهن_وصف_ٿيل USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(بنية Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_هلي = FALSE;
  إذا(s == CURL_SOCKET_BAD) {
    إذا(SOCKERRNO == SOCKENOMEM)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  وإلا {
    multi->ipv6_هلي = TRUE;
    sclose(s);
  }
  أرجع CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ساكن bool ipv6works(بنية Curl_easy *معلومات)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->multi);
  أرجع معلومات ? معلومات->multi->ipv6_هلي : FALSE;
}
#جيڪڏهن_پورو /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت حرف *hostname)
{
  بنية in_addr اندريون;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  بنية in6_addr in6;
#جيڪڏهن_پورو
  إذا(curlx_inet_pton(AF_INET, hostname, &اندريون) > 0
#جيڪڏهن_وصف_ٿيل USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#جيڪڏهن_پورو
    )
    أرجع TRUE;
  أرجع FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ساكن bool tailmatch(ثابت حرف *ڀريل, size_t flen,
                      ثابت حرف *part, size_t plen)
{
  إذا(plen > flen)
    أرجع FALSE;
  أرجع curl_strnequal(part, &ڀريل[flen - plen], plen);
}

ساكن CURLcode hostip_resolv_failed(بنية Curl_easy *معلومات,
                                     بنية Curl_peer *peer,
                                     bool for_proxy)
{
  failf(معلومات, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  أرجع RESOLV_FAIL(for_proxy);
}

ساكن bool can_resolve_dns_queries(بنية Curl_easy *معلومات,
                                    uint8_t dns_queries)
{
  (فراغ)معلومات;
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(معلومات))
    أرجع FALSE;
  أرجع TRUE;
}

CURLcode Curl_resolv_announce_شروعات(بنية Curl_easy *معلومات,
                                    فراغ *resolver)
{
  إذا(معلومات->سيٽ.resolver_شروعات) {
    بنية Curl_mapi_guard guard;
    صحيح rc;

    CURL_TRC_DNS(معلومات, "announcing resolve to application");
    CURL_CBAPI_شروعات(&guard, معلومات, easy_resolver_شروعات);
    rc = معلومات->سيٽ.resolver_شروعات(resolver, عدم,
                                  معلومات->سيٽ.resolver_شروعات_client);
    CURL_CBAPI_پڇاڙي(&guard);
    إذا(rc) {
      CURL_TRC_DNS(معلومات, "application aborted resolve");
      أرجع CURLE_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC

ساكن بنية Curl_resolv_async *hostip_async_new(بنية Curl_easy *معلومات,
                                                  uint8_t dns_queries,
                                                  بنية Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  بنية Curl_resolv_async *async;

  إذا(!معلومات->multi) {
    DEBUGASSERT(0);
    أرجع عدم;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, حجم(*async));
  إذا(!async)
    أرجع عدم;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  إذا(معلومات->multi->آخري_resolv_id == UINT32_MAX)
    معلومات->multi->آخري_resolv_id = 1; /* wrap around */
  وإلا
    معلومات->multi->آخري_resolv_id++;
  async->id = معلومات->multi->آخري_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->شروعات = *Curl_pgrs_now(معلومات);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  إذا(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  أرجع async;
}

ساكن CURLcode hostip_resolv_take_نتيجو(بنية Curl_easy *معلومات,
                                          بنية Curl_resolv_async *async,
                                          بنية Curl_dns_داخلا **pdns)
{
  CURLcode نتيجو;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  إذا(async->doh)
    نتيجو = Curl_doh_take_نتيجو(معلومات, async, pdns);
  وإلا
#جيڪڏهن_پورو
  نتيجو = Curl_async_take_نتيجو(معلومات, async, pdns);

  إذا(نتيجو == CURLE_AGAIN) {
    CURL_TRC_DNS(معلومات, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->پورٽ);
    نتيجو = CURLE_OK;
  }
  وإلا إذا(IS_RESOLV_FAIL(نتيجو)) {
    نتيجو = Curl_async_failed(معلومات, async, عدم);
  }
  وإلا إذا(نتيجو) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(معلومات, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحيح)نتيجو, async->peer->hostname, async->peer->پورٽ);
  }
  وإلا {
    CURL_TRC_DNS(معلومات, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->پورٽ);
    DEBUGASSERT(*pdns);
  }

  أرجع نتيجو;
}

timediff_t Curl_resolv_elapsed_ms(بنية Curl_easy *معلومات,
                                  uint32_t resolv_id,
                                  ثابت بنية curltime *pnow)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  إذا(!async)
    أرجع CURL_TIMEOUT_RESOLVE_MS;
  أرجع curlx_ptimediff_ms(pnow, &async->شروعات);
}

bool Curl_resolv_has_answers(بنية Curl_easy *معلومات,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  إذا(!async || async->پورو)
    أرجع TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  إذا((check_queries & async->dns_responses) != check_queries) {
    أرجع FALSE;
  }
  أرجع TRUE;
}

ثابت بنية Curl_addrinfo *Curl_resolv_get_ai(بنية Curl_easy *معلومات,
                                               uint32_t resolv_id,
                                               صحيح ai_family,
                                               بنانشان صحيح اشاريو)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  إذا(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    أرجع عدم;
  بدل(ai_family) {
  حالة AF_INET:
    أرجع Curl_addrinfo_get(async->ai_A, ai_family, اشاريو);
#جيڪڏهن_وصف_ٿيل USE_IPV6
  حالة AF_INET6:
    أرجع Curl_addrinfo_get(async->ai_AAAA, ai_family, اشاريو);
#جيڪڏهن_پورو
  افتراضي:
    أرجع عدم;
  }
}

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR

CURLcode Curl_resolv_https(بنية Curl_easy *معلومات,
                           بنية Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           بنية Curl_dns_داخلا **pdns)
{
  أرجع Curl_resolv(معلومات, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت بنية Curl_https_rrinfo *
Curl_resolv_get_https(بنية Curl_easy *معلومات, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async;
  إذا(!Curl_resolv_knows_https(معلومات, resolv_id))
    أرجع عدم;
  async = Curl_async_get(معلومات, resolv_id);
  أرجع async ? async->httpsrr : عدم;
}

bool Curl_resolv_knows_https(بنية Curl_easy *معلومات, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  إذا(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    أرجع ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  أرجع TRUE; /* we know it will never come */
}

#جيڪڏهن_پورو /* USE_HTTPSRR */

#جيڪڏهن_پورو /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ساكن CURLcode hostip_resolv_شروعات(بنية Curl_easy *معلومات,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    بنية Curl_dns_داخلا **pdns,
                                    bool *pnegative)
{
#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC
  بنية Curl_resolv_async *async = عدم;
#جيڪڏهن_پورو
  بنية Curl_addrinfo *addr = عدم;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode نتيجو = CURLE_OK;

  *pnegative = FALSE;

  (فراغ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

  /* Check for "known" things to resolve ourselves. */
  إذا(addr_queries) {
#جيڪڏهن_وصف_نه_ٿيل USE_RESOLVE_ON_IPS
    إذا(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      نتيجو = Curl_resolv_announce_شروعات(معلومات, عدم);
      إذا(نتيجو)
        اذهب ٻاهريون;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      نتيجو = Curl_str2addr(peer->hostname, peer->پورٽ, &addr);
      اذهب ٻاهريون;
    }
#جيڪڏهن_پورو

    hostname_len = لڙي_ڊيگهه(peer->hostname);
    إذا(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      نتيجو = Curl_resolv_announce_شروعات(معلومات, عدم);
      إذا(نتيجو)
        اذهب ٻاهريون;
      addr = get_localhost(peer->پورٽ, peer->hostname);
      إذا(!addr)
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  إذا(!Curl_is_ipaddr(peer->hostname) && allowDOH && معلومات->سيٽ.doh) {
    نتيجو = Curl_resolv_announce_شروعات(معلومات, عدم);
    إذا(نتيجو)
      اذهب ٻاهريون;
    إذا(!async) {
      async = hostip_async_new(معلومات, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      إذا(!async) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }
    نتيجو = Curl_doh(معلومات, async);
    اذهب ٻاهريون;
  }
#وإلا
  (فراغ)allowDOH;
#جيڪڏهن_پورو

  /* Can we provide the requested IP specifics in resolving? */
  إذا(!can_resolve_dns_queries(معلومات, dns_queries)) {
    نتيجو = RESOLV_FAIL(for_proxy);
    اذهب ٻاهريون;
  }

#جيڪڏهن_وصف_ٿيل CURLRES_ASYNCH
  (فراغ)addr;
  إذا(!async) {
    async = hostip_async_new(معلومات, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    إذا(!async) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }
  نتيجو = Curl_async_getaddrinfo(معلومات, async);
  إذا(نتيجو == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_نتيجو(معلومات, async, pdns);
    إذا(r2)
      نتيجو = r2;
    وإلا إذا(*pdns)
      نتيجو = CURLE_OK;
  }
#وإلا
  نتيجو = Curl_resolv_announce_شروعات(معلومات, عدم);
  إذا(نتيجو)
    اذهب ٻاهريون;
  addr = Curl_sync_getaddrinfo(معلومات, dns_queries, peer->hostname, peer->پورٽ,
                               transport);
  إذا(!addr) {
    نتيجو = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#جيڪڏهن_پورو

ٻاهريون:
  إذا(!نتيجو) {
    إذا(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(معلومات, dns_queries, &addr, peer);
      إذا(!*pdns)
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    }
    وإلا إذا(!*pdns)
      نتيجو = CURLE_AGAIN;
  }
  وإلا إذا(*pdns)
    Curl_dns_داخلا_unlink(معلومات, pdns);
  وإلا إذا(addr)
    Curl_freeaddrinfo(addr);

#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC
  إذا(async) {
    إذا(نتيجو == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->ايندڙ = معلومات->state.async;
      معلومات->state.async = async;
    }
    وإلا {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(معلومات, async);
    }
  }
#جيڪڏهن_پورو
  أرجع نتيجو;
}

ساكن CURLcode hostip_resolv(بنية Curl_easy *معلومات,
                              uint8_t dns_queries,
                              بنية Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              بنية Curl_dns_داخلا **pdns)
{
  size_t hostname_len;
  CURLcode نتيجو = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (فراغ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

#جيڪڏهن_وصف_ٿيل CURL_DISABLE_DOH
  (فراغ)allowDOH;
#جيڪڏهن_پورو

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = لڙي_ڊيگهه(peer->hostname);
  DEBUGASSERT(hostname_len);
  إذا(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(معلومات, "Not resolving .onion address (RFC 7686)");
    اذهب ٻاهريون;
  }

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  CURL_TRC_DNS(معلومات, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->پورٽ);
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(معلومات, "DEBUG fail ipv6 resolve");
    نتيجو = hostip_resolv_failed(معلومات, peer, for_proxy);
    اذهب ٻاهريون;
  }
#جيڪڏهن_پورو
  /* Let's check our DNS cache first */
  نتيجو = Curl_dnscache_get(معلومات, dns_queries, peer, pdns);
  إذا(*pdns) {
    infof(معلومات, "Hostname %s was found in DNS cache", peer->hostname);
    نتيجو = CURLE_OK;
  }
  وإلا إذا(نتيجو) {
    infof(معلومات, "Negative DNS entry");
    نتيجو = hostip_resolv_failed(معلومات, peer, for_proxy);
  }
  وإلا {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    نتيجو = hostip_resolv_شروعات(معلومات, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(معلومات, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحيح)نتيجو);
  }

ٻاهريون:
  إذا(نتيجو && (نتيجو != CURLE_AGAIN)) {
    Curl_dns_داخلا_unlink(معلومات, pdns);
    إذا(IS_RESOLV_FAIL(نتيجو)) {
      إذا(cache_dns && negative)
        Curl_dnscache_جوڙيو_negative(معلومات, dns_queries, peer);
      إذا(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(معلومات, "Could not resolve: %s:%u", peer->hostname, peer->پورٽ);
    }
    وإلا {
      failf(معلومات, "Error %d resolving %s:%u",
            (صحيح)نتيجو, peer->hostname, peer->پورٽ);
    }
  }
  وإلا إذا(cache_dns && *pdns) {
    نتيجو = Curl_dnscache_جوڙيو(معلومات, *pdns);
    إذا(نتيجو)
      Curl_dns_داخلا_unlink(معلومات, pdns);
  }

  أرجع نتيجو;
}

CURLcode Curl_resolv_blocking(بنية Curl_easy *معلومات,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورٽ,
                              uint8_t transport,
                              بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_peer *peer = عدم;
  CURLcode نتيجو;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = عدم;

  نتيجو = Curl_peer_create(معلومات, معلومات->conn->scheme, hostname, پورٽ, &peer);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* We cannot do a blocking resolve using DoH currently */
  نتيجو = hostip_resolv(معلومات, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  بدل(نتيجو) {
  حالة CURLE_OK:
    DEBUGASSERT(*pdns);
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC
  حالة CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    نتيجو = Curl_async_await(معلومات, resolv_id, pdns);
    Curl_resolv_destroy(معلومات, resolv_id);
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    اكسر;
  }

ٻاهريون:
  Curl_peer_unlink(&peer);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل USE_ALARM_TIMEOUT
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

ساكن CURLcode resolv_alarm_timeout(بنية Curl_easy *معلومات,
                                     uint8_t dns_queries,
                                     بنية Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     بنية Curl_dns_داخلا **داخلا_2)
{
#جيڪڏهن_وصف_ٿيل HAVE_SIGACTION
  بنية sigaction keep_sigact; /* store the old struct here */
  متطاير bool keep_copysig = FALSE; /* whether old sigact has been saved */
  بنية sigaction sigact;
#وإلا
#جيڪڏهن_وصف_ٿيل HAVE_SIGNAL
  فراغ (*keep_sigact)(صحيح);       /* store the old handler here */
#جيڪڏهن_پورو /* HAVE_SIGNAL */
#جيڪڏهن_پورو /* HAVE_SIGACTION */
  متطاير ڊگهو timeout;
  متطاير بنانشان صحيح prev_alarm = 0;
  CURLcode نتيجو;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!معلومات->سيٽ.no_signal);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  DEBUGASSERT(!معلومات->سيٽ.doh);
#جيڪڏهن_پورو

  *داخلا_2 = عدم;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (ڊگهو)timeout_ms;
  إذا(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(معلومات,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_تالو_تالو(&curl_jmpenv_تالو);

  إذا(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(معلومات, "name lookup timed out");
    نتيجو = CURLE_OPERATION_TIMEDOUT;
    اذهب clean_مٿي;
  }
  وإلا {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#جيڪڏهن_وصف_ٿيل HAVE_SIGACTION
    sigaction(SIGALRM, عدم, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#جيڪڏهن_وصف_ٿيل SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#جيڪڏهن_پورو
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, عدم);
#وإلا /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#جيڪڏهن_وصف_ٿيل HAVE_SIGNAL
    keep_sigact = إشارة(SIGALRM, alarmfunc);
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  نتيجو = hostip_resolv(معلومات, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, داخلا_2);

clean_مٿي:
  إذا(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#جيڪڏهن_وصف_ٿيل HAVE_SIGACTION
  إذا(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, عدم); /* put it back */
  }
#وإلا
#جيڪڏهن_وصف_ٿيل HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  إشارة(SIGALRM, keep_sigact);
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* HAVE_SIGACTION */

  curl_simple_تالو_unlock(&curl_jmpenv_تالو);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  إذا(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(معلومات, معلومات->conn, Curl_pgrs_now(معلومات)) / 1000;

    /* the alarm period is counted in even number of seconds */
    بنانشان ڊگهو alarm_سيٽ = (بنانشان ڊگهو)(prev_alarm - elapsed_secs);

    إذا(!alarm_سيٽ ||
       ((alarm_سيٽ >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      failf(معلومات, "Previous alarm fired off");
    }
    وإلا
      alarm((بنانشان صحيح)alarm_سيٽ);
  }

  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_ALARM_TIMEOUT */

#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
ساكن CURLcode resolv_unix(بنية Curl_easy *معلومات,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_addrinfo *addr;
  CURLcode نتيجو;

  DEBUGASSERT(peer->unix_socket);
  *pdns = عدم;

  نتيجو = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(معلومات, "Unix socket path too long: '%s'", peer->hostname);
      نتيجو = CURLE_COULDNT_RESOLVE_HOST;
    }
    أرجع نتيجو;
  }

  *pdns = Curl_dnsc_mk_addr(معلومات, 0, &addr, peer);
  أرجع *pdns ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}
#جيڪڏهن_پورو /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(بنية Curl_easy *معلومات,
                     بنية Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     بنية Curl_dns_داخلا **pdns)
{
  *presolv_id = 0;
  *pdns = عدم;

  إذا(timeout_ms < 0)
    /* got an already expired timeout */
    أرجع CURLE_OPERATION_TIMEDOUT;
  وإلا إذا(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  إذا((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    أرجع resolv_unix(معلومات, peer, pdns);
#وإلا
  إذا(peer->unix_socket)
    أرجع hostip_resolv_failed(معلومات, peer, for_proxy);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_ALARM_TIMEOUT
  إذا(dns_queries & CURL_DNSQ_ADDR) {
    إذا(timeout_ms && معلومات->سيٽ.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    إذا(timeout_ms && !Curl_doh_wanted(معلومات)) {
      أرجع resolv_alarm_timeout(معلومات, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#جيڪڏهن_پورو /* !USE_ALARM_TIMEOUT */

#جيڪڏهن_وصف_نه_ٿيل CURLRES_ASYNCH
  إذا(timeout_ms)
    infof(معلومات, "timeout on name lookup is not supported");
#جيڪڏهن_پورو

  أرجع hostip_resolv(معلومات, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#جيڪڏهن_وصف_ٿيل USE_CURL_ASYNC

بنية Curl_resolv_async *Curl_async_get(بنية Curl_easy *معلومات,
                                         uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = معلومات->state.async;
  لكل(; async; async = async->ايندڙ) {
    إذا(async->id == resolv_id)
      أرجع async;
  }
  أرجع عدم;
}

CURLcode Curl_resolv_take_نتيجو(بنية Curl_easy *معلومات, uint32_t resolv_id,
                                 بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  CURLcode نتيجو;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  نتيجو = Curl_dnscache_get(معلومات, async->dns_queries, async->peer, pdns);
  إذا(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(معلومات, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(معلومات, async);
    أرجع CURLE_OK;
  }
  وإلا إذا(نتيجو) {
    Curl_async_shutdown(معلومات, async);
    أرجع Curl_async_failed(معلومات, async, عدم);
  }

  نتيجو = hostip_resolv_take_نتيجو(معلومات, async, pdns);

  إذا(*pdns) {
    /* Add to cache */
    نتيجو = Curl_dnscache_جوڙيو(معلومات, *pdns);
    إذا(نتيجو)
      Curl_dns_داخلا_unlink(معلومات, pdns);
  }
  وإلا إذا(IS_RESOLV_FAIL(نتيجو)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    إذا(async->negative_answer)
      Curl_dnscache_جوڙيو_negative(معلومات, async->dns_queries, async->peer);
    إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(معلومات, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->پورٽ);
  }
  وإلا إذا(نتيجو) {
    failf(معلومات, "Error %d resolving %s:%u",
          (صحيح)نتيجو, async->peer->hostname, async->peer->پورٽ);
  }
  أرجع نتيجو;
}

CURLcode Curl_resolv_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  بنية Curl_resolv_async *async = معلومات->state.async;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)ps;
  لكل(; async && !نتيجو; async = async->ايندڙ) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
    إذا(async->doh) /* DoH has nothing for the pollset */
      استمر;
#جيڪڏهن_پورو
    نتيجو = Curl_async_pollset(معلومات, async, ps);
  }
  أرجع نتيجو;
}

فراغ Curl_resolv_destroy(بنية Curl_easy *معلومات, uint32_t resolv_id)
{
  بنية Curl_resolv_async **panchor = &معلومات->state.async;

  لكل(; *panchor; panchor = &(*panchor)->ايندڙ) {
    بنية Curl_resolv_async *async = *panchor;
    إذا(async->id == resolv_id) {
      *panchor = async->ايندڙ;
      Curl_async_destroy(معلومات, async);
      اكسر;
    }
  }
}

فراغ Curl_resolv_shutdown_all(بنية Curl_easy *معلومات)
{
  بنية Curl_resolv_async *async = معلومات->state.async;
  لكل(; async; async = async->ايندڙ) {
    Curl_async_shutdown(معلومات, async);
  }
}

فراغ Curl_resolv_destroy_all(بنية Curl_easy *معلومات)
{
  طالما(معلومات->state.async) {
    بنية Curl_resolv_async *async = معلومات->state.async;
    معلومات->state.async = async->ايندڙ;
    Curl_async_destroy(معلومات, async);
  }
}

#جيڪڏهن_پورو /* USE_CURL_ASYNC */
