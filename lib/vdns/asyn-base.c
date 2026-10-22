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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ARES
#అన్తర్భావ <ares.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#నిర్వచితమైతే CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_resolv_async *async)
{
  ఒకవేళ(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &async->ప్రారంభం);
    తిరిగి async->timeout_ms - elapsed_ms;
  }
  తిరిగి Curl_timeleft_ms(దత్తాంశం);
}

#నిర్వచితమైతే USE_ARES

#ఒకవేళ ARES_సంస్కరణ < 0x011000
#దోషం "c-ares 1.16.0 or greater required"
#అయితే_ముగింపు

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                           ares_channel channel,
                           నిర్మాణం easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  గుర్తులేని పూర్ణాంకం bitmap, i;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(channel);
  ఒకవేళ(!channel)
    తిరిగి CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  కొరకు(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    పూర్ణాంకం flags = 0;
    ఒకవేళ(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_లోపల;
    ఒకవేళ(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_బహిర్గతం;
    ఒకవేళ(!flags)
      ఆపు;
    ఫలితం = Curl_pollset_change(దత్తాంశం, ps, sockets[i], flags, 0);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి ఫలితం;
}

timediff_t Curl_ares_timeout_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  ఒకవేళ(!channel)
    తిరిగి -1;

  async_timeout_ms = Curl_async_timeleft_ms(దత్తాంశం, async);
  ఒకవేళ((async_timeout_ms > 0) && (async_timeout_ms < గరిష్ఠ_పూర్ణాంకం)) {
    నిర్మాణం timeval timebuf;
    నిర్మాణం timeval *timeout;
    నిర్మాణం timeval చివర = { (పూర్ణాంకం)async_timeout_ms / 1000,
                           ((పూర్ణాంకం)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &చివర, &timebuf);
    ఒకవేళ(timeout)
      తిరిగి curlx_tvtoms(timeout);
  }
  తిరిగి async_timeout_ms;
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
పూర్ణాంకం Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  పూర్ణాంకం nfds;
  పూర్ణాంకం bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  నిర్మాణం pollfd pfd[ARES_GETSOCK_MAXNUM];
  పూర్ణాంకం i;
  పూర్ణాంకం అంకె = 0;

  ఒకవేళ(!channel)
    తిరిగి 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  కొరకు(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    ఒకవేళ(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    ఒకవేళ(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    ఒకవేళ(pfd[i].events)
      అంకె++;
    లేకపోతే
      ఆపు;
  }

  ఒకవేళ(అంకె) {
    nfds = Curl_poll(pfd, (గుర్తులేని పూర్ణాంకం)అంకె, timeout_ms);
    ఒకవేళ(nfds < 0)
      తిరిగి -1;
  }
  లేకపోతే
    nfds = 0;

  ఒకవేళ(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  లేకపోతే {
    /* move through the descriptors and ask for processing on them */
    కొరకు(i = 0; i < అంకె; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  తిరిగి nfds;
}

#అయితే_ముగింపు /* USE_ARES */

#అయితే_ముగింపు /* CURLRES_ASYNCH */

#నిర్వచితమైతే USE_CURL_ASYNC

#అన్తర్భావ "doh.h"

శూన్య Curl_async_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం Curl_resolv_async *async)
{
  ఒకవేళ(async) {
    CURL_TRC_DNS(దత్తాంశం, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#నిర్వచితమైతే USE_RESOLV_ARES
    Curl_async_ares_shutdown(దత్తాంశం, async);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(దత్తాంశం, async);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
    Curl_doh_cleanup(దత్తాంశం, async);
#అయితే_ముగింపు
  }
}

శూన్య Curl_async_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_resolv_async *async)
{
  ఒకవేళ(async) {
    CURL_TRC_DNS(దత్తాంశం, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#నిర్వచితమైతే USE_RESOLV_ARES
    Curl_async_ares_destroy(దత్తాంశం, async);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(దత్తాంశం, async);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
    Curl_doh_cleanup(దత్తాంశం, async);
#అయితే_ముగింపు
    ఒకవేళ(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    ఒకవేళ(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#నిర్వచితమైతే USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#అయితే_ముగింపు
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_resolv_async *async,
                           స్థిరరాశి అక్షరం *detail)
{
  స్థిరరాశి అక్షరం *host_or_proxy = "host";
  CURLcode ఫలితం = CURLE_COULDNT_RESOLVE_HOST;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(async->for_proxy) {
    host_or_proxy = "proxy";
    ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
  }
#అయితే_ముగింపు

  ఒకవేళ(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(దత్తాంశం, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_CURL_ASYNC */
