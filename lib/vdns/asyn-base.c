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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#جے_تعریف_شدہ USE_ARES
#ضمن <ares.h>
#جے_انت

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
#جے_تعریف_شدہ CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(بنية Curl_easy *انکڑے,
                                  بنية Curl_resolv_async *async)
{
  إذا(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &async->شروع);
    أرجع async->timeout_ms - elapsed_ms;
  }
  أرجع Curl_timeleft_ms(انکڑے);
}

#جے_تعریف_شدہ USE_ARES

#إذا ARES_نسخہ < 0x011000
#غلطی "c-ares 1.16.0 or greater required"
#جے_انت

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(بنية Curl_easy *انکڑے,
                           ares_channel channel,
                           بنية easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  بنانشانی صحيح bitmap, i;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  لكل(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    صحيح flags = 0;
    إذا(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_وچ;
    إذا(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_باہرلا;
    إذا(!flags)
      اكسر;
    نتیجہ = Curl_pollset_change(انکڑے, ps, sockets[i], flags, 0);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع نتیجہ;
}

timediff_t Curl_ares_timeout_ms(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع -1;

  async_timeout_ms = Curl_async_timeleft_ms(انکڑے, async);
  إذا((async_timeout_ms > 0) && (async_timeout_ms < ودھ_توں_ودھ_عدد)) {
    بنية timeval timebuf;
    بنية timeval *timeout;
    بنية timeval سرا = { (صحيح)async_timeout_ms / 1000,
                           ((صحيح)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &سرا, &timebuf);
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
  صحيح عدد = 0;

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
      عدد++;
    وإلا
      اكسر;
  }

  إذا(عدد) {
    nfds = Curl_poll(pfd, (بنانشانی صحيح)عدد, timeout_ms);
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
    لكل(i = 0; i < عدد; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  أرجع nfds;
}

#جے_انت /* USE_ARES */

#جے_انت /* CURLRES_ASYNCH */

#جے_تعریف_شدہ USE_CURL_ASYNC

#ضمن "doh.h"

فراغ Curl_async_shutdown(بنية Curl_easy *انکڑے,
                         بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(انکڑے, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#جے_تعریف_شدہ USE_RESOLV_ARES
    Curl_async_ares_shutdown(انکڑے, async);
#جے_انت
#جے_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(انکڑے, async);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DOH
    Curl_doh_cleanup(انکڑے, async);
#جے_انت
  }
}

فراغ Curl_async_destroy(بنية Curl_easy *انکڑے,
                        بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(انکڑے, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#جے_تعریف_شدہ USE_RESOLV_ARES
    Curl_async_ares_destroy(انکڑے, async);
#جے_انت
#جے_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(انکڑے, async);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DOH
    Curl_doh_cleanup(انکڑے, async);
#جے_انت
    إذا(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    إذا(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#جے_تعریف_شدہ USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#جے_انت
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(بنية Curl_easy *انکڑے,
                           بنية Curl_resolv_async *async,
                           ثابت حرف *detail)
{
  ثابت حرف *host_or_proxy = "host";
  CURLcode نتیجہ = CURLE_COULDNT_RESOLVE_HOST;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(async->for_proxy) {
    host_or_proxy = "proxy";
    نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
  }
#جے_انت

  إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(انکڑے, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  أرجع نتیجہ;
}

#جے_انت /* USE_CURL_ASYNC */
