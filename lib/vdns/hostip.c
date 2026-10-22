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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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

#ಆದರೆ defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ALARM_TIMEOUT
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#ವ್ಯಾಖ್ಯಾನಿಸು IS_RESOLV_FAIL(ಫಲಿತಾಂಶ) \
  (((ಫಲಿತಾಂಶ) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((ಫಲಿತಾಂಶ) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
ಸ್ಥಿರ bool ipv6works(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು ipv6works(x) FALSE
#ಆದರೆ_ಅಂತ್ಯ

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

uint8_t Curl_resolv_dns_queries(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint8_t ip_ಆವೃತ್ತಿ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆಯ್ಕೆ(ip_ಆವೃತ್ತಿ) {
  ಸ್ಥಿತಿ CURL_IPRESOLVE_V6:
    ಹಿಂತಿರುಗಿ CURL_DNSQ_AAAA;
  ಸ್ಥಿತಿ CURL_IPRESOLVE_V4:
    ಹಿಂತಿರುಗಿ CURL_DNSQ_A;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(ipv6works(ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURL_DNSQ_A;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_resolv_query_str(uint8_t dns_queries)
{
  ಆಯ್ಕೆ(dns_queries) {
  ಸ್ಥಿತಿ (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ಹಿಂತಿರುಗಿ "A+AAAA+HTTPS";
  ಸ್ಥಿತಿ (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ಹಿಂತಿರುಗಿ "A+AAAA";
  ಸ್ಥಿತಿ (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ಹಿಂತಿರುಗಿ "AAAA+HTTPS";
  ಸ್ಥಿತಿ (CURL_DNSQ_AAAA):
    ಹಿಂತಿರುಗಿ "AAAA";
  ಸ್ಥಿತಿ (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ಹಿಂತಿರುಗಿ "A+HTTPS";
  ಸ್ಥಿತಿ (CURL_DNSQ_A):
    ಹಿಂತಿರುಗಿ "A";
  ಸ್ಥಿತಿ (CURL_DNSQ_HTTPS):
    ಹಿಂತಿರುಗಿ "HTTPS";
  ಸ್ಥಿತಿ 0:
    ಹಿಂತಿರುಗಿ "-";
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ "???";
  }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
ಶೂನ್ಯ Curl_printable_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai, ಅಕ್ಷರ *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  ಆಯ್ಕೆ(ai->ai_family) {
  ಸ್ಥಿತಿ AF_INET: {
    ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_in *sa4 = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ai->ai_addr;
    ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ipaddr4 = &sa4->sin_addr;
    (ಶೂನ್ಯ)curlx_inet_ntop(ai->ai_family, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ipaddr4, buf, bufsize);
    ನಿಲ್ಲಿಸು;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ AF_INET6: {
    ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_in6 *sa6 = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ai->ai_addr;
    ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ipaddr6 = &sa6->sin6_addr;
    (ಶೂನ್ಯ)curlx_inet_ntop(ai->ai_family, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ipaddr6, buf, bufsize);
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
ಸ್ಥಿರ sigjmp_buf curl_jmpenv;
ಸ್ಥಿರ curl_simple_ಬೀಗ curl_jmpenv_ಬೀಗ = CURL_SIMPLE_ಬೀಗ_INIT;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
/* return a static IPv6 ::1 for the name */
ಸ್ಥಿರ ರಚನೆ Curl_addrinfo *get_localhost6(uint16_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ರಚನೆ Curl_addrinfo *ca;
  ಸ್ಥಿರಾಂಕ size_t ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
  ಸ್ಥಿರಾಂಕ size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
  ರಚನೆ sockaddr_in6 sa6;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ipv6[16];
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ port16 = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)(ಪೋರ್ಟ್ & 0xffff);
  ca = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ + hostlen + 1);
  ಆದರೆ(!ca)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  memset(&sa6, 0, ಗಾತ್ರ(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (ಶೂನ್ಯ)curlx_inet_pton(AF_INET6, "::1", ipv6);
  ಸ್ಮರಣೆ_ನಕಲು(&sa6.sin6_addr, ipv6, ಗಾತ್ರ(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ಗಾತ್ರ;
  ca->ai_ಮುಂದಿನ      = ಖಾಲಿ;
  ca->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
  ಸ್ಮರಣೆ_ನಕಲು(ca->ai_addr, &sa6, ss_ಗಾತ್ರ);
  ca->ai_canonname = (ಅಕ್ಷರ *)ca->ai_addr + ss_ಗಾತ್ರ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ಹಿಂತಿರುಗಿ ca;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು get_localhost6(x, y) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

/* return a static IPv4 127.0.0.1 for the given name */
ಸ್ಥಿರ ರಚನೆ Curl_addrinfo *get_localhost(uint16_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ರಚನೆ Curl_addrinfo *ca;
  ರಚನೆ Curl_addrinfo *ca6;
  ಸ್ಥಿರಾಂಕ size_t ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
  ಸ್ಥಿರಾಂಕ size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
  ರಚನೆ sockaddr_in sa;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ipv4;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ port16 = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)(ಪೋರ್ಟ್ & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, ಗಾತ್ರ(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  ಆದರೆ(curlx_inet_pton(AF_INET, "127.0.0.1", (ಅಕ್ಷರ *)&ipv4) < 1)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಸ್ಮರಣೆ_ನಕಲು(&sa.sin_addr, &ipv4, ಗಾತ್ರ(ipv4));

  ca = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ + hostlen + 1);
  ಆದರೆ(!ca)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_ಗಾತ್ರ;
  ca->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
  ಸ್ಮರಣೆ_ನಕಲು(ca->ai_addr, &sa, ss_ಗಾತ್ರ);
  ca->ai_canonname = (ಅಕ್ಷರ *)ca->ai_addr + ss_ಗಾತ್ರ;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(ಪೋರ್ಟ್, name);
  ಆದರೆ(!ca6)
    ಹಿಂತಿರುಗಿ ca;
  ca6->ai_ಮುಂದಿನ = ca;
  ಹಿಂತಿರುಗಿ ca6;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(ರಚನೆ Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_ಕೆಲಸಮಾಡುತ್ತದೆ = FALSE;
  ಆದರೆ(s == CURL_SOCKET_BAD) {
    ಆದರೆ(SOCKERRNO == SOCKENOMEM)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    multi->ipv6_ಕೆಲಸಮಾಡುತ್ತದೆ = TRUE;
    sclose(s);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
ಸ್ಥಿರ bool ipv6works(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->multi->ipv6_ಕೆಲಸಮಾಡುತ್ತದೆ : FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname)
{
  ರಚನೆ in_addr ಒಳ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ in6_addr in6;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(curlx_inet_pton(AF_INET, hostname, &ಒಳ) > 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#ಆದರೆ_ಅಂತ್ಯ
    )
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
ಸ್ಥಿರ bool tailmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುಂಬಿದ, size_t flen,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *part, size_t plen)
{
  ಆದರೆ(plen > flen)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ curl_strnequal(part, &ತುಂಬಿದ[flen - plen], plen);
}

ಸ್ಥಿರ CURLcode hostip_resolv_failed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_peer *peer,
                                     bool for_proxy)
{
  failf(ದತ್ತಾಂಶ, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ಹಿಂತಿರುಗಿ RESOLV_FAIL(for_proxy);
}

ಸ್ಥಿರ bool can_resolve_dns_queries(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t dns_queries)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

CURLcode Curl_resolv_announce_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಶೂನ್ಯ *resolver)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.resolver_ಆರಂಭ) {
    ರಚನೆ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಂಕ rc;

    CURL_TRC_DNS(ದತ್ತಾಂಶ, "announcing resolve to application");
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_resolver_ಆರಂಭ);
    rc = ದತ್ತಾಂಶ->ಗಣ.resolver_ಆರಂಭ(resolver, ಖಾಲಿ,
                                  ದತ್ತಾಂಶ->ಗಣ.resolver_ಆರಂಭ_client);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ಆದರೆ(rc) {
      CURL_TRC_DNS(ದತ್ತಾಂಶ, "application aborted resolve");
      ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC

ಸ್ಥಿರ ರಚನೆ Curl_resolv_async *hostip_async_new(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                  uint8_t dns_queries,
                                                  ರಚನೆ Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  ರಚನೆ Curl_resolv_async *async;

  ಆದರೆ(!ದತ್ತಾಂಶ->multi) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, ಗಾತ್ರ(*async));
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  ಆದರೆ(ದತ್ತಾಂಶ->multi->ಕೊನೆಯ_resolv_id == UINT32_MAX)
    ದತ್ತಾಂಶ->multi->ಕೊನೆಯ_resolv_id = 1; /* wrap around */
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->multi->ಕೊನೆಯ_resolv_id++;
  async->id = ದತ್ತಾಂಶ->multi->ಕೊನೆಯ_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->ಆರಂಭ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  ಆದರೆ(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ಹಿಂತಿರುಗಿ async;
}

ಸ್ಥಿರ CURLcode hostip_resolv_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_resolv_async *async,
                                          ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* If async resolving is ongoing, this must be set */
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಆದರೆ(async->doh)
    ಫಲಿತಾಂಶ = Curl_doh_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಫಲಿತಾಂಶ = Curl_async_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->ಪೋರ್ಟ್);
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(IS_RESOLV_FAIL(ಫಲಿತಾಂಶ)) {
    ಫಲಿತಾಂಶ = Curl_async_failed(ದತ್ತಾಂಶ, async, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, async->peer->hostname, async->peer->ಪೋರ್ಟ್);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->ಪೋರ್ಟ್);
    DEBUGASSERT(*pdns);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

timediff_t Curl_resolv_elapsed_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  uint32_t resolv_id,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ CURL_TIMEOUT_RESOLVE_MS;
  ಹಿಂತಿರುಗಿ curlx_ptimediff_ms(pnow, &async->ಆರಂಭ);
}

bool Curl_resolv_has_answers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  ಆದರೆ(!async || async->ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  ಆದರೆ((check_queries & async->dns_responses) != check_queries) {
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *Curl_resolv_get_ai(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                               uint32_t resolv_id,
                                               ಪೂರ್ಣಾಂಕ ai_family,
                                               ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ಆದರೆ(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆಯ್ಕೆ(ai_family) {
  ಸ್ಥಿತಿ AF_INET:
    ಹಿಂತಿರುಗಿ Curl_addrinfo_get(async->ai_A, ai_family, ಸೂಚ್ಯಂಕ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ AF_INET6:
    ಹಿಂತಿರುಗಿ Curl_addrinfo_get(async->ai_AAAA, ai_family, ಸೂಚ್ಯಂಕ);
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

CURLcode Curl_resolv_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ಹಿಂತಿರುಗಿ Curl_resolv(ದತ್ತಾಂಶ, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *
Curl_resolv_get_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id)
{
  ರಚನೆ Curl_resolv_async *async;
  ಆದರೆ(!Curl_resolv_knows_https(ದತ್ತಾಂಶ, resolv_id))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ಹಿಂತಿರುಗಿ async ? async->httpsrr : ಖಾಲಿ;
}

bool Curl_resolv_knows_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ಆದರೆ(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ಹಿಂತಿರುಗಿ ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ಹಿಂತಿರುಗಿ TRUE; /* we know it will never come */
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
ಸ್ಥಿರ CURLcode hostip_resolv_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t dns_queries,
                                    ರಚನೆ Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    ರಚನೆ Curl_dns_ನಮೂದು **pdns,
                                    bool *pnegative)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ರಚನೆ Curl_resolv_async *async = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ Curl_addrinfo *addr = ಖಾಲಿ;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pnegative = FALSE;

  (ಶೂನ್ಯ)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ಖಾಲಿ;

  /* Check for "known" things to resolve ourselves. */
  ಆದರೆ(addr_queries) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_RESOLVE_ON_IPS
    ಆದರೆ(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, ಖಾಲಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      ಫಲಿತಾಂಶ = Curl_str2addr(peer->hostname, peer->ಪೋರ್ಟ್, &addr);
      ಹೋಗು ಹೊರಹೋಗು;
    }
#ಆದರೆ_ಅಂತ್ಯ

    hostname_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->hostname);
    ಆದರೆ(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, ಖಾಲಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      addr = get_localhost(peer->ಪೋರ್ಟ್, peer->hostname);
      ಆದರೆ(!addr)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಆದರೆ(!Curl_is_ipaddr(peer->hostname) && allowDOH && ದತ್ತಾಂಶ->ಗಣ.doh) {
    ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(!async) {
      async = hostip_async_new(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      ಆದರೆ(!async) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಫಲಿತಾಂಶ = Curl_doh(ದತ್ತಾಂಶ, async);
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)allowDOH;
#ಆದರೆ_ಅಂತ್ಯ

  /* Can we provide the requested IP specifics in resolving? */
  ಆದರೆ(!can_resolve_dns_queries(ದತ್ತಾಂಶ, dns_queries)) {
    ಫಲಿತಾಂಶ = RESOLV_FAIL(for_proxy);
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_ASYNCH
  (ಶೂನ್ಯ)addr;
  ಆದರೆ(!async) {
    async = hostip_async_new(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    ಆದರೆ(!async) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಫಲಿತಾಂಶ = Curl_async_getaddrinfo(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);
    ಆದರೆ(r2)
      ಫಲಿತಾಂಶ = r2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*pdns)
      ಫಲಿತಾಂಶ = CURLE_OK;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, ಖಾಲಿ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  addr = Curl_sync_getaddrinfo(ದತ್ತಾಂಶ, dns_queries, peer->hostname, peer->ಪೋರ್ಟ್,
                               transport);
  ಆದರೆ(!addr) {
    ಫಲಿತಾಂಶ = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(ದತ್ತಾಂಶ, dns_queries, &addr, peer);
      ಆದರೆ(!*pdns)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!*pdns)
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*pdns)
    Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, pdns);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(addr)
    Curl_freeaddrinfo(addr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ಆದರೆ(async) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->ಮುಂದಿನ = ದತ್ತಾಂಶ->state.async;
      ದತ್ತಾಂಶ->state.async = async;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(ದತ್ತಾಂಶ, async);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode hostip_resolv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              uint8_t dns_queries,
                              ರಚನೆ Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  size_t hostname_len;
  CURLcode ಫಲಿತಾಂಶ = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (ಶೂನ್ಯ)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_DOH
  (ಶೂನ್ಯ)allowDOH;
#ಆದರೆ_ಅಂತ್ಯ

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->hostname);
  DEBUGASSERT(hostname_len);
  ಆದರೆ(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(ದತ್ತಾಂಶ, "Not resolving .onion address (RFC 7686)");
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->ಪೋರ್ಟ್);
  ಆದರೆ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(ದತ್ತಾಂಶ, "DEBUG fail ipv6 resolve");
    ಫಲಿತಾಂಶ = hostip_resolv_failed(ದತ್ತಾಂಶ, peer, for_proxy);
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* Let's check our DNS cache first */
  ಫಲಿತಾಂಶ = Curl_dnscache_get(ದತ್ತಾಂಶ, dns_queries, peer, pdns);
  ಆದರೆ(*pdns) {
    infof(ದತ್ತಾಂಶ, "Hostname %s was found in DNS cache", peer->hostname);
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    infof(ದತ್ತಾಂಶ, "Negative DNS entry");
    ಫಲಿತಾಂಶ = hostip_resolv_failed(ದತ್ತಾಂಶ, peer, for_proxy);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    ಫಲಿತಾಂಶ = hostip_resolv_ಆರಂಭ(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN)) {
    Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, pdns);
    ಆದರೆ(IS_RESOLV_FAIL(ಫಲಿತಾಂಶ)) {
      ಆದರೆ(cache_dns && negative)
        Curl_dnscache_ಕೂಡಿಸು_negative(ದತ್ತಾಂಶ, dns_queries, peer);
      ಆದರೆ(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(ದತ್ತಾಂಶ, "Could not resolve: %s:%u", peer->hostname, peer->ಪೋರ್ಟ್);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "Error %d resolving %s:%u",
            (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, peer->hostname, peer->ಪೋರ್ಟ್);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cache_dns && *pdns) {
    ಫಲಿತಾಂಶ = Curl_dnscache_ಕೂಡಿಸು(ದತ್ತಾಂಶ, *pdns);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, pdns);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_resolv_blocking(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              uint8_t dns_queries,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                              uint16_t ಪೋರ್ಟ್,
                              uint8_t transport,
                              ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = Curl_peer_create(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->scheme, hostname, ಪೋರ್ಟ್, &peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* We cannot do a blocking resolve using DoH currently */
  ಫಲಿತಾಂಶ = hostip_resolv(ದತ್ತಾಂಶ, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  ಆಯ್ಕೆ(ಫಲಿತಾಂಶ) {
  ಸ್ಥಿತಿ CURLE_OK:
    DEBUGASSERT(*pdns);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ಸ್ಥಿತಿ CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    ಫಲಿತಾಂಶ = Curl_async_await(ದತ್ತಾಂಶ, resolv_id, pdns);
    Curl_resolv_destroy(ದತ್ತಾಂಶ, resolv_id);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
  Curl_peer_unlink(&peer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN ಸ್ಥಿರ ಶೂನ್ಯ alarmfunc(ಪೂರ್ಣಾಂಕ sig)
{
  (ಶೂನ್ಯ)sig;
  siglongjmp(curl_jmpenv, 1);
}

ಸ್ಥಿರ CURLcode resolv_alarm_timeout(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     uint8_t dns_queries,
                                     ರಚನೆ Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     ರಚನೆ Curl_dns_ನಮೂದು **ನಮೂದು)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGACTION
  ರಚನೆ sigaction keep_sigact; /* store the old struct here */
  ಅಸ್ಥಿರ bool keep_copysig = FALSE; /* whether old sigact has been saved */
  ರಚನೆ sigaction sigact;
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGNAL
  ಶೂನ್ಯ (*keep_sigact)(ಪೂರ್ಣಾಂಕ);       /* store the old handler here */
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SIGNAL */
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SIGACTION */
  ಅಸ್ಥಿರ ದೀರ್ಘ timeout;
  ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ prev_alarm = 0;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!ದತ್ತಾಂಶ->ಗಣ.no_signal);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  DEBUGASSERT(!ದತ್ತಾಂಶ->ಗಣ.doh);
#ಆದರೆ_ಅಂತ್ಯ

  *ನಮೂದು = ಖಾಲಿ;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (ದೀರ್ಘ)timeout_ms;
  ಆದರೆ(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(ದತ್ತಾಂಶ,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_ಬೀಗ_ಬೀಗ(&curl_jmpenv_ಬೀಗ);

  ಆದರೆ(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(ದತ್ತಾಂಶ, "name lookup timed out");
    ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
    ಹೋಗು clean_ಮೇಲೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGACTION
    sigaction(SIGALRM, ಖಾಲಿ, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#ಆದರೆ_ಅಂತ್ಯ
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGNAL
    keep_sigact = ಸಂಕೇತ(SIGALRM, alarmfunc);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  ಫಲಿತಾಂಶ = hostip_resolv(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, ನಮೂದು);

clean_ಮೇಲೆ:
  ಆದರೆ(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGACTION
  ಆದರೆ(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ಖಾಲಿ); /* put it back */
  }
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  ಸಂಕೇತ(SIGALRM, keep_sigact);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SIGACTION */

  curl_simple_ಬೀಗ_unlock(&curl_jmpenv_ಬೀಗ);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  ಆದರೆ(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, Curl_pgrs_now(ದತ್ತಾಂಶ)) / 1000;

    /* the alarm period is counted in even number of seconds */
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ alarm_ಗಣ = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(prev_alarm - elapsed_secs);

    ಆದರೆ(!alarm_ಗಣ ||
       ((alarm_ಗಣ >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      failf(ದತ್ತಾಂಶ, "Previous alarm fired off");
    }
    ಇಲ್ಲದಿದ್ದರೆ
      alarm((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)alarm_ಗಣ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_ALARM_TIMEOUT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
ಸ್ಥಿರ CURLcode resolv_unix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *peer,
                            ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_addrinfo *addr;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(ದತ್ತಾಂಶ, "Unix socket path too long: '%s'", peer->hostname);
      ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_HOST;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *pdns = Curl_dnsc_mk_addr(ದತ್ತಾಂಶ, 0, &addr, peer);
  ಹಿಂತಿರುಗಿ *pdns ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ರಚನೆ Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  *presolv_id = 0;
  *pdns = ಖಾಲಿ;

  ಆದರೆ(timeout_ms < 0)
    /* got an already expired timeout */
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಆದರೆ((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ಹಿಂತಿರುಗಿ resolv_unix(ದತ್ತಾಂಶ, peer, pdns);
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(peer->unix_socket)
    ಹಿಂತಿರುಗಿ hostip_resolv_failed(ದತ್ತಾಂಶ, peer, for_proxy);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ALARM_TIMEOUT
  ಆದರೆ(dns_queries & CURL_DNSQ_ADDR) {
    ಆದರೆ(timeout_ms && ದತ್ತಾಂಶ->ಗಣ.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    ಆದರೆ(timeout_ms && !Curl_doh_wanted(ದತ್ತಾಂಶ)) {
      ಹಿಂತಿರುಗಿ resolv_alarm_timeout(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* !USE_ALARM_TIMEOUT */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURLRES_ASYNCH
  ಆದರೆ(timeout_ms)
    infof(ದತ್ತಾಂಶ, "timeout on name lookup is not supported");
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ hostip_resolv(ದತ್ತಾಂಶ, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC

ರಚನೆ Curl_resolv_async *Curl_async_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         uint32_t resolv_id)
{
  ರಚನೆ Curl_resolv_async *async = ದತ್ತಾಂಶ->state.async;
  ಗಾಗಿ(; async; async = async->ಮುಂದಿನ) {
    ಆದರೆ(async->id == resolv_id)
      ಹಿಂತಿರುಗಿ async;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode Curl_resolv_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id,
                                 ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  CURLcode ಫಲಿತಾಂಶ;

  /* If async resolving is ongoing, this must be set */
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  ಫಲಿತಾಂಶ = Curl_dnscache_get(ದತ್ತಾಂಶ, async->dns_queries, async->peer, pdns);
  ಆದರೆ(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(ದತ್ತಾಂಶ, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(ದತ್ತಾಂಶ, async);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    Curl_async_shutdown(ದತ್ತಾಂಶ, async);
    ಹಿಂತಿರುಗಿ Curl_async_failed(ದತ್ತಾಂಶ, async, ಖಾಲಿ);
  }

  ಫಲಿತಾಂಶ = hostip_resolv_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);

  ಆದರೆ(*pdns) {
    /* Add to cache */
    ಫಲಿತಾಂಶ = Curl_dnscache_ಕೂಡಿಸು(ದತ್ತಾಂಶ, *pdns);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, pdns);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(IS_RESOLV_FAIL(ಫಲಿತಾಂಶ)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    ಆದರೆ(async->negative_answer)
      Curl_dnscache_ಕೂಡಿಸು_negative(ದತ್ತಾಂಶ, async->dns_queries, async->peer);
    ಆದರೆ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ದತ್ತಾಂಶ, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->ಪೋರ್ಟ್);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "Error %d resolving %s:%u",
          (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, async->peer->hostname, async->peer->ಪೋರ್ಟ್);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_resolv_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ Curl_resolv_async *async = ದತ್ತಾಂಶ->state.async;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)ps;
  ಗಾಗಿ(; async && !ಫಲಿತಾಂಶ; async = async->ಮುಂದಿನ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
    ಆದರೆ(async->doh) /* DoH has nothing for the pollset */
      ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = Curl_async_pollset(ದತ್ತಾಂಶ, async, ps);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_resolv_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id)
{
  ರಚನೆ Curl_resolv_async **panchor = &ದತ್ತಾಂಶ->state.async;

  ಗಾಗಿ(; *panchor; panchor = &(*panchor)->ಮುಂದಿನ) {
    ರಚನೆ Curl_resolv_async *async = *panchor;
    ಆದರೆ(async->id == resolv_id) {
      *panchor = async->ಮುಂದಿನ;
      Curl_async_destroy(ದತ್ತಾಂಶ, async);
      ನಿಲ್ಲಿಸು;
    }
  }
}

ಶೂನ್ಯ Curl_resolv_shutdown_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_resolv_async *async = ದತ್ತಾಂಶ->state.async;
  ಗಾಗಿ(; async; async = async->ಮುಂದಿನ) {
    Curl_async_shutdown(ದತ್ತಾಂಶ, async);
  }
}

ಶೂನ್ಯ Curl_resolv_destroy_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆಗ(ದತ್ತಾಂಶ->state.async) {
    ರಚನೆ Curl_resolv_async *async = ದತ್ತಾಂಶ->state.async;
    ದತ್ತಾಂಶ->state.async = async->ಮುಂದಿನ;
    Curl_async_destroy(ದತ್ತಾಂಶ, async);
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */
