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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

#که_تعریف_شوی USE_ARES
#ضمن <ares.h>
#د_که_پای

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
#که_تعریف_شوی CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(بنية Curl_easy *معلومات,
                                  بنية Curl_resolv_async *async)
{
  إذا(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &async->پیل);
    أرجع async->timeout_ms - elapsed_ms;
  }
  أرجع Curl_timeleft_ms(معلومات);
}

#که_تعریف_شوی USE_ARES

#إذا ARES_نسخه < 0x011000
#تېروتنه "c-ares 1.16.0 or greater required"
#د_که_پای

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(بنية Curl_easy *معلومات,
                           ares_channel channel,
                           بنية easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  بینښه صحيح bitmap, i;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  لكل(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    صحيح flags = 0;
    إذا(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_دننه;
    إذا(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_بهر;
    إذا(!flags)
      اكسر;
    پایله = Curl_pollset_change(معلومات, ps, sockets[i], flags, 0);
    إذا(پایله)
      أرجع پایله;
  }
  أرجع پایله;
}

timediff_t Curl_ares_timeout_ms(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  إذا(!channel)
    أرجع -1;

  async_timeout_ms = Curl_async_timeleft_ms(معلومات, async);
  إذا((async_timeout_ms > 0) && (async_timeout_ms < لوی_عدد)) {
    بنية timeval timebuf;
    بنية timeval *timeout;
    بنية timeval پای_ټکی = { (صحيح)async_timeout_ms / 1000,
                           ((صحيح)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &پای_ټکی, &timebuf);
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
    nfds = Curl_poll(pfd, (بینښه صحيح)عدد, timeout_ms);
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

#د_که_پای /* USE_ARES */

#د_که_پای /* CURLRES_ASYNCH */

#که_تعریف_شوی USE_CURL_ASYNC

#ضمن "doh.h"

فراغ Curl_async_shutdown(بنية Curl_easy *معلومات,
                         بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(معلومات, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#که_تعریف_شوی USE_RESOLV_ARES
    Curl_async_ares_shutdown(معلومات, async);
#د_که_پای
#که_تعریف_شوی USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(معلومات, async);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_DOH
    Curl_doh_cleanup(معلومات, async);
#د_که_پای
  }
}

فراغ Curl_async_destroy(بنية Curl_easy *معلومات,
                        بنية Curl_resolv_async *async)
{
  إذا(async) {
    CURL_TRC_DNS(معلومات, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#که_تعریف_شوی USE_RESOLV_ARES
    Curl_async_ares_destroy(معلومات, async);
#د_که_پای
#که_تعریف_شوی USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(معلومات, async);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_DOH
    Curl_doh_cleanup(معلومات, async);
#د_که_پای
    إذا(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    إذا(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#که_تعریف_شوی USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#د_که_پای
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(بنية Curl_easy *معلومات,
                           بنية Curl_resolv_async *async,
                           ثابت حرف *detail)
{
  ثابت حرف *host_or_proxy = "host";
  CURLcode پایله = CURLE_COULDNT_RESOLVE_HOST;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  إذا(async->for_proxy) {
    host_or_proxy = "proxy";
    پایله = CURLE_COULDNT_RESOLVE_PROXY;
  }
#د_که_پای

  إذا(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(معلومات, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  أرجع پایله;
}

#د_که_پای /* USE_CURL_ASYNC */
