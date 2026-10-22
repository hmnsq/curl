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

#که_تعریف_شوی USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

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

#إذا ARES_نسخه < 0x011000
#تېروتنه "c-ares 1.16.0 or greater required"
#د_که_پای

#که_تعریف_شوی USE_HTTPSRR
#إذا ARES_نسخه < 0x011c00
#تېروتنه "c-ares 1.28.0 or greater required for HTTPSRR"
#د_که_پای
#تعریف_کړه HTTPSRR_کار_کوي
#د_که_پای

#تعریف_کړه CARES_TIMEOUT_فی_ATTEMPT 2000

ساكن صحيح ares_ver = 0;

ساكن CURLcode async_ares_سېټ_dns_servers(بنية Curl_easy *معلومات,
                                           بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سېټ_dns_interface(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سېټ_dns_local_ip4(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سېټ_dns_local_ip6(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحيح Curl_async_global_init(فراغ)
{
#که_تعریف_شوی CARES_HAVE_ARES_LIBRARY_INIT
  إذا(ares_library_init(ARES_LIB_INIT_ALL)) {
    أرجع CURLE_FAILED_INIT;
  }
#د_که_پای
  ares_نسخه(&ares_ver);
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
#که_تعریف_شوی CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#د_که_پای
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
  CURLcode پایله = CURLE_OK;

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
    options.timeout = CARES_TIMEOUT_فی_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  حالت = ares_init_options(&ares->channel, &options, optmask);
  إذا(حالت != ARES_SUCCESS) {
    ares->channel = عدم;
    پایله = (حالت == ARES_ENOMEM) ? CURLE_بهر_د_حافظه : CURLE_FAILED_INIT;
    اذهب بهر;
  }

  پایله = async_ares_سېټ_dns_servers(معلومات, async);
  إذا(پایله && پایله != CURLE_NOT_BUILT_دننه)
    اذهب بهر;

  پایله = async_ares_سېټ_dns_interface(معلومات, async);
  إذا(پایله && پایله != CURLE_NOT_BUILT_دننه)
    اذهب بهر;

  پایله = async_ares_سېټ_dns_local_ip4(معلومات, async);
  إذا(پایله && پایله != CURLE_NOT_BUILT_دننه)
    اذهب بهر;

  پایله = async_ares_سېټ_dns_local_ip6(معلومات, async);
  إذا(پایله && پایله != CURLE_NOT_BUILT_دننه)
    اذهب بهر;

  پایله = CURLE_OK;

بهر:
  إذا(پایله && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
  أرجع پایله;
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
  CURLcode پایله = CURLE_OK;

  إذا(ares->channel) {
    پایله = Curl_ares_pollset(معلومات, ares->channel, ps);
    إذا(!پایله) {
      timediff_t ms = Curl_ares_timeout_ms(معلومات, async, ares->channel);
      Curl_expire(معلومات, ms, EXPIRE_ASYNC_NAME);
    }
  }
  أرجع پایله;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_پایله(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_ثبت **pdns)
{
  بنية async_ares_ctx *ares = &async->ares;
  بنية Curl_dns_ثبت *dns = عدم;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;
  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  إذا(Curl_ares_perform(ares->channel, 0) < 0) {
    پایله = CURLE_UNRECOVERABLE_POLL;
    اذهب بهر;
  }

  إذا(async->queries_ongoing) {
    پایله = CURLE_AGAIN;
    اذهب بهر;
  }

  /* all c-ares operations done, what is the result to report? */
  پایله = ares->پایله;
  إذا(ares->ares_حالت == ARES_SUCCESS && !پایله) {

    إذا(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(معلومات, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      إذا(!dns) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
    }

#که_تعریف_شوی HTTPSRR_کار_کوي
    إذا(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(معلومات, &async->httpsrr, async->peer);
      إذا(!dns) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
    }
#د_که_پای
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  إذا(!پایله && !dns) {
    ثابت حرف *پیغام = عدم;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_حالت == ARES_ENOTFOUND) ||
       (ares->ares_حالت == ARES_ENODATA));
    إذا(ares->ares_حالت != ARES_SUCCESS)
      پیغام = ares_strerror(ares->ares_حالت);
    پایله = Curl_async_failed(معلومات, async, پیغام);
  }

  CURL_TRC_DNS(معلومات, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحيح)پایله, (صحيح)ares->پایله, ares->ares_حالت,
               dns ? "" : "not ");

بهر:
  إذا(پایله != CURLE_AGAIN)
    ares->پایله = پایله;
  *pdns = پایله ? عدم : dns;
  أرجع پایله;
}

ساكن timediff_t async_ares_poll_timeout(بنية async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  بنية timeval *ares_calced, وخت_buf, max_timeout;
  صحيح itimeout_ms;

#إذا TIMEDIFF_T_MAX > لوی_عدد
  itimeout_ms = (timeout_ms > لوی_عدد) ? لوی_عدد :
                 ((timeout_ms < 0) ? -1 : (صحيح)timeout_ms);
#وإلا
  itimeout_ms = (صحيح)timeout_ms;
#د_که_پای
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &وخت_buf);
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
                          بنية Curl_dns_ثبت **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية curltime پیل = *Curl_pgrs_now(معلومات);
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم; /* clear on entry */

  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  طالما(!پایله) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(معلومات);
    إذا(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &پیل);
      إذا(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      وإلا
        timeout_ms = -1;
    }

    إذا(timeout_ms < 0) {
      پایله = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }

    إذا(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      پایله = CURLE_UNRECOVERABLE_POLL;
      اكسر;
    }

    پایله = Curl_async_take_پایله(معلومات, async, pdns);
    إذا(پایله == CURLE_AGAIN)
      پایله = CURLE_OK;
    وإلا إذا(پایله || *pdns)
      اكسر;

    إذا(Curl_pgrsUpdate(معلومات)) {
      پایله = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }

  إذا(پایله)
    ares_cancel(ares->channel);
  أرجع پایله;
}

#تعریف_کړه CARES_MAX_ADDRS 256

ساكن bool async_ares_addr_کتل_شوی(ثابت بنية Curl_addrinfo *cafirst,
                                 صحيح family, size_t ss_اندازه,
                                 ثابت فراغ *addr)
{
  ثابت بنية Curl_addrinfo *ca;
  لكل(ca = cafirst; ca; ca = ca->ai_بل) {
    إذا((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_اندازه) &&
       !memcmp(ca->ai_addr, addr, ss_اندازه))
      أرجع TRUE;
  }
  أرجع FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ساكن بنية Curl_addrinfo *async_ares_node2addr(
  بنية ares_addrinfo_غوټه *غوټه)
{
  /* traverse the ares_addrinfo_node list */
  بنية ares_addrinfo_غوټه *ai;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  صحيح error = 0;
  بینښه صحيح naddrs = 0;

  لكل(ai = غوټه; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_بل) {
    size_t ss_اندازه;
    بنية Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    إذا(ai->ai_family == AF_INET)
      ss_اندازه = حجم(بنية sockaddr_in);
#که_تعریف_شوی USE_IPV6
    وإلا إذا(ai->ai_family == AF_INET6)
      ss_اندازه = حجم(بنية sockaddr_in6);
#د_که_پای
    وإلا
      استمر;

    /* ignore elements without required address info */
    إذا(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    إذا((size_t)ai->ai_addrlen < ss_اندازه)
      استمر;

    /* ignore duplicate addresses already collected */
    إذا(async_ares_addr_کتل_شوی(cafirst, ai->ai_family, ss_اندازه, ai->ai_addr))
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_اندازه);
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
    ca->ai_addrlen   = (curl_socklen_t)ss_اندازه;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_بل      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    حافظه_نقل(ca->ai_addr, ai->ai_addr, ss_اندازه);

    /* if the return list is empty, this becomes the first element */
    إذا(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    إذا(calast)
      calast->ai_بل = ca;
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
  async->بشپړ = !async->queries_ongoing;
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

#که_تعریف_شوی CURLRES_IPV6
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
  async->بشپړ = !async->queries_ongoing;
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
#د_که_پای /* CURLRES_IPV6 */

#که_تعریف_شوی USE_HTTPSRR
ساكن فراغ async_ares_rr_بشپړ(فراغ *user_معلومات, ares_حالت_t حالت,
                               size_t timeouts,
                               ثابت ares_dns_ریکارډ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  إذا(!async)
    أرجع;

  (فراغ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->بشپړ = !async->queries_ongoing;
  ares->ares_حالت = حالت;
  إذا((حالت != ARES_ENOTFOUND) && (حالت != ARES_ENODATA))
    ares->transient_err = TRUE;
  إذا((ARES_SUCCESS != حالت) || !dnsrec)
    أرجع;
  ares->پایله = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#د_که_پای /* USE_HTTPSRR */

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
  CURLcode پایله = CURLE_OK;

  إذا(ares->channel) {
    DEBUGASSERT(0);
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  پایله = async_ares_init(معلومات, async);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_resolv_announce_پیل(معلومات, ares->channel);
  إذا(پایله)
    اذهب بهر;

#إذا defined(CURLVERBOSE) && ARES_نسخه >= 0x011800 /* >= v1.24.0 */
  إذا(CURL_TRC_DNS_is_verbose(معلومات)) {
    حرف *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(معلومات, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#د_که_پای

  curl_msnprintf(service, حجم(service), "%d", async->peer->پورټ);
  socktype =
    (Curl_conn_get_transport(معلومات, معلومات->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#که_تعریف_شوی CURLRES_IPV6
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
#د_که_پای /* CURLRES_IPV6 */

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

#که_تعریف_شوی USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *https_name = عدم;
    إذا(async->peer->پورټ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->پورټ, async->peer->hostname);
      إذا(!https_name)
        أرجع CURLE_بهر_د_حافظه;
    }
    CURL_TRC_DNS(معلومات, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_دننه, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_بشپړ, async, عدم);
    curlx_free(https_name);
  }
#د_که_پای /* USE_HTTPSRR */

بهر:
  ares->پایله = پایله;
  أرجع پایله ? پایله : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ساكن CURLcode async_ares_سېټ_dns_servers(بنية Curl_easy *معلومات,
                                           بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  CURLcode پایله = CURLE_NOT_BUILT_دننه;
  ثابت حرف *servers = CURL_EASY_STR(معلومات, STRING_DNS_SERVERS);
  صحيح ares_پایله = ARES_SUCCESS;

#که_تعریف_شوی DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#د_که_پای

  إذا(!servers)
    أرجع CURLE_OK;

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_پایله = ares_سېټ_servers_ports_csv(ares->channel, servers);
  بدل(ares_پایله) {
  حالة ARES_SUCCESS:
    پایله = CURLE_OK;
    اكسر;
  حالة ARES_ENOMEM:
    پایله = CURLE_بهر_د_حافظه;
    اكسر;
  حالة ARES_ENOTINITIALIZED:
  حالة ARES_ENODATA:
  حالة ARES_EBADSTR:
  افتراضي:
    DEBUGF(infof(معلومات, "bad servers set"));
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode async_ares_سېټ_dns_interface(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  ثابت حرف *interf = CURL_EASY_STR(معلومات, STRING_DNS_INTERFACE);

  إذا(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سېټ_local_dev(ares->channel, interf);

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سېټ_dns_local_ip4(بنية Curl_easy *معلومات,
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
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سېټ_local_ip4(ares->channel, ntohl(a4.s_addr));

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سېټ_dns_local_ip6(بنية Curl_easy *معلومات,
                                             بنية Curl_resolv_async *async)
{
#که_تعریف_شوی USE_IPV6
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بینښه حرف a6[INET6_ADDRSTRLEN];
  ثابت حرف *local_ip6 = CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP6);

  إذا(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, حجم(a6));
  }
  وإلا {
    إذا(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(معلومات, "bad DNS IPv6 address"));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سېټ_local_ip6(ares->channel, a6);

  أرجع CURLE_OK;
#وإلا /* no IPv6 support */
  (فراغ)معلومات;
  (فراغ)async;
  أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
}

#د_که_پای /* USE_RESOLV_ARES */
