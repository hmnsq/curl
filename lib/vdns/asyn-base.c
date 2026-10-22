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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_ARES
#شامل <ares.h>
#اگر_ختم

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
#اگر_تعریف_شدہ CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_resolv_async *async)
{
  اگر(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &async->آغاز);
    واپس async->timeout_ms - elapsed_ms;
  }
  واپس Curl_timeleft_ms(ڈیٹا);
}

#اگر_تعریف_شدہ USE_ARES

#اگر ARES_نسخہ < 0x011000
#غلطی "c-ares 1.16.0 or greater required"
#اگر_ختم

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(ساخت Curl_easy *ڈیٹا,
                           ares_channel channel,
                           ساخت easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  غیرعلامتی صحیح bitmap, i;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(channel);
  اگر(!channel)
    واپس CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  برائے(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    صحیح flags = 0;
    اگر(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_اندر;
    اگر(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_برآمد;
    اگر(!flags)
      توڑو;
    نتیجہ = Curl_pollset_change(ڈیٹا, ps, sockets[i], flags, 0);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس نتیجہ;
}

timediff_t Curl_ares_timeout_ms(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  اگر(!channel)
    واپس -1;

  async_timeout_ms = Curl_async_timeleft_ms(ڈیٹا, async);
  اگر((async_timeout_ms > 0) && (async_timeout_ms < زیادہ_سے_زیادہ_عدد)) {
    ساخت timeval timebuf;
    ساخت timeval *timeout;
    ساخت timeval انتہا = { (صحیح)async_timeout_ms / 1000,
                           ((صحیح)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &انتہا, &timebuf);
    اگر(timeout)
      واپس curlx_tvtoms(timeout);
  }
  واپس async_timeout_ms;
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
  ساخت pollfd pfd[ARES_GETSOCK_MAXNUM];
  صحیح i;
  صحیح عدد = 0;

  اگر(!channel)
    واپس 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  برائے(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
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
      عدد++;
    ورنہ
      توڑو;
  }

  اگر(عدد) {
    nfds = Curl_poll(pfd, (غیرعلامتی صحیح)عدد, timeout_ms);
    اگر(nfds < 0)
      واپس -1;
  }
  ورنہ
    nfds = 0;

  اگر(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  ورنہ {
    /* move through the descriptors and ask for processing on them */
    برائے(i = 0; i < عدد; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  واپس nfds;
}

#اگر_ختم /* USE_ARES */

#اگر_ختم /* CURLRES_ASYNCH */

#اگر_تعریف_شدہ USE_CURL_ASYNC

#شامل "doh.h"

خالی Curl_async_shutdown(ساخت Curl_easy *ڈیٹا,
                         ساخت Curl_resolv_async *async)
{
  اگر(async) {
    CURL_TRC_DNS(ڈیٹا, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#اگر_تعریف_شدہ USE_RESOLV_ARES
    Curl_async_ares_shutdown(ڈیٹا, async);
#اگر_ختم
#اگر_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(ڈیٹا, async);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
    Curl_doh_cleanup(ڈیٹا, async);
#اگر_ختم
  }
}

خالی Curl_async_destroy(ساخت Curl_easy *ڈیٹا,
                        ساخت Curl_resolv_async *async)
{
  اگر(async) {
    CURL_TRC_DNS(ڈیٹا, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#اگر_تعریف_شدہ USE_RESOLV_ARES
    Curl_async_ares_destroy(ڈیٹا, async);
#اگر_ختم
#اگر_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(ڈیٹا, async);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
    Curl_doh_cleanup(ڈیٹا, async);
#اگر_ختم
    اگر(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    اگر(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#اگر_تعریف_شدہ USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#اگر_ختم
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_resolv_async *async,
                           ثابت حرف *detail)
{
  ثابت حرف *host_or_proxy = "host";
  CURLcode نتیجہ = CURLE_COULDNT_RESOLVE_HOST;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(async->for_proxy) {
    host_or_proxy = "proxy";
    نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
  }
#اگر_ختم

  اگر(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(ڈیٹا, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  واپس نتیجہ;
}

#اگر_ختم /* USE_CURL_ASYNC */
