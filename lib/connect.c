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

गणना alpnid Curl_alpn2alpnid(ध्रुव अचिह्नित वर्ण *name, size_t len)
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

#यदि_अन्तः

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
स्थिर timediff_t connect_वामः_ms(संरचना Curl_easy *दत्त,
                                  timediff_t प्रयुक्तम्_ms)
{
  timediff_t timeout_ms = (दत्त->समुच्चयः.connecttimeout > 0) ?
    दत्त->समुच्चयः.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t वामः_ms = timeout_ms - प्रयुक्तम्_ms;
  प्रत्यागम वामः_ms ? वामः_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(संरचना Curl_easy *दत्त,
                                    ध्रुव संरचना curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(संरचना Curl_easy *दत्त,
                                    ध्रुव संरचना curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  यदि(दत्त->conn && CURL_CONN_अन्तर्_SHUTDOWN(दत्त->conn, FIRSTSOCKET))
    प्रत्यागम Curl_cshutdn_timeleft_ms(दत्त, दत्त->conn, FIRSTSOCKET);
  अन्यथा यदि(Curl_is_connecting(दत्त)) {
    ctimeleft_ms = connect_वामः_ms(दत्त,
      Curl_pgrs_since_ms(दत्त, pnow, TIMER_STARTSINGLE));
  }
  अन्यथा यदि(!दत्त->समुच्चयः.timeout || दत्त->समुच्चयः.connect_only) {
    प्रत्यागम 0; /* no timeout in place or checked, return "no limit" */
  }

  यदि(दत्त->समुच्चयः.timeout) {
    timeleft_ms = दत्त->समुच्चयः.timeout -
                  Curl_pgrs_since_ms(दत्त, pnow, TIMER_STARTOP);
    यदि(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  यदि(!ctimeleft_ms)
    प्रत्यागम timeleft_ms;
  अन्यथा यदि(!timeleft_ms)
    प्रत्यागम ctimeleft_ms;
  प्रत्यागम CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(संरचना Curl_easy *दत्त)
{
  प्रत्यागम timeleft_now_ms(दत्त, Curl_pgrs_now(दत्त));
}

timediff_t Curl_timeleft_now_ms(संरचना Curl_easy *दत्त,
                                ध्रुव संरचना curltime *pnow)
{
  प्रत्यागम timeleft_now_ms(दत्त, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(संरचना Curl_easy *दत्त,
                                  संरचना connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  संरचना connectdata *conn = Curl_cpool_get_चरमः_conn(दत्त);
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
      conn->द्वयङ्काः.close = FALSE;
      भङ्ग;
    पक्ष CONNCTRL_CONN_CLOSE:
      conn->द्वयङ्काः.close = TRUE;
      भङ्ग;
    पक्ष CONNCTRL_प्रवाहः_CLOSE:
      /* stream close when multiplexing does not affect connection */
      यदि(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->द्वयङ्काः.close = TRUE;
      भङ्ग;
    स्वभाव:
      DEBUGASSERT(0);
      भङ्ग;
  }
}

CURLcode Curl_conn_सज्जता(संरचना Curl_easy *दत्त,
                         संरचना connectdata *conn,
                         int8_t sockindex,
                         पूर्णाङ्क ssl_बहुलकम्)
{
  संरचना Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode परिणाम = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  यदि(!first_peer)
    प्रत्यागम CURLE_FAILED_INIT;

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  यदि(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_बहुलकम् != CURL_CF_SSL_DISABLE);
    परिणाम = Curl_cf_https_सज्जता(
      दत्त, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  यदि(!conn->cfilter[sockindex]) {
    परिणाम = Curl_cf_सज्जता_सङ्कलय(दत्त, conn, sockindex,
                               conn->transport_wanted, ssl_बहुलकम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(दत्त, conn->ip_संस्करणम्);
  परिणाम = Curl_conn_dns_सङ्कलय_addr_resolve(दत्त, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  DEBUGASSERT(conn->cfilter[sockindex]);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर CURLcode conn_connect_trace(संरचना Curl_easy *दत्त,
                                   संरचना Curl_cfilter *cf)
{
  यदि(Curl_trc_is_verbose(दत्त)) {
    संरचना ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode परिणाम;

    परिणाम = Curl_conn_cf_get_ip_info(cf, दत्त, &is_ipv6, &ipquad);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    infof(दत्त, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(दत्त->conn),
          ipquad.remote_ip, ipquad.remote_पत्तनम्,
          ipquad.local_ip, ipquad.local_पत्तनम्);
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

/**
 * Update connection statistics
 */
स्थिर शून्य conn_report_stats(संरचना Curl_easy *दत्त, पूर्णाङ्क sockindex)
{
  /* We do gather stats for the second socket...yet */
  यदि(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(दत्त, दत्त->conn, sockindex);
  }
}

शून्य Curl_conn_report_failed(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  यदि(!दत्त->conn || (sockindex != FIRSTSOCKET))
    प्रत्यागम;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (शून्य)Curl_conn_cf_cntrl(दत्त->conn->cfilter[sockindex], दत्त, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
  conn_report_stats(दत्त, sockindex);
}

CURLcode Curl_conn_connect(संरचना Curl_easy *दत्त,
                           int8_t sockindex,
                           bool blocking,
                           bool *समाप्तम्)
{
#परिभाषय CF_CONN_अङ्कः_POLLS_ON_राशिः 5
  संरचना pollfd a_few_on_राशिः[CF_CONN_अङ्कः_POLLS_ON_राशिः];
  संरचना easy_pollset ps;
  संरचना curl_pollfds cpfds;
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(दत्त->conn->scheme->flags & PROTOPT_NONETWORK) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  cf = दत्त->conn->cfilter[sockindex];
  यदि(!cf) {
    *समाप्तम् = FALSE;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  *समाप्तम् = (bool)cf->connected;
  यदि(*समाप्तम्)
    प्रत्यागम CURLE_OK;

  यदि(!दत्त->conn->connect_started.समुच्चयः[sockindex]) {
    दत्त->conn->connect_started.at[sockindex] = *Curl_pgrs_now(दत्त);
    दत्त->conn->connect_started.समुच्चयः[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_राशिः, CF_CONN_अङ्कः_POLLS_ON_राशिः);
  यावत्(!*समाप्तम्) {
    यदि(Curl_conn_needs_flush(दत्त, sockindex)) {
      DEBUGF(infof(दत्त, "Curl_conn_connect(index=%d), flush", sockindex));
      परिणाम = Curl_conn_flush(दत्त, sockindex);
      यदि(परिणाम && (परिणाम != CURLE_AGAIN))
        गच्छ बहिर्गमनम्;
    }

    परिणाम = cf->cft->do_connect(cf, दत्त, समाप्तम्);
    CURL_TRC_CF(दत्त, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (पूर्णाङ्क)परिणाम, *समाप्तम्);
    यदि(!परिणाम && *समाप्तम्) {
      /* A final sanity check on connection security */
      यदि((दत्त->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(दत्त, "transfer requires SSL, but not connected via SSL");
        परिणाम = CURLE_FAILED_INIT;
        गच्छ बहिर्गमनम्;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(दत्त, दत्त->conn);
      conn_report_stats(दत्त, sockindex);
      Curl_cpool_conn_was_प्रयुक्तम्(दत्त, दत्त->conn, Curl_pgrs_now(दत्त));
      VERBOSE(परिणाम = conn_connect_trace(दत्त, cf));
      VERBOSE(Curl_conn_trc_filters(दत्त, sockindex, "connected"));
      Curl_conn_remove_सज्जता_filters(दत्त, sockindex);
      VERBOSE(Curl_conn_trc_filters(दत्त, sockindex, "reduced to"));
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "Curl_conn_connect(), filter returned %d",
                  (पूर्णाङ्क)परिणाम);
      VERBOSE(Curl_conn_trc_filters(दत्त, sockindex, "failed to connect"));
      Curl_conn_report_failed(दत्त, sockindex);
      गच्छ बहिर्गमनम्;
    }

    यदि(!blocking)
      गच्छ बहिर्गमनम्;
    अन्यथा {
      /* check allowed time left */
      ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);
      timediff_t timeout_ms = Curl_timeleft_now_ms(दत्त, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, दत्त);
      पूर्णाङ्क rc;

      यदि(!Curl_is_connecting(दत्त)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ध्रुव संरचना curltime *started =
          &दत्त->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_वामः_ms(दत्त,
          curlx_ptimediff_ms(pnow, started));
        यदि(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      यदि(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(दत्त, "connect timeout");
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        गच्छ बहिर्गमनम्;
      }

      CURL_TRC_CF(दत्त, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      यदि(sockfd != CURL_SOCKET_BAD)
        परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, &ps, sockfd);
      यदि(!परिणाम)
        परिणाम = Curl_conn_adjust_pollset(दत्त, दत्त->conn, &ps);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      परिणाम = Curl_pollfds_सङ्कलय_ps(&cpfds, &ps);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(दत्त, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      यदि(rc < 0) {
        परिणाम = CURLE_COULDNT_CONNECT;
        गच्छ बहिर्गमनम्;
      }
      /* continue iterating */
    }
  }

बहिर्गमनम्:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  प्रत्यागम परिणाम;
}

शून्य Curl_conn_समुच्चयः_multiplex(संरचना connectdata *conn)
{
  यदि(!conn->द्वयङ्काः.multiplex) {
    conn->द्वयङ्काः.multiplex = TRUE;
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
    (conn->द्वारा_peer2 ? conn->द्वारा_peer2 : conn->origin2) :
    (conn->द्वारा_peer ? conn->द्वारा_peer : conn->origin);
}

संरचना Curl_peer *Curl_conn_get_first_peer(संरचना connectdata *conn,
                                           int8_t sockindex)
{
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->socks_proxy.peer)
    प्रत्यागम conn->socks_proxy.peer;
  यदि(conn->http_proxy.peer)
    प्रत्यागम conn->http_proxy.peer;
#यदि_अन्तः
  प्रत्यागम (sockindex == SECONDARYSOCKET) ?
    (conn->द्वारा_peer2 ? conn->द्वारा_peer2 : conn->origin2) :
    (conn->द्वारा_peer ? conn->द्वारा_peer : conn->origin);
}
