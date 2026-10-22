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
#شامل "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "url.h"
#شامل "multiif.h"
#شامل "curlx/inet_pton.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "progress.h"
#شامل "curlx/timediff.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل <ares.h>

#اگر ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#اگر ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

ایستا صحیح ares_ver = 0;

ایستا CURLcode async_ares_set_dns_servers(ساختار Curl_easy *data,
                                           ساختار Curl_resolv_async *async);
ایستا CURLcode async_ares_set_dns_interface(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async);
ایستا CURLcode async_ares_set_dns_local_ip4(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async);
ایستا CURLcode async_ares_set_dns_local_ip6(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحیح Curl_async_global_init(تهی)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  اگر(ares_library_init(ARES_LIB_INIT_ALL)) {
    بازگشت CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  بازگشت CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
تهی Curl_async_global_cleanup(تهی)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

ایستا تهی sock_state_cb(تهی *data, ares_socket_t socket_fd,
                          صحیح readable, صحیح writable)
{
  ساختار Curl_easy *easy = data;
  اگر(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ایستا CURLcode async_ares_init(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = &async->ares;
  صحیح status;
  ساختار ares_options options;
  صحیح optmask = ARES_OPT_SOCK_STATE_CB;
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
  اگر(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  اگر(status != ARES_SUCCESS) {
    ares->channel = پوچ;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    برو out;
  }

  result = async_ares_set_dns_servers(data, async);
  اگر(result && result != CURLE_NOT_BUILT_IN)
    برو out;

  result = async_ares_set_dns_interface(data, async);
  اگر(result && result != CURLE_NOT_BUILT_IN)
    برو out;

  result = async_ares_set_dns_local_ip4(data, async);
  اگر(result && result != CURLE_NOT_BUILT_IN)
    برو out;

  result = async_ares_set_dns_local_ip6(data, async);
  اگر(result && result != CURLE_NOT_BUILT_IN)
    برو out;

  result = CURLE_OK;

out:
  اگر(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = پوچ;
  }
  بازگشت result;
}

تهی Curl_async_ares_shutdown(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

تهی Curl_async_ares_destroy(ساختار Curl_easy *data,
                             ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = &async->ares;
  (تهی)data;
  اگر(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = پوچ;
  }
}

CURLcode Curl_async_pollset(ساختار Curl_easy *data,
                            ساختار Curl_resolv_async *async,
                            ساختار easy_pollset *ps)
{
  ساختار async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  اگر(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    اگر(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  بازگشت result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async,
                                ساختار Curl_dns_entry **pdns)
{
  ساختار async_ares_ctx *ares = &async->ares;
  ساختار Curl_dns_entry *dns = پوچ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = پوچ;
  اگر(!ares)
    بازگشت CURLE_FAILED_INIT;

  اگر(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    برو out;
  }

  اگر(async->queries_ongoing) {
    result = CURLE_AGAIN;
    برو out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  اگر(ares->ares_status == ARES_SUCCESS && !result) {

    اگر(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      اگر(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }

#ifdef HTTPSRR_WORKS
    اگر(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      اگر(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  اگر(!result && !dns) {
    ثابت نویسه *پیام = پوچ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    اگر(ares->ares_status != ARES_SUCCESS)
      پیام = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, پیام);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحیح)result, (صحیح)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  اگر(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? پوچ : dns;
  بازگشت result;
}

ایستا timediff_t async_ares_poll_timeout(ساختار async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ساختار timeval *ares_calced, time_buf, max_timeout;
  صحیح itimeout_ms;

#اگر TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (صحیح)timeout_ms);
#وگرنه
  itimeout_ms = (صحیح)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  اگر(!ares_calced->tv_sec)
    بازگشت (timediff_t)(ares_calced->tv_usec / 1000);
  وگرنه
    بازگشت 1000;
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
CURLcode Curl_async_await(ساختار Curl_easy *data, uint32_t resolv_id,
                          ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;
  ساختار curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = پوچ; /* clear on entry */

  اگر(!ares)
    بازگشت CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  وقتی(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    اگر(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      اگر(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      وگرنه
        timeout_ms = -1;
    }

    اگر(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    }

    اگر(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      بشکن;
    }

    result = Curl_async_take_result(data, async, pdns);
    اگر(result == CURLE_AGAIN)
      result = CURLE_OK;
    وگرنه اگر(result || *pdns)
      بشکن;

    اگر(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      بشکن;
    }
  }

  اگر(result)
    ares_cancel(ares->channel);
  بازگشت result;
}

#define CARES_MAX_ADDRS 256

ایستا bool async_ares_addr_seen(ثابت ساختار Curl_addrinfo *cafirst,
                                 صحیح family, size_t ss_size,
                                 ثابت تهی *addr)
{
  ثابت ساختار Curl_addrinfo *ca;
  برای(ca = cafirst; ca; ca = ca->ai_next) {
    اگر((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ایستا ساختار Curl_addrinfo *async_ares_node2addr(
  ساختار ares_addrinfo_گره *گره)
{
  /* traverse the ares_addrinfo_node list */
  ساختار ares_addrinfo_گره *ai;
  ساختار Curl_addrinfo *cafirst = پوچ;
  ساختار Curl_addrinfo *calast = پوچ;
  صحیح error = 0;
  unsigned صحیح naddrs = 0;

  برای(ai = گره; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    ساختار Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_size = اندازه(ساختار sockaddr_in);
#ifdef USE_IPV6
    وگرنه اگر(ai->ai_family == AF_INET6)
      ss_size = اندازه(ساختار sockaddr_in6);
#endif
    وگرنه
      ادامه;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ادامه;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_size)
      ادامه;

    /* ignore duplicate addresses already collected */
    اگر(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      ادامه;

    ca = curlx_malloc(اندازه(ساختار Curl_addrinfo) + ss_size);
    اگر(!ca) {
      error = EAI_MEMORY;
      بشکن;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = پوچ;
    ca->ai_canonname = پوچ;
    ca->ai_next      = پوچ;

    ca->ai_addr = (تهی *)((نویسه *)ca + اندازه(ساختار Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  اگر(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = پوچ;
  }

  بازگشت cafirst;
}

ایستا تهی async_ares_A_cb(تهی *user_data, صحیح status, صحیح timeouts,
                            ساختار ares_addrinfo *ares_ai)
{
  ساختار Curl_resolv_async *async = user_data;
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;

  (تهی)timeouts;
  اگر(!async)
    بازگشت;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  اگر(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  وگرنه {
    اگر((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    اگر(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
ایستا تهی async_ares_AAAA_cb(تهی *user_data, صحیح status, صحیح timeouts,
                               ساختار ares_addrinfo *ares_ai)
{
  ساختار Curl_resolv_async *async = user_data;
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;

  (تهی)timeouts;
  اگر(!async)
    بازگشت;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  اگر(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  وگرنه {
    اگر((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    اگر(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
ایستا تهی async_ares_rr_done(تهی *user_data, ares_status_t status,
                               size_t timeouts,
                               ثابت ares_dns_record_t *dnsrec)
{
  ساختار Curl_resolv_async *async = user_data;
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;

  اگر(!async)
    بازگشت;

  (تهی)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  اگر((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  اگر((ARES_SUCCESS != status) || !dnsrec)
    بازگشت;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = &async->ares;
  نویسه service[12];
  صحیح socktype;
  CURLcode result = CURLE_OK;

  اگر(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    برو out;
  }

  result = async_ares_init(data, async);
  اگر(result)
    برو out;

  result = Curl_resolv_announce_start(data, ares->channel);
  اگر(result)
    برو out;

#اگر defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  اگر(CURL_TRC_DNS_is_verbose(data)) {
    نویسه *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, اندازه(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  اگر(async->dns_queries & CURL_DNSQ_AAAA) {
    ساختار ares_addrinfo_hints hints;

    memset(&hints, 0, اندازه(hints));
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

  اگر(async->dns_queries & CURL_DNSQ_A) {
    ساختار ares_addrinfo_hints hints;

    memset(&hints, 0, اندازه(hints));
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
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    نویسه *https_name = پوچ;
    اگر(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      اگر(!https_name)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, پوچ);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  بازگشت result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ایستا CURLcode async_ares_set_dns_servers(ساختار Curl_easy *data,
                                           ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;
  CURLcode result = CURLE_NOT_BUILT_IN;
  ثابت نویسه *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  صحیح ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  اگر(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  اگر(!servers)
    بازگشت CURLE_OK;

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  انتخاب(ares_result) {
  حالت ARES_SUCCESS:
    result = CURLE_OK;
    بشکن;
  حالت ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    بشکن;
  حالت ARES_ENOTINITIALIZED:
  حالت ARES_ENODATA:
  حالت ARES_EBADSTR:
  پیشفرض:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode async_ares_set_dns_interface(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;
  ثابت نویسه *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  اگر(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  بازگشت CURLE_OK;
}

ایستا CURLcode async_ares_set_dns_local_ip4(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async)
{
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;
  ساختار in_addr a4;
  ثابت نویسه *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  اگر(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  وگرنه {
    اگر(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  اگر(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  بازگشت CURLE_OK;
}

ایستا CURLcode async_ares_set_dns_local_ip6(ساختار Curl_easy *data,
                                             ساختار Curl_resolv_async *async)
{
#ifdef USE_IPV6
  ساختار async_ares_ctx *ares = async ? &async->ares : پوچ;
  unsigned نویسه a6[INET6_ADDRSTRLEN];
  ثابت نویسه *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  اگر(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, اندازه(a6));
  }
  وگرنه {
    اگر(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  بازگشت CURLE_OK;
#وگرنه /* no IPv6 support */
  (تهی)data;
  (تهی)async;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
