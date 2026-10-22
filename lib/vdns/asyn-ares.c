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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/timediff.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ <ares.h>

#എങ്കിൽ ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#എങ്കിൽ ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

സ്ഥിതം പൂർണ്ണസംഖ്യ ares_ver = 0;

സ്ഥിതം CURLcode async_ares_set_dns_servers(ഘടന Curl_easy *data,
                                           ഘടന Curl_resolv_async *async);
സ്ഥിതം CURLcode async_ares_set_dns_interface(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async);
സ്ഥിതം CURLcode async_ares_set_dns_local_ip4(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async);
സ്ഥിതം CURLcode async_ares_set_dns_local_ip6(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
പൂർണ്ണസംഖ്യ Curl_async_global_init(ശൂന്യം)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  എങ്കിൽ(ares_library_init(ARES_LIB_INIT_ALL)) {
    തിരികെ CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  തിരികെ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
ശൂന്യം Curl_async_global_cleanup(ശൂന്യം)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

സ്ഥിതം ശൂന്യം sock_state_cb(ശൂന്യം *data, ares_socket_t socket_fd,
                          പൂർണ്ണസംഖ്യ readable, പൂർണ്ണസംഖ്യ writable)
{
  ഘടന Curl_easy *easy = data;
  എങ്കിൽ(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

സ്ഥിതം CURLcode async_ares_init(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = &async->ares;
  പൂർണ്ണസംഖ്യ status;
  ഘടന ares_options options;
  പൂർണ്ണസംഖ്യ optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode result = CURLE_OK;

  /* initial status - failed */
  ares->ares_status = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_data = data;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  എങ്കിൽ(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  എങ്കിൽ(status != ARES_SUCCESS) {
    ares->channel = ശൂന്യ;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    പോകുക out;
  }

  result = async_ares_set_dns_servers(data, async);
  എങ്കിൽ(result && result != CURLE_NOT_BUILT_IN)
    പോകുക out;

  result = async_ares_set_dns_interface(data, async);
  എങ്കിൽ(result && result != CURLE_NOT_BUILT_IN)
    പോകുക out;

  result = async_ares_set_dns_local_ip4(data, async);
  എങ്കിൽ(result && result != CURLE_NOT_BUILT_IN)
    പോകുക out;

  result = async_ares_set_dns_local_ip6(data, async);
  എങ്കിൽ(result && result != CURLE_NOT_BUILT_IN)
    പോകുക out;

  result = CURLE_OK;

out:
  എങ്കിൽ(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ശൂന്യ;
  }
  തിരികെ result;
}

ശൂന്യം Curl_async_ares_shutdown(ഘടന Curl_easy *data,
                              ഘടന Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

ശൂന്യം Curl_async_ares_destroy(ഘടന Curl_easy *data,
                             ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = &async->ares;
  (ശൂന്യം)data;
  എങ്കിൽ(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ശൂന്യ;
  }
}

CURLcode Curl_async_pollset(ഘടന Curl_easy *data,
                            ഘടന Curl_resolv_async *async,
                            ഘടന easy_pollset *ps)
{
  ഘടന async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    എങ്കിൽ(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  തിരികെ result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async,
                                ഘടന Curl_dns_entry **pdns)
{
  ഘടന async_ares_ctx *ares = &async->ares;
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ശൂന്യ;
  എങ്കിൽ(!ares)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    പോകുക out;
  }

  എങ്കിൽ(async->queries_ongoing) {
    result = CURLE_AGAIN;
    പോകുക out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  എങ്കിൽ(ares->ares_status == ARES_SUCCESS && !result) {

    എങ്കിൽ(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      എങ്കിൽ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }

#ifdef HTTPSRR_WORKS
    എങ്കിൽ(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      എങ്കിൽ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  എങ്കിൽ(!result && !dns) {
    സ്ഥിരം അക്ഷരം *സന്ദേശം = ശൂന്യ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    എങ്കിൽ(ares->ares_status != ARES_SUCCESS)
      സന്ദേശം = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, സന്ദേശം);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (പൂർണ്ണസംഖ്യ)result, (പൂർണ്ണസംഖ്യ)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  എങ്കിൽ(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? ശൂന്യ : dns;
  തിരികെ result;
}

സ്ഥിതം timediff_t async_ares_poll_timeout(ഘടന async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ഘടന timeval *ares_calced, time_buf, max_timeout;
  പൂർണ്ണസംഖ്യ itimeout_ms;

#എങ്കിൽ TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (പൂർണ്ണസംഖ്യ)timeout_ms);
#അല്ലെങ്കിൽ
  itimeout_ms = (പൂർണ്ണസംഖ്യ)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  എങ്കിൽ(!ares_calced->tv_sec)
    തിരികെ (timediff_t)(ares_calced->tv_usec / 1000);
  അല്ലെങ്കിൽ
    തിരികെ 1000;
}

/*
 * Curl_async_await()
 *
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 *
 * 'pdns' MUST be non-NULL.
 *
 * Returns CURLE_COULDNT_RESOLVE_HOST if the host was not resolved,
 * CURLE_OPERATION_TIMEDOUT if a time-out occurred, or other errors.
 */
CURLcode Curl_async_await(ഘടന Curl_easy *data, uint32_t resolv_id,
                          ഘടന Curl_dns_entry **pdns)
{
  ഘടന Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;
  ഘടന curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ശൂന്യ; /* clear on entry */

  എങ്കിൽ(!ares)
    തിരികെ CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  ആകുമ്പോൾ(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    എങ്കിൽ(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      എങ്കിൽ(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      അല്ലെങ്കിൽ
        timeout_ms = -1;
    }

    എങ്കിൽ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    }

    എങ്കിൽ(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      നിർത്തുക;
    }

    result = Curl_async_take_result(data, async, pdns);
    എങ്കിൽ(result == CURLE_AGAIN)
      result = CURLE_OK;
    അല്ലെങ്കിൽ എങ്കിൽ(result || *pdns)
      നിർത്തുക;

    എങ്കിൽ(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      നിർത്തുക;
    }
  }

  എങ്കിൽ(result)
    ares_cancel(ares->channel);
  തിരികെ result;
}

#define CARES_MAX_ADDRS 256

സ്ഥിതം bool async_ares_addr_seen(സ്ഥിരം ഘടന Curl_addrinfo *cafirst,
                                 പൂർണ്ണസംഖ്യ family, size_t ss_size,
                                 സ്ഥിരം ശൂന്യം *addr)
{
  സ്ഥിരം ഘടന Curl_addrinfo *ca;
  വേണ്ടി(ca = cafirst; ca; ca = ca->ai_next) {
    എങ്കിൽ((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
സ്ഥിതം ഘടന Curl_addrinfo *async_ares_node2addr(
  ഘടന ares_addrinfo_കണ്ണി *കണ്ണി)
{
  /* traverse the ares_addrinfo_node list */
  ഘടന ares_addrinfo_കണ്ണി *ai;
  ഘടന Curl_addrinfo *cafirst = ശൂന്യ;
  ഘടന Curl_addrinfo *calast = ശൂന്യ;
  പൂർണ്ണസംഖ്യ error = 0;
  unsigned പൂർണ്ണസംഖ്യ naddrs = 0;

  വേണ്ടി(ai = കണ്ണി; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    ഘടന Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    എങ്കിൽ(ai->ai_family == AF_INET)
      ss_size = വലുപ്പം(ഘടന sockaddr_in);
#ifdef USE_IPV6
    അല്ലെങ്കിൽ എങ്കിൽ(ai->ai_family == AF_INET6)
      ss_size = വലുപ്പം(ഘടന sockaddr_in6);
#endif
    അല്ലെങ്കിൽ
      തുടരുക;

    /* ignore elements without required address info */
    എങ്കിൽ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      തുടരുക;

    /* ignore elements with bogus address size */
    എങ്കിൽ((size_t)ai->ai_addrlen < ss_size)
      തുടരുക;

    /* ignore duplicate addresses already collected */
    എങ്കിൽ(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      തുടരുക;

    ca = curlx_malloc(വലുപ്പം(ഘടന Curl_addrinfo) + ss_size);
    എങ്കിൽ(!ca) {
      error = EAI_MEMORY;
      നിർത്തുക;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ശൂന്യ;
    ca->ai_canonname = ശൂന്യ;
    ca->ai_next      = ശൂന്യ;

    ca->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ca + വലുപ്പം(ഘടന Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    എങ്കിൽ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    എങ്കിൽ(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  എങ്കിൽ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ശൂന്യ;
  }

  തിരികെ cafirst;
}

സ്ഥിതം ശൂന്യം async_ares_A_cb(ശൂന്യം *user_data, പൂർണ്ണസംഖ്യ status, പൂർണ്ണസംഖ്യ timeouts,
                            ഘടന ares_addrinfo *ares_ai)
{
  ഘടന Curl_resolv_async *async = user_data;
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;

  (ശൂന്യം)timeouts;
  എങ്കിൽ(!async)
    തിരികെ;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  എങ്കിൽ(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    എങ്കിൽ(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
സ്ഥിതം ശൂന്യം async_ares_AAAA_cb(ശൂന്യം *user_data, പൂർണ്ണസംഖ്യ status, പൂർണ്ണസംഖ്യ timeouts,
                               ഘടന ares_addrinfo *ares_ai)
{
  ഘടന Curl_resolv_async *async = user_data;
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;

  (ശൂന്യം)timeouts;
  എങ്കിൽ(!async)
    തിരികെ;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  എങ്കിൽ(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    എങ്കിൽ(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
സ്ഥിതം ശൂന്യം async_ares_rr_done(ശൂന്യം *user_data, ares_status_t status,
                               size_t timeouts,
                               സ്ഥിരം ares_dns_record_t *dnsrec)
{
  ഘടന Curl_resolv_async *async = user_data;
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;

  എങ്കിൽ(!async)
    തിരികെ;

  (ശൂന്യം)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  എങ്കിൽ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  എങ്കിൽ((ARES_SUCCESS != status) || !dnsrec)
    തിരികെ;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = &async->ares;
  അക്ഷരം service[12];
  പൂർണ്ണസംഖ്യ socktype;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }

  result = async_ares_init(data, async);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_resolv_announce_start(data, ares->channel);
  എങ്കിൽ(result)
    പോകുക out;

#എങ്കിൽ defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  എങ്കിൽ(CURL_TRC_DNS_is_verbose(data)) {
    അക്ഷരം *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, വലുപ്പം(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_AAAA) {
    ഘടന ares_addrinfo_hints hints;

    memset(&hints, 0, വലുപ്പം(hints));
    CURL_TRC_DNS(data, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#endif /* CURLRES_IPV6 */

  എങ്കിൽ(async->dns_queries & CURL_DNSQ_A) {
    ഘടന ares_addrinfo_hints hints;

    memset(&hints, 0, വലുപ്പം(hints));
    CURL_TRC_DNS(data, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#ifdef USE_HTTPSRR
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_HTTPS) {
    അക്ഷരം *https_name = ശൂന്യ;
    എങ്കിൽ(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      എങ്കിൽ(!https_name)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, ശൂന്യ);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  തിരികെ result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
സ്ഥിതം CURLcode async_ares_set_dns_servers(ഘടന Curl_easy *data,
                                           ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;
  CURLcode result = CURLE_NOT_BUILT_IN;
  സ്ഥിരം അക്ഷരം *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  പൂർണ്ണസംഖ്യ ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  എങ്കിൽ(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  എങ്കിൽ(!servers)
    തിരികെ CURLE_OK;

  /* if channel is not there, this is a parameter check */
  എങ്കിൽ(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  തിരഞ്ഞെടുപ്പ്(ares_result) {
  അവസ്ഥ ARES_SUCCESS:
    result = CURLE_OK;
    നിർത്തുക;
  അവസ്ഥ ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    നിർത്തുക;
  അവസ്ഥ ARES_ENOTINITIALIZED:
  അവസ്ഥ ARES_ENODATA:
  അവസ്ഥ ARES_EBADSTR:
  സ്ഥിരസ്ഥിതി:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode async_ares_set_dns_interface(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;
  സ്ഥിരം അക്ഷരം *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  എങ്കിൽ(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  എങ്കിൽ(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode async_ares_set_dns_local_ip4(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async)
{
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;
  ഘടന in_addr a4;
  സ്ഥിരം അക്ഷരം *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  എങ്കിൽ(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  എങ്കിൽ(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode async_ares_set_dns_local_ip6(ഘടന Curl_easy *data,
                                             ഘടന Curl_resolv_async *async)
{
#ifdef USE_IPV6
  ഘടന async_ares_ctx *ares = async ? &async->ares : ശൂന്യ;
  unsigned അക്ഷരം a6[INET6_ADDRSTRLEN];
  സ്ഥിരം അക്ഷരം *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  എങ്കിൽ(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, വലുപ്പം(a6));
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  എങ്കിൽ(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  തിരികെ CURLE_OK;
#അല്ലെങ്കിൽ /* no IPv6 support */
  (ശൂന്യം)data;
  (ശൂന്യം)async;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
