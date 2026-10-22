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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#ifdef USE_ARES
#অন্তর্ভাব <ares.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#ifdef CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_resolv_async *async)
{
  যদি(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &async->start);
    ফেরত async->timeout_ms - elapsed_ms;
  }
  ফেরত Curl_timeleft_ms(তথ্য);
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
CURLcode Curl_ares_pollset(কাঠামো Curl_easy *তথ্য,
                           ares_channel channel,
                           কাঠামো easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  unsigned পূর্ণসংখ্যা bitmap, i;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(channel);
  যদি(!channel)
    ফেরত CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  জন্য(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    পূর্ণসংখ্যা flags = 0;
    যদি(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_IN;
    যদি(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_OUT;
    যদি(!flags)
      বিরতি;
    ফলাফল = Curl_pollset_change(তথ্য, ps, sockets[i], flags, 0);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত ফলাফল;
}

timediff_t Curl_ares_timeout_ms(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  যদি(!channel)
    ফেরত -1;

  async_timeout_ms = Curl_async_timeleft_ms(তথ্য, async);
  যদি((async_timeout_ms > 0) && (async_timeout_ms < INT_MAX)) {
    কাঠামো timeval timebuf;
    কাঠামো timeval *timeout;
    কাঠামো timeval end = { (পূর্ণসংখ্যা)async_timeout_ms / 1000,
                           ((পূর্ণসংখ্যা)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &end, &timebuf);
    যদি(timeout)
      ফেরত curlx_tvtoms(timeout);
  }
  ফেরত async_timeout_ms;
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
পূর্ণসংখ্যা Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  পূর্ণসংখ্যা nfds;
  পূর্ণসংখ্যা bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  কাঠামো pollfd pfd[ARES_GETSOCK_MAXNUM];
  পূর্ণসংখ্যা i;
  পূর্ণসংখ্যা অঙ্ক = 0;

  যদি(!channel)
    ফেরত 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  জন্য(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
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
      বিরতি;
  }

  যদি(অঙ্ক) {
    nfds = Curl_poll(pfd, (unsigned পূর্ণসংখ্যা)অঙ্ক, timeout_ms);
    যদি(nfds < 0)
      ফেরত -1;
  }
  অন্যথা
    nfds = 0;

  যদি(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  অন্যথা {
    /* move through the descriptors and ask for processing on them */
    জন্য(i = 0; i < অঙ্ক; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  ফেরত nfds;
}

#endif /* USE_ARES */

#endif /* CURLRES_ASYNCH */

#ifdef USE_CURL_ASYNC

#অন্তর্ভাব "doh.h"

শূন্য Curl_async_shutdown(কাঠামো Curl_easy *তথ্য,
                         কাঠামো Curl_resolv_async *async)
{
  যদি(async) {
    CURL_TRC_DNS(তথ্য, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#ifdef USE_RESOLV_ARES
    Curl_async_ares_shutdown(তথ্য, async);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(তথ্য, async);
#endif
#ifndef CURL_DISABLE_DOH
    Curl_doh_cleanup(তথ্য, async);
#endif
  }
}

শূন্য Curl_async_destroy(কাঠামো Curl_easy *তথ্য,
                        কাঠামো Curl_resolv_async *async)
{
  যদি(async) {
    CURL_TRC_DNS(তথ্য, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#ifdef USE_RESOLV_ARES
    Curl_async_ares_destroy(তথ্য, async);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(তথ্য, async);
#endif
#ifndef CURL_DISABLE_DOH
    Curl_doh_cleanup(তথ্য, async);
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

CURLcode Curl_async_failed(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_resolv_async *async,
                           ধ্রুবক অক্ষর *detail)
{
  ধ্রুবক অক্ষর *host_or_proxy = "host";
  CURLcode ফলাফল = CURLE_COULDNT_RESOLVE_HOST;

#ifndef CURL_DISABLE_PROXY
  যদি(async->for_proxy) {
    host_or_proxy = "proxy";
    ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
  }
#endif

  যদি(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(তথ্য, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  ফেরত ফলাফল;
}

#endif /* USE_CURL_ASYNC */
