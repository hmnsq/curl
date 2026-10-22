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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-https-connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਜੇ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ਗਿਣਤੀ alpnid Curl_alpn2alpnid(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *name, size_t len)
{
  ਜੇ(len == 2) {
    ਜੇ(!memcmp(name, "h1", 2))
      ਵਾਪਸ ALPN_h1;
    ਜੇ(!memcmp(name, "h2", 2))
      ਵਾਪਸ ALPN_h2;
    ਜੇ(!memcmp(name, "h3", 2))
      ਵਾਪਸ ALPN_h3;
  }
  ਨਹੀਂਤਾਂ ਜੇ(len == 8 && !memcmp(name, "http/1.1", 8))
    ਵਾਪਸ ALPN_h1;
  ਵਾਪਸ ALPN_none; /* unknown, probably rubbish input */
}

#ਜੇ_ਅੰਤ

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ਸਥਿਰ timediff_t connect_ਖੱਬਾ_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  timediff_t ਵਰਤਿਆ_ms)
{
  timediff_t timeout_ms = (ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout > 0) ?
    ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ਖੱਬਾ_ms = timeout_ms - ਵਰਤਿਆ_ms;
  ਵਾਪਸ ਖੱਬਾ_ms ? ਖੱਬਾ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਬਣਤਰ curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ਜੇ(ਅੰਕੜੇ->conn && CURL_CONN_ਵਿੱਚ_SHUTDOWN(ਅੰਕੜੇ->conn, FIRSTSOCKET))
    ਵਾਪਸ Curl_cshutdn_timeleft_ms(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, FIRSTSOCKET);
  ਨਹੀਂਤਾਂ ਜੇ(Curl_is_connecting(ਅੰਕੜੇ)) {
    ctimeleft_ms = connect_ਖੱਬਾ_ms(ਅੰਕੜੇ,
      Curl_pgrs_since_ms(ਅੰਕੜੇ, pnow, TIMER_STARTSINGLE));
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.timeout || ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
    ਵਾਪਸ 0; /* no timeout in place or checked, return "no limit" */
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timeout) {
    timeleft_ms = ਅੰਕੜੇ->ਸੈੱਟ.timeout -
                  Curl_pgrs_since_ms(ਅੰਕੜੇ, pnow, TIMER_STARTOP);
    ਜੇ(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ਜੇ(!ctimeleft_ms)
    ਵਾਪਸ timeleft_ms;
  ਨਹੀਂਤਾਂ ਜੇ(!timeleft_ms)
    ਵਾਪਸ ctimeleft_ms;
  ਵਾਪਸ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ timeleft_now_ms(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ));
}

timediff_t Curl_timeleft_now_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਵਾਪਸ timeleft_now_ms(ਅੰਕੜੇ, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ਬਣਤਰ connectdata *conn = Curl_cpool_get_ਆਖਰੀ_conn(ਅੰਕੜੇ);
  ਜੇ(connp)
    *connp = conn;
  ਵਾਪਸ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

ਖਾਲੀ Curl_conncontrol(ਬਣਤਰ connectdata *conn, ਪੂਰਨ ctrl)
{
  ਜੇ(!conn) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }
  ਚੋਣ(ctrl) {
    ਹਾਲਤ CONNCTRL_CONN_KEEP:
      conn->ਬਿੱਟ.close = FALSE;
      ਰੋਕੋ;
    ਹਾਲਤ CONNCTRL_CONN_CLOSE:
      conn->ਬਿੱਟ.close = TRUE;
      ਰੋਕੋ;
    ਹਾਲਤ CONNCTRL_ਧਾਰਾ_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ਜੇ(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->ਬਿੱਟ.close = TRUE;
      ਰੋਕੋ;
    ਮੂਲ:
      DEBUGASSERT(0);
      ਰੋਕੋ;
  }
}

CURLcode Curl_conn_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ connectdata *conn,
                         int8_t sockindex,
                         ਪੂਰਨ ssl_ਬਹੁਲਕ)
{
  ਬਣਤਰ Curl_peer *ਪਹਿਲਾ_peer = Curl_conn_get_ਪਹਿਲਾ_peer(conn, sockindex);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ਜੇ(!ਪਹਿਲਾ_peer)
    ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਜੇ(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_ਬਹੁਲਕ != CURL_CF_SSL_DISABLE);
    ਨਤੀਜਾ = Curl_cf_https_ਤਿਆਰੀ(
      ਅੰਕੜੇ, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ਜੇ(!conn->cfilter[sockindex]) {
    ਨਤੀਜਾ = Curl_cf_ਤਿਆਰੀ_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex,
                               conn->transport_wanted, ssl_ਬਹੁਲਕ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(ਅੰਕੜੇ, conn->ip_ਸੰਸਕਰਣ);
  ਨਤੀਜਾ = Curl_conn_dns_ਜੋੜੋ_addr_resolve(ਅੰਕੜੇ, conn, sockindex,
                                          ਪਹਿਲਾ_peer, dns_queries,
                                          conn->transport_wanted);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  DEBUGASSERT(conn->cfilter[sockindex]);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ CURLcode conn_connect_trace(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_cfilter *cf)
{
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    ਬਣਤਰ ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode ਨਤੀਜਾ;

    ਨਤੀਜਾ = Curl_conn_cf_get_ip_info(cf, ਅੰਕੜੇ, &is_ipv6, &ipquad);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    infof(ਅੰਕੜੇ, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(ਅੰਕੜੇ->conn),
          ipquad.remote_ip, ipquad.remote_ਪੋਰਟ,
          ipquad.local_ip, ipquad.local_ਪੋਰਟ);
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

/**
 * Update connection statistics
 */
ਸਥਿਰ ਖਾਲੀ conn_report_stats(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ sockindex)
{
  /* We do gather stats for the second socket...yet */
  ਜੇ(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, sockindex);
  }
}

ਖਾਲੀ Curl_conn_report_failed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਜੇ(!ਅੰਕੜੇ->conn || (sockindex != FIRSTSOCKET))
    ਵਾਪਸ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (ਖਾਲੀ)Curl_conn_cf_cntrl(ਅੰਕੜੇ->conn->cfilter[sockindex], ਅੰਕੜੇ, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ਨਿਰਾ);
  conn_report_stats(ਅੰਕੜੇ, sockindex);
}

CURLcode Curl_conn_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           int8_t sockindex,
                           bool blocking,
                           bool *ਪੂਰਾ)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CONN_ਅੰਕ_POLLS_ON_ਢੇਰ 5
  ਬਣਤਰ pollfd a_few_on_ਢੇਰ[CF_CONN_ਅੰਕ_POLLS_ON_ਢੇਰ];
  ਬਣਤਰ easy_pollset ps;
  ਬਣਤਰ curl_pollfds cpfds;
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_NONETWORK) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜੇ(!cf) {
    *ਪੂਰਾ = FALSE;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  *ਪੂਰਾ = (bool)cf->connected;
  ਜੇ(*ਪੂਰਾ)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(!ਅੰਕੜੇ->conn->connect_started.ਸੈੱਟ[sockindex]) {
    ਅੰਕੜੇ->conn->connect_started.at[sockindex] = *Curl_pgrs_now(ਅੰਕੜੇ);
    ਅੰਕੜੇ->conn->connect_started.ਸੈੱਟ[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ਢੇਰ, CF_CONN_ਅੰਕ_POLLS_ON_ਢੇਰ);
  ਜਦੋਂ(!*ਪੂਰਾ) {
    ਜੇ(Curl_conn_needs_flush(ਅੰਕੜੇ, sockindex)) {
      DEBUGF(infof(ਅੰਕੜੇ, "Curl_conn_connect(index=%d), flush", sockindex));
      ਨਤੀਜਾ = Curl_conn_flush(ਅੰਕੜੇ, sockindex);
      ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਨਤੀਜਾ = cf->cft->do_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
    ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
      /* A final sanity check on connection security */
      ਜੇ((ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(ਅੰਕੜੇ, "transfer requires SSL, but not connected via SSL");
        ਨਤੀਜਾ = CURLE_FAILED_INIT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
      conn_report_stats(ਅੰਕੜੇ, sockindex);
      Curl_cpool_conn_was_ਵਰਤਿਆ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, Curl_pgrs_now(ਅੰਕੜੇ));
      VERBOSE(ਨਤੀਜਾ = conn_connect_trace(ਅੰਕੜੇ, cf));
      VERBOSE(Curl_conn_trc_filters(ਅੰਕੜੇ, sockindex, "connected"));
      Curl_conn_remove_ਤਿਆਰੀ_filters(ਅੰਕੜੇ, sockindex);
      VERBOSE(Curl_conn_trc_filters(ਅੰਕੜੇ, sockindex, "reduced to"));
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Curl_conn_connect(), filter returned %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      VERBOSE(Curl_conn_trc_filters(ਅੰਕੜੇ, sockindex, "failed to connect"));
      Curl_conn_report_failed(ਅੰਕੜੇ, sockindex);
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(!blocking)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਹੀਂਤਾਂ {
      /* check allowed time left */
      ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);
      timediff_t timeout_ms = Curl_timeleft_now_ms(ਅੰਕੜੇ, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
      ਪੂਰਨ rc;

      ਜੇ(!Curl_is_connecting(ਅੰਕੜੇ)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ਅਚਲ ਬਣਤਰ curltime *started =
          &ਅੰਕੜੇ->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ਖੱਬਾ_ms(ਅੰਕੜੇ,
          curlx_ptimediff_ms(pnow, started));
        ਜੇ(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ਜੇ(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(ਅੰਕੜੇ, "connect timeout");
        ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }

      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ਜੇ(sockfd != CURL_SOCKET_BAD)
        ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, &ps, sockfd);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, &ps);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਨਤੀਜਾ = Curl_pollfds_ਜੋੜੋ_ps(&cpfds, &ps);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ਜੇ(rc < 0) {
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* continue iterating */
    }
  }

ਬਾਹਰਲਾ:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_conn_ਸੈੱਟ_multiplex(ਬਣਤਰ connectdata *conn)
{
  ਜੇ(!conn->ਬਿੱਟ.multiplex) {
    conn->ਬਿੱਟ.multiplex = TRUE;
    ਜੇ(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ਬਣਤਰ Curl_peer *Curl_conn_get_origin(ਬਣਤਰ connectdata *conn,
                                       int8_t sockindex)
{
  ਵਾਪਸ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ਬਣਤਰ Curl_peer *Curl_conn_get_destination(ਬਣਤਰ connectdata *conn,
                                            int8_t sockindex)
{
  ਵਾਪਸ (sockindex == SECONDARYSOCKET) ?
    (conn->ਰਾਹੀਂ_peer2 ? conn->ਰਾਹੀਂ_peer2 : conn->origin2) :
    (conn->ਰਾਹੀਂ_peer ? conn->ਰਾਹੀਂ_peer : conn->origin);
}

ਬਣਤਰ Curl_peer *Curl_conn_get_ਪਹਿਲਾ_peer(ਬਣਤਰ connectdata *conn,
                                           int8_t sockindex)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->socks_proxy.peer)
    ਵਾਪਸ conn->socks_proxy.peer;
  ਜੇ(conn->http_proxy.peer)
    ਵਾਪਸ conn->http_proxy.peer;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (sockindex == SECONDARYSOCKET) ?
    (conn->ਰਾਹੀਂ_peer2 ? conn->ਰਾਹੀਂ_peer2 : conn->origin2) :
    (conn->ਰਾਹੀਂ_peer ? conn->ਰਾਹੀਂ_peer : conn->origin);
}
