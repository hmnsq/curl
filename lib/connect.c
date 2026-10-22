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

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "strerror.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "cf-https-connect.h"
#شامل "cf-setup.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "conncache.h"
#شامل "multihandle.h"
#شامل "select.h"
#شامل "vdns/cf-dns.h"
#شامل "curlx/strparse.h"

#اگر !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

شمار alpnid Curl_alpn2alpnid(ثابت غیرعلامتی حرف *name, size_t len)
{
  اگر(len == 2) {
    اگر(!memcmp(name, "h1", 2))
      واپس ALPN_h1;
    اگر(!memcmp(name, "h2", 2))
      واپس ALPN_h2;
    اگر(!memcmp(name, "h3", 2))
      واپس ALPN_h3;
  }
  ورنہ اگر(len == 8 && !memcmp(name, "http/1.1", 8))
    واپس ALPN_h1;
  واپس ALPN_none; /* unknown, probably rubbish input */
}

#اگر_ختم

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ساکن timediff_t connect_بایاں_ms(ساخت Curl_easy *ڈیٹا,
                                  timediff_t مستعمل_ms)
{
  timediff_t timeout_ms = (ڈیٹا->سیٹ.connecttimeout > 0) ?
    ڈیٹا->سیٹ.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t بایاں_ms = timeout_ms - مستعمل_ms;
  واپس بایاں_ms ? بایاں_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ساخت Curl_easy *ڈیٹا,
                                    ثابت ساخت curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ساخت Curl_easy *ڈیٹا,
                                    ثابت ساخت curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  اگر(ڈیٹا->conn && CURL_CONN_اندر_SHUTDOWN(ڈیٹا->conn, FIRSTSOCKET))
    واپس Curl_cshutdn_timeleft_ms(ڈیٹا, ڈیٹا->conn, FIRSTSOCKET);
  ورنہ اگر(Curl_is_connecting(ڈیٹا)) {
    ctimeleft_ms = connect_بایاں_ms(ڈیٹا,
      Curl_pgrs_since_ms(ڈیٹا, pnow, TIMER_STARTSINGLE));
  }
  ورنہ اگر(!ڈیٹا->سیٹ.timeout || ڈیٹا->سیٹ.connect_only) {
    واپس 0; /* no timeout in place or checked, return "no limit" */
  }

  اگر(ڈیٹا->سیٹ.timeout) {
    timeleft_ms = ڈیٹا->سیٹ.timeout -
                  Curl_pgrs_since_ms(ڈیٹا, pnow, TIMER_STARTOP);
    اگر(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  اگر(!ctimeleft_ms)
    واپس timeleft_ms;
  ورنہ اگر(!timeleft_ms)
    واپس ctimeleft_ms;
  واپس CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ساخت Curl_easy *ڈیٹا)
{
  واپس timeleft_now_ms(ڈیٹا, Curl_pgrs_now(ڈیٹا));
}

timediff_t Curl_timeleft_now_ms(ساخت Curl_easy *ڈیٹا,
                                ثابت ساخت curltime *pnow)
{
  واپس timeleft_now_ms(ڈیٹا, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ساخت connectdata *conn = Curl_cpool_get_آخری_conn(ڈیٹا);
  اگر(connp)
    *connp = conn;
  واپس conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

خالی Curl_conncontrol(ساخت connectdata *conn, صحیح ctrl)
{
  اگر(!conn) {
    DEBUGASSERT(0);
    واپس;
  }
  انتخاب(ctrl) {
    حالت CONNCTRL_CONN_KEEP:
      conn->بٹ.close = FALSE;
      توڑو;
    حالت CONNCTRL_CONN_CLOSE:
      conn->بٹ.close = TRUE;
      توڑو;
    حالت CONNCTRL_دھارا_CLOSE:
      /* stream close when multiplexing does not affect connection */
      اگر(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->بٹ.close = TRUE;
      توڑو;
    طےشدہ:
      DEBUGASSERT(0);
      توڑو;
  }
}

CURLcode Curl_conn_تیاری(ساخت Curl_easy *ڈیٹا,
                         ساخت connectdata *conn,
                         int8_t sockindex,
                         صحیح ssl_منوال)
{
  ساخت Curl_peer *پہلا_peer = Curl_conn_get_پہلا_peer(conn, sockindex);
  CURLcode نتیجہ = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  اگر(!پہلا_peer)
    واپس CURLE_FAILED_INIT;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  اگر(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_منوال != CURL_CF_SSL_DISABLE);
    نتیجہ = Curl_cf_https_تیاری(
      ڈیٹا, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  اگر(!conn->cfilter[sockindex]) {
    نتیجہ = Curl_cf_تیاری_جوڑو(ڈیٹا, conn, sockindex,
                               conn->transport_wanted, ssl_منوال);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(ڈیٹا, conn->ip_نسخہ);
  نتیجہ = Curl_conn_dns_جوڑو_addr_resolve(ڈیٹا, conn, sockindex,
                                          پہلا_peer, dns_queries,
                                          conn->transport_wanted);
  اگر(نتیجہ)
    جاؤ برآمد;

  DEBUGASSERT(conn->cfilter[sockindex]);
برآمد:
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن CURLcode conn_connect_trace(ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_cfilter *cf)
{
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    ساخت ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode نتیجہ;

    نتیجہ = Curl_conn_cf_get_ip_info(cf, ڈیٹا, &is_ipv6, &ipquad);
    اگر(نتیجہ)
      واپس نتیجہ;

    infof(ڈیٹا, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(ڈیٹا->conn),
          ipquad.remote_ip, ipquad.remote_پورٹ,
          ipquad.local_ip, ipquad.local_پورٹ);
  }
  واپس CURLE_OK;
}
#اگر_ختم

/**
 * Update connection statistics
 */
ساکن خالی conn_report_stats(ساخت Curl_easy *ڈیٹا, صحیح sockindex)
{
  /* We do gather stats for the second socket...yet */
  اگر(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(ڈیٹا, ڈیٹا->conn, sockindex);
  }
}

خالی Curl_conn_report_failed(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  اگر(!ڈیٹا->conn || (sockindex != FIRSTSOCKET))
    واپس;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (خالی)Curl_conn_cf_cntrl(ڈیٹا->conn->cfilter[sockindex], ڈیٹا, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, تہی);
  conn_report_stats(ڈیٹا, sockindex);
}

CURLcode Curl_conn_connect(ساخت Curl_easy *ڈیٹا,
                           int8_t sockindex,
                           bool blocking,
                           bool *مکمل)
{
#تعریف_کرو CF_CONN_عدد_POLLS_ON_ڈھیر 5
  ساخت pollfd a_few_on_ڈھیر[CF_CONN_عدد_POLLS_ON_ڈھیر];
  ساخت easy_pollset ps;
  ساخت curl_pollfds cpfds;
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(ڈیٹا->conn->scheme->flags & PROTOPT_NONETWORK) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  cf = ڈیٹا->conn->cfilter[sockindex];
  اگر(!cf) {
    *مکمل = FALSE;
    واپس CURLE_FAILED_INIT;
  }

  *مکمل = (bool)cf->connected;
  اگر(*مکمل)
    واپس CURLE_OK;

  اگر(!ڈیٹا->conn->connect_started.سیٹ[sockindex]) {
    ڈیٹا->conn->connect_started.at[sockindex] = *Curl_pgrs_now(ڈیٹا);
    ڈیٹا->conn->connect_started.سیٹ[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ڈھیر, CF_CONN_عدد_POLLS_ON_ڈھیر);
  جب(!*مکمل) {
    اگر(Curl_conn_needs_flush(ڈیٹا, sockindex)) {
      DEBUGF(infof(ڈیٹا, "Curl_conn_connect(index=%d), flush", sockindex));
      نتیجہ = Curl_conn_flush(ڈیٹا, sockindex);
      اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
        جاؤ برآمد;
    }

    نتیجہ = cf->cft->do_connect(cf, ڈیٹا, مکمل);
    CURL_TRC_CF(ڈیٹا, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (صحیح)نتیجہ, *مکمل);
    اگر(!نتیجہ && *مکمل) {
      /* A final sanity check on connection security */
      اگر((ڈیٹا->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(ڈیٹا, "transfer requires SSL, but not connected via SSL");
        نتیجہ = CURLE_FAILED_INIT;
        جاؤ برآمد;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(ڈیٹا, ڈیٹا->conn);
      conn_report_stats(ڈیٹا, sockindex);
      Curl_cpool_conn_was_مستعمل(ڈیٹا, ڈیٹا->conn, Curl_pgrs_now(ڈیٹا));
      VERBOSE(نتیجہ = conn_connect_trace(ڈیٹا, cf));
      VERBOSE(Curl_conn_trc_filters(ڈیٹا, sockindex, "connected"));
      Curl_conn_remove_تیاری_filters(ڈیٹا, sockindex);
      VERBOSE(Curl_conn_trc_filters(ڈیٹا, sockindex, "reduced to"));
      جاؤ برآمد;
    }
    ورنہ اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "Curl_conn_connect(), filter returned %d",
                  (صحیح)نتیجہ);
      VERBOSE(Curl_conn_trc_filters(ڈیٹا, sockindex, "failed to connect"));
      Curl_conn_report_failed(ڈیٹا, sockindex);
      جاؤ برآمد;
    }

    اگر(!blocking)
      جاؤ برآمد;
    ورنہ {
      /* check allowed time left */
      ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);
      timediff_t timeout_ms = Curl_timeleft_now_ms(ڈیٹا, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, ڈیٹا);
      صحیح rc;

      اگر(!Curl_is_connecting(ڈیٹا)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ثابت ساخت curltime *started =
          &ڈیٹا->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_بایاں_ms(ڈیٹا,
          curlx_ptimediff_ms(pnow, started));
        اگر(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      اگر(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(ڈیٹا, "connect timeout");
        نتیجہ = CURLE_OPERATION_TIMEDOUT;
        جاؤ برآمد;
      }

      CURL_TRC_CF(ڈیٹا, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      اگر(sockfd != CURL_SOCKET_BAD)
        نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, &ps, sockfd);
      اگر(!نتیجہ)
        نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, ڈیٹا->conn, &ps);
      اگر(نتیجہ)
        جاؤ برآمد;
      نتیجہ = Curl_pollfds_جوڑو_ps(&cpfds, &ps);
      اگر(نتیجہ)
        جاؤ برآمد;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(ڈیٹا, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      اگر(rc < 0) {
        نتیجہ = CURLE_COULDNT_CONNECT;
        جاؤ برآمد;
      }
      /* continue iterating */
    }
  }

برآمد:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  واپس نتیجہ;
}

خالی Curl_conn_سیٹ_multiplex(ساخت connectdata *conn)
{
  اگر(!conn->بٹ.multiplex) {
    conn->بٹ.multiplex = TRUE;
    اگر(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ساخت Curl_peer *Curl_conn_get_origin(ساخت connectdata *conn,
                                       int8_t sockindex)
{
  واپس (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ساخت Curl_peer *Curl_conn_get_destination(ساخت connectdata *conn,
                                            int8_t sockindex)
{
  واپس (sockindex == SECONDARYSOCKET) ?
    (conn->بذریعہ_peer2 ? conn->بذریعہ_peer2 : conn->origin2) :
    (conn->بذریعہ_peer ? conn->بذریعہ_peer : conn->origin);
}

ساخت Curl_peer *Curl_conn_get_پہلا_peer(ساخت connectdata *conn,
                                           int8_t sockindex)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->socks_proxy.peer)
    واپس conn->socks_proxy.peer;
  اگر(conn->http_proxy.peer)
    واپس conn->http_proxy.peer;
#اگر_ختم
  واپس (sockindex == SECONDARYSOCKET) ?
    (conn->بذریعہ_peer2 ? conn->بذریعہ_peer2 : conn->origin2) :
    (conn->بذریعہ_peer ? conn->بذریعہ_peer : conn->origin);
}
