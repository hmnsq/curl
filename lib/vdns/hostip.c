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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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

#यदि defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#परिभाषय USE_ALARM_TIMEOUT
#यदि_अन्तः

#परिभाषय RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#परिभाषय IS_RESOLV_FAIL(परिणाम) \
  (((परिणाम) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((परिणाम) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#यदि_परिभाषितम् USE_IPV6
स्थिर bool ipv6works(संरचना Curl_easy *दत्त);
#अन्यथा
#परिभाषय ipv6works(x) FALSE
#यदि_अन्तः

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

uint8_t Curl_resolv_dns_queries(संरचना Curl_easy *दत्त, uint8_t ip_संस्करणम्)
{
  (शून्य)दत्त;
  विकल्प(ip_संस्करणम्) {
  पक्ष CURL_IPRESOLVE_V6:
    प्रत्यागम CURL_DNSQ_AAAA;
  पक्ष CURL_IPRESOLVE_V4:
    प्रत्यागम CURL_DNSQ_A;
  स्वभाव:
    यदि(ipv6works(दत्त))
      प्रत्यागम (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    अन्यथा
      प्रत्यागम CURL_DNSQ_A;
  }
}

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_resolv_query_str(uint8_t dns_queries)
{
  विकल्प(dns_queries) {
  पक्ष (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    प्रत्यागम "A+AAAA+HTTPS";
  पक्ष (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    प्रत्यागम "A+AAAA";
  पक्ष (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    प्रत्यागम "AAAA+HTTPS";
  पक्ष (CURL_DNSQ_AAAA):
    प्रत्यागम "AAAA";
  पक्ष (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    प्रत्यागम "A+HTTPS";
  पक्ष (CURL_DNSQ_A):
    प्रत्यागम "A";
  पक्ष (CURL_DNSQ_HTTPS):
    प्रत्यागम "HTTPS";
  पक्ष 0:
    प्रत्यागम "-";
  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम "???";
  }
}
#यदि_अन्तः

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
शून्य Curl_printable_स्थानाङ्कः(ध्रुव संरचना Curl_addrinfo *ai, वर्ण *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  विकल्प(ai->ai_family) {
  पक्ष AF_INET: {
    ध्रुव संरचना sockaddr_in *sa4 = (ध्रुव शून्य *)ai->ai_addr;
    ध्रुव संरचना in_addr *ipaddr4 = &sa4->sin_addr;
    (शून्य)curlx_inet_ntop(ai->ai_family, (ध्रुव शून्य *)ipaddr4, buf, bufsize);
    भङ्ग;
  }
#यदि_परिभाषितम् USE_IPV6
  पक्ष AF_INET6: {
    ध्रुव संरचना sockaddr_in6 *sa6 = (ध्रुव शून्य *)ai->ai_addr;
    ध्रुव संरचना in6_addr *ipaddr6 = &sa6->sin6_addr;
    (शून्य)curlx_inet_ntop(ai->ai_family, (ध्रुव शून्य *)ipaddr6, buf, bufsize);
    भङ्ग;
  }
#यदि_अन्तः
  स्वभाव:
    भङ्ग;
  }
}

#यदि_परिभाषितम् USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
स्थिर sigjmp_buf curl_jmpenv;
स्थिर curl_simple_तालकम् curl_jmpenv_तालकम् = CURL_SIMPLE_तालकम्_INIT;
#यदि_अन्तः

#यदि_परिभाषितम् USE_IPV6
/* return a static IPv6 ::1 for the name */
स्थिर संरचना Curl_addrinfo *get_localhost6(uint16_t पत्तनम्, ध्रुव वर्ण *name)
{
  संरचना Curl_addrinfo *ca;
  ध्रुव size_t ss_आकार = परिमाण(संरचना sockaddr_in6);
  ध्रुव size_t hostlen = वाक्यदैर्घ्यम्(name);
  संरचना sockaddr_in6 sa6;
  अचिह्नित वर्ण ipv6[16];
  अचिह्नित ह्रस्व port16 = (अचिह्नित ह्रस्व)(पत्तनम् & 0xffff);
  ca = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + ss_आकार + hostlen + 1);
  यदि(!ca)
    प्रत्यागम रिक्त;

  स्मृतिं_पूरय(&sa6, 0, परिमाण(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (शून्य)curlx_inet_pton(AF_INET6, "::1", ipv6);
  स्मृतिं_प्रतिलिख(&sa6.sin6_addr, ipv6, परिमाण(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
  ca->ai_अग्र      = रिक्त;
  ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना Curl_addrinfo));
  स्मृतिं_प्रतिलिख(ca->ai_addr, &sa6, ss_आकार);
  ca->ai_canonname = (वर्ण *)ca->ai_addr + ss_आकार;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  प्रत्यागम ca;
}
#अन्यथा
#परिभाषय get_localhost6(x, y) रिक्त
#यदि_अन्तः

/* return a static IPv4 127.0.0.1 for the given name */
स्थिर संरचना Curl_addrinfo *get_localhost(uint16_t पत्तनम्, ध्रुव वर्ण *name)
{
  संरचना Curl_addrinfo *ca;
  संरचना Curl_addrinfo *ca6;
  ध्रुव size_t ss_आकार = परिमाण(संरचना sockaddr_in);
  ध्रुव size_t hostlen = वाक्यदैर्घ्यम्(name);
  संरचना sockaddr_in sa;
  अचिह्नित पूर्णाङ्क ipv4;
  अचिह्नित ह्रस्व port16 = (अचिह्नित ह्रस्व)(पत्तनम् & 0xffff);

  /* memset to clear the sa.sin_zero field */
  स्मृतिं_पूरय(&sa, 0, परिमाण(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  यदि(curlx_inet_pton(AF_INET, "127.0.0.1", (वर्ण *)&ipv4) < 1)
    प्रत्यागम रिक्त;
  स्मृतिं_प्रतिलिख(&sa.sin_addr, &ipv4, परिमाण(ipv4));

  ca = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + ss_आकार + hostlen + 1);
  यदि(!ca)
    प्रत्यागम रिक्त;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
  ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना Curl_addrinfo));
  स्मृतिं_प्रतिलिख(ca->ai_addr, &sa, ss_आकार);
  ca->ai_canonname = (वर्ण *)ca->ai_addr + ss_आकार;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(पत्तनम्, name);
  यदि(!ca6)
    प्रत्यागम ca;
  ca6->ai_अग्र = ca;
  प्रत्यागम ca6;
}

#यदि_परिभाषितम् USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(संरचना Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_कार्यं_करोति = FALSE;
  यदि(s == CURL_SOCKET_BAD) {
    यदि(SOCKERRNO == SOCKENOMEM)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा {
    multi->ipv6_कार्यं_करोति = TRUE;
    sclose(s);
  }
  प्रत्यागम CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
स्थिर bool ipv6works(संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->multi);
  प्रत्यागम दत्त ? दत्त->multi->ipv6_कार्यं_करोति : FALSE;
}
#यदि_अन्तः /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(ध्रुव वर्ण *hostname)
{
  संरचना in_addr अन्तर्;
#यदि_परिभाषितम् USE_IPV6
  संरचना in6_addr in6;
#यदि_अन्तः
  यदि(curlx_inet_pton(AF_INET, hostname, &अन्तर्) > 0
#यदि_परिभाषितम् USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#यदि_अन्तः
    )
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
स्थिर bool tailmatch(ध्रुव वर्ण *पूर्णम्, size_t flen,
                      ध्रुव वर्ण *part, size_t plen)
{
  यदि(plen > flen)
    प्रत्यागम FALSE;
  प्रत्यागम curl_strnequal(part, &पूर्णम्[flen - plen], plen);
}

स्थिर CURLcode hostip_resolv_failed(संरचना Curl_easy *दत्त,
                                     संरचना Curl_peer *peer,
                                     bool for_proxy)
{
  failf(दत्त, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  प्रत्यागम RESOLV_FAIL(for_proxy);
}

स्थिर bool can_resolve_dns_queries(संरचना Curl_easy *दत्त,
                                    uint8_t dns_queries)
{
  (शून्य)दत्त;
  यदि((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(दत्त))
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

CURLcode Curl_resolv_announce_start(संरचना Curl_easy *दत्त,
                                    शून्य *resolver)
{
  यदि(दत्त->समुच्चयः.resolver_start) {
    संरचना Curl_mapi_guard guard;
    पूर्णाङ्क rc;

    CURL_TRC_DNS(दत्त, "announcing resolve to application");
    CURL_CBAPI_START(&guard, दत्त, easy_resolver_start);
    rc = दत्त->समुच्चयः.resolver_start(resolver, रिक्त,
                                  दत्त->समुच्चयः.resolver_start_client);
    CURL_CBAPI_अन्तः(&guard);
    यदि(rc) {
      CURL_TRC_DNS(दत्त, "application aborted resolve");
      प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
    }
  }
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् USE_CURL_ASYNC

स्थिर संरचना Curl_resolv_async *hostip_async_new(संरचना Curl_easy *दत्त,
                                                  uint8_t dns_queries,
                                                  संरचना Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  संरचना Curl_resolv_async *async;

  यदि(!दत्त->multi) {
    DEBUGASSERT(0);
    प्रत्यागम रिक्त;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, परिमाण(*async));
  यदि(!async)
    प्रत्यागम रिक्त;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  यदि(दत्त->multi->चरमः_resolv_id == UINT32_MAX)
    दत्त->multi->चरमः_resolv_id = 1; /* wrap around */
  अन्यथा
    दत्त->multi->चरमः_resolv_id++;
  async->id = दत्त->multi->चरमः_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(दत्त);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  यदि(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  प्रत्यागम async;
}

स्थिर CURLcode hostip_resolv_take_परिणाम(संरचना Curl_easy *दत्त,
                                          संरचना Curl_resolv_async *async,
                                          संरचना Curl_dns_प्रविष्टिः **pdns)
{
  CURLcode परिणाम;

  /* If async resolving is ongoing, this must be set */
  यदि(!async)
    प्रत्यागम CURLE_FAILED_INIT;

#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  यदि(async->doh)
    परिणाम = Curl_doh_take_परिणाम(दत्त, async, pdns);
  अन्यथा
#यदि_अन्तः
  परिणाम = Curl_async_take_परिणाम(दत्त, async, pdns);

  यदि(परिणाम == CURLE_AGAIN) {
    CURL_TRC_DNS(दत्त, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->पत्तनम्);
    परिणाम = CURLE_OK;
  }
  अन्यथा यदि(IS_RESOLV_FAIL(परिणाम)) {
    परिणाम = Curl_async_failed(दत्त, async, रिक्त);
  }
  अन्यथा यदि(परिणाम) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(दत्त, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (पूर्णाङ्क)परिणाम, async->peer->hostname, async->peer->पत्तनम्);
  }
  अन्यथा {
    CURL_TRC_DNS(दत्त, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->पत्तनम्);
    DEBUGASSERT(*pdns);
  }

  प्रत्यागम परिणाम;
}

timediff_t Curl_resolv_elapsed_ms(संरचना Curl_easy *दत्त,
                                  uint32_t resolv_id,
                                  ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  यदि(!async)
    प्रत्यागम CURL_TIMEOUT_RESOLVE_MS;
  प्रत्यागम curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(संरचना Curl_easy *दत्त,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  यदि(!async || async->समाप्तम्)
    प्रत्यागम TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  यदि((check_queries & async->dns_responses) != check_queries) {
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

ध्रुव संरचना Curl_addrinfo *Curl_resolv_get_ai(संरचना Curl_easy *दत्त,
                                               uint32_t resolv_id,
                                               पूर्णाङ्क ai_family,
                                               अचिह्नित पूर्णाङ्क सूचकांक)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  यदि(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    प्रत्यागम रिक्त;
  विकल्प(ai_family) {
  पक्ष AF_INET:
    प्रत्यागम Curl_addrinfo_get(async->ai_A, ai_family, सूचकांक);
#यदि_परिभाषितम् USE_IPV6
  पक्ष AF_INET6:
    प्रत्यागम Curl_addrinfo_get(async->ai_AAAA, ai_family, सूचकांक);
#यदि_अन्तः
  स्वभाव:
    प्रत्यागम रिक्त;
  }
}

#यदि_परिभाषितम् USE_HTTPSRR

CURLcode Curl_resolv_https(संरचना Curl_easy *दत्त,
                           संरचना Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           संरचना Curl_dns_प्रविष्टिः **pdns)
{
  प्रत्यागम Curl_resolv(दत्त, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

ध्रुव संरचना Curl_https_rrinfo *
Curl_resolv_get_https(संरचना Curl_easy *दत्त, uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async;
  यदि(!Curl_resolv_knows_https(दत्त, resolv_id))
    प्रत्यागम रिक्त;
  async = Curl_async_get(दत्त, resolv_id);
  प्रत्यागम async ? async->httpsrr : रिक्त;
}

bool Curl_resolv_knows_https(संरचना Curl_easy *दत्त, uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  यदि(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    प्रत्यागम ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  प्रत्यागम TRUE; /* we know it will never come */
}

#यदि_अन्तः /* USE_HTTPSRR */

#यदि_अन्तः /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
स्थिर CURLcode hostip_resolv_start(संरचना Curl_easy *दत्त,
                                    uint8_t dns_queries,
                                    संरचना Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    संरचना Curl_dns_प्रविष्टिः **pdns,
                                    bool *pnegative)
{
#यदि_परिभाषितम् USE_CURL_ASYNC
  संरचना Curl_resolv_async *async = रिक्त;
#यदि_अन्तः
  संरचना Curl_addrinfo *addr = रिक्त;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode परिणाम = CURLE_OK;

  *pnegative = FALSE;

  (शून्य)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = रिक्त;

  /* Check for "known" things to resolve ourselves. */
  यदि(addr_queries) {
#यदि_अपरिभाषितम् USE_RESOLVE_ON_IPS
    यदि(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      परिणाम = Curl_resolv_announce_start(दत्त, रिक्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      परिणाम = Curl_str2addr(peer->hostname, peer->पत्तनम्, &addr);
      गच्छ बहिर्गमनम्;
    }
#यदि_अन्तः

    hostname_len = वाक्यदैर्घ्यम्(peer->hostname);
    यदि(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      परिणाम = Curl_resolv_announce_start(दत्त, रिक्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      addr = get_localhost(peer->पत्तनम्, peer->hostname);
      यदि(!addr)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  यदि(!Curl_is_ipaddr(peer->hostname) && allowDOH && दत्त->समुच्चयः.doh) {
    परिणाम = Curl_resolv_announce_start(दत्त, रिक्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    यदि(!async) {
      async = hostip_async_new(दत्त, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      यदि(!async) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
    परिणाम = Curl_doh(दत्त, async);
    गच्छ बहिर्गमनम्;
  }
#अन्यथा
  (शून्य)allowDOH;
#यदि_अन्तः

  /* Can we provide the requested IP specifics in resolving? */
  यदि(!can_resolve_dns_queries(दत्त, dns_queries)) {
    परिणाम = RESOLV_FAIL(for_proxy);
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् CURLRES_ASYNCH
  (शून्य)addr;
  यदि(!async) {
    async = hostip_async_new(दत्त, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    यदि(!async) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
  परिणाम = Curl_async_getaddrinfo(दत्त, async);
  यदि(परिणाम == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_परिणाम(दत्त, async, pdns);
    यदि(r2)
      परिणाम = r2;
    अन्यथा यदि(*pdns)
      परिणाम = CURLE_OK;
  }
#अन्यथा
  परिणाम = Curl_resolv_announce_start(दत्त, रिक्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  addr = Curl_sync_getaddrinfo(दत्त, dns_queries, peer->hostname, peer->पत्तनम्,
                               transport);
  यदि(!addr) {
    परिणाम = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#यदि_अन्तः

बहिर्गमनम्:
  यदि(!परिणाम) {
    यदि(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(दत्त, dns_queries, &addr, peer);
      यदि(!*pdns)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    अन्यथा यदि(!*pdns)
      परिणाम = CURLE_AGAIN;
  }
  अन्यथा यदि(*pdns)
    Curl_dns_प्रविष्टिः_unlink(दत्त, pdns);
  अन्यथा यदि(addr)
    Curl_freeaddrinfo(addr);

#यदि_परिभाषितम् USE_CURL_ASYNC
  यदि(async) {
    यदि(परिणाम == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->अग्र = दत्त->state.async;
      दत्त->state.async = async;
    }
    अन्यथा {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(दत्त, async);
    }
  }
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

स्थिर CURLcode hostip_resolv(संरचना Curl_easy *दत्त,
                              uint8_t dns_queries,
                              संरचना Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              संरचना Curl_dns_प्रविष्टिः **pdns)
{
  size_t hostname_len;
  CURLcode परिणाम = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (शून्य)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = रिक्त;

#यदि_परिभाषितम् CURL_DISABLE_DOH
  (शून्य)allowDOH;
#यदि_अन्तः

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = वाक्यदैर्घ्यम्(peer->hostname);
  DEBUGASSERT(hostname_len);
  यदि(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(दत्त, "Not resolving .onion address (RFC 7686)");
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् DEBUGBUILD
  CURL_TRC_DNS(दत्त, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->पत्तनम्);
  यदि((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(दत्त, "DEBUG fail ipv6 resolve");
    परिणाम = hostip_resolv_failed(दत्त, peer, for_proxy);
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः
  /* Let's check our DNS cache first */
  परिणाम = Curl_dnscache_get(दत्त, dns_queries, peer, pdns);
  यदि(*pdns) {
    infof(दत्त, "Hostname %s was found in DNS cache", peer->hostname);
    परिणाम = CURLE_OK;
  }
  अन्यथा यदि(परिणाम) {
    infof(दत्त, "Negative DNS entry");
    परिणाम = hostip_resolv_failed(दत्त, peer, for_proxy);
  }
  अन्यथा {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    परिणाम = hostip_resolv_start(दत्त, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(दत्त, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (पूर्णाङ्क)परिणाम);
  }

बहिर्गमनम्:
  यदि(परिणाम && (परिणाम != CURLE_AGAIN)) {
    Curl_dns_प्रविष्टिः_unlink(दत्त, pdns);
    यदि(IS_RESOLV_FAIL(परिणाम)) {
      यदि(cache_dns && negative)
        Curl_dnscache_सङ्कलय_negative(दत्त, dns_queries, peer);
      यदि(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(दत्त, "Could not resolve: %s:%u", peer->hostname, peer->पत्तनम्);
    }
    अन्यथा {
      failf(दत्त, "Error %d resolving %s:%u",
            (पूर्णाङ्क)परिणाम, peer->hostname, peer->पत्तनम्);
    }
  }
  अन्यथा यदि(cache_dns && *pdns) {
    परिणाम = Curl_dnscache_सङ्कलय(दत्त, *pdns);
    यदि(परिणाम)
      Curl_dns_प्रविष्टिः_unlink(दत्त, pdns);
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_resolv_blocking(संरचना Curl_easy *दत्त,
                              uint8_t dns_queries,
                              ध्रुव वर्ण *hostname,
                              uint16_t पत्तनम्,
                              uint8_t transport,
                              संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode परिणाम;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = रिक्त;

  परिणाम = Curl_peer_create(दत्त, दत्त->conn->scheme, hostname, पत्तनम्, &peer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* We cannot do a blocking resolve using DoH currently */
  परिणाम = hostip_resolv(दत्त, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  विकल्प(परिणाम) {
  पक्ष CURLE_OK:
    DEBUGASSERT(*pdns);
    भङ्ग;
#यदि_परिभाषितम् USE_CURL_ASYNC
  पक्ष CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    परिणाम = Curl_async_await(दत्त, resolv_id, pdns);
    Curl_resolv_destroy(दत्त, resolv_id);
    भङ्ग;
#यदि_अन्तः
  स्वभाव:
    भङ्ग;
  }

बहिर्गमनम्:
  Curl_peer_unlink(&peer);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN स्थिर शून्य alarmfunc(पूर्णाङ्क sig)
{
  (शून्य)sig;
  siglongjmp(curl_jmpenv, 1);
}

स्थिर CURLcode resolv_alarm_timeout(संरचना Curl_easy *दत्त,
                                     uint8_t dns_queries,
                                     संरचना Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     संरचना Curl_dns_प्रविष्टिः **प्रविष्टिः)
{
#यदि_परिभाषितम् HAVE_SIGACTION
  संरचना sigaction keep_sigact; /* store the old struct here */
  चञ्चल bool keep_copysig = FALSE; /* whether old sigact has been saved */
  संरचना sigaction sigact;
#अन्यथा
#यदि_परिभाषितम् HAVE_SIGNAL
  शून्य (*keep_sigact)(पूर्णाङ्क);       /* store the old handler here */
#यदि_अन्तः /* HAVE_SIGNAL */
#यदि_अन्तः /* HAVE_SIGACTION */
  चञ्चल दीर्घ timeout;
  चञ्चल अचिह्नित पूर्णाङ्क पूर्व_alarm = 0;
  CURLcode परिणाम;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!दत्त->समुच्चयः.no_signal);
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  DEBUGASSERT(!दत्त->समुच्चयः.doh);
#यदि_अन्तः

  *प्रविष्टिः = रिक्त;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (दीर्घ)timeout_ms;
  यदि(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(दत्त,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_तालकम्_तालकम्(&curl_jmpenv_तालकम्);

  यदि(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(दत्त, "name lookup timed out");
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    गच्छ clean_ऊर्ध्वम्;
  }
  अन्यथा {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#यदि_परिभाषितम् HAVE_SIGACTION
    sigaction(SIGALRM, रिक्त, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#यदि_परिभाषितम् SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#यदि_अन्तः
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, रिक्त);
#अन्यथा /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#यदि_परिभाषितम् HAVE_SIGNAL
    keep_sigact = सङ्केत(SIGALRM, alarmfunc);
#यदि_अन्तः
#यदि_अन्तः /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    पूर्व_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  परिणाम = hostip_resolv(दत्त, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, प्रविष्टिः);

clean_ऊर्ध्वम्:
  यदि(!पूर्व_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#यदि_परिभाषितम् HAVE_SIGACTION
  यदि(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, रिक्त); /* put it back */
  }
#अन्यथा
#यदि_परिभाषितम् HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  सङ्केत(SIGALRM, keep_sigact);
#यदि_अन्तः
#यदि_अन्तः /* HAVE_SIGACTION */

  curl_simple_तालकम्_unlock(&curl_jmpenv_तालकम्);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  यदि(पूर्व_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(दत्त, दत्त->conn, Curl_pgrs_now(दत्त)) / 1000;

    /* the alarm period is counted in even number of seconds */
    अचिह्नित दीर्घ alarm_समुच्चयः = (अचिह्नित दीर्घ)(पूर्व_alarm - elapsed_secs);

    यदि(!alarm_समुच्चयः ||
       ((alarm_समुच्चयः >= 0x80000000) && (पूर्व_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      failf(दत्त, "Previous alarm fired off");
    }
    अन्यथा
      alarm((अचिह्नित पूर्णाङ्क)alarm_समुच्चयः);
  }

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_ALARM_TIMEOUT */

#यदि_परिभाषितम् USE_UNIX_SOCKETS
स्थिर CURLcode resolv_unix(संरचना Curl_easy *दत्त,
                            संरचना Curl_peer *peer,
                            संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_addrinfo *addr;
  CURLcode परिणाम;

  DEBUGASSERT(peer->unix_socket);
  *pdns = रिक्त;

  परिणाम = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(दत्त, "Unix socket path too long: '%s'", peer->hostname);
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
    प्रत्यागम परिणाम;
  }

  *pdns = Curl_dnsc_mk_addr(दत्त, 0, &addr, peer);
  प्रत्यागम *pdns ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}
#यदि_अन्तः /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(संरचना Curl_easy *दत्त,
                     संरचना Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     संरचना Curl_dns_प्रविष्टिः **pdns)
{
  *presolv_id = 0;
  *pdns = रिक्त;

  यदि(timeout_ms < 0)
    /* got an already expired timeout */
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  अन्यथा यदि(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#यदि_परिभाषितम् USE_UNIX_SOCKETS
  यदि((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    प्रत्यागम resolv_unix(दत्त, peer, pdns);
#अन्यथा
  यदि(peer->unix_socket)
    प्रत्यागम hostip_resolv_failed(दत्त, peer, for_proxy);
#यदि_अन्तः

#यदि_परिभाषितम् USE_ALARM_TIMEOUT
  यदि(dns_queries & CURL_DNSQ_ADDR) {
    यदि(timeout_ms && दत्त->समुच्चयः.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    यदि(timeout_ms && !Curl_doh_wanted(दत्त)) {
      प्रत्यागम resolv_alarm_timeout(दत्त, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#यदि_अन्तः /* !USE_ALARM_TIMEOUT */

#यदि_अपरिभाषितम् CURLRES_ASYNCH
  यदि(timeout_ms)
    infof(दत्त, "timeout on name lookup is not supported");
#यदि_अन्तः

  प्रत्यागम hostip_resolv(दत्त, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#यदि_परिभाषितम् USE_CURL_ASYNC

संरचना Curl_resolv_async *Curl_async_get(संरचना Curl_easy *दत्त,
                                         uint32_t resolv_id)
{
  संरचना Curl_resolv_async *async = दत्त->state.async;
  प्रति(; async; async = async->अग्र) {
    यदि(async->id == resolv_id)
      प्रत्यागम async;
  }
  प्रत्यागम रिक्त;
}

CURLcode Curl_resolv_take_परिणाम(संरचना Curl_easy *दत्त, uint32_t resolv_id,
                                 संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  CURLcode परिणाम;

  /* If async resolving is ongoing, this must be set */
  यदि(!async)
    प्रत्यागम CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  परिणाम = Curl_dnscache_get(दत्त, async->dns_queries, async->peer, pdns);
  यदि(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(दत्त, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(दत्त, async);
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(परिणाम) {
    Curl_async_shutdown(दत्त, async);
    प्रत्यागम Curl_async_failed(दत्त, async, रिक्त);
  }

  परिणाम = hostip_resolv_take_परिणाम(दत्त, async, pdns);

  यदि(*pdns) {
    /* Add to cache */
    परिणाम = Curl_dnscache_सङ्कलय(दत्त, *pdns);
    यदि(परिणाम)
      Curl_dns_प्रविष्टिः_unlink(दत्त, pdns);
  }
  अन्यथा यदि(IS_RESOLV_FAIL(परिणाम)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    यदि(async->negative_answer)
      Curl_dnscache_सङ्कलय_negative(दत्त, async->dns_queries, async->peer);
    यदि(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(दत्त, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->पत्तनम्);
  }
  अन्यथा यदि(परिणाम) {
    failf(दत्त, "Error %d resolving %s:%u",
          (पूर्णाङ्क)परिणाम, async->peer->hostname, async->peer->पत्तनम्);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_resolv_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  संरचना Curl_resolv_async *async = दत्त->state.async;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)ps;
  प्रति(; async && !परिणाम; async = async->अग्र) {
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
    यदि(async->doh) /* DoH has nothing for the pollset */
      अनुवर्त;
#यदि_अन्तः
    परिणाम = Curl_async_pollset(दत्त, async, ps);
  }
  प्रत्यागम परिणाम;
}

शून्य Curl_resolv_destroy(संरचना Curl_easy *दत्त, uint32_t resolv_id)
{
  संरचना Curl_resolv_async **panchor = &दत्त->state.async;

  प्रति(; *panchor; panchor = &(*panchor)->अग्र) {
    संरचना Curl_resolv_async *async = *panchor;
    यदि(async->id == resolv_id) {
      *panchor = async->अग्र;
      Curl_async_destroy(दत्त, async);
      भङ्ग;
    }
  }
}

शून्य Curl_resolv_shutdown_all(संरचना Curl_easy *दत्त)
{
  संरचना Curl_resolv_async *async = दत्त->state.async;
  प्रति(; async; async = async->अग्र) {
    Curl_async_shutdown(दत्त, async);
  }
}

शून्य Curl_resolv_destroy_all(संरचना Curl_easy *दत्त)
{
  यावत्(दत्त->state.async) {
    संरचना Curl_resolv_async *async = दत्त->state.async;
    दत्त->state.async = async->अग्र;
    Curl_async_destroy(दत्त, async);
  }
}

#यदि_अन्तः /* USE_CURL_ASYNC */
