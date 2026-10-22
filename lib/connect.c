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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "strerror.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "cf-https-connect.h"
#অন্তর্ভাৱ "cf-setup.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "conncache.h"
#অন্তর্ভাৱ "multihandle.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "vdns/cf-dns.h"
#অন্তর্ভাৱ "curlx/strparse.h"

#যদি !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

গণনা alpnid Curl_alpn2alpnid(ধ্রুৱ unsigned ৱর্ণ *name, size_t len)
{
  যদি(len == 2) {
    যদি(!memcmp(name, "h1", 2))
      ঘূৰাই ALPN_h1;
    যদি(!memcmp(name, "h2", 2))
      ঘূৰাই ALPN_h2;
    যদি(!memcmp(name, "h3", 2))
      ঘূৰাই ALPN_h3;
  }
  অন্যথা যদি(len == 8 && !memcmp(name, "http/1.1", 8))
    ঘূৰাই ALPN_h1;
  ঘূৰাই ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
স্থির timediff_t connect_বাম_ms(সংরচনা Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t বাম_ms = timeout_ms - used_ms;
  ঘূৰাই বাম_ms ? বাম_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(সংরচনা Curl_easy *data,
                                    ধ্রুৱ সংরচনা curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(সংরচনা Curl_easy *data,
                                    ধ্রুৱ সংরচনা curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  যদি(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    ঘূৰাই Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  অন্যথা যদি(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_বাম_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  অন্যথা যদি(!data->set.timeout || data->set.connect_only) {
    ঘূৰাই 0; /* no timeout in place or checked, return "no limit" */
  }

  যদি(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    যদি(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  যদি(!ctimeleft_ms)
    ঘূৰাই timeleft_ms;
  অন্যথা যদি(!timeleft_ms)
    ঘূৰাই ctimeleft_ms;
  ঘূৰাই CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(সংরচনা Curl_easy *data)
{
  ঘূৰাই timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(সংরচনা Curl_easy *data,
                                ধ্রুৱ সংরচনা curltime *pnow)
{
  ঘূৰাই timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  সংরচনা connectdata *conn = Curl_cpool_get_last_conn(data);
  যদি(connp)
    *connp = conn;
  ঘূৰাই conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

শূন্য Curl_conncontrol(সংরচনা connectdata *conn, পূর্ণাঙ্ক ctrl)
{
  যদি(!conn) {
    DEBUGASSERT(0);
    ঘূৰাই;
  }
  ৱিকল্প(ctrl) {
    পক্ষ CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      ভঙ্গ;
    পক্ষ CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      ভঙ্গ;
    পক্ষ CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      যদি(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      ভঙ্গ;
    স্ৱভাৱ:
      DEBUGASSERT(0);
      ভঙ্গ;
  }
}

CURLcode Curl_conn_setup(সংরচনা Curl_easy *data,
                         সংরচনা connectdata *conn,
                         int8_t sockindex,
                         পূর্ণাঙ্ক ssl_mode)
{
  সংরচনা Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  যদি(!first_peer)
    ঘূৰাই CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  যদি(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    যদি(result)
      গচ্ছ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  যদি(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    যদি(result)
      গচ্ছ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  যদি(result)
    গচ্ছ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  ঘূৰাই result;
}

#ifdef CURLVERBOSE
স্থির CURLcode conn_connect_trace(সংরচনা Curl_easy *data,
                                   সংরচনা Curl_cfilter *cf)
{
  যদি(Curl_trc_is_verbose(data)) {
    সংরচনা ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    যদি(result)
      ঘূৰাই result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  ঘূৰাই CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
স্থির শূন্য conn_report_stats(সংরচনা Curl_easy *data, পূর্ণাঙ্ক sockindex)
{
  /* We do gather stats for the second socket...yet */
  যদি(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

শূন্য Curl_conn_report_failed(সংরচনা Curl_easy *data, int8_t sockindex)
{
  যদি(!data->conn || (sockindex != FIRSTSOCKET))
    ঘূৰাই;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (শূন্য)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, রিক্ত);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(সংরচনা Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_অঙ্ক_POLLS_ON_স্তূপ 5
  সংরচনা pollfd a_few_on_স্তূপ[CF_CONN_অঙ্ক_POLLS_ON_স্তূপ];
  সংরচনা easy_pollset ps;
  সংরচনা curl_pollfds cpfds;
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  যদি(!cf) {
    *done = FALSE;
    ঘূৰাই CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  যদি(*done)
    ঘূৰাই CURLE_OK;

  যদি(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_স্তূপ, CF_CONN_অঙ্ক_POLLS_ON_স্তূপ);
  যেতিয়ালৈ(!*done) {
    যদি(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      যদি(result && (result != CURLE_AGAIN))
        গচ্ছ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (পূর্ণাঙ্ক)result, *done);
    যদি(!result && *done) {
      /* A final sanity check on connection security */
      যদি((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        গচ্ছ out;
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
      গচ্ছ out;
    }
    অন্যথা যদি(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (পূর্ণাঙ্ক)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      গচ্ছ out;
    }

    যদি(!blocking)
      গচ্ছ out;
    অন্যথা {
      /* check allowed time left */
      ধ্রুৱ সংরচনা curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      পূর্ণাঙ্ক rc;

      যদি(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ধ্রুৱ সংরচনা curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_বাম_ms(data,
          curlx_ptimediff_ms(pnow, started));
        যদি(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      যদি(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        গচ্ছ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      যদি(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      যদি(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      যদি(result)
        গচ্ছ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      যদি(result)
        গচ্ছ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      যদি(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        গচ্ছ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ঘূৰাই result;
}

শূন্য Curl_conn_set_multiplex(সংরচনা connectdata *conn)
{
  যদি(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    যদি(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

সংরচনা Curl_peer *Curl_conn_get_origin(সংরচনা connectdata *conn,
                                       int8_t sockindex)
{
  ঘূৰাই (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

সংরচনা Curl_peer *Curl_conn_get_destination(সংরচনা connectdata *conn,
                                            int8_t sockindex)
{
  ঘূৰাই (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

সংরচনা Curl_peer *Curl_conn_get_first_peer(সংরচনা connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer)
    ঘূৰাই conn->socks_proxy.peer;
  যদি(conn->http_proxy.peer)
    ঘূৰাই conn->http_proxy.peer;
#endif
  ঘূৰাই (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
