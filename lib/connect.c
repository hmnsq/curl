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

تعداد alpnid Curl_alpn2alpnid(ثابت بنانشان حرف *name, size_t len)
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

#جيڪڏهن_پورو

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ساكن timediff_t connect_کاٻو_ms(بنية Curl_easy *معلومات,
                                  timediff_t استعمال_ٿيل_ms)
{
  timediff_t timeout_ms = (معلومات->سيٽ.connecttimeout > 0) ?
    معلومات->سيٽ.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t کاٻو_ms = timeout_ms - استعمال_ٿيل_ms;
  أرجع کاٻو_ms ? کاٻو_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *معلومات,
                                    ثابت بنية curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *معلومات,
                                    ثابت بنية curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  إذا(معلومات->conn && CURL_CONN_اندريون_SHUTDOWN(معلومات->conn, FIRSTSOCKET))
    أرجع Curl_cshutdn_timeleft_ms(معلومات, معلومات->conn, FIRSTSOCKET);
  وإلا إذا(Curl_is_connecting(معلومات)) {
    ctimeleft_ms = connect_کاٻو_ms(معلومات,
      Curl_pgrs_since_ms(معلومات, pnow, TIMER_STARTSINGLE));
  }
  وإلا إذا(!معلومات->سيٽ.timeout || معلومات->سيٽ.connect_only) {
    أرجع 0; /* no timeout in place or checked, return "no limit" */
  }

  إذا(معلومات->سيٽ.timeout) {
    timeleft_ms = معلومات->سيٽ.timeout -
                  Curl_pgrs_since_ms(معلومات, pnow, TIMER_STARTOP);
    إذا(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  إذا(!ctimeleft_ms)
    أرجع timeleft_ms;
  وإلا إذا(!timeleft_ms)
    أرجع ctimeleft_ms;
  أرجع CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(بنية Curl_easy *معلومات)
{
  أرجع timeleft_now_ms(معلومات, Curl_pgrs_now(معلومات));
}

timediff_t Curl_timeleft_now_ms(بنية Curl_easy *معلومات,
                                ثابت بنية curltime *pnow)
{
  أرجع timeleft_now_ms(معلومات, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(بنية Curl_easy *معلومات,
                                  بنية connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  بنية connectdata *conn = Curl_cpool_get_آخري_conn(معلومات);
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
      conn->بٽ.close = FALSE;
      اكسر;
    حالة CONNCTRL_CONN_CLOSE:
      conn->بٽ.close = TRUE;
      اكسر;
    حالة CONNCTRL_وهڪرو_CLOSE:
      /* stream close when multiplexing does not affect connection */
      إذا(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->بٽ.close = TRUE;
      اكسر;
    افتراضي:
      DEBUGASSERT(0);
      اكسر;
  }
}

CURLcode Curl_conn_تياري(بنية Curl_easy *معلومات,
                         بنية connectdata *conn,
                         int8_t sockindex,
                         صحيح ssl_گهڻائي)
{
  بنية Curl_peer *پهريون_peer = Curl_conn_get_پهريون_peer(conn, sockindex);
  CURLcode نتيجو = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  إذا(!پهريون_peer)
    أرجع CURLE_FAILED_INIT;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  إذا(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_گهڻائي != CURL_CF_SSL_DISABLE);
    نتيجو = Curl_cf_https_تياري(
      معلومات, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  إذا(!conn->cfilter[sockindex]) {
    نتيجو = Curl_cf_تياري_جوڙيو(معلومات, conn, sockindex,
                               conn->transport_wanted, ssl_گهڻائي);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(معلومات, conn->ip_نسخو);
  نتيجو = Curl_conn_dns_جوڙيو_addr_resolve(معلومات, conn, sockindex,
                                          پهريون_peer, dns_queries,
                                          conn->transport_wanted);
  إذا(نتيجو)
    اذهب ٻاهريون;

  DEBUGASSERT(conn->cfilter[sockindex]);
ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن CURLcode conn_connect_trace(بنية Curl_easy *معلومات,
                                   بنية Curl_cfilter *cf)
{
  إذا(Curl_trc_is_verbose(معلومات)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode نتيجو;

    نتيجو = Curl_conn_cf_get_ip_info(cf, معلومات, &is_ipv6, &ipquad);
    إذا(نتيجو)
      أرجع نتيجو;

    infof(معلومات, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(معلومات->conn),
          ipquad.remote_ip, ipquad.remote_پورٽ,
          ipquad.local_ip, ipquad.local_پورٽ);
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

/**
 * Update connection statistics
 */
ساكن فراغ conn_report_stats(بنية Curl_easy *معلومات, صحيح sockindex)
{
  /* We do gather stats for the second socket...yet */
  إذا(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(معلومات, معلومات->conn, sockindex);
  }
}

فراغ Curl_conn_report_failed(بنية Curl_easy *معلومات, int8_t sockindex)
{
  إذا(!معلومات->conn || (sockindex != FIRSTSOCKET))
    أرجع;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (فراغ)Curl_conn_cf_cntrl(معلومات->conn->cfilter[sockindex], معلومات, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  conn_report_stats(معلومات, sockindex);
}

CURLcode Curl_conn_connect(بنية Curl_easy *معلومات,
                           int8_t sockindex,
                           bool blocking,
                           bool *پورو)
{
#وصف_ڪريو CF_CONN_انگ_POLLS_ON_ڍير 5
  بنية pollfd a_few_on_ڍير[CF_CONN_انگ_POLLS_ON_ڍير];
  بنية easy_pollset ps;
  بنية curl_pollfds cpfds;
  بنية Curl_cfilter *cf;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(معلومات->conn->scheme->flags & PROTOPT_NONETWORK) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  cf = معلومات->conn->cfilter[sockindex];
  إذا(!cf) {
    *پورو = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  *پورو = (bool)cf->connected;
  إذا(*پورو)
    أرجع CURLE_OK;

  إذا(!معلومات->conn->connect_started.سيٽ[sockindex]) {
    معلومات->conn->connect_started.at[sockindex] = *Curl_pgrs_now(معلومات);
    معلومات->conn->connect_started.سيٽ[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ڍير, CF_CONN_انگ_POLLS_ON_ڍير);
  طالما(!*پورو) {
    إذا(Curl_conn_needs_flush(معلومات, sockindex)) {
      DEBUGF(infof(معلومات, "Curl_conn_connect(index=%d), flush", sockindex));
      نتيجو = Curl_conn_flush(معلومات, sockindex);
      إذا(نتيجو && (نتيجو != CURLE_AGAIN))
        اذهب ٻاهريون;
    }

    نتيجو = cf->cft->do_connect(cf, معلومات, پورو);
    CURL_TRC_CF(معلومات, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (صحيح)نتيجو, *پورو);
    إذا(!نتيجو && *پورو) {
      /* A final sanity check on connection security */
      إذا((معلومات->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(معلومات->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(معلومات, "transfer requires SSL, but not connected via SSL");
        نتيجو = CURLE_FAILED_INIT;
        اذهب ٻاهريون;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(معلومات, معلومات->conn);
      conn_report_stats(معلومات, sockindex);
      Curl_cpool_conn_was_استعمال_ٿيل(معلومات, معلومات->conn, Curl_pgrs_now(معلومات));
      VERBOSE(نتيجو = conn_connect_trace(معلومات, cf));
      VERBOSE(Curl_conn_trc_filters(معلومات, sockindex, "connected"));
      Curl_conn_remove_تياري_filters(معلومات, sockindex);
      VERBOSE(Curl_conn_trc_filters(معلومات, sockindex, "reduced to"));
      اذهب ٻاهريون;
    }
    وإلا إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "Curl_conn_connect(), filter returned %d",
                  (صحيح)نتيجو);
      VERBOSE(Curl_conn_trc_filters(معلومات, sockindex, "failed to connect"));
      Curl_conn_report_failed(معلومات, sockindex);
      اذهب ٻاهريون;
    }

    إذا(!blocking)
      اذهب ٻاهريون;
    وإلا {
      /* check allowed time left */
      ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);
      timediff_t timeout_ms = Curl_timeleft_now_ms(معلومات, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, معلومات);
      صحيح rc;

      إذا(!Curl_is_connecting(معلومات)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ثابت بنية curltime *started =
          &معلومات->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_کاٻو_ms(معلومات,
          curlx_ptimediff_ms(pnow, started));
        إذا(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      إذا(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(معلومات, "connect timeout");
        نتيجو = CURLE_OPERATION_TIMEDOUT;
        اذهب ٻاهريون;
      }

      CURL_TRC_CF(معلومات, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      إذا(sockfd != CURL_SOCKET_BAD)
        نتيجو = Curl_pollset_سيٽ_ٻاهريون_only(معلومات, &ps, sockfd);
      إذا(!نتيجو)
        نتيجو = Curl_conn_adjust_pollset(معلومات, معلومات->conn, &ps);
      إذا(نتيجو)
        اذهب ٻاهريون;
      نتيجو = Curl_pollfds_جوڙيو_ps(&cpfds, &ps);
      إذا(نتيجو)
        اذهب ٻاهريون;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(معلومات, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      إذا(rc < 0) {
        نتيجو = CURLE_COULDNT_CONNECT;
        اذهب ٻاهريون;
      }
      /* continue iterating */
    }
  }

ٻاهريون:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتيجو;
}

فراغ Curl_conn_سيٽ_multiplex(بنية connectdata *conn)
{
  إذا(!conn->بٽ.multiplex) {
    conn->بٽ.multiplex = TRUE;
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
    (conn->ذريعي_peer2 ? conn->ذريعي_peer2 : conn->origin2) :
    (conn->ذريعي_peer ? conn->ذريعي_peer : conn->origin);
}

بنية Curl_peer *Curl_conn_get_پهريون_peer(بنية connectdata *conn,
                                           int8_t sockindex)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#جيڪڏهن_پورو
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->ذريعي_peer2 ? conn->ذريعي_peer2 : conn->origin2) :
    (conn->ذريعي_peer ? conn->ذريعي_peer : conn->origin);
}
