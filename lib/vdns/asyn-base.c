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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#ifdef USE_ARES
#அந்தர்பாவ <ares.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_resolv_async *async)
{
  எனில்(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    திரும்பு async->timeout_ms - elapsed_ms;
  }
  திரும்பு Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#எனில் ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(கட்டமைப்பு Curl_easy *data,
                           ares_channel channel,
                           கட்டமைப்பு easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned முழு bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  எனில்(!channel)
    திரும்பு CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  க்கு(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    முழு flags = 0;
    எனில்(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    எனில்(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    எனில்(!flags)
      நிறுத்து;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு result;
}

timediff_t Curl_ares_timeout_ms(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  எனில்(!channel)
    திரும்பு -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  எனில்((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    கட்டமைப்பு timeval timebuf;
    கட்டமைப்பு timeval *timeout;
    கட்டமைப்பு timeval end = { (முழு)async_timeout_ms / 1000,
                           ((முழு)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    எனில்(timeout)
      திரும்பு curlx_tvtoms(timeout);
  }
  திரும்பு async_timeout_ms;
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
முழு Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  முழு nfds;
  முழு bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  கட்டமைப்பு pollfd pfd[ARES_GETSOCK_MAXNUM];
  முழு i;
  முழு இலக்கம் = 0;

  எனில்(!channel)
    திரும்பு 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  க்கு(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    எனில்(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    எனில்(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    எனில்(pfd[i].events)
      இலக்கம்++;
    இல்லையெனில்
      நிறுத்து;
  }

  எனில்(இலக்கம்) {
    nfds = Curl_poll(pfd, (unsigned முழு)இலக்கம், timeout_ms);
    எனில்(nfds < 0)
      திரும்பு -1;
  }
  இல்லையெனில்
    nfds = 0;

  எனில்(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  இல்லையெனில் {
    /* move through the descriptors and ask for processing on them */
    க்கு(i = 0; i < இலக்கம்; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  திரும்பு nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#அந்தர்பாவ "doh.h"

வெறுமை Curl_async_shutdown(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு Curl_resolv_async *async)
{
  எனில்(async) {
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

வெறுமை Curl_async_destroy(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு Curl_resolv_async *async)
{
  எனில்(async) {
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
    எனில்(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    எனில்(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_resolv_async *async,
                           மாறிலி எழுத்து *detail)
{
  மாறிலி எழுத்து *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  எனில்(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  எனில்(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  திரும்பு result;
}

#endif /* USE_CURL_ASYNC */
