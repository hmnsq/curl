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

تعداد alpnid Curl_alpn2alpnid(ثابت بنانشانی حرف *name, size_t len)
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

#جے_انت

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ساكن timediff_t connect_بایاں_ms(بنية Curl_easy *انکڑے,
                                  timediff_t ورتیا_ms)
{
  timediff_t timeout_ms = (انکڑے->سیٹ.connecttimeout > 0) ?
    انکڑے->سیٹ.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t بایاں_ms = timeout_ms - ورتیا_ms;
  أرجع بایاں_ms ? بایاں_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *انکڑے,
                                    ثابت بنية curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *انکڑے,
                                    ثابت بنية curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  إذا(انکڑے->conn && CURL_CONN_وچ_SHUTDOWN(انکڑے->conn, FIRSTSOCKET))
    أرجع Curl_cshutdn_timeleft_ms(انکڑے, انکڑے->conn, FIRSTSOCKET);
  وإلا إذا(Curl_is_connecting(انکڑے)) {
    ctimeleft_ms = connect_بایاں_ms(انکڑے,
      Curl_pgrs_since_ms(انکڑے, pnow, TIMER_STARTSINGLE));
  }
  وإلا إذا(!انکڑے->سیٹ.timeout || انکڑے->سیٹ.connect_only) {
    أرجع 0; /* no timeout in place or checked, return "no limit" */
  }

  إذا(انکڑے->سیٹ.timeout) {
    timeleft_ms = انکڑے->سیٹ.timeout -
                  Curl_pgrs_since_ms(انکڑے, pnow, TIMER_STARTOP);
    إذا(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  إذا(!ctimeleft_ms)
    أرجع timeleft_ms;
  وإلا إذا(!timeleft_ms)
    أرجع ctimeleft_ms;
  أرجع CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(بنية Curl_easy *انکڑے)
{
  أرجع timeleft_now_ms(انکڑے, Curl_pgrs_now(انکڑے));
}

timediff_t Curl_timeleft_now_ms(بنية Curl_easy *انکڑے,
                                ثابت بنية curltime *pnow)
{
  أرجع timeleft_now_ms(انکڑے, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(بنية Curl_easy *انکڑے,
                                  بنية connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  بنية connectdata *conn = Curl_cpool_get_آخری_conn(انکڑے);
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
      conn->بٹ.close = FALSE;
      اكسر;
    حالة CONNCTRL_CONN_CLOSE:
      conn->بٹ.close = TRUE;
      اكسر;
    حالة CONNCTRL_دھارا_CLOSE:
      /* stream close when multiplexing does not affect connection */
      إذا(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->بٹ.close = TRUE;
      اكسر;
    افتراضي:
      DEBUGASSERT(0);
      اكسر;
  }
}

CURLcode Curl_conn_تیاری(بنية Curl_easy *انکڑے,
                         بنية connectdata *conn,
                         int8_t sockindex,
                         صحيح ssl_منوال)
{
  بنية Curl_peer *پہلا_peer = Curl_conn_get_پہلا_peer(conn, sockindex);
  CURLcode نتیجہ = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  إذا(!پہلا_peer)
    أرجع CURLE_FAILED_INIT;

#جے_بنا_تعریف CURL_DISABLE_HTTP
  إذا(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_منوال != CURL_CF_SSL_DISABLE);
    نتیجہ = Curl_cf_https_تیاری(
      انکڑے, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
#جے_انت /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  إذا(!conn->cfilter[sockindex]) {
    نتیجہ = Curl_cf_تیاری_جوڑو(انکڑے, conn, sockindex,
                               conn->transport_wanted, ssl_منوال);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(انکڑے, conn->ip_نسخہ);
  نتیجہ = Curl_conn_dns_جوڑو_addr_resolve(انکڑے, conn, sockindex,
                                          پہلا_peer, dns_queries,
                                          conn->transport_wanted);
  إذا(نتیجہ)
    اذهب باہرلا;

  DEBUGASSERT(conn->cfilter[sockindex]);
باہرلا:
  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن CURLcode conn_connect_trace(بنية Curl_easy *انکڑے,
                                   بنية Curl_cfilter *cf)
{
  إذا(Curl_trc_is_verbose(انکڑے)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode نتیجہ;

    نتیجہ = Curl_conn_cf_get_ip_info(cf, انکڑے, &is_ipv6, &ipquad);
    إذا(نتیجہ)
      أرجع نتیجہ;

    infof(انکڑے, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(انکڑے->conn),
          ipquad.remote_ip, ipquad.remote_پورٹ,
          ipquad.local_ip, ipquad.local_پورٹ);
  }
  أرجع CURLE_OK;
}
#جے_انت

/**
 * Update connection statistics
 */
ساكن فراغ conn_report_stats(بنية Curl_easy *انکڑے, صحيح sockindex)
{
  /* We do gather stats for the second socket...yet */
  إذا(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(انکڑے, انکڑے->conn, sockindex);
  }
}

فراغ Curl_conn_report_failed(بنية Curl_easy *انکڑے, int8_t sockindex)
{
  إذا(!انکڑے->conn || (sockindex != FIRSTSOCKET))
    أرجع;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (فراغ)Curl_conn_cf_cntrl(انکڑے->conn->cfilter[sockindex], انکڑے, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  conn_report_stats(انکڑے, sockindex);
}

CURLcode Curl_conn_connect(بنية Curl_easy *انکڑے,
                           int8_t sockindex,
                           bool blocking,
                           bool *پورا)
{
#تعریف_کرو CF_CONN_عدد_POLLS_ON_ڈھیر 5
  بنية pollfd a_few_on_ڈھیر[CF_CONN_عدد_POLLS_ON_ڈھیر];
  بنية easy_pollset ps;
  بنية curl_pollfds cpfds;
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  إذا(انکڑے->conn->scheme->flags & PROTOPT_NONETWORK) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  cf = انکڑے->conn->cfilter[sockindex];
  إذا(!cf) {
    *پورا = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  *پورا = (bool)cf->connected;
  إذا(*پورا)
    أرجع CURLE_OK;

  إذا(!انکڑے->conn->connect_started.سیٹ[sockindex]) {
    انکڑے->conn->connect_started.at[sockindex] = *Curl_pgrs_now(انکڑے);
    انکڑے->conn->connect_started.سیٹ[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ڈھیر, CF_CONN_عدد_POLLS_ON_ڈھیر);
  طالما(!*پورا) {
    إذا(Curl_conn_needs_flush(انکڑے, sockindex)) {
      DEBUGF(infof(انکڑے, "Curl_conn_connect(index=%d), flush", sockindex));
      نتیجہ = Curl_conn_flush(انکڑے, sockindex);
      إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
        اذهب باہرلا;
    }

    نتیجہ = cf->cft->do_connect(cf, انکڑے, پورا);
    CURL_TRC_CF(انکڑے, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (صحيح)نتیجہ, *پورا);
    إذا(!نتیجہ && *پورا) {
      /* A final sanity check on connection security */
      إذا((انکڑے->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(انکڑے, "transfer requires SSL, but not connected via SSL");
        نتیجہ = CURLE_FAILED_INIT;
        اذهب باہرلا;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(انکڑے, انکڑے->conn);
      conn_report_stats(انکڑے, sockindex);
      Curl_cpool_conn_was_ورتیا(انکڑے, انکڑے->conn, Curl_pgrs_now(انکڑے));
      VERBOSE(نتیجہ = conn_connect_trace(انکڑے, cf));
      VERBOSE(Curl_conn_trc_filters(انکڑے, sockindex, "connected"));
      Curl_conn_remove_تیاری_filters(انکڑے, sockindex);
      VERBOSE(Curl_conn_trc_filters(انکڑے, sockindex, "reduced to"));
      اذهب باہرلا;
    }
    وإلا إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "Curl_conn_connect(), filter returned %d",
                  (صحيح)نتیجہ);
      VERBOSE(Curl_conn_trc_filters(انکڑے, sockindex, "failed to connect"));
      Curl_conn_report_failed(انکڑے, sockindex);
      اذهب باہرلا;
    }

    إذا(!blocking)
      اذهب باہرلا;
    وإلا {
      /* check allowed time left */
      ثابت بنية curltime *pnow = Curl_pgrs_now(انکڑے);
      timediff_t timeout_ms = Curl_timeleft_now_ms(انکڑے, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, انکڑے);
      صحيح rc;

      إذا(!Curl_is_connecting(انکڑے)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ثابت بنية curltime *started =
          &انکڑے->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_بایاں_ms(انکڑے,
          curlx_ptimediff_ms(pnow, started));
        إذا(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      إذا(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(انکڑے, "connect timeout");
        نتیجہ = CURLE_OPERATION_TIMEDOUT;
        اذهب باہرلا;
      }

      CURL_TRC_CF(انکڑے, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      إذا(sockfd != CURL_SOCKET_BAD)
        نتیجہ = Curl_pollset_سیٹ_باہرلا_only(انکڑے, &ps, sockfd);
      إذا(!نتیجہ)
        نتیجہ = Curl_conn_adjust_pollset(انکڑے, انکڑے->conn, &ps);
      إذا(نتیجہ)
        اذهب باہرلا;
      نتیجہ = Curl_pollfds_جوڑو_ps(&cpfds, &ps);
      إذا(نتیجہ)
        اذهب باہرلا;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(انکڑے, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      إذا(rc < 0) {
        نتیجہ = CURLE_COULDNT_CONNECT;
        اذهب باہرلا;
      }
      /* continue iterating */
    }
  }

باہرلا:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتیجہ;
}

فراغ Curl_conn_سیٹ_multiplex(بنية connectdata *conn)
{
  إذا(!conn->بٹ.multiplex) {
    conn->بٹ.multiplex = TRUE;
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
    (conn->راہیں_peer2 ? conn->راہیں_peer2 : conn->origin2) :
    (conn->راہیں_peer ? conn->راہیں_peer : conn->origin);
}

بنية Curl_peer *Curl_conn_get_پہلا_peer(بنية connectdata *conn,
                                           int8_t sockindex)
{
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#جے_انت
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->راہیں_peer2 ? conn->راہیں_peer2 : conn->origin2) :
    (conn->راہیں_peer ? conn->راہیں_peer : conn->origin);
}
