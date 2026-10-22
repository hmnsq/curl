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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#ifdef USE_ARES
#अन्तर्भाव <ares.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(संरचना Curl_easy *data,
                                  संरचना Curl_resolv_async *async)
{
  जर(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    परत async->timeout_ms - elapsed_ms;
  }
  परत Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#जर ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(संरचना Curl_easy *data,
                           ares_channel channel,
                           संरचना easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  अचिन्हित पूर्णांक bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  जर(!channel)
    परत CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  साठी(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    पूर्णांक flags = 0;
    जर(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    जर(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    जर(!flags)
      थांबा;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    जर(result)
      परत result;
  }
  परत result;
}

timediff_t Curl_ares_timeout_ms(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  जर(!channel)
    परत -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  जर((async_timeout_ms > 0) && (async_timeout_ms < पूर्णांक_उच्च)) {
    संरचना timeval timebuf;
    संरचना timeval *timeout;
    संरचना timeval end = { (पूर्णांक)async_timeout_ms / 1000,
                           ((पूर्णांक)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    जर(timeout)
      परत curlx_tvtoms(timeout);
  }
  परत async_timeout_ms;
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
पूर्णांक Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  पूर्णांक nfds;
  पूर्णांक bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  संरचना pollfd pfd[ARES_GETSOCK_MAXNUM];
  पूर्णांक i;
  पूर्णांक अंक = 0;

  जर(!channel)
    परत 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  साठी(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    जर(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    जर(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    जर(pfd[i].events)
      अंक++;
    अन्यथा
      थांबा;
  }

  जर(अंक) {
    nfds = Curl_poll(pfd, (अचिन्हित पूर्णांक)अंक, timeout_ms);
    जर(nfds < 0)
      परत -1;
  }
  अन्यथा
    nfds = 0;

  जर(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  अन्यथा {
    /* move through the descriptors and ask for processing on them */
    साठी(i = 0; i < अंक; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  परत nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#अन्तर्भाव "doh.h"

शून्य Curl_async_shutdown(संरचना Curl_easy *data,
                         संरचना Curl_resolv_async *async)
{
  जर(async) {
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

शून्य Curl_async_destroy(संरचना Curl_easy *data,
                        संरचना Curl_resolv_async *async)
{
  जर(async) {
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
    जर(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    जर(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(संरचना Curl_easy *data,
                           संरचना Curl_resolv_async *async,
                           स्थिरांक अक्षर *detail)
{
  स्थिरांक अक्षर *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  जर(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  जर(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  परत result;
}

#endif /* USE_CURL_ASYNC */
