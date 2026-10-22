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

#सचे !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

गणना alpnid Curl_alpn2alpnid(ध्रुव unsigned वर्ण *name, size_t len)
{
  सचे(len == 2) {
    सचे(!memcmp(name, "h1", 2))
      पटिनिवत्त ALPN_h1;
    सचे(!memcmp(name, "h2", 2))
      पटिनिवत्त ALPN_h2;
    सचे(!memcmp(name, "h3", 2))
      पटिनिवत्त ALPN_h3;
  }
  अञ्ञथा सचे(len == 8 && !memcmp(name, "http/1.1", 8))
    पटिनिवत्त ALPN_h1;
  पटिनिवत्त ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
स्थिर timediff_t connect_वामः_ms(संरचना Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t वामः_ms = timeout_ms - used_ms;
  पटिनिवत्त वामः_ms ? वामः_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
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

  सचे(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    पटिनिवत्त Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  अञ्ञथा सचे(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_वामः_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  अञ्ञथा सचे(!data->set.timeout || data->set.connect_only) {
    पटिनिवत्त 0; /* no timeout in place or checked, return "no limit" */
  }

  सचे(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    सचे(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  सचे(!ctimeleft_ms)
    पटिनिवत्त timeleft_ms;
  अञ्ञथा सचे(!timeleft_ms)
    पटिनिवत्त ctimeleft_ms;
  पटिनिवत्त CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(संरचना Curl_easy *data)
{
  पटिनिवत्त timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(संरचना Curl_easy *data,
                                ध्रुव संरचना curltime *pnow)
{
  पटिनिवत्त timeleft_now_ms(data, pnow);
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
  सचे(connp)
    *connp = conn;
  पटिनिवत्त conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

शून्य Curl_conncontrol(संरचना connectdata *conn, पूर्णाङ्क ctrl)
{
  सचे(!conn) {
    DEBUGASSERT(0);
    पटिनिवत्त;
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
      सचे(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
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

  सचे(!first_peer)
    पटिनिवत्त CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  सचे(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    सचे(result)
      गच्छ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  सचे(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    सचे(result)
      गच्छ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  सचे(result)
    गच्छ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  पटिनिवत्त result;
}

#ifdef CURLVERBOSE
स्थिर CURLcode conn_connect_trace(संरचना Curl_easy *data,
                                   संरचना Curl_cfilter *cf)
{
  सचे(Curl_trc_is_verbose(data)) {
    संरचना ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    सचे(result)
      पटिनिवत्त result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  पटिनिवत्त CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
स्थिर शून्य conn_report_stats(संरचना Curl_easy *data, पूर्णाङ्क sockindex)
{
  /* We do gather stats for the second socket...yet */
  सचे(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

शून्य Curl_conn_report_failed(संरचना Curl_easy *data, int8_t sockindex)
{
  सचे(!data->conn || (sockindex != FIRSTSOCKET))
    पटिनिवत्त;
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
#define CF_CONN_अङ्कः_POLLS_ON_राशिः 5
  संरचना pollfd a_few_on_राशिः[CF_CONN_अङ्कः_POLLS_ON_राशिः];
  संरचना easy_pollset ps;
  संरचना curl_pollfds cpfds;
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  सचे(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  सचे(!cf) {
    *done = FALSE;
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  सचे(*done)
    पटिनिवत्त CURLE_OK;

  सचे(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_राशिः, CF_CONN_अङ्कः_POLLS_ON_राशिः);
  याव(!*done) {
    सचे(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      सचे(result && (result != CURLE_AGAIN))
        गच्छ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (पूर्णाङ्क)result, *done);
    सचे(!result && *done) {
      /* A final sanity check on connection security */
      सचे((data->state.origin->scheme->flags & PROTOPT_SSL) &&
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
    अञ्ञथा सचे(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (पूर्णाङ्क)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      गच्छ out;
    }

    सचे(!blocking)
      गच्छ out;
    अञ्ञथा {
      /* check allowed time left */
      ध्रुव संरचना curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      पूर्णाङ्क rc;

      सचे(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ध्रुव संरचना curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_वामः_ms(data,
          curlx_ptimediff_ms(pnow, started));
        सचे(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      सचे(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        गच्छ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      सचे(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      सचे(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      सचे(result)
        गच्छ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      सचे(result)
        गच्छ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      सचे(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  पटिनिवत्त result;
}

शून्य Curl_conn_set_multiplex(संरचना connectdata *conn)
{
  सचे(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    सचे(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

संरचना Curl_peer *Curl_conn_get_origin(संरचना connectdata *conn,
                                       int8_t sockindex)
{
  पटिनिवत्त (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

संरचना Curl_peer *Curl_conn_get_destination(संरचना connectdata *conn,
                                            int8_t sockindex)
{
  पटिनिवत्त (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

संरचना Curl_peer *Curl_conn_get_first_peer(संरचना connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  सचे(conn->socks_proxy.peer)
    पटिनिवत्त conn->socks_proxy.peer;
  सचे(conn->http_proxy.peer)
    पटिनिवत्त conn->http_proxy.peer;
#endif
  पटिनिवत्त (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
