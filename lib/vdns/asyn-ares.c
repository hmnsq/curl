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

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/timediff.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव <ares.h>

#जर ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#जर ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

स्थिर पूर्णांक ares_ver = 0;

स्थिर CURLcode async_ares_set_dns_servers(संरचना Curl_easy *data,
                                           संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_set_dns_interface(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_set_dns_local_ip4(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_set_dns_local_ip6(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
पूर्णांक Curl_async_global_init(शून्य)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  जर(ares_library_init(ARES_LIB_INIT_ALL)) {
    परत CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  परत CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
शून्य Curl_async_global_cleanup(शून्य)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

स्थिर शून्य sock_state_cb(शून्य *data, ares_socket_t socket_fd,
                          पूर्णांक readable, पूर्णांक writable)
{
  संरचना Curl_easy *easy = data;
  जर(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

स्थिर CURLcode async_ares_init(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  पूर्णांक status;
  संरचना ares_options options;
  पूर्णांक optmask = ARES_OPT_SOCK_STATE_CB;
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
  जर(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  जर(status != ARES_SUCCESS) {
    ares->channel = रिक्त;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    जा out;
  }

  result = async_ares_set_dns_servers(data, async);
  जर(result && result != CURLE_NOT_BUILT_IN)
    जा out;

  result = async_ares_set_dns_interface(data, async);
  जर(result && result != CURLE_NOT_BUILT_IN)
    जा out;

  result = async_ares_set_dns_local_ip4(data, async);
  जर(result && result != CURLE_NOT_BUILT_IN)
    जा out;

  result = async_ares_set_dns_local_ip6(data, async);
  जर(result && result != CURLE_NOT_BUILT_IN)
    जा out;

  result = CURLE_OK;

out:
  जर(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = रिक्त;
  }
  परत result;
}

शून्य Curl_async_ares_shutdown(संरचना Curl_easy *data,
                              संरचना Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

शून्य Curl_async_ares_destroy(संरचना Curl_easy *data,
                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  (शून्य)data;
  जर(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = रिक्त;
  }
}

CURLcode Curl_async_pollset(संरचना Curl_easy *data,
                            संरचना Curl_resolv_async *async,
                            संरचना easy_pollset *ps)
{
  संरचना async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  जर(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    जर(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  परत result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async,
                                संरचना Curl_dns_entry **pdns)
{
  संरचना async_ares_ctx *ares = &async->ares;
  संरचना Curl_dns_entry *dns = रिक्त;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त;
  जर(!ares)
    परत CURLE_FAILED_INIT;

  जर(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    जा out;
  }

  जर(async->queries_ongoing) {
    result = CURLE_AGAIN;
    जा out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  जर(ares->ares_status == ARES_SUCCESS && !result) {

    जर(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      जर(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }

#ifdef HTTPSRR_WORKS
    जर(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      जर(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  जर(!result && !dns) {
    स्थिरांक अक्षर *संदेश = रिक्त;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    जर(ares->ares_status != ARES_SUCCESS)
      संदेश = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, संदेश);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (पूर्णांक)result, (पूर्णांक)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  जर(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? रिक्त : dns;
  परत result;
}

स्थिर timediff_t async_ares_poll_timeout(संरचना async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  संरचना timeval *ares_calced, time_buf, max_timeout;
  पूर्णांक itimeout_ms;

#जर TIMEDIFF_T_MAX > पूर्णांक_उच्च
  itimeout_ms = (timeout_ms > पूर्णांक_उच्च) ? पूर्णांक_उच्च :
                 ((timeout_ms < 0) ? -1 : (पूर्णांक)timeout_ms);
#अन्यथा
  itimeout_ms = (पूर्णांक)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  जर(!ares_calced->tv_sec)
    परत (timediff_t)(ares_calced->tv_usec / 1000);
  अन्यथा
    परत 1000;
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
CURLcode Curl_async_await(संरचना Curl_easy *data, uint32_t resolv_id,
                          संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  संरचना curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त; /* clear on entry */

  जर(!ares)
    परत CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  पर्यंत(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    जर(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      जर(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      अन्यथा
        timeout_ms = -1;
    }

    जर(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      थांबा;
    }

    जर(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      थांबा;
    }

    result = Curl_async_take_result(data, async, pdns);
    जर(result == CURLE_AGAIN)
      result = CURLE_OK;
    अन्यथा जर(result || *pdns)
      थांबा;

    जर(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      थांबा;
    }
  }

  जर(result)
    ares_cancel(ares->channel);
  परत result;
}

#define CARES_MAX_ADDRS 256

स्थिर bool async_ares_addr_seen(स्थिरांक संरचना Curl_addrinfo *cafirst,
                                 पूर्णांक family, माप_प्रकार ss_size,
                                 स्थिरांक शून्य *addr)
{
  स्थिरांक संरचना Curl_addrinfo *ca;
  साठी(ca = cafirst; ca; ca = ca->ai_next) {
    जर((ca->ai_family == family) &&
       ((माप_प्रकार)ca->ai_addrlen == ss_size) &&
       !स_भेद(ca->ai_addr, addr, ss_size))
      परत TRUE;
  }
  परत FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
स्थिर संरचना Curl_addrinfo *async_ares_node2addr(
  संरचना ares_addrinfo_गाठ *गाठ)
{
  /* traverse the ares_addrinfo_node list */
  संरचना ares_addrinfo_गाठ *ai;
  संरचना Curl_addrinfo *cafirst = रिक्त;
  संरचना Curl_addrinfo *calast = रिक्त;
  पूर्णांक error = 0;
  अचिन्हित पूर्णांक naddrs = 0;

  साठी(ai = गाठ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    माप_प्रकार ss_size;
    संरचना Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    जर(ai->ai_family == AF_INET)
      ss_size = आकार(संरचना sockaddr_in);
#ifdef USE_IPV6
    अन्यथा जर(ai->ai_family == AF_INET6)
      ss_size = आकार(संरचना sockaddr_in6);
#endif
    अन्यथा
      सुरूठेवा;

    /* ignore elements without required address info */
    जर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      सुरूठेवा;

    /* ignore elements with bogus address size */
    जर((माप_प्रकार)ai->ai_addrlen < ss_size)
      सुरूठेवा;

    /* ignore duplicate addresses already collected */
    जर(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      सुरूठेवा;

    ca = curlx_malloc(आकार(संरचना Curl_addrinfo) + ss_size);
    जर(!ca) {
      error = EAI_MEMORY;
      थांबा;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_next      = रिक्त;

    ca->ai_addr = (शून्य *)((अक्षर *)ca + आकार(संरचना Curl_addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    जर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    जर(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  जर(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = रिक्त;
  }

  परत cafirst;
}

स्थिर शून्य async_ares_A_cb(शून्य *user_data, पूर्णांक status, पूर्णांक timeouts,
                            संरचना ares_addrinfo *ares_ai)
{
  संरचना Curl_resolv_async *async = user_data;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  (शून्य)timeouts;
  जर(!async)
    परत;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  जर(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    जर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    जर(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
स्थिर शून्य async_ares_AAAA_cb(शून्य *user_data, पूर्णांक status, पूर्णांक timeouts,
                               संरचना ares_addrinfo *ares_ai)
{
  संरचना Curl_resolv_async *async = user_data;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  (शून्य)timeouts;
  जर(!async)
    परत;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  जर(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    जर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    जर(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
स्थिर शून्य async_ares_rr_done(शून्य *user_data, ares_status_t status,
                               माप_प्रकार timeouts,
                               स्थिरांक ares_dns_record_t *dnsrec)
{
  संरचना Curl_resolv_async *async = user_data;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  जर(!async)
    परत;

  (शून्य)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  जर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  जर((ARES_SUCCESS != status) || !dnsrec)
    परत;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  अक्षर service[12];
  पूर्णांक socktype;
  CURLcode result = CURLE_OK;

  जर(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    जा out;
  }

  result = async_ares_init(data, async);
  जर(result)
    जा out;

  result = Curl_resolv_announce_start(data, ares->channel);
  जर(result)
    जा out;

#जर defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  जर(CURL_TRC_DNS_is_verbose(data)) {
    अक्षर *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, आकार(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  जर(async->dns_queries & CURL_DNSQ_AAAA) {
    संरचना ares_addrinfo_hints hints;

    स_रखो(&hints, 0, आकार(hints));
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

  जर(async->dns_queries & CURL_DNSQ_A) {
    संरचना ares_addrinfo_hints hints;

    स_रखो(&hints, 0, आकार(hints));
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
  जर(async->dns_queries & CURL_DNSQ_HTTPS) {
    अक्षर *https_name = रिक्त;
    जर(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      जर(!https_name)
        परत CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, रिक्त);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  परत result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
स्थिर CURLcode async_ares_set_dns_servers(संरचना Curl_easy *data,
                                           संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  CURLcode result = CURLE_NOT_BUILT_IN;
  स्थिरांक अक्षर *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  पूर्णांक ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  जर(दो_पर्या("CURL_DNS_SERVER"))
    servers = दो_पर्या("CURL_DNS_SERVER");
#endif

  जर(!servers)
    परत CURLE_OK;

  /* if channel is not there, this is a parameter check */
  जर(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  निवड(ares_result) {
  स्थिती ARES_SUCCESS:
    result = CURLE_OK;
    थांबा;
  स्थिती ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    थांबा;
  स्थिती ARES_ENOTINITIALIZED:
  स्थिती ARES_ENODATA:
  स्थिती ARES_EBADSTR:
  पूर्वनिर्धारित:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    थांबा;
  }
  परत result;
}

स्थिर CURLcode async_ares_set_dns_interface(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  स्थिरांक अक्षर *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  जर(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  जर(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  परत CURLE_OK;
}

स्थिर CURLcode async_ares_set_dns_local_ip4(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  संरचना in_addr a4;
  स्थिरांक अक्षर *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  जर(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  अन्यथा {
    जर(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  जर(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  परत CURLE_OK;
}

स्थिर CURLcode async_ares_set_dns_local_ip6(संरचना Curl_easy *data,
                                             संरचना Curl_resolv_async *async)
{
#ifdef USE_IPV6
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  अचिन्हित अक्षर a6[INET6_ADDRSTRLEN];
  स्थिरांक अक्षर *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  जर(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    स_रखो(a6, 0, आकार(a6));
  }
  अन्यथा {
    जर(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  जर(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  परत CURLE_OK;
#अन्यथा /* no IPv6 support */
  (शून्य)data;
  (शून्य)async;
  परत CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
