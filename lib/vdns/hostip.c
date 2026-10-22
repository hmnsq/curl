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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IN6_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in6.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "easy_lock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/dnscache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/doh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਜੇ defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ALARM_TIMEOUT
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_RESOLV_FAIL(ਨਤੀਜਾ) \
  (((ਨਤੀਜਾ) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((ਨਤੀਜਾ) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
ਸਥਿਰ bool ipv6works(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ipv6works(x) FALSE
#ਜੇ_ਅੰਤ

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

uint8_t Curl_resolv_dns_queries(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint8_t ip_ਸੰਸਕਰਣ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਚੋਣ(ip_ਸੰਸਕਰਣ) {
  ਹਾਲਤ CURL_IPRESOLVE_V6:
    ਵਾਪਸ CURL_DNSQ_AAAA;
  ਹਾਲਤ CURL_IPRESOLVE_V4:
    ਵਾਪਸ CURL_DNSQ_A;
  ਮੂਲ:
    ਜੇ(ipv6works(ਅੰਕੜੇ))
      ਵਾਪਸ (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURL_DNSQ_A;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_resolv_query_str(uint8_t dns_queries)
{
  ਚੋਣ(dns_queries) {
  ਹਾਲਤ (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ਵਾਪਸ "A+AAAA+HTTPS";
  ਹਾਲਤ (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ਵਾਪਸ "A+AAAA";
  ਹਾਲਤ (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ਵਾਪਸ "AAAA+HTTPS";
  ਹਾਲਤ (CURL_DNSQ_AAAA):
    ਵਾਪਸ "AAAA";
  ਹਾਲਤ (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ਵਾਪਸ "A+HTTPS";
  ਹਾਲਤ (CURL_DNSQ_A):
    ਵਾਪਸ "A";
  ਹਾਲਤ (CURL_DNSQ_HTTPS):
    ਵਾਪਸ "HTTPS";
  ਹਾਲਤ 0:
    ਵਾਪਸ "-";
  ਮੂਲ:
    DEBUGASSERT(0);
    ਵਾਪਸ "???";
  }
}
#ਜੇ_ਅੰਤ

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
ਖਾਲੀ Curl_printable_ਪਤਾ(ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai, ਅੱਖਰ *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  ਚੋਣ(ai->ai_family) {
  ਹਾਲਤ AF_INET: {
    ਅਚਲ ਬਣਤਰ sockaddr_in *sa4 = (ਅਚਲ ਖਾਲੀ *)ai->ai_addr;
    ਅਚਲ ਬਣਤਰ in_addr *ipaddr4 = &sa4->sin_addr;
    (ਖਾਲੀ)curlx_inet_ntop(ai->ai_family, (ਅਚਲ ਖਾਲੀ *)ipaddr4, buf, bufsize);
    ਰੋਕੋ;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ AF_INET6: {
    ਅਚਲ ਬਣਤਰ sockaddr_in6 *sa6 = (ਅਚਲ ਖਾਲੀ *)ai->ai_addr;
    ਅਚਲ ਬਣਤਰ in6_addr *ipaddr6 = &sa6->sin6_addr;
    (ਖਾਲੀ)curlx_inet_ntop(ai->ai_family, (ਅਚਲ ਖਾਲੀ *)ipaddr6, buf, bufsize);
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਰੋਕੋ;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ਸਥਿਰ sigjmp_buf curl_jmpenv;
ਸਥਿਰ curl_simple_ਤਾਲਾ curl_jmpenv_ਤਾਲਾ = CURL_SIMPLE_ਤਾਲਾ_INIT;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
/* return a static IPv6 ::1 for the name */
ਸਥਿਰ ਬਣਤਰ Curl_addrinfo *get_localhost6(uint16_t ਪੋਰਟ, ਅਚਲ ਅੱਖਰ *name)
{
  ਬਣਤਰ Curl_addrinfo *ca;
  ਅਚਲ size_t ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
  ਅਚਲ size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(name);
  ਬਣਤਰ sockaddr_in6 sa6;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ipv6[16];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ port16 = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)(ਪੋਰਟ & 0xffff);
  ca = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ + hostlen + 1);
  ਜੇ(!ca)
    ਵਾਪਸ ਨਿਰਾ;

  memset(&sa6, 0, ਆਕਾਰ(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (ਖਾਲੀ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&sa6.sin6_addr, ipv6, ਆਕਾਰ(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ਮਾਪ;
  ca->ai_ਅਗਲਾ      = ਨਿਰਾ;
  ca->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ca + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_addr, &sa6, ss_ਮਾਪ);
  ca->ai_canonname = (ਅੱਖਰ *)ca->ai_addr + ss_ਮਾਪ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ਵਾਪਸ ca;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_localhost6(x, y) ਨਿਰਾ
#ਜੇ_ਅੰਤ

/* return a static IPv4 127.0.0.1 for the given name */
ਸਥਿਰ ਬਣਤਰ Curl_addrinfo *get_localhost(uint16_t ਪੋਰਟ, ਅਚਲ ਅੱਖਰ *name)
{
  ਬਣਤਰ Curl_addrinfo *ca;
  ਬਣਤਰ Curl_addrinfo *ca6;
  ਅਚਲ size_t ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
  ਅਚਲ size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(name);
  ਬਣਤਰ sockaddr_in sa;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ipv4;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ port16 = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)(ਪੋਰਟ & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, ਆਕਾਰ(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  ਜੇ(curlx_inet_pton(AF_INET, "127.0.0.1", (ਅੱਖਰ *)&ipv4) < 1)
    ਵਾਪਸ ਨਿਰਾ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&sa.sin_addr, &ipv4, ਆਕਾਰ(ipv4));

  ca = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ + hostlen + 1);
  ਜੇ(!ca)
    ਵਾਪਸ ਨਿਰਾ;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ਮਾਪ;
  ca->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ca + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_addr, &sa, ss_ਮਾਪ);
  ca->ai_canonname = (ਅੱਖਰ *)ca->ai_addr + ss_ਮਾਪ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(ਪੋਰਟ, name);
  ਜੇ(!ca6)
    ਵਾਪਸ ca;
  ca6->ai_ਅਗਲਾ = ca;
  ਵਾਪਸ ca6;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(ਬਣਤਰ Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_ਚੱਲਦਾ = FALSE;
  ਜੇ(s == CURL_SOCKET_BAD) {
    ਜੇ(SOCKERRNO == SOCKENOMEM)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ {
    multi->ipv6_ਚੱਲਦਾ = TRUE;
    sclose(s);
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ਸਥਿਰ bool ipv6works(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  ਵਾਪਸ ਅੰਕੜੇ ? ਅੰਕੜੇ->multi->ipv6_ਚੱਲਦਾ : FALSE;
}
#ਜੇ_ਅੰਤ /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ਅਚਲ ਅੱਖਰ *hostname)
{
  ਬਣਤਰ in_addr ਵਿੱਚ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ in6_addr in6;
#ਜੇ_ਅੰਤ
  ਜੇ(curlx_inet_pton(AF_INET, hostname, &ਵਿੱਚ) > 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#ਜੇ_ਅੰਤ
    )
    ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ਸਥਿਰ bool tailmatch(ਅਚਲ ਅੱਖਰ *ਭਰਿਆ, size_t flen,
                      ਅਚਲ ਅੱਖਰ *part, size_t plen)
{
  ਜੇ(plen > flen)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ curl_strnequal(part, &ਭਰਿਆ[flen - plen], plen);
}

ਸਥਿਰ CURLcode hostip_resolv_failed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_peer *peer,
                                     bool for_proxy)
{
  failf(ਅੰਕੜੇ, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ਵਾਪਸ RESOLV_FAIL(for_proxy);
}

ਸਥਿਰ bool can_resolve_dns_queries(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t dns_queries)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(ਅੰਕੜੇ))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

CURLcode Curl_resolv_announce_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਖਾਲੀ *resolver)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.resolver_ਸ਼ੁਰੂ) {
    ਬਣਤਰ Curl_mapi_guard guard;
    ਪੂਰਨ rc;

    CURL_TRC_DNS(ਅੰਕੜੇ, "announcing resolve to application");
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_resolver_ਸ਼ੁਰੂ);
    rc = ਅੰਕੜੇ->ਸੈੱਟ.resolver_ਸ਼ੁਰੂ(resolver, ਨਿਰਾ,
                                  ਅੰਕੜੇ->ਸੈੱਟ.resolver_ਸ਼ੁਰੂ_client);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ਜੇ(rc) {
      CURL_TRC_DNS(ਅੰਕੜੇ, "application aborted resolve");
      ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC

ਸਥਿਰ ਬਣਤਰ Curl_resolv_async *hostip_async_new(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                  uint8_t dns_queries,
                                                  ਬਣਤਰ Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  ਬਣਤਰ Curl_resolv_async *async;

  ਜੇ(!ਅੰਕੜੇ->multi) {
    DEBUGASSERT(0);
    ਵਾਪਸ ਨਿਰਾ;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, ਆਕਾਰ(*async));
  ਜੇ(!async)
    ਵਾਪਸ ਨਿਰਾ;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  ਜੇ(ਅੰਕੜੇ->multi->ਆਖਰੀ_resolv_id == UINT32_MAX)
    ਅੰਕੜੇ->multi->ਆਖਰੀ_resolv_id = 1; /* wrap around */
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->multi->ਆਖਰੀ_resolv_id++;
  async->id = ਅੰਕੜੇ->multi->ਆਖਰੀ_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->ਸ਼ੁਰੂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  ਜੇ(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ਵਾਪਸ async;
}

ਸਥਿਰ CURLcode hostip_resolv_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_resolv_async *async,
                                          ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  CURLcode ਨਤੀਜਾ;

  /* If async resolving is ongoing, this must be set */
  ਜੇ(!async)
    ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਜੇ(async->doh)
    ਨਤੀਜਾ = Curl_doh_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਨਤੀਜਾ = Curl_async_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->ਪੋਰਟ);
    ਨਤੀਜਾ = CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(IS_RESOLV_FAIL(ਨਤੀਜਾ)) {
    ਨਤੀਜਾ = Curl_async_failed(ਅੰਕੜੇ, async, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (ਪੂਰਨ)ਨਤੀਜਾ, async->peer->hostname, async->peer->ਪੋਰਟ);
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->ਪੋਰਟ);
    DEBUGASSERT(*pdns);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

timediff_t Curl_resolv_elapsed_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  uint32_t resolv_id,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਜੇ(!async)
    ਵਾਪਸ CURL_TIMEOUT_RESOLVE_MS;
  ਵਾਪਸ curlx_ptimediff_ms(pnow, &async->ਸ਼ੁਰੂ);
}

bool Curl_resolv_has_answers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  ਜੇ(!async || async->ਪੂਰਾ)
    ਵਾਪਸ TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  ਜੇ((check_queries & async->dns_responses) != check_queries) {
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਅਚਲ ਬਣਤਰ Curl_addrinfo *Curl_resolv_get_ai(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                               uint32_t resolv_id,
                                               ਪੂਰਨ ai_family,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਜੇ(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ਵਾਪਸ ਨਿਰਾ;
  ਚੋਣ(ai_family) {
  ਹਾਲਤ AF_INET:
    ਵਾਪਸ Curl_addrinfo_get(async->ai_A, ai_family, ਸੂਚਕਾਂਕ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ AF_INET6:
    ਵਾਪਸ Curl_addrinfo_get(async->ai_AAAA, ai_family, ਸੂਚਕਾਂਕ);
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ ਨਿਰਾ;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

CURLcode Curl_resolv_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਵਾਪਸ Curl_resolv(ਅੰਕੜੇ, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *
Curl_resolv_get_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id)
{
  ਬਣਤਰ Curl_resolv_async *async;
  ਜੇ(!Curl_resolv_knows_https(ਅੰਕੜੇ, resolv_id))
    ਵਾਪਸ ਨਿਰਾ;
  async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਵਾਪਸ async ? async->httpsrr : ਨਿਰਾ;
}

bool Curl_resolv_knows_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਜੇ(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ਵਾਪਸ ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ਵਾਪਸ TRUE; /* we know it will never come */
}

#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ਸਥਿਰ CURLcode hostip_resolv_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t dns_queries,
                                    ਬਣਤਰ Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns,
                                    bool *pnegative)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਬਣਤਰ Curl_resolv_async *async = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ Curl_addrinfo *addr = ਨਿਰਾ;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pnegative = FALSE;

  (ਖਾਲੀ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ਨਿਰਾ;

  /* Check for "known" things to resolve ourselves. */
  ਜੇ(addr_queries) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_RESOLVE_ON_IPS
    ਜੇ(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ਨਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      ਨਤੀਜਾ = Curl_str2addr(peer->hostname, peer->ਪੋਰਟ, &addr);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
#ਜੇ_ਅੰਤ

    hostname_len = ਲੜੀ_ਲੰਬਾਈ(peer->hostname);
    ਜੇ(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ਨਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      addr = get_localhost(peer->ਪੋਰਟ, peer->hostname);
      ਜੇ(!addr)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਜੇ(!Curl_is_ipaddr(peer->hostname) && allowDOH && ਅੰਕੜੇ->ਸੈੱਟ.doh) {
    ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(!async) {
      async = hostip_async_new(ਅੰਕੜੇ, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      ਜੇ(!async) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਤੀਜਾ = Curl_doh(ਅੰਕੜੇ, async);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)allowDOH;
#ਜੇ_ਅੰਤ

  /* Can we provide the requested IP specifics in resolving? */
  ਜੇ(!can_resolve_dns_queries(ਅੰਕੜੇ, dns_queries)) {
    ਨਤੀਜਾ = RESOLV_FAIL(for_proxy);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_ASYNCH
  (ਖਾਲੀ)addr;
  ਜੇ(!async) {
    async = hostip_async_new(ਅੰਕੜੇ, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    ਜੇ(!async) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਤੀਜਾ = Curl_async_getaddrinfo(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);
    ਜੇ(r2)
      ਨਤੀਜਾ = r2;
    ਨਹੀਂਤਾਂ ਜੇ(*pdns)
      ਨਤੀਜਾ = CURLE_OK;
  }
#ਨਹੀਂਤਾਂ
  ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ਨਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  addr = Curl_sync_getaddrinfo(ਅੰਕੜੇ, dns_queries, peer->hostname, peer->ਪੋਰਟ,
                               transport);
  ਜੇ(!addr) {
    ਨਤੀਜਾ = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(ਅੰਕੜੇ, dns_queries, &addr, peer);
      ਜੇ(!*pdns)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!*pdns)
      ਨਤੀਜਾ = CURLE_AGAIN;
  }
  ਨਹੀਂਤਾਂ ਜੇ(*pdns)
    Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, pdns);
  ਨਹੀਂਤਾਂ ਜੇ(addr)
    Curl_freeaddrinfo(addr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਜੇ(async) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->ਅਗਲਾ = ਅੰਕੜੇ->state.async;
      ਅੰਕੜੇ->state.async = async;
    }
    ਨਹੀਂਤਾਂ {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(ਅੰਕੜੇ, async);
    }
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode hostip_resolv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              uint8_t dns_queries,
                              ਬਣਤਰ Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  size_t hostname_len;
  CURLcode ਨਤੀਜਾ = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (ਖਾਲੀ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  (ਖਾਲੀ)allowDOH;
#ਜੇ_ਅੰਤ

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = ਲੜੀ_ਲੰਬਾਈ(peer->hostname);
  DEBUGASSERT(hostname_len);
  ਜੇ(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(ਅੰਕੜੇ, "Not resolving .onion address (RFC 7686)");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->ਪੋਰਟ);
  ਜੇ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(ਅੰਕੜੇ, "DEBUG fail ipv6 resolve");
    ਨਤੀਜਾ = hostip_resolv_failed(ਅੰਕੜੇ, peer, for_proxy);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ
  /* Let's check our DNS cache first */
  ਨਤੀਜਾ = Curl_dnscache_get(ਅੰਕੜੇ, dns_queries, peer, pdns);
  ਜੇ(*pdns) {
    infof(ਅੰਕੜੇ, "Hostname %s was found in DNS cache", peer->hostname);
    ਨਤੀਜਾ = CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    infof(ਅੰਕੜੇ, "Negative DNS entry");
    ਨਤੀਜਾ = hostip_resolv_failed(ਅੰਕੜੇ, peer, for_proxy);
  }
  ਨਹੀਂਤਾਂ {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    ਨਤੀਜਾ = hostip_resolv_ਸ਼ੁਰੂ(ਅੰਕੜੇ, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (ਪੂਰਨ)ਨਤੀਜਾ);
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN)) {
    Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, pdns);
    ਜੇ(IS_RESOLV_FAIL(ਨਤੀਜਾ)) {
      ਜੇ(cache_dns && negative)
        Curl_dnscache_ਜੋੜੋ_negative(ਅੰਕੜੇ, dns_queries, peer);
      ਜੇ(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(ਅੰਕੜੇ, "Could not resolve: %s:%u", peer->hostname, peer->ਪੋਰਟ);
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "Error %d resolving %s:%u",
            (ਪੂਰਨ)ਨਤੀਜਾ, peer->hostname, peer->ਪੋਰਟ);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(cache_dns && *pdns) {
    ਨਤੀਜਾ = Curl_dnscache_ਜੋੜੋ(ਅੰਕੜੇ, *pdns);
    ਜੇ(ਨਤੀਜਾ)
      Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, pdns);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_resolv_blocking(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              uint8_t dns_queries,
                              ਅਚਲ ਅੱਖਰ *hostname,
                              uint16_t ਪੋਰਟ,
                              uint8_t transport,
                              ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ਨਿਰਾ;

  ਨਤੀਜਾ = Curl_peer_create(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->scheme, hostname, ਪੋਰਟ, &peer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* We cannot do a blocking resolve using DoH currently */
  ਨਤੀਜਾ = hostip_resolv(ਅੰਕੜੇ, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  ਚੋਣ(ਨਤੀਜਾ) {
  ਹਾਲਤ CURLE_OK:
    DEBUGASSERT(*pdns);
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਹਾਲਤ CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    ਨਤੀਜਾ = Curl_async_await(ਅੰਕੜੇ, resolv_id, pdns);
    Curl_resolv_destroy(ਅੰਕੜੇ, resolv_id);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
  Curl_peer_unlink(&peer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ਸਥਿਰ ਖਾਲੀ alarmfunc(ਪੂਰਨ sig)
{
  (ਖਾਲੀ)sig;
  siglongjmp(curl_jmpenv, 1);
}

ਸਥਿਰ CURLcode resolv_alarm_timeout(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     uint8_t dns_queries,
                                     ਬਣਤਰ Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **ਇੰਦਰਾਜ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGACTION
  ਬਣਤਰ sigaction keep_sigact; /* store the old struct here */
  ਅਸਥਿਰ bool keep_copysig = FALSE; /* whether old sigact has been saved */
  ਬਣਤਰ sigaction sigact;
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGNAL
  ਖਾਲੀ (*keep_sigact)(ਪੂਰਨ);       /* store the old handler here */
#ਜੇ_ਅੰਤ /* HAVE_SIGNAL */
#ਜੇ_ਅੰਤ /* HAVE_SIGACTION */
  ਅਸਥਿਰ ਲੰਮਾ timeout;
  ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ prev_alarm = 0;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!ਅੰਕੜੇ->ਸੈੱਟ.no_signal);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  DEBUGASSERT(!ਅੰਕੜੇ->ਸੈੱਟ.doh);
#ਜੇ_ਅੰਤ

  *ਇੰਦਰਾਜ = ਨਿਰਾ;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (ਲੰਮਾ)timeout_ms;
  ਜੇ(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(ਅੰਕੜੇ,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_ਤਾਲਾ_ਤਾਲਾ(&curl_jmpenv_ਤਾਲਾ);

  ਜੇ(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(ਅੰਕੜੇ, "name lookup timed out");
    ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
    ਜਾਓ clean_ਉੱਪਰ;
  }
  ਨਹੀਂਤਾਂ {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGACTION
    sigaction(SIGALRM, ਨਿਰਾ, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#ਜੇ_ਅੰਤ
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGNAL
    keep_sigact = ਸੰਕੇਤ(SIGALRM, alarmfunc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  ਨਤੀਜਾ = hostip_resolv(ਅੰਕੜੇ, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, ਇੰਦਰਾਜ);

clean_ਉੱਪਰ:
  ਜੇ(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGACTION
  ਜੇ(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ਨਿਰਾ); /* put it back */
  }
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  ਸੰਕੇਤ(SIGALRM, keep_sigact);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* HAVE_SIGACTION */

  curl_simple_ਤਾਲਾ_unlock(&curl_jmpenv_ਤਾਲਾ);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  ਜੇ(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, Curl_pgrs_now(ਅੰਕੜੇ)) / 1000;

    /* the alarm period is counted in even number of seconds */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ alarm_ਸੈੱਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(prev_alarm - elapsed_secs);

    ਜੇ(!alarm_ਸੈੱਟ ||
       ((alarm_ਸੈੱਟ >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      failf(ਅੰਕੜੇ, "Previous alarm fired off");
    }
    ਨਹੀਂਤਾਂ
      alarm((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)alarm_ਸੈੱਟ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_ALARM_TIMEOUT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
ਸਥਿਰ CURLcode resolv_unix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *peer,
                            ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_addrinfo *addr;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ਨਿਰਾ;

  ਨਤੀਜਾ = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(ਅੰਕੜੇ, "Unix socket path too long: '%s'", peer->hostname);
      ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_HOST;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  *pdns = Curl_dnsc_mk_addr(ਅੰਕੜੇ, 0, &addr, peer);
  ਵਾਪਸ *pdns ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਬਣਤਰ Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  *presolv_id = 0;
  *pdns = ਨਿਰਾ;

  ਜੇ(timeout_ms < 0)
    /* got an already expired timeout */
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  ਨਹੀਂਤਾਂ ਜੇ(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਜੇ((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ਵਾਪਸ resolv_unix(ਅੰਕੜੇ, peer, pdns);
#ਨਹੀਂਤਾਂ
  ਜੇ(peer->unix_socket)
    ਵਾਪਸ hostip_resolv_failed(ਅੰਕੜੇ, peer, for_proxy);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ALARM_TIMEOUT
  ਜੇ(dns_queries & CURL_DNSQ_ADDR) {
    ਜੇ(timeout_ms && ਅੰਕੜੇ->ਸੈੱਟ.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    ਜੇ(timeout_ms && !Curl_doh_wanted(ਅੰਕੜੇ)) {
      ਵਾਪਸ resolv_alarm_timeout(ਅੰਕੜੇ, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#ਜੇ_ਅੰਤ /* !USE_ALARM_TIMEOUT */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURLRES_ASYNCH
  ਜੇ(timeout_ms)
    infof(ਅੰਕੜੇ, "timeout on name lookup is not supported");
#ਜੇ_ਅੰਤ

  ਵਾਪਸ hostip_resolv(ਅੰਕੜੇ, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC

ਬਣਤਰ Curl_resolv_async *Curl_async_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         uint32_t resolv_id)
{
  ਬਣਤਰ Curl_resolv_async *async = ਅੰਕੜੇ->state.async;
  ਲਈ(; async; async = async->ਅਗਲਾ) {
    ਜੇ(async->id == resolv_id)
      ਵਾਪਸ async;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode Curl_resolv_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id,
                                 ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  CURLcode ਨਤੀਜਾ;

  /* If async resolving is ongoing, this must be set */
  ਜੇ(!async)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  ਨਤੀਜਾ = Curl_dnscache_get(ਅੰਕੜੇ, async->dns_queries, async->peer, pdns);
  ਜੇ(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(ਅੰਕੜੇ, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(ਅੰਕੜੇ, async);
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    Curl_async_shutdown(ਅੰਕੜੇ, async);
    ਵਾਪਸ Curl_async_failed(ਅੰਕੜੇ, async, ਨਿਰਾ);
  }

  ਨਤੀਜਾ = hostip_resolv_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);

  ਜੇ(*pdns) {
    /* Add to cache */
    ਨਤੀਜਾ = Curl_dnscache_ਜੋੜੋ(ਅੰਕੜੇ, *pdns);
    ਜੇ(ਨਤੀਜਾ)
      Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, pdns);
  }
  ਨਹੀਂਤਾਂ ਜੇ(IS_RESOLV_FAIL(ਨਤੀਜਾ)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    ਜੇ(async->negative_answer)
      Curl_dnscache_ਜੋੜੋ_negative(ਅੰਕੜੇ, async->dns_queries, async->peer);
    ਜੇ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ਅੰਕੜੇ, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->ਪੋਰਟ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "Error %d resolving %s:%u",
          (ਪੂਰਨ)ਨਤੀਜਾ, async->peer->hostname, async->peer->ਪੋਰਟ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_resolv_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ Curl_resolv_async *async = ਅੰਕੜੇ->state.async;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)ps;
  ਲਈ(; async && !ਨਤੀਜਾ; async = async->ਅਗਲਾ) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
    ਜੇ(async->doh) /* DoH has nothing for the pollset */
      ਜਾਰੀ;
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = Curl_async_pollset(ਅੰਕੜੇ, async, ps);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_resolv_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id)
{
  ਬਣਤਰ Curl_resolv_async **panchor = &ਅੰਕੜੇ->state.async;

  ਲਈ(; *panchor; panchor = &(*panchor)->ਅਗਲਾ) {
    ਬਣਤਰ Curl_resolv_async *async = *panchor;
    ਜੇ(async->id == resolv_id) {
      *panchor = async->ਅਗਲਾ;
      Curl_async_destroy(ਅੰਕੜੇ, async);
      ਰੋਕੋ;
    }
  }
}

ਖਾਲੀ Curl_resolv_shutdown_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_resolv_async *async = ਅੰਕੜੇ->state.async;
  ਲਈ(; async; async = async->ਅਗਲਾ) {
    Curl_async_shutdown(ਅੰਕੜੇ, async);
  }
}

ਖਾਲੀ Curl_resolv_destroy_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜਦੋਂ(ਅੰਕੜੇ->state.async) {
    ਬਣਤਰ Curl_resolv_async *async = ਅੰਕੜੇ->state.async;
    ਅੰਕੜੇ->state.async = async->ਅਗਲਾ;
    Curl_async_destroy(ਅੰਕੜੇ, async);
  }
}

#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */
