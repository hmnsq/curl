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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "cf-https-connect.h"
#ಅನ್ತರ್ಭಾವ "cf-setup.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ಯದಿ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ಗಣನಾ alpnid Curl_alpn2alpnid(ಧ್ರುವ unsigned ವರ್ಣ *name, size_t len)
{
  ಯದಿ(len == 2) {
    ಯದಿ(!memcmp(name, "h1", 2))
      ಪ್ರತ್ಯಾಗಮ ALPN_h1;
    ಯದಿ(!memcmp(name, "h2", 2))
      ಪ್ರತ್ಯಾಗಮ ALPN_h2;
    ಯದಿ(!memcmp(name, "h3", 2))
      ಪ್ರತ್ಯಾಗಮ ALPN_h3;
  }
  ಅನ್ಯಥಾ ಯದಿ(len == 8 && !memcmp(name, "http/1.1", 8))
    ಪ್ರತ್ಯಾಗಮ ALPN_h1;
  ಪ್ರತ್ಯಾಗಮ ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ಸ್ಥಿರ timediff_t connect_ಎಡ_ms(ಸಂರಚನಾ Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ಎಡ_ms = timeout_ms - used_ms;
  ಪ್ರತ್ಯಾಗಮ ಎಡ_ms ? ಎಡ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ಸಂರಚನಾ Curl_easy *data,
                                    ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ಸಂರಚನಾ Curl_easy *data,
                                    ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ಯದಿ(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    ಪ್ರತ್ಯಾಗಮ Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  ಅನ್ಯಥಾ ಯದಿ(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_ಎಡ_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  ಅನ್ಯಥಾ ಯದಿ(!data->set.timeout || data->set.connect_only) {
    ಪ್ರತ್ಯಾಗಮ 0; /* no timeout in place or checked, return "no limit" */
  }

  ಯದಿ(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    ಯದಿ(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ಯದಿ(!ctimeleft_ms)
    ಪ್ರತ್ಯಾಗಮ timeleft_ms;
  ಅನ್ಯಥಾ ಯದಿ(!timeleft_ms)
    ಪ್ರತ್ಯಾಗಮ ctimeleft_ms;
  ಪ್ರತ್ಯಾಗಮ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಪ್ರತ್ಯಾಗಮ timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ಸಂರಚನಾ connectdata *conn = Curl_cpool_get_last_conn(data);
  ಯದಿ(connp)
    *connp = conn;
  ಪ್ರತ್ಯಾಗಮ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

ಶೂನ್ಯ Curl_conncontrol(ಸಂರಚನಾ connectdata *conn, ಪೂರ್ಣಾಙ್ಕ ctrl)
{
  ಯದಿ(!conn) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }
  ವಿಕಲ್ಪ(ctrl) {
    ಪಕ್ಷ CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      ಭಙ್ಗ;
    ಪಕ್ಷ CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      ಭಙ್ಗ;
    ಪಕ್ಷ CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ಯದಿ(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      DEBUGASSERT(0);
      ಭಙ್ಗ;
  }
}

CURLcode Curl_conn_setup(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ connectdata *conn,
                         int8_t sockindex,
                         ಪೂರ್ಣಾಙ್ಕ ssl_mode)
{
  ಸಂರಚನಾ Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ಯದಿ(!first_peer)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  ಯದಿ(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ಯದಿ(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ CURLcode conn_connect_trace(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಯದಿ(Curl_trc_is_verbose(data)) {
    ಸಂರಚನಾ ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
ಸ್ಥಿರ ಶೂನ್ಯ conn_report_stats(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ sockindex)
{
  /* We do gather stats for the second socket...yet */
  ಯದಿ(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

ಶೂನ್ಯ Curl_conn_report_failed(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಯದಿ(!data->conn || (sockindex != FIRSTSOCKET))
    ಪ್ರತ್ಯಾಗಮ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (ಶೂನ್ಯ)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ರಿಕ್ತ);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(ಸಂರಚನಾ Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ 5
  ಸಂರಚನಾ pollfd a_few_on_ರಾಶಿ[CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ಸಂರಚನಾ easy_pollset ps;
  ಸಂರಚನಾ curl_pollfds cpfds;
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  ಯದಿ(!cf) {
    *done = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  ಯದಿ(*done)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ);
  ಯಾವತ್(!*done) {
    ಯದಿ(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      ಯದಿ(result && (result != CURLE_AGAIN))
        ಗಚ್ಛ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (ಪೂರ್ಣಾಙ್ಕ)result, *done);
    ಯದಿ(!result && *done) {
      /* A final sanity check on connection security */
      ಯದಿ((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        ಗಚ್ಛ out;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(data, data->conn);
      conn_report_stats(data, sockindex);
      Curl_cpool_conn_was_used(data, data->conn, Curl_pgrs_now(data));
      VERBOSE(result = conn_connect_trace(data, cf));
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "connected"));
      Curl_conn_remove_setup_filters(data, sockindex);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "reduced to"));
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      ಗಚ್ಛ out;
    }

    ಯದಿ(!blocking)
      ಗಚ್ಛ out;
    ಅನ್ಯಥಾ {
      /* check allowed time left */
      ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      ಪೂರ್ಣಾಙ್ಕ rc;

      ಯದಿ(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ಧ್ರುವ ಸಂರಚನಾ curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ಎಡ_ms(data,
          curlx_ptimediff_ms(pnow, started));
        ಯದಿ(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ಯದಿ(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        ಗಚ್ಛ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ಯದಿ(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      ಯದಿ(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ಯದಿ(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        ಗಚ್ಛ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_conn_set_multiplex(ಸಂರಚನಾ connectdata *conn)
{
  ಯದಿ(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    ಯದಿ(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ಸಂರಚನಾ Curl_peer *Curl_conn_get_origin(ಸಂರಚನಾ connectdata *conn,
                                       int8_t sockindex)
{
  ಪ್ರತ್ಯಾಗಮ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ಸಂರಚನಾ Curl_peer *Curl_conn_get_destination(ಸಂರಚನಾ connectdata *conn,
                                            int8_t sockindex)
{
  ಪ್ರತ್ಯಾಗಮ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

ಸಂರಚನಾ Curl_peer *Curl_conn_get_first_peer(ಸಂರಚನಾ connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->socks_proxy.peer)
    ಪ್ರತ್ಯಾಗಮ conn->socks_proxy.peer;
  ಯದಿ(conn->http_proxy.peer)
    ಪ್ರತ್ಯಾಗಮ conn->http_proxy.peer;
#endif
  ಪ್ರತ್ಯಾಗಮ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
