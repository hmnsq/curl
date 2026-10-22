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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/timediff.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ <ares.h>

#ఒకవేళ ARES_సంస్కరణ < 0x011000
#దోషం "c-ares 1.16.0 or greater required"
#అయితే_ముగింపు

#నిర్వచితమైతే USE_HTTPSRR
#ఒకవేళ ARES_సంస్కరణ < 0x011c00
#దోషం "c-ares 1.28.0 or greater required for HTTPSRR"
#అయితే_ముగింపు
#నిర్వచించు HTTPSRR_పనిచేస్తుంది
#అయితే_ముగింపు

#నిర్వచించు CARES_TIMEOUT_చొప్పున_ATTEMPT 2000

స్థిర పూర్ణాంకం ares_ver = 0;

స్థిర CURLcode async_ares_సమితి_dns_servers(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం Curl_resolv_async *async);
స్థిర CURLcode async_ares_సమితి_dns_interface(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async);
స్థిర CURLcode async_ares_సమితి_dns_local_ip4(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async);
స్థిర CURLcode async_ares_సమితి_dns_local_ip6(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
పూర్ణాంకం Curl_async_global_init(శూన్య)
{
#నిర్వచితమైతే CARES_HAVE_ARES_LIBRARY_INIT
  ఒకవేళ(ares_library_init(ARES_LIB_INIT_ALL)) {
    తిరిగి CURLE_FAILED_INIT;
  }
#అయితే_ముగింపు
  ares_సంస్కరణ(&ares_ver);
  తిరిగి CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
శూన్య Curl_async_global_cleanup(శూన్య)
{
#నిర్వచితమైతే CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#అయితే_ముగింపు
}

స్థిర శూన్య sock_state_cb(శూన్య *దత్తాంశం, ares_socket_t socket_fd,
                          పూర్ణాంకం readable, పూర్ణాంకం writable)
{
  నిర్మాణం Curl_easy *easy = దత్తాంశం;
  ఒకవేళ(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

స్థిర CURLcode async_ares_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = &async->ares;
  పూర్ణాంకం స్థితి_2;
  నిర్మాణం ares_options options;
  పూర్ణాంకం optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode ఫలితం = CURLE_OK;

  /* initial status - failed */
  ares->ares_స్థితి = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_దత్తాంశం = దత్తాంశం;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  ఒకవేళ(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_చొప్పున_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  స్థితి_2 = ares_init_options(&ares->channel, &options, optmask);
  ఒకవేళ(స్థితి_2 != ARES_SUCCESS) {
    ares->channel = ఖాళీ;
    ఫలితం = (స్థితి_2 == ARES_ENOMEM) ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = async_ares_సమితి_dns_servers(దత్తాంశం, async);
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_NOT_BUILT_లోపల)
    వెళ్ళు బహిర్గతం;

  ఫలితం = async_ares_సమితి_dns_interface(దత్తాంశం, async);
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_NOT_BUILT_లోపల)
    వెళ్ళు బహిర్గతం;

  ఫలితం = async_ares_సమితి_dns_local_ip4(దత్తాంశం, async);
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_NOT_BUILT_లోపల)
    వెళ్ళు బహిర్గతం;

  ఫలితం = async_ares_సమితి_dns_local_ip6(దత్తాంశం, async);
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_NOT_BUILT_లోపల)
    వెళ్ళు బహిర్గతం;

  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ఖాళీ;
  }
  తిరిగి ఫలితం;
}

శూన్య Curl_async_ares_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(దత్తాంశం, async);
}

శూన్య Curl_async_ares_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = &async->ares;
  (శూన్య)దత్తాంశం;
  ఒకవేళ(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ఖాళీ;
  }
}

CURLcode Curl_async_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_resolv_async *async,
                            నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం async_ares_ctx *ares = &async->ares;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ares->channel) {
    ఫలితం = Curl_ares_pollset(దత్తాంశం, ares->channel, ps);
    ఒకవేళ(!ఫలితం) {
      timediff_t ms = Curl_ares_timeout_ms(దత్తాంశం, async, ares->channel);
      Curl_expire(దత్తాంశం, ms, EXPIRE_ASYNC_NAME);
    }
  }
  తిరిగి ఫలితం;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async,
                                నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం async_ares_ctx *ares = &async->ares;
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ఖాళీ;
  ఒకవేళ(!ares)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(Curl_ares_perform(ares->channel, 0) < 0) {
    ఫలితం = CURLE_UNRECOVERABLE_POLL;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(async->queries_ongoing) {
    ఫలితం = CURLE_AGAIN;
    వెళ్ళు బహిర్గతం;
  }

  /* all c-ares operations done, what is the result to report? */
  ఫలితం = ares->ఫలితం;
  ఒకవేళ(ares->ares_స్థితి == ARES_SUCCESS && !ఫలితం) {

    ఒకవేళ(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(దత్తాంశం, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      ఒకవేళ(!dns) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }

#నిర్వచితమైతే HTTPSRR_పనిచేస్తుంది
    ఒకవేళ(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(దత్తాంశం, &async->httpsrr, async->peer);
      ఒకవేళ(!dns) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
#అయితే_ముగింపు
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  ఒకవేళ(!ఫలితం && !dns) {
    స్థిరరాశి అక్షరం *సందేశం = ఖాళీ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_స్థితి == ARES_ENOTFOUND) ||
       (ares->ares_స్థితి == ARES_ENODATA));
    ఒకవేళ(ares->ares_స్థితి != ARES_SUCCESS)
      సందేశం = ares_strerror(ares->ares_స్థితి);
    ఫలితం = Curl_async_failed(దత్తాంశం, async, సందేశం);
  }

  CURL_TRC_DNS(దత్తాంశం, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (పూర్ణాంకం)ఫలితం, (పూర్ణాంకం)ares->ఫలితం, ares->ares_స్థితి,
               dns ? "" : "not ");

బహిర్గతం:
  ఒకవేళ(ఫలితం != CURLE_AGAIN)
    ares->ఫలితం = ఫలితం;
  *pdns = ఫలితం ? ఖాళీ : dns;
  తిరిగి ఫలితం;
}

స్థిర timediff_t async_ares_poll_timeout(నిర్మాణం async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  నిర్మాణం timeval *ares_calced, సమయం_buf, max_timeout;
  పూర్ణాంకం itimeout_ms;

#ఒకవేళ TIMEDIFF_T_MAX > గరిష్ఠ_పూర్ణాంకం
  itimeout_ms = (timeout_ms > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం :
                 ((timeout_ms < 0) ? -1 : (పూర్ణాంకం)timeout_ms);
#లేకపోతే
  itimeout_ms = (పూర్ణాంకం)timeout_ms;
#అయితే_ముగింపు
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &సమయం_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  ఒకవేళ(!ares_calced->tv_sec)
    తిరిగి (timediff_t)(ares_calced->tv_usec / 1000);
  లేకపోతే
    తిరిగి 1000;
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
CURLcode Curl_async_await(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id,
                          నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;
  నిర్మాణం curltime ప్రారంభం = *Curl_pgrs_now(దత్తాంశం);
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ఖాళీ; /* clear on entry */

  ఒకవేళ(!ares)
    తిరిగి CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  అయితే(!ఫలితం) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(దత్తాంశం);
    ఒకవేళ(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &ప్రారంభం);
      ఒకవేళ(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      లేకపోతే
        timeout_ms = -1;
    }

    ఒకవేళ(timeout_ms < 0) {
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    }

    ఒకవేళ(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      ఫలితం = CURLE_UNRECOVERABLE_POLL;
      ఆపు;
    }

    ఫలితం = Curl_async_take_ఫలితం(దత్తాంశం, async, pdns);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఫలితం = CURLE_OK;
    లేకపోతే ఒకవేళ(ఫలితం || *pdns)
      ఆపు;

    ఒకవేళ(Curl_pgrsUpdate(దత్తాంశం)) {
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
      ఆపు;
    }
  }

  ఒకవేళ(ఫలితం)
    ares_cancel(ares->channel);
  తిరిగి ఫలితం;
}

#నిర్వచించు CARES_MAX_ADDRS 256

స్థిర bool async_ares_addr_చూసిన(స్థిరరాశి నిర్మాణం Curl_addrinfo *cafirst,
                                 పూర్ణాంకం family, size_t ss_కొలత,
                                 స్థిరరాశి శూన్య *addr)
{
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ca;
  కొరకు(ca = cafirst; ca; ca = ca->ai_తదుపరి) {
    ఒకవేళ((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_కొలత) &&
       !memcmp(ca->ai_addr, addr, ss_కొలత))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
స్థిర నిర్మాణం Curl_addrinfo *async_ares_node2addr(
  నిర్మాణం ares_addrinfo_కణుపు *కణుపు)
{
  /* traverse the ares_addrinfo_node list */
  నిర్మాణం ares_addrinfo_కణుపు *ai;
  నిర్మాణం Curl_addrinfo *cafirst = ఖాళీ;
  నిర్మాణం Curl_addrinfo *calast = ఖాళీ;
  పూర్ణాంకం error = 0;
  గుర్తులేని పూర్ణాంకం naddrs = 0;

  కొరకు(ai = కణుపు; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_తదుపరి) {
    size_t ss_కొలత;
    నిర్మాణం Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ఒకవేళ(ai->ai_family == AF_INET)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);
#నిర్వచితమైతే USE_IPV6
    లేకపోతే ఒకవేళ(ai->ai_family == AF_INET6)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
#అయితే_ముగింపు
    లేకపోతే
      కొనసాగు;

    /* ignore elements without required address info */
    ఒకవేళ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      కొనసాగు;

    /* ignore elements with bogus address size */
    ఒకవేళ((size_t)ai->ai_addrlen < ss_కొలత)
      కొనసాగు;

    /* ignore duplicate addresses already collected */
    ఒకవేళ(async_ares_addr_చూసిన(cafirst, ai->ai_family, ss_కొలత, ai->ai_addr))
      కొనసాగు;

    ca = curlx_malloc(పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత);
    ఒకవేళ(!ca) {
      error = EAI_MEMORY;
      ఆపు;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_కొలత;
    ca->ai_addr      = ఖాళీ;
    ca->ai_canonname = ఖాళీ;
    ca->ai_తదుపరి      = ఖాళీ;

    ca->ai_addr = (శూన్య *)((అక్షరం *)ca + పరిమాణం(నిర్మాణం Curl_addrinfo));
    జ్ఞాపకం_నకలు(ca->ai_addr, ai->ai_addr, ss_కొలత);

    /* if the return list is empty, this becomes the first element */
    ఒకవేళ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ఒకవేళ(calast)
      calast->ai_తదుపరి = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  ఒకవేళ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ఖాళీ;
  }

  తిరిగి cafirst;
}

స్థిర శూన్య async_ares_A_cb(శూన్య *user_దత్తాంశం, పూర్ణాంకం స్థితి_2, పూర్ణాంకం timeouts,
                            నిర్మాణం ares_addrinfo *ares_ai)
{
  నిర్మాణం Curl_resolv_async *async = user_దత్తాంశం;
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;

  (శూన్య)timeouts;
  ఒకవేళ(!async)
    తిరిగి;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->పూర్తి = !async->queries_ongoing;
  ఒకవేళ(స్థితి_2 == ARES_SUCCESS) {
    ares->ares_స్థితి = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  లేకపోతే {
    ఒకవేళ((స్థితి_2 != ARES_ENOTFOUND) && (స్థితి_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    ఒకవేళ(ares->ares_స్థితి != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_స్థితి = స్థితి_2;
  }
}

#నిర్వచితమైతే CURLRES_IPV6
స్థిర శూన్య async_ares_AAAA_cb(శూన్య *user_దత్తాంశం, పూర్ణాంకం స్థితి_2, పూర్ణాంకం timeouts,
                               నిర్మాణం ares_addrinfo *ares_ai)
{
  నిర్మాణం Curl_resolv_async *async = user_దత్తాంశం;
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;

  (శూన్య)timeouts;
  ఒకవేళ(!async)
    తిరిగి;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->పూర్తి = !async->queries_ongoing;
  ఒకవేళ(స్థితి_2 == ARES_SUCCESS) {
    ares->ares_స్థితి = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  లేకపోతే {
    ఒకవేళ((స్థితి_2 != ARES_ENOTFOUND) && (స్థితి_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    ఒకవేళ(ares->ares_స్థితి != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_స్థితి = స్థితి_2;
  }
}
#అయితే_ముగింపు /* CURLRES_IPV6 */

#నిర్వచితమైతే USE_HTTPSRR
స్థిర శూన్య async_ares_rr_పూర్తి(శూన్య *user_దత్తాంశం, ares_స్థితి_t స్థితి_2,
                               size_t timeouts,
                               స్థిరరాశి ares_dns_రికార్డు_t *dnsrec)
{
  నిర్మాణం Curl_resolv_async *async = user_దత్తాంశం;
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;

  ఒకవేళ(!async)
    తిరిగి;

  (శూన్య)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->పూర్తి = !async->queries_ongoing;
  ares->ares_స్థితి = స్థితి_2;
  ఒకవేళ((స్థితి_2 != ARES_ENOTFOUND) && (స్థితి_2 != ARES_ENODATA))
    ares->transient_err = TRUE;
  ఒకవేళ((ARES_SUCCESS != స్థితి_2) || !dnsrec)
    తిరిగి;
  ares->ఫలితం = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#అయితే_ముగింపు /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = &async->ares;
  అక్షరం service[12];
  పూర్ణాంకం socktype;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ares->channel) {
    DEBUGASSERT(0);
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = async_ares_init(దత్తాంశం, async);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, ares->channel);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

#ఒకవేళ defined(CURLVERBOSE) && ARES_సంస్కరణ >= 0x011800 /* >= v1.24.0 */
  ఒకవేళ(CURL_TRC_DNS_is_verbose(దత్తాంశం)) {
    అక్షరం *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(దత్తాంశం, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#అయితే_ముగింపు

  curl_msnprintf(service, పరిమాణం(service), "%d", async->peer->పోర్టు);
  socktype =
    (Curl_conn_get_transport(దత్తాంశం, దత్తాంశం->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#నిర్వచితమైతే CURLRES_IPV6
  ఒకవేళ(async->dns_queries & CURL_DNSQ_AAAA) {
    నిర్మాణం ares_addrinfo_hints hints;

    memset(&hints, 0, పరిమాణం(hints));
    CURL_TRC_DNS(దత్తాంశం, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#అయితే_ముగింపు /* CURLRES_IPV6 */

  ఒకవేళ(async->dns_queries & CURL_DNSQ_A) {
    నిర్మాణం ares_addrinfo_hints hints;

    memset(&hints, 0, పరిమాణం(hints));
    CURL_TRC_DNS(దత్తాంశం, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#నిర్వచితమైతే USE_HTTPSRR
  ఒకవేళ(async->dns_queries & CURL_DNSQ_HTTPS) {
    అక్షరం *https_name = ఖాళీ;
    ఒకవేళ(async->peer->పోర్టు != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->పోర్టు, async->peer->hostname);
      ఒకవేళ(!https_name)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    CURL_TRC_DNS(దత్తాంశం, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_లోపల, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_పూర్తి, async, ఖాళీ);
    curlx_free(https_name);
  }
#అయితే_ముగింపు /* USE_HTTPSRR */

బహిర్గతం:
  ares->ఫలితం = ఫలితం;
  తిరిగి ఫలితం ? ఫలితం : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
స్థిర CURLcode async_ares_సమితి_dns_servers(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;
  CURLcode ఫలితం = CURLE_NOT_BUILT_లోపల;
  స్థిరరాశి అక్షరం *servers = CURL_EASY_STR(దత్తాంశం, STRING_DNS_SERVERS);
  పూర్ణాంకం ares_ఫలితం = ARES_SUCCESS;

#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#అయితే_ముగింపు

  ఒకవేళ(!servers)
    తిరిగి CURLE_OK;

  /* if channel is not there, this is a parameter check */
  ఒకవేళ(ares && ares->channel)
    ares_ఫలితం = ares_సమితి_servers_ports_csv(ares->channel, servers);
  ఎంపిక(ares_ఫలితం) {
  స్థితి ARES_SUCCESS:
    ఫలితం = CURLE_OK;
    ఆపు;
  స్థితి ARES_ENOMEM:
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఆపు;
  స్థితి ARES_ENOTINITIALIZED:
  స్థితి ARES_ENODATA:
  స్థితి ARES_EBADSTR:
  అప్రమేయ:
    DEBUGF(infof(దత్తాంశం, "bad servers set"));
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode async_ares_సమితి_dns_interface(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;
  స్థిరరాశి అక్షరం *interf = CURL_EASY_STR(దత్తాంశం, STRING_DNS_INTERFACE);

  ఒకవేళ(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  ఒకవేళ(ares && ares->channel)
    ares_సమితి_local_dev(ares->channel, interf);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode async_ares_సమితి_dns_local_ip4(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;
  నిర్మాణం in_addr a4;
  స్థిరరాశి అక్షరం *local_ip4 = CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP4);

  ఒకవేళ(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  లేకపోతే {
    ఒకవేళ(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(దత్తాంశం, "bad DNS IPv4 address"));
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  ఒకవేళ(ares && ares->channel)
    ares_సమితి_local_ip4(ares->channel, ntohl(a4.s_addr));

  తిరిగి CURLE_OK;
}

స్థిర CURLcode async_ares_సమితి_dns_local_ip6(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_resolv_async *async)
{
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం async_ares_ctx *ares = async ? &async->ares : ఖాళీ;
  గుర్తులేని అక్షరం a6[INET6_ADDRSTRLEN];
  స్థిరరాశి అక్షరం *local_ip6 = CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP6);

  ఒకవేళ(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, పరిమాణం(a6));
  }
  లేకపోతే {
    ఒకవేళ(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(దత్తాంశం, "bad DNS IPv6 address"));
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
  }

  /* if channel is not there, this is a parameter check */
  ఒకవేళ(ares && ares->channel)
    ares_సమితి_local_ip6(ares->channel, a6);

  తిరిగి CURLE_OK;
#లేకపోతే /* no IPv6 support */
  (శూన్య)దత్తాంశం;
  (శూన్య)async;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* USE_RESOLV_ARES */
