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

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "strerror.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "cf-https-connect.h"
#ضمن "cf-setup.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "conncache.h"
#ضمن "multihandle.h"
#ضمن "select.h"
#ضمن "vdns/cf-dns.h"
#ضمن "curlx/strparse.h"

#إذا !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

تعداد alpnid Curl_alpn2alpnid(ثابت بیعلامت حرف *name, size_t len)
{
  إذا(len == 2) {
    إذا(!memcmp(name, "h1", 2))
      أرجع ALPN_h1;
    إذا(!memcmp(name, "h2", 2))
      أرجع ALPN_h2;
    إذا(!memcmp(name, "h3", 2))
      أرجع ALPN_h3;
  }
  وإلا إذا(len == 8 && !memcmp(name, "http/1.1", 8))
    أرجع ALPN_h1;
  أرجع ALPN_none; /* unknown, probably rubbish input */
}

#پایان_اگر

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ساكن timediff_t connect_چپ_ms(بنية Curl_easy *داده,
                                  timediff_t استفاده_شده_ms)
{
  timediff_t timeout_ms = (داده->مجموعه.connecttimeout > 0) ?
    داده->مجموعه.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t چپ_ms = timeout_ms - استفاده_شده_ms;
  أرجع چپ_ms ? چپ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *داده,
                                    ثابت بنية curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *داده,
                                    ثابت بنية curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  إذا(داده->conn && CURL_CONN_درون_SHUTDOWN(داده->conn, FIRSTSOCKET))
    أرجع Curl_cshutdn_timeleft_ms(داده, داده->conn, FIRSTSOCKET);
  وإلا إذا(Curl_is_connecting(داده)) {
    ctimeleft_ms = connect_چپ_ms(داده,
      Curl_pgrs_since_ms(داده, pnow, TIMER_STARTSINGLE));
  }
  وإلا إذا(!داده->مجموعه.timeout || داده->مجموعه.connect_only) {
    أرجع 0; /* no timeout in place or checked, return "no limit" */
  }

  إذا(داده->مجموعه.timeout) {
    timeleft_ms = داده->مجموعه.timeout -
                  Curl_pgrs_since_ms(داده, pnow, TIMER_STARTOP);
    إذا(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  إذا(!ctimeleft_ms)
    أرجع timeleft_ms;
  وإلا إذا(!timeleft_ms)
    أرجع ctimeleft_ms;
  أرجع CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(بنية Curl_easy *داده)
{
  أرجع timeleft_now_ms(داده, Curl_pgrs_now(داده));
}

timediff_t Curl_timeleft_now_ms(بنية Curl_easy *داده,
                                ثابت بنية curltime *pnow)
{
  أرجع timeleft_now_ms(داده, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(بنية Curl_easy *داده,
                                  بنية connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  بنية connectdata *conn = Curl_cpool_get_آخرین_conn(داده);
  إذا(connp)
    *connp = conn;
  أرجع conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

فراغ Curl_conncontrol(بنية connectdata *conn, صحيح ctrl)
{
  إذا(!conn) {
    DEBUGASSERT(0);
    أرجع;
  }
  بدل(ctrl) {
    حالة CONNCTRL_CONN_KEEP:
      conn->بیتها.close = FALSE;
      اكسر;
    حالة CONNCTRL_CONN_CLOSE:
      conn->بیتها.close = TRUE;
      اكسر;
    حالة CONNCTRL_جریان_CLOSE:
      /* stream close when multiplexing does not affect connection */
      إذا(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->بیتها.close = TRUE;
      اكسر;
    افتراضي:
      DEBUGASSERT(0);
      اكسر;
  }
}

CURLcode Curl_conn_راهاندازی(بنية Curl_easy *داده,
                         بنية connectdata *conn,
                         int8_t sockindex,
                         صحيح ssl_نما)
{
  بنية Curl_peer *اولی_peer = Curl_conn_get_اولی_peer(conn, sockindex);
  CURLcode نتیجه = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(داده);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  إذا(!اولی_peer)
    أرجع CURLE_FAILED_INIT;

#اگر_تعریف_نشده CURL_DISABLE_HTTP
  إذا(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_نما != CURL_CF_SSL_DISABLE);
    نتیجه = Curl_cf_https_راهاندازی(
      داده, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  إذا(!conn->cfilter[sockindex]) {
    نتیجه = Curl_cf_راهاندازی_جمع_کن(داده, conn, sockindex,
                               conn->transport_wanted, ssl_نما);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(داده, conn->ip_نسخه);
  نتیجه = Curl_conn_dns_جمع_کن_addr_resolve(داده, conn, sockindex,
                                          اولی_peer, dns_queries,
                                          conn->transport_wanted);
  إذا(نتیجه)
    اذهب بیرون;

  DEBUGASSERT(conn->cfilter[sockindex]);
بیرون:
  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
ساكن CURLcode conn_connect_trace(بنية Curl_easy *داده,
                                   بنية Curl_cfilter *cf)
{
  إذا(Curl_trc_is_verbose(داده)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode نتیجه;

    نتیجه = Curl_conn_cf_get_ip_info(cf, داده, &is_ipv6, &ipquad);
    إذا(نتیجه)
      أرجع نتیجه;

    infof(داده, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(داده->conn),
          ipquad.remote_ip, ipquad.remote_پورت,
          ipquad.local_ip, ipquad.local_پورت);
  }
  أرجع CURLE_OK;
}
#پایان_اگر

/**
 * Update connection statistics
 */
ساكن فراغ conn_report_stats(بنية Curl_easy *داده, صحيح sockindex)
{
  /* We do gather stats for the second socket...yet */
  إذا(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(داده, داده->conn, sockindex);
  }
}

فراغ Curl_conn_report_failed(بنية Curl_easy *داده, int8_t sockindex)
{
  إذا(!داده->conn || (sockindex != FIRSTSOCKET))
    أرجع;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (فراغ)Curl_conn_cf_cntrl(داده->conn->cfilter[sockindex], داده, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  conn_report_stats(داده, sockindex);
}

CURLcode Curl_conn_connect(بنية Curl_easy *داده,
                           int8_t sockindex,
                           bool blocking,
                           bool *انجامشده)
{
#تعریف_کن CF_CONN_رقم_POLLS_ON_پشته 5
  بنية pollfd a_few_on_پشته[CF_CONN_رقم_POLLS_ON_پشته];
  بنية easy_pollset ps;
  بنية curl_pollfds cpfds;
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(داده->conn->scheme->flags & PROTOPT_NONETWORK) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  cf = داده->conn->cfilter[sockindex];
  إذا(!cf) {
    *انجامشده = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  *انجامشده = (bool)cf->connected;
  إذا(*انجامشده)
    أرجع CURLE_OK;

  إذا(!داده->conn->connect_started.مجموعه[sockindex]) {
    داده->conn->connect_started.at[sockindex] = *Curl_pgrs_now(داده);
    داده->conn->connect_started.مجموعه[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_پشته, CF_CONN_رقم_POLLS_ON_پشته);
  طالما(!*انجامشده) {
    إذا(Curl_conn_needs_flush(داده, sockindex)) {
      DEBUGF(infof(داده, "Curl_conn_connect(index=%d), flush", sockindex));
      نتیجه = Curl_conn_flush(داده, sockindex);
      إذا(نتیجه && (نتیجه != CURLE_AGAIN))
        اذهب بیرون;
    }

    نتیجه = cf->cft->do_connect(cf, داده, انجامشده);
    CURL_TRC_CF(داده, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (صحيح)نتیجه, *انجامشده);
    إذا(!نتیجه && *انجامشده) {
      /* A final sanity check on connection security */
      إذا((داده->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(داده->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(داده, "transfer requires SSL, but not connected via SSL");
        نتیجه = CURLE_FAILED_INIT;
        اذهب بیرون;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(داده, داده->conn);
      conn_report_stats(داده, sockindex);
      Curl_cpool_conn_was_استفاده_شده(داده, داده->conn, Curl_pgrs_now(داده));
      VERBOSE(نتیجه = conn_connect_trace(داده, cf));
      VERBOSE(Curl_conn_trc_filters(داده, sockindex, "connected"));
      Curl_conn_remove_راهاندازی_filters(داده, sockindex);
      VERBOSE(Curl_conn_trc_filters(داده, sockindex, "reduced to"));
      اذهب بیرون;
    }
    وإلا إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "Curl_conn_connect(), filter returned %d",
                  (صحيح)نتیجه);
      VERBOSE(Curl_conn_trc_filters(داده, sockindex, "failed to connect"));
      Curl_conn_report_failed(داده, sockindex);
      اذهب بیرون;
    }

    إذا(!blocking)
      اذهب بیرون;
    وإلا {
      /* check allowed time left */
      ثابت بنية curltime *pnow = Curl_pgrs_now(داده);
      timediff_t timeout_ms = Curl_timeleft_now_ms(داده, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, داده);
      صحيح rc;

      إذا(!Curl_is_connecting(داده)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ثابت بنية curltime *started =
          &داده->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_چپ_ms(داده,
          curlx_ptimediff_ms(pnow, started));
        إذا(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      إذا(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(داده, "connect timeout");
        نتیجه = CURLE_OPERATION_TIMEDOUT;
        اذهب بیرون;
      }

      CURL_TRC_CF(داده, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      إذا(sockfd != CURL_SOCKET_BAD)
        نتیجه = Curl_pollset_مجموعه_بیرون_only(داده, &ps, sockfd);
      إذا(!نتیجه)
        نتیجه = Curl_conn_adjust_pollset(داده, داده->conn, &ps);
      إذا(نتیجه)
        اذهب بیرون;
      نتیجه = Curl_pollfds_جمع_کن_ps(&cpfds, &ps);
      إذا(نتیجه)
        اذهب بیرون;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(داده, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      إذا(rc < 0) {
        نتیجه = CURLE_COULDNT_CONNECT;
        اذهب بیرون;
      }
      /* continue iterating */
    }
  }

بیرون:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتیجه;
}

فراغ Curl_conn_مجموعه_multiplex(بنية connectdata *conn)
{
  إذا(!conn->بیتها.multiplex) {
    conn->بیتها.multiplex = TRUE;
    إذا(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

بنية Curl_peer *Curl_conn_get_origin(بنية connectdata *conn,
                                       int8_t sockindex)
{
  أرجع (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

بنية Curl_peer *Curl_conn_get_destination(بنية connectdata *conn,
                                            int8_t sockindex)
{
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->از_طریق_peer2 ? conn->از_طریق_peer2 : conn->origin2) :
    (conn->از_طریق_peer ? conn->از_طریق_peer : conn->origin);
}

بنية Curl_peer *Curl_conn_get_اولی_peer(بنية connectdata *conn,
                                           int8_t sockindex)
{
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#پایان_اگر
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->از_طریق_peer2 ? conn->از_طریق_peer2 : conn->origin2) :
    (conn->از_طریق_peer ? conn->از_طریق_peer : conn->origin);
}
