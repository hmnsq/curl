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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_resolv_async *async)
{
  ਜੇ(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &async->ਸ਼ੁਰੂ);
    ਵਾਪਸ async->timeout_ms - elapsed_ms;
  }
  ਵਾਪਸ Curl_timeleft_ms(ਅੰਕੜੇ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES

#ਜੇ ARES_ਸੰਸਕਰਣ < 0x011000
#ਗਲਤੀ "c-ares 1.16.0 or greater required"
#ਜੇ_ਅੰਤ

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ares_channel channel,
                           ਬਣਤਰ easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ bitmap, i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(channel);
  ਜੇ(!channel)
    ਵਾਪਸ CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    ਪੂਰਨ flags = 0;
    ਜੇ(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_ਵਿੱਚ;
    ਜੇ(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_ਬਾਹਰਲਾ;
    ਜੇ(!flags)
      ਰੋਕੋ;
    ਨਤੀਜਾ = Curl_pollset_change(ਅੰਕੜੇ, ps, sockets[i], flags, 0);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

timediff_t Curl_ares_timeout_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  ਜੇ(!channel)
    ਵਾਪਸ -1;

  async_timeout_ms = Curl_async_timeleft_ms(ਅੰਕੜੇ, async);
  ਜੇ((async_timeout_ms > 0) && (async_timeout_ms < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)) {
    ਬਣਤਰ timeval timebuf;
    ਬਣਤਰ timeval *timeout;
    ਬਣਤਰ timeval ਸਿਰਾ = { (ਪੂਰਨ)async_timeout_ms / 1000,
                           ((ਪੂਰਨ)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &ਸਿਰਾ, &timebuf);
    ਜੇ(timeout)
      ਵਾਪਸ curlx_tvtoms(timeout);
  }
  ਵਾਪਸ async_timeout_ms;
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
ਪੂਰਨ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  ਪੂਰਨ nfds;
  ਪੂਰਨ bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ਬਣਤਰ pollfd pfd[ARES_GETSOCK_MAXNUM];
  ਪੂਰਨ i;
  ਪੂਰਨ ਅੰਕ = 0;

  ਜੇ(!channel)
    ਵਾਪਸ 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  ਲਈ(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    ਜੇ(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    ਜੇ(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    ਜੇ(pfd[i].events)
      ਅੰਕ++;
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
  }

  ਜੇ(ਅੰਕ) {
    nfds = Curl_poll(pfd, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਅੰਕ, timeout_ms);
    ਜੇ(nfds < 0)
      ਵਾਪਸ -1;
  }
  ਨਹੀਂਤਾਂ
    nfds = 0;

  ਜੇ(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  ਨਹੀਂਤਾਂ {
    /* move through the descriptors and ask for processing on them */
    ਲਈ(i = 0; i < ਅੰਕ; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ਵਾਪਸ nfds;
}

#ਜੇ_ਅੰਤ /* USE_ARES */

#ਜੇ_ਅੰਤ /* CURLRES_ASYNCH */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC

#ਅਨ੍ਤਰ੍ਭਾਵ "doh.h"

ਖਾਲੀ Curl_async_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ Curl_resolv_async *async)
{
  ਜੇ(async) {
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
    Curl_async_ares_shutdown(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
    Curl_doh_cleanup(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
  }
}

ਖਾਲੀ Curl_async_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_resolv_async *async)
{
  ਜੇ(async) {
    CURL_TRC_DNS(ਅੰਕੜੇ, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
    Curl_async_ares_destroy(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
    Curl_doh_cleanup(ਅੰਕੜੇ, async);
#ਜੇ_ਅੰਤ
    ਜੇ(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    ਜੇ(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#ਜੇ_ਅੰਤ
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_resolv_async *async,
                           ਅਚਲ ਅੱਖਰ *detail)
{
  ਅਚਲ ਅੱਖਰ *host_or_proxy = "host";
  CURLcode ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_HOST;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(async->for_proxy) {
    host_or_proxy = "proxy";
    ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(ਅੰਕੜੇ, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */
