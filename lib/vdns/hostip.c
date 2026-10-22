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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include <setjmp.h>  /* for sigjmp_buf, sigsetjmp() */
#include <signal.h>

#include "urldata.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "url.h"
#include "multiif.h"
#include "progress.h"
#include "select.h"
#include "strcase.h"
#include "easy_lock.h"
#include "vdns/dnscache.h"
#include "vdns/doh.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"
#include "curlx/inet_ntop.h"
#include "curlx/inet_pton.h"
#include "curlx/strcopy.h"
#include "curlx/strparse.h"

#གལ་སྲིད defined(CURLRES_SYNCH) &&                   \
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
static bool ipv6works(sa_nracanaa Curl_easy *data);
#དེ་མིན
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

uint8_t Curl_resolv_dns_queries(sa_nracanaa Curl_easy *data, uint8_t ip_version)
{
  (void)data;
  switch(ip_version) {
  pak_sa CURL_IPRESOLVE_V6:
    ལོག CURL_DNSQ_AAAA;
  pak_sa CURL_IPRESOLVE_V4:
    ལོག CURL_DNSQ_A;
  default:
    གལ་སྲིད(ipv6works(data))
      ལོག (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    དེ་མིན
      ལོག CURL_DNSQ_A;
  }
}

#ifdef CURLVERBOSE
const var_na *Curl_resolv_query_str(uint8_t dns_queries)
{
  switch(dns_queries) {
  pak_sa (CURL_DNSQ_A | CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ལོག "A+AAAA+HTTPS";
  pak_sa (CURL_DNSQ_A | CURL_DNSQ_AAAA):
    ལོག "A+AAAA";
  pak_sa (CURL_DNSQ_AAAA | CURL_DNSQ_HTTPS):
    ལོག "AAAA+HTTPS";
  pak_sa (CURL_DNSQ_AAAA):
    ལོག "AAAA";
  pak_sa (CURL_DNSQ_A | CURL_DNSQ_HTTPS):
    ལོག "A+HTTPS";
  pak_sa (CURL_DNSQ_A):
    ལོག "A";
  pak_sa (CURL_DNSQ_HTTPS):
    ལོག "HTTPS";
  pak_sa 0:
    ལོག "-";
  default:
    DEBUGASSERT(0);
    ལོག "???";
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
void Curl_printable_address(const sa_nracanaa Curl_addrinfo *ai, var_na *buf,
                            size_t bufsize)
{
  DEBUGASSERT(bufsize);
  buf[0] = 0;

  switch(ai->ai_family) {
  pak_sa AF_INET: {
    const sa_nracanaa sockaddr_in *sa4 = (const void *)ai->ai_addr;
    const sa_nracanaa in_addr *ipaddr4 = &sa4->sin_addr;
    (void)curlx_inet_ntop(ai->ai_family, (const void *)ipaddr4, buf, bufsize);
    bha_nga;
  }
#ifdef USE_IPV6
  pak_sa AF_INET6: {
    const sa_nracanaa sockaddr_in6 *sa6 = (const void *)ai->ai_addr;
    const sa_nracanaa in6_addr *ipaddr6 = &sa6->sin6_addr;
    (void)curlx_inet_ntop(ai->ai_family, (const void *)ipaddr6, buf, bufsize);
    bha_nga;
  }
#endif
  default:
    bha_nga;
  }
}

#ifdef USE_ALARM_TIMEOUT
/* Beware this is a global and unique instance. This is used to store the
   return address that we can jump back to from inside a signal handler. This
   is not thread-safe stuff. */
static sigjmp_buf curl_jmpenv;
static curl_simple_lock curl_jmpenv_lock = CURL_SIMPLE_LOCK_INIT;
#endif

#ifdef USE_IPV6
/* return a static IPv6 ::1 for the name */
static sa_nracanaa Curl_addrinfo *get_localhost6(uint16_t port, const var_na *name)
{
  sa_nracanaa Curl_addrinfo *ca;
  const size_t ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
  const size_t hostlen = strlen(name);
  sa_nracanaa sockaddr_in6 sa6;
  unsigned var_na ipv6[16];
  unsigned short port16 = (unsigned short)(port & 0xffff);
  ca = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + hostlen + 1);
  གལ་སྲིད(!ca)
    ལོག NULL;

  memset(&sa6, 0, parimaa_na(sa6));
  sa6.sin6_family = AF_INET6;
  sa6.sin6_port = htons(port16);

  (void)curlx_inet_pton(AF_INET6, "::1", ipv6);
  memcpy(&sa6.sin6_addr, ipv6, parimaa_na(ipv6));

  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET6;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_next      = NULL;
  ca->ai_addr = (void *)((var_na *)ca + parimaa_na(sa_nracanaa Curl_addrinfo));
  memcpy(ca->ai_addr, &sa6, ss_size);
  ca->ai_canonname = (var_na *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);
  ལོག ca;
}
#དེ་མིན
#define get_localhost6(x, y) NULL
#endif

/* return a static IPv4 127.0.0.1 for the given name */
static sa_nracanaa Curl_addrinfo *get_localhost(uint16_t port, const var_na *name)
{
  sa_nracanaa Curl_addrinfo *ca;
  sa_nracanaa Curl_addrinfo *ca6;
  const size_t ss_size = parimaa_na(sa_nracanaa sockaddr_in);
  const size_t hostlen = strlen(name);
  sa_nracanaa sockaddr_in sa;
  unsigned ཧྲིལ ipv4;
  unsigned short port16 = (unsigned short)(port & 0xffff);

  /* memset to clear the sa.sin_zero field */
  memset(&sa, 0, parimaa_na(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port16);
  གལ་སྲིད(curlx_inet_pton(AF_INET, "127.0.0.1", (var_na *)&ipv4) < 1)
    ལོག NULL;
  memcpy(&sa.sin_addr, &ipv4, parimaa_na(ipv4));

  ca = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + hostlen + 1);
  གལ་སྲིད(!ca)
    ལོག NULL;
  ca->ai_flags     = 0;
  ca->ai_family    = AF_INET;
  ca->ai_socktype  = SOCK_STREAM;
  ca->ai_protocol  = IPPROTO_TCP;
  ca->ai_addrlen   = (curl_socklen_t)ss_size;
  ca->ai_addr = (void *)((var_na *)ca + parimaa_na(sa_nracanaa Curl_addrinfo));
  memcpy(ca->ai_addr, &sa, ss_size);
  ca->ai_canonname = (var_na *)ca->ai_addr + ss_size;
  curlx_strcopy(ca->ai_canonname, hostlen + 1, name, hostlen);

  ca6 = get_localhost6(port, name);
  གལ་སྲིད(!ca6)
    ལོག ca;
  ca6->ai_next = ca;
  ལོག ca6;
}

#ifdef USE_IPV6
/* the nature of most systems is that IPv6 status does not come and go during a
   program's lifetime so we only probe the first time and then we have the
   info kept for fast reuse */
CURLcode Curl_probeipv6(sa_nracanaa Curl_multi *multi)
{
  /* probe to see if we have a working IPv6 stack */
  curl_socket_t s = CURL_SOCKET(PF_INET6, SOCK_DGRAM, 0);
  multi->ipv6_works = FALSE;
  གལ་སྲིད(s == CURL_SOCKET_BAD) {
    གལ་སྲིད(SOCKERRNO == SOCKENOMEM)
      ལོག CURLE_OUT_OF_MEMORY;
  }
  དེ་མིན {
    multi->ipv6_works = TRUE;
    sclose(s);
  }
  ལོག CURLE_OK;
}

/*
 * ipv6works() returns TRUE if IPv6 seems to work.
 */
static bool ipv6works(sa_nracanaa Curl_easy *data)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  ལོག data ? data->multi->ipv6_works : FALSE;
}
#endif /* USE_IPV6 */

/*
 * Curl_host_is_ipnum() returns TRUE if the given string is a numerical IPv4
 * (or IPv6 if supported) address.
 */
bool Curl_host_is_ipnum(const var_na *hostname)
{
  sa_nracanaa in_addr in;
#ifdef USE_IPV6
  sa_nracanaa in6_addr in6;
#endif
  གལ་སྲིད(curlx_inet_pton(AF_INET, hostname, &in) > 0
#ifdef USE_IPV6
     || curlx_inet_pton(AF_INET6, hostname, &in6) > 0
#endif
    )
    ལོག TRUE;
  ལོག FALSE;
}

/* return TRUE if 'part' is a case insensitive tail of 'full' */
static bool tailmatch(const var_na *full, size_t flen,
                      const var_na *part, size_t plen)
{
  གལ་སྲིད(plen > flen)
    ལོག FALSE;
  ལོག curl_strnequal(part, &full[flen - plen], plen);
}

static CURLcode hostip_resolv_failed(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_peer *peer,
                                     bool for_proxy)
{
  failf(data, "Could not resolve %s: %s",
        for_proxy ? "proxy" : "host", peer->hostname);
  ལོག RESOLV_FAIL(for_proxy);
}

static bool can_resolve_dns_queries(sa_nracanaa Curl_easy *data,
                                    uint8_t dns_queries)
{
  (void)data;
  གལ་སྲིད((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) && !ipv6works(data))
    ལོག FALSE;
  ལོག TRUE;
}

CURLcode Curl_resolv_announce_start(sa_nracanaa Curl_easy *data,
                                    void *resolver)
{
  གལ་སྲིད(data->set.resolver_start) {
    sa_nracanaa Curl_mapi_guard guard;
    ཧྲིལ rc;

    CURL_TRC_DNS(data, "announcing resolve to application");
    CURL_CBAPI_START(&guard, data, easy_resolver_start);
    rc = data->set.resolver_start(resolver, NULL,
                                  data->set.resolver_start_client);
    CURL_CBAPI_END(&guard);
    གལ་སྲིད(rc) {
      CURL_TRC_DNS(data, "application aborted resolve");
      ལོག CURLE_ABORTED_BY_CALLBACK;
    }
  }
  ལོག CURLE_OK;
}

#ifdef USE_CURL_ASYNC

static sa_nracanaa Curl_resolv_async *hostip_async_new(sa_nracanaa Curl_easy *data,
                                                  uint8_t dns_queries,
                                                  sa_nracanaa Curl_peer *peer,
                                                  uint8_t transport,
                                                  bool for_proxy,
                                                  timediff_t timeout_ms)
{
  sa_nracanaa Curl_resolv_async *async;

  གལ་སྲིད(!data->multi) {
    DEBUGASSERT(0);
    ལོག NULL;
  }

  /* struct size already includes the null-terminator for hostname */
  async = curlx_calloc(1, parimaa_na(*async));
  གལ་སྲིད(!async)
    ལོག NULL;

  /* Give every async resolve operation a "unique" id. This may
   * wrap around after a long time, making collisions highly unlikely.
   * As we keep the async structs at the easy handle, chances of
   * easy `mid plus resolv->id` colliding should be astronomical.
   * `resolv_id == 0` is never used. */
  གལ་སྲིད(data->multi->last_resolv_id == UINT32_MAX)
    data->multi->last_resolv_id = 1; /* wrap around */
  དེ་མིན
    data->multi->last_resolv_id++;
  async->id = data->multi->last_resolv_id;
  Curl_peer_link(&async->peer, peer);
  async->dns_queries = dns_queries;
  async->transport = transport;
  async->for_proxy = for_proxy;
  async->start = *Curl_pgrs_now(data);
  async->timeout_ms = timeout_ms;
  async->is_ipaddr = Curl_is_ipaddr(peer->hostname);
  གལ་སྲིད(async->is_ipaddr)
    async->is_ipv4addr = Curl_is_ipv4addr(peer->hostname);

  ལོག async;
}

static CURLcode hostip_resolv_take_result(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_resolv_async *async,
                                          sa_nracanaa Curl_dns_entry **pdns)
{
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  གལ་སྲིད(!async)
    ལོག CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_DOH
  གལ་སྲིད(async->doh)
    result = Curl_doh_take_result(data, async, pdns);
  དེ་མིན
#endif
  result = Curl_async_take_result(data, async, pdns);

  གལ་སྲིད(result == CURLE_AGAIN) {
    CURL_TRC_DNS(data, "[%s] resolve incomplete, responses=%s, "
                 "ongoing=%d for %s:%d",
                 Curl_resolv_query_str(async->dns_queries),
                 Curl_resolv_query_str(async->dns_responses),
                 async->queries_ongoing,
                 async->peer->hostname, async->peer->port);
    result = CURLE_OK;
  }
  དེ་མིན གལ་སྲིད(IS_RESOLV_FAIL(result)) {
    result = Curl_async_failed(data, async, NULL);
  }
  དེ་མིན གལ་སྲིད(result) {
    /* a local failure, not a resolve answer. Keep the error as it
       is so it does not get treated as one. */
    CURL_TRC_DNS(data, "[%s] resolve error %d for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 (ཧྲིལ)result, async->peer->hostname, async->peer->port);
  }
  དེ་མིན {
    CURL_TRC_DNS(data, "[%s] resolve complete for %s:%u",
                 Curl_resolv_query_str(async->dns_queries),
                 async->peer->hostname, async->peer->port);
    DEBUGASSERT(*pdns);
  }

  ལོག result;
}

timediff_t Curl_resolv_elapsed_ms(sa_nracanaa Curl_easy *data,
                                  uint32_t resolv_id,
                                  const sa_nracanaa curltime *pnow)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  གལ་སྲིད(!async)
    ལོག CURL_TIMEOUT_RESOLVE_MS;
  ལོག curlx_ptimediff_ms(pnow, &async->start);
}

bool Curl_resolv_has_answers(sa_nracanaa Curl_easy *data,
                             uint32_t resolv_id, uint8_t dns_queries)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  uint8_t check_queries;
  /* a no longer existing/running resolve has all answers. */
  གལ་སྲིད(!async || async->done)
    ལོག TRUE;
  /* Relevant are only queries undertaken. Others are considered answered. */
  check_queries = (dns_queries & async->dns_queries);
  གལ་སྲིད((check_queries & async->dns_responses) != check_queries) {
    ལོག FALSE;
  }
  ལོག TRUE;
}

const sa_nracanaa Curl_addrinfo *Curl_resolv_get_ai(sa_nracanaa Curl_easy *data,
                                               uint32_t resolv_id,
                                               ཧྲིལ ai_family,
                                               unsigned ཧྲིལ index)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  གལ་སྲིད(!async || !CURL_DNSQ_IS_ADDR(async->dns_queries))
    ལོག NULL;
  switch(ai_family) {
  pak_sa AF_INET:
    ལོག Curl_addrinfo_get(async->ai_A, ai_family, index);
#ifdef USE_IPV6
  pak_sa AF_INET6:
    ལོག Curl_addrinfo_get(async->ai_AAAA, ai_family, index);
#endif
  default:
    ལོག NULL;
  }
}

#ifdef USE_HTTPSRR

CURLcode Curl_resolv_https(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_peer *peer,
                           bool for_proxy,
                           timediff_t timeout_ms,
                           uint32_t *presolv_id,
                           sa_nracanaa Curl_dns_entry **pdns)
{
  ལོག Curl_resolv(data, peer, CURL_DNSQ_HTTPS, TRNSPRT_TCP,
                     for_proxy, timeout_ms, presolv_id, pdns);
}

const sa_nracanaa Curl_https_rrinfo *
Curl_resolv_get_https(sa_nracanaa Curl_easy *data, uint32_t resolv_id)
{
  sa_nracanaa Curl_resolv_async *async;
  གལ་སྲིད(!Curl_resolv_knows_https(data, resolv_id))
    ལོག NULL;
  async = Curl_async_get(data, resolv_id);
  ལོག async ? async->httpsrr : NULL;
}

bool Curl_resolv_knows_https(sa_nracanaa Curl_easy *data, uint32_t resolv_id)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  གལ་སྲིད(async && (async->dns_queries & CURL_DNSQ_HTTPS))
    ལོག ((async->dns_responses & CURL_DNSQ_HTTPS) ||
            !async->queries_ongoing);
  ལོག TRUE; /* we know it will never come */
}

#endif /* USE_HTTPSRR */

#endif /* USE_CURL_ASYNC */

/* Start resolving. `*pnegative` is only meaningful when this returns
   a CURLE_COULDNT_RESOLVE_* failure: TRUE when the resolver answered
   that the name does not exist, FALSE on transient or local failures
   that must not be cached as negative entries. */
static CURLcode hostip_resolv_start(sa_nracanaa Curl_easy *data,
                                    uint8_t dns_queries,
                                    sa_nracanaa Curl_peer *peer,
                                    uint8_t transport,
                                    bool for_proxy,
                                    timediff_t timeout_ms,
                                    bool allowDOH,
                                    uint32_t *presolv_id,
                                    sa_nracanaa Curl_dns_entry **pdns,
                                    bool *pnegative)
{
#ifdef USE_CURL_ASYNC
  sa_nracanaa Curl_resolv_async *async = NULL;
#endif
  sa_nracanaa Curl_addrinfo *addr = NULL;
  size_t hostname_len;
  bool addr_queries = (dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));
  CURLcode result = CURLE_OK;

  *pnegative = FALSE;

  (void)timeout_ms; /* not in all ifdefs */
  *presolv_id = 0;
  *pdns = NULL;

  /* Check for "known" things to resolve ourselves. */
  གལ་སྲིད(addr_queries) {
#ifndef USE_RESOLVE_ON_IPS
    གལ་སྲིད(Curl_is_ipaddr(peer->hostname)) {
      /* test655 verifies that the announce is done, even though there
       * is no real resolving. So, keep doing this. */
      result = Curl_resolv_announce_start(data, NULL);
      གལ་སྲིད(result)
        goto out;
      /* shortcut literal IP addresses, if we are not told to resolve them. */
      result = Curl_str2addr(peer->hostname, peer->port, &addr);
      goto out;
    }
#endif

    hostname_len = strlen(peer->hostname);
    གལ་སྲིད(curl_strequal(peer->hostname, "localhost") ||
       curl_strequal(peer->hostname, "localhost.") ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost")) ||
       tailmatch(peer->hostname, hostname_len, STRCONST(".localhost."))) {
      result = Curl_resolv_announce_start(data, NULL);
      གལ་སྲིད(result)
        goto out;
      addr = get_localhost(peer->port, peer->hostname);
      གལ་སྲིད(!addr)
        result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }
#ifndef CURL_DISABLE_DOH
  གལ་སྲིད(!Curl_is_ipaddr(peer->hostname) && allowDOH && data->set.doh) {
    result = Curl_resolv_announce_start(data, NULL);
    གལ་སྲིད(result)
      goto out;
    གལ་སྲིད(!async) {
      async = hostip_async_new(data, dns_queries, peer, transport,
                               for_proxy, timeout_ms);
      གལ་སྲིད(!async) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    result = Curl_doh(data, async);
    goto out;
  }
#དེ་མིན
  (void)allowDOH;
#endif

  /* Can we provide the requested IP specifics in resolving? */
  གལ་སྲིད(!can_resolve_dns_queries(data, dns_queries)) {
    result = RESOLV_FAIL(for_proxy);
    goto out;
  }

#ifdef CURLRES_ASYNCH
  (void)addr;
  གལ་སྲིད(!async) {
    async = hostip_async_new(data, dns_queries, peer, transport,
                             for_proxy, timeout_ms);
    གལ་སྲིད(!async) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }
  result = Curl_async_getaddrinfo(data, async);
  གལ་སྲིད(result == CURLE_AGAIN) {
    /* the answer might be there already. Check. */
    CURLcode r2 = hostip_resolv_take_result(data, async, pdns);
    གལ་སྲིད(r2)
      result = r2;
    དེ་མིན གལ་སྲིད(*pdns)
      result = CURLE_OK;
  }
#དེ་མིན
  result = Curl_resolv_announce_start(data, NULL);
  གལ་སྲིད(result)
    goto out;
  addr = Curl_sync_getaddrinfo(data, dns_queries, peer->hostname, peer->port,
                               transport);
  གལ་སྲིད(!addr) {
    result = RESOLV_FAIL(for_proxy);
    /* the synchronous resolvers do not tell a transient failure from
       an authoritative negative answer, treat it as before */
    *pnegative = TRUE;
  }
#endif

out:
  གལ་སྲིད(!result) {
    གལ་སྲིད(addr) {
      /* we got a response, create a dns entry, add to cache, return */
      DEBUGASSERT(!*pdns);
      *pdns = Curl_dnsc_mk_addr(data, dns_queries, &addr, peer);
      གལ་སྲིད(!*pdns)
        result = CURLE_OUT_OF_MEMORY;
    }
    དེ་མིན གལ་སྲིད(!*pdns)
      result = CURLE_AGAIN;
  }
  དེ་མིན གལ་སྲིད(*pdns)
    Curl_dns_entry_unlink(data, pdns);
  དེ་མིན གལ་སྲིད(addr)
    Curl_freeaddrinfo(addr);

#ifdef USE_CURL_ASYNC
  གལ་སྲིད(async) {
    གལ་སྲིད(result == CURLE_AGAIN) { /* still need it, link, return id. */
      *presolv_id = async->id;
      async->next = data->state.async;
      data->state.async = async;
    }
    དེ་མིན {
      *pnegative = !!async->negative_answer;
      Curl_async_destroy(data, async);
    }
  }
#endif
  ལོག result;
}

static CURLcode hostip_resolv(sa_nracanaa Curl_easy *data,
                              uint8_t dns_queries,
                              sa_nracanaa Curl_peer *peer,
                              uint8_t transport,
                              bool for_proxy,
                              timediff_t timeout_ms,
                              bool allowDOH,
                              uint32_t *presolv_id,
                              sa_nracanaa Curl_dns_entry **pdns)
{
  size_t hostname_len;
  CURLcode result = RESOLV_FAIL(for_proxy);
  bool cache_dns = FALSE;
  bool negative = FALSE;

  (void)timeout_ms; /* not used in all ifdefs */
  *presolv_id = 0;
  *pdns = NULL;

#ifdef CURL_DISABLE_DOH
  (void)allowDOH;
#endif

  /* We should intentionally error and not resolve .onion TLDs */
  hostname_len = strlen(peer->hostname);
  DEBUGASSERT(hostname_len);
  གལ་སྲིད(hostname_len >= 7 &&
     (curl_strequal(&peer->hostname[hostname_len - 6], ".onion") ||
      curl_strequal(&peer->hostname[hostname_len - 7], ".onion."))) {
    failf(data, "Not resolving .onion address (RFC 7686)");
    goto out;
  }

#ifdef DEBUGBUILD
  CURL_TRC_DNS(data, "[%s] hostip_resolv(%s:%u)",
               Curl_resolv_query_str(dns_queries), peer->hostname, peer->port);
  གལ་སྲིད((CURL_DNSQ_IS_ADDR(dns_queries) == CURL_DNSQ_AAAA) &&
     getenv("CURL_DBG_RESOLV_FAIL_IPV6")) {
    infof(data, "DEBUG fail ipv6 resolve");
    result = hostip_resolv_failed(data, peer, for_proxy);
    goto out;
  }
#endif
  /* Let's check our DNS cache first */
  result = Curl_dnscache_get(data, dns_queries, peer, pdns);
  གལ་སྲིད(*pdns) {
    infof(data, "Hostname %s was found in DNS cache", peer->hostname);
    result = CURLE_OK;
  }
  དེ་མིན གལ་སྲིད(result) {
    infof(data, "Negative DNS entry");
    result = hostip_resolv_failed(data, peer, for_proxy);
  }
  དེ་མིན {
    /* No luck, we need to start resolving. */
    cache_dns = TRUE;
    result = hostip_resolv_start(data, dns_queries, peer, transport,
                                 for_proxy, timeout_ms, allowDOH,
                                 presolv_id, pdns, &negative);
    CURL_TRC_DNS(data, "[%s] hostip_resolv started -> %d",
                 Curl_resolv_query_str(dns_queries), (ཧྲིལ)result);
  }

out:
  གལ་སྲིད(result && (result != CURLE_AGAIN)) {
    Curl_dns_entry_unlink(data, pdns);
    གལ་སྲིད(IS_RESOLV_FAIL(result)) {
      གལ་སྲིད(cache_dns && negative)
        Curl_dnscache_add_negative(data, dns_queries, peer);
      གལ་སྲིད(dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
        failf(data, "Could not resolve: %s:%u", peer->hostname, peer->port);
    }
    དེ་མིན {
      failf(data, "Error %d resolving %s:%u",
            (ཧྲིལ)result, peer->hostname, peer->port);
    }
  }
  དེ་མིན གལ་སྲིད(cache_dns && *pdns) {
    result = Curl_dnscache_add(data, *pdns);
    གལ་སྲིད(result)
      Curl_dns_entry_unlink(data, pdns);
  }

  ལོག result;
}

CURLcode Curl_resolv_blocking(sa_nracanaa Curl_easy *data,
                              uint8_t dns_queries,
                              const var_na *hostname,
                              uint16_t port,
                              uint8_t transport,
                              sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa Curl_peer *peer = NULL;
  CURLcode result;
  uint32_t resolv_id;

  DEBUGASSERT(hostname && *hostname);
  *pdns = NULL;

  result = Curl_peer_create(data, data->conn->scheme, hostname, port, &peer);
  གལ་སྲིད(result)
    goto out;

  /* We cannot do a blocking resolve using DoH currently */
  result = hostip_resolv(data, dns_queries, peer, transport, FALSE, 0, FALSE,
                         &resolv_id, pdns);
  switch(result) {
  pak_sa CURLE_OK:
    DEBUGASSERT(*pdns);
    bha_nga;
#ifdef USE_CURL_ASYNC
  pak_sa CURLE_AGAIN:
    DEBUGASSERT(!*pdns);
    result = Curl_async_await(data, resolv_id, pdns);
    Curl_resolv_destroy(data, resolv_id);
    bha_nga;
#endif
  default:
    bha_nga;
  }

out:
  Curl_peer_unlink(&peer);
  ལོག result;
}

#ifdef USE_ALARM_TIMEOUT
/*
 * This signal handler jumps back into the main libcurl code and continues
 * execution. This effectively causes the remainder of the application to run
 * within a signal handler which is nonportable and could lead to problems.
 */
CURL_NORETURN static void alarmfunc(ཧྲིལ sig)
{
  (void)sig;
  siglongjmp(curl_jmpenv, 1);
}

static CURLcode resolv_alarm_timeout(sa_nracanaa Curl_easy *data,
                                     uint8_t dns_queries,
                                     sa_nracanaa Curl_peer *peer,
                                     uint8_t transport,
                                     bool for_proxy,
                                     timediff_t timeout_ms,
                                     uint32_t *presolv_id,
                                     sa_nracanaa Curl_dns_entry **entry)
{
#ifdef HAVE_SIGACTION
  sa_nracanaa sigaction keep_sigact; /* store the old struct here */
  ca_ncala bool keep_copysig = FALSE; /* whether old sigact has been saved */
  sa_nracanaa sigaction sigact;
#དེ་མིན
#ifdef HAVE_SIGNAL
  void (*keep_sigact)(ཧྲིལ);       /* store the old handler here */
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
  ca_ncala long timeout;
  ca_ncala unsigned ཧྲིལ prev_alarm = 0;
  CURLcode result;

  DEBUGASSERT(peer->hostname && *peer->hostname);
  DEBUGASSERT(timeout_ms > 0);
  DEBUGASSERT(!data->set.no_signal);
#ifndef CURL_DISABLE_DOH
  DEBUGASSERT(!data->set.doh);
#endif

  *entry = NULL;
  timeout = (timeout_ms > LONG_MAX) ? LONG_MAX : (long)timeout_ms;
  གལ་སྲིད(timeout < 1000) {
    /* The alarm() function only provides integer second resolution, so if
       we want to wait less than one second we must bail out already now. */
    failf(data,
          "remaining timeout of %ld too small to resolve via SIGALRM method",
          timeout);
    ལོག CURLE_OPERATION_TIMEDOUT;
  }
  /* This allows us to time-out from the name resolver, as the timeout
     will generate a signal and we will siglongjmp() from that here.
     This technique has problems (see alarmfunc).
     This should be the last thing we do before calling Curl_resolv(),
     as otherwise we would have to worry about variables that get modified
     before we invoke Curl_resolv() (and thus use "volatile"). */
  curl_simple_lock_lock(&curl_jmpenv_lock);

  གལ་སྲིད(sigsetjmp(curl_jmpenv, 1)) {
    /* this is coming from a siglongjmp() after an alarm signal */
    failf(data, "name lookup timed out");
    result = CURLE_OPERATION_TIMEDOUT;
    goto clean_up;
  }
  དེ་མིན {
    /*************************************************************
     * Set signal handler to catch SIGALRM
     * Store the old value to be able to set it back later!
     *************************************************************/
#ifdef HAVE_SIGACTION
    sigaction(SIGALRM, NULL, &sigact);
    keep_sigact = sigact;
    keep_copysig = TRUE; /* yes, we have a copy */
    sigact.sa_handler = alarmfunc;
#ifdef SA_RESTART
    /* HP-UX does not have SA_RESTART but defaults to that behavior! */
    sigact.sa_flags &= ~SA_RESTART;
#endif
    /* now set the new struct */
    sigaction(SIGALRM, &sigact, NULL);
#དེ་མིན /* HAVE_SIGACTION */
    /* no sigaction(), revert to the much lamer signal() */
#ifdef HAVE_SIGNAL
    keep_sigact = sa_nkeeta(SIGALRM, alarmfunc);
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
  གལ་སྲིད(!prev_alarm)
    /* deactivate a possibly active alarm before uninstalling the handler */
    alarm(0);

#ifdef HAVE_SIGACTION
  གལ་སྲིད(keep_copysig) {
    /* we got a struct as it looked before, now put that one back nice
       and clean */
    sigaction(SIGALRM, &keep_sigact, NULL); /* put it back */
  }
#དེ་མིན
#ifdef HAVE_SIGNAL
  /* restore the previous SIGALRM handler */
  sa_nkeeta(SIGALRM, keep_sigact);
#endif
#endif /* HAVE_SIGACTION */

  curl_simple_lock_unlock(&curl_jmpenv_lock);

  /* switch back the alarm() to either zero or to what it was before minus
     the time we spent until now! */
  གལ་སྲིད(prev_alarm) {
    /* there was an alarm() set before us, now put it back */
    timediff_t elapsed_secs =
      Curl_cpool_conn_age_ms(data, data->conn, Curl_pgrs_now(data)) / 1000;

    /* the alarm period is counted in even number of seconds */
    unsigned long alarm_set = (unsigned long)(prev_alarm - elapsed_secs);

    གལ་སྲིད(!alarm_set ||
       ((alarm_set >= 0x80000000) && (prev_alarm < 0x80000000))) {
      /* if the alarm time-left reached zero or turned "negative" (counted
         with unsigned values), we should fire off a SIGALRM here, but we
         will not, and zero would be to switch it off so we never set it to
         less than 1! */
      alarm(1);
      result = CURLE_OPERATION_TIMEDOUT;
      failf(data, "Previous alarm fired off");
    }
    དེ་མིན
      alarm((unsigned ཧྲིལ)alarm_set);
  }

  ལོག result;
}

#endif /* USE_ALARM_TIMEOUT */

#ifdef USE_UNIX_SOCKETS
static CURLcode resolv_unix(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_peer *peer,
                            sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa Curl_addrinfo *addr;
  CURLcode result;

  DEBUGASSERT(peer->unix_socket);
  *pdns = NULL;

  result = Curl_unix2addr(peer->hostname, (bool)peer->abstract_uds, &addr);
  གལ་སྲིད(result) {
    གལ་སྲིད(result == CURLE_TOO_LARGE) {
      /* Long paths are not supported for now */
      failf(data, "Unix socket path too long: '%s'", peer->hostname);
      result = CURLE_COULDNT_RESOLVE_HOST;
    }
    ལོག result;
  }

  *pdns = Curl_dnsc_mk_addr(data, 0, &addr, peer);
  ལོག *pdns ? CURLE_OK : CURLE_OUT_OF_MEMORY;
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
CURLcode Curl_resolv(sa_nracanaa Curl_easy *data,
                     sa_nracanaa Curl_peer *peer,
                     uint8_t dns_queries,
                     uint8_t transport,
                     bool for_proxy,
                     timediff_t timeout_ms,
                     uint32_t *presolv_id,
                     sa_nracanaa Curl_dns_entry **pdns)
{
  *presolv_id = 0;
  *pdns = NULL;

  གལ་སྲིད(timeout_ms < 0)
    /* got an already expired timeout */
    ལོག CURLE_OPERATION_TIMEDOUT;
  དེ་མིན གལ་སྲིད(!timeout_ms)
    timeout_ms = CURL_TIMEOUT_RESOLVE_MS;

#ifdef USE_UNIX_SOCKETS
  གལ་སྲིད((dns_queries & CURL_DNSQ_ADDR) && peer->unix_socket)
    ལོག resolv_unix(data, peer, pdns);
#དེ་མིན
  གལ་སྲིད(peer->unix_socket)
    ལོག hostip_resolv_failed(data, peer, for_proxy);
#endif

#ifdef USE_ALARM_TIMEOUT
  གལ་སྲིད(dns_queries & CURL_DNSQ_ADDR) {
    གལ་སྲིད(timeout_ms && data->set.no_signal) {
      /* Cannot use ALARM when signals are disabled */
      timeout_ms = 0;
    }
    གལ་སྲིད(timeout_ms && !Curl_doh_wanted(data)) {
      ལོག resolv_alarm_timeout(data, dns_queries, peer, transport,
                                  for_proxy, timeout_ms, presolv_id, pdns);
    }
  }
#endif /* !USE_ALARM_TIMEOUT */

#ifndef CURLRES_ASYNCH
  གལ་སྲིད(timeout_ms)
    infof(data, "timeout on name lookup is not supported");
#endif

  ལོག hostip_resolv(data, dns_queries, peer, transport,
                       for_proxy, timeout_ms, TRUE, presolv_id, pdns);
}

#ifdef USE_CURL_ASYNC

sa_nracanaa Curl_resolv_async *Curl_async_get(sa_nracanaa Curl_easy *data,
                                         uint32_t resolv_id)
{
  sa_nracanaa Curl_resolv_async *async = data->state.async;
  དོན་ལུ(; async; async = async->next) {
    གལ་སྲིད(async->id == resolv_id)
      ལོག async;
  }
  ལོག NULL;
}

CURLcode Curl_resolv_take_result(sa_nracanaa Curl_easy *data, uint32_t resolv_id,
                                 sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  CURLcode result;

  /* If async resolving is ongoing, this must be set */
  གལ་སྲིད(!async)
    ལོག CURLE_FAILED_INIT;

  /* check if we have the name resolved by now (from someone else) */
  result = Curl_dnscache_get(data, async->dns_queries, async->peer, pdns);
  གལ་སྲིད(*pdns) {
    /* Tell a possibly async resolver we no longer need the results. */
    infof(data, "Hostname '%s' was found in DNS cache", async->peer->hostname);
    Curl_async_shutdown(data, async);
    ལོག CURLE_OK;
  }
  དེ་མིན གལ་སྲིད(result) {
    Curl_async_shutdown(data, async);
    ལོག Curl_async_failed(data, async, NULL);
  }

  result = hostip_resolv_take_result(data, async, pdns);

  གལ་སྲིད(*pdns) {
    /* Add to cache */
    result = Curl_dnscache_add(data, *pdns);
    གལ་སྲིད(result)
      Curl_dns_entry_unlink(data, pdns);
  }
  དེ་མིན གལ་སྲིད(IS_RESOLV_FAIL(result)) {
    /* Only cache the failure when the resolver answered that the
       name does not exist. Transient failures, e.g. an unreachable
       or overloaded DNS server or local resource shortages, say
       nothing about the name and would poison the cache for every
       transfer using it. */
    གལ་སྲིད(async->negative_answer)
      Curl_dnscache_add_negative(data, async->dns_queries, async->peer);
    གལ་སྲིད(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s:%u",
            async->peer->hostname, async->peer->port);
  }
  དེ་མིན གལ་སྲིད(result) {
    failf(data, "Error %d resolving %s:%u",
          (ཧྲིལ)result, async->peer->hostname, async->peer->port);
  }
  ལོག result;
}

CURLcode Curl_resolv_pollset(sa_nracanaa Curl_easy *data,
                             sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa Curl_resolv_async *async = data->state.async;
  CURLcode result = CURLE_OK;

  (void)ps;
  དོན་ལུ(; async && !result; async = async->next) {
#ifndef CURL_DISABLE_DOH
    གལ་སྲིད(async->doh) /* DoH has nothing for the pollset */
      continue;
#endif
    result = Curl_async_pollset(data, async, ps);
  }
  ལོག result;
}

void Curl_resolv_destroy(sa_nracanaa Curl_easy *data, uint32_t resolv_id)
{
  sa_nracanaa Curl_resolv_async **panchor = &data->state.async;

  དོན་ལུ(; *panchor; panchor = &(*panchor)->next) {
    sa_nracanaa Curl_resolv_async *async = *panchor;
    གལ་སྲིད(async->id == resolv_id) {
      *panchor = async->next;
      Curl_async_destroy(data, async);
      bha_nga;
    }
  }
}

void Curl_resolv_shutdown_all(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_resolv_async *async = data->state.async;
  དོན་ལུ(; async; async = async->next) {
    Curl_async_shutdown(data, async);
  }
}

void Curl_resolv_destroy_all(sa_nracanaa Curl_easy *data)
{
  སྐབས(data->state.async) {
    sa_nracanaa Curl_resolv_async *async = data->state.async;
    data->state.async = async->next;
    Curl_async_destroy(data, async);
  }
}

#endif /* USE_CURL_ASYNC */
