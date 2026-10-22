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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strerror.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "cf-https-connect.h"
#અન્તર્ભાવ "cf-setup.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "conncache.h"
#અન્તર્ભાવ "multihandle.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "curlx/strparse.h"

#જો !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ગણના alpnid Curl_alpn2alpnid(અચલ unsigned અક્ષર *name, size_t len)
{
  જો(len == 2) {
    જો(!memcmp(name, "h1", 2))
      પાછા ALPN_h1;
    જો(!memcmp(name, "h2", 2))
      પાછા ALPN_h2;
    જો(!memcmp(name, "h3", 2))
      પાછા ALPN_h3;
  }
  નહીંતર જો(len == 8 && !memcmp(name, "http/1.1", 8))
    પાછા ALPN_h1;
  પાછા ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
સ્થિર timediff_t connect_ડાબું_ms(સંરચના Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ડાબું_ms = timeout_ms - used_ms;
  પાછા ડાબું_ms ? ડાબું_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(સંરચના Curl_easy *data,
                                    અચલ સંરચના curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(સંરચના Curl_easy *data,
                                    અચલ સંરચના curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  જો(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    પાછા Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  નહીંતર જો(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_ડાબું_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  નહીંતર જો(!data->set.timeout || data->set.connect_only) {
    પાછા 0; /* no timeout in place or checked, return "no limit" */
  }

  જો(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    જો(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  જો(!ctimeleft_ms)
    પાછા timeleft_ms;
  નહીંતર જો(!timeleft_ms)
    પાછા ctimeleft_ms;
  પાછા CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(સંરચના Curl_easy *data)
{
  પાછા timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(સંરચના Curl_easy *data,
                                અચલ સંરચના curltime *pnow)
{
  પાછા timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(સંરચના Curl_easy *data,
                                  સંરચના connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  સંરચના connectdata *conn = Curl_cpool_get_last_conn(data);
  જો(connp)
    *connp = conn;
  પાછા conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

શૂન્ય Curl_conncontrol(સંરચના connectdata *conn, પૂર્ણાંક ctrl)
{
  જો(!conn) {
    DEBUGASSERT(0);
    પાછા;
  }
  પસંદગી(ctrl) {
    સ્થિતિ CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      તોડો;
    સ્થિતિ CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      તોડો;
    સ્થિતિ CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      જો(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      તોડો;
    મૂળભૂત:
      DEBUGASSERT(0);
      તોડો;
  }
}

CURLcode Curl_conn_setup(સંરચના Curl_easy *data,
                         સંરચના connectdata *conn,
                         int8_t sockindex,
                         પૂર્ણાંક ssl_mode)
{
  સંરચના Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  જો(!first_peer)
    પાછા CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  જો(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    જો(result)
      જાઓ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  જો(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    જો(result)
      જાઓ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  જો(result)
    જાઓ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  પાછા result;
}

#ifdef CURLVERBOSE
સ્થિર CURLcode conn_connect_trace(સંરચના Curl_easy *data,
                                   સંરચના Curl_cfilter *cf)
{
  જો(Curl_trc_is_verbose(data)) {
    સંરચના ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    જો(result)
      પાછા result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  પાછા CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
સ્થિર શૂન્ય conn_report_stats(સંરચના Curl_easy *data, પૂર્ણાંક sockindex)
{
  /* We do gather stats for the second socket...yet */
  જો(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

શૂન્ય Curl_conn_report_failed(સંરચના Curl_easy *data, int8_t sockindex)
{
  જો(!data->conn || (sockindex != FIRSTSOCKET))
    પાછા;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (શૂન્ય)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ખાલી);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(સંરચના Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_અંક_POLLS_ON_ઢગલો 5
  સંરચના pollfd a_few_on_ઢગલો[CF_CONN_અંક_POLLS_ON_ઢગલો];
  સંરચના easy_pollset ps;
  સંરચના curl_pollfds cpfds;
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  જો(!cf) {
    *done = FALSE;
    પાછા CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  જો(*done)
    પાછા CURLE_OK;

  જો(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ઢગલો, CF_CONN_અંક_POLLS_ON_ઢગલો);
  જ્યારે(!*done) {
    જો(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      જો(result && (result != CURLE_AGAIN))
        જાઓ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (પૂર્ણાંક)result, *done);
    જો(!result && *done) {
      /* A final sanity check on connection security */
      જો((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        જાઓ out;
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
      જાઓ out;
    }
    નહીંતર જો(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (પૂર્ણાંક)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      જાઓ out;
    }

    જો(!blocking)
      જાઓ out;
    નહીંતર {
      /* check allowed time left */
      અચલ સંરચના curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      પૂર્ણાંક rc;

      જો(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        અચલ સંરચના curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ડાબું_ms(data,
          curlx_ptimediff_ms(pnow, started));
        જો(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      જો(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        જાઓ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      જો(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      જો(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      જો(result)
        જાઓ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      જો(result)
        જાઓ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      જો(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        જાઓ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  પાછા result;
}

શૂન્ય Curl_conn_set_multiplex(સંરચના connectdata *conn)
{
  જો(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    જો(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

સંરચના Curl_peer *Curl_conn_get_origin(સંરચના connectdata *conn,
                                       int8_t sockindex)
{
  પાછા (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

સંરચના Curl_peer *Curl_conn_get_destination(સંરચના connectdata *conn,
                                            int8_t sockindex)
{
  પાછા (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

સંરચના Curl_peer *Curl_conn_get_first_peer(સંરચના connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  જો(conn->socks_proxy.peer)
    પાછા conn->socks_proxy.peer;
  જો(conn->http_proxy.peer)
    પાછા conn->http_proxy.peer;
#endif
  પાછા (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
