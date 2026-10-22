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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#अगर_घोषित USE_ARES
#समावेश <ares.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "connect.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#अगर_घोषित CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_resolv_async *async)
{
  अगर(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &async->start);
    वापस async->timeout_ms - elapsed_ms;
  }
  वापस Curl_timeleft_ms(तथ्य);
}

#अगर_घोषित USE_ARES

#अगर ARES_VERSION < 0x011000
#त्रुटि "c-ares 1.16.0 or greater required"
#पूर्ण_अगर

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(काष्ठा Curl_easy *तथ्य,
                           ares_channel channel,
                           काष्ठा easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  अचिन्हित पूर्णांक bitmap, i;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(channel);
  अगर(!channel)
    वापस CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  क्रम(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    पूर्णांक flags = 0;
    अगर(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    अगर(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    अगर(!flags)
      अवरोध;
    परिणाम = Curl_pollset_change(तथ्य, ps, sockets[i], flags, 0);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस परिणाम;
}

timediff_t Curl_ares_timeout_ms(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  अगर(!channel)
    वापस -1;

  async_timeout_ms = Curl_async_timeleft_ms(तथ्य, async);
  अगर((async_timeout_ms > 0) && (async_timeout_ms < पूर्णांक_उच्च)) {
    काष्ठा timeval timebuf;
    काष्ठा timeval *timeout;
    काष्ठा timeval end = { (पूर्णांक)async_timeout_ms / 1000,
                           ((पूर्णांक)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    अगर(timeout)
      वापस curlx_tvtoms(timeout);
  }
  वापस async_timeout_ms;
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
  काष्ठा pollfd pfd[ARES_GETSOCK_MAXNUM];
  पूर्णांक i;
  पूर्णांक अंक = 0;

  अगर(!channel)
    वापस 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  क्रम(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    अगर(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    अगर(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    अगर(pfd[i].events)
      अंक++;
    अन्यथा
      अवरोध;
  }

  अगर(अंक) {
    nfds = Curl_poll(pfd, (अचिन्हित पूर्णांक)अंक, timeout_ms);
    अगर(nfds < 0)
      वापस -1;
  }
  अन्यथा
    nfds = 0;

  अगर(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  अन्यथा {
    /* move through the descriptors and ask for processing on them */
    क्रम(i = 0; i < अंक; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  वापस nfds;
}

#पूर्ण_अगर /* USE_ARES */

#पूर्ण_अगर /* CURLRES_ASYNCH */

#अगर_घोषित USE_CURL_ASYNC

#समावेश "doh.h"

व्योम Curl_async_shutdown(काष्ठा Curl_easy *तथ्य,
                         काष्ठा Curl_resolv_async *async)
{
  अगर(async) {
    CURL_TRC_DNS(तथ्य, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#अगर_घोषित USE_RESOLV_ARES
    Curl_async_ares_shutdown(तथ्य, async);
#पूर्ण_अगर
#अगर_घोषित USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(तथ्य, async);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DOH
    Curl_doh_cleanup(तथ्य, async);
#पूर्ण_अगर
  }
}

व्योम Curl_async_destroy(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_resolv_async *async)
{
  अगर(async) {
    CURL_TRC_DNS(तथ्य, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#अगर_घोषित USE_RESOLV_ARES
    Curl_async_ares_destroy(तथ्य, async);
#पूर्ण_अगर
#अगर_घोषित USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(तथ्य, async);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DOH
    Curl_doh_cleanup(तथ्य, async);
#पूर्ण_अगर
    अगर(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    अगर(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#अगर_घोषित USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#पूर्ण_अगर
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_resolv_async *async,
                           स्थिर अक्षर *detail)
{
  स्थिर अक्षर *host_or_proxy = "host";
  CURLcode परिणाम = CURLE_COULDNT_RESOLVE_HOST;

#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(async->for_proxy) {
    host_or_proxy = "proxy";
    परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
  }
#पूर्ण_अगर

  अगर(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(तथ्य, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_CURL_ASYNC */
