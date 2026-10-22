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

#अगर_घोषित USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
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

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "url.h"
#समावेश "multiif.h"
#समावेश "curlx/inet_pton.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "progress.h"
#समावेश "curlx/timediff.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"
#समावेश <ares.h>

#अगर ARES_VERSION < 0x011000
#त्रुटि "c-ares 1.16.0 or greater required"
#पूर्ण_अगर

#अगर_घोषित USE_HTTPSRR
#अगर ARES_VERSION < 0x011c00
#त्रुटि "c-ares 1.28.0 or greater required for HTTPSRR"
#पूर्ण_अगर
#घोषणा HTTPSRR_WORKS
#पूर्ण_अगर

#घोषणा CARES_TIMEOUT_PER_ATTEMPT 2000

अटल पूर्णांक ares_ver = 0;

अटल CURLcode async_ares_set_dns_servers(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा Curl_resolv_async *async);
अटल CURLcode async_ares_set_dns_interface(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async);
अटल CURLcode async_ares_set_dns_local_ip4(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async);
अटल CURLcode async_ares_set_dns_local_ip6(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
पूर्णांक Curl_async_global_init(व्योम)
{
#अगर_घोषित CARES_HAVE_ARES_LIBRARY_INIT
  अगर(ares_library_init(ARES_LIB_INIT_ALL)) {
    वापस CURLE_FAILED_INIT;
  }
#पूर्ण_अगर
  ares_version(&ares_ver);
  वापस CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
व्योम Curl_async_global_cleanup(व्योम)
{
#अगर_घोषित CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#पूर्ण_अगर
}

अटल व्योम sock_state_cb(व्योम *तथ्य, ares_socket_t socket_fd,
                          पूर्णांक readable, पूर्णांक writable)
{
  काष्ठा Curl_easy *easy = तथ्य;
  अगर(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

अटल CURLcode async_ares_init(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = &async->ares;
  पूर्णांक status;
  काष्ठा ares_options options;
  पूर्णांक optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode परिणाम = CURLE_OK;

  /* initial status - failed */
  ares->ares_status = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_तथ्य = तथ्य;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  अगर(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  अगर(status != ARES_SUCCESS) {
    ares->channel = शून्य;
    परिणाम = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    जाओ out;
  }

  परिणाम = async_ares_set_dns_servers(तथ्य, async);
  अगर(परिणाम && परिणाम != CURLE_NOT_BUILT_IN)
    जाओ out;

  परिणाम = async_ares_set_dns_interface(तथ्य, async);
  अगर(परिणाम && परिणाम != CURLE_NOT_BUILT_IN)
    जाओ out;

  परिणाम = async_ares_set_dns_local_ip4(तथ्य, async);
  अगर(परिणाम && परिणाम != CURLE_NOT_BUILT_IN)
    जाओ out;

  परिणाम = async_ares_set_dns_local_ip6(तथ्य, async);
  अगर(परिणाम && परिणाम != CURLE_NOT_BUILT_IN)
    जाओ out;

  परिणाम = CURLE_OK;

out:
  अगर(परिणाम && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = शून्य;
  }
  वापस परिणाम;
}

व्योम Curl_async_ares_shutdown(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(तथ्य, async);
}

व्योम Curl_async_ares_destroy(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = &async->ares;
  (व्योम)तथ्य;
  अगर(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = शून्य;
  }
}

CURLcode Curl_async_pollset(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_resolv_async *async,
                            काष्ठा easy_pollset *ps)
{
  काष्ठा async_ares_ctx *ares = &async->ares;
  CURLcode परिणाम = CURLE_OK;

  अगर(ares->channel) {
    परिणाम = Curl_ares_pollset(तथ्य, ares->channel, ps);
    अगर(!परिणाम) {
      timediff_t ms = Curl_ares_timeout_ms(तथ्य, async, ares->channel);
      Curl_expire(तथ्य, ms, EXPIRE_ASYNC_NAME);
    }
  }
  वापस परिणाम;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async,
                                काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा async_ares_ctx *ares = &async->ares;
  काष्ठा Curl_dns_entry *dns = शून्य;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = शून्य;
  अगर(!ares)
    वापस CURLE_FAILED_INIT;

  अगर(Curl_ares_perform(ares->channel, 0) < 0) {
    परिणाम = CURLE_UNRECOVERABLE_POLL;
    जाओ out;
  }

  अगर(async->queries_ongoing) {
    परिणाम = CURLE_AGAIN;
    जाओ out;
  }

  /* all c-ares operations done, what is the result to report? */
  परिणाम = ares->परिणाम;
  अगर(ares->ares_status == ARES_SUCCESS && !परिणाम) {

    अगर(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(तथ्य, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      अगर(!dns) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }

#अगर_घोषित HTTPSRR_WORKS
    अगर(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(तथ्य, &async->httpsrr, async->peer);
      अगर(!dns) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
#पूर्ण_अगर
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  अगर(!परिणाम && !dns) {
    स्थिर अक्षर *संदेश = शून्य;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    अगर(ares->ares_status != ARES_SUCCESS)
      संदेश = ares_strerror(ares->ares_status);
    परिणाम = Curl_async_failed(तथ्य, async, संदेश);
  }

  CURL_TRC_DNS(तथ्य, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (पूर्णांक)परिणाम, (पूर्णांक)ares->परिणाम, ares->ares_status,
               dns ? "" : "not ");

out:
  अगर(परिणाम != CURLE_AGAIN)
    ares->परिणाम = परिणाम;
  *pdns = परिणाम ? शून्य : dns;
  वापस परिणाम;
}

अटल timediff_t async_ares_poll_timeout(काष्ठा async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  काष्ठा timeval *ares_calced, time_buf, max_timeout;
  पूर्णांक itimeout_ms;

#अगर TIMEDIFF_T_MAX > पूर्णांक_उच्च
  itimeout_ms = (timeout_ms > पूर्णांक_उच्च) ? पूर्णांक_उच्च :
                 ((timeout_ms < 0) ? -1 : (पूर्णांक)timeout_ms);
#अन्यथा
  itimeout_ms = (पूर्णांक)timeout_ms;
#पूर्ण_अगर
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  अगर(!ares_calced->tv_sec)
    वापस (timediff_t)(ares_calced->tv_usec / 1000);
  अन्यथा
    वापस 1000;
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
CURLcode Curl_async_await(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id,
                          काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;
  काष्ठा curltime start = *Curl_pgrs_now(तथ्य);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = शून्य; /* clear on entry */

  अगर(!ares)
    वापस CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  जबतक(!परिणाम) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(तथ्य);
    अगर(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &start);
      अगर(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      अन्यथा
        timeout_ms = -1;
    }

    अगर(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    }

    अगर(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      परिणाम = CURLE_UNRECOVERABLE_POLL;
      अवरोध;
    }

    परिणाम = Curl_async_take_परिणाम(तथ्य, async, pdns);
    अगर(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    अन्यथा अगर(परिणाम || *pdns)
      अवरोध;

    अगर(Curl_pgrsUpdate(तथ्य)) {
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      अवरोध;
    }
  }

  अगर(परिणाम)
    ares_cancel(ares->channel);
  वापस परिणाम;
}

#घोषणा CARES_MAX_ADDRS 256

अटल bool async_ares_addr_seen(स्थिर काष्ठा Curl_addrinfo *cafirst,
                                 पूर्णांक family, माप_प्रकार ss_आकार,
                                 स्थिर व्योम *addr)
{
  स्थिर काष्ठा Curl_addrinfo *ca;
  क्रम(ca = cafirst; ca; ca = ca->ai_अग्र) {
    अगर((ca->ai_family == family) &&
       ((माप_प्रकार)ca->ai_addrlen == ss_आकार) &&
       !स_भेद(ca->ai_addr, addr, ss_आकार))
      वापस TRUE;
  }
  वापस FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
अटल काष्ठा Curl_addrinfo *async_ares_node2addr(
  काष्ठा ares_addrinfo_गाँठ *गाँठ)
{
  /* traverse the ares_addrinfo_node list */
  काष्ठा ares_addrinfo_गाँठ *ai;
  काष्ठा Curl_addrinfo *cafirst = शून्य;
  काष्ठा Curl_addrinfo *calast = शून्य;
  पूर्णांक error = 0;
  अचिन्हित पूर्णांक naddrs = 0;

  क्रम(ai = गाँठ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_अग्र) {
    माप_प्रकार ss_आकार;
    काष्ठा Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    अगर(ai->ai_family == AF_INET)
      ss_आकार = माप(काष्ठा sockaddr_in);
#अगर_घोषित USE_IPV6
    अन्यथा अगर(ai->ai_family == AF_INET6)
      ss_आकार = माप(काष्ठा sockaddr_in6);
#पूर्ण_अगर
    अन्यथा
      जारी;

    /* ignore elements without required address info */
    अगर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      जारी;

    /* ignore elements with bogus address size */
    अगर((माप_प्रकार)ai->ai_addrlen < ss_आकार)
      जारी;

    /* ignore duplicate addresses already collected */
    अगर(async_ares_addr_seen(cafirst, ai->ai_family, ss_आकार, ai->ai_addr))
      जारी;

    ca = curlx_malloc(माप(काष्ठा Curl_addrinfo) + ss_आकार);
    अगर(!ca) {
      error = EAI_MEMORY;
      अवरोध;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = शून्य;
    ca->ai_canonname = शून्य;
    ca->ai_अग्र      = शून्य;

    ca->ai_addr = (व्योम *)((अक्षर *)ca + माप(काष्ठा Curl_addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_आकार);

    /* if the return list is empty, this becomes the first element */
    अगर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    अगर(calast)
      calast->ai_अग्र = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  अगर(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = शून्य;
  }

  वापस cafirst;
}

अटल व्योम async_ares_A_cb(व्योम *user_तथ्य, पूर्णांक status, पूर्णांक timeouts,
                            काष्ठा ares_addrinfo *ares_ai)
{
  काष्ठा Curl_resolv_async *async = user_तथ्य;
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;

  (व्योम)timeouts;
  अगर(!async)
    वापस;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  अगर(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    अगर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    अगर(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#अगर_घोषित CURLRES_IPV6
अटल व्योम async_ares_AAAA_cb(व्योम *user_तथ्य, पूर्णांक status, पूर्णांक timeouts,
                               काष्ठा ares_addrinfo *ares_ai)
{
  काष्ठा Curl_resolv_async *async = user_तथ्य;
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;

  (व्योम)timeouts;
  अगर(!async)
    वापस;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  अगर(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    अगर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    अगर(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#पूर्ण_अगर /* CURLRES_IPV6 */

#अगर_घोषित USE_HTTPSRR
अटल व्योम async_ares_rr_done(व्योम *user_तथ्य, ares_status_t status,
                               माप_प्रकार timeouts,
                               स्थिर ares_dns_record_t *dnsrec)
{
  काष्ठा Curl_resolv_async *async = user_तथ्य;
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;

  अगर(!async)
    वापस;

  (व्योम)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  अगर((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  अगर((ARES_SUCCESS != status) || !dnsrec)
    वापस;
  ares->परिणाम = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#पूर्ण_अगर /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = &async->ares;
  अक्षर service[12];
  पूर्णांक socktype;
  CURLcode परिणाम = CURLE_OK;

  अगर(ares->channel) {
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  परिणाम = async_ares_init(तथ्य, async);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_resolv_announce_start(तथ्य, ares->channel);
  अगर(परिणाम)
    जाओ out;

#अगर defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  अगर(CURL_TRC_DNS_is_verbose(तथ्य)) {
    अक्षर *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(तथ्य, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#पूर्ण_अगर

  curl_msnprintf(service, माप(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(तथ्य, तथ्य->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#अगर_घोषित CURLRES_IPV6
  अगर(async->dns_queries & CURL_DNSQ_AAAA) {
    काष्ठा ares_addrinfo_hints hints;

    स_रखो(&hints, 0, माप(hints));
    CURL_TRC_DNS(तथ्य, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#पूर्ण_अगर /* CURLRES_IPV6 */

  अगर(async->dns_queries & CURL_DNSQ_A) {
    काष्ठा ares_addrinfo_hints hints;

    स_रखो(&hints, 0, माप(hints));
    CURL_TRC_DNS(तथ्य, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#अगर_घोषित USE_HTTPSRR
  अगर(async->dns_queries & CURL_DNSQ_HTTPS) {
    अक्षर *https_name = शून्य;
    अगर(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      अगर(!https_name)
        वापस CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(तथ्य, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, शून्य);
    curlx_free(https_name);
  }
#पूर्ण_अगर /* USE_HTTPSRR */

out:
  ares->परिणाम = परिणाम;
  वापस परिणाम ? परिणाम : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
अटल CURLcode async_ares_set_dns_servers(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;
  CURLcode परिणाम = CURLE_NOT_BUILT_IN;
  स्थिर अक्षर *servers = CURL_EASY_STR(तथ्य, STRING_DNS_SERVERS);
  पूर्णांक ares_परिणाम = ARES_SUCCESS;

#अगर_घोषित DEBUGBUILD
  अगर(दो_पर्या("CURL_DNS_SERVER"))
    servers = दो_पर्या("CURL_DNS_SERVER");
#पूर्ण_अगर

  अगर(!servers)
    वापस CURLE_OK;

  /* if channel is not there, this is a parameter check */
  अगर(ares && ares->channel)
    ares_परिणाम = ares_set_servers_ports_csv(ares->channel, servers);
  चयन(ares_परिणाम) {
  हाल ARES_SUCCESS:
    परिणाम = CURLE_OK;
    अवरोध;
  हाल ARES_ENOMEM:
    परिणाम = CURLE_OUT_OF_MEMORY;
    अवरोध;
  हाल ARES_ENOTINITIALIZED:
  हाल ARES_ENODATA:
  हाल ARES_EBADSTR:
  शेष:
    DEBUGF(infof(तथ्य, "bad servers set"));
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode async_ares_set_dns_interface(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;
  स्थिर अक्षर *interf = CURL_EASY_STR(तथ्य, STRING_DNS_INTERFACE);

  अगर(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  अगर(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  वापस CURLE_OK;
}

अटल CURLcode async_ares_set_dns_local_ip4(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;
  काष्ठा in_addr a4;
  स्थिर अक्षर *local_ip4 = CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP4);

  अगर(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  अन्यथा {
    अगर(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(तथ्य, "bad DNS IPv4 address"));
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  अगर(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  वापस CURLE_OK;
}

अटल CURLcode async_ares_set_dns_local_ip6(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_resolv_async *async)
{
#अगर_घोषित USE_IPV6
  काष्ठा async_ares_ctx *ares = async ? &async->ares : शून्य;
  अचिन्हित अक्षर a6[INET6_ADDRSTRLEN];
  स्थिर अक्षर *local_ip6 = CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP6);

  अगर(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    स_रखो(a6, 0, माप(a6));
  }
  अन्यथा {
    अगर(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(तथ्य, "bad DNS IPv6 address"));
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  अगर(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  वापस CURLE_OK;
#अन्यथा /* no IPv6 support */
  (व्योम)तथ्य;
  (व्योम)async;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

#पूर्ण_अगर /* USE_RESOLV_ARES */
