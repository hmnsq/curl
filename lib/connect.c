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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "cf-https-connect.h"
#अन्तर्भाव "cf-setup.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "conncache.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "curlx/strparse.h"

#यदि !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

गणना alpnid Curl_alpn2alpnid(ध्रुव unsigned वर्ण *name, size_t len)
{
  यदि(len == 2) {
    यदि(!memcmp(name, "h1", 2))
      प्रत्यागम ALPN_h1;
    यदि(!memcmp(name, "h2", 2))
      प्रत्यागम ALPN_h2;
    यदि(!memcmp(name, "h3", 2))
      प्रत्यागम ALPN_h3;
  }
  अन्यथा यदि(len == 8 && !memcmp(name, "http/1.1", 8))
    प्रत्यागम ALPN_h1;
  प्रत्यागम ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
स्थिर timediff_t connect_बायाँ_ms(संरचना Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t बायाँ_ms = timeout_ms - used_ms;
  प्रत्यागम बायाँ_ms ? बायाँ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(संरचना Curl_easy *data,
                                    ध्रुव संरचना curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(संरचना Curl_easy *data,
                                    ध्रुव संरचना curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  यदि(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    प्रत्यागम Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  अन्यथा यदि(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_बायाँ_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  अन्यथा यदि(!data->set.timeout || data->set.connect_only) {
    प्रत्यागम 0; /* no timeout in place or checked, return "no limit" */
  }

  यदि(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    यदि(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  यदि(!ctimeleft_ms)
    प्रत्यागम timeleft_ms;
  अन्यथा यदि(!timeleft_ms)
    प्रत्यागम ctimeleft_ms;
  प्रत्यागम CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(संरचना Curl_easy *data)
{
  प्रत्यागम timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(संरचना Curl_easy *data,
                                ध्रुव संरचना curltime *pnow)
{
  प्रत्यागम timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(संरचना Curl_easy *data,
                                  संरचना connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  संरचना connectdata *conn = Curl_cpool_get_last_conn(data);
  यदि(connp)
    *connp = conn;
  प्रत्यागम conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

शून्य Curl_conncontrol(संरचना connectdata *conn, पूर्णाङ्क ctrl)
{
  यदि(!conn) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }
  विकल्प(ctrl) {
    पक्ष CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      भङ्ग;
    पक्ष CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      भङ्ग;
    पक्ष CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      यदि(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      भङ्ग;
    स्वभाव:
      DEBUGASSERT(0);
      भङ्ग;
  }
}

CURLcode Curl_conn_setup(संरचना Curl_easy *data,
                         संरचना connectdata *conn,
                         int8_t sockindex,
                         पूर्णाङ्क ssl_mode)
{
  संरचना Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  यदि(!first_peer)
    प्रत्यागम CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  यदि(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    यदि(result)
      गच्छ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  यदि(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    यदि(result)
      गच्छ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  यदि(result)
    गच्छ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  प्रत्यागम result;
}

#ifdef CURLVERBOSE
स्थिर CURLcode conn_connect_trace(संरचना Curl_easy *data,
                                   संरचना Curl_cfilter *cf)
{
  यदि(Curl_trc_is_verbose(data)) {
    संरचना ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    यदि(result)
      प्रत्यागम result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  प्रत्यागम CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
स्थिर शून्य conn_report_stats(संरचना Curl_easy *data, पूर्णाङ्क sockindex)
{
  /* We do gather stats for the second socket...yet */
  यदि(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

शून्य Curl_conn_report_failed(संरचना Curl_easy *data, int8_t sockindex)
{
  यदि(!data->conn || (sockindex != FIRSTSOCKET))
    प्रत्यागम;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (शून्य)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(संरचना Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_अंक_POLLS_ON_ढेर 5
  संरचना pollfd a_few_on_ढेर[CF_CONN_अंक_POLLS_ON_ढेर];
  संरचना easy_pollset ps;
  संरचना curl_pollfds cpfds;
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  यदि(!cf) {
    *done = FALSE;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  यदि(*done)
    प्रत्यागम CURLE_OK;

  यदि(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ढेर, CF_CONN_अंक_POLLS_ON_ढेर);
  यावत्(!*done) {
    यदि(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      यदि(result && (result != CURLE_AGAIN))
        गच्छ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (पूर्णाङ्क)result, *done);
    यदि(!result && *done) {
      /* A final sanity check on connection security */
      यदि((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        गच्छ out;
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
      गच्छ out;
    }
    अन्यथा यदि(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (पूर्णाङ्क)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      गच्छ out;
    }

    यदि(!blocking)
      गच्छ out;
    अन्यथा {
      /* check allowed time left */
      ध्रुव संरचना curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      पूर्णाङ्क rc;

      यदि(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ध्रुव संरचना curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_बायाँ_ms(data,
          curlx_ptimediff_ms(pnow, started));
        यदि(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      यदि(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        गच्छ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      यदि(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      यदि(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      यदि(result)
        गच्छ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      यदि(result)
        गच्छ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      यदि(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  प्रत्यागम result;
}

शून्य Curl_conn_set_multiplex(संरचना connectdata *conn)
{
  यदि(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    यदि(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

संरचना Curl_peer *Curl_conn_get_origin(संरचना connectdata *conn,
                                       int8_t sockindex)
{
  प्रत्यागम (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

संरचना Curl_peer *Curl_conn_get_destination(संरचना connectdata *conn,
                                            int8_t sockindex)
{
  प्रत्यागम (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

संरचना Curl_peer *Curl_conn_get_first_peer(संरचना connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  यदि(conn->socks_proxy.peer)
    प्रत्यागम conn->socks_proxy.peer;
  यदि(conn->http_proxy.peer)
    प्रत्यागम conn->http_proxy.peer;
#endif
  प्रत्यागम (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
