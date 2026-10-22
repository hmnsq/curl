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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#ifdef USE_ARES
#شامل <ares.h>
#endif

#شامل "urldata.h"
#شامل "connect.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "url.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ساختار Curl_easy *data,
                                  ساختار Curl_resolv_async *async)
{
  اگر(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    بازگشت async->timeout_ms - elapsed_ms;
  }
  بازگشت Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#اگر ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(ساختار Curl_easy *data,
                           ares_channel channel,
                           ساختار easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned صحیح bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  اگر(!channel)
    بازگشت CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  برای(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    صحیح flags = 0;
    اگر(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    اگر(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    اگر(!flags)
      بشکن;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    اگر(result)
      بازگشت result;
  }
  بازگشت result;
}

timediff_t Curl_ares_timeout_ms(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  اگر(!channel)
    بازگشت -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  اگر((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    ساختار timeval timebuf;
    ساختار timeval *timeout;
    ساختار timeval end = { (صحیح)async_timeout_ms / 1000,
                           ((صحیح)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    اگر(timeout)
      بازگشت curlx_tvtoms(timeout);
  }
  بازگشت async_timeout_ms;
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
صحیح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  صحیح nfds;
  صحیح bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ساختار pollfd pfd[ARES_GETSOCK_MAXNUM];
  صحیح i;
  صحیح رقم = 0;

  اگر(!channel)
    بازگشت 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  برای(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    اگر(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    اگر(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    اگر(pfd[i].events)
      رقم++;
    وگرنه
      بشکن;
  }

  اگر(رقم) {
    nfds = Curl_poll(pfd, (unsigned صحیح)رقم, timeout_ms);
    اگر(nfds < 0)
      بازگشت -1;
  }
  وگرنه
    nfds = 0;

  اگر(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  وگرنه {
    /* move through the descriptors and ask for processing on them */
    برای(i = 0; i < رقم; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  بازگشت nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#شامل "doh.h"

تهی Curl_async_shutdown(ساختار Curl_easy *data,
                         ساختار Curl_resolv_async *async)
{
  اگر(async) {
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

تهی Curl_async_destroy(ساختار Curl_easy *data,
                        ساختار Curl_resolv_async *async)
{
  اگر(async) {
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
    اگر(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    اگر(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ساختار Curl_easy *data,
                           ساختار Curl_resolv_async *async,
                           ثابت نویسه *detail)
{
  ثابت نویسه *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  اگر(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  اگر(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  بازگشت result;
}

#endif /* USE_CURL_ASYNC */
