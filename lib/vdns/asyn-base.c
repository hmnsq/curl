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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#ifdef USE_ARES
#অন্তর্ভাৱ <ares.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(সংরচনা Curl_easy *data,
                                  সংরচনা Curl_resolv_async *async)
{
  যদি(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    ঘূৰাই async->timeout_ms - elapsed_ms;
  }
  ঘূৰাই Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#যদি ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(সংরচনা Curl_easy *data,
                           ares_channel channel,
                           সংরচনা easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned পূর্ণাঙ্ক bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  যদি(!channel)
    ঘূৰাই CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  বাবে(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    পূর্ণাঙ্ক flags = 0;
    যদি(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    যদি(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    যদি(!flags)
      ভঙ্গ;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    যদি(result)
      ঘূৰাই result;
  }
  ঘূৰাই result;
}

timediff_t Curl_ares_timeout_ms(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  যদি(!channel)
    ঘূৰাই -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  যদি((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    সংরচনা timeval timebuf;
    সংরচনা timeval *timeout;
    সংরচনা timeval end = { (পূর্ণাঙ্ক)async_timeout_ms / 1000,
                           ((পূর্ণাঙ্ক)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    যদি(timeout)
      ঘূৰাই curlx_tvtoms(timeout);
  }
  ঘূৰাই async_timeout_ms;
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
পূর্ণাঙ্ক Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  পূর্ণাঙ্ক nfds;
  পূর্ণাঙ্ক bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  সংরচনা pollfd pfd[ARES_GETSOCK_MAXNUM];
  পূর্ণাঙ্ক i;
  পূর্ণাঙ্ক অঙ্ক = 0;

  যদি(!channel)
    ঘূৰাই 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  বাবে(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    যদি(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    যদি(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    যদি(pfd[i].events)
      অঙ্ক++;
    অন্যথা
      ভঙ্গ;
  }

  যদি(অঙ্ক) {
    nfds = Curl_poll(pfd, (unsigned পূর্ণাঙ্ক)অঙ্ক, timeout_ms);
    যদি(nfds < 0)
      ঘূৰাই -1;
  }
  অন্যথা
    nfds = 0;

  যদি(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  অন্যথা {
    /* move through the descriptors and ask for processing on them */
    বাবে(i = 0; i < অঙ্ক; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ঘূৰাই nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#অন্তর্ভাৱ "doh.h"

শূন্য Curl_async_shutdown(সংরচনা Curl_easy *data,
                         সংরচনা Curl_resolv_async *async)
{
  যদি(async) {
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

শূন্য Curl_async_destroy(সংরচনা Curl_easy *data,
                        সংরচনা Curl_resolv_async *async)
{
  যদি(async) {
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
    যদি(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    যদি(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(সংরচনা Curl_easy *data,
                           সংরচনা Curl_resolv_async *async,
                           ধ্রুৱ ৱর্ণ *detail)
{
  ধ্রুৱ ৱর্ণ *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  যদি(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  যদি(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ঘূৰাই result;
}

#endif /* USE_CURL_ASYNC */
