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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/timediff.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ <ares.h>

#எனில் ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#எனில் ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

நிலையான முழு ares_ver = 0;

நிலையான CURLcode async_ares_set_dns_servers(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு Curl_resolv_async *async);
நிலையான CURLcode async_ares_set_dns_interface(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async);
நிலையான CURLcode async_ares_set_dns_local_ip4(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async);
நிலையான CURLcode async_ares_set_dns_local_ip6(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
முழு Curl_async_global_init(வெறுமை)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  எனில்(ares_library_init(ARES_LIB_INIT_ALL)) {
    திரும்பு CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  திரும்பு CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
வெறுமை Curl_async_global_cleanup(வெறுமை)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

நிலையான வெறுமை sock_state_cb(வெறுமை *data, ares_socket_t socket_fd,
                          முழு readable, முழு writable)
{
  கட்டமைப்பு Curl_easy *easy = data;
  எனில்(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

நிலையான CURLcode async_ares_init(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = &async->ares;
  முழு status;
  கட்டமைப்பு ares_options options;
  முழு optmask = ARES_OPT_SOCK_STATE_CB;
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
  எனில்(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  எனில்(status != ARES_SUCCESS) {
    ares->channel = வெற்று;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    செல் out;
  }

  result = async_ares_set_dns_servers(data, async);
  எனில்(result && result != CURLE_NOT_BUILT_IN)
    செல் out;

  result = async_ares_set_dns_interface(data, async);
  எனில்(result && result != CURLE_NOT_BUILT_IN)
    செல் out;

  result = async_ares_set_dns_local_ip4(data, async);
  எனில்(result && result != CURLE_NOT_BUILT_IN)
    செல் out;

  result = async_ares_set_dns_local_ip6(data, async);
  எனில்(result && result != CURLE_NOT_BUILT_IN)
    செல் out;

  result = CURLE_OK;

out:
  எனில்(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = வெற்று;
  }
  திரும்பு result;
}

வெறுமை Curl_async_ares_shutdown(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

வெறுமை Curl_async_ares_destroy(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = &async->ares;
  (வெறுமை)data;
  எனில்(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = வெற்று;
  }
}

CURLcode Curl_async_pollset(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_resolv_async *async,
                            கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  எனில்(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    எனில்(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  திரும்பு result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async,
                                கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு async_ares_ctx *ares = &async->ares;
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = வெற்று;
  எனில்(!ares)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    செல் out;
  }

  எனில்(async->queries_ongoing) {
    result = CURLE_AGAIN;
    செல் out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  எனில்(ares->ares_status == ARES_SUCCESS && !result) {

    எனில்(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      எனில்(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }

#ifdef HTTPSRR_WORKS
    எனில்(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      எனில்(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  எனில்(!result && !dns) {
    மாறிலி எழுத்து *செய்தி = வெற்று;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    எனில்(ares->ares_status != ARES_SUCCESS)
      செய்தி = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, செய்தி);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (முழு)result, (முழு)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  எனில்(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? வெற்று : dns;
  திரும்பு result;
}

நிலையான timediff_t async_ares_poll_timeout(கட்டமைப்பு async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  கட்டமைப்பு timeval *ares_calced, time_buf, max_timeout;
  முழு itimeout_ms;

#எனில் TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (முழு)timeout_ms);
#இல்லையெனில்
  itimeout_ms = (முழு)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  எனில்(!ares_calced->tv_sec)
    திரும்பு (timediff_t)(ares_calced->tv_usec / 1000);
  இல்லையெனில்
    திரும்பு 1000;
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
CURLcode Curl_async_await(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id,
                          கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;
  கட்டமைப்பு curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = வெற்று; /* clear on entry */

  எனில்(!ares)
    திரும்பு CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  போது(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    எனில்(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      எனில்(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      இல்லையெனில்
        timeout_ms = -1;
    }

    எனில்(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    }

    எனில்(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      நிறுத்து;
    }

    result = Curl_async_take_result(data, async, pdns);
    எனில்(result == CURLE_AGAIN)
      result = CURLE_OK;
    இல்லையெனில் எனில்(result || *pdns)
      நிறுத்து;

    எனில்(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      நிறுத்து;
    }
  }

  எனில்(result)
    ares_cancel(ares->channel);
  திரும்பு result;
}

#define CARES_MAX_ADDRS 256

நிலையான bool async_ares_addr_seen(மாறிலி கட்டமைப்பு Curl_addrinfo *cafirst,
                                 முழு family, size_t ss_size,
                                 மாறிலி வெறுமை *addr)
{
  மாறிலி கட்டமைப்பு Curl_addrinfo *ca;
  க்கு(ca = cafirst; ca; ca = ca->ai_next) {
    எனில்((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
நிலையான கட்டமைப்பு Curl_addrinfo *async_ares_node2addr(
  கட்டமைப்பு ares_addrinfo_முனை *முனை)
{
  /* traverse the ares_addrinfo_node list */
  கட்டமைப்பு ares_addrinfo_முனை *ai;
  கட்டமைப்பு Curl_addrinfo *cafirst = வெற்று;
  கட்டமைப்பு Curl_addrinfo *calast = வெற்று;
  முழு error = 0;
  unsigned முழு naddrs = 0;

  க்கு(ai = முனை; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    கட்டமைப்பு Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    எனில்(ai->ai_family == AF_INET)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in);
#ifdef USE_IPV6
    இல்லையெனில் எனில்(ai->ai_family == AF_INET6)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
#endif
    இல்லையெனில்
      தொடர்;

    /* ignore elements without required address info */
    எனில்(!ai->ai_addr || !(ai->ai_addrlen > 0))
      தொடர்;

    /* ignore elements with bogus address size */
    எனில்((size_t)ai->ai_addrlen < ss_size)
      தொடர்;

    /* ignore duplicate addresses already collected */
    எனில்(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      தொடர்;

    ca = curlx_malloc(அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size);
    எனில்(!ca) {
      error = EAI_MEMORY;
      நிறுத்து;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = வெற்று;
    ca->ai_canonname = வெற்று;
    ca->ai_next      = வெற்று;

    ca->ai_addr = (வெறுமை *)((எழுத்து *)ca + அளவு(கட்டமைப்பு Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    எனில்(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    எனில்(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  எனில்(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = வெற்று;
  }

  திரும்பு cafirst;
}

நிலையான வெறுமை async_ares_A_cb(வெறுமை *user_data, முழு status, முழு timeouts,
                            கட்டமைப்பு ares_addrinfo *ares_ai)
{
  கட்டமைப்பு Curl_resolv_async *async = user_data;
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;

  (வெறுமை)timeouts;
  எனில்(!async)
    திரும்பு;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  எனில்(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  இல்லையெனில் {
    எனில்((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    எனில்(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
நிலையான வெறுமை async_ares_AAAA_cb(வெறுமை *user_data, முழு status, முழு timeouts,
                               கட்டமைப்பு ares_addrinfo *ares_ai)
{
  கட்டமைப்பு Curl_resolv_async *async = user_data;
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;

  (வெறுமை)timeouts;
  எனில்(!async)
    திரும்பு;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  எனில்(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  இல்லையெனில் {
    எனில்((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    எனில்(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
நிலையான வெறுமை async_ares_rr_done(வெறுமை *user_data, ares_status_t status,
                               size_t timeouts,
                               மாறிலி ares_dns_record_t *dnsrec)
{
  கட்டமைப்பு Curl_resolv_async *async = user_data;
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;

  எனில்(!async)
    திரும்பு;

  (வெறுமை)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  எனில்((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  எனில்((ARES_SUCCESS != status) || !dnsrec)
    திரும்பு;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = &async->ares;
  எழுத்து service[12];
  முழு socktype;
  CURLcode result = CURLE_OK;

  எனில்(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  result = async_ares_init(data, async);
  எனில்(result)
    செல் out;

  result = Curl_resolv_announce_start(data, ares->channel);
  எனில்(result)
    செல் out;

#எனில் defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  எனில்(CURL_TRC_DNS_is_verbose(data)) {
    எழுத்து *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, அளவு(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  எனில்(async->dns_queries & CURL_DNSQ_AAAA) {
    கட்டமைப்பு ares_addrinfo_hints hints;

    memset(&hints, 0, அளவு(hints));
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

  எனில்(async->dns_queries & CURL_DNSQ_A) {
    கட்டமைப்பு ares_addrinfo_hints hints;

    memset(&hints, 0, அளவு(hints));
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
  எனில்(async->dns_queries & CURL_DNSQ_HTTPS) {
    எழுத்து *https_name = வெற்று;
    எனில்(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      எனில்(!https_name)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, வெற்று);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  திரும்பு result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
நிலையான CURLcode async_ares_set_dns_servers(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;
  CURLcode result = CURLE_NOT_BUILT_IN;
  மாறிலி எழுத்து *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  முழு ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  எனில்(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  எனில்(!servers)
    திரும்பு CURLE_OK;

  /* if channel is not there, this is a parameter check */
  எனில்(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  தேர்வு(ares_result) {
  நிலை ARES_SUCCESS:
    result = CURLE_OK;
    நிறுத்து;
  நிலை ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    நிறுத்து;
  நிலை ARES_ENOTINITIALIZED:
  நிலை ARES_ENODATA:
  நிலை ARES_EBADSTR:
  இயல்பு:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode async_ares_set_dns_interface(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;
  மாறிலி எழுத்து *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  எனில்(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  எனில்(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode async_ares_set_dns_local_ip4(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;
  கட்டமைப்பு in_addr a4;
  மாறிலி எழுத்து *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  எனில்(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  இல்லையெனில் {
    எனில்(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  எனில்(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  திரும்பு CURLE_OK;
}

நிலையான CURLcode async_ares_set_dns_local_ip6(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_resolv_async *async)
{
#ifdef USE_IPV6
  கட்டமைப்பு async_ares_ctx *ares = async ? &async->ares : வெற்று;
  unsigned எழுத்து a6[INET6_ADDRSTRLEN];
  மாறிலி எழுத்து *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  எனில்(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, அளவு(a6));
  }
  இல்லையெனில் {
    எனில்(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  எனில்(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  திரும்பு CURLE_OK;
#இல்லையெனில் /* no IPv6 support */
  (வெறுமை)data;
  (வெறுமை)async;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
