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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timediff.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>

#ਜੇ ARES_ਸੰਸਕਰਣ < 0x011000
#ਗਲਤੀ "c-ares 1.16.0 or greater required"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
#ਜੇ ARES_ਸੰਸਕਰਣ < 0x011c00
#ਗਲਤੀ "c-ares 1.28.0 or greater required for HTTPSRR"
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPSRR_ਚੱਲਦਾ
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CARES_TIMEOUT_ਪ੍ਰਤੀ_ATTEMPT 2000

ਸਥਿਰ ਪੂਰਨ ares_ver = 0;

ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_servers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ Curl_resolv_async *async);
ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_interface(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async);
ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_local_ip4(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async);
ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_local_ip6(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
ਪੂਰਨ Curl_async_global_init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CARES_HAVE_ARES_LIBRARY_INIT
  ਜੇ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜੇ_ਅੰਤ
  ares_ਸੰਸਕਰਣ(&ares_ver);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
ਖਾਲੀ Curl_async_global_cleanup(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ sock_state_cb(ਖਾਲੀ *ਅੰਕੜੇ, ares_socket_t socket_fd,
                          ਪੂਰਨ readable, ਪੂਰਨ writable)
{
  ਬਣਤਰ Curl_easy *easy = ਅੰਕੜੇ;
  ਜੇ(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ਸਥਿਰ CURLcode async_ares_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = &async->ares;
  ਪੂਰਨ ਸਥਿਤੀ;
  ਬਣਤਰ ares_options options;
  ਪੂਰਨ optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* initial status - failed */
  ares->ares_ਸਥਿਤੀ = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_ਅੰਕੜੇ = ਅੰਕੜੇ;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  ਜੇ(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_ਪ੍ਰਤੀ_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  ਸਥਿਤੀ = ares_init_options(&ares->channel, &options, optmask);
  ਜੇ(ਸਥਿਤੀ != ARES_SUCCESS) {
    ares->channel = ਨਿਰਾ;
    ਨਤੀਜਾ = (ਸਥਿਤੀ == ARES_ENOMEM) ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = async_ares_ਸੈੱਟ_dns_servers(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = async_ares_ਸੈੱਟ_dns_interface(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = async_ares_ਸੈੱਟ_dns_local_ip4(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = async_ares_ਸੈੱਟ_dns_local_ip6(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ਨਿਰਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_async_ares_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(ਅੰਕੜੇ, async);
}

ਖਾਲੀ Curl_async_ares_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = &async->ares;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ਨਿਰਾ;
  }
}

CURLcode Curl_async_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_resolv_async *async,
                            ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ async_ares_ctx *ares = &async->ares;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ares->channel) {
    ਨਤੀਜਾ = Curl_ares_pollset(ਅੰਕੜੇ, ares->channel, ps);
    ਜੇ(!ਨਤੀਜਾ) {
      timediff_t ms = Curl_ares_timeout_ms(ਅੰਕੜੇ, async, ares->channel);
      Curl_expire(ਅੰਕੜੇ, ms, EXPIRE_ASYNC_NAME);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async,
                                ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ async_ares_ctx *ares = &async->ares;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ਨਿਰਾ;
  ਜੇ(!ares)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(Curl_ares_perform(ares->channel, 0) < 0) {
    ਨਤੀਜਾ = CURLE_UNRECOVERABLE_POLL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(async->queries_ongoing) {
    ਨਤੀਜਾ = CURLE_AGAIN;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* all c-ares operations done, what is the result to report? */
  ਨਤੀਜਾ = ares->ਨਤੀਜਾ;
  ਜੇ(ares->ares_ਸਥਿਤੀ == ARES_SUCCESS && !ਨਤੀਜਾ) {

    ਜੇ(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(ਅੰਕੜੇ, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      ਜੇ(!dns) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HTTPSRR_ਚੱਲਦਾ
    ਜੇ(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(ਅੰਕੜੇ, &async->httpsrr, async->peer);
      ਜੇ(!dns) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
#ਜੇ_ਅੰਤ
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  ਜੇ(!ਨਤੀਜਾ && !dns) {
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = ਨਿਰਾ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_ਸਥਿਤੀ == ARES_ENOTFOUND) ||
       (ares->ares_ਸਥਿਤੀ == ARES_ENODATA));
    ਜੇ(ares->ares_ਸਥਿਤੀ != ARES_SUCCESS)
      ਸੁਨੇਹਾ = ares_strerror(ares->ares_ਸਥਿਤੀ);
    ਨਤੀਜਾ = Curl_async_failed(ਅੰਕੜੇ, async, ਸੁਨੇਹਾ);
  }

  CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (ਪੂਰਨ)ਨਤੀਜਾ, (ਪੂਰਨ)ares->ਨਤੀਜਾ, ares->ares_ਸਥਿਤੀ,
               dns ? "" : "not ");

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
    ares->ਨਤੀਜਾ = ਨਤੀਜਾ;
  *pdns = ਨਤੀਜਾ ? ਨਿਰਾ : dns;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ timediff_t async_ares_poll_timeout(ਬਣਤਰ async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ਬਣਤਰ timeval *ares_calced, ਸਮਾਂ_buf, max_timeout;
  ਪੂਰਨ itimeout_ms;

#ਜੇ TIMEDIFF_T_MAX > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
  itimeout_ms = (timeout_ms > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ :
                 ((timeout_ms < 0) ? -1 : (ਪੂਰਨ)timeout_ms);
#ਨਹੀਂਤਾਂ
  itimeout_ms = (ਪੂਰਨ)timeout_ms;
#ਜੇ_ਅੰਤ
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &ਸਮਾਂ_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  ਜੇ(!ares_calced->tv_sec)
    ਵਾਪਸ (timediff_t)(ares_calced->tv_usec / 1000);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ 1000;
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
CURLcode Curl_async_await(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id,
                          ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;
  ਬਣਤਰ curltime ਸ਼ੁਰੂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ਨਿਰਾ; /* clear on entry */

  ਜੇ(!ares)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  ਜਦੋਂ(!ਨਤੀਜਾ) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
    ਜੇ(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &ਸ਼ੁਰੂ);
      ਜੇ(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      ਨਹੀਂਤਾਂ
        timeout_ms = -1;
    }

    ਜੇ(timeout_ms < 0) {
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    }

    ਜੇ(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      ਨਤੀਜਾ = CURLE_UNRECOVERABLE_POLL;
      ਰੋਕੋ;
    }

    ਨਤੀਜਾ = Curl_async_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਨਤੀਜਾ = CURLE_OK;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ || *pdns)
      ਰੋਕੋ;

    ਜੇ(Curl_pgrsUpdate(ਅੰਕੜੇ)) {
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
      ਰੋਕੋ;
    }
  }

  ਜੇ(ਨਤੀਜਾ)
    ares_cancel(ares->channel);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CARES_MAX_ADDRS 256

ਸਥਿਰ bool async_ares_addr_ਵੇਖਿਆ(ਅਚਲ ਬਣਤਰ Curl_addrinfo *cafirst,
                                 ਪੂਰਨ family, size_t ss_ਮਾਪ,
                                 ਅਚਲ ਖਾਲੀ *addr)
{
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ca;
  ਲਈ(ca = cafirst; ca; ca = ca->ai_ਅਗਲਾ) {
    ਜੇ((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_ਮਾਪ) &&
       !memcmp(ca->ai_addr, addr, ss_ਮਾਪ))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ਸਥਿਰ ਬਣਤਰ Curl_addrinfo *async_ares_node2addr(
  ਬਣਤਰ ares_addrinfo_ਗੰਢ *ਗੰਢ)
{
  /* traverse the ares_addrinfo_node list */
  ਬਣਤਰ ares_addrinfo_ਗੰਢ *ai;
  ਬਣਤਰ Curl_addrinfo *cafirst = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo *calast = ਨਿਰਾ;
  ਪੂਰਨ error = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ naddrs = 0;

  ਲਈ(ai = ਗੰਢ; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_ਅਗਲਾ) {
    size_t ss_ਮਾਪ;
    ਬਣਤਰ Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ਜੇ(ai->ai_family == AF_INET)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ ਜੇ(ai->ai_family == AF_INET6)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
      ਜਾਰੀ;

    /* ignore elements without required address info */
    ਜੇ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ਜਾਰੀ;

    /* ignore elements with bogus address size */
    ਜੇ((size_t)ai->ai_addrlen < ss_ਮਾਪ)
      ਜਾਰੀ;

    /* ignore duplicate addresses already collected */
    ਜੇ(async_ares_addr_ਵੇਖਿਆ(cafirst, ai->ai_family, ss_ਮਾਪ, ai->ai_addr))
      ਜਾਰੀ;

    ca = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ);
    ਜੇ(!ca) {
      error = EAI_MEMORY;
      ਰੋਕੋ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ਮਾਪ;
    ca->ai_addr      = ਨਿਰਾ;
    ca->ai_canonname = ਨਿਰਾ;
    ca->ai_ਅਗਲਾ      = ਨਿਰਾ;

    ca->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ca + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_addr, ai->ai_addr, ss_ਮਾਪ);

    /* if the return list is empty, this becomes the first element */
    ਜੇ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ਜੇ(calast)
      calast->ai_ਅਗਲਾ = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  ਜੇ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ਨਿਰਾ;
  }

  ਵਾਪਸ cafirst;
}

ਸਥਿਰ ਖਾਲੀ async_ares_A_cb(ਖਾਲੀ *user_ਅੰਕੜੇ, ਪੂਰਨ ਸਥਿਤੀ, ਪੂਰਨ timeouts,
                            ਬਣਤਰ ares_addrinfo *ares_ai)
{
  ਬਣਤਰ Curl_resolv_async *async = user_ਅੰਕੜੇ;
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;

  (ਖਾਲੀ)timeouts;
  ਜੇ(!async)
    ਵਾਪਸ;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->ਪੂਰਾ = !async->queries_ongoing;
  ਜੇ(ਸਥਿਤੀ == ARES_SUCCESS) {
    ares->ares_ਸਥਿਤੀ = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ((ਸਥਿਤੀ != ARES_ENOTFOUND) && (ਸਥਿਤੀ != ARES_ENODATA))
      ares->transient_err = TRUE;
    ਜੇ(ares->ares_ਸਥਿਤੀ != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_ਸਥਿਤੀ = ਸਥਿਤੀ;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
ਸਥਿਰ ਖਾਲੀ async_ares_AAAA_cb(ਖਾਲੀ *user_ਅੰਕੜੇ, ਪੂਰਨ ਸਥਿਤੀ, ਪੂਰਨ timeouts,
                               ਬਣਤਰ ares_addrinfo *ares_ai)
{
  ਬਣਤਰ Curl_resolv_async *async = user_ਅੰਕੜੇ;
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;

  (ਖਾਲੀ)timeouts;
  ਜੇ(!async)
    ਵਾਪਸ;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->ਪੂਰਾ = !async->queries_ongoing;
  ਜੇ(ਸਥਿਤੀ == ARES_SUCCESS) {
    ares->ares_ਸਥਿਤੀ = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ((ਸਥਿਤੀ != ARES_ENOTFOUND) && (ਸਥਿਤੀ != ARES_ENODATA))
      ares->transient_err = TRUE;
    ਜੇ(ares->ares_ਸਥਿਤੀ != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_ਸਥਿਤੀ = ਸਥਿਤੀ;
  }
}
#ਜੇ_ਅੰਤ /* CURLRES_IPV6 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
ਸਥਿਰ ਖਾਲੀ async_ares_rr_ਪੂਰਾ(ਖਾਲੀ *user_ਅੰਕੜੇ, ares_ਸਥਿਤੀ_t ਸਥਿਤੀ,
                               size_t timeouts,
                               ਅਚਲ ares_dns_ਰਿਕਾਰਡ_t *dnsrec)
{
  ਬਣਤਰ Curl_resolv_async *async = user_ਅੰਕੜੇ;
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;

  ਜੇ(!async)
    ਵਾਪਸ;

  (ਖਾਲੀ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->ਪੂਰਾ = !async->queries_ongoing;
  ares->ares_ਸਥਿਤੀ = ਸਥਿਤੀ;
  ਜੇ((ਸਥਿਤੀ != ARES_ENOTFOUND) && (ਸਥਿਤੀ != ARES_ENODATA))
    ares->transient_err = TRUE;
  ਜੇ((ARES_SUCCESS != ਸਥਿਤੀ) || !dnsrec)
    ਵਾਪਸ;
  ares->ਨਤੀਜਾ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = &async->ares;
  ਅੱਖਰ service[12];
  ਪੂਰਨ socktype;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ares->channel) {
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = async_ares_init(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ares->channel);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ defined(CURLVERBOSE) && ARES_ਸੰਸਕਰਣ >= 0x011800 /* >= v1.24.0 */
  ਜੇ(CURL_TRC_DNS_is_verbose(ਅੰਕੜੇ)) {
    ਅੱਖਰ *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(ਅੰਕੜੇ, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#ਜੇ_ਅੰਤ

  curl_msnprintf(service, ਆਕਾਰ(service), "%d", async->peer->ਪੋਰਟ);
  socktype =
    (Curl_conn_get_transport(ਅੰਕੜੇ, ਅੰਕੜੇ->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
  ਜੇ(async->dns_queries & CURL_DNSQ_AAAA) {
    ਬਣਤਰ ares_addrinfo_hints hints;

    memset(&hints, 0, ਆਕਾਰ(hints));
    CURL_TRC_DNS(ਅੰਕੜੇ, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#ਜੇ_ਅੰਤ /* CURLRES_IPV6 */

  ਜੇ(async->dns_queries & CURL_DNSQ_A) {
    ਬਣਤਰ ares_addrinfo_hints hints;

    memset(&hints, 0, ਆਕਾਰ(hints));
    CURL_TRC_DNS(ਅੰਕੜੇ, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਜੇ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ਅੱਖਰ *https_name = ਨਿਰਾ;
    ਜੇ(async->peer->ਪੋਰਟ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->ਪੋਰਟ, async->peer->hostname);
      ਜੇ(!https_name)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    CURL_TRC_DNS(ਅੰਕੜੇ, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_ਵਿੱਚ, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_ਪੂਰਾ, async, ਨਿਰਾ);
    curlx_free(https_name);
  }
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

ਬਾਹਰਲਾ:
  ares->ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_servers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
  ਅਚਲ ਅੱਖਰ *servers = CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_SERVERS);
  ਪੂਰਨ ares_ਨਤੀਜਾ = ARES_SUCCESS;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#ਜੇ_ਅੰਤ

  ਜੇ(!servers)
    ਵਾਪਸ CURLE_OK;

  /* if channel is not there, this is a parameter check */
  ਜੇ(ares && ares->channel)
    ares_ਨਤੀਜਾ = ares_ਸੈੱਟ_servers_ports_csv(ares->channel, servers);
  ਚੋਣ(ares_ਨਤੀਜਾ) {
  ਹਾਲਤ ARES_SUCCESS:
    ਨਤੀਜਾ = CURLE_OK;
    ਰੋਕੋ;
  ਹਾਲਤ ARES_ENOMEM:
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਰੋਕੋ;
  ਹਾਲਤ ARES_ENOTINITIALIZED:
  ਹਾਲਤ ARES_ENODATA:
  ਹਾਲਤ ARES_EBADSTR:
  ਮੂਲ:
    DEBUGF(infof(ਅੰਕੜੇ, "bad servers set"));
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_interface(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *interf = CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_INTERFACE);

  ਜੇ(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  ਜੇ(ares && ares->channel)
    ares_ਸੈੱਟ_local_dev(ares->channel, interf);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_local_ip4(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;
  ਬਣਤਰ in_addr a4;
  ਅਚਲ ਅੱਖਰ *local_ip4 = CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP4);

  ਜੇ(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(ਅੰਕੜੇ, "bad DNS IPv4 address"));
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  ਜੇ(ares && ares->channel)
    ares_ਸੈੱਟ_local_ip4(ares->channel, ntohl(a4.s_addr));

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode async_ares_ਸੈੱਟ_dns_local_ip6(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_resolv_async *async)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ async_ares_ctx *ares = async ? &async->ares : ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ a6[INET6_ADDRSTRLEN];
  ਅਚਲ ਅੱਖਰ *local_ip6 = CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP6);

  ਜੇ(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, ਆਕਾਰ(a6));
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(ਅੰਕੜੇ, "bad DNS IPv6 address"));
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
  }

  /* if channel is not there, this is a parameter check */
  ਜੇ(ares && ares->channel)
    ares_ਸੈੱਟ_local_ip6(ares->channel, a6);

  ਵਾਪਸ CURLE_OK;
#ਨਹੀਂਤਾਂ /* no IPv6 support */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)async;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* USE_RESOLV_ARES */
