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

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ifdef USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#endif

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
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಯದಿ(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    ಪ್ರತ್ಯಾಗಮ async->timeout_ms - elapsed_ms;
  }
  ಪ್ರತ್ಯಾಗಮ Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#ಯದಿ ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(ಸಂರಚನಾ Curl_easy *data,
                           ares_channel channel,
                           ಸಂರಚನಾ easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned ಪೂರ್ಣಾಙ್ಕ bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  ಯದಿ(!channel)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    ಪೂರ್ಣಾಙ್ಕ flags = 0;
    ಯದಿ(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    ಯದಿ(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    ಯದಿ(!flags)
      ಭಙ್ಗ;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

timediff_t Curl_ares_timeout_ms(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  ಯದಿ(!channel)
    ಪ್ರತ್ಯಾಗಮ -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  ಯದಿ((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    ಸಂರಚನಾ timeval timebuf;
    ಸಂರಚನಾ timeval *timeout;
    ಸಂರಚನಾ timeval end = { (ಪೂರ್ಣಾಙ್ಕ)async_timeout_ms / 1000,
                           ((ಪೂರ್ಣಾಙ್ಕ)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    ಯದಿ(timeout)
      ಪ್ರತ್ಯಾಗಮ curlx_tvtoms(timeout);
  }
  ಪ್ರತ್ಯಾಗಮ async_timeout_ms;
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
ಪೂರ್ಣಾಙ್ಕ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  ಪೂರ್ಣಾಙ್ಕ nfds;
  ಪೂರ್ಣಾಙ್ಕ bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ಸಂರಚನಾ pollfd pfd[ARES_GETSOCK_MAXNUM];
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = 0;

  ಯದಿ(!channel)
    ಪ್ರತ್ಯಾಗಮ 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  ಪ್ರತಿ(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    ಯದಿ(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    ಯದಿ(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    ಯದಿ(pfd[i].events)
      ಅಂಕೆ++;
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
  }

  ಯದಿ(ಅಂಕೆ) {
    nfds = Curl_poll(pfd, (unsigned ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ, timeout_ms);
    ಯದಿ(nfds < 0)
      ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಅನ್ಯಥಾ
    nfds = 0;

  ಯದಿ(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  ಅನ್ಯಥಾ {
    /* move through the descriptors and ask for processing on them */
    ಪ್ರತಿ(i = 0; i < ಅಂಕೆ; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ಪ್ರತ್ಯಾಗಮ nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#ಅನ್ತರ್ಭಾವ "doh.h"

ಶೂನ್ಯ Curl_async_shutdown(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಯದಿ(async) {
    CURL_TRC_DNS(data, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#ifdef USE_RESOLV_ARES
    Curl_async_ares_shutdown(data, async);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(data, async);
#endif
#ifndef CURL_DISABLE_DOH
    Curl_doh_cleanup(data, async);
#endif
  }
}

ಶೂನ್ಯ Curl_async_destroy(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಯದಿ(async) {
    CURL_TRC_DNS(data, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#ifdef USE_RESOLV_ARES
    Curl_async_ares_destroy(data, async);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(data, async);
#endif
#ifndef CURL_DISABLE_DOH
    Curl_doh_cleanup(data, async);
#endif
    ಯದಿ(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    ಯದಿ(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_resolv_async *async,
                           ಧ್ರುವ ವರ್ಣ *detail)
{
  ಧ್ರುವ ವರ್ಣ *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  ಯದಿ(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  ಯದಿ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_CURL_ASYNC */
