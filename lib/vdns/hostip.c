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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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
#تعریف_کرو USE_ALARM_TIMEOUT
#جے_انت

#تعریف_کرو RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#تعریف_کرو IS_RESOLV_FAIL(نتیجہ) \
  (((نتیجہ) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((نتیجہ) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#جے_تعریف_شدہ USE_IPV6
ساكن bool ipv6works(بنية Curl_easy *انکڑے);
#وإلا
#تعریف_کرو ipv6works(x) FALSE
#جے_انت

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

uint8_t Curl_resolv_dns_queries(بنية Curl_easy *انکڑے, uint8_t ip_نسخہ)
{
  (فراغ)انکڑے;
  بدل(ip_نسخہ) {
  حالة CURL_IPRESOLVE_V6:
    أرجع CURL_DNSQ_AAAA;
  حالة CURL_IPRESOLVE_V4:
    أرجع CURL_DNSQ_A;
  افتراضي:
    إذا(ipv6works(انکڑے))
      أرجع (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    وإلا
      أرجع CURL_DNSQ_A;
  }
}

#جے_تعریف_شدہ CURLVERBOSE
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
#جے_انت

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
فراغ Curl_printable_پتہ(ثابت بنية Curl_addrinfo *ai, حرف *buf,
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
#جے_تعریف_شدہ USE_IPV6
  حالة AF_INET6: {
    ثابت بنية sockaddr_in6 *sa6 = (ثابت فراغ *)ai->ai_addr;
    ثابت بنية in6_addr *ipaddr6 = &sa6->sin6_addr;
    (فراغ)curlx_inet_ntop(ai->ai_family, (ثابت فراغ *)ipaddr6, buf, bufsize);
    اكسر;
  }
#جے_انت
  افتراضي:
    اكسر;
  }
}

#جے_تعریف_شدہ USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ساكن sigjmp_buf curl_jmpenv;
ساكن curl_simple_جندرا curl_jmpenv_جندرا = CURL_SIMPLE_جندرا_INIT;
#جے_انت

#جے_تعریف_شدہ USE_IPV6
/* return a static IPv6 ::1 for the name */
ساكن بنية Curl_addrinfo *get_localhost6(uint16_t پورٹ, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  ثابت size_t ss_ناپ = حجم(بنية sockaddr_in6);
  ثابت size_t hostlen = لڑی_لمبائی(name);
  بنية sockaddr_in6 sa6;
  بنانشانی حرف ipv6[16];
  بنانشانی چھوٹا port16 = (بنانشانی چھوٹا)(پورٹ & 0xffff);
  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_ناپ + hostlen + 1);
  إذا(!ca)
    أرجع عدم;

  memset(&sa6, 0, حجم(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (فراغ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  حافظہ_نقل(&sa6.sin6_addr, ipv6, حجم(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ناپ;
  ca->ai_اگلا      = عدم;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  حافظہ_نقل(ca->ai_addr, &sa6, ss_ناپ);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_ناپ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  أرجع ca;
}
#وإلا
#تعریف_کرو get_localhost6(x, y) عدم
#جے_انت

/* return a static IPv4 127.0.0.1 for the given name */
ساكن بنية Curl_addrinfo *get_localhost(uint16_t پورٹ, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  بنية Curl_addrinfo *ca6;
  ثابت size_t ss_ناپ = حجم(بنية sockaddr_in);
  ثابت size_t hostlen = لڑی_لمبائی(name);
  بنية sockaddr_in sa;
  بنانشانی صحيح ipv4;
  بنانشانی چھوٹا port16 = (بنانشانی چھوٹا)(پورٹ & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, حجم(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  إذا(curlx_inet_pton(AF_INET, "127.0.0.1", (حرف *)&ipv4) < 1)
    أرجع عدم;
  حافظہ_نقل(&sa.sin_addr, &ipv4, حجم(ipv4));

  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_ناپ + hostlen + 1);
  إذا(!ca)
    أرجع عدم;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ناپ;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  حافظہ_نقل(ca->ai_addr, &sa, ss_ناپ);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_ناپ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(پورٹ, name);
  إذا(!ca6)
    أرجع ca;
  ca6->ai_اگلا = ca;
  أرجع ca6;
}

#جے_تعریف_شدہ USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(بنية Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_چلدا = FALSE;
  إذا(s == CURL_SOCKET_BAD) {
    إذا(SOCKERRNO == SOCKENOMEM)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }
  وإلا {
    multi->ipv6_چلدا = TRUE;
    sclose(s);
  }
  أرجع CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ساكن bool ipv6works(بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->multi);
  أرجع انکڑے ? انکڑے->multi->ipv6_چلدا : FALSE;
}
#جے_انت /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت حرف *hostname)
{
  بنية in_addr وچ;
#جے_تعریف_شدہ USE_IPV6
  بنية in6_addr in6;
#جے_انت
  إذا(curlx_inet_pton(AF_INET, hostname, &وچ) > 0
#جے_تعریف_شدہ USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#جے_انت
    )
    أرجع TRUE;
  أرجع FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ساكن bool tailmatch(ثابت حرف *بھریا, size_t flen,
                      ثابت حرف *part, size_t plen)
{
  إذا(plen > flen)
    أرجع FALSE;
  أرجع curl_strnequal(part, &بھریا[flen - plen], plen);
}

ساكن CURLcode hostip_resolv_failed(بنية Curl_easy *انکڑے,
                                     بنية Curl_peer *peer,
                                     bool for_proxy)
{
  failf(انکڑے, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  أرجع RESOLV_FAIL(for_proxy);
}

ساكن bool can_resolve_dns_queries(بنية Curl_easy *انکڑے,
                                    uint8_t dns_queries)
{
  (فراغ)انکڑے;
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(انکڑے))
    أرجع FALSE;
  أرجع TRUE;
}

CURLcode Curl_resolv_announce_شروع(بنية Curl_easy *انکڑے,
                                    فراغ *resolver)
{
  إذا(انکڑے->سیٹ.resolver_شروع) {
    بنية Curl_mapi_guard guard;
    صحيح rc;

    CURL_TRC_DNS(انکڑے, "announcing resolve to application");
    CURL_CBAPI_شروع(&guard, انکڑے, easy_resolver_شروع);
    rc = انکڑے->سیٹ.resolver_شروع(resolver, عدم,
                                  انکڑے->سیٹ.resolver_شروع_client);
    CURL_CBAPI_سرا(&guard);
    إذا(rc) {
      CURL_TRC_DNS(انکڑے, "application aborted resolve");
      أرجع CURLE_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLE_OK;
}

#جے_تعریف_شدہ USE_CURL_ASYNC

ساكن بنية Curl_resolv_async *hostip_async_new(بنية Curl_easy *انکڑے,
                                                  uint8_t dns_queries,
                                                  بنية Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  بنية Curl_resolv_async *async;

  إذا(!انکڑے->multi) {
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
  إذا(انکڑے->multi->آخری_resolv_id == UINT32_MAX)
    انکڑے->multi->آخری_resolv_id = 1; /* wrap around */
  وإلا
    انکڑے->multi->آخری_resolv_id++;
  async->id = انکڑے->multi->آخری_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->شروع = *Curl_pgrs_now(انکڑے);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  إذا(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  أرجع async;
}

ساكن CURLcode hostip_resolv_take_نتیجہ(بنية Curl_easy *انکڑے,
                                          بنية Curl_resolv_async *async,
                                          بنية Curl_dns_اندراج **pdns)
{
  CURLcode نتیجہ;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

#جے_بنا_تعریف CURL_DISABLE_DOH
  إذا(async->doh)
    نتیجہ = Curl_doh_take_نتیجہ(انکڑے, async, pdns);
  وإلا
#جے_انت
  نتیجہ = Curl_async_take_نتیجہ(انکڑے, async, pdns);

  إذا(نتیجہ == CURLE_AGAIN) {
    CURL_TRC_DNS(انکڑے, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->پورٹ);
    نتیجہ = CURLE_OK;
  }
  وإلا إذا(IS_RESOLV_FAIL(نتیجہ)) {
    نتیجہ = Curl_async_failed(انکڑے, async, عدم);
  }
  وإلا إذا(نتیجہ) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(انکڑے, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحيح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  وإلا {
    CURL_TRC_DNS(انکڑے, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->پورٹ);
    DEBUGASSERT(*pdns);
  }

  أرجع نتیجہ;
}

timediff_t Curl_resolv_elapsed_ms(بنية Curl_easy *انکڑے,
                                  uint32_t resolv_id,
                                  ثابت بنية curltime *pnow)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  إذا(!async)
    أرجع CURL_TIMEOUT_RESOLVE_MS;
  أرجع curlx_ptimediff_ms(pnow, &async->شروع);
}

bool Curl_resolv_has_answers(بنية Curl_easy *انکڑے,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  إذا(!async || async->پورا)
    أرجع TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  إذا((check_queries & async->dns_responses) != check_queries) {
    أرجع FALSE;
  }
  أرجع TRUE;
}

ثابت بنية Curl_addrinfo *Curl_resolv_get_ai(بنية Curl_easy *انکڑے,
                                               uint32_t resolv_id,
                                               صحيح ai_family,
                                               بنانشانی صحيح اشاریہ)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  إذا(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    أرجع عدم;
  بدل(ai_family) {
  حالة AF_INET:
    أرجع Curl_addrinfo_get(async->ai_A, ai_family, اشاریہ);
#جے_تعریف_شدہ USE_IPV6
  حالة AF_INET6:
    أرجع Curl_addrinfo_get(async->ai_AAAA, ai_family, اشاریہ);
#جے_انت
  افتراضي:
    أرجع عدم;
  }
}

#جے_تعریف_شدہ USE_HTTPSRR

CURLcode Curl_resolv_https(بنية Curl_easy *انکڑے,
                           بنية Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           بنية Curl_dns_اندراج **pdns)
{
  أرجع Curl_resolv(انکڑے, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت بنية Curl_https_rrinfo *
Curl_resolv_get_https(بنية Curl_easy *انکڑے, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async;
  إذا(!Curl_resolv_knows_https(انکڑے, resolv_id))
    أرجع عدم;
  async = Curl_async_get(انکڑے, resolv_id);
  أرجع async ? async->httpsrr : عدم;
}

bool Curl_resolv_knows_https(بنية Curl_easy *انکڑے, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  إذا(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    أرجع ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  أرجع TRUE; /* we know it will never come */
}

#جے_انت /* USE_HTTPSRR */

#جے_انت /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ساكن CURLcode hostip_resolv_شروع(بنية Curl_easy *انکڑے,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    بنية Curl_dns_اندراج **pdns,
                                    bool *pnegative)
{
#جے_تعریف_شدہ USE_CURL_ASYNC
  بنية Curl_resolv_async *async = عدم;
#جے_انت
  بنية Curl_addrinfo *addr = عدم;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode نتیجہ = CURLE_OK;

  *pnegative = FALSE;

  (فراغ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

  /* Check for "known" things to resolve ourselves. */
  إذا(addr_queries) {
#جے_بنا_تعریف USE_RESOLVE_ON_IPS
    إذا(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      نتیجہ = Curl_resolv_announce_شروع(انکڑے, عدم);
      إذا(نتیجہ)
        اذهب باہرلا;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      نتیجہ = Curl_str2addr(peer->hostname, peer->پورٹ, &addr);
      اذهب باہرلا;
    }
#جے_انت

    hostname_len = لڑی_لمبائی(peer->hostname);
    إذا(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      نتیجہ = Curl_resolv_announce_شروع(انکڑے, عدم);
      إذا(نتیجہ)
        اذهب باہرلا;
      addr = get_localhost(peer->پورٹ, peer->hostname);
      إذا(!addr)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }
#جے_بنا_تعریف CURL_DISABLE_DOH
  إذا(!Curl_is_ipaddr(peer->hostname) && allowDOH && انکڑے->سیٹ.doh) {
    نتیجہ = Curl_resolv_announce_شروع(انکڑے, عدم);
    إذا(نتیجہ)
      اذهب باہرلا;
    إذا(!async) {
      async = hostip_async_new(انکڑے, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      إذا(!async) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }
    نتیجہ = Curl_doh(انکڑے, async);
    اذهب باہرلا;
  }
#وإلا
  (فراغ)allowDOH;
#جے_انت

  /* Can we provide the requested IP specifics in resolving? */
  إذا(!can_resolve_dns_queries(انکڑے, dns_queries)) {
    نتیجہ = RESOLV_FAIL(for_proxy);
    اذهب باہرلا;
  }

#جے_تعریف_شدہ CURLRES_ASYNCH
  (فراغ)addr;
  إذا(!async) {
    async = hostip_async_new(انکڑے, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    إذا(!async) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }
  نتیجہ = Curl_async_getaddrinfo(انکڑے, async);
  إذا(نتیجہ == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_نتیجہ(انکڑے, async, pdns);
    إذا(r2)
      نتیجہ = r2;
    وإلا إذا(*pdns)
      نتیجہ = CURLE_OK;
  }
#وإلا
  نتیجہ = Curl_resolv_announce_شروع(انکڑے, عدم);
  إذا(نتیجہ)
    اذهب باہرلا;
  addr = Curl_sync_getaddrinfo(انکڑے, dns_queries, peer->hostname, peer->پورٹ,
                               transport);
  إذا(!addr) {
    نتیجہ = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#جے_انت

باہرلا:
  إذا(!نتیجہ) {
    إذا(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(انکڑے, dns_queries, &addr, peer);
      إذا(!*pdns)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
    }
    وإلا إذا(!*pdns)
      نتیجہ = CURLE_AGAIN;
  }
  وإلا إذا(*pdns)
    Curl_dns_اندراج_unlink(انکڑے, pdns);
  وإلا إذا(addr)
    Curl_freeaddrinfo(addr);

#جے_تعریف_شدہ USE_CURL_ASYNC
  إذا(async) {
    إذا(نتیجہ == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->اگلا = انکڑے->state.async;
      انکڑے->state.async = async;
    }
    وإلا {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(انکڑے, async);
    }
  }
#جے_انت
  أرجع نتیجہ;
}

ساكن CURLcode hostip_resolv(بنية Curl_easy *انکڑے,
                              uint8_t dns_queries,
                              بنية Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              بنية Curl_dns_اندراج **pdns)
{
  size_t hostname_len;
  CURLcode نتیجہ = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (فراغ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

#جے_تعریف_شدہ CURL_DISABLE_DOH
  (فراغ)allowDOH;
#جے_انت

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = لڑی_لمبائی(peer->hostname);
  DEBUGASSERT(hostname_len);
  إذا(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(انکڑے, "Not resolving .onion address (RFC 7686)");
    اذهب باہرلا;
  }

#جے_تعریف_شدہ DEBUGBUILD
  CURL_TRC_DNS(انکڑے, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->پورٹ);
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(انکڑے, "DEBUG fail ipv6 resolve");
    نتیجہ = hostip_resolv_failed(انکڑے, peer, for_proxy);
    اذهب باہرلا;
  }
#جے_انت
  /* Let's check our DNS cache first */
  نتیجہ = Curl_dnscache_get(انکڑے, dns_queries, peer, pdns);
  إذا(*pdns) {
    infof(انکڑے, "Hostname %s was found in DNS cache", peer->hostname);
    نتیجہ = CURLE_OK;
  }
  وإلا إذا(نتیجہ) {
    infof(انکڑے, "Negative DNS entry");
    نتیجہ = hostip_resolv_failed(انکڑے, peer, for_proxy);
  }
  وإلا {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    نتیجہ = hostip_resolv_شروع(انکڑے, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(انکڑے, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحيح)نتیجہ);
  }

باہرلا:
  إذا(نتیجہ && (نتیجہ != CURLE_AGAIN)) {
    Curl_dns_اندراج_unlink(انکڑے, pdns);
    إذا(IS_RESOLV_FAIL(نتیجہ)) {
      إذا(cache_dns && negative)
        Curl_dnscache_جوڑو_negative(انکڑے, dns_queries, peer);
      إذا(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(انکڑے, "Could not resolve: %s:%u", peer->hostname, peer->پورٹ);
    }
    وإلا {
      failf(انکڑے, "Error %d resolving %s:%u",
            (صحيح)نتیجہ, peer->hostname, peer->پورٹ);
    }
  }
  وإلا إذا(cache_dns && *pdns) {
    نتیجہ = Curl_dnscache_جوڑو(انکڑے, *pdns);
    إذا(نتیجہ)
      Curl_dns_اندراج_unlink(انکڑے, pdns);
  }

  أرجع نتیجہ;
}

CURLcode Curl_resolv_blocking(بنية Curl_easy *انکڑے,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورٹ,
                              uint8_t transport,
                              بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_peer *peer = عدم;
  CURLcode نتیجہ;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = عدم;

  نتیجہ = Curl_peer_create(انکڑے, انکڑے->conn->scheme, hostname, پورٹ, &peer);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* We cannot do a blocking resolve using DoH currently */
  نتیجہ = hostip_resolv(انکڑے, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  بدل(نتیجہ) {
  حالة CURLE_OK:
    DEBUGASSERT(*pdns);
    اكسر;
#جے_تعریف_شدہ USE_CURL_ASYNC
  حالة CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    نتیجہ = Curl_async_await(انکڑے, resolv_id, pdns);
    Curl_resolv_destroy(انکڑے, resolv_id);
    اكسر;
#جے_انت
  افتراضي:
    اكسر;
  }

باہرلا:
  Curl_peer_unlink(&peer);
  أرجع نتیجہ;
}

#جے_تعریف_شدہ USE_ALARM_TIMEOUT
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

ساكن CURLcode resolv_alarm_timeout(بنية Curl_easy *انکڑے,
                                     uint8_t dns_queries,
                                     بنية Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     بنية Curl_dns_اندراج **اندراج)
{
#جے_تعریف_شدہ HAVE_SIGACTION
  بنية sigaction keep_sigact; /* store the old struct here */
  متطاير bool keep_copysig = FALSE; /* whether old sigact has been saved */
  بنية sigaction sigact;
#وإلا
#جے_تعریف_شدہ HAVE_SIGNAL
  فراغ (*keep_sigact)(صحيح);       /* store the old handler here */
#جے_انت /* HAVE_SIGNAL */
#جے_انت /* HAVE_SIGACTION */
  متطاير لما timeout;
  متطاير بنانشانی صحيح prev_alarm = 0;
  CURLcode نتیجہ;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!انکڑے->سیٹ.no_signal);
#جے_بنا_تعریف CURL_DISABLE_DOH
  DEBUGASSERT(!انکڑے->سیٹ.doh);
#جے_انت

  *اندراج = عدم;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (لما)timeout_ms;
  إذا(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(انکڑے,
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
  curl_simple_جندرا_جندرا(&curl_jmpenv_جندرا);

  إذا(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(انکڑے, "name lookup timed out");
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    اذهب clean_اتے;
  }
  وإلا {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#جے_تعریف_شدہ HAVE_SIGACTION
    sigaction(SIGALRM, عدم, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#جے_تعریف_شدہ SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#جے_انت
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, عدم);
#وإلا /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#جے_تعریف_شدہ HAVE_SIGNAL
    keep_sigact = إشارة(SIGALRM, alarmfunc);
#جے_انت
#جے_انت /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  نتیجہ = hostip_resolv(انکڑے, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, اندراج);

clean_اتے:
  إذا(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#جے_تعریف_شدہ HAVE_SIGACTION
  إذا(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, عدم); /* put it back */
  }
#وإلا
#جے_تعریف_شدہ HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  إشارة(SIGALRM, keep_sigact);
#جے_انت
#جے_انت /* HAVE_SIGACTION */

  curl_simple_جندرا_unlock(&curl_jmpenv_جندرا);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  إذا(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(انکڑے, انکڑے->conn, Curl_pgrs_now(انکڑے)) / 1000;

    /* the alarm period is counted in even number of seconds */
    بنانشانی لما alarm_سیٹ = (بنانشانی لما)(prev_alarm - elapsed_secs);

    إذا(!alarm_سیٹ ||
       ((alarm_سیٹ >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      failf(انکڑے, "Previous alarm fired off");
    }
    وإلا
      alarm((بنانشانی صحيح)alarm_سیٹ);
  }

  أرجع نتیجہ;
}

#جے_انت /* USE_ALARM_TIMEOUT */

#جے_تعریف_شدہ USE_UNIX_SOCKETS
ساكن CURLcode resolv_unix(بنية Curl_easy *انکڑے,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_addrinfo *addr;
  CURLcode نتیجہ;

  DEBUGASSERT(peer->unix_socket);
  *pdns = عدم;

  نتیجہ = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(انکڑے, "Unix socket path too long: '%s'", peer->hostname);
      نتیجہ = CURLE_COULDNT_RESOLVE_HOST;
    }
    أرجع نتیجہ;
  }

  *pdns = Curl_dnsc_mk_addr(انکڑے, 0, &addr, peer);
  أرجع *pdns ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
}
#جے_انت /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(بنية Curl_easy *انکڑے,
                     بنية Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     بنية Curl_dns_اندراج **pdns)
{
  *presolv_id = 0;
  *pdns = عدم;

  إذا(timeout_ms < 0)
    /* got an already expired timeout */
    أرجع CURLE_OPERATION_TIMEDOUT;
  وإلا إذا(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#جے_تعریف_شدہ USE_UNIX_SOCKETS
  إذا((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    أرجع resolv_unix(انکڑے, peer, pdns);
#وإلا
  إذا(peer->unix_socket)
    أرجع hostip_resolv_failed(انکڑے, peer, for_proxy);
#جے_انت

#جے_تعریف_شدہ USE_ALARM_TIMEOUT
  إذا(dns_queries & CURL_DNSQ_ADDR) {
    إذا(timeout_ms && انکڑے->سیٹ.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    إذا(timeout_ms && !Curl_doh_wanted(انکڑے)) {
      أرجع resolv_alarm_timeout(انکڑے, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#جے_انت /* !USE_ALARM_TIMEOUT */

#جے_بنا_تعریف CURLRES_ASYNCH
  إذا(timeout_ms)
    infof(انکڑے, "timeout on name lookup is not supported");
#جے_انت

  أرجع hostip_resolv(انکڑے, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#جے_تعریف_شدہ USE_CURL_ASYNC

بنية Curl_resolv_async *Curl_async_get(بنية Curl_easy *انکڑے,
                                         uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = انکڑے->state.async;
  لكل(; async; async = async->اگلا) {
    إذا(async->id == resolv_id)
      أرجع async;
  }
  أرجع عدم;
}

CURLcode Curl_resolv_take_نتیجہ(بنية Curl_easy *انکڑے, uint32_t resolv_id,
                                 بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  CURLcode نتیجہ;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  نتیجہ = Curl_dnscache_get(انکڑے, async->dns_queries, async->peer, pdns);
  إذا(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(انکڑے, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(انکڑے, async);
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجہ) {
    Curl_async_shutdown(انکڑے, async);
    أرجع Curl_async_failed(انکڑے, async, عدم);
  }

  نتیجہ = hostip_resolv_take_نتیجہ(انکڑے, async, pdns);

  إذا(*pdns) {
    /* Add to cache */
    نتیجہ = Curl_dnscache_جوڑو(انکڑے, *pdns);
    إذا(نتیجہ)
      Curl_dns_اندراج_unlink(انکڑے, pdns);
  }
  وإلا إذا(IS_RESOLV_FAIL(نتیجہ)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    إذا(async->negative_answer)
      Curl_dnscache_جوڑو_negative(انکڑے, async->dns_queries, async->peer);
    إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(انکڑے, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->پورٹ);
  }
  وإلا إذا(نتیجہ) {
    failf(انکڑے, "Error %d resolving %s:%u",
          (صحيح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  أرجع نتیجہ;
}

CURLcode Curl_resolv_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  بنية Curl_resolv_async *async = انکڑے->state.async;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)ps;
  لكل(; async && !نتیجہ; async = async->اگلا) {
#جے_بنا_تعریف CURL_DISABLE_DOH
    إذا(async->doh) /* DoH has nothing for the pollset */
      استمر;
#جے_انت
    نتیجہ = Curl_async_pollset(انکڑے, async, ps);
  }
  أرجع نتیجہ;
}

فراغ Curl_resolv_destroy(بنية Curl_easy *انکڑے, uint32_t resolv_id)
{
  بنية Curl_resolv_async **panchor = &انکڑے->state.async;

  لكل(; *panchor; panchor = &(*panchor)->اگلا) {
    بنية Curl_resolv_async *async = *panchor;
    إذا(async->id == resolv_id) {
      *panchor = async->اگلا;
      Curl_async_destroy(انکڑے, async);
      اكسر;
    }
  }
}

فراغ Curl_resolv_shutdown_all(بنية Curl_easy *انکڑے)
{
  بنية Curl_resolv_async *async = انکڑے->state.async;
  لكل(; async; async = async->اگلا) {
    Curl_async_shutdown(انکڑے, async);
  }
}

فراغ Curl_resolv_destroy_all(بنية Curl_easy *انکڑے)
{
  طالما(انکڑے->state.async) {
    بنية Curl_resolv_async *async = انکڑے->state.async;
    انکڑے->state.async = async->اگلا;
    Curl_async_destroy(انکڑے, async);
  }
}

#جے_انت /* USE_CURL_ASYNC */
