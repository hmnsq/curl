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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ifdef USE_ARES
#ଅନ୍ତର୍ଭାଵ <ares.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_resolv_async *async)
{
  ଯଦି(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    ଫେରନ୍ତୁ async->timeout_ms - elapsed_ms;
  }
  ଫେରନ୍ତୁ Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#ଯଦି ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(ସଂରଚନା Curl_easy *data,
                           ares_channel channel,
                           ସଂରଚନା easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  ଯଦି(!channel)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
    ଯଦି(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    ଯଦି(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    ଯଦି(!flags)
      ଭାଙ୍ଗ;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ result;
}

timediff_t Curl_ares_timeout_ms(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  ଯଦି(!channel)
    ଫେରନ୍ତୁ -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  ଯଦି((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    ସଂରଚନା timeval timebuf;
    ସଂରଚନା timeval *timeout;
    ସଂରଚନା timeval end = { (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)async_timeout_ms / 1000,
                           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    ଯଦି(timeout)
      ଫେରନ୍ତୁ curlx_tvtoms(timeout);
  }
  ଫେରନ୍ତୁ async_timeout_ms;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfds;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ସଂରଚନା pollfd pfd[ARES_GETSOCK_MAXNUM];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = 0;

  ଯଦି(!channel)
    ଫେରନ୍ତୁ 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  ପାଇଁ(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    ଯଦି(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    ଯଦି(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    ଯଦି(pfd[i].events)
      ଅଙ୍କ++;
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;
  }

  ଯଦି(ଅଙ୍କ) {
    nfds = Curl_poll(pfd, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ, timeout_ms);
    ଯଦି(nfds < 0)
      ଫେରନ୍ତୁ -1;
  }
  ଅନ୍ୟଥା
    nfds = 0;

  ଯଦି(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  ଅନ୍ୟଥା {
    /* move through the descriptors and ask for processing on them */
    ପାଇଁ(i = 0; i < ଅଙ୍କ; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ଫେରନ୍ତୁ nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#ଅନ୍ତର୍ଭାଵ "doh.h"

ଶୂନ୍ୟ Curl_async_shutdown(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା Curl_resolv_async *async)
{
  ଯଦି(async) {
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

ଶୂନ୍ୟ Curl_async_destroy(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା Curl_resolv_async *async)
{
  ଯଦି(async) {
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
    ଯଦି(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    ଯଦି(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_resolv_async *async,
                           ଅଚଳ ଅକ୍ଷର *detail)
{
  ଅଚଳ ଅକ୍ଷର *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  ଯଦି(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  ଯଦି(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ଫେରନ୍ତୁ result;
}

#endif /* USE_CURL_ASYNC */
