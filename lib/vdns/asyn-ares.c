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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ <ares.h>

#ಆದರೆ ARES_ಆವೃತ್ತಿ < 0x011000
#ದೋಷ "c-ares 1.16.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
#ಆದರೆ ARES_ಆವೃತ್ತಿ < 0x011c00
#ದೋಷ "c-ares 1.28.0 or greater required for HTTPSRR"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPSRR_ಕೆಲಸಮಾಡುತ್ತದೆ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CARES_TIMEOUT_ಪ್ರತಿ_ATTEMPT 2000

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ares_ver = 0;

ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_servers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_interface(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_local_ip4(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_local_ip6(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
ಪೂರ್ಣಾಂಕ Curl_async_global_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CARES_HAVE_ARES_LIBRARY_INIT
  ಆದರೆ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ares_ಆವೃತ್ತಿ(&ares_ver);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
ಶೂನ್ಯ Curl_async_global_cleanup(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ sock_state_cb(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ares_socket_t socket_fd,
                          ಪೂರ್ಣಾಂಕ readable, ಪೂರ್ಣಾಂಕ writable)
{
  ರಚನೆ Curl_easy *easy = ದತ್ತಾಂಶ;
  ಆದರೆ(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ಸ್ಥಿರ CURLcode async_ares_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = &async->ares;
  ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
  ರಚನೆ ares_options options;
  ಪೂರ್ಣಾಂಕ optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* initial status - failed */
  ares->ares_ಸ್ಥಿತಿ = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  ಆದರೆ(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_ಪ್ರತಿ_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  ಸ್ಥಿತಿ_2 = ares_init_options(&ares->channel, &options, optmask);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != ARES_SUCCESS) {
    ares->channel = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = (ಸ್ಥಿತಿ_2 == ARES_ENOMEM) ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = async_ares_ಗಣ_dns_servers(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = async_ares_ಗಣ_dns_interface(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = async_ares_ಗಣ_dns_local_ip4(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = async_ares_ಗಣ_dns_local_ip6(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_async_ares_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(ದತ್ತಾಂಶ, async);
}

ಶೂನ್ಯ Curl_async_ares_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = &async->ares;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ಖಾಲಿ;
  }
}

CURLcode Curl_async_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_resolv_async *async,
                            ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ async_ares_ctx *ares = &async->ares;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ares->channel) {
    ಫಲಿತಾಂಶ = Curl_ares_pollset(ದತ್ತಾಂಶ, ares->channel, ps);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      timediff_t ms = Curl_ares_timeout_ms(ದತ್ತಾಂಶ, async, ares->channel);
      Curl_expire(ದತ್ತಾಂಶ, ms, EXPIRE_ASYNC_NAME);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async,
                                ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ async_ares_ctx *ares = &async->ares;
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ಖಾಲಿ;
  ಆದರೆ(!ares)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(Curl_ares_perform(ares->channel, 0) < 0) {
    ಫಲಿತಾಂಶ = CURLE_UNRECOVERABLE_POLL;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(async->queries_ongoing) {
    ಫಲಿತಾಂಶ = CURLE_AGAIN;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* all c-ares operations done, what is the result to report? */
  ಫಲಿತಾಂಶ = ares->ಫಲಿತಾಂಶ;
  ಆದರೆ(ares->ares_ಸ್ಥಿತಿ == ARES_SUCCESS && !ಫಲಿತಾಂಶ) {

    ಆದರೆ(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(ದತ್ತಾಂಶ, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      ಆದರೆ(!dns) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HTTPSRR_ಕೆಲಸಮಾಡುತ್ತದೆ
    ಆದರೆ(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(ದತ್ತಾಂಶ, &async->httpsrr, async->peer);
      ಆದರೆ(!dns) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  ಆದರೆ(!ಫಲಿತಾಂಶ && !dns) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = ಖಾಲಿ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_ಸ್ಥಿತಿ == ARES_ENOTFOUND) ||
       (ares->ares_ಸ್ಥಿತಿ == ARES_ENODATA));
    ಆದರೆ(ares->ares_ಸ್ಥಿತಿ != ARES_SUCCESS)
      ಸಂದೇಶ = ares_strerror(ares->ares_ಸ್ಥಿತಿ);
    ಫಲಿತಾಂಶ = Curl_async_failed(ದತ್ತಾಂಶ, async, ಸಂದೇಶ);
  }

  CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, (ಪೂರ್ಣಾಂಕ)ares->ಫಲಿತಾಂಶ, ares->ares_ಸ್ಥಿತಿ,
               dns ? "" : "not ");

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
    ares->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  *pdns = ಫಲಿತಾಂಶ ? ಖಾಲಿ : dns;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ timediff_t async_ares_poll_timeout(ರಚನೆ async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ರಚನೆ timeval *ares_calced, ಸಮಯ_buf, max_timeout;
  ಪೂರ್ಣಾಂಕ itimeout_ms;

#ಆದರೆ TIMEDIFF_T_MAX > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
  itimeout_ms = (timeout_ms > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ :
                 ((timeout_ms < 0) ? -1 : (ಪೂರ್ಣಾಂಕ)timeout_ms);
#ಇಲ್ಲದಿದ್ದರೆ
  itimeout_ms = (ಪೂರ್ಣಾಂಕ)timeout_ms;
#ಆದರೆ_ಅಂತ್ಯ
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &ಸಮಯ_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  ಆದರೆ(!ares_calced->tv_sec)
    ಹಿಂತಿರುಗಿ (timediff_t)(ares_calced->tv_usec / 1000);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ 1000;
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
CURLcode Curl_async_await(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id,
                          ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;
  ರಚನೆ curltime ಆರಂಭ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ಖಾಲಿ; /* clear on entry */

  ಆದರೆ(!ares)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  ಆಗ(!ಫಲಿತಾಂಶ) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
    ಆದರೆ(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &ಆರಂಭ);
      ಆದರೆ(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      ಇಲ್ಲದಿದ್ದರೆ
        timeout_ms = -1;
    }

    ಆದರೆ(timeout_ms < 0) {
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      ಫಲಿತಾಂಶ = CURLE_UNRECOVERABLE_POLL;
      ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = Curl_async_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಫಲಿತಾಂಶ = CURLE_OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ || *pdns)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(Curl_pgrsUpdate(ದತ್ತಾಂಶ)) {
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ares_cancel(ares->channel);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CARES_MAX_ADDRS 256

ಸ್ಥಿರ bool async_ares_addr_ನೋಡಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *cafirst,
                                 ಪೂರ್ಣಾಂಕ family, size_t ss_ಗಾತ್ರ,
                                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *addr)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ca;
  ಗಾಗಿ(ca = cafirst; ca; ca = ca->ai_ಮುಂದಿನ) {
    ಆದರೆ((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_ಗಾತ್ರ) &&
       !memcmp(ca->ai_addr, addr, ss_ಗಾತ್ರ))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ಸ್ಥಿರ ರಚನೆ Curl_addrinfo *async_ares_node2addr(
  ರಚನೆ ares_addrinfo_ಗಂಟು *ಗಂಟು)
{
  /* traverse the ares_addrinfo_node list */
  ರಚನೆ ares_addrinfo_ಗಂಟು *ai;
  ರಚನೆ Curl_addrinfo *cafirst = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo *calast = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ error = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ naddrs = 0;

  ಗಾಗಿ(ai = ಗಂಟು; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_ಮುಂದಿನ) {
    size_t ss_ಗಾತ್ರ;
    ರಚನೆ Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಆದರೆ(ai->ai_family == AF_INET)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ai->ai_family == AF_INET6)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
      ಮುಂದುವರಿ;

    /* ignore elements without required address info */
    ಆದರೆ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಮುಂದುವರಿ;

    /* ignore elements with bogus address size */
    ಆದರೆ((size_t)ai->ai_addrlen < ss_ಗಾತ್ರ)
      ಮುಂದುವರಿ;

    /* ignore duplicate addresses already collected */
    ಆದರೆ(async_ares_addr_ನೋಡಿದ(cafirst, ai->ai_family, ss_ಗಾತ್ರ, ai->ai_addr))
      ಮುಂದುವರಿ;

    ca = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ);
    ಆದರೆ(!ca) {
      error = EAI_MEMORY;
      ನಿಲ್ಲಿಸು;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ಗಾತ್ರ;
    ca->ai_addr      = ಖಾಲಿ;
    ca->ai_canonname = ಖಾಲಿ;
    ca->ai_ಮುಂದಿನ      = ಖಾಲಿ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
    ಸ್ಮರಣೆ_ನಕಲು(ca->ai_addr, ai->ai_addr, ss_ಗಾತ್ರ);

    /* if the return list is empty, this becomes the first element */
    ಆದರೆ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಆದರೆ(calast)
      calast->ai_ಮುಂದಿನ = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  ಆದರೆ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ cafirst;
}

ಸ್ಥಿರ ಶೂನ್ಯ async_ares_A_cb(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ timeouts,
                            ರಚನೆ ares_addrinfo *ares_ai)
{
  ರಚನೆ Curl_resolv_async *async = user_ದತ್ತಾಂಶ;
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;

  (ಶೂನ್ಯ)timeouts;
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->ಮುಗಿದ = !async->queries_ongoing;
  ಆದರೆ(ಸ್ಥಿತಿ_2 == ARES_SUCCESS) {
    ares->ares_ಸ್ಥಿತಿ = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ((ಸ್ಥಿತಿ_2 != ARES_ENOTFOUND) && (ಸ್ಥಿತಿ_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    ಆದರೆ(ares->ares_ಸ್ಥಿತಿ != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_ಸ್ಥಿತಿ = ಸ್ಥಿತಿ_2;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
ಸ್ಥಿರ ಶೂನ್ಯ async_ares_AAAA_cb(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ timeouts,
                               ರಚನೆ ares_addrinfo *ares_ai)
{
  ರಚನೆ Curl_resolv_async *async = user_ದತ್ತಾಂಶ;
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;

  (ಶೂನ್ಯ)timeouts;
  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->ಮುಗಿದ = !async->queries_ongoing;
  ಆದರೆ(ಸ್ಥಿತಿ_2 == ARES_SUCCESS) {
    ares->ares_ಸ್ಥಿತಿ = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ((ಸ್ಥಿತಿ_2 != ARES_ENOTFOUND) && (ಸ್ಥಿತಿ_2 != ARES_ENODATA))
      ares->transient_err = TRUE;
    ಆದರೆ(ares->ares_ಸ್ಥಿತಿ != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_ಸ್ಥಿತಿ = ಸ್ಥಿತಿ_2;
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_IPV6 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
ಸ್ಥಿರ ಶೂನ್ಯ async_ares_rr_ಮುಗಿದ(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ares_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2,
                               size_t timeouts,
                               ಸ್ಥಿರಾಂಕ ares_dns_ದಾಖಲೆ_t *dnsrec)
{
  ರಚನೆ Curl_resolv_async *async = user_ದತ್ತಾಂಶ;
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;

  ಆದರೆ(!async)
    ಹಿಂತಿರುಗಿ;

  (ಶೂನ್ಯ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->ಮುಗಿದ = !async->queries_ongoing;
  ares->ares_ಸ್ಥಿತಿ = ಸ್ಥಿತಿ_2;
  ಆದರೆ((ಸ್ಥಿತಿ_2 != ARES_ENOTFOUND) && (ಸ್ಥಿತಿ_2 != ARES_ENODATA))
    ares->transient_err = TRUE;
  ಆದರೆ((ARES_SUCCESS != ಸ್ಥಿತಿ_2) || !dnsrec)
    ಹಿಂತಿರುಗಿ;
  ares->ಫಲಿತಾಂಶ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = &async->ares;
  ಅಕ್ಷರ service[12];
  ಪೂರ್ಣಾಂಕ socktype;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ares->channel) {
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = async_ares_init(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, ares->channel);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

#ಆದರೆ defined(CURLVERBOSE) && ARES_ಆವೃತ್ತಿ >= 0x011800 /* >= v1.24.0 */
  ಆದರೆ(CURL_TRC_DNS_is_verbose(ದತ್ತಾಂಶ)) {
    ಅಕ್ಷರ *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#ಆದರೆ_ಅಂತ್ಯ

  curl_msnprintf(service, ಗಾತ್ರ(service), "%d", async->peer->ಪೋರ್ಟ್);
  socktype =
    (Curl_conn_get_transport(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
  ಆದರೆ(async->dns_queries & CURL_DNSQ_AAAA) {
    ರಚನೆ ares_addrinfo_hints hints;

    memset(&hints, 0, ಗಾತ್ರ(hints));
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_IPV6 */

  ಆದರೆ(async->dns_queries & CURL_DNSQ_A) {
    ರಚನೆ ares_addrinfo_hints hints;

    memset(&hints, 0, ಗಾತ್ರ(hints));
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಆದರೆ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ಅಕ್ಷರ *https_name = ಖಾಲಿ;
    ಆದರೆ(async->peer->ಪೋರ್ಟ್ != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->ಪೋರ್ಟ್, async->peer->hostname);
      ಆದರೆ(!https_name)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_ಒಳ, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_ಮುಗಿದ, async, ಖಾಲಿ);
    curlx_free(https_name);
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

ಹೊರಹೋಗು:
  ares->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_servers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *servers = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_SERVERS);
  ಪೂರ್ಣಾಂಕ ares_ಫಲಿತಾಂಶ = ARES_SUCCESS;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!servers)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* if channel is not there, this is a parameter check */
  ಆದರೆ(ares && ares->channel)
    ares_ಫಲಿತಾಂಶ = ares_ಗಣ_servers_ports_csv(ares->channel, servers);
  ಆಯ್ಕೆ(ares_ಫಲಿತಾಂಶ) {
  ಸ್ಥಿತಿ ARES_SUCCESS:
    ಫಲಿತಾಂಶ = CURLE_OK;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ARES_ENOMEM:
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ARES_ENOTINITIALIZED:
  ಸ್ಥಿತಿ ARES_ENODATA:
  ಸ್ಥಿತಿ ARES_EBADSTR:
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGF(infof(ದತ್ತಾಂಶ, "bad servers set"));
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_interface(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *interf = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_INTERFACE);

  ಆದರೆ(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  ಆದರೆ(ares && ares->channel)
    ares_ಗಣ_local_dev(ares->channel, interf);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_local_ip4(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;
  ರಚನೆ in_addr a4;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *local_ip4 = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP4);

  ಆದರೆ(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "bad DNS IPv4 address"));
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  ಆದರೆ(ares && ares->channel)
    ares_ಗಣ_local_ip4(ares->channel, ntohl(a4.s_addr));

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode async_ares_ಗಣ_dns_local_ip6(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_resolv_async *async)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ async_ares_ctx *ares = async ? &async->ares : ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ a6[INET6_ADDRSTRLEN];
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *local_ip6 = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP6);

  ಆದರೆ(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, ಗಾತ್ರ(a6));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "bad DNS IPv6 address"));
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
  }

  /* if channel is not there, this is a parameter check */
  ಆದರೆ(ares && ares->channel)
    ares_ಗಣ_local_ip6(ares->channel, a6);

  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಇಲ್ಲದಿದ್ದರೆ /* no IPv6 support */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)async;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_RESOLV_ARES */
