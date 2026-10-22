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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN6_H
#समावेश <netinet/in6.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#समावेश <signal.h>

#समावेश "urldata.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "url.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "strcase.h"
#समावेश "easy_lock.h"
#समावेश "vdns/dnscache.h"
#समावेश "vdns/doh.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/strcopy.h"
#समावेश "curlx/strparse.h"

#अगर defined(CURLRES_SYNCH) &&                   \
  defined(HAVE_ALARM) &&                        \
  defined(SIGALRM) &&                           \
  defined(HAVE_SIGSETJMP) &&                    \
  defined(GLOBAL_INIT_IS_THREADSAFE)
/* alarm-based timeouts can only be used with all the dependencies satisfied */
#घोषणा USE_ALARM_TIMEOUT
#पूर्ण_अगर

#घोषणा RESOLV_FAIL(for_proxy) \
  ((for_proxy) ? CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST)

#घोषणा IS_RESOLV_FAIL(परिणाम) \
  (((परिणाम) == CURLE_COULDNT_RESOLVE_HOST) || \
   ((परिणाम) == CURLE_COULDNT_RESOLVE_PROXY))
/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
#अगर_घोषित USE_IPV6
अटल bool ipv6works(काष्ठा Curl_easy *तथ्य);
#अन्यथा
#घोषणा ipv6works(x) FALSE
#पूर्ण_अगर

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

uint8_t Curl_resolv_dns_queries(काष्ठा Curl_easy *तथ्य, uint8_t ip_version)
{
  (व्योम)तथ्य;
  चयन(ip_version) {
  हाल CURL_IPRESOLVE_V6:
    वापस CURL_DNSQ_AAAA;
  हाल CURL_IPRESOLVE_V4:
    वापस CURL_DNSQ_A;
  शेष:
    अगर(ipv6works(तथ्य))
      वापस (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    अन्यथा
      वापस CURL_DNSQ_A;
  }
}

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_resolv_query_str(uint8_t dns_queries)
{
  चयन(dns_queries) {
  हाल (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    वापस "A+AAAA+HTTPS";
  हाल (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    वापस "A+AAAA";
  हाल (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    वापस "AAAA+HTTPS";
  हाल (CURL_DNSQ_AAAA):
    वापस "AAAA";
  हाल (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    वापस "A+HTTPS";
  हाल (CURL_DNSQ_A):
    वापस "A";
  हाल (CURL_DNSQ_HTTPS):
    वापस "HTTPS";
  हाल 0:
    वापस "-";
  शेष:
    DEBUGASSERT(0);
    वापस "???";
  }
}
#पूर्ण_अगर

/*
 * Curl_printable_address() stores a printable version of the 1st address
 * given in the 'ai' argument. The result will be stored in the buf that is
 * bufsize bytes big.
 *
 * If the conversion fails, the target buffer is empty.
 */
व्योम Curl_printable_address(स्थिर काष्ठा Curl_addrinfo *ai, अक्षर *buf,
                            माप_प्रकार bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  चयन(ai->ai_family) {
  हाल AF_INET: {
    स्थिर काष्ठा sockaddr_in *sa4 = (स्थिर व्योम *)ai->ai_addr;
    स्थिर काष्ठा in_addr *ipaddr4 = &sa4->sin_addr;
    (व्योम)curlx_inet_ntop(ai->ai_family, (स्थिर व्योम *)ipaddr4, buf, bufsize);
    अवरोध;
  }
#अगर_घोषित USE_IPV6
  हाल AF_INET6: {
    स्थिर काष्ठा sockaddr_in6 *sa6 = (स्थिर व्योम *)ai->ai_addr;
    स्थिर काष्ठा in6_addr *ipaddr6 = &sa6->sin6_addr;
    (व्योम)curlx_inet_ntop(ai->ai_family, (स्थिर व्योम *)ipaddr6, buf, bufsize);
    अवरोध;
  }
#पूर्ण_अगर
  शेष:
    अवरोध;
  }
}

#अगर_घोषित USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
अटल sigjmp_buf curl_jmpenv;
अटल curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#पूर्ण_अगर

#अगर_घोषित USE_IPV6
/* return a static IPv6 ::1 for the name */
अटल काष्ठा Curl_addrinfo *get_localhost6(uint16_t port, स्थिर अक्षर *name)
{
  काष्ठा Curl_addrinfo *ca;
  स्थिर माप_प्रकार ss_आकार = माप(काष्ठा sockaddr_in6);
  स्थिर माप_प्रकार hostlen = म_माप(name);
  काष्ठा sockaddr_in6 sa6;
  अचिन्हित अक्षर ipv6[16];
  अचिन्हित लघु port16 = (अचिन्हित लघु)(port & 0xffff);
  ca = curlx_calloc(1, माप(काष्ठा Curl_addrinfo) + ss_आकार + hostlen + 1);
  अगर(!ca)
    वापस शून्य;

  स_रखो(&sa6, 0, माप(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (व्योम)curlx_inet_pton(AF_INET6, "::1", ipv6);
  स_नकल(&sa6.sin6_addr, ipv6, माप(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
  ca->ai_अग्र      = शून्य;
  ca->ai_addr = (व्योम *)((अक्षर *)ca + माप(काष्ठा Curl_addrinfo));
  स_नकल(ca->ai_addr, &sa6, ss_आकार);
  ca->ai_canonname = (अक्षर *)ca->ai_addr + ss_आकार;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  वापस ca;
}
#अन्यथा
#घोषणा get_localhost6(x, y) शून्य
#पूर्ण_अगर

/* return a static IPv4 127.0.0.1 for the given name */
अटल काष्ठा Curl_addrinfo *get_localhost(uint16_t port, स्थिर अक्षर *name)
{
  काष्ठा Curl_addrinfo *ca;
  काष्ठा Curl_addrinfo *ca6;
  स्थिर माप_प्रकार ss_आकार = माप(काष्ठा sockaddr_in);
  स्थिर माप_प्रकार hostlen = म_माप(name);
  काष्ठा sockaddr_in sa;
  अचिन्हित पूर्णांक ipv4;
  अचिन्हित लघु port16 = (अचिन्हित लघु)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  स_रखो(&sa, 0, माप(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  अगर(curlx_inet_pton(AF_INET, "127.0.0.1", (अक्षर *)&ipv4) < 1)
    वापस शून्य;
  स_नकल(&sa.sin_addr, &ipv4, माप(ipv4));

  ca = curlx_calloc(1, माप(काष्ठा Curl_addrinfo) + ss_आकार + hostlen + 1);
  अगर(!ca)
    वापस शून्य;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
  ca->ai_addr = (व्योम *)((अक्षर *)ca + माप(काष्ठा Curl_addrinfo));
  स_नकल(ca->ai_addr, &sa, ss_आकार);
  ca->ai_canonname = (अक्षर *)ca->ai_addr + ss_आकार;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  अगर(!ca6)
    वापस ca;
  ca6->ai_अग्र = ca;
  वापस ca6;
}

#अगर_घोषित USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(काष्ठा Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  अगर(s == CURL_SOCKET_BAD) {
    अगर(SOCKERRNO == SOCKENOMEM)
      वापस CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  वापस CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
अटल bool ipv6works(काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->multi);
  वापस तथ्य ? तथ्य->multi->ipv6_works : FALSE;
}
#पूर्ण_अगर /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(स्थिर अक्षर *hostname)
{
  काष्ठा in_addr in;
#अगर_घोषित USE_IPV6
  काष्ठा in6_addr in6;
#पूर्ण_अगर
  अगर(curlx_inet_pton(AF_INET, hostname, &in) > 0
#अगर_घोषित USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#पूर्ण_अगर
    )
    वापस TRUE;
  वापस FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
अटल bool tailmatch(स्थिर अक्षर *full, माप_प्रकार flen,
                      स्थिर अक्षर *part, माप_प्रकार plen)
{
  अगर(plen > flen)
    वापस FALSE;
  वापस curl_strnequal(part, &full[flen - plen], plen);
}

अटल CURLcode hostip_resolv_failed(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_peer *peer,
                                     bool for_proxy)
{
  failf(तथ्य, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  वापस RESOLV_FAIL(for_proxy);
}

अटल bool can_resolve_dns_queries(काष्ठा Curl_easy *तथ्य,
                                    uint8_t dns_queries)
{
  (व्योम)तथ्य;
  अगर((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(तथ्य))
    वापस FALSE;
  वापस TRUE;
}

CURLcode Curl_resolv_announce_start(काष्ठा Curl_easy *तथ्य,
                                    व्योम *resolver)
{
  अगर(तथ्य->set.resolver_start) {
    काष्ठा Curl_mapi_guard guard;
    पूर्णांक rc;

    CURL_TRC_DNS(तथ्य, "announcing resolve to application");
    CURL_CBAPI_START(&guard, तथ्य, easy_resolver_start);
    rc = तथ्य->set.resolver_start(resolver, शून्य,
                                  तथ्य->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    अगर(rc) {
      CURL_TRC_DNS(तथ्य, "application aborted resolve");
      वापस CURLE_ABORTED_BY_CALLBACK;
    }
  }
  वापस CURLE_OK;
}

#अगर_घोषित USE_CURL_ASYNC

अटल काष्ठा Curl_resolv_async *hostip_async_new(काष्ठा Curl_easy *तथ्य,
                                                  uint8_t dns_queries,
                                                  काष्ठा Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  काष्ठा Curl_resolv_async *async;

  अगर(!तथ्य->multi) {
    DEBUGASSERT(0);
    वापस शून्य;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, माप(*async));
  अगर(!async)
    वापस शून्य;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  अगर(तथ्य->multi->last_resolv_id == UINT32_MAX)
    तथ्य->multi->last_resolv_id = 1; /* wrap around */
  अन्यथा
    तथ्य->multi->last_resolv_id++;
  async->id = तथ्य->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(तथ्य);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  अगर(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  वापस async;
}

अटल CURLcode hostip_resolv_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_resolv_async *async,
                                          काष्ठा Curl_dns_entry **pdns)
{
  CURLcode परिणाम;

  /* If async resolving is ongoing, this must be set */
  अगर(!async)
    वापस CURLE_FAILED_INIT;

#अगर_अघोषित CURL_DISABLE_DOH
  अगर(async->doh)
    परिणाम = Curl_doh_take_परिणाम(तथ्य, async, pdns);
  अन्यथा
#पूर्ण_अगर
  परिणाम = Curl_async_take_परिणाम(तथ्य, async, pdns);

  अगर(परिणाम == CURLE_AGAIN) {
    CURL_TRC_DNS(तथ्य, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    परिणाम = CURLE_OK;
  }
  अन्यथा अगर(IS_RESOLV_FAIL(परिणाम)) {
    परिणाम = Curl_async_failed(तथ्य, async, शून्य);
  }
  अन्यथा अगर(परिणाम) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(तथ्य, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (पूर्णांक)परिणाम, async->peer->hostname, async->peer->port);
  }
  अन्यथा {
    CURL_TRC_DNS(तथ्य, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  वापस परिणाम;
}

timediff_t Curl_resolv_elapsed_ms(काष्ठा Curl_easy *तथ्य,
                                  uint32_t resolv_id,
                                  स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  अगर(!async)
    वापस CURL_TIMEOUT_RESOLVE_MS;
  वापस curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(काष्ठा Curl_easy *तथ्य,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  अगर(!async || async->done)
    वापस TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  अगर((check_queries & async->dns_responses) != check_queries) {
    वापस FALSE;
  }
  वापस TRUE;
}

स्थिर काष्ठा Curl_addrinfo *Curl_resolv_get_ai(काष्ठा Curl_easy *तथ्य,
                                               uint32_t resolv_id,
                                               पूर्णांक ai_family,
                                               अचिन्हित पूर्णांक सूचकांक)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  अगर(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    वापस शून्य;
  चयन(ai_family) {
  हाल AF_INET:
    वापस Curl_addrinfo_get(async->ai_A, ai_family, सूचकांक);
#अगर_घोषित USE_IPV6
  हाल AF_INET6:
    वापस Curl_addrinfo_get(async->ai_AAAA, ai_family, सूचकांक);
#पूर्ण_अगर
  शेष:
    वापस शून्य;
  }
}

#अगर_घोषित USE_HTTPSRR

CURLcode Curl_resolv_https(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           काष्ठा Curl_dns_entry **pdns)
{
  वापस Curl_resolv(तथ्य, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

स्थिर काष्ठा Curl_https_rrinfo *
Curl_resolv_get_https(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id)
{
  काष्ठा Curl_resolv_async *async;
  अगर(!Curl_resolv_knows_https(तथ्य, resolv_id))
    वापस शून्य;
  async = Curl_async_get(तथ्य, resolv_id);
  वापस async ? async->httpsrr : शून्य;
}

bool Curl_resolv_knows_https(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  अगर(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    वापस ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  वापस TRUE; /* we know it will never come */
}

#पूर्ण_अगर /* USE_HTTPSRR */

#पूर्ण_अगर /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
अटल CURLcode hostip_resolv_start(काष्ठा Curl_easy *तथ्य,
                                    uint8_t dns_queries,
                                    काष्ठा Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    काष्ठा Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#अगर_घोषित USE_CURL_ASYNC
  काष्ठा Curl_resolv_async *async = शून्य;
#पूर्ण_अगर
  काष्ठा Curl_addrinfo *addr = शून्य;
  माप_प्रकार hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode परिणाम = CURLE_OK;

  *pnegative = FALSE;

  (व्योम)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = शून्य;

  /* Check for "known" things to resolve ourselves. */
  अगर(addr_queries) {
#अगर_अघोषित USE_RESOLVE_ON_IPS
    अगर(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      परिणाम = Curl_resolv_announce_start(तथ्य, शून्य);
      अगर(परिणाम)
        जाओ out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      परिणाम = Curl_str2addr(peer->hostname, peer->port, &addr);
      जाओ out;
    }
#पूर्ण_अगर

    hostname_len = म_माप(peer->hostname);
    अगर(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      परिणाम = Curl_resolv_announce_start(तथ्य, शून्य);
      अगर(परिणाम)
        जाओ out;
      addr = get_localhost(peer->port, peer->hostname);
      अगर(!addr)
        परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
#अगर_अघोषित CURL_DISABLE_DOH
  अगर(!Curl_is_ipaddr(peer->hostname) && allowDOH && तथ्य->set.doh) {
    परिणाम = Curl_resolv_announce_start(तथ्य, शून्य);
    अगर(परिणाम)
      जाओ out;
    अगर(!async) {
      async = hostip_async_new(तथ्य, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      अगर(!async) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
    परिणाम = Curl_doh(तथ्य, async);
    जाओ out;
  }
#अन्यथा
  (व्योम)allowDOH;
#पूर्ण_अगर

  /* Can we provide the requested IP specifics in resolving? */
  अगर(!can_resolve_dns_queries(तथ्य, dns_queries)) {
    परिणाम = RESOLV_FAIL(for_proxy);
    जाओ out;
  }

#अगर_घोषित CURLRES_ASYNCH
  (व्योम)addr;
  अगर(!async) {
    async = hostip_async_new(तथ्य, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    अगर(!async) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
  परिणाम = Curl_async_getaddrinfo(तथ्य, async);
  अगर(परिणाम == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_परिणाम(तथ्य, async, pdns);
    अगर(r2)
      परिणाम = r2;
    अन्यथा अगर(*pdns)
      परिणाम = CURLE_OK;
  }
#अन्यथा
  परिणाम = Curl_resolv_announce_start(तथ्य, शून्य);
  अगर(परिणाम)
    जाओ out;
  addr = Curl_sync_getaddrinfo(तथ्य, dns_queries, peer->hostname, peer->port,
                               transport);
  अगर(!addr) {
    परिणाम = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#पूर्ण_अगर

out:
  अगर(!परिणाम) {
    अगर(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(तथ्य, dns_queries, &addr, peer);
      अगर(!*pdns)
        परिणाम = CURLE_OUT_OF_MEMORY;
    }
    अन्यथा अगर(!*pdns)
      परिणाम = CURLE_AGAIN;
  }
  अन्यथा अगर(*pdns)
    Curl_dns_entry_unlink(तथ्य, pdns);
  अन्यथा अगर(addr)
    Curl_freeaddrinfo(addr);

#अगर_घोषित USE_CURL_ASYNC
  अगर(async) {
    अगर(परिणाम == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->अग्र = तथ्य->state.async;
      तथ्य->state.async = async;
    }
    अन्यथा {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(तथ्य, async);
    }
  }
#पूर्ण_अगर
  वापस परिणाम;
}

अटल CURLcode hostip_resolv(काष्ठा Curl_easy *तथ्य,
                              uint8_t dns_queries,
                              काष्ठा Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              काष्ठा Curl_dns_entry **pdns)
{
  माप_प्रकार hostname_len;
  CURLcode परिणाम = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (व्योम)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = शून्य;

#अगर_घोषित CURL_DISABLE_DOH
  (व्योम)allowDOH;
#पूर्ण_अगर

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = म_माप(peer->hostname);
  DEBUGASSERT(hostname_len);
  अगर(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(तथ्य, "Not resolving .onion address (RFC 7686)");
    जाओ out;
  }

#अगर_घोषित DEBUGBUILD
  CURL_TRC_DNS(तथ्य, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  अगर((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     दो_पर्या("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(तथ्य, "DEBUG fail ipv6 resolve");
    परिणाम = hostip_resolv_failed(तथ्य, peer, for_proxy);
    जाओ out;
  }
#पूर्ण_अगर
  /* Let's check our DNS cache first */
  परिणाम = Curl_dnscache_get(तथ्य, dns_queries, peer, pdns);
  अगर(*pdns) {
    infof(तथ्य, "Hostname %s was found in DNS cache", peer->hostname);
    परिणाम = CURLE_OK;
  }
  अन्यथा अगर(परिणाम) {
    infof(तथ्य, "Negative DNS entry");
    परिणाम = hostip_resolv_failed(तथ्य, peer, for_proxy);
  }
  अन्यथा {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    परिणाम = hostip_resolv_start(तथ्य, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(तथ्य, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (पूर्णांक)परिणाम);
  }

out:
  अगर(परिणाम && (परिणाम != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(तथ्य, pdns);
    अगर(IS_RESOLV_FAIL(परिणाम)) {
      अगर(cache_dns && negative)
        Curl_dnscache_add_negative(तथ्य, dns_queries, peer);
      अगर(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(तथ्य, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    अन्यथा {
      failf(तथ्य, "Error %d resolving %s:%u",
            (पूर्णांक)परिणाम, peer->hostname, peer->port);
    }
  }
  अन्यथा अगर(cache_dns && *pdns) {
    परिणाम = Curl_dnscache_add(तथ्य, *pdns);
    अगर(परिणाम)
      Curl_dns_entry_unlink(तथ्य, pdns);
  }

  वापस परिणाम;
}

CURLcode Curl_resolv_blocking(काष्ठा Curl_easy *तथ्य,
                              uint8_t dns_queries,
                              स्थिर अक्षर *hostname,
                              uint16_t port,
                              uint8_t transport,
                              काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_peer *peer = शून्य;
  CURLcode परिणाम;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = शून्य;

  परिणाम = Curl_peer_create(तथ्य, तथ्य->conn->scheme, hostname, port, &peer);
  अगर(परिणाम)
    जाओ out;

  /* We cannot do a blocking resolve using DoH currently */
  परिणाम = hostip_resolv(तथ्य, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  चयन(परिणाम) {
  हाल CURLE_OK:
    DEBUGASSERT(*pdns);
    अवरोध;
#अगर_घोषित USE_CURL_ASYNC
  हाल CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    परिणाम = Curl_async_await(तथ्य, resolv_id, pdns);
    Curl_resolv_destroy(तथ्य, resolv_id);
    अवरोध;
#पूर्ण_अगर
  शेष:
    अवरोध;
  }

out:
  Curl_peer_unlink(&peer);
  वापस परिणाम;
}

#अगर_घोषित USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN अटल व्योम alarmfunc(पूर्णांक sig)
{
  (व्योम)sig;
  siglongjmp(curl_jmpenv, 1);
}

अटल CURLcode resolv_alarm_timeout(काष्ठा Curl_easy *तथ्य,
                                     uint8_t dns_queries,
                                     काष्ठा Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     काष्ठा Curl_dns_entry **entry)
{
#अगर_घोषित HAVE_SIGACTION
  काष्ठा sigaction keep_sigact; /* store the old struct here */
  अस्थिर bool keep_copysig = FALSE; /* whether old sigact has been saved */
  काष्ठा sigaction sigact;
#अन्यथा
#अगर_घोषित HAVE_SIGNAL
  व्योम (*keep_sigact)(पूर्णांक);       /* store the old handler here */
#पूर्ण_अगर /* HAVE_SIGNAL */
#पूर्ण_अगर /* HAVE_SIGACTION */
  अस्थिर दीर्घ timeout;
  अस्थिर अचिन्हित पूर्णांक पूर्व_alarm = 0;
  CURLcode परिणाम;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!तथ्य->set.no_signal);
#अगर_अघोषित CURL_DISABLE_DOH
  DEBUGASSERT(!तथ्य->set.doh);
#पूर्ण_अगर

  *entry = शून्य;
  timeout = (timeout_ms > दीर्घ_उच्च) ? दीर्घ_उच्च : (दीर्घ)timeout_ms;
  अगर(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(तथ्य,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    वापस CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  अगर(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(तथ्य, "name lookup timed out");
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    जाओ clean_up;
  }
  अन्यथा {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#अगर_घोषित HAVE_SIGACTION
    sigaction(SIGALRM, शून्य, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#अगर_घोषित SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#पूर्ण_अगर
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, शून्य);
#अन्यथा /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#अगर_घोषित HAVE_SIGNAL
    keep_sigact = संकेत(SIGALRM, alarmfunc);
#पूर्ण_अगर
#पूर्ण_अगर /* HAVE_SIGACTION */

    /* alarm() makes a signal get sent when the timeout fires off, and that
       will abort system calls */
    पूर्व_alarm = alarm(curlx_sltoui(timeout / 1000L));
  }

  /* Perform the actual name resolution. This might be interrupted by an
   * alarm if it takes too long. */
  परिणाम = hostip_resolv(तथ्य, dns_queries, peer, transport,
                         for_proxy, timeout_ms, FALSE, presolv_id, entry);

clean_up:
  अगर(!पूर्व_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#अगर_घोषित HAVE_SIGACTION
  अगर(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, शून्य); /* put it back */
  }
#अन्यथा
#अगर_घोषित HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  संकेत(SIGALRM, keep_sigact);
#पूर्ण_अगर
#पूर्ण_अगर /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  अगर(पूर्व_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(तथ्य, तथ्य->conn, Curl_pgrs_now(तथ्य)) / 1000;

    /* the alarm period is counted in even number of seconds */
    अचिन्हित दीर्घ alarm_set = (अचिन्हित दीर्घ)(पूर्व_alarm - elapsed_secs);

    अगर(!alarm_set ||
       ((alarm_set >= 0x80000000) && (पूर्व_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      failf(तथ्य, "Previous alarm fired off");
    }
    अन्यथा
      alarm((अचिन्हित पूर्णांक)alarm_set);
  }

  वापस परिणाम;
}

#पूर्ण_अगर /* USE_ALARM_TIMEOUT */

#अगर_घोषित USE_UNIX_SOCKETS
अटल CURLcode resolv_unix(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *peer,
                            काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_addrinfo *addr;
  CURLcode परिणाम;

  DEBUGASSERT(peer->unix_socket);
  *pdns = शून्य;

  परिणाम = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(तथ्य, "Unix socket path too long: '%s'", peer->hostname);
      परिणाम = CURLE_COULDNT_RESOLVE_HOST;
    }
    वापस परिणाम;
  }

  *pdns = Curl_dnsc_mk_addr(तथ्य, 0, &addr, peer);
  वापस *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}
#पूर्ण_अगर /* USE_UNIX_SOCKETS */

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
CURLcode Curl_resolv(काष्ठा Curl_easy *तथ्य,
                     काष्ठा Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     काष्ठा Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = शून्य;

  अगर(timeout_ms < 0)
    /* got an already expired timeout */
    वापस CURLE_OPERATION_TIMEDOUT;
  अन्यथा अगर(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#अगर_घोषित USE_UNIX_SOCKETS
  अगर((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    वापस resolv_unix(तथ्य, peer, pdns);
#अन्यथा
  अगर(peer->unix_socket)
    वापस hostip_resolv_failed(तथ्य, peer, for_proxy);
#पूर्ण_अगर

#अगर_घोषित USE_ALARM_TIMEOUT
  अगर(dns_queries & CURL_DNSQ_ADDR) {
    अगर(timeout_ms && तथ्य->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    अगर(timeout_ms && !Curl_doh_wanted(तथ्य)) {
      वापस resolv_alarm_timeout(तथ्य, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#पूर्ण_अगर /* !USE_ALARM_TIMEOUT */

#अगर_अघोषित CURLRES_ASYNCH
  अगर(timeout_ms)
    infof(तथ्य, "timeout on name lookup is not supported");
#पूर्ण_अगर

  वापस hostip_resolv(तथ्य, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#अगर_घोषित USE_CURL_ASYNC

काष्ठा Curl_resolv_async *Curl_async_get(काष्ठा Curl_easy *तथ्य,
                                         uint32_t resolv_id)
{
  काष्ठा Curl_resolv_async *async = तथ्य->state.async;
  क्रम(; async; async = async->अग्र) {
    अगर(async->id == resolv_id)
      वापस async;
  }
  वापस शून्य;
}

CURLcode Curl_resolv_take_परिणाम(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id,
                                 काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  CURLcode परिणाम;

  /* If async resolving is ongoing, this must be set */
  अगर(!async)
    वापस CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  परिणाम = Curl_dnscache_get(तथ्य, async->dns_queries, async->peer, pdns);
  अगर(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(तथ्य, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(तथ्य, async);
    वापस CURLE_OK;
  }
  अन्यथा अगर(परिणाम) {
    Curl_async_shutdown(तथ्य, async);
    वापस Curl_async_failed(तथ्य, async, शून्य);
  }

  परिणाम = hostip_resolv_take_परिणाम(तथ्य, async, pdns);

  अगर(*pdns) {
    /* Add to cache */
    परिणाम = Curl_dnscache_add(तथ्य, *pdns);
    अगर(परिणाम)
      Curl_dns_entry_unlink(तथ्य, pdns);
  }
  अन्यथा अगर(IS_RESOLV_FAIL(परिणाम)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    अगर(async->negative_answer)
      Curl_dnscache_add_negative(तथ्य, async->dns_queries, async->peer);
    अगर(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(तथ्य, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  अन्यथा अगर(परिणाम) {
    failf(तथ्य, "Error %d resolving %s:%u",
          (पूर्णांक)परिणाम, async->peer->hostname, async->peer->port);
  }
  वापस परिणाम;
}

CURLcode Curl_resolv_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  काष्ठा Curl_resolv_async *async = तथ्य->state.async;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)ps;
  क्रम(; async && !परिणाम; async = async->अग्र) {
#अगर_अघोषित CURL_DISABLE_DOH
    अगर(async->doh) /* DoH has nothing for the pollset */
      जारी;
#पूर्ण_अगर
    परिणाम = Curl_async_pollset(तथ्य, async, ps);
  }
  वापस परिणाम;
}

व्योम Curl_resolv_destroy(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id)
{
  काष्ठा Curl_resolv_async **panchor = &तथ्य->state.async;

  क्रम(; *panchor; panchor = &(*panchor)->अग्र) {
    काष्ठा Curl_resolv_async *async = *panchor;
    अगर(async->id == resolv_id) {
      *panchor = async->अग्र;
      Curl_async_destroy(तथ्य, async);
      अवरोध;
    }
  }
}

व्योम Curl_resolv_shutdown_all(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_resolv_async *async = तथ्य->state.async;
  क्रम(; async; async = async->अग्र) {
    Curl_async_shutdown(तथ्य, async);
  }
}

व्योम Curl_resolv_destroy_all(काष्ठा Curl_easy *तथ्य)
{
  जबतक(तथ्य->state.async) {
    काष्ठा Curl_resolv_async *async = तथ्य->state.async;
    तथ्य->state.async = async->अग्र;
    Curl_async_destroy(तथ्य, async);
  }
}

#पूर्ण_अगर /* USE_CURL_ASYNC */
