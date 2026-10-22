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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IN6_H
#అన్తర్భావ <netinet/in6.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#అన్తర్భావ <signal.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "easy_lock.h"
#అన్తర్భావ "vdns/dnscache.h"
#అన్తర్భావ "vdns/doh.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strcopy.h"
#అన్తర్భావ "curlx/strparse.h"

#ఒకవేళ defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#నిర్వచించు USE_ALARM_TIMEOUT
#అయితే_ముగింపు

#నిర్వచించు RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#నిర్వచించు IS_RESOLV_FAIL(ఫలితం) \
  (((ఫలితం) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((ఫలితం) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#నిర్వచితమైతే USE_IPV6
స్థిర bool ipv6works(నిర్మాణం Curl_easy *దత్తాంశం);
#లేకపోతే
#నిర్వచించు ipv6works(x) FALSE
#అయితే_ముగింపు

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

uint8_t Curl_resolv_dns_queries(నిర్మాణం Curl_easy *దత్తాంశం, uint8_t ip_సంస్కరణ)
{
  (శూన్య)దత్తాంశం;
  ఎంపిక(ip_సంస్కరణ) {
  స్థితి CURL_IPRESOLVE_V6:
    తిరిగి CURL_DNSQ_AAAA;
  స్థితి CURL_IPRESOLVE_V4:
    తిరిగి CURL_DNSQ_A;
  అప్రమేయ:
    ఒకవేళ(ipv6works(దత్తాంశం))
      తిరిగి (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    లేకపోతే
      తిరిగి CURL_DNSQ_A;
  }
}

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_resolv_query_str(uint8_t dns_queries)
{
  ఎంపిక(dns_queries) {
  స్థితి (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    తిరిగి "A+AAAA+HTTPS";
  స్థితి (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    తిరిగి "A+AAAA";
  స్థితి (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    తిరిగి "AAAA+HTTPS";
  స్థితి (CURL_DNSQ_AAAA):
    తిరిగి "AAAA";
  స్థితి (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    తిరిగి "A+HTTPS";
  స్థితి (CURL_DNSQ_A):
    తిరిగి "A";
  స్థితి (CURL_DNSQ_HTTPS):
    తిరిగి "HTTPS";
  స్థితి 0:
    తిరిగి "-";
  అప్రమేయ:
    DEBUGASSERT(0);
    తిరిగి "???";
  }
}
#అయితే_ముగింపు

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
శూన్య Curl_printable_చిరునామా(స్థిరరాశి నిర్మాణం Curl_addrinfo *ai, అక్షరం *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  ఎంపిక(ai->ai_family) {
  స్థితి AF_INET: {
    స్థిరరాశి నిర్మాణం sockaddr_in *sa4 = (స్థిరరాశి శూన్య *)ai->ai_addr;
    స్థిరరాశి నిర్మాణం in_addr *ipaddr4 = &sa4->sin_addr;
    (శూన్య)curlx_inet_ntop(ai->ai_family, (స్థిరరాశి శూన్య *)ipaddr4, buf, bufsize);
    ఆపు;
  }
#నిర్వచితమైతే USE_IPV6
  స్థితి AF_INET6: {
    స్థిరరాశి నిర్మాణం sockaddr_in6 *sa6 = (స్థిరరాశి శూన్య *)ai->ai_addr;
    స్థిరరాశి నిర్మాణం in6_addr *ipaddr6 = &sa6->sin6_addr;
    (శూన్య)curlx_inet_ntop(ai->ai_family, (స్థిరరాశి శూన్య *)ipaddr6, buf, bufsize);
    ఆపు;
  }
#అయితే_ముగింపు
  అప్రమేయ:
    ఆపు;
  }
}

#నిర్వచితమైతే USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
స్థిర sigjmp_buf curl_jmpenv;
స్థిర curl_simple_తాళం curl_jmpenv_తాళం = CURL_SIMPLE_తాళం_INIT;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_IPV6
/* return a static IPv6 ::1 for the name */
స్థిర నిర్మాణం Curl_addrinfo *get_localhost6(uint16_t పోర్టు, స్థిరరాశి అక్షరం *name)
{
  నిర్మాణం Curl_addrinfo *ca;
  స్థిరరాశి size_t ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
  స్థిరరాశి size_t hostlen = అక్షరశ్రేణి_పొడవు(name);
  నిర్మాణం sockaddr_in6 sa6;
  గుర్తులేని అక్షరం ipv6[16];
  గుర్తులేని పొట్టి port16 = (గుర్తులేని పొట్టి)(పోర్టు & 0xffff);
  ca = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత + hostlen + 1);
  ఒకవేళ(!ca)
    తిరిగి ఖాళీ;

  memset(&sa6, 0, పరిమాణం(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (శూన్య)curlx_inet_pton(AF_INET6, "::1", ipv6);
  జ్ఞాపకం_నకలు(&sa6.sin6_addr, ipv6, పరిమాణం(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_కొలత;
  ca->ai_తదుపరి      = ఖాళీ;
  ca->ai_addr = (శూన్య *)((అక్షరం *)ca + పరిమాణం(నిర్మాణం Curl_addrinfo));
  జ్ఞాపకం_నకలు(ca->ai_addr, &sa6, ss_కొలత);
  ca->ai_canonname = (అక్షరం *)ca->ai_addr + ss_కొలత;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  తిరిగి ca;
}
#లేకపోతే
#నిర్వచించు get_localhost6(x, y) ఖాళీ
#అయితే_ముగింపు

/* return a static IPv4 127.0.0.1 for the given name */
స్థిర నిర్మాణం Curl_addrinfo *get_localhost(uint16_t పోర్టు, స్థిరరాశి అక్షరం *name)
{
  నిర్మాణం Curl_addrinfo *ca;
  నిర్మాణం Curl_addrinfo *ca6;
  స్థిరరాశి size_t ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);
  స్థిరరాశి size_t hostlen = అక్షరశ్రేణి_పొడవు(name);
  నిర్మాణం sockaddr_in sa;
  గుర్తులేని పూర్ణాంకం ipv4;
  గుర్తులేని పొట్టి port16 = (గుర్తులేని పొట్టి)(పోర్టు & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, పరిమాణం(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  ఒకవేళ(curlx_inet_pton(AF_INET, "127.0.0.1", (అక్షరం *)&ipv4) < 1)
    తిరిగి ఖాళీ;
  జ్ఞాపకం_నకలు(&sa.sin_addr, &ipv4, పరిమాణం(ipv4));

  ca = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత + hostlen + 1);
  ఒకవేళ(!ca)
    తిరిగి ఖాళీ;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_కొలత;
  ca->ai_addr = (శూన్య *)((అక్షరం *)ca + పరిమాణం(నిర్మాణం Curl_addrinfo));
  జ్ఞాపకం_నకలు(ca->ai_addr, &sa, ss_కొలత);
  ca->ai_canonname = (అక్షరం *)ca->ai_addr + ss_కొలత;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(పోర్టు, name);
  ఒకవేళ(!ca6)
    తిరిగి ca;
  ca6->ai_తదుపరి = ca;
  తిరిగి ca6;
}

#నిర్వచితమైతే USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(నిర్మాణం Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_పనిచేస్తుంది = FALSE;
  ఒకవేళ(s == CURL_SOCKET_BAD) {
    ఒకవేళ(SOCKERRNO == SOCKENOMEM)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే {
    multi->ipv6_పనిచేస్తుంది = TRUE;
    sclose(s);
  }
  తిరిగి CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
స్థిర bool ipv6works(నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->multi);
  తిరిగి దత్తాంశం ? దత్తాంశం->multi->ipv6_పనిచేస్తుంది : FALSE;
}
#అయితే_ముగింపు /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(స్థిరరాశి అక్షరం *hostname)
{
  నిర్మాణం in_addr లోపల;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం in6_addr in6;
#అయితే_ముగింపు
  ఒకవేళ(curlx_inet_pton(AF_INET, hostname, &లోపల) > 0
#నిర్వచితమైతే USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#అయితే_ముగింపు
    )
    తిరిగి TRUE;
  తిరిగి FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
స్థిర bool tailmatch(స్థిరరాశి అక్షరం *నిండిన, size_t flen,
                      స్థిరరాశి అక్షరం *part, size_t plen)
{
  ఒకవేళ(plen > flen)
    తిరిగి FALSE;
  తిరిగి curl_strnequal(part, &నిండిన[flen - plen], plen);
}

స్థిర CURLcode hostip_resolv_failed(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_peer *peer,
                                     bool for_proxy)
{
  failf(దత్తాంశం, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  తిరిగి RESOLV_FAIL(for_proxy);
}

స్థిర bool can_resolve_dns_queries(నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t dns_queries)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(దత్తాంశం))
    తిరిగి FALSE;
  తిరిగి TRUE;
}

CURLcode Curl_resolv_announce_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    శూన్య *resolver)
{
  ఒకవేళ(దత్తాంశం->సమితి.resolver_ప్రారంభం) {
    నిర్మాణం Curl_mapi_guard guard;
    పూర్ణాంకం rc;

    CURL_TRC_DNS(దత్తాంశం, "announcing resolve to application");
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_resolver_ప్రారంభం);
    rc = దత్తాంశం->సమితి.resolver_ప్రారంభం(resolver, ఖాళీ,
                                  దత్తాంశం->సమితి.resolver_ప్రారంభం_client);
    CURL_CBAPI_చివర(&guard);
    ఒకవేళ(rc) {
      CURL_TRC_DNS(దత్తాంశం, "application aborted resolve");
      తిరిగి CURLE_ABORTED_BY_CALLBACK;
    }
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే USE_CURL_ASYNC

స్థిర నిర్మాణం Curl_resolv_async *hostip_async_new(నిర్మాణం Curl_easy *దత్తాంశం,
                                                  uint8_t dns_queries,
                                                  నిర్మాణం Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  నిర్మాణం Curl_resolv_async *async;

  ఒకవేళ(!దత్తాంశం->multi) {
    DEBUGASSERT(0);
    తిరిగి ఖాళీ;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, పరిమాణం(*async));
  ఒకవేళ(!async)
    తిరిగి ఖాళీ;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  ఒకవేళ(దత్తాంశం->multi->చివరి_resolv_id == UINT32_MAX)
    దత్తాంశం->multi->చివరి_resolv_id = 1; /* wrap around */
  లేకపోతే
    దత్తాంశం->multi->చివరి_resolv_id++;
  async->id = దత్తాంశం->multi->చివరి_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->ప్రారంభం = *Curl_pgrs_now(దత్తాంశం);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  ఒకవేళ(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  తిరిగి async;
}

స్థిర CURLcode hostip_resolv_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_resolv_async *async,
                                          నిర్మాణం Curl_dns_నమోదు **pdns)
{
  CURLcode ఫలితం;

  /* If async resolving is ongoing, this must be set */
  ఒకవేళ(!async)
    తిరిగి CURLE_FAILED_INIT;

#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  ఒకవేళ(async->doh)
    ఫలితం = Curl_doh_take_ఫలితం(దత్తాంశం, async, pdns);
  లేకపోతే
#అయితే_ముగింపు
  ఫలితం = Curl_async_take_ఫలితం(దత్తాంశం, async, pdns);

  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    CURL_TRC_DNS(దత్తాంశం, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->పోర్టు);
    ఫలితం = CURLE_OK;
  }
  లేకపోతే ఒకవేళ(IS_RESOLV_FAIL(ఫలితం)) {
    ఫలితం = Curl_async_failed(దత్తాంశం, async, ఖాళీ);
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(దత్తాంశం, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (పూర్ణాంకం)ఫలితం, async->peer->hostname, async->peer->పోర్టు);
  }
  లేకపోతే {
    CURL_TRC_DNS(దత్తాంశం, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->పోర్టు);
    DEBUGASSERT(*pdns);
  }

  తిరిగి ఫలితం;
}

timediff_t Curl_resolv_elapsed_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  uint32_t resolv_id,
                                  స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  ఒకవేళ(!async)
    తిరిగి CURL_TIMEOUT_RESOLVE_MS;
  తిరిగి curlx_ptimediff_ms(pnow, &async->ప్రారంభం);
}

bool Curl_resolv_has_answers(నిర్మాణం Curl_easy *దత్తాంశం,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  ఒకవేళ(!async || async->పూర్తి)
    తిరిగి TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  ఒకవేళ((check_queries & async->dns_responses) != check_queries) {
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిరరాశి నిర్మాణం Curl_addrinfo *Curl_resolv_get_ai(నిర్మాణం Curl_easy *దత్తాంశం,
                                               uint32_t resolv_id,
                                               పూర్ణాంకం ai_family,
                                               గుర్తులేని పూర్ణాంకం సూచీ)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  ఒకవేళ(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    తిరిగి ఖాళీ;
  ఎంపిక(ai_family) {
  స్థితి AF_INET:
    తిరిగి Curl_addrinfo_get(async->ai_A, ai_family, సూచీ);
#నిర్వచితమైతే USE_IPV6
  స్థితి AF_INET6:
    తిరిగి Curl_addrinfo_get(async->ai_AAAA, ai_family, సూచీ);
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి ఖాళీ;
  }
}

#నిర్వచితమైతే USE_HTTPSRR

CURLcode Curl_resolv_https(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           నిర్మాణం Curl_dns_నమోదు **pdns)
{
  తిరిగి Curl_resolv(దత్తాంశం, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

స్థిరరాశి నిర్మాణం Curl_https_rrinfo *
Curl_resolv_get_https(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id)
{
  నిర్మాణం Curl_resolv_async *async;
  ఒకవేళ(!Curl_resolv_knows_https(దత్తాంశం, resolv_id))
    తిరిగి ఖాళీ;
  async = Curl_async_get(దత్తాంశం, resolv_id);
  తిరిగి async ? async->httpsrr : ఖాళీ;
}

bool Curl_resolv_knows_https(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  ఒకవేళ(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    తిరిగి ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  తిరిగి TRUE; /* we know it will never come */
}

#అయితే_ముగింపు /* USE_HTTPSRR */

#అయితే_ముగింపు /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
స్థిర CURLcode hostip_resolv_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t dns_queries,
                                    నిర్మాణం Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    నిర్మాణం Curl_dns_నమోదు **pdns,
                                    bool *pnegative)
{
#నిర్వచితమైతే USE_CURL_ASYNC
  నిర్మాణం Curl_resolv_async *async = ఖాళీ;
#అయితే_ముగింపు
  నిర్మాణం Curl_addrinfo *addr = ఖాళీ;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode ఫలితం = CURLE_OK;

  *pnegative = FALSE;

  (శూన్య)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ఖాళీ;

  /* Check for "known" things to resolve ourselves. */
  ఒకవేళ(addr_queries) {
#నిర్వచితం_కాకపోతే USE_RESOLVE_ON_IPS
    ఒకవేళ(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, ఖాళీ);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      ఫలితం = Curl_str2addr(peer->hostname, peer->పోర్టు, &addr);
      వెళ్ళు బహిర్గతం;
    }
#అయితే_ముగింపు

    hostname_len = అక్షరశ్రేణి_పొడవు(peer->hostname);
    ఒకవేళ(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, ఖాళీ);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      addr = get_localhost(peer->పోర్టు, peer->hostname);
      ఒకవేళ(!addr)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  ఒకవేళ(!Curl_is_ipaddr(peer->hostname) && allowDOH && దత్తాంశం->సమితి.doh) {
    ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, ఖాళీ);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(!async) {
      async = hostip_async_new(దత్తాంశం, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      ఒకవేళ(!async) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
    ఫలితం = Curl_doh(దత్తాంశం, async);
    వెళ్ళు బహిర్గతం;
  }
#లేకపోతే
  (శూన్య)allowDOH;
#అయితే_ముగింపు

  /* Can we provide the requested IP specifics in resolving? */
  ఒకవేళ(!can_resolve_dns_queries(దత్తాంశం, dns_queries)) {
    ఫలితం = RESOLV_FAIL(for_proxy);
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే CURLRES_ASYNCH
  (శూన్య)addr;
  ఒకవేళ(!async) {
    async = hostip_async_new(దత్తాంశం, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    ఒకవేళ(!async) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
  ఫలితం = Curl_async_getaddrinfo(దత్తాంశం, async);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_ఫలితం(దత్తాంశం, async, pdns);
    ఒకవేళ(r2)
      ఫలితం = r2;
    లేకపోతే ఒకవేళ(*pdns)
      ఫలితం = CURLE_OK;
  }
#లేకపోతే
  ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, ఖాళీ);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  addr = Curl_sync_getaddrinfo(దత్తాంశం, dns_queries, peer->hostname, peer->పోర్టు,
                               transport);
  ఒకవేళ(!addr) {
    ఫలితం = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#అయితే_ముగింపు

బహిర్గతం:
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(దత్తాంశం, dns_queries, &addr, peer);
      ఒకవేళ(!*pdns)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే ఒకవేళ(!*pdns)
      ఫలితం = CURLE_AGAIN;
  }
  లేకపోతే ఒకవేళ(*pdns)
    Curl_dns_నమోదు_unlink(దత్తాంశం, pdns);
  లేకపోతే ఒకవేళ(addr)
    Curl_freeaddrinfo(addr);

#నిర్వచితమైతే USE_CURL_ASYNC
  ఒకవేళ(async) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->తదుపరి = దత్తాంశం->state.async;
      దత్తాంశం->state.async = async;
    }
    లేకపోతే {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(దత్తాంశం, async);
    }
  }
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

స్థిర CURLcode hostip_resolv(నిర్మాణం Curl_easy *దత్తాంశం,
                              uint8_t dns_queries,
                              నిర్మాణం Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              నిర్మాణం Curl_dns_నమోదు **pdns)
{
  size_t hostname_len;
  CURLcode ఫలితం = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (శూన్య)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ఖాళీ;

#నిర్వచితమైతే CURL_DISABLE_DOH
  (శూన్య)allowDOH;
#అయితే_ముగింపు

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = అక్షరశ్రేణి_పొడవు(peer->hostname);
  DEBUGASSERT(hostname_len);
  ఒకవేళ(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(దత్తాంశం, "Not resolving .onion address (RFC 7686)");
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే DEBUGBUILD
  CURL_TRC_DNS(దత్తాంశం, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->పోర్టు);
  ఒకవేళ((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(దత్తాంశం, "DEBUG fail ipv6 resolve");
    ఫలితం = hostip_resolv_failed(దత్తాంశం, peer, for_proxy);
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు
  /* Let's check our DNS cache first */
  ఫలితం = Curl_dnscache_get(దత్తాంశం, dns_queries, peer, pdns);
  ఒకవేళ(*pdns) {
    infof(దత్తాంశం, "Hostname %s was found in DNS cache", peer->hostname);
    ఫలితం = CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    infof(దత్తాంశం, "Negative DNS entry");
    ఫలితం = hostip_resolv_failed(దత్తాంశం, peer, for_proxy);
  }
  లేకపోతే {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    ఫలితం = hostip_resolv_ప్రారంభం(దత్తాంశం, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(దత్తాంశం, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (పూర్ణాంకం)ఫలితం);
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN)) {
    Curl_dns_నమోదు_unlink(దత్తాంశం, pdns);
    ఒకవేళ(IS_RESOLV_FAIL(ఫలితం)) {
      ఒకవేళ(cache_dns && negative)
        Curl_dnscache_కూడు_negative(దత్తాంశం, dns_queries, peer);
      ఒకవేళ(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(దత్తాంశం, "Could not resolve: %s:%u", peer->hostname, peer->పోర్టు);
    }
    లేకపోతే {
      failf(దత్తాంశం, "Error %d resolving %s:%u",
            (పూర్ణాంకం)ఫలితం, peer->hostname, peer->పోర్టు);
    }
  }
  లేకపోతే ఒకవేళ(cache_dns && *pdns) {
    ఫలితం = Curl_dnscache_కూడు(దత్తాంశం, *pdns);
    ఒకవేళ(ఫలితం)
      Curl_dns_నమోదు_unlink(దత్తాంశం, pdns);
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_resolv_blocking(నిర్మాణం Curl_easy *దత్తాంశం,
                              uint8_t dns_queries,
                              స్థిరరాశి అక్షరం *hostname,
                              uint16_t పోర్టు,
                              uint8_t transport,
                              నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_peer *peer = ఖాళీ;
  CURLcode ఫలితం;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ఖాళీ;

  ఫలితం = Curl_peer_create(దత్తాంశం, దత్తాంశం->conn->scheme, hostname, పోర్టు, &peer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* We cannot do a blocking resolve using DoH currently */
  ఫలితం = hostip_resolv(దత్తాంశం, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  ఎంపిక(ఫలితం) {
  స్థితి CURLE_OK:
    DEBUGASSERT(*pdns);
    ఆపు;
#నిర్వచితమైతే USE_CURL_ASYNC
  స్థితి CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    ఫలితం = Curl_async_await(దత్తాంశం, resolv_id, pdns);
    Curl_resolv_destroy(దత్తాంశం, resolv_id);
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    ఆపు;
  }

బహిర్గతం:
  Curl_peer_unlink(&peer);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN స్థిర శూన్య alarmfunc(పూర్ణాంకం sig)
{
  (శూన్య)sig;
  siglongjmp(curl_jmpenv, 1);
}

స్థిర CURLcode resolv_alarm_timeout(నిర్మాణం Curl_easy *దత్తాంశం,
                                     uint8_t dns_queries,
                                     నిర్మాణం Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     నిర్మాణం Curl_dns_నమోదు **నమోదు_2)
{
#నిర్వచితమైతే HAVE_SIGACTION
  నిర్మాణం sigaction keep_sigact; /* store the old struct here */
  అస్థిర bool keep_copysig = FALSE; /* whether old sigact has been saved */
  నిర్మాణం sigaction sigact;
#లేకపోతే
#నిర్వచితమైతే HAVE_SIGNAL
  శూన్య (*keep_sigact)(పూర్ణాంకం);       /* store the old handler here */
#అయితే_ముగింపు /* HAVE_SIGNAL */
#అయితే_ముగింపు /* HAVE_SIGACTION */
  అస్థిర దీర్ఘ timeout;
  అస్థిర గుర్తులేని పూర్ణాంకం prev_alarm = 0;
  CURLcode ఫలితం;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!దత్తాంశం->సమితి.no_signal);
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  DEBUGASSERT(!దత్తాంశం->సమితి.doh);
#అయితే_ముగింపు

  *నమోదు_2 = ఖాళీ;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (దీర్ఘ)timeout_ms;
  ఒకవేళ(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(దత్తాంశం,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_తాళం_తాళం(&curl_jmpenv_తాళం);

  ఒకవేళ(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(దత్తాంశం, "name lookup timed out");
    ఫలితం = CURLE_OPERATION_TIMEDOUT;
    వెళ్ళు clean_పైకి;
  }
  లేకపోతే {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#నిర్వచితమైతే HAVE_SIGACTION
    sigaction(SIGALRM, ఖాళీ, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#నిర్వచితమైతే SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#అయితే_ముగింపు
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ఖాళీ);
#లేకపోతే /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#నిర్వచితమైతే HAVE_SIGNAL
    keep_sigact = సంకేతం(SIGALRM, alarmfunc);
#అయితే_ముగింపు
#అయితే_ముగింపు /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    prev_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  ఫలితం = hostip_resolv(దత్తాంశం, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, నమోదు_2);

clean_పైకి:
  ఒకవేళ(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#నిర్వచితమైతే HAVE_SIGACTION
  ఒకవేళ(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ఖాళీ); /* put it back */
  }
#లేకపోతే
#నిర్వచితమైతే HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  సంకేతం(SIGALRM, keep_sigact);
#అయితే_ముగింపు
#అయితే_ముగింపు /* HAVE_SIGACTION */

  curl_simple_తాళం_unlock(&curl_jmpenv_తాళం);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  ఒకవేళ(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(దత్తాంశం, దత్తాంశం->conn, Curl_pgrs_now(దత్తాంశం)) / 1000;

    /* the alarm period is counted in even number of seconds */
    గుర్తులేని దీర్ఘ alarm_సమితి = (గుర్తులేని దీర్ఘ)(prev_alarm - elapsed_secs);

    ఒకవేళ(!alarm_సమితి ||
       ((alarm_సమితి >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      failf(దత్తాంశం, "Previous alarm fired off");
    }
    లేకపోతే
      alarm((గుర్తులేని పూర్ణాంకం)alarm_సమితి);
  }

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_ALARM_TIMEOUT */

#నిర్వచితమైతే USE_UNIX_SOCKETS
స్థిర CURLcode resolv_unix(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_peer *peer,
                            నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_addrinfo *addr;
  CURLcode ఫలితం;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ఖాళీ;

  ఫలితం = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(దత్తాంశం, "Unix socket path too long: '%s'", peer->hostname);
      ఫలితం = CURLE_COULDNT_RESOLVE_HOST;
    }
    తిరిగి ఫలితం;
  }

  *pdns = Curl_dnsc_mk_addr(దత్తాంశం, 0, &addr, peer);
  తిరిగి *pdns ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(నిర్మాణం Curl_easy *దత్తాంశం,
                     నిర్మాణం Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     నిర్మాణం Curl_dns_నమోదు **pdns)
{
  *presolv_id = 0;
  *pdns = ఖాళీ;

  ఒకవేళ(timeout_ms < 0)
    /* got an already expired timeout */
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  లేకపోతే ఒకవేళ(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#నిర్వచితమైతే USE_UNIX_SOCKETS
  ఒకవేళ((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    తిరిగి resolv_unix(దత్తాంశం, peer, pdns);
#లేకపోతే
  ఒకవేళ(peer->unix_socket)
    తిరిగి hostip_resolv_failed(దత్తాంశం, peer, for_proxy);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ALARM_TIMEOUT
  ఒకవేళ(dns_queries & CURL_DNSQ_ADDR) {
    ఒకవేళ(timeout_ms && దత్తాంశం->సమితి.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    ఒకవేళ(timeout_ms && !Curl_doh_wanted(దత్తాంశం)) {
      తిరిగి resolv_alarm_timeout(దత్తాంశం, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#అయితే_ముగింపు /* !USE_ALARM_TIMEOUT */

#నిర్వచితం_కాకపోతే CURLRES_ASYNCH
  ఒకవేళ(timeout_ms)
    infof(దత్తాంశం, "timeout on name lookup is not supported");
#అయితే_ముగింపు

  తిరిగి hostip_resolv(దత్తాంశం, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#నిర్వచితమైతే USE_CURL_ASYNC

నిర్మాణం Curl_resolv_async *Curl_async_get(నిర్మాణం Curl_easy *దత్తాంశం,
                                         uint32_t resolv_id)
{
  నిర్మాణం Curl_resolv_async *async = దత్తాంశం->state.async;
  కొరకు(; async; async = async->తదుపరి) {
    ఒకవేళ(async->id == resolv_id)
      తిరిగి async;
  }
  తిరిగి ఖాళీ;
}

CURLcode Curl_resolv_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id,
                                 నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  CURLcode ఫలితం;

  /* If async resolving is ongoing, this must be set */
  ఒకవేళ(!async)
    తిరిగి CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  ఫలితం = Curl_dnscache_get(దత్తాంశం, async->dns_queries, async->peer, pdns);
  ఒకవేళ(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(దత్తాంశం, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(దత్తాంశం, async);
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    Curl_async_shutdown(దత్తాంశం, async);
    తిరిగి Curl_async_failed(దత్తాంశం, async, ఖాళీ);
  }

  ఫలితం = hostip_resolv_take_ఫలితం(దత్తాంశం, async, pdns);

  ఒకవేళ(*pdns) {
    /* Add to cache */
    ఫలితం = Curl_dnscache_కూడు(దత్తాంశం, *pdns);
    ఒకవేళ(ఫలితం)
      Curl_dns_నమోదు_unlink(దత్తాంశం, pdns);
  }
  లేకపోతే ఒకవేళ(IS_RESOLV_FAIL(ఫలితం)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    ఒకవేళ(async->negative_answer)
      Curl_dnscache_కూడు_negative(దత్తాంశం, async->dns_queries, async->peer);
    ఒకవేళ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(దత్తాంశం, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->పోర్టు);
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "Error %d resolving %s:%u",
          (పూర్ణాంకం)ఫలితం, async->peer->hostname, async->peer->పోర్టు);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_resolv_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం Curl_resolv_async *async = దత్తాంశం->state.async;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)ps;
  కొరకు(; async && !ఫలితం; async = async->తదుపరి) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
    ఒకవేళ(async->doh) /* DoH has nothing for the pollset */
      కొనసాగు;
#అయితే_ముగింపు
    ఫలితం = Curl_async_pollset(దత్తాంశం, async, ps);
  }
  తిరిగి ఫలితం;
}

శూన్య Curl_resolv_destroy(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id)
{
  నిర్మాణం Curl_resolv_async **panchor = &దత్తాంశం->state.async;

  కొరకు(; *panchor; panchor = &(*panchor)->తదుపరి) {
    నిర్మాణం Curl_resolv_async *async = *panchor;
    ఒకవేళ(async->id == resolv_id) {
      *panchor = async->తదుపరి;
      Curl_async_destroy(దత్తాంశం, async);
      ఆపు;
    }
  }
}

శూన్య Curl_resolv_shutdown_all(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_resolv_async *async = దత్తాంశం->state.async;
  కొరకు(; async; async = async->తదుపరి) {
    Curl_async_shutdown(దత్తాంశం, async);
  }
}

శూన్య Curl_resolv_destroy_all(నిర్మాణం Curl_easy *దత్తాంశం)
{
  అయితే(దత్తాంశం->state.async) {
    నిర్మాణం Curl_resolv_async *async = దత్తాంశం->state.async;
    దత్తాంశం->state.async = async->తదుపరి;
    Curl_async_destroy(దత్తాంశం, async);
  }
}

#అయితే_ముగింపు /* USE_CURL_ASYNC */
