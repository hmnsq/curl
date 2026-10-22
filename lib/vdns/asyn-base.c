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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#ifdef USE_ARES
#include <ares.h>
#endif

#include "urldata.h"
#include "connect.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "multiif.h"
#include "progress.h"
#include "select.h"
#include "url.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(struct Curl_easy *data,
                                  struct Curl_resolv_async *async)
{
  만약(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    반환 async->timeout_ms - elapsed_ms;
  }
  반환 Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#만약 ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(struct Curl_easy *data,
                           ares_channel channel,
                           struct easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned 정수 bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  만약(!channel)
    반환 CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  위해(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    정수 flags = 0;
    만약(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    만약(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    만약(!flags)
      break;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    만약(result)
      반환 result;
  }
  반환 result;
}

timediff_t Curl_ares_timeout_ms(struct Curl_easy *data,
                                struct Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  만약(!channel)
    반환 -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  만약((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    struct timeval timebuf;
    struct timeval *timeout;
    struct timeval end = { (정수)async_timeout_ms / 1000,
                           ((정수)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    만약(timeout)
      반환 curlx_tvtoms(timeout);
  }
  반환 async_timeout_ms;
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
정수 Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  정수 nfds;
  정수 bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  struct pollfd pfd[ARES_GETSOCK_MAXNUM];
  정수 i;
  정수 num = 0;

  만약(!channel)
    반환 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  위해(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    만약(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    만약(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    만약(pfd[i].events)
      num++;
    아니면
      break;
  }

  만약(num) {
    nfds = Curl_poll(pfd, (unsigned 정수)num, timeout_ms);
    만약(nfds < 0)
      반환 -1;
  }
  아니면
    nfds = 0;

  만약(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  아니면 {
    /* move through the descriptors and ask for processing on them */
    위해(i = 0; i < num; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  반환 nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#include "doh.h"

void Curl_async_shutdown(struct Curl_easy *data,
                         struct Curl_resolv_async *async)
{
  만약(async) {
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

void Curl_async_destroy(struct Curl_easy *data,
                        struct Curl_resolv_async *async)
{
  만약(async) {
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
    만약(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    만약(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(struct Curl_easy *data,
                           struct Curl_resolv_async *async,
                           const char *detail)
{
  const char *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  만약(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  만약(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  반환 result;
}

#endif /* USE_CURL_ASYNC */
