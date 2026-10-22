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

#اگر_تعریف_شدہ USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

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

#اگر ARES_نسخہ < 0x011000
#غلطی "c-ares 1.16.0 or greater required"
#اگر_ختم

#اگر_تعریف_شدہ USE_HTTPSRR
#اگر ARES_نسخہ < 0x011c00
#غلطی "c-ares 1.28.0 or greater required for HTTPSRR"
#اگر_ختم
#تعریف_کرو HTTPSRR_چلتا
#اگر_ختم

#تعریف_کرو CARES_TIMEOUT_فی_ATTEMPT 2000

ساکن صحیح ares_ver = 0;

ساکن CURLcode async_ares_سیٹ_dns_servers(ساخت Curl_easy *ڈیٹا,
                                           ساخت Curl_resolv_async *async);
ساکن CURLcode async_ares_سیٹ_dns_interface(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async);
ساکن CURLcode async_ares_سیٹ_dns_local_ip4(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async);
ساکن CURLcode async_ares_سیٹ_dns_local_ip6(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
صحیح Curl_async_global_init(خالی)
{
#اگر_تعریف_شدہ CARES_HAVE_ARES_LIBRARY_INIT
  اگر(ares_library_init(ARES_LIB_INIT_ALL)) {
    واپس CURLE_FAILED_INIT;
  }
#اگر_ختم
  ares_نسخہ(&ares_ver);
  واپس CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
خالی Curl_async_global_cleanup(خالی)
{
#اگر_تعریف_شدہ CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#اگر_ختم
}

ساکن خالی sock_state_cb(خالی *ڈیٹا, ares_socket_t socket_fd,
                          صحیح readable, صحیح writable)
{
  ساخت Curl_easy *easy = ڈیٹا;
  اگر(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ساکن CURLcode async_ares_init(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = &async->ares;
  صحیح حالت_2;
  ساخت ares_options options;
  صحیح optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode نتیجہ = CURLE_OK;

  /* initial status - failed */
  ares->ares_حالت = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_ڈیٹا = ڈیٹا;

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
    options.timeout = CARES_TIMEOUT_فی_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  حالت_2 = ares_init_options(&ares->channel, &options, optmask);
  اگر(حالت_2 != ARES_SUCCESS) {
    ares->channel = تہی;
    نتیجہ = (حالت_2 == ARES_ENOMEM) ? CURLE_برآمد_کا_حافظہ : CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  نتیجہ = async_ares_سیٹ_dns_servers(ڈیٹا, async);
  اگر(نتیجہ && نتیجہ != CURLE_NOT_BUILT_اندر)
    جاؤ برآمد;

  نتیجہ = async_ares_سیٹ_dns_interface(ڈیٹا, async);
  اگر(نتیجہ && نتیجہ != CURLE_NOT_BUILT_اندر)
    جاؤ برآمد;

  نتیجہ = async_ares_سیٹ_dns_local_ip4(ڈیٹا, async);
  اگر(نتیجہ && نتیجہ != CURLE_NOT_BUILT_اندر)
    جاؤ برآمد;

  نتیجہ = async_ares_سیٹ_dns_local_ip6(ڈیٹا, async);
  اگر(نتیجہ && نتیجہ != CURLE_NOT_BUILT_اندر)
    جاؤ برآمد;

  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = تہی;
  }
  واپس نتیجہ;
}

خالی Curl_async_ares_shutdown(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(ڈیٹا, async);
}

خالی Curl_async_ares_destroy(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = &async->ares;
  (خالی)ڈیٹا;
  اگر(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = تہی;
  }
}

CURLcode Curl_async_pollset(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_resolv_async *async,
                            ساخت easy_pollset *ps)
{
  ساخت async_ares_ctx *ares = &async->ares;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ares->channel) {
    نتیجہ = Curl_ares_pollset(ڈیٹا, ares->channel, ps);
    اگر(!نتیجہ) {
      timediff_t ms = Curl_ares_timeout_ms(ڈیٹا, async, ares->channel);
      Curl_expire(ڈیٹا, ms, EXPIRE_ASYNC_NAME);
    }
  }
  واپس نتیجہ;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async,
                                ساخت Curl_dns_اندراج **pdns)
{
  ساخت async_ares_ctx *ares = &async->ares;
  ساخت Curl_dns_اندراج *dns = تہی;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = تہی;
  اگر(!ares)
    واپس CURLE_FAILED_INIT;

  اگر(Curl_ares_perform(ares->channel, 0) < 0) {
    نتیجہ = CURLE_UNRECOVERABLE_POLL;
    جاؤ برآمد;
  }

  اگر(async->queries_ongoing) {
    نتیجہ = CURLE_AGAIN;
    جاؤ برآمد;
  }

  /* all c-ares operations done, what is the result to report? */
  نتیجہ = ares->نتیجہ;
  اگر(ares->ares_حالت == ARES_SUCCESS && !نتیجہ) {

    اگر(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(ڈیٹا, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      اگر(!dns) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }

#اگر_تعریف_شدہ HTTPSRR_چلتا
    اگر(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(ڈیٹا, &async->httpsrr, async->peer);
      اگر(!dns) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
#اگر_ختم
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  اگر(!نتیجہ && !dns) {
    ثابت حرف *پیغام = تہی;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_حالت == ARES_ENOTFOUND) ||
       (ares->ares_حالت == ARES_ENODATA));
    اگر(ares->ares_حالت != ARES_SUCCESS)
      پیغام = ares_strerror(ares->ares_حالت);
    نتیجہ = Curl_async_failed(ڈیٹا, async, پیغام);
  }

  CURL_TRC_DNS(ڈیٹا, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (صحیح)نتیجہ, (صحیح)ares->نتیجہ, ares->ares_حالت,
               dns ? "" : "not ");

برآمد:
  اگر(نتیجہ != CURLE_AGAIN)
    ares->نتیجہ = نتیجہ;
  *pdns = نتیجہ ? تہی : dns;
  واپس نتیجہ;
}

ساکن timediff_t async_ares_poll_timeout(ساخت async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ساخت timeval *ares_calced, وقت_buf, max_timeout;
  صحیح itimeout_ms;

#اگر TIMEDIFF_T_MAX > زیادہ_سے_زیادہ_عدد
  itimeout_ms = (timeout_ms > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد :
                 ((timeout_ms < 0) ? -1 : (صحیح)timeout_ms);
#ورنہ
  itimeout_ms = (صحیح)timeout_ms;
#اگر_ختم
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &وقت_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  اگر(!ares_calced->tv_sec)
    واپس (timediff_t)(ares_calced->tv_usec / 1000);
  ورنہ
    واپس 1000;
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
CURLcode Curl_async_await(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id,
                          ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;
  ساخت curltime آغاز = *Curl_pgrs_now(ڈیٹا);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = تہی; /* clear on entry */

  اگر(!ares)
    واپس CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  جب(!نتیجہ) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(ڈیٹا);
    اگر(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &آغاز);
      اگر(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      ورنہ
        timeout_ms = -1;
    }

    اگر(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    }

    اگر(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      نتیجہ = CURLE_UNRECOVERABLE_POLL;
      توڑو;
    }

    نتیجہ = Curl_async_take_نتیجہ(ڈیٹا, async, pdns);
    اگر(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    ورنہ اگر(نتیجہ || *pdns)
      توڑو;

    اگر(Curl_pgrsUpdate(ڈیٹا)) {
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      توڑو;
    }
  }

  اگر(نتیجہ)
    ares_cancel(ares->channel);
  واپس نتیجہ;
}

#تعریف_کرو CARES_MAX_ADDRS 256

ساکن bool async_ares_addr_نظر_آیا(ثابت ساخت Curl_addrinfo *cafirst,
                                 صحیح family, size_t ss_سائز,
                                 ثابت خالی *addr)
{
  ثابت ساخت Curl_addrinfo *ca;
  برائے(ca = cafirst; ca; ca = ca->ai_اگلا) {
    اگر((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_سائز) &&
       !memcmp(ca->ai_addr, addr, ss_سائز))
      واپس TRUE;
  }
  واپس FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ساکن ساخت Curl_addrinfo *async_ares_node2addr(
  ساخت ares_addrinfo_گرہ *گرہ)
{
  /* traverse the ares_addrinfo_node list */
  ساخت ares_addrinfo_گرہ *ai;
  ساخت Curl_addrinfo *cafirst = تہی;
  ساخت Curl_addrinfo *calast = تہی;
  صحیح error = 0;
  غیرعلامتی صحیح naddrs = 0;

  برائے(ai = گرہ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_اگلا) {
    size_t ss_سائز;
    ساخت Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_سائز = حجم(ساخت sockaddr_in);
#اگر_تعریف_شدہ USE_IPV6
    ورنہ اگر(ai->ai_family == AF_INET6)
      ss_سائز = حجم(ساخت sockaddr_in6);
#اگر_ختم
    ورنہ
      جاری;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      جاری;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_سائز)
      جاری;

    /* ignore duplicate addresses already collected */
    اگر(async_ares_addr_نظر_آیا(cafirst, ai->ai_family, ss_سائز, ai->ai_addr))
      جاری;

    ca = curlx_malloc(حجم(ساخت Curl_addrinfo) + ss_سائز);
    اگر(!ca) {
      error = EAI_MEMORY;
      توڑو;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_سائز;
    ca->ai_addr      = تہی;
    ca->ai_canonname = تہی;
    ca->ai_اگلا      = تہی;

    ca->ai_addr = (خالی *)((حرف *)ca + حجم(ساخت Curl_addrinfo));
    حافظہ_نقل(ca->ai_addr, ai->ai_addr, ss_سائز);

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_اگلا = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  اگر(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = تہی;
  }

  واپس cafirst;
}

ساکن خالی async_ares_A_cb(خالی *user_ڈیٹا, صحیح حالت_2, صحیح timeouts,
                            ساخت ares_addrinfo *ares_ai)
{
  ساخت Curl_resolv_async *async = user_ڈیٹا;
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;

  (خالی)timeouts;
  اگر(!async)
    واپس;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->مکمل = !async->queries_ongoing;
  اگر(حالت_2 == ARES_SUCCESS) {
    ares->ares_حالت = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ورنہ {
    اگر((حالت_2 != ARES_ENOTFOUND) && (حالت_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    اگر(ares->ares_حالت != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_حالت = حالت_2;
  }
}

#اگر_تعریف_شدہ CURLRES_IPV6
ساکن خالی async_ares_AAAA_cb(خالی *user_ڈیٹا, صحیح حالت_2, صحیح timeouts,
                               ساخت ares_addrinfo *ares_ai)
{
  ساخت Curl_resolv_async *async = user_ڈیٹا;
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;

  (خالی)timeouts;
  اگر(!async)
    واپس;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->مکمل = !async->queries_ongoing;
  اگر(حالت_2 == ARES_SUCCESS) {
    ares->ares_حالت = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ورنہ {
    اگر((حالت_2 != ARES_ENOTFOUND) && (حالت_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    اگر(ares->ares_حالت != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_حالت = حالت_2;
  }
}
#اگر_ختم /* CURLRES_IPV6 */

#اگر_تعریف_شدہ USE_HTTPSRR
ساکن خالی async_ares_rr_مکمل(خالی *user_ڈیٹا, ares_حالت_t حالت_2,
                               size_t timeouts,
                               ثابت ares_dns_ریکارڈ_t *dnsrec)
{
  ساخت Curl_resolv_async *async = user_ڈیٹا;
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;

  اگر(!async)
    واپس;

  (خالی)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->مکمل = !async->queries_ongoing;
  ares->ares_حالت = حالت_2;
  اگر((حالت_2 != ARES_ENOTFOUND) && (حالت_2 != ARES_ENODATA))
    ares->transient_err = TRUE;
  اگر((ARES_SUCCESS != حالت_2) || !dnsrec)
    واپس;
  ares->نتیجہ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#اگر_ختم /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = &async->ares;
  حرف service[12];
  صحیح socktype;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ares->channel) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  نتیجہ = async_ares_init(ڈیٹا, async);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, ares->channel);
  اگر(نتیجہ)
    جاؤ برآمد;

#اگر defined(CURLVERBOSE) && ARES_نسخہ >= 0x011800 /* >= v1.24.0 */
  اگر(CURL_TRC_DNS_is_verbose(ڈیٹا)) {
    حرف *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(ڈیٹا, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#اگر_ختم

  curl_msnprintf(service, حجم(service), "%d", async->peer->پورٹ);
  socktype =
    (Curl_conn_get_transport(ڈیٹا, ڈیٹا->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#اگر_تعریف_شدہ CURLRES_IPV6
  اگر(async->dns_queries & CURL_DNSQ_AAAA) {
    ساخت ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(ڈیٹا, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#اگر_ختم /* CURLRES_IPV6 */

  اگر(async->dns_queries & CURL_DNSQ_A) {
    ساخت ares_addrinfo_hints hints;

    memset(&hints, 0, حجم(hints));
    CURL_TRC_DNS(ڈیٹا, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#اگر_تعریف_شدہ USE_HTTPSRR
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *https_name = تہی;
    اگر(async->peer->پورٹ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->پورٹ, async->peer->hostname);
      اگر(!https_name)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    CURL_TRC_DNS(ڈیٹا, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_اندر, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_مکمل, async, تہی);
    curlx_free(https_name);
  }
#اگر_ختم /* USE_HTTPSRR */

برآمد:
  ares->نتیجہ = نتیجہ;
  واپس نتیجہ ? نتیجہ : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ساکن CURLcode async_ares_سیٹ_dns_servers(ساخت Curl_easy *ڈیٹا,
                                           ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;
  CURLcode نتیجہ = CURLE_NOT_BUILT_اندر;
  ثابت حرف *servers = CURL_EASY_STR(ڈیٹا, STRING_DNS_SERVERS);
  صحیح ares_نتیجہ = ARES_SUCCESS;

#اگر_تعریف_شدہ DEBUGBUILD
  اگر(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#اگر_ختم

  اگر(!servers)
    واپس CURLE_OK;

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_نتیجہ = ares_سیٹ_servers_ports_csv(ares->channel, servers);
  انتخاب(ares_نتیجہ) {
  حالت ARES_SUCCESS:
    نتیجہ = CURLE_OK;
    توڑو;
  حالت ARES_ENOMEM:
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    توڑو;
  حالت ARES_ENOTINITIALIZED:
  حالت ARES_ENODATA:
  حالت ARES_EBADSTR:
  طےشدہ:
    DEBUGF(infof(ڈیٹا, "bad servers set"));
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode async_ares_سیٹ_dns_interface(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;
  ثابت حرف *interf = CURL_EASY_STR(ڈیٹا, STRING_DNS_INTERFACE);

  اگر(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_سیٹ_local_dev(ares->channel, interf);

  واپس CURLE_OK;
}

ساکن CURLcode async_ares_سیٹ_dns_local_ip4(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async)
{
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;
  ساخت in_addr a4;
  ثابت حرف *local_ip4 = CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP4);

  اگر(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  ورنہ {
    اگر(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(ڈیٹا, "bad DNS IPv4 address"));
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  اگر(ares && ares->channel)
    ares_سیٹ_local_ip4(ares->channel, ntohl(a4.s_addr));

  واپس CURLE_OK;
}

ساکن CURLcode async_ares_سیٹ_dns_local_ip6(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_resolv_async *async)
{
#اگر_تعریف_شدہ USE_IPV6
  ساخت async_ares_ctx *ares = async ? &async->ares : تہی;
  غیرعلامتی حرف a6[INET6_ADDRSTRLEN];
  ثابت حرف *local_ip6 = CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP6);

  اگر(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, حجم(a6));
  }
  ورنہ {
    اگر(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(ڈیٹا, "bad DNS IPv6 address"));
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* if channel is not there, this is a parameter check */
  اگر(ares && ares->channel)
    ares_سیٹ_local_ip6(ares->channel, a6);

  واپس CURLE_OK;
#ورنہ /* no IPv6 support */
  (خالی)ڈیٹا;
  (خالی)async;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

#اگر_ختم /* USE_RESOLV_ARES */
