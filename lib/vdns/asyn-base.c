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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#ifdef USE_ARES
#അന്തര്ഭാവ <ares.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ഘടന Curl_easy *data,
                                  ഘടന Curl_resolv_async *async)
{
  എങ്കിൽ(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
    തിരികെ async->timeout_ms - elapsed_ms;
  }
  തിരികെ Curl_timeleft_ms(data);
}

#ifdef USE_ARES

#എങ്കിൽ ARES_VERSION < 0x011000
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
CURLcode Curl_ares_pollset(ഘടന Curl_easy *data,
                           ares_channel channel,
                           ഘടന easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned പൂർണ്ണസംഖ്യ bitmap, i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(channel);
  എങ്കിൽ(!channel)
    തിരികെ CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    പൂർണ്ണസംഖ്യ flags = 0;
    എങ്കിൽ(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    എങ്കിൽ(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    എങ്കിൽ(!flags)
      നിർത്തുക;
    result = Curl_pollset_change(data, ps, sockets[i], flags, 0);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ result;
}

timediff_t Curl_ares_timeout_ms(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  എങ്കിൽ(!channel)
    തിരികെ -1;

  async_timeout_ms = Curl_async_timeleft_ms(data, async);
  എങ്കിൽ((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    ഘടന timeval timebuf;
    ഘടന timeval *timeout;
    ഘടന timeval end = { (പൂർണ്ണസംഖ്യ)async_timeout_ms / 1000,
                           ((പൂർണ്ണസംഖ്യ)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    എങ്കിൽ(timeout)
      തിരികെ curlx_tvtoms(timeout);
  }
  തിരികെ async_timeout_ms;
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
പൂർണ്ണസംഖ്യ Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  പൂർണ്ണസംഖ്യ nfds;
  പൂർണ്ണസംഖ്യ bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  ഘടന pollfd pfd[ARES_GETSOCK_MAXNUM];
  പൂർണ്ണസംഖ്യ i;
  പൂർണ്ണസംഖ്യ അക്കം = 0;

  എങ്കിൽ(!channel)
    തിരികെ 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  വേണ്ടി(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    എങ്കിൽ(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    എങ്കിൽ(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    എങ്കിൽ(pfd[i].events)
      അക്കം++;
    അല്ലെങ്കിൽ
      നിർത്തുക;
  }

  എങ്കിൽ(അക്കം) {
    nfds = Curl_poll(pfd, (unsigned പൂർണ്ണസംഖ്യ)അക്കം, timeout_ms);
    എങ്കിൽ(nfds < 0)
      തിരികെ -1;
  }
  അല്ലെങ്കിൽ
    nfds = 0;

  എങ്കിൽ(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  അല്ലെങ്കിൽ {
    /* move through the descriptors and ask for processing on them */
    വേണ്ടി(i = 0; i < അക്കം; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  തിരികെ nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#അന്തര്ഭാവ "doh.h"

ശൂന്യം Curl_async_shutdown(ഘടന Curl_easy *data,
                         ഘടന Curl_resolv_async *async)
{
  എങ്കിൽ(async) {
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

ശൂന്യം Curl_async_destroy(ഘടന Curl_easy *data,
                        ഘടന Curl_resolv_async *async)
{
  എങ്കിൽ(async) {
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
    എങ്കിൽ(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    എങ്കിൽ(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#ifdef USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#endif
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ഘടന Curl_easy *data,
                           ഘടന Curl_resolv_async *async,
                           സ്ഥിരം അക്ഷരം *detail)
{
  സ്ഥിരം അക്ഷരം *host_or_proxy = "host";
  CURLcode result = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(async->for_proxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  എങ്കിൽ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(data, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  തിരികെ result;
}

#endif /* USE_CURL_ASYNC */
