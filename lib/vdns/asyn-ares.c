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

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
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

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/timediff.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ <ares.h>

#જો ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#જો ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

સ્થિર પૂર્ણાંક ares_ver = 0;

સ્થિર CURLcode async_ares_set_dns_servers(સંરચના Curl_easy *data,
                                           સંરચના Curl_resolv_async *async);
સ્થિર CURLcode async_ares_set_dns_interface(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async);
સ્થિર CURLcode async_ares_set_dns_local_ip4(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async);
સ્થિર CURLcode async_ares_set_dns_local_ip6(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
પૂર્ણાંક Curl_async_global_init(શૂન્ય)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  જો(ares_library_init(ARES_LIB_INIT_ALL)) {
    પાછા CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  પાછા CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
શૂન્ય Curl_async_global_cleanup(શૂન્ય)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

સ્થિર શૂન્ય sock_state_cb(શૂન્ય *data, ares_socket_t socket_fd,
                          પૂર્ણાંક readable, પૂર્ણાંક writable)
{
  સંરચના Curl_easy *easy = data;
  જો(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

સ્થિર CURLcode async_ares_init(સંરચના Curl_easy *data,
                                સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = &async->ares;
  પૂર્ણાંક status;
  સંરચના ares_options options;
  પૂર્ણાંક optmask = ARES_OPT_SOCK_STATE_CB;
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
  જો(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  જો(status != ARES_SUCCESS) {
    ares->channel = ખાલી;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    જાઓ out;
  }

  result = async_ares_set_dns_servers(data, async);
  જો(result && result != CURLE_NOT_BUILT_IN)
    જાઓ out;

  result = async_ares_set_dns_interface(data, async);
  જો(result && result != CURLE_NOT_BUILT_IN)
    જાઓ out;

  result = async_ares_set_dns_local_ip4(data, async);
  જો(result && result != CURLE_NOT_BUILT_IN)
    જાઓ out;

  result = async_ares_set_dns_local_ip6(data, async);
  જો(result && result != CURLE_NOT_BUILT_IN)
    જાઓ out;

  result = CURLE_OK;

out:
  જો(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ખાલી;
  }
  પાછા result;
}

શૂન્ય Curl_async_ares_shutdown(સંરચના Curl_easy *data,
                              સંરચના Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

શૂન્ય Curl_async_ares_destroy(સંરચના Curl_easy *data,
                             સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = &async->ares;
  (શૂન્ય)data;
  જો(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ખાલી;
  }
}

CURLcode Curl_async_pollset(સંરચના Curl_easy *data,
                            સંરચના Curl_resolv_async *async,
                            સંરચના easy_pollset *ps)
{
  સંરચના async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  જો(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    જો(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  પાછા result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(સંરચના Curl_easy *data,
                                સંરચના Curl_resolv_async *async,
                                સંરચના Curl_dns_entry **pdns)
{
  સંરચના async_ares_ctx *ares = &async->ares;
  સંરચના Curl_dns_entry *dns = ખાલી;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ખાલી;
  જો(!ares)
    પાછા CURLE_FAILED_INIT;

  જો(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    જાઓ out;
  }

  જો(async->queries_ongoing) {
    result = CURLE_AGAIN;
    જાઓ out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  જો(ares->ares_status == ARES_SUCCESS && !result) {

    જો(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      જો(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }

#ifdef HTTPSRR_WORKS
    જો(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      જો(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  જો(!result && !dns) {
    અચલ અક્ષર *સંદેશ = ખાલી;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    જો(ares->ares_status != ARES_SUCCESS)
      સંદેશ = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, સંદેશ);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (પૂર્ણાંક)result, (પૂર્ણાંક)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  જો(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? ખાલી : dns;
  પાછા result;
}

સ્થિર timediff_t async_ares_poll_timeout(સંરચના async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  સંરચના timeval *ares_calced, time_buf, max_timeout;
  પૂર્ણાંક itimeout_ms;

#જો TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (પૂર્ણાંક)timeout_ms);
#નહીંતર
  itimeout_ms = (પૂર્ણાંક)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  જો(!ares_calced->tv_sec)
    પાછા (timediff_t)(ares_calced->tv_usec / 1000);
  નહીંતર
    પાછા 1000;
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
CURLcode Curl_async_await(સંરચના Curl_easy *data, uint32_t resolv_id,
                          સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;
  સંરચના curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ખાલી; /* clear on entry */

  જો(!ares)
    પાછા CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  જ્યારે(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    જો(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      જો(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      નહીંતર
        timeout_ms = -1;
    }

    જો(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    }

    જો(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      તોડો;
    }

    result = Curl_async_take_result(data, async, pdns);
    જો(result == CURLE_AGAIN)
      result = CURLE_OK;
    નહીંતર જો(result || *pdns)
      તોડો;

    જો(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      તોડો;
    }
  }

  જો(result)
    ares_cancel(ares->channel);
  પાછા result;
}

#define CARES_MAX_ADDRS 256

સ્થિર bool async_ares_addr_seen(અચલ સંરચના Curl_addrinfo *cafirst,
                                 પૂર્ણાંક family, size_t ss_size,
                                 અચલ શૂન્ય *addr)
{
  અચલ સંરચના Curl_addrinfo *ca;
  માટે(ca = cafirst; ca; ca = ca->ai_next) {
    જો((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      પાછા TRUE;
  }
  પાછા FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
સ્થિર સંરચના Curl_addrinfo *async_ares_node2addr(
  સંરચના ares_addrinfo_ગાંઠ *ગાંઠ)
{
  /* traverse the ares_addrinfo_node list */
  સંરચના ares_addrinfo_ગાંઠ *ai;
  સંરચના Curl_addrinfo *cafirst = ખાલી;
  સંરચના Curl_addrinfo *calast = ખાલી;
  પૂર્ણાંક error = 0;
  unsigned પૂર્ણાંક naddrs = 0;

  માટે(ai = ગાંઠ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    સંરચના Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    જો(ai->ai_family == AF_INET)
      ss_size = કદ(સંરચના sockaddr_in);
#ifdef USE_IPV6
    નહીંતર જો(ai->ai_family == AF_INET6)
      ss_size = કદ(સંરચના sockaddr_in6);
#endif
    નહીંતર
      ચાલુ;

    /* ignore elements without required address info */
    જો(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ચાલુ;

    /* ignore elements with bogus address size */
    જો((size_t)ai->ai_addrlen < ss_size)
      ચાલુ;

    /* ignore duplicate addresses already collected */
    જો(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      ચાલુ;

    ca = curlx_malloc(કદ(સંરચના Curl_addrinfo) + ss_size);
    જો(!ca) {
      error = EAI_MEMORY;
      તોડો;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ખાલી;
    ca->ai_canonname = ખાલી;
    ca->ai_next      = ખાલી;

    ca->ai_addr = (શૂન્ય *)((અક્ષર *)ca + કદ(સંરચના Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    જો(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    જો(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  જો(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ખાલી;
  }

  પાછા cafirst;
}

સ્થિર શૂન્ય async_ares_A_cb(શૂન્ય *user_data, પૂર્ણાંક status, પૂર્ણાંક timeouts,
                            સંરચના ares_addrinfo *ares_ai)
{
  સંરચના Curl_resolv_async *async = user_data;
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;

  (શૂન્ય)timeouts;
  જો(!async)
    પાછા;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  જો(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  નહીંતર {
    જો((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    જો(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
સ્થિર શૂન્ય async_ares_AAAA_cb(શૂન્ય *user_data, પૂર્ણાંક status, પૂર્ણાંક timeouts,
                               સંરચના ares_addrinfo *ares_ai)
{
  સંરચના Curl_resolv_async *async = user_data;
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;

  (શૂન્ય)timeouts;
  જો(!async)
    પાછા;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  જો(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  નહીંતર {
    જો((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    જો(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
સ્થિર શૂન્ય async_ares_rr_done(શૂન્ય *user_data, ares_status_t status,
                               size_t timeouts,
                               અચલ ares_dns_record_t *dnsrec)
{
  સંરચના Curl_resolv_async *async = user_data;
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;

  જો(!async)
    પાછા;

  (શૂન્ય)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  જો((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  જો((ARES_SUCCESS != status) || !dnsrec)
    પાછા;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(સંરચના Curl_easy *data,
                                સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = &async->ares;
  અક્ષર service[12];
  પૂર્ણાંક socktype;
  CURLcode result = CURLE_OK;

  જો(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    જાઓ out;
  }

  result = async_ares_init(data, async);
  જો(result)
    જાઓ out;

  result = Curl_resolv_announce_start(data, ares->channel);
  જો(result)
    જાઓ out;

#જો defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  જો(CURL_TRC_DNS_is_verbose(data)) {
    અક્ષર *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, કદ(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  જો(async->dns_queries & CURL_DNSQ_AAAA) {
    સંરચના ares_addrinfo_hints hints;

    memset(&hints, 0, કદ(hints));
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

  જો(async->dns_queries & CURL_DNSQ_A) {
    સંરચના ares_addrinfo_hints hints;

    memset(&hints, 0, કદ(hints));
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
  જો(async->dns_queries & CURL_DNSQ_HTTPS) {
    અક્ષર *https_name = ખાલી;
    જો(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      જો(!https_name)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, ખાલી);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  પાછા result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
સ્થિર CURLcode async_ares_set_dns_servers(સંરચના Curl_easy *data,
                                           સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;
  CURLcode result = CURLE_NOT_BUILT_IN;
  અચલ અક્ષર *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  પૂર્ણાંક ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  જો(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  જો(!servers)
    પાછા CURLE_OK;

  /* if channel is not there, this is a parameter check */
  જો(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  પસંદગી(ares_result) {
  સ્થિતિ ARES_SUCCESS:
    result = CURLE_OK;
    તોડો;
  સ્થિતિ ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    તોડો;
  સ્થિતિ ARES_ENOTINITIALIZED:
  સ્થિતિ ARES_ENODATA:
  સ્થિતિ ARES_EBADSTR:
  મૂળભૂત:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode async_ares_set_dns_interface(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;
  અચલ અક્ષર *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  જો(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  જો(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  પાછા CURLE_OK;
}

સ્થિર CURLcode async_ares_set_dns_local_ip4(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async)
{
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;
  સંરચના in_addr a4;
  અચલ અક્ષર *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  જો(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  નહીંતર {
    જો(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  જો(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  પાછા CURLE_OK;
}

સ્થિર CURLcode async_ares_set_dns_local_ip6(સંરચના Curl_easy *data,
                                             સંરચના Curl_resolv_async *async)
{
#ifdef USE_IPV6
  સંરચના async_ares_ctx *ares = async ? &async->ares : ખાલી;
  unsigned અક્ષર a6[INET6_ADDRSTRLEN];
  અચલ અક્ષર *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  જો(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, કદ(a6));
  }
  નહીંતર {
    જો(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  જો(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  પાછા CURLE_OK;
#નહીંતર /* no IPv6 support */
  (શૂન્ય)data;
  (શૂન્ય)async;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
