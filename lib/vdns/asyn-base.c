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
#ضمن "curl_setup.h"

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#اگر_تعریف_شده USE_ARES
#ضمن <ares.h>
#پایان_اگر

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#اگر_تعریف_شده CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(بنية Curl_easy *داده,
                                  بنية Curl_resolv_async *async)
{
  إذا(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(داده), &async->شروع);
    أرجع async->timeout_ms - elapsed_ms;
  }
  أرجع Curl_timeleft_ms(داده);
}

#اگر_تعریف_شده USE_ARES

#إذا ARES_نسخه < 0x011000
#خطا "c-ares 1.16.0 or greater required"
#پایان_اگر

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(بنية Curl_easy *داده,
                           ares_channel channel,
                           بنية easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  بیعلامت صحيح bitmap, i;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  لكل(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    صحيح flags = 0;
    إذا(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_درون;
    إذا(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_بیرون;
    إذا(!flags)
      اكسر;
    نتیجه = Curl_pollset_change(داده, ps, sockets[i], flags, 0);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع نتیجه;
}

timediff_t Curl_ares_timeout_ms(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع -1;

  async_timeout_ms = Curl_async_timeleft_ms(داده, async);
  إذا((async_timeout_ms > 0) && (async_timeout_ms < بیشترین_صحیح)) {
    بنية timeval timebuf;
    بنية timeval *timeout;
    بنية timeval انتها = { (صحيح)async_timeout_ms / 1000,
                           ((صحيح)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &انتها, &timebuf);
    إذا(timeout)
      أرجع curlx_tvtoms(timeout);
  }
  أرجع async_timeout_ms;
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
صحيح Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  صحيح nfds;
  صحيح bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  بنية pollfd pfd[ARES_GETSOCK_MAXNUM];
  صحيح i;
  صحيح رقم = 0;

  إذا(!channel)
    أرجع 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  لكل(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    إذا(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    إذا(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    إذا(pfd[i].events)
      رقم++;
    وإلا
      اكسر;
  }

  إذا(رقم) {
    nfds = Curl_poll(pfd, (بیعلامت صحيح)رقم, timeout_ms);
    إذا(nfds < 0)
      أرجع -1;
  }
  وإلا
    nfds = 0;

  إذا(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  وإلا {
    /* move through the descriptors and ask for processing on them */
    لكل(i = 0; i < رقم; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  أرجع nfds;
}

#پایان_اگر /* USE_ARES */

#پایان_اگر /* CURLRES_ASYNCH */

#اگر_تعریف_شده USE_CURL_ASYNC

#ضمن "doh.h"

فراغ Curl_async_shutdown(بنية Curl_easy *داده,
                         بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(داده, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#اگر_تعریف_شده USE_RESOLV_ARES
    Curl_async_ares_shutdown(داده, async);
#پایان_اگر
#اگر_تعریف_شده USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(داده, async);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DOH
    Curl_doh_cleanup(داده, async);
#پایان_اگر
  }
}

فراغ Curl_async_destroy(بنية Curl_easy *داده,
                        بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(داده, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#اگر_تعریف_شده USE_RESOLV_ARES
    Curl_async_ares_destroy(داده, async);
#پایان_اگر
#اگر_تعریف_شده USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(داده, async);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DOH
    Curl_doh_cleanup(داده, async);
#پایان_اگر
    إذا(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    إذا(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#اگر_تعریف_شده USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#پایان_اگر
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(بنية Curl_easy *داده,
                           بنية Curl_resolv_async *async,
                           ثابت حرف *detail)
{
  ثابت حرف *host_or_proxy = "host";
  CURLcode نتیجه = CURLE_COULDNT_RESOLVE_HOST;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(async->for_proxy) {
    host_or_proxy = "proxy";
    نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
  }
#پایان_اگر

  إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(داده, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  أرجع نتیجه;
}

#پایان_اگر /* USE_CURL_ASYNC */
