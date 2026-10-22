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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_ARES
#अन्तर्भाव <ares.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"

/***********************************************************************
 * Only for builds using asynchronous name resolves
 **********************************************************************/
#यदि_परिभाषितम् CURLRES_ASYNCH

timediff_t Curl_async_timeleft_ms(संरचना Curl_easy *दत्त,
                                  संरचना Curl_resolv_async *async)
{
  यदि(async->timeout_ms) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &async->start);
    प्रत्यागम async->timeout_ms - elapsed_ms;
  }
  प्रत्यागम Curl_timeleft_ms(दत्त);
}

#यदि_परिभाषितम् USE_ARES

#यदि ARES_संस्करणम् < 0x011000
#दोषः "c-ares 1.16.0 or greater required"
#यदि_अन्तः

/*
 * Curl_ares_pollset() is called when the outside world (using
 * curl_multi_fdset()) wants to get our fd_set setup and we are talking with
 * ares. The caller must make sure that this function is only called when we
 * have a working ares channel.
 *
 * Returns: sockets-in-use-bitmap
 */
CURLcode Curl_ares_pollset(संरचना Curl_easy *दत्त,
                           ares_channel channel,
                           संरचना easy_pollset *ps)
{
  curl_socket_t sockets[16];  /* ARES documented limit */
  अचिह्नित पूर्णाङ्क bitmap, i;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(channel);
  यदि(!channel)
    प्रत्यागम CURLE_FAILED_INIT;

  bitmap = ares_getsock(channel, (ares_socket_t *)sockets,
                        CURL_ARRAYSIZE(sockets));
  प्रति(i = 0; i < CURL_ARRAYSIZE(sockets); ++i) {
    पूर्णाङ्क flags = 0;
    यदि(ARES_GETSOCK_READABLE(bitmap, i))
      flags |= CURL_POLL_अन्तर्;
    यदि(ARES_GETSOCK_WRITABLE(bitmap, i))
      flags |= CURL_POLL_बहिर्गमनम्;
    यदि(!flags)
      भङ्ग;
    परिणाम = Curl_pollset_change(दत्त, ps, sockets[i], flags, 0);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम परिणाम;
}

timediff_t Curl_ares_timeout_ms(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async,
                                ares_channel channel)
{
  timediff_t async_timeout_ms;

  DEBUGASSERT(channel);
  यदि(!channel)
    प्रत्यागम -1;

  async_timeout_ms = Curl_async_timeleft_ms(दत्त, async);
  यदि((async_timeout_ms > 0) && (async_timeout_ms < अधिकतम_पूर्णाङ्कः)) {
    संरचना timeval timebuf;
    संरचना timeval *timeout;
    संरचना timeval अन्तः = { (पूर्णाङ्क)async_timeout_ms / 1000,
                           ((पूर्णाङ्क)async_timeout_ms % 1000) * 1000 };

    timeout = ares_timeout(channel, &अन्तः, &timebuf);
    यदि(timeout)
      प्रत्यागम curlx_tvtoms(timeout);
  }
  प्रत्यागम async_timeout_ms;
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
पूर्णाङ्क Curl_ares_perform(ares_channel channel, timediff_t timeout_ms)
{
  पूर्णाङ्क nfds;
  पूर्णाङ्क bitmask;
  ares_socket_t socks[ARES_GETSOCK_MAXNUM];
  संरचना pollfd pfd[ARES_GETSOCK_MAXNUM];
  पूर्णाङ्क i;
  पूर्णाङ्क अङ्कः = 0;

  यदि(!channel)
    प्रत्यागम 0;

  bitmask = ares_getsock(channel, socks, ARES_GETSOCK_MAXNUM);

  प्रति(i = 0; i < ARES_GETSOCK_MAXNUM; i++) {
    pfd[i].events = 0;
    pfd[i].revents = 0;
    यदि(ARES_GETSOCK_READABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLRDNORM | POLLIN;
    }
    यदि(ARES_GETSOCK_WRITABLE(bitmask, i)) {
      pfd[i].fd = socks[i];
      pfd[i].events |= POLLWRNORM | POLLOUT;
    }
    यदि(pfd[i].events)
      अङ्कः++;
    अन्यथा
      भङ्ग;
  }

  यदि(अङ्कः) {
    nfds = Curl_poll(pfd, (अचिह्नित पूर्णाङ्क)अङ्कः, timeout_ms);
    यदि(nfds < 0)
      प्रत्यागम -1;
  }
  अन्यथा
    nfds = 0;

  यदि(!nfds)
    /* Call ares_process() unconditionally here, even if we timed out
       above, as otherwise the ares name resolve will not timeout! */
    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
  अन्यथा {
    /* move through the descriptors and ask for processing on them */
    प्रति(i = 0; i < अङ्कः; i++)
      ares_process_fd(channel,
                      (pfd[i].revents & (POLLRDNORM | POLLIN)) ?
                      pfd[i].fd : ARES_SOCKET_BAD,
                      (pfd[i].revents & (POLLWRNORM | POLLOUT)) ?
                      pfd[i].fd : ARES_SOCKET_BAD);
  }
  प्रत्यागम nfds;
}

#यदि_अन्तः /* USE_ARES */

#यदि_अन्तः /* CURLRES_ASYNCH */

#यदि_परिभाषितम् USE_CURL_ASYNC

#अन्तर्भाव "doh.h"

शून्य Curl_async_shutdown(संरचना Curl_easy *दत्त,
                         संरचना Curl_resolv_async *async)
{
  यदि(async) {
    CURL_TRC_DNS(दत्त, "[%u] shutdown async", async->id);
    async->shutdown = TRUE;
#यदि_परिभाषितम् USE_RESOLV_ARES
    Curl_async_ares_shutdown(दत्त, async);
#यदि_अन्तः
#यदि_परिभाषितम् USE_RESOLV_THREADED
    Curl_async_thrdd_shutdown(दत्त, async);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
    Curl_doh_cleanup(दत्त, async);
#यदि_अन्तः
  }
}

शून्य Curl_async_destroy(संरचना Curl_easy *दत्त,
                        संरचना Curl_resolv_async *async)
{
  यदि(async) {
    CURL_TRC_DNS(दत्त, "[%u] destroy async", async->id);
    async->shutdown = TRUE;
#यदि_परिभाषितम् USE_RESOLV_ARES
    Curl_async_ares_destroy(दत्त, async);
#यदि_अन्तः
#यदि_परिभाषितम् USE_RESOLV_THREADED
    Curl_async_thrdd_destroy(दत्त, async);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
    Curl_doh_cleanup(दत्त, async);
#यदि_अन्तः
    यदि(async->ai_A)
      Curl_freeaddrinfo(async->ai_A);
    यदि(async->ai_AAAA)
      Curl_freeaddrinfo(async->ai_AAAA);
#यदि_परिभाषितम् USE_HTTPSRR
    Curl_httpsrr_destroy(async->httpsrr);
#यदि_अन्तः
    Curl_peer_unlink(&async->peer);
    curlx_safefree(async);
  }
}

CURLcode Curl_async_failed(संरचना Curl_easy *दत्त,
                           संरचना Curl_resolv_async *async,
                           ध्रुव वर्ण *detail)
{
  ध्रुव वर्ण *host_or_proxy = "host";
  CURLcode परिणाम = CURLE_COULDNT_RESOLVE_HOST;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(async->for_proxy) {
    host_or_proxy = "proxy";
    परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
  }
#यदि_अन्तः

  यदि(async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
    failf(दत्त, "Could not resolve %s: %s%s%s%s",
          host_or_proxy, async->peer->hostname,
          detail ? " (" : "", detail ? detail : "", detail ? ")" : "");
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_CURL_ASYNC */
