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
#ضمن "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "url.h"
#ضمن "multiif.h"
#ضمن "curlx/inet_pton.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "progress.h"
#ضمن "curlx/timediff.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"
#ضمن <ares.h>

#אם ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#אם ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

ساكن صحيح ares_ver = 0;

ساكن CURLcode async_ares_set_dns_servers(بنية Curl_easy *data,
                                           بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_set_dns_interface(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_set_dns_local_ip4(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_set_dns_local_ip6(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحيح Curl_async_global_init(فراغ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  אם(ares_library_init(ARES_LIB_INIT_ALL)) {
    החזר CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  החזר CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
فراغ Curl_async_global_cleanup(فراغ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

ساكن فراغ sock_state_cb(فراغ *data, ares_socket_t socket_fd,
                          صحيح readable, صحيح writable)
{
  بنية Curl_easy *easy = data;
  אם(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ساكن CURLcode async_ares_init(بنية Curl_easy *data,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  صحيح status;
  بنية ares_options options;
  صحيح optmask = ARES_OPT_SOCK_STATE_CB;
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
  אם(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  אם(status != ARES_SUCCESS) {
    ares->channel = عدم;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    اذهب out;
  }

  result = async_ares_set_dns_servers(data, async);
  אם(result && result != CURLE_NOT_BUILT_IN)
    اذهب out;

  result = async_ares_set_dns_interface(data, async);
  אם(result && result != CURLE_NOT_BUILT_IN)
    اذهب out;

  result = async_ares_set_dns_local_ip4(data, async);
  אם(result && result != CURLE_NOT_BUILT_IN)
    اذهب out;

  result = async_ares_set_dns_local_ip6(data, async);
  אם(result && result != CURLE_NOT_BUILT_IN)
    اذهب out;

  result = CURLE_OK;

out:
  אם(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
  החזר result;
}

فراغ Curl_async_ares_shutdown(بنية Curl_easy *data,
                              بنية Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

فراغ Curl_async_ares_destroy(بنية Curl_easy *data,
                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  (فراغ)data;
  אם(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
}

CURLcode Curl_async_pollset(بنية Curl_easy *data,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  بنية async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  אם(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    אם(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  החזר result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(بنية Curl_easy *data,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_entry **pdns)
{
  بنية async_ares_ctx *ares = &async->ares;
  بنية Curl_dns_entry *dns = عدم;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;
  אם(!ares)
    החזר CURLE_FAILED_INIT;

  אם(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    اذهب out;
  }

  אם(async->queries_ongoing) {
    result = CURLE_AGAIN;
    اذهب out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  אם(ares->ares_status == ARES_SUCCESS && !result) {

    אם(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      אם(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }

#ifdef HTTPSRR_WORKS
    אם(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      אם(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  אם(!result && !dns) {
    ثابت حرف *msg = عدم;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    אם(ares->ares_status != ARES_SUCCESS)
      msg = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, msg);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحيح)result, (صحيح)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  אם(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? عدم : dns;
  החזר result;
}

ساكن timediff_t async_ares_poll_timeout(بنية async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  بنية timeval *ares_calced, time_buf, max_timeout;
  صحيح itimeout_ms;

#אם TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (صحيح)timeout_ms);
#אחרת
  itimeout_ms = (صحيح)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  אם(!ares_calced->tv_sec)
    החזר (timediff_t)(ares_calced->tv_usec / 1000);
  אחרת
    החזר 1000;
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
CURLcode Curl_async_await(بنية Curl_easy *data, uint32_t resolv_id,
                          بنية Curl_dns_entry **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم; /* clear on entry */

  אם(!ares)
    החזר CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  כלעוד(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    אם(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      אם(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      אחרת
        timeout_ms = -1;
    }

    אם(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }

    אם(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      اكسر;
    }

    result = Curl_async_take_result(data, async, pdns);
    אם(result == CURLE_AGAIN)
      result = CURLE_OK;
    אחרת אם(result || *pdns)
      اكسر;

    אם(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }

  אם(result)
    ares_cancel(ares->channel);
  החזר result;
}

#define CARES_MAX_ADDRS 256

ساكن bool async_ares_addr_seen(ثابت بنية Curl_addrinfo *cafirst,
                                 صحيح family, size_t ss_size,
                                 ثابت فراغ *addr)
{
  ثابت بنية Curl_addrinfo *ca;
  עבור(ca = cafirst; ca; ca = ca->ai_next) {
    אם((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      החזר TRUE;
  }
  החזר FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ساكن بنية Curl_addrinfo *async_ares_node2addr(
  بنية ares_addrinfo_node *node)
{
  /* traverse the ares_addrinfo_node list */
  بنية ares_addrinfo_node *ai;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  صحيح error = 0;
  unsigned صحيح naddrs = 0;

  עבור(ai = node; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    بنية Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    אם(ai->ai_family == AF_INET)
      ss_size = حجم(بنية sockaddr_in);
#ifdef USE_IPV6
    אחרת אם(ai->ai_family == AF_INET6)
      ss_size = حجم(بنية sockaddr_in6);
#endif
    אחרת
      استمر;

    /* ignore elements without required address info */
    אם(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    אם((size_t)ai->ai_addrlen < ss_size)
      استمر;

    /* ignore duplicate addresses already collected */
    אם(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_size);
    אם(!ca) {
      error = EAI_MEMORY;
      اكسر;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_next      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    אם(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    אם(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  אם(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = عدم;
  }

  החזר cafirst;
}

ساكن فراغ async_ares_A_cb(فراغ *user_data, صحيح status, صحيح timeouts,
                            بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_data;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  אם(!async)
    החזר;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  אם(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  אחרת {
    אם((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    אם(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
ساكن فراغ async_ares_AAAA_cb(فراغ *user_data, صحيح status, صحيح timeouts,
                               بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_data;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  אם(!async)
    החזר;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  אם(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  אחרת {
    אם((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    אם(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
ساكن فراغ async_ares_rr_done(فراغ *user_data, ares_status_t status,
                               size_t timeouts,
                               ثابت ares_dns_record_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_data;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  אם(!async)
    החזר;

  (فراغ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  אם((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  אם((ARES_SUCCESS != status) || !dnsrec)
    החזר;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(بنية Curl_easy *data,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  حرف service[12];
  صحيح socktype;
  CURLcode result = CURLE_OK;

  אם(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  result = async_ares_init(data, async);
  אם(result)
    اذهب out;

  result = Curl_resolv_announce_start(data, ares->channel);
  אם(result)
    اذهب out;

#אם defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  אם(CURL_TRC_DNS_is_verbose(data)) {
    حرف *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, حجم(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  אם(async->dns_queries & CURL_DNSQ_AAAA) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
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

  אם(async->dns_queries & CURL_DNSQ_A) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
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
  אם(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *https_name = عدم;
    אם(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      אם(!https_name)
        החזר CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, عدم);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  החזר result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ساكن CURLcode async_ares_set_dns_servers(بنية Curl_easy *data,
                                           بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  CURLcode result = CURLE_NOT_BUILT_IN;
  ثابت حرف *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  صحيح ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  אם(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  אם(!servers)
    החזר CURLE_OK;

  /* if channel is not there, this is a parameter check */
  אם(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  بدل(ares_result) {
  حالة ARES_SUCCESS:
    result = CURLE_OK;
    اكسر;
  حالة ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    اكسر;
  حالة ARES_ENOTINITIALIZED:
  حالة ARES_ENODATA:
  حالة ARES_EBADSTR:
  افتراضي:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اكسر;
  }
  החזר result;
}

ساكن CURLcode async_ares_set_dns_interface(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  ثابت حرف *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  אם(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  אם(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  החזר CURLE_OK;
}

ساكن CURLcode async_ares_set_dns_local_ip4(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية in_addr a4;
  ثابت حرف *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  אם(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  אחרת {
    אם(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  אם(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  החזר CURLE_OK;
}

ساكن CURLcode async_ares_set_dns_local_ip6(بنية Curl_easy *data,
                                             بنية Curl_resolv_async *async)
{
#ifdef USE_IPV6
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  unsigned حرف a6[INET6_ADDRSTRLEN];
  ثابت حرف *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  אם(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, حجم(a6));
  }
  אחרת {
    אם(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  אם(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  החזר CURLE_OK;
#אחרת /* no IPv6 support */
  (فراغ)data;
  (فراغ)async;
  החזר CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
