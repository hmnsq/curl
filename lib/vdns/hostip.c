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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#અન્તર્ભાવ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#અન્તર્ભાવ <signal.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "easy_lock.h"
#અન્તર્ભાવ "vdns/dnscache.h"
#અન્તર્ભાવ "vdns/doh.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strcopy.h"
#અન્તર્ભાવ "curlx/strparse.h"

#જો defined(CURLRES_SYNCH) &&                   \
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
સ્થિર bool ipv6works(સંરચના Curl_easy *data);
#નહીંતર
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

uint8_t Curl_resolv_dns_queries(સંરચના Curl_easy *data, uint8_t ip_version)
{
  (શૂન્ય)data;
  પસંદગી(ip_version) {
  સ્થિતિ CURL_IPRESOLVE_V6:
    પાછા CURL_DNSQ_AAAA;
  સ્થિતિ CURL_IPRESOLVE_V4:
    પાછા CURL_DNSQ_A;
  મૂળભૂત:
    જો(ipv6works(data))
      પાછા (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    નહીંતર
      પાછા CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
અચલ અક્ષર *Curl_resolv_query_str(uint8_t dns_queries)
{
  પસંદગી(dns_queries) {
  સ્થિતિ (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    પાછા "A+AAAA+HTTPS";
  સ્થિતિ (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    પાછા "A+AAAA";
  સ્થિતિ (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    પાછા "AAAA+HTTPS";
  સ્થિતિ (CURL_DNSQ_AAAA):
    પાછા "AAAA";
  સ્થિતિ (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    પાછા "A+HTTPS";
  સ્થિતિ (CURL_DNSQ_A):
    પાછા "A";
  સ્થિતિ (CURL_DNSQ_HTTPS):
    પાછા "HTTPS";
  સ્થિતિ 0:
    પાછા "-";
  મૂળભૂત:
    DEBUGASSERT(0);
    પાછા "???";
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
શૂન્ય Curl_printable_address(અચલ સંરચના Curl_addrinfo *ai, અક્ષર *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  પસંદગી(ai->ai_family) {
  સ્થિતિ AF_INET: {
    અચલ સંરચના sockaddr_in *sa4 = (અચલ શૂન્ય *)ai->ai_addr;
    અચલ સંરચના in_addr *ipaddr4 = &sa4->sin_addr;
    (શૂન્ય)curlx_inet_ntop(ai->ai_family, (અચલ શૂન્ય *)ipaddr4, buf, bufsize);
    તોડો;
  }
#ifdef USE_IPV6
  સ્થિતિ AF_INET6: {
    અચલ સંરચના sockaddr_in6 *sa6 = (અચલ શૂન્ય *)ai->ai_addr;
    અચલ સંરચના in6_addr *ipaddr6 = &sa6->sin6_addr;
    (શૂન્ય)curlx_inet_ntop(ai->ai_family, (અચલ શૂન્ય *)ipaddr6, buf, bufsize);
    તોડો;
  }
#endif
  મૂળભૂત:
    તોડો;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
સ્થિર sigjmp_buf curl_jmpenv;
સ્થિર curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
સ્થિર સંરચના Curl_addrinfo *get_localhost6(uint16_t port, અચલ અક્ષર *name)
{
  સંરચના Curl_addrinfo *ca;
  અચલ size_t ss_size = કદ(સંરચના sockaddr_in6);
  અચલ size_t hostlen = strlen(name);
  સંરચના sockaddr_in6 sa6;
  unsigned અક્ષર ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, કદ(સંરચના Curl_addrinfo) + ss_size + hostlen + 1);
  જો(!ca)
    પાછા ખાલી;

  memset(&sa6, 0, કદ(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (શૂન્ય)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, કદ(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = ખાલી;
  ca->ai_addr = (શૂન્ય *)((અક્ષર *)ca + કદ(સંરચના Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (અક્ષર *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  પાછા ca;
}
#નહીંતર
#define get_localhost6(x, y) ખાલી
#endif

/* return a static IPv4 127.0.0.1 for the given name */
સ્થિર સંરચના Curl_addrinfo *get_localhost(uint16_t port, અચલ અક્ષર *name)
{
  સંરચના Curl_addrinfo *ca;
  સંરચના Curl_addrinfo *ca6;
  અચલ size_t ss_size = કદ(સંરચના sockaddr_in);
  અચલ size_t hostlen = strlen(name);
  સંરચના sockaddr_in sa;
  unsigned પૂર્ણાંક ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, કદ(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  જો(curlx_inet_pton(AF_INET, "127.0.0.1", (અક્ષર *)&ipv4) < 1)
    પાછા ખાલી;
  memcpy(&sa.sin_addr, &ipv4, કદ(ipv4));

  ca = curlx_calloc(1, કદ(સંરચના Curl_addrinfo) + ss_size + hostlen + 1);
  જો(!ca)
    પાછા ખાલી;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (શૂન્ય *)((અક્ષર *)ca + કદ(સંરચના Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (અક્ષર *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  જો(!ca6)
    પાછા ca;
  ca6->ai_next = ca;
  પાછા ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(સંરચના Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  જો(s == CURL_SOCKET_BAD) {
    જો(SOCKERRNO == SOCKENOMEM)
      પાછા CURLE_OUT_OF_MEMORY;
  }
  નહીંતર {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  પાછા CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
સ્થિર bool ipv6works(સંરચના Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  પાછા data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(અચલ અક્ષર *hostname)
{
  સંરચના in_addr in;
#ifdef USE_IPV6
  સંરચના in6_addr in6;
#endif
  જો(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    પાછા TRUE;
  પાછા FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
સ્થિર bool tailmatch(અચલ અક્ષર *full, size_t flen,
                      અચલ અક્ષર *part, size_t plen)
{
  જો(plen > flen)
    પાછા FALSE;
  પાછા curl_strnequal(part, &full[flen - plen], plen);
}

સ્થિર CURLcode hostip_resolv_failed(સંરચના Curl_easy *data,
                                     સંરચના Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  પાછા RESOLV_FAIL(for_proxy);
}

સ્થિર bool can_resolve_dns_queries(સંરચના Curl_easy *data,
                                    uint8_t dns_queries)
{
  (શૂન્ય)data;
  જો((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    પાછા FALSE;
  પાછા TRUE;
}

CURLcode Curl_resolv_announce_start(સંરચના Curl_easy *data,
                                    શૂન્ય *resolver)
{
  જો(data->set.resolver_start) {
    સંરચના Curl_mapi_guard guard;
    પૂર્ણાંક rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, ખાલી,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    જો(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      પાછા CURLE_ABORTED_BY_CALLBACK;
    }
  }
  પાછા CURLE_OK;
}

#ifdef USE_CURL_ASYNC

સ્થિર સંરચના Curl_resolv_async *hostip_async_new(સંરચના Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  સંરચના Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  સંરચના Curl_resolv_async *async;

  જો(!data->multi) {
    DEBUGASSERT(0);
    પાછા ખાલી;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, કદ(*async));
  જો(!async)
    પાછા ખાલી;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  જો(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  નહીંતર
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  જો(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  પાછા async;
}

સ્થિર CURLcode hostip_resolv_take_result(સંરચના Curl_easy *data,
                                          સંરચના Curl_resolv_async *async,
                                          સંરચના Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  જો(!async)
    પાછા CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  જો(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  નહીંતર
#endif
  result = Curl_async_take_result(data, async, pdns);

  જો(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  નહીંતર જો(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, ખાલી);
  }
  નહીંતર જો(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (પૂર્ણાંક)result, async->peer->hostname, async->peer->port);
  }
  નહીંતર {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  પાછા result;
}

timediff_t Curl_resolv_elapsed_ms(સંરચના Curl_easy *data,
                                  uint32_t resolv_id,
                                  અચલ સંરચના curltime *pnow)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  જો(!async)
    પાછા CURL_TIMEOUT_RESOLVE_MS;
  પાછા curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(સંરચના Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  જો(!async || async->done)
    પાછા TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  જો((check_queries & async->dns_responses) != check_queries) {
    પાછા FALSE;
  }
  પાછા TRUE;
}

અચલ સંરચના Curl_addrinfo *Curl_resolv_get_ai(સંરચના Curl_easy *data,
                                               uint32_t resolv_id,
                                               પૂર્ણાંક ai_family,
                                               unsigned પૂર્ણાંક index)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  જો(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    પાછા ખાલી;
  પસંદગી(ai_family) {
  સ્થિતિ AF_INET:
    પાછા Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  સ્થિતિ AF_INET6:
    પાછા Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  મૂળભૂત:
    પાછા ખાલી;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(સંરચના Curl_easy *data,
                           સંરચના Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           સંરચના Curl_dns_entry **pdns)
{
  પાછા Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

અચલ સંરચના Curl_https_rrinfo *
Curl_resolv_get_https(સંરચના Curl_easy *data, uint32_t resolv_id)
{
  સંરચના Curl_resolv_async *async;
  જો(!Curl_resolv_knows_https(data, resolv_id))
    પાછા ખાલી;
  async = Curl_async_get(data, resolv_id);
  પાછા async ? async->httpsrr : ખાલી;
}

bool Curl_resolv_knows_https(સંરચના Curl_easy *data, uint32_t resolv_id)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  જો(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    પાછા ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  પાછા TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
સ્થિર CURLcode hostip_resolv_start(સંરચના Curl_easy *data,
                                    uint8_t dns_queries,
                                    સંરચના Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    સંરચના Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  સંરચના Curl_resolv_async *async = ખાલી;
#endif
  સંરચના Curl_addrinfo *addr = ખાલી;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (શૂન્ય)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = ખાલી;

  /* Check for "known" things to resolve ourselves. */
  જો(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    જો(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, ખાલી);
      જો(result)
        જાઓ out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      જાઓ out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    જો(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, ખાલી);
      જો(result)
        જાઓ out;
      addr = get_localhost(peer->port, peer->hostname);
      જો(!addr)
        result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }
#ifndef CURL_DISABLE_DOH
  જો(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, ખાલી);
    જો(result)
      જાઓ out;
    જો(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      જો(!async) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
    result = Curl_doh(data, async);
    જાઓ out;
  }
#નહીંતર
  (શૂન્ય)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  જો(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    જાઓ out;
  }

#ifdef CURLRES_ASYNCH
  (શૂન્ય)addr;
  જો(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    જો(!async) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  જો(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    જો(r2)
      result = r2;
    નહીંતર જો(*pdns)
      result = CURLE_OK;
  }
#નહીંતર
  result = Curl_resolv_announce_start(data, ખાલી);
  જો(result)
    જાઓ out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  જો(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  જો(!result) {
    જો(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      જો(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    નહીંતર જો(!*pdns)
      result = CURLE_AGAIN;
  }
  નહીંતર જો(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  નહીંતર જો(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  જો(async) {
    જો(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    નહીંતર {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  પાછા result;
}

સ્થિર CURLcode hostip_resolv(સંરચના Curl_easy *data,
                              uint8_t dns_queries,
                              સંરચના Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              સંરચના Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (શૂન્ય)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = ખાલી;

#ifdef CURL_DISABLE_DOH
  (શૂન્ય)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  જો(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    જાઓ out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  જો((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    જાઓ out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  જો(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  નહીંતર જો(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  નહીંતર {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (પૂર્ણાંક)result);
  }

out:
  જો(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    જો(IS_RESOLV_FAIL(result)) {
      જો(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      જો(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    નહીંતર {
      failf(data, "Error %d resolving %s:%u",
            (પૂર્ણાંક)result, peer->hostname, peer->port);
    }
  }
  નહીંતર જો(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    જો(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  પાછા result;
}

CURLcode Curl_resolv_blocking(સંરચના Curl_easy *data,
                              uint8_t dns_queries,
                              અચલ અક્ષર *hostname,
                              uint16_t port,
                              uint8_t transport,
                              સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_peer *peer = ખાલી;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = ખાલી;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  જો(result)
    જાઓ out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  પસંદગી(result) {
  સ્થિતિ CURLE_OK:
    DEBUGASSERT(*pdns);
    તોડો;
#ifdef USE_CURL_ASYNC
  સ્થિતિ CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    તોડો;
#endif
  મૂળભૂત:
    તોડો;
  }

out:
  Curl_peer_unlink(&peer);
  પાછા result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN સ્થિર શૂન્ય alarmfunc(પૂર્ણાંક sig)
{
  (શૂન્ય)sig;
  siglongjmp(curl_jmpenv, 1);
}

સ્થિર CURLcode resolv_alarm_timeout(સંરચના Curl_easy *data,
                                     uint8_t dns_queries,
                                     સંરચના Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     સંરચના Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  સંરચના sigaction keep_sigact; /* store the old struct here */
  અસ્થિર bool keep_copysig = FALSE; /* whether old sigact has been saved */
  સંરચના sigaction sigact;
#નહીંતર
#ifdef HAVE_SIGNAL
  શૂન્ય (*keep_sigact)(પૂર્ણાંક);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  અસ્થિર long timeout;
  અસ્થિર unsigned પૂર્ણાંક prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = ખાલી;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  જો(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    પાછા CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  જો(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    જાઓ clean_up;
  }
  નહીંતર {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, ખાલી, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, ખાલી);
#નહીંતર /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = સંકેત(SIGALRM, alarmfunc);
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
  જો(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  જો(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, ખાલી); /* put it back */
  }
#નહીંતર
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  સંકેત(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  જો(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    જો(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    નહીંતર
      alarm((unsigned પૂર્ણાંક)alarm_set);
  }

  પાછા result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
સ્થિર CURLcode resolv_unix(સંરચના Curl_easy *data,
                            સંરચના Curl_peer *peer,
                            સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = ખાલી;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  જો(result) {
    જો(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    પાછા result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  પાછા *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(સંરચના Curl_easy *data,
                     સંરચના Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     સંરચના Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = ખાલી;

  જો(timeout_ms < 0)
    /* got an already expired timeout */
    પાછા CURLE_OPERATION_TIMEDOUT;
  નહીંતર જો(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  જો((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    પાછા resolv_unix(data, peer, pdns);
#નહીંતર
  જો(peer->unix_socket)
    પાછા hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  જો(dns_queries & CURL_DNSQ_ADDR) {
    જો(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    જો(timeout_ms && !Curl_doh_wanted(data)) {
      પાછા resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  જો(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  પાછા hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

સંરચના Curl_resolv_async *Curl_async_get(સંરચના Curl_easy *data,
                                         uint32_t resolv_id)
{
  સંરચના Curl_resolv_async *async = data->state.async;
  માટે(; async; async = async->next) {
    જો(async->id == resolv_id)
      પાછા async;
  }
  પાછા ખાલી;
}

CURLcode Curl_resolv_take_result(સંરચના Curl_easy *data, uint32_t resolv_id,
                                 સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  જો(!async)
    પાછા CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  જો(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    પાછા CURLE_OK;
  }
  નહીંતર જો(result) {
    Curl_async_shutdown(data, async);
    પાછા Curl_async_failed(data, async, ખાલી);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  જો(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    જો(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  નહીંતર જો(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    જો(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    જો(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  નહીંતર જો(result) {
    failf(data, "Error %d resolving %s:%u",
          (પૂર્ણાંક)result, async->peer->hostname, async->peer->port);
  }
  પાછા result;
}

CURLcode Curl_resolv_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps)
{
  સંરચના Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (શૂન્ય)ps;
  માટે(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    જો(async->doh) /* DoH has nothing for the pollset */
      ચાલુ;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  પાછા result;
}

શૂન્ય Curl_resolv_destroy(સંરચના Curl_easy *data, uint32_t resolv_id)
{
  સંરચના Curl_resolv_async **panchor = &data->state.async;

  માટે(; *panchor; panchor = &(*panchor)->next) {
    સંરચના Curl_resolv_async *async = *panchor;
    જો(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      તોડો;
    }
  }
}

શૂન્ય Curl_resolv_shutdown_all(સંરચના Curl_easy *data)
{
  સંરચના Curl_resolv_async *async = data->state.async;
  માટે(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

શૂન્ય Curl_resolv_destroy_all(સંરચના Curl_easy *data)
{
  જ્યારે(data->state.async) {
    સંરચના Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
