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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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
#تعریف_کن USE_ALARM_TIMEOUT
#پایان_اگر

#تعریف_کن RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#تعریف_کن IS_RESOLV_FAIL(نتیجه) \
  (((نتیجه) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((نتیجه) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#اگر_تعریف_شده USE_IPV6
ساكن bool ipv6works(بنية Curl_easy *داده);
#وإلا
#تعریف_کن ipv6works(x) FALSE
#پایان_اگر

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

uint8_t Curl_resolv_dns_queries(بنية Curl_easy *داده, uint8_t ip_نسخه)
{
  (فراغ)داده;
  بدل(ip_نسخه) {
  حالة CURL_IPRESOLVE_V6:
    أرجع CURL_DNSQ_AAAA;
  حالة CURL_IPRESOLVE_V4:
    أرجع CURL_DNSQ_A;
  افتراضي:
    إذا(ipv6works(داده))
      أرجع (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    وإلا
      أرجع CURL_DNSQ_A;
  }
}

#اگر_تعریف_شده CURLVERBOSE
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
#پایان_اگر

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
فراغ Curl_printable_آدرس(ثابت بنية Curl_addrinfo *ai, حرف *buf,
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
#اگر_تعریف_شده USE_IPV6
  حالة AF_INET6: {
    ثابت بنية sockaddr_in6 *sa6 = (ثابت فراغ *)ai->ai_addr;
    ثابت بنية in6_addr *ipaddr6 = &sa6->sin6_addr;
    (فراغ)curlx_inet_ntop(ai->ai_family, (ثابت فراغ *)ipaddr6, buf, bufsize);
    اكسر;
  }
#پایان_اگر
  افتراضي:
    اكسر;
  }
}

#اگر_تعریف_شده USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ساكن sigjmp_buf curl_jmpenv;
ساكن curl_simple_قفل curl_jmpenv_قفل = CURL_SIMPLE_قفل_INIT;
#پایان_اگر

#اگر_تعریف_شده USE_IPV6
/* return a static IPv6 ::1 for the name */
ساكن بنية Curl_addrinfo *get_localhost6(uint16_t پورت, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  ثابت size_t ss_اندازه = حجم(بنية sockaddr_in6);
  ثابت size_t hostlen = طول_رشته(name);
  بنية sockaddr_in6 sa6;
  بیعلامت حرف ipv6[16];
  بیعلامت کوتاه port16 = (بیعلامت کوتاه)(پورت & 0xffff);
  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_اندازه + hostlen + 1);
  إذا(!ca)
    أرجع عدم;

  memset(&sa6, 0, حجم(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (فراغ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  کپی_حافظه(&sa6.sin6_addr, ipv6, حجم(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_اندازه;
  ca->ai_بعدی      = عدم;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  کپی_حافظه(ca->ai_addr, &sa6, ss_اندازه);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_اندازه;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  أرجع ca;
}
#وإلا
#تعریف_کن get_localhost6(x, y) عدم
#پایان_اگر

/* return a static IPv4 127.0.0.1 for the given name */
ساكن بنية Curl_addrinfo *get_localhost(uint16_t پورت, ثابت حرف *name)
{
  بنية Curl_addrinfo *ca;
  بنية Curl_addrinfo *ca6;
  ثابت size_t ss_اندازه = حجم(بنية sockaddr_in);
  ثابت size_t hostlen = طول_رشته(name);
  بنية sockaddr_in sa;
  بیعلامت صحيح ipv4;
  بیعلامت کوتاه port16 = (بیعلامت کوتاه)(پورت & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, حجم(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  إذا(curlx_inet_pton(AF_INET, "127.0.0.1", (حرف *)&ipv4) < 1)
    أرجع عدم;
  کپی_حافظه(&sa.sin_addr, &ipv4, حجم(ipv4));

  ca = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_اندازه + hostlen + 1);
  إذا(!ca)
    أرجع عدم;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_اندازه;
  ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
  کپی_حافظه(ca->ai_addr, &sa, ss_اندازه);
  ca->ai_canonname = (حرف *)ca->ai_addr + ss_اندازه;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(پورت, name);
  إذا(!ca6)
    أرجع ca;
  ca6->ai_بعدی = ca;
  أرجع ca6;
}

#اگر_تعریف_شده USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(بنية Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_کار_میکند = FALSE;
  إذا(s == CURL_SOCKET_BAD) {
    إذا(SOCKERRNO == SOCKENOMEM)
      أرجع CURLE_بیرون_از_حافظه;
  }
  وإلا {
    multi->ipv6_کار_میکند = TRUE;
    sclose(s);
  }
  أرجع CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ساكن bool ipv6works(بنية Curl_easy *داده)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->multi);
  أرجع داده ? داده->multi->ipv6_کار_میکند : FALSE;
}
#پایان_اگر /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ثابت حرف *hostname)
{
  بنية in_addr درون;
#اگر_تعریف_شده USE_IPV6
  بنية in6_addr in6;
#پایان_اگر
  إذا(curlx_inet_pton(AF_INET, hostname, &درون) > 0
#اگر_تعریف_شده USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#پایان_اگر
    )
    أرجع TRUE;
  أرجع FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ساكن bool tailmatch(ثابت حرف *پر, size_t flen,
                      ثابت حرف *part, size_t plen)
{
  إذا(plen > flen)
    أرجع FALSE;
  أرجع curl_strnequal(part, &پر[flen - plen], plen);
}

ساكن CURLcode hostip_resolv_failed(بنية Curl_easy *داده,
                                     بنية Curl_peer *peer,
                                     bool for_proxy)
{
  failf(داده, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  أرجع RESOLV_FAIL(for_proxy);
}

ساكن bool can_resolve_dns_queries(بنية Curl_easy *داده,
                                    uint8_t dns_queries)
{
  (فراغ)داده;
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(داده))
    أرجع FALSE;
  أرجع TRUE;
}

CURLcode Curl_resolv_announce_شروع(بنية Curl_easy *داده,
                                    فراغ *resolver)
{
  إذا(داده->مجموعه.resolver_شروع) {
    بنية Curl_mapi_guard guard;
    صحيح rc;

    CURL_TRC_DNS(داده, "announcing resolve to application");
    CURL_CBAPI_شروع(&guard, داده, easy_resolver_شروع);
    rc = داده->مجموعه.resolver_شروع(resolver, عدم,
                                  داده->مجموعه.resolver_شروع_client);
    CURL_CBAPI_انتها(&guard);
    إذا(rc) {
      CURL_TRC_DNS(داده, "application aborted resolve");
      أرجع CURLE_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_شده USE_CURL_ASYNC

ساكن بنية Curl_resolv_async *hostip_async_new(بنية Curl_easy *داده,
                                                  uint8_t dns_queries,
                                                  بنية Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  بنية Curl_resolv_async *async;

  إذا(!داده->multi) {
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
  إذا(داده->multi->آخرین_resolv_id == UINT32_MAX)
    داده->multi->آخرین_resolv_id = 1; /* wrap around */
  وإلا
    داده->multi->آخرین_resolv_id++;
  async->id = داده->multi->آخرین_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->شروع = *Curl_pgrs_now(داده);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  إذا(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  أرجع async;
}

ساكن CURLcode hostip_resolv_take_نتیجه(بنية Curl_easy *داده,
                                          بنية Curl_resolv_async *async,
                                          بنية Curl_dns_مدخل **pdns)
{
  CURLcode نتیجه;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

#اگر_تعریف_نشده CURL_DISABLE_DOH
  إذا(async->doh)
    نتیجه = Curl_doh_take_نتیجه(داده, async, pdns);
  وإلا
#پایان_اگر
  نتیجه = Curl_async_take_نتیجه(داده, async, pdns);

  إذا(نتیجه == CURLE_AGAIN) {
    CURL_TRC_DNS(داده, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->پورت);
    نتیجه = CURLE_OK;
  }
  وإلا إذا(IS_RESOLV_FAIL(نتیجه)) {
    نتیجه = Curl_async_failed(داده, async, عدم);
  }
  وإلا إذا(نتیجه) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(داده, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (صحيح)نتیجه, async->peer->hostname, async->peer->پورت);
  }
  وإلا {
    CURL_TRC_DNS(داده, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->پورت);
    DEBUGASSERT(*pdns);
  }

  أرجع نتیجه;
}

timediff_t Curl_resolv_elapsed_ms(بنية Curl_easy *داده,
                                  uint32_t resolv_id,
                                  ثابت بنية curltime *pnow)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  إذا(!async)
    أرجع CURL_TIMEOUT_RESOLVE_MS;
  أرجع curlx_ptimediff_ms(pnow, &async->شروع);
}

bool Curl_resolv_has_answers(بنية Curl_easy *داده,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  إذا(!async || async->انجامشده)
    أرجع TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  إذا((check_queries & async->dns_responses) != check_queries) {
    أرجع FALSE;
  }
  أرجع TRUE;
}

ثابت بنية Curl_addrinfo *Curl_resolv_get_ai(بنية Curl_easy *داده,
                                               uint32_t resolv_id,
                                               صحيح ai_family,
                                               بیعلامت صحيح شاخص)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  إذا(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    أرجع عدم;
  بدل(ai_family) {
  حالة AF_INET:
    أرجع Curl_addrinfo_get(async->ai_A, ai_family, شاخص);
#اگر_تعریف_شده USE_IPV6
  حالة AF_INET6:
    أرجع Curl_addrinfo_get(async->ai_AAAA, ai_family, شاخص);
#پایان_اگر
  افتراضي:
    أرجع عدم;
  }
}

#اگر_تعریف_شده USE_HTTPSRR

CURLcode Curl_resolv_https(بنية Curl_easy *داده,
                           بنية Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           بنية Curl_dns_مدخل **pdns)
{
  أرجع Curl_resolv(داده, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ثابت بنية Curl_https_rrinfo *
Curl_resolv_get_https(بنية Curl_easy *داده, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async;
  إذا(!Curl_resolv_knows_https(داده, resolv_id))
    أرجع عدم;
  async = Curl_async_get(داده, resolv_id);
  أرجع async ? async->httpsrr : عدم;
}

bool Curl_resolv_knows_https(بنية Curl_easy *داده, uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  إذا(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    أرجع ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  أرجع TRUE; /* we know it will never come */
}

#پایان_اگر /* USE_HTTPSRR */

#پایان_اگر /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ساكن CURLcode hostip_resolv_شروع(بنية Curl_easy *داده,
                                    uint8_t dns_queries,
                                    بنية Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    بنية Curl_dns_مدخل **pdns,
                                    bool *pnegative)
{
#اگر_تعریف_شده USE_CURL_ASYNC
  بنية Curl_resolv_async *async = عدم;
#پایان_اگر
  بنية Curl_addrinfo *addr = عدم;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode نتیجه = CURLE_OK;

  *pnegative = FALSE;

  (فراغ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

  /* Check for "known" things to resolve ourselves. */
  إذا(addr_queries) {
#اگر_تعریف_نشده USE_RESOLVE_ON_IPS
    إذا(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      نتیجه = Curl_resolv_announce_شروع(داده, عدم);
      إذا(نتیجه)
        اذهب بیرون;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      نتیجه = Curl_str2addr(peer->hostname, peer->پورت, &addr);
      اذهب بیرون;
    }
#پایان_اگر

    hostname_len = طول_رشته(peer->hostname);
    إذا(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      نتیجه = Curl_resolv_announce_شروع(داده, عدم);
      إذا(نتیجه)
        اذهب بیرون;
      addr = get_localhost(peer->پورت, peer->hostname);
      إذا(!addr)
        نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
#اگر_تعریف_نشده CURL_DISABLE_DOH
  إذا(!Curl_is_ipaddr(peer->hostname) && allowDOH && داده->مجموعه.doh) {
    نتیجه = Curl_resolv_announce_شروع(داده, عدم);
    إذا(نتیجه)
      اذهب بیرون;
    إذا(!async) {
      async = hostip_async_new(داده, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      إذا(!async) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
    }
    نتیجه = Curl_doh(داده, async);
    اذهب بیرون;
  }
#وإلا
  (فراغ)allowDOH;
#پایان_اگر

  /* Can we provide the requested IP specifics in resolving? */
  إذا(!can_resolve_dns_queries(داده, dns_queries)) {
    نتیجه = RESOLV_FAIL(for_proxy);
    اذهب بیرون;
  }

#اگر_تعریف_شده CURLRES_ASYNCH
  (فراغ)addr;
  إذا(!async) {
    async = hostip_async_new(داده, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    إذا(!async) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
  نتیجه = Curl_async_getaddrinfo(داده, async);
  إذا(نتیجه == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_نتیجه(داده, async, pdns);
    إذا(r2)
      نتیجه = r2;
    وإلا إذا(*pdns)
      نتیجه = CURLE_OK;
  }
#وإلا
  نتیجه = Curl_resolv_announce_شروع(داده, عدم);
  إذا(نتیجه)
    اذهب بیرون;
  addr = Curl_sync_getaddrinfo(داده, dns_queries, peer->hostname, peer->پورت,
                               transport);
  إذا(!addr) {
    نتیجه = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#پایان_اگر

بیرون:
  إذا(!نتیجه) {
    إذا(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(داده, dns_queries, &addr, peer);
      إذا(!*pdns)
        نتیجه = CURLE_بیرون_از_حافظه;
    }
    وإلا إذا(!*pdns)
      نتیجه = CURLE_AGAIN;
  }
  وإلا إذا(*pdns)
    Curl_dns_مدخل_unlink(داده, pdns);
  وإلا إذا(addr)
    Curl_freeaddrinfo(addr);

#اگر_تعریف_شده USE_CURL_ASYNC
  إذا(async) {
    إذا(نتیجه == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->بعدی = داده->state.async;
      داده->state.async = async;
    }
    وإلا {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(داده, async);
    }
  }
#پایان_اگر
  أرجع نتیجه;
}

ساكن CURLcode hostip_resolv(بنية Curl_easy *داده,
                              uint8_t dns_queries,
                              بنية Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              بنية Curl_dns_مدخل **pdns)
{
  size_t hostname_len;
  CURLcode نتیجه = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (فراغ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = عدم;

#اگر_تعریف_شده CURL_DISABLE_DOH
  (فراغ)allowDOH;
#پایان_اگر

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = طول_رشته(peer->hostname);
  DEBUGASSERT(hostname_len);
  إذا(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(داده, "Not resolving .onion address (RFC 7686)");
    اذهب بیرون;
  }

#اگر_تعریف_شده DEBUGBUILD
  CURL_TRC_DNS(داده, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->پورت);
  إذا((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(داده, "DEBUG fail ipv6 resolve");
    نتیجه = hostip_resolv_failed(داده, peer, for_proxy);
    اذهب بیرون;
  }
#پایان_اگر
  /* Let's check our DNS cache first */
  نتیجه = Curl_dnscache_get(داده, dns_queries, peer, pdns);
  إذا(*pdns) {
    infof(داده, "Hostname %s was found in DNS cache", peer->hostname);
    نتیجه = CURLE_OK;
  }
  وإلا إذا(نتیجه) {
    infof(داده, "Negative DNS entry");
    نتیجه = hostip_resolv_failed(داده, peer, for_proxy);
  }
  وإلا {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    نتیجه = hostip_resolv_شروع(داده, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(داده, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (صحيح)نتیجه);
  }

بیرون:
  إذا(نتیجه && (نتیجه != CURLE_AGAIN)) {
    Curl_dns_مدخل_unlink(داده, pdns);
    إذا(IS_RESOLV_FAIL(نتیجه)) {
      إذا(cache_dns && negative)
        Curl_dnscache_جمع_کن_negative(داده, dns_queries, peer);
      إذا(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(داده, "Could not resolve: %s:%u", peer->hostname, peer->پورت);
    }
    وإلا {
      failf(داده, "Error %d resolving %s:%u",
            (صحيح)نتیجه, peer->hostname, peer->پورت);
    }
  }
  وإلا إذا(cache_dns && *pdns) {
    نتیجه = Curl_dnscache_جمع_کن(داده, *pdns);
    إذا(نتیجه)
      Curl_dns_مدخل_unlink(داده, pdns);
  }

  أرجع نتیجه;
}

CURLcode Curl_resolv_blocking(بنية Curl_easy *داده,
                              uint8_t dns_queries,
                              ثابت حرف *hostname,
                              uint16_t پورت,
                              uint8_t transport,
                              بنية Curl_dns_مدخل **pdns)
{
  بنية Curl_peer *peer = عدم;
  CURLcode نتیجه;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = عدم;

  نتیجه = Curl_peer_create(داده, داده->conn->scheme, hostname, پورت, &peer);
  إذا(نتیجه)
    اذهب بیرون;

  /* We cannot do a blocking resolve using DoH currently */
  نتیجه = hostip_resolv(داده, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  بدل(نتیجه) {
  حالة CURLE_OK:
    DEBUGASSERT(*pdns);
    اكسر;
#اگر_تعریف_شده USE_CURL_ASYNC
  حالة CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    نتیجه = Curl_async_await(داده, resolv_id, pdns);
    Curl_resolv_destroy(داده, resolv_id);
    اكسر;
#پایان_اگر
  افتراضي:
    اكسر;
  }

بیرون:
  Curl_peer_unlink(&peer);
  أرجع نتیجه;
}

#اگر_تعریف_شده USE_ALARM_TIMEOUT
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

ساكن CURLcode resolv_alarm_timeout(بنية Curl_easy *داده,
                                     uint8_t dns_queries,
                                     بنية Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     بنية Curl_dns_مدخل **مدخل)
{
#اگر_تعریف_شده HAVE_SIGACTION
  بنية sigaction keep_sigact; /* store the old struct here */
  متطاير bool keep_copysig = FALSE; /* whether old sigact has been saved */
  بنية sigaction sigact;
#وإلا
#اگر_تعریف_شده HAVE_SIGNAL
  فراغ (*keep_sigact)(صحيح);       /* store the old handler here */
#پایان_اگر /* HAVE_SIGNAL */
#پایان_اگر /* HAVE_SIGACTION */
  متطاير دراز timeout;
  متطاير بیعلامت صحيح prev_alarm = 0;
  CURLcode نتیجه;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!داده->مجموعه.no_signal);
#اگر_تعریف_نشده CURL_DISABLE_DOH
  DEBUGASSERT(!داده->مجموعه.doh);
#پایان_اگر

  *مدخل = عدم;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (دراز)timeout_ms;
  إذا(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(داده,
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
  curl_simple_قفل_قفل(&curl_jmpenv_قفل);

  إذا(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(داده, "name lookup timed out");
    نتیجه = CURLE_OPERATION_TIMEDOUT;
    اذهب clean_به_بالا;
  }
  وإلا {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#اگر_تعریف_شده HAVE_SIGACTION
    sigaction(SIGALRM, عدم, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#اگر_تعریف_شده SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#پایان_اگر
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, عدم);
#وإلا /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#اگر_تعریف_شده HAVE_SIGNAL
    keep_sigact = إشارة(SIGALRM, alarmfunc);
#پایان_اگر
#پایان_اگر /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  نتیجه = hostip_resolv(داده, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, مدخل);

clean_به_بالا:
  إذا(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#اگر_تعریف_شده HAVE_SIGACTION
  إذا(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, عدم); /* put it back */
  }
#وإلا
#اگر_تعریف_شده HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  إشارة(SIGALRM, keep_sigact);
#پایان_اگر
#پایان_اگر /* HAVE_SIGACTION */

  curl_simple_قفل_unlock(&curl_jmpenv_قفل);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  إذا(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(داده, داده->conn, Curl_pgrs_now(داده)) / 1000;

    /* the alarm period is counted in even number of seconds */
    بیعلامت دراز alarm_مجموعه = (بیعلامت دراز)(prev_alarm - elapsed_secs);

    إذا(!alarm_مجموعه ||
       ((alarm_مجموعه >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      failf(داده, "Previous alarm fired off");
    }
    وإلا
      alarm((بیعلامت صحيح)alarm_مجموعه);
  }

  أرجع نتیجه;
}

#پایان_اگر /* USE_ALARM_TIMEOUT */

#اگر_تعریف_شده USE_UNIX_SOCKETS
ساكن CURLcode resolv_unix(بنية Curl_easy *داده,
                            بنية Curl_peer *peer,
                            بنية Curl_dns_مدخل **pdns)
{
  بنية Curl_addrinfo *addr;
  CURLcode نتیجه;

  DEBUGASSERT(peer->unix_socket);
  *pdns = عدم;

  نتیجه = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(داده, "Unix socket path too long: '%s'", peer->hostname);
      نتیجه = CURLE_COULDNT_RESOLVE_HOST;
    }
    أرجع نتیجه;
  }

  *pdns = Curl_dnsc_mk_addr(داده, 0, &addr, peer);
  أرجع *pdns ? CURLE_OK : CURLE_بیرون_از_حافظه;
}
#پایان_اگر /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(بنية Curl_easy *داده,
                     بنية Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     بنية Curl_dns_مدخل **pdns)
{
  *presolv_id = 0;
  *pdns = عدم;

  إذا(timeout_ms < 0)
    /* got an already expired timeout */
    أرجع CURLE_OPERATION_TIMEDOUT;
  وإلا إذا(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#اگر_تعریف_شده USE_UNIX_SOCKETS
  إذا((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    أرجع resolv_unix(داده, peer, pdns);
#وإلا
  إذا(peer->unix_socket)
    أرجع hostip_resolv_failed(داده, peer, for_proxy);
#پایان_اگر

#اگر_تعریف_شده USE_ALARM_TIMEOUT
  إذا(dns_queries & CURL_DNSQ_ADDR) {
    إذا(timeout_ms && داده->مجموعه.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    إذا(timeout_ms && !Curl_doh_wanted(داده)) {
      أرجع resolv_alarm_timeout(داده, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#پایان_اگر /* !USE_ALARM_TIMEOUT */

#اگر_تعریف_نشده CURLRES_ASYNCH
  إذا(timeout_ms)
    infof(داده, "timeout on name lookup is not supported");
#پایان_اگر

  أرجع hostip_resolv(داده, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#اگر_تعریف_شده USE_CURL_ASYNC

بنية Curl_resolv_async *Curl_async_get(بنية Curl_easy *داده,
                                         uint32_t resolv_id)
{
  بنية Curl_resolv_async *async = داده->state.async;
  لكل(; async; async = async->بعدی) {
    إذا(async->id == resolv_id)
      أرجع async;
  }
  أرجع عدم;
}

CURLcode Curl_resolv_take_نتیجه(بنية Curl_easy *داده, uint32_t resolv_id,
                                 بنية Curl_dns_مدخل **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  CURLcode نتیجه;

  /* If async resolving is ongoing, this must be set */
  إذا(!async)
    أرجع CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  نتیجه = Curl_dnscache_get(داده, async->dns_queries, async->peer, pdns);
  إذا(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(داده, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(داده, async);
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجه) {
    Curl_async_shutdown(داده, async);
    أرجع Curl_async_failed(داده, async, عدم);
  }

  نتیجه = hostip_resolv_take_نتیجه(داده, async, pdns);

  إذا(*pdns) {
    /* Add to cache */
    نتیجه = Curl_dnscache_جمع_کن(داده, *pdns);
    إذا(نتیجه)
      Curl_dns_مدخل_unlink(داده, pdns);
  }
  وإلا إذا(IS_RESOLV_FAIL(نتیجه)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    إذا(async->negative_answer)
      Curl_dnscache_جمع_کن_negative(داده, async->dns_queries, async->peer);
    إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(داده, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->پورت);
  }
  وإلا إذا(نتیجه) {
    failf(داده, "Error %d resolving %s:%u",
          (صحيح)نتیجه, async->peer->hostname, async->peer->پورت);
  }
  أرجع نتیجه;
}

CURLcode Curl_resolv_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  بنية Curl_resolv_async *async = داده->state.async;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)ps;
  لكل(; async && !نتیجه; async = async->بعدی) {
#اگر_تعریف_نشده CURL_DISABLE_DOH
    إذا(async->doh) /* DoH has nothing for the pollset */
      استمر;
#پایان_اگر
    نتیجه = Curl_async_pollset(داده, async, ps);
  }
  أرجع نتیجه;
}

فراغ Curl_resolv_destroy(بنية Curl_easy *داده, uint32_t resolv_id)
{
  بنية Curl_resolv_async **panchor = &داده->state.async;

  لكل(; *panchor; panchor = &(*panchor)->بعدی) {
    بنية Curl_resolv_async *async = *panchor;
    إذا(async->id == resolv_id) {
      *panchor = async->بعدی;
      Curl_async_destroy(داده, async);
      اكسر;
    }
  }
}

فراغ Curl_resolv_shutdown_all(بنية Curl_easy *داده)
{
  بنية Curl_resolv_async *async = داده->state.async;
  لكل(; async; async = async->بعدی) {
    Curl_async_shutdown(داده, async);
  }
}

فراغ Curl_resolv_destroy_all(بنية Curl_easy *داده)
{
  طالما(داده->state.async) {
    بنية Curl_resolv_async *async = داده->state.async;
    داده->state.async = async->بعدی;
    Curl_async_destroy(داده, async);
  }
}

#پایان_اگر /* USE_CURL_ASYNC */
