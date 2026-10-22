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

#ಆದರೆ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ಎಣಿಕೆ alpnid Curl_alpn2alpnid(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *name, size_t len)
{
  ಆದರೆ(len == 2) {
    ಆದರೆ(!memcmp(name, "h1", 2))
      ಹಿಂತಿರುಗಿ ALPN_h1;
    ಆದರೆ(!memcmp(name, "h2", 2))
      ಹಿಂತಿರುಗಿ ALPN_h2;
    ಆದರೆ(!memcmp(name, "h3", 2))
      ಹಿಂತಿರುಗಿ ALPN_h3;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 8 && !memcmp(name, "http/1.1", 8))
    ಹಿಂತಿರುಗಿ ALPN_h1;
  ಹಿಂತಿರುಗಿ ALPN_none; /* unknown, probably rubbish input */
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ಸ್ಥಿರ timediff_t connect_ಎಡ_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  timediff_t ಬಳಸಿದ_ms)
{
  timediff_t timeout_ms = (ದತ್ತಾಂಶ->ಗಣ.connecttimeout > 0) ?
    ದತ್ತಾಂಶ->ಗಣ.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ಎಡ_ms = timeout_ms - ಬಳಸಿದ_ms;
  ಹಿಂತಿರುಗಿ ಎಡ_ms ? ಎಡ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ಆದರೆ(ದತ್ತಾಂಶ->conn && CURL_CONN_ಒಳ_SHUTDOWN(ದತ್ತಾಂಶ->conn, FIRSTSOCKET))
    ಹಿಂತಿರುಗಿ Curl_cshutdn_timeleft_ms(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, FIRSTSOCKET);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_is_connecting(ದತ್ತಾಂಶ)) {
    ctimeleft_ms = connect_ಎಡ_ms(ದತ್ತಾಂಶ,
      Curl_pgrs_since_ms(ದತ್ತಾಂಶ, pnow, TIMER_STARTSINGLE));
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.timeout || ದತ್ತಾಂಶ->ಗಣ.connect_only) {
    ಹಿಂತಿರುಗಿ 0; /* no timeout in place or checked, return "no limit" */
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timeout) {
    timeleft_ms = ದತ್ತಾಂಶ->ಗಣ.timeout -
                  Curl_pgrs_since_ms(ದತ್ತಾಂಶ, pnow, TIMER_STARTOP);
    ಆದರೆ(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ಆದರೆ(!ctimeleft_ms)
    ಹಿಂತಿರುಗಿ timeleft_ms;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!timeleft_ms)
    ಹಿಂತಿರುಗಿ ctimeleft_ms;
  ಹಿಂತಿರುಗಿ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ timeleft_now_ms(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ));
}

timediff_t Curl_timeleft_now_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ಹಿಂತಿರುಗಿ timeleft_now_ms(ದತ್ತಾಂಶ, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ರಚನೆ connectdata *conn = Curl_cpool_get_ಕೊನೆಯ_conn(ದತ್ತಾಂಶ);
  ಆದರೆ(connp)
    *connp = conn;
  ಹಿಂತಿರುಗಿ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

ಶೂನ್ಯ Curl_conncontrol(ರಚನೆ connectdata *conn, ಪೂರ್ಣಾಂಕ ctrl)
{
  ಆದರೆ(!conn) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }
  ಆಯ್ಕೆ(ctrl) {
    ಸ್ಥಿತಿ CONNCTRL_CONN_KEEP:
      conn->ಬಿಟ್ಟುಗಳು.close = FALSE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CONNCTRL_CONN_CLOSE:
      conn->ಬಿಟ್ಟುಗಳು.close = TRUE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CONNCTRL_ಹರಿವು_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ಆದರೆ(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->ಬಿಟ್ಟುಗಳು.close = TRUE;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      DEBUGASSERT(0);
      ನಿಲ್ಲಿಸು;
  }
}

CURLcode Curl_conn_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ connectdata *conn,
                         int8_t sockindex,
                         ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ)
{
  ರಚನೆ Curl_peer *ಮೊದಲ_peer = Curl_conn_get_ಮೊದಲ_peer(conn, sockindex);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ಆದರೆ(!ಮೊದಲ_peer)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಆದರೆ(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_ಬಹುಲಕ != CURL_CF_SSL_DISABLE);
    ಫಲಿತಾಂಶ = Curl_cf_https_ಸಿದ್ಧತೆ(
      ದತ್ತಾಂಶ, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ಆದರೆ(!conn->cfilter[sockindex]) {
    ಫಲಿತಾಂಶ = Curl_cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex,
                               conn->transport_wanted, ssl_ಬಹುಲಕ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(ದತ್ತಾಂಶ, conn->ip_ಆವೃತ್ತಿ);
  ಫಲಿತಾಂಶ = Curl_conn_dns_ಕೂಡಿಸು_addr_resolve(ದತ್ತಾಂಶ, conn, sockindex,
                                          ಮೊದಲ_peer, dns_queries,
                                          conn->transport_wanted);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  DEBUGASSERT(conn->cfilter[sockindex]);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ CURLcode conn_connect_trace(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_cfilter *cf)
{
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    ರಚನೆ ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = Curl_conn_cf_get_ip_info(cf, ದತ್ತಾಂಶ, &is_ipv6, &ipquad);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    infof(ದತ್ತಾಂಶ, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(ದತ್ತಾಂಶ->conn),
          ipquad.remote_ip, ipquad.remote_ಪೋರ್ಟ್,
          ipquad.local_ip, ipquad.local_ಪೋರ್ಟ್);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Update connection statistics
 */
ಸ್ಥಿರ ಶೂನ್ಯ conn_report_stats(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ sockindex)
{
  /* We do gather stats for the second socket...yet */
  ಆದರೆ(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, sockindex);
  }
}

ಶೂನ್ಯ Curl_conn_report_failed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->conn || (sockindex != FIRSTSOCKET))
    ಹಿಂತಿರುಗಿ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (ಶೂನ್ಯ)Curl_conn_cf_cntrl(ದತ್ತಾಂಶ->conn->cfilter[sockindex], ದತ್ತಾಂಶ, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ಖಾಲಿ);
  conn_report_stats(ದತ್ತಾಂಶ, sockindex);
}

CURLcode Curl_conn_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           int8_t sockindex,
                           bool blocking,
                           bool *ಮುಗಿದ)
{
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ 5
  ರಚನೆ pollfd a_few_on_ರಾಶಿ[CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ರಚನೆ easy_pollset ps;
  ರಚನೆ curl_pollfds cpfds;
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_NONETWORK) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆದರೆ(!cf) {
    *ಮುಗಿದ = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  *ಮುಗಿದ = (bool)cf->connected;
  ಆದರೆ(*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn->connect_started.ಗಣ[sockindex]) {
    ದತ್ತಾಂಶ->conn->connect_started.at[sockindex] = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    ದತ್ತಾಂಶ->conn->connect_started.ಗಣ[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, CF_CONN_ಅಂಕೆ_POLLS_ON_ರಾಶಿ);
  ಆಗ(!*ಮುಗಿದ) {
    ಆದರೆ(Curl_conn_needs_flush(ದತ್ತಾಂಶ, sockindex)) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_conn_connect(index=%d), flush", sockindex));
      ಫಲಿತಾಂಶ = Curl_conn_flush(ದತ್ತಾಂಶ, sockindex);
      ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
        ಹೋಗು ಹೊರಹೋಗು;
    }

    ಫಲಿತಾಂಶ = cf->cft->do_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
    ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
      /* A final sanity check on connection security */
      ಆದರೆ((ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(ದತ್ತಾಂಶ, "transfer requires SSL, but not connected via SSL");
        ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
      conn_report_stats(ದತ್ತಾಂಶ, sockindex);
      Curl_cpool_conn_was_ಬಳಸಿದ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, Curl_pgrs_now(ದತ್ತಾಂಶ));
      VERBOSE(ಫಲಿತಾಂಶ = conn_connect_trace(ದತ್ತಾಂಶ, cf));
      VERBOSE(Curl_conn_trc_filters(ದತ್ತಾಂಶ, sockindex, "connected"));
      Curl_conn_remove_ಸಿದ್ಧತೆ_filters(ದತ್ತಾಂಶ, sockindex);
      VERBOSE(Curl_conn_trc_filters(ದತ್ತಾಂಶ, sockindex, "reduced to"));
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Curl_conn_connect(), filter returned %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      VERBOSE(Curl_conn_trc_filters(ದತ್ತಾಂಶ, sockindex, "failed to connect"));
      Curl_conn_report_failed(ದತ್ತಾಂಶ, sockindex);
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(!blocking)
      ಹೋಗು ಹೊರಹೋಗು;
    ಇಲ್ಲದಿದ್ದರೆ {
      /* check allowed time left */
      ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
      timediff_t timeout_ms = Curl_timeleft_now_ms(ದತ್ತಾಂಶ, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
      ಪೂರ್ಣಾಂಕ rc;

      ಆದರೆ(!Curl_is_connecting(ದತ್ತಾಂಶ)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *started =
          &ದತ್ತಾಂಶ->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ಎಡ_ms(ದತ್ತಾಂಶ,
          curlx_ptimediff_ms(pnow, started));
        ಆದರೆ(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ಆದರೆ(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(ದತ್ತಾಂಶ, "connect timeout");
        ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
        ಹೋಗು ಹೊರಹೋಗು;
      }

      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ಆದರೆ(sockfd != CURL_SOCKET_BAD)
        ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, &ps, sockfd);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, &ps);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಫಲಿತಾಂಶ = Curl_pollfds_ಕೂಡಿಸು_ps(&cpfds, &ps);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ಆದರೆ(rc < 0) {
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* continue iterating */
    }
  }

ಹೊರಹೋಗು:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_conn_ಗಣ_multiplex(ರಚನೆ connectdata *conn)
{
  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.multiplex) {
    conn->ಬಿಟ್ಟುಗಳು.multiplex = TRUE;
    ಆದರೆ(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ರಚನೆ Curl_peer *Curl_conn_get_origin(ರಚನೆ connectdata *conn,
                                       int8_t sockindex)
{
  ಹಿಂತಿರುಗಿ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ರಚನೆ Curl_peer *Curl_conn_get_destination(ರಚನೆ connectdata *conn,
                                            int8_t sockindex)
{
  ಹಿಂತಿರುಗಿ (sockindex == SECONDARYSOCKET) ?
    (conn->ಮೂಲಕ_peer2 ? conn->ಮೂಲಕ_peer2 : conn->origin2) :
    (conn->ಮೂಲಕ_peer ? conn->ಮೂಲಕ_peer : conn->origin);
}

ರಚನೆ Curl_peer *Curl_conn_get_ಮೊದಲ_peer(ರಚನೆ connectdata *conn,
                                           int8_t sockindex)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->socks_proxy.peer)
    ಹಿಂತಿರುಗಿ conn->socks_proxy.peer;
  ಆದರೆ(conn->http_proxy.peer)
    ಹಿಂತಿರುಗಿ conn->http_proxy.peer;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (sockindex == SECONDARYSOCKET) ?
    (conn->ಮೂಲಕ_peer2 ? conn->ಮೂಲಕ_peer2 : conn->origin2) :
    (conn->ಮೂಲಕ_peer ? conn->ಮೂಲಕ_peer : conn->origin);
}
