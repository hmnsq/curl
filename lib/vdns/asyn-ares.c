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

#جے_تعریف_شدہ USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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

#إذا ARES_نسخہ < 0x011000
#غلطی "c-ares 1.16.0 or greater required"
#جے_انت

#جے_تعریف_شدہ USE_HTTPSRR
#إذا ARES_نسخہ < 0x011c00
#غلطی "c-ares 1.28.0 or greater required for HTTPSRR"
#جے_انت
#تعریف_کرو HTTPSRR_چلدا
#جے_انت

#تعریف_کرو CARES_TIMEOUT_فی_ATTEMPT 2000

ساكن صحيح ares_ver = 0;

ساكن CURLcode async_ares_سیٹ_dns_servers(بنية Curl_easy *انکڑے,
                                           بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سیٹ_dns_interface(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سیٹ_dns_local_ip4(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async);
ساكن CURLcode async_ares_سیٹ_dns_local_ip6(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحيح Curl_async_global_init(فراغ)
{
#جے_تعریف_شدہ CARES_HAVE_ARES_LIBRARY_INIT
  إذا(ares_library_init(ARES_LIB_INIT_ALL)) {
    أرجع CURLE_FAILED_INIT;
  }
#جے_انت
  ares_نسخہ(&ares_ver);
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
#جے_تعریف_شدہ CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#جے_انت
}

ساكن فراغ sock_state_cb(فراغ *انکڑے, ares_socket_t socket_fd,
                          صحيح readable, صحيح writable)
{
  بنية Curl_easy *easy = انکڑے;
  إذا(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ساكن CURLcode async_ares_init(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  صحيح حالت;
  بنية ares_options options;
  صحيح optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode نتیجہ = CURLE_OK;

  /* initial status - failed */
  ares->ares_حالت = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_انکڑے = انکڑے;

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
    نتیجہ = (حالت == ARES_ENOMEM) ? CURLE_باہرلا_دا_حافظہ : CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  نتیجہ = async_ares_سیٹ_dns_servers(انکڑے, async);
  إذا(نتیجہ && نتیجہ != CURLE_NOT_BUILT_وچ)
    اذهب باہرلا;

  نتیجہ = async_ares_سیٹ_dns_interface(انکڑے, async);
  إذا(نتیجہ && نتیجہ != CURLE_NOT_BUILT_وچ)
    اذهب باہرلا;

  نتیجہ = async_ares_سیٹ_dns_local_ip4(انکڑے, async);
  إذا(نتیجہ && نتیجہ != CURLE_NOT_BUILT_وچ)
    اذهب باہرلا;

  نتیجہ = async_ares_سیٹ_dns_local_ip6(انکڑے, async);
  إذا(نتیجہ && نتیجہ != CURLE_NOT_BUILT_وچ)
    اذهب باہرلا;

  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
  أرجع نتیجہ;
}

فراغ Curl_async_ares_shutdown(بنية Curl_easy *انکڑے,
                              بنية Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(انکڑے, async);
}

فراغ Curl_async_ares_destroy(بنية Curl_easy *انکڑے,
                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  (فراغ)انکڑے;
  إذا(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = عدم;
  }
}

CURLcode Curl_async_pollset(بنية Curl_easy *انکڑے,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  بنية async_ares_ctx *ares = &async->ares;
  CURLcode نتیجہ = CURLE_OK;

  إذا(ares->channel) {
    نتیجہ = Curl_ares_pollset(انکڑے, ares->channel, ps);
    إذا(!نتیجہ) {
      timediff_t ms = Curl_ares_timeout_ms(انکڑے, async, ares->channel);
      Curl_expire(انکڑے, ms, EXPIRE_ASYNC_NAME);
    }
  }
  أرجع نتیجہ;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_نتیجہ(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_اندراج **pdns)
{
  بنية async_ares_ctx *ares = &async->ares;
  بنية Curl_dns_اندراج *dns = عدم;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;
  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  إذا(Curl_ares_perform(ares->channel, 0) < 0) {
    نتیجہ = CURLE_UNRECOVERABLE_POLL;
    اذهب باہرلا;
  }

  إذا(async->queries_ongoing) {
    نتیجہ = CURLE_AGAIN;
    اذهب باہرلا;
  }

  /* all c-ares operations done, what is the result to report? */
  نتیجہ = ares->نتیجہ;
  إذا(ares->ares_حالت == ARES_SUCCESS && !نتیجہ) {

    إذا(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(انکڑے, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      إذا(!dns) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }

#جے_تعریف_شدہ HTTPSRR_چلدا
    إذا(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(انکڑے, &async->httpsrr, async->peer);
      إذا(!dns) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }
#جے_انت
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  إذا(!نتیجہ && !dns) {
    ثابت حرف *پیغام = عدم;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_حالت == ARES_ENOTFOUND) ||
       (ares->ares_حالت == ARES_ENODATA));
    إذا(ares->ares_حالت != ARES_SUCCESS)
      پیغام = ares_strerror(ares->ares_حالت);
    نتیجہ = Curl_async_failed(انکڑے, async, پیغام);
  }

  CURL_TRC_DNS(انکڑے, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحيح)نتیجہ, (صحيح)ares->نتیجہ, ares->ares_حالت,
               dns ? "" : "not ");

باہرلا:
  إذا(نتیجہ != CURLE_AGAIN)
    ares->نتیجہ = نتیجہ;
  *pdns = نتیجہ ? عدم : dns;
  أرجع نتیجہ;
}

ساكن timediff_t async_ares_poll_timeout(بنية async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  بنية timeval *ares_calced, ویلا_buf, max_timeout;
  صحيح itimeout_ms;

#إذا TIMEDIFF_T_MAX > ودھ_توں_ودھ_عدد
  itimeout_ms = (timeout_ms > ودھ_توں_ودھ_عدد) ? ودھ_توں_ودھ_عدد :
                 ((timeout_ms < 0) ? -1 : (صحيح)timeout_ms);
#وإلا
  itimeout_ms = (صحيح)timeout_ms;
#جے_انت
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &ویلا_buf);
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
CURLcode Curl_async_await(بنية Curl_easy *انکڑے, uint32_t resolv_id,
                          بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية curltime شروع = *Curl_pgrs_now(انکڑے);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم; /* clear on entry */

  إذا(!ares)
    أرجع CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  طالما(!نتیجہ) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(انکڑے);
    إذا(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &شروع);
      إذا(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      وإلا
        timeout_ms = -1;
    }

    إذا(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }

    إذا(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      نتیجہ = CURLE_UNRECOVERABLE_POLL;
      اكسر;
    }

    نتیجہ = Curl_async_take_نتیجہ(انکڑے, async, pdns);
    إذا(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    وإلا إذا(نتیجہ || *pdns)
      اكسر;

    إذا(Curl_pgrsUpdate(انکڑے)) {
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }

  إذا(نتیجہ)
    ares_cancel(ares->channel);
  أرجع نتیجہ;
}

#تعریف_کرو CARES_MAX_ADDRS 256

ساكن bool async_ares_addr_تکیا(ثابت بنية Curl_addrinfo *cafirst,
                                 صحيح family, size_t ss_ناپ,
                                 ثابت فراغ *addr)
{
  ثابت بنية Curl_addrinfo *ca;
  لكل(ca = cafirst; ca; ca = ca->ai_اگلا) {
    إذا((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_ناپ) &&
       !memcmp(ca->ai_addr, addr, ss_ناپ))
      أرجع TRUE;
  }
  أرجع FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ساكن بنية Curl_addrinfo *async_ares_node2addr(
  بنية ares_addrinfo_گرہ *گرہ)
{
  /* traverse the ares_addrinfo_node list */
  بنية ares_addrinfo_گرہ *ai;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  صحيح error = 0;
  بنانشانی صحيح naddrs = 0;

  لكل(ai = گرہ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_اگلا) {
    size_t ss_ناپ;
    بنية Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    إذا(ai->ai_family == AF_INET)
      ss_ناپ = حجم(بنية sockaddr_in);
#جے_تعریف_شدہ USE_IPV6
    وإلا إذا(ai->ai_family == AF_INET6)
      ss_ناپ = حجم(بنية sockaddr_in6);
#جے_انت
    وإلا
      استمر;

    /* ignore elements without required address info */
    إذا(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    إذا((size_t)ai->ai_addrlen < ss_ناپ)
      استمر;

    /* ignore duplicate addresses already collected */
    إذا(async_ares_addr_تکیا(cafirst, ai->ai_family, ss_ناپ, ai->ai_addr))
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_ناپ);
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
    ca->ai_addrlen   = (curl_socklen_t)ss_ناپ;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_اگلا      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    حافظہ_نقل(ca->ai_addr, ai->ai_addr, ss_ناپ);

    /* if the return list is empty, this becomes the first element */
    إذا(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    إذا(calast)
      calast->ai_اگلا = ca;
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

ساكن فراغ async_ares_A_cb(فراغ *user_انکڑے, صحيح حالت, صحيح timeouts,
                            بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_انکڑے;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  إذا(!async)
    أرجع;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->پورا = !async->queries_ongoing;
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

#جے_تعریف_شدہ CURLRES_IPV6
ساكن فراغ async_ares_AAAA_cb(فراغ *user_انکڑے, صحيح حالت, صحيح timeouts,
                               بنية ares_addrinfo *ares_ai)
{
  بنية Curl_resolv_async *async = user_انکڑے;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  (فراغ)timeouts;
  إذا(!async)
    أرجع;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->پورا = !async->queries_ongoing;
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
#جے_انت /* CURLRES_IPV6 */

#جے_تعریف_شدہ USE_HTTPSRR
ساكن فراغ async_ares_rr_پورا(فراغ *user_انکڑے, ares_حالت_t حالت,
                               size_t timeouts,
                               ثابت ares_dns_ریکارڈ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_انکڑے;
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;

  إذا(!async)
    أرجع;

  (فراغ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->پورا = !async->queries_ongoing;
  ares->ares_حالت = حالت;
  إذا((حالت != ARES_ENOTFOUND) && (حالت != ARES_ENODATA))
    ares->transient_err = TRUE;
  إذا((ARES_SUCCESS != حالت) || !dnsrec)
    أرجع;
  ares->نتیجہ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#جے_انت /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = &async->ares;
  حرف service[12];
  صحيح socktype;
  CURLcode نتیجہ = CURLE_OK;

  إذا(ares->channel) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  نتیجہ = async_ares_init(انکڑے, async);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_resolv_announce_شروع(انکڑے, ares->channel);
  إذا(نتیجہ)
    اذهب باہرلا;

#إذا defined(CURLVERBOSE) && ARES_نسخہ >= 0x011800 /* >= v1.24.0 */
  إذا(CURL_TRC_DNS_is_verbose(انکڑے)) {
    حرف *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(انکڑے, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#جے_انت

  curl_msnprintf(service, حجم(service), "%d", async->peer->پورٹ);
  socktype =
    (Curl_conn_get_transport(انکڑے, انکڑے->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#جے_تعریف_شدہ CURLRES_IPV6
  إذا(async->dns_queries & CURL_DNSQ_AAAA) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(انکڑے, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#جے_انت /* CURLRES_IPV6 */

  إذا(async->dns_queries & CURL_DNSQ_A) {
    بنية ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(انکڑے, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#جے_تعریف_شدہ USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *https_name = عدم;
    إذا(async->peer->پورٹ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->پورٹ, async->peer->hostname);
      إذا(!https_name)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
    CURL_TRC_DNS(انکڑے, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_وچ, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_پورا, async, عدم);
    curlx_free(https_name);
  }
#جے_انت /* USE_HTTPSRR */

باہرلا:
  ares->نتیجہ = نتیجہ;
  أرجع نتیجہ ? نتیجہ : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ساكن CURLcode async_ares_سیٹ_dns_servers(بنية Curl_easy *انکڑے,
                                           بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  CURLcode نتیجہ = CURLE_NOT_BUILT_وچ;
  ثابت حرف *servers = CURL_EASY_STR(انکڑے, STRING_DNS_SERVERS);
  صحيح ares_نتیجہ = ARES_SUCCESS;

#جے_تعریف_شدہ DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#جے_انت

  إذا(!servers)
    أرجع CURLE_OK;

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_نتیجہ = ares_سیٹ_servers_ports_csv(ares->channel, servers);
  بدل(ares_نتیجہ) {
  حالة ARES_SUCCESS:
    نتیجہ = CURLE_OK;
    اكسر;
  حالة ARES_ENOMEM:
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اكسر;
  حالة ARES_ENOTINITIALIZED:
  حالة ARES_ENODATA:
  حالة ARES_EBADSTR:
  افتراضي:
    DEBUGF(infof(انکڑے, "bad servers set"));
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode async_ares_سیٹ_dns_interface(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  ثابت حرف *interf = CURL_EASY_STR(انکڑے, STRING_DNS_INTERFACE);

  إذا(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سیٹ_local_dev(ares->channel, interf);

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سیٹ_dns_local_ip4(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async)
{
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنية in_addr a4;
  ثابت حرف *local_ip4 = CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP4);

  إذا(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  وإلا {
    إذا(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(انکڑے, "bad DNS IPv4 address"));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سیٹ_local_ip4(ares->channel, ntohl(a4.s_addr));

  أرجع CURLE_OK;
}

ساكن CURLcode async_ares_سیٹ_dns_local_ip6(بنية Curl_easy *انکڑے,
                                             بنية Curl_resolv_async *async)
{
#جے_تعریف_شدہ USE_IPV6
  بنية async_ares_ctx *ares = async ? &async->ares : عدم;
  بنانشانی حرف a6[INET6_ADDRSTRLEN];
  ثابت حرف *local_ip6 = CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP6);

  إذا(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, حجم(a6));
  }
  وإلا {
    إذا(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(انکڑے, "bad DNS IPv6 address"));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there, this is a parameter check */
  إذا(ares && ares->channel)
    ares_سیٹ_local_ip6(ares->channel, a6);

  أرجع CURLE_OK;
#وإلا /* no IPv6 support */
  (فراغ)انکڑے;
  (فراغ)async;
  أرجع CURLE_NOT_BUILT_وچ;
#جے_انت
}

#جے_انت /* USE_RESOLV_ARES */
