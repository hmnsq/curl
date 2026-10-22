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

#جيڪڏهن_وصف_ٿيل USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

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

#إذا ARES_نسخو < 0x011000
#غلطي "c-ares 1.16.0 or greater required"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
#إذا ARES_نسخو < 0x011c00
#غلطي "c-ares 1.28.0 or greater required for HTTPSRR"
#جيڪڏهن_پورو
#وصف_ڪريو HTTPSRR_هلي
#جيڪڏهن_پورو

#وصف_ڪريو CARES_TIMEOUT_في_ATTEMPT 2000

ساكن صحيح ares_ver = 0;

ساكن CURLcode async_ares_سيٽ_dns_servers(بنية Curl_easy *معلومات,
                                           بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سيٽ_dns_interface(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سيٽ_dns_local_ip4(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سيٽ_dns_local_ip6(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحيح Curl_async_global_init(فراغ)
{
#جيڪڏهن_وصف_ٿيل CARES_HAVE_ARES_LIBRARY_INIT
  إذا(ares_library_init(ARES_LIB_INIT_ALL)) {
    أرجع CURLE_FAILED_INIT;
  }
#جيڪڏهن_پورو
  ares_نسخو(&ares_ver);
  أرجع CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
فراغ Curl_async_global_cleanup(فراغ)
{
#جيڪڏهن_وصف_ٿيل CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#جيڪڏهن_پورو
}

ساكن فراغ sock_state_cb(فراغ *معلومات, ares_socket_t socket_fd,
                          صحيح readable, صحيح writable)
{
  بنية Curl_easy *easy = معلومات;
  إذا(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ساكن CURLcode async_ares_init(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  صحيح حالت;
  بنية ares_options options;
  صحيح optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode نتيجو = CURLE_OK;

  /* initial status - failed */
  ares->ares_حالت = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_معلومات = معلومات;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  إذا(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_في_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  حالت = ares_init_options(&ares->channel, &options, optmask);
  إذا(حالت != ARES_SUCCESS) {
    ares->channel = عدم;
    نتيجو = (حالت == ARES_ENOMEM) ? CURLE_ٻاهريون_جو_يادگيري : CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  نتيجو = async_ares_سيٽ_dns_servers(معلومات, async);
  إذا(نتيجو && نتيجو != CURLE_NOT_BUILT_اندريون)
    اذهب ٻاهريون;

  نتيجو = async_ares_سيٽ_dns_interface(معلومات, async);
  إذا(نتيجو && نتيجو != CURLE_NOT_BUILT_اندريون)
    اذهب ٻاهريون;

  نتيجو = async_ares_سيٽ_dns_local_ip4(معلومات, async);
  إذا(نتيجو && نتيجو != CURLE_NOT_BUILT_اندريون)
    اذهب ٻاهريون;

  نتيجو = async_ares_سيٽ_dns_local_ip6(معلومات, async);
  إذا(نتيجو && نتيجو != CURLE_NOT_BUILT_اندريون)
    اذهب ٻاهريون;

  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
  أرجع نتيجو;
}

فراغ Curl_async_ares_shutdown(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(معلومات, async);
}

فراغ Curl_async_ares_destroy(بنية Curl_easy *معلومات,
                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  (فراغ)معلومات;
  إذا(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
}

CURLcode Curl_async_pollset(بنية Curl_easy *معلومات,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  بنية async_ares_ctx *ares = &async->ares;
  CURLcode نتيجو = CURLE_OK;

  إذا(ares->channel) {
    نتيجو = Curl_ares_pollset(معلومات, ares->channel, ps);
    إذا(!نتيجو) {
      timediff_t ms = Curl_ares_timeout_ms(معلومات, async, ares->channel);
      Curl_expire(معلومات, ms, EXPIRE_ASYNC_NAME);
    }
  }
  أرجع نتيجو;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_نتيجو(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_داخلا **pdns)
{
  بنية async_ares_ctx *ares = &async->ares;
  بنية Curl_dns_داخلا *dns = عدم;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;
  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  إذا(Curl_ares_perform(ares->channel, 0) < 0) {
    نتيجو = CURLE_UNRECOVERABLE_POLL;
    اذهب ٻاهريون;
  }

  إذا(async->queries_ongoing) {
    نتيجو = CURLE_AGAIN;
    اذهب ٻاهريون;
  }

  /* all c-ares operations done, what is the result to report? */
  نتيجو = ares->نتيجو;
  إذا(ares->ares_حالت == ARES_SUCCESS && !نتيجو) {

    إذا(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(معلومات, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      إذا(!dns) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }

#جيڪڏهن_وصف_ٿيل HTTPSRR_هلي
    إذا(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(معلومات, &async->httpsrr, async->peer);
      إذا(!dns) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }
#جيڪڏهن_پورو
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  إذا(!نتيجو && !dns) {
    ثابت حرف *نياپو = عدم;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_حالت == ARES_ENOTFOUND) ||
       (ares->ares_حالت == ARES_ENODATA));
    إذا(ares->ares_حالت != ARES_SUCCESS)
      نياپو = ares_strerror(ares->ares_حالت);
    نتيجو = Curl_async_failed(معلومات, async, نياپو);
  }

  CURL_TRC_DNS(معلومات, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحيح)نتيجو, (صحيح)ares->نتيجو, ares->ares_حالت,
               dns ? "" : "not ");

ٻاهريون:
  إذا(نتيجو != CURLE_AGAIN)
    ares->نتيجو = نتيجو;
  *pdns = نتيجو ? عدم : dns;
  أرجع نتيجو;
}

ساكن timediff_t async_ares_poll_timeout(بنية async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  بنية timeval *ares_calced, وقت_buf, max_timeout;
  صحيح itimeout_ms;

#إذا TIMEDIFF_T_MAX > وڌ_کان_وڌ_عدد
  itimeout_ms = (timeout_ms > وڌ_کان_وڌ_عدد) ? وڌ_کان_وڌ_عدد :
                 ((timeout_ms < 0) ? -1 : (صحيح)timeout_ms);
#وإلا
  itimeout_ms = (صحيح)timeout_ms;
#جيڪڏهن_پورو
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &وقت_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  إذا(!ares_calced->tv_sec)
    أرجع (timediff_t)(ares_calced->tv_usec / 1000);
  وإلا
    أرجع 1000;
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
CURLcode Curl_async_await(بنية Curl_easy *معلومات, uint32_t resolv_id,
                          بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية curltime شروعات = *Curl_pgrs_now(معلومات);
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم; /* clear on entry */

  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  طالما(!نتيجو) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(معلومات);
    إذا(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &شروعات);
      إذا(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      وإلا
        timeout_ms = -1;
    }

    إذا(timeout_ms < 0) {
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }

    إذا(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      نتيجو = CURLE_UNRECOVERABLE_POLL;
      اكسر;
    }

    نتيجو = Curl_async_take_نتيجو(معلومات, async, pdns);
    إذا(نتيجو == CURLE_AGAIN)
      نتيجو = CURLE_OK;
    وإلا إذا(نتيجو || *pdns)
      اكسر;

    إذا(Curl_pgrsUpdate(معلومات)) {
      نتيجو = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }

  إذا(نتيجو)
    ares_cancel(ares->channel);
  أرجع نتيجو;
}

#وصف_ڪريو CARES_MAX_ADDRS 256

ساكن bool async_ares_addr_ڏٺل(ثابت بنية Curl_addrinfo *cafirst,
                                 صحيح family, size_t ss_سائيز,
                                 ثابت فراغ *addr)
{
  ثابت بنية Curl_addrinfo *ca;
  لكل(ca = cafirst; ca; ca = ca->ai_ايندڙ) {
    إذا((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_سائيز) &&
       !memcmp(ca->ai_addr, addr, ss_سائيز))
      أرجع TRUE;
  }
  أرجع FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ساكن بنية Curl_addrinfo *async_ares_node2addr(
  بنية ares_addrinfo_ڳنڍ *ڳنڍ)
{
  /* traverse the ares_addrinfo_node list */
  بنية ares_addrinfo_ڳنڍ *ai;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  صحيح error = 0;
  بنانشان صحيح naddrs = 0;

  لكل(ai = ڳنڍ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_ايندڙ) {
    size_t ss_سائيز;
    بنية Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    إذا(ai->ai_family == AF_INET)
      ss_سائيز = حجم(بنية sockaddr_in);
#جيڪڏهن_وصف_ٿيل USE_IPV6
    وإلا إذا(ai->ai_family == AF_INET6)
      ss_سائيز = حجم(بنية sockaddr_in6);
#جيڪڏهن_پورو
    وإلا
      استمر;

    /* ignore elements without required address info */
    إذا(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    إذا((size_t)ai->ai_addrlen < ss_سائيز)
      استمر;

    /* ignore duplicate addresses already collected */
    إذا(async_ares_addr_ڏٺل(cafirst, ai->ai_family, ss_سائيز, ai->ai_addr))
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_سائيز);
    إذا(!ca) {
      error = EAI_MEMORY;
      اكسر;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_سائيز;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_ايندڙ      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    يادگيري_نقل(ca->ai_addr, ai->ai_addr, ss_سائيز);

    /* if the return list is empty, this becomes the first element */
    إذا(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    إذا(calast)
      calast->ai_ايندڙ = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  إذا(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = عدم;
  }

  أرجع cafirst;
}

ساكن فراغ async_ares_A_cb(فراغ *user_معلومات, صحيح حالت, صحيح timeouts,
                            بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  إذا(!async)
    أرجع;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->پورو = !async->queries_ongoing;
  إذا(حالت == ARES_SUCCESS) {
    ares->ares_حالت = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  وإلا {
    إذا((حالت != ARES_ENOTFOUND) && (حالت != ARES_ENODATA))
      ares->transient_err = TRUE;
    إذا(ares->ares_حالت != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_حالت = حالت;
  }
}

#جيڪڏهن_وصف_ٿيل CURLRES_IPV6
ساكن فراغ async_ares_AAAA_cb(فراغ *user_معلومات, صحيح حالت, صحيح timeouts,
                               بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  إذا(!async)
    أرجع;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->پورو = !async->queries_ongoing;
  إذا(حالت == ARES_SUCCESS) {
    ares->ares_حالت = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  وإلا {
    إذا((حالت != ARES_ENOTFOUND) && (حالت != ARES_ENODATA))
      ares->transient_err = TRUE;
    إذا(ares->ares_حالت != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_حالت = حالت;
  }
}
#جيڪڏهن_پورو /* CURLRES_IPV6 */

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
ساكن فراغ async_ares_rr_پورو(فراغ *user_معلومات, ares_حالت_t حالت,
                               size_t timeouts,
                               ثابت ares_dns_رڪارڊ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  إذا(!async)
    أرجع;

  (فراغ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->پورو = !async->queries_ongoing;
  ares->ares_حالت = حالت;
  إذا((حالت != ARES_ENOTFOUND) && (حالت != ARES_ENODATA))
    ares->transient_err = TRUE;
  إذا((ARES_SUCCESS != حالت) || !dnsrec)
    أرجع;
  ares->نتيجو = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#جيڪڏهن_پورو /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  حرف service[12];
  صحيح socktype;
  CURLcode نتيجو = CURLE_OK;

  إذا(ares->channel) {
    DEBUGASSERT(0);
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  نتيجو = async_ares_init(معلومات, async);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = Curl_resolv_announce_شروعات(معلومات, ares->channel);
  إذا(نتيجو)
    اذهب ٻاهريون;

#إذا defined(CURLVERBOSE) && ARES_نسخو >= 0x011800 /* >= v1.24.0 */
  إذا(CURL_TRC_DNS_is_verbose(معلومات)) {
    حرف *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(معلومات, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#جيڪڏهن_پورو

  curl_msnprintf(service, حجم(service), "%d", async->peer->پورٽ);
  socktype =
    (Curl_conn_get_transport(معلومات, معلومات->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#جيڪڏهن_وصف_ٿيل CURLRES_IPV6
  إذا(async->dns_queries & CURL_DNSQ_AAAA) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(معلومات, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#جيڪڏهن_پورو /* CURLRES_IPV6 */

  إذا(async->dns_queries & CURL_DNSQ_A) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(معلومات, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *https_name = عدم;
    إذا(async->peer->پورٽ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->پورٽ, async->peer->hostname);
      إذا(!https_name)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    CURL_TRC_DNS(معلومات, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_اندريون, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_پورو, async, عدم);
    curlx_free(https_name);
  }
#جيڪڏهن_پورو /* USE_HTTPSRR */

ٻاهريون:
  ares->نتيجو = نتيجو;
  أرجع نتيجو ? نتيجو : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ساكن CURLcode async_ares_سيٽ_dns_servers(بنية Curl_easy *معلومات,
                                           بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  CURLcode نتيجو = CURLE_NOT_BUILT_اندريون;
  ثابت حرف *servers = CURL_EASY_STR(معلومات, STRING_DNS_SERVERS);
  صحيح ares_نتيجو = ARES_SUCCESS;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#جيڪڏهن_پورو

  إذا(!servers)
    أرجع CURLE_OK;

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_نتيجو = ares_سيٽ_servers_ports_csv(ares->channel, servers);
  بدل(ares_نتيجو) {
  حالة ARES_SUCCESS:
    نتيجو = CURLE_OK;
    اكسر;
  حالة ARES_ENOMEM:
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اكسر;
  حالة ARES_ENOTINITIALIZED:
  حالة ARES_ENODATA:
  حالة ARES_EBADSTR:
  افتراضي:
    DEBUGF(infof(معلومات, "bad servers set"));
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اكسر;
  }
  أرجع نتيجو;
}

ساكن CURLcode async_ares_سيٽ_dns_interface(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  ثابت حرف *interf = CURL_EASY_STR(معلومات, STRING_DNS_INTERFACE);

  إذا(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سيٽ_local_dev(ares->channel, interf);

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سيٽ_dns_local_ip4(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية in_addr a4;
  ثابت حرف *local_ip4 = CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP4);

  إذا(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  وإلا {
    إذا(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(معلومات, "bad DNS IPv4 address"));
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سيٽ_local_ip4(ares->channel, ntohl(a4.s_addr));

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سيٽ_dns_local_ip6(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async)
{
#جيڪڏهن_وصف_ٿيل USE_IPV6
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنانشان حرف a6[INET6_ADDRSTRLEN];
  ثابت حرف *local_ip6 = CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP6);

  إذا(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, حجم(a6));
  }
  وإلا {
    إذا(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(معلومات, "bad DNS IPv6 address"));
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
  }

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سيٽ_local_ip6(ares->channel, a6);

  أرجع CURLE_OK;
#وإلا /* no IPv6 support */
  (فراغ)معلومات;
  (فراغ)async;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

#جيڪڏهن_پورو /* USE_RESOLV_ARES */
