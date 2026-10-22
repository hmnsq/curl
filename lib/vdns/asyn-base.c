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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_resolv_async *async)
{
  ಆದರೆ(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &async->ಆರಂಭ);
    ಹಿಂತಿರುಗಿ async->timeout_ms - elapsed_ms;
  }
  ಹಿಂತಿರುಗಿ Curl_timeleft_ms(ದತ್ತಾಂಶ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES

#ಆದರೆ ARES_ಆವೃತ್ತಿ < 0x011000
#ದೋಷ "c-ares 1.16.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ares_channel channel,
                           ರಚನೆ easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ bitmap, i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(channel);
  ಆದರೆ(!channel)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    ಪೂರ್ಣಾಂಕ flags = 0;
    ಆದರೆ(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_ಒಳ;
    ಆದರೆ(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_ಹೊರಹೋಗು;
    ಆದರೆ(!flags)
      ನಿಲ್ಲಿಸು;
    ಫಲಿತಾಂಶ = Curl_pollset_change(ದತ್ತಾಂಶ, ps, sockets[i], flags, 0);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

timediff_t Curl_ares_timeout_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  ಆದರೆ(!channel)
    ಹಿಂತಿರುಗಿ -1;

  async_timeout_ms = Curl_async_timeleft_ms(ದತ್ತಾಂಶ, async);
  ಆದರೆ((async_timeout_ms > 0) && (async_timeout_ms < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)) {
    ರಚನೆ timeval timebuf;
    ರಚನೆ timeval *timeout;
    ರಚನೆ timeval ಕೊನೆ = { (ಪೂರ್ಣಾಂಕ)async_timeout_ms / 1000,
                           ((ಪೂರ್ಣಾಂಕ)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &ಕೊನೆ, &timebuf);
    ಆದರೆ(timeout)
      ಹಿಂತಿರುಗಿ curlx_tvtoms(timeout);
  }
  ಹಿಂತಿರುಗಿ async_timeout_ms;
}

/*
 * Curl_ares_perform()
 *
 * 1) Ask ares what sockets it currently plays with, then
 * 2) wait for the timeout period to check for action on ares' sockets.
 * 3) tell ares to act on all the sockets marked as "with action"
 *
 * return number of sockets it worked on, or -1 on error
 */
ಪೂರ್ಣಾಂಕ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  ಪೂರ್ಣಾಂಕ nfds;
  ಪೂರ್ಣಾಂಕ bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ರಚನೆ pollfd pfd[ARES_GETSOCK_MAXNUM];
  ಪೂರ್ಣಾಂಕ i;
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ = 0;

  ಆದರೆ(!channel)
    ಹಿಂತಿರುಗಿ 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  ಗಾಗಿ(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    ಆದರೆ(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    ಆದರೆ(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    ಆದರೆ(pfd[i].events)
      ಅಂಕೆ++;
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಅಂಕೆ) {
    nfds = Curl_poll(pfd, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಂಕೆ, timeout_ms);
    ಆದರೆ(nfds < 0)
      ಹಿಂತಿರುಗಿ -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    nfds = 0;

  ಆದರೆ(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  ಇಲ್ಲದಿದ್ದರೆ {
    /* move through the descriptors and ask for processing on them */
    ಗಾಗಿ(i = 0; i < ಅಂಕೆ; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ಹಿಂತಿರುಗಿ nfds;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_ARES */

#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_ASYNCH */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC

#ಅನ್ತರ್ಭಾವ "doh.h"

ಶೂನ್ಯ Curl_async_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ Curl_resolv_async *async)
{
  ಆದರೆ(async) {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
    Curl_async_ares_shutdown(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
    Curl_doh_cleanup(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
  }
}

ಶೂನ್ಯ Curl_async_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_resolv_async *async)
{
  ಆದರೆ(async) {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
    Curl_async_ares_destroy(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
    Curl_doh_cleanup(ದತ್ತಾಂಶ, async);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    ಆದರೆ(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#ಆದರೆ_ಅಂತ್ಯ
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_resolv_async *async,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *detail)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host_or_proxy = "host";
  CURLcode ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_HOST;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(async->for_proxy) {
    host_or_proxy = "proxy";
    ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(ದತ್ತಾಂಶ, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */
