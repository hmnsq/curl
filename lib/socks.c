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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "socks.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
గణన socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_ప్రారంభం,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_ప్రారంభం,
  SOCKS5_ST_REQ0_SEND,
  SOCKS5_ST_RESP0_RECV, /* set up read */
  SOCKS5_ST_GSSAPI_INIT,
  SOCKS5_ST_AUTH_INIT, /* setup outgoing auth buffer */
  SOCKS5_ST_AUTH_SEND, /* send auth */
  SOCKS5_ST_AUTH_RECV, /* read auth response */
  SOCKS5_ST_REQ1_INIT,  /* init SOCKS "request" */
  SOCKS5_ST_RESOLVING,
  SOCKS5_ST_REQ1_SEND,
  SOCKS5_ST_RESP1_RECV,
  /* Terminal states, all SOCKS versions */
  SOCKS_ST_SUCCESS,
  SOCKS_ST_FAILED
};

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి cf_socks_statename[] = {
  "SOCKS_INIT",
  "SOCKS4_START",
  "SOCKS4_RESOLVING",
  "SOCKS4_SEND",
  "SOCKS4_RECV",
  "SOCKS5_START",
  "SOCKS5_REQ0_SEND",
  "SOCKS5_RESP0_RECV",
  "SOCKS5_GSSAPI_INIT",
  "SOCKS5_AUTH_INIT",
  "SOCKS5_AUTH_SEND",
  "SOCKS5_AUTH_RECV",
  "SOCKS5_REQ1_INIT",
  "SOCKS5_RESOLVING",
  "SOCKS5_REQ1_SEND",
  "SOCKS5_RESP1_RECV",
  "SOCKS_SUCCESS",
  "SOCKS_FAILED"
};
#అయితే_ముగింపు

#నిర్వచించు SOCKS_CHUNK_కొలత    1024
#నిర్వచించు SOCKS_CHUNKS        1

నిర్మాణం socks_ctx {
  గణన socks_state_t state;
  నిర్మాణం bufq iobuf;
  నిర్మాణం Curl_peer *dest;
  నిర్మాణం Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_సంస్కరణ;
  uint8_t proxy_type;
  గుర్తులేని అక్షరం సంస్కరణ_2;
  బిట్టు(resolve_local);
  బిట్టు(socks4a);
};

#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            అక్షరం *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode ఫలితం;

  *pnread = 0;
  కొరకు(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(దత్తాంశం);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);

    ఒకవేళ(timeout_ms < 0) {
      /* we already got the timeout */
      తిరిగి CURLE_OPERATION_TIMEDOUT;
    }
    ఒకవేళ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ఒకవేళ(SOCKET_READABLE(sock, timeout_ms) <= 0)
      తిరిగి CURLE_OPERATION_TIMEDOUT;
    ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం, buf, blen, &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      కొనసాగు;
    లేకపోతే ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ(blen == nread) {
      *pnread += nread;
      తిరిగి CURLE_OK;
    }
    ఒకవేళ(!nread) /* EOF */
      తిరిగి CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#అయితే_ముగింపు

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#నిర్వచించు sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#లేకపోతే
#నిర్వచించు sxstate(x, c, d, y) socksstate(x, c, d, y)
#అయితే_ముగింపు

/* always use this function to change state, to make debugging easier */
స్థిర శూన్య socksstate(నిర్మాణం socks_ctx *sx,
                       నిర్మాణం Curl_cfilter *cf,
                       నిర్మాణం Curl_easy *దత్తాంశం,
                       గణన socks_state_t state
#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , పూర్ణాంకం lineno
#అయితే_ముగింపు
)
{
  గణన socks_state_t oldstate = sx->state;

  ఒకవేళ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    తిరిగి;

  sx->state = state;

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(దత్తాంశం, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#లేకపోతే
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
}

స్థిర CURLproxycode socks_failed(నిర్మాణం socks_ctx *sx,
                                  నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, దత్తాంశం, SOCKS_ST_FAILED);
  sx->presult = presult;
  తిరిగి presult;
}

స్థిర CURLproxycode socks_flush(నిర్మాణం socks_ctx *sx,
                                 నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool *పూర్తి)
{
  CURLcode ఫలితం;
  size_t nwritten;

  *పూర్తి = FALSE;
  అయితే(!Curl_bufq_is_ఖాళీలు(&sx->iobuf)) {
    ఫలితం = Curl_cf_send_bufq(cf->తదుపరి, దత్తాంశం, &sx->iobuf, ఖాళీ, 0,
                               &nwritten);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      తిరిగి CURLPX_OK;
    లేకపోతే ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Failed to send SOCKS request: %s",
            curl_easy_strerror(ఫలితం));
      తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_SEND_CONNECT);
    }
  }
  *పూర్తి = TRUE;
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks_recv(నిర్మాణం socks_ctx *sx,
                                నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                size_t min_బైట్లు,
                                bool *పూర్తి)
{
  CURLcode ఫలితం;
  size_t nread;

  *పూర్తి = FALSE;
  అయితే(Curl_bufq_len(&sx->iobuf) < min_బైట్లు) {
    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &sx->iobuf,
                               min_బైట్లు - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      తిరిగి CURLPX_OK;
    లేకపోతే ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(ఫలితం));
      తిరిగి CURLPX_RECV_CONNECT;
    }
    లేకపోతే ఒకవేళ(!nread) { /* EOF */
      ఒకవేళ(Curl_bufq_len(&sx->iobuf) < min_బైట్లు) {
        failf(దత్తాంశం, "Failed to receive SOCKS response, "
              "proxy closed connection");
        తిరిగి CURLPX_RECV_CONNECT;
      }
      ఆపు;
    }
  }
  *పూర్తి = TRUE;
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks4_req_కూడు_hd(నిర్మాణం socks_ctx *sx,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  గుర్తులేని అక్షరం buf[4];
  size_t nwritten;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (గుర్తులేని అక్షరం)((sx->dest->పోర్టు >> 8) & 0xffU); /* MSB */
  buf[3] = (గుర్తులేని అక్షరం)(sx->dest->పోర్టు & 0xffU);        /* LSB */

  ఫలితం = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 4))
    తిరిగి CURLPX_SEND_REQUEST;
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks4_req_కూడు_user(నిర్మాణం socks_ctx *sx,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  size_t nwritten;

  ఒకవేళ(sx->creds) {
    size_t plen = అక్షరశ్రేణి_పొడవు(sx->creds->user);
    ఒకవేళ(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(దత్తాంశం, "Too long SOCKS proxy username");
      తిరిగి CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    ఫలితం = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != (plen + 1)))
      తిరిగి CURLPX_SEND_REQUEST;
  }
  లేకపోతే {
    /* empty username */
    గుర్తులేని అక్షరం b = 0;
    ఫలితం = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != 1))
      తిరిగి CURLPX_SEND_REQUEST;
  }
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks4_resolving(నిర్మాణం socks_ctx *sx,
                                      నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *పూర్తి)
{
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai = ఖాళీ;
  CURLcode ఫలితం;
  size_t nwritten;

  *పూర్తి = FALSE;
  ఫలితం = Curl_conn_dns_addr_ఫలితం(cf->conn, cf->sockindex, sx->dest);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_AGAIN) {
      failf(దత్తాంశం, "error %d resolving SOCKS destination %s:%u",
            (పూర్ణాంకం)ఫలితం, sx->dest->hostname, sx->dest->పోర్టు);
      తిరిగి CURLPX_RESOLVE_HOST;
    }
    తిరిగి CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(దత్తాంశం, sx->dest, cf->sockindex, AF_INET, 0);
  ఒకవేళ(ai) {
    నిర్మాణం sockaddr_in *saddr_లోపల;
    అక్షరం ipbuf[64];

    Curl_printable_చిరునామా(ai, ipbuf, పరిమాణం(ipbuf));
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_లోపల = (నిర్మాణం sockaddr_in *)(శూన్య *)ai->ai_addr;
    ఫలితం = Curl_bufq_write(&sx->iobuf,
                             (గుర్తులేని అక్షరం *)&saddr_లోపల->sin_addr.s_addr, 4,
                             &nwritten);

    ఒకవేళ(ఫలితం || (nwritten != 4))
      తిరిగి CURLPX_SEND_REQUEST;
  }
  లేకపోతే {
    /* No IPv4 address resolved */
    failf(దత్తాంశం, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    తిరిగి CURLPX_RESOLVE_HOST;
  }

  *పూర్తి = TRUE;
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks4_check_resp(నిర్మాణం socks_ctx *sx,
                                       నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి గుర్తులేని అక్షరం *resp;
  size_t rlen;

  ఒకవేళ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(దత్తాంశం, "SOCKS4 reply is incomplete.");
    తిరిగి CURLPX_RECV_CONNECT;
  }

  DEBUGASSERT(rlen == 8);
  /*
   * Response format
   *
   *     +----+----+----+----+----+----+----+----+
   *     | VN | CD | DSTPORT |      DSTIP        |
   *     +----+----+----+----+----+----+----+----+
   * # of bytes:  1    1      2              4
   *
   * VN is the version of the reply code and should be 0. CD is the result
   * code with one of the following values:
   *
   * 90: request granted
   * 91: request rejected or failed
   * 92: request rejected because SOCKS server cannot connect to
   *     identd on the client
   * 93: request rejected because the client program and identd
   *     report different user-ids
   */

  /* wrong version ? */
  ఒకవేళ(resp[0]) {
    failf(దత్తాంశం, "SOCKS4 reply has wrong version, version should be 0.");
    తిరిగి CURLPX_BAD_సంస్కరణ;
  }

  /*ఫలితం*/
  ఎంపిక(resp[1]) {
  స్థితి 90:
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    తిరిగి CURLPX_OK;
  స్థితి 91:
    failf(దత్తాంశం,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (గుర్తులేని పూర్ణాంకం)((resp[2] << 8) | resp[3]), resp[1]);
    తిరిగి CURLPX_REQUEST_FAILED;
  స్థితి 92:
    failf(దత్తాంశం,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (గుర్తులేని పూర్ణాంకం)((resp[2] << 8) | resp[3]), resp[1]);
    తిరిగి CURLPX_IDENTD;
  స్థితి 93:
    failf(దత్తాంశం,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (గుర్తులేని పూర్ణాంకం)((resp[2] << 8) | resp[3]), resp[1]);
    తిరిగి CURLPX_IDENTD_తేడా;
  అప్రమేయ:
    failf(దత్తాంశం,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (గుర్తులేని పూర్ణాంకం)((resp[2] << 8) | resp[3]), resp[1]);
    తిరిగి CURLPX_UNKNOWN_FAIL;
  }
}

/*
 * This function logs in to a SOCKS4 proxy and sends the specifics to the final
 * destination server.
 *
 * Reference :
 *   https://www.openssh.com/txt/socks4.protocol
 *
 * Note :
 *   Set protocol4a=true for  "SOCKS 4A (Simple Extension to SOCKS 4 Protocol)"
 *   Nonsupport "Identification Protocol (RFC1413)"
 */
స్థిర CURLproxycode socks4_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం socks_ctx *sx,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode ఫలితం;
  bool పూర్తి;

process_state:
  ఎంపిక(sx->state) {
  స్థితి SOCKS_ST_INIT:
    sx->సంస్కరణ_2 = 4;
    sxstate(sx, cf, దత్తాంశం, SOCKS4_ST_ప్రారంభం);
    FALLTHROUGH();

  స్థితి SOCKS4_ST_ప్రారంభం:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_సంస్కరణ = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->పోర్టు);

    /*
     * Compose socks4 request
     *
     * Request format
     *
     *     +----+----+----+----+----+----+----+----+----+----+....+----+
     *     | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
     *     +----+----+----+----+----+----+----+----+----+----+....+----+
     * # of bytes:  1    1      2              4           variable       1
     */
    presult = socks4_req_కూడు_hd(sx, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ఒకవేళ(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      గుర్తులేని అక్షరం buf[4] = { 0, 0, 0, 1 };
      size_t hlen = అక్షరశ్రేణి_పొడవు(sx->dest->hostname) + 1; /* including NUL */

      ఒకవేళ(hlen > 255) {
        failf(దత్తాంశం, "SOCKS4: too long hostname");
        తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_LONG_HOSTNAME);
      }
      ఫలితం = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ఒకవేళ(ఫలితం || (nwritten != 4))
        తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_SEND_REQUEST);
      presult = socks4_req_కూడు_user(sx, దత్తాంశం);
      ఒకవేళ(presult)
        తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
      ఫలితం = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ఒకవేళ(ఫలితం || (nwritten != hlen))
        తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, దత్తాంశం, SOCKS4_ST_SEND);
      వెళ్ళు process_state;
    }
    sxstate(sx, cf, దత్తాంశం, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  స్థితి SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    /* append user */
    presult = socks4_req_కూడు_user(sx, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    sxstate(sx, cf, దత్తాంశం, SOCKS4_ST_SEND);
    FALLTHROUGH();

  స్థితి SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    sxstate(sx, cf, దత్తాంశం, SOCKS4_ST_RECV);
    FALLTHROUGH();

  స్థితి SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, దత్తాంశం, 8, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    presult = socks4_check_resp(sx, cf, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    sxstate(sx, cf, దత్తాంశం, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  స్థితి SOCKS_ST_SUCCESS:
    తిరిగి CURLPX_OK;

  స్థితి SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    తిరిగి sx->presult;

  అప్రమేయ:
    DEBUGASSERT(0);
    తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_SEND_REQUEST);
  }
}

స్థిర CURLproxycode socks5_req0_init(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం socks_ctx *sx,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి గుర్తులేని అక్షరం auth = దత్తాంశం->సమితి.socks5auth;
  గుర్తులేని అక్షరం req[5]; /* version + len + 3 possible auth methods */
  గుర్తులేని అక్షరం nauths;
  size_t req_len, nwritten;
  CURLcode ఫలితం;

  (శూన్య)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ఒకవేళ(!sx->resolve_local && అక్షరశ్రేణి_పొడవు(sx->dest->hostname) > 255) {
    failf(దత్తాంశం, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    తిరిగి CURLPX_LONG_HOSTNAME;
  }

  ఒకవేళ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(దత్తాంశం, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ఒకవేళ(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ఒకవేళ(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->బిట్లు.socks5_authenticated = FALSE;

  req[0] = 5;   /*సంస్కరణ*/
  nauths = 0;
  ఒకవేళ(!దత్తాంశం->సమితి.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ఒకవేళ(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#అయితే_ముగింపు
  ఒకవేళ(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ఒకవేళ(!nauths) {
    failf(దత్తాంశం, "SOCKS5: no acceptable authentication method is available.");
    తిరిగి CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  ఫలితం = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != req_len))
    తిరిగి CURLPX_SEND_REQUEST;
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks5_check_resp0(నిర్మాణం socks_ctx *sx,
                                        నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి గుర్తులేని అక్షరం *resp;
  గుర్తులేని అక్షరం auth_బహుళకం;
  size_t rlen;

  ఒకవేళ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(దత్తాంశం, "SOCKS5 initial reply is incomplete.");
    తిరిగి CURLPX_RECV_CONNECT;
  }

  ఒకవేళ(resp[0] != 5) {
    failf(దత్తాంశం, "Received invalid version in initial SOCKS5 response.");
    తిరిగి CURLPX_BAD_సంస్కరణ;
  }

  auth_బహుళకం = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  ఎంపిక(auth_బహుళకం) {
  స్థితి 0:
    ఒకవేళ(దత్తాంశం->సమితి.socks5_auth_only) {
      failf(దత్తాంశం, "SOCKS5 proxy selected no authentication.");
      తిరిగి CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ1_INIT);
    తిరిగి CURLPX_OK;
  స్థితి 1:
    ఒకవేళ(దత్తాంశం->సమితి.socks5auth & CURLAUTH_GSSAPI) {
      ఒకవేళ(cf->sockindex == FIRSTSOCKET)
        cf->conn->బిట్లు.socks5_authenticated = TRUE;
      sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_GSSAPI_INIT);
      తిరిగి CURLPX_OK;
    }
    failf(దత్తాంశం, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    తిరిగి CURLPX_GSSAPI_PERMSG;
  స్థితి 2:
    /* regular name + password authentication */
    ఒకవేళ(దత్తాంశం->సమితి.socks5auth & CURLAUTH_BASIC) {
      ఒకవేళ(cf->sockindex == FIRSTSOCKET)
        cf->conn->బిట్లు.socks5_authenticated = TRUE;
      sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_AUTH_INIT);
      తిరిగి CURLPX_OK;
    }
    failf(దత్తాంశం, "BASIC authentication proposed but not enabled.");
    తిరిగి CURLPX_NO_AUTH;
  స్థితి 255:
    failf(దత్తాంశం, "No authentication method was acceptable.");
    తిరిగి CURLPX_NO_AUTH;
  అప్రమేయ:
    failf(దత్తాంశం, "Unknown SOCKS5 mode attempted to be used by server.");
    తిరిగి CURLPX_UNKNOWN_బహుళకం;
  }
}

స్థిర CURLproxycode socks5_auth_init(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం socks_ctx *sx,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  గుర్తులేని అక్షరం buf[2];
  CURLcode ఫలితం;

  ఒకవేళ(sx->creds) {
    ulen = అక్షరశ్రేణి_పొడవు(sx->creds->user);
    plen = అక్షరశ్రేణి_పొడవు(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ఒకవేళ(ulen > 255) {
      failf(దత్తాంశం, "Excessive username length for proxy auth");
      తిరిగి CURLPX_LONG_USER;
    }
    ఒకవేళ(plen > 255) {
      failf(దత్తాంశం, "Excessive password length for proxy auth");
      తిరిగి CURLPX_LONG_PASSWD;
    }
  }

  /*   username/password request looks like
   * +----+------+----------+------+----------+
   * |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
   * +----+------+----------+------+----------+
   * | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
   * +----+------+----------+------+----------+
   */
  buf[0] = 1;    /* username/pw subnegotiation version */
  buf[1] = (గుర్తులేని అక్షరం)ulen;
  ఫలితం = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 2))
    తిరిగి CURLPX_SEND_REQUEST;
  ఒకవేళ(ulen) {
    ఫలితం = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != ulen))
      తిరిగి CURLPX_SEND_REQUEST;
  }
  buf[0] = (గుర్తులేని అక్షరం)plen;
  ఫలితం = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 1))
    తిరిగి CURLPX_SEND_REQUEST;
  ఒకవేళ(plen) {
    ఫలితం = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != plen))
      తిరిగి CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_AUTH_SEND);
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks5_check_auth_resp(నిర్మాణం socks_ctx *sx,
                                            నిర్మాణం Curl_cfilter *cf,
                                            నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి గుర్తులేని అక్షరం *resp;
  గుర్తులేని అక్షరం auth_స్థితి;
  size_t rlen;

  (శూన్య)cf;
  ఒకవేళ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(దత్తాంశం, "SOCKS5 sub-negotiation response incomplete.");
    తిరిగి CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_స్థితి = resp[1];
  ఒకవేళ(auth_స్థితి) {
    failf(దత్తాంశం, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    తిరిగి CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks5_req1_init(నిర్మాణం socks_ctx *sx,
                                      నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  గుర్తులేని అక్షరం req[5];
  గుర్తులేని అక్షరం ipbuf[16];
  స్థిరరాశి గుర్తులేని అక్షరం *destination;
  గుర్తులేని అక్షరం desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode ఫలితం;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ఒకవేళ(sx->resolve_local) {
    /* rest of request is added after resolving */
    ఫలితం = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != 3))
      తిరిగి CURLPX_SEND_REQUEST;
    తిరిగి CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ఒకవేళ(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      తిరిగి CURLPX_BAD_చిరునామా_TYPE;
  }
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  లేకపోతే {
    స్థిరరాశి size_t hostname_len = అక్షరశ్రేణి_పొడవు(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (స్థిరరాశి గుర్తులేని అక్షరం *)sx->dest->hostname;
    destlen = (గుర్తులేని అక్షరం)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  ఫలితం = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != hdlen))
    తిరిగి CURLPX_SEND_REQUEST;
  ఫలితం = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != destlen))
    తిరిగి CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (గుర్తులేని అక్షరం)((sx->dest->పోర్టు >> 8) & 0xff);
  req[1] = (గుర్తులేని అక్షరం)(sx->dest->పోర్టు & 0xff);
  ఫలితం = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 2))
    తిరిగి CURLPX_SEND_REQUEST;
  CURL_TRC_CF(దత్తాంశం, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->పోర్టు);
  తిరిగి CURLPX_OK;
}

స్థిర CURLproxycode socks5_resolving(నిర్మాణం socks_ctx *sx,
                                      నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *పూర్తి)
{
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai = ఖాళీ;
  అక్షరం dest[MAX_IPADR_LEN];  /* printable address */
  స్థిరరాశి గుర్తులేని అక్షరం *destination = ఖాళీ;
  గుర్తులేని అక్షరం desttype = 1, destlen = 4;
  గుర్తులేని అక్షరం req[2];
  CURLcode ఫలితం;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *పూర్తి = FALSE;
  ఫలితం = Curl_conn_dns_addr_ఫలితం(cf->conn, cf->sockindex, sx->dest);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_AGAIN) {
      failf(దత్తాంశం, "error %d resolving SOCKS destination %s:%u",
            (పూర్ణాంకం)ఫలితం, sx->dest->hostname, sx->dest->పోర్టు);
      తిరిగి CURLPX_RESOLVE_HOST;
    }
    తిరిగి CURLPX_OK;
  }

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(దత్తాంశం->సమితి.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(దత్తాంశం, sx->dest, cf->sockindex, AF_INET6, 0);
#అయితే_ముగింపు
  ఒకవేళ(!ai)
    ai = Curl_conn_dns_get_ai(దత్తాంశం, sx->dest, cf->sockindex, AF_INET, 0);

  ఒకవేళ(!ai) {
    failf(దత్తాంశం, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    వెళ్ళు బహిర్గతం;
  }

  Curl_printable_చిరునామా(ai, dest, పరిమాణం(dest));

  ఒకవేళ(ai->ai_family == AF_INET) {
    నిర్మాణం sockaddr_in *saddr_లోపల;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_లోపల = (నిర్మాణం sockaddr_in *)(శూన్య *)ai->ai_addr;
    destination = (స్థిరరాశి గుర్తులేని అక్షరం *)&saddr_లోపల->sin_addr.s_addr;
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->పోర్టు);
  }
#నిర్వచితమైతే USE_IPV6
  లేకపోతే ఒకవేళ(ai->ai_family == AF_INET6) {
    నిర్మాణం sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (నిర్మాణం sockaddr_in6 *)(శూన్య *)ai->ai_addr;
    destination = (స్థిరరాశి గుర్తులేని అక్షరం *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->పోర్టు);
  }
#అయితే_ముగింపు

  ఒకవేళ(!destination) {
    failf(దత్తాంశం, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    వెళ్ళు బహిర్గతం;
  }

  req[0] = desttype;
  ఫలితం = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    వెళ్ళు బహిర్గతం;
  }
  /* PORT MSB+LSB */
  req[0] = (గుర్తులేని అక్షరం)((sx->dest->పోర్టు >> 8) & 0xffU);
  req[1] = (గుర్తులేని అక్షరం)(sx->dest->పోర్టు & 0xffU);
  ఫలితం = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    వెళ్ళు బహిర్గతం;
  }

బహిర్గతం:
  *పూర్తి = (presult == CURLPX_OK);
  తిరిగి presult;
}

స్థిర CURLproxycode socks5_recv_resp1(నిర్మాణం socks_ctx *sx,
                                       నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *పూర్తి)
{
  స్థిరరాశి గుర్తులేని అక్షరం *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, దత్తాంశం, resp_len, పూర్తి);
  ఒకవేళ(presult)
    తిరిగి presult;
  లేకపోతే ఒకవేళ(!*పూర్తి)
    తిరిగి CURLPX_OK;

  ఒకవేళ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(దత్తాంశం, "SOCKS5 response is incomplete.");
    తిరిగి CURLPX_RECV_CONNECT;
  }

  /* Response packet includes BND.ADDR is variable length parameter by RFC
     1928, so the response packet MUST be read until the end to avoid errors
     at subsequent protocol level.

     +----+-----+-------+------+----------+----------+
     |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
     +----+-----+-------+------+----------+----------+
     | 1  |  1  | 0x00  |  1   | Variable |    2     |
     +----+-----+-------+------+----------+----------+

     ATYP:
     o IPv4 address: 0x01, BND.ADDR = 4-byte
     o domain name:  0x03, BND.ADDR = [ 1-byte length, string ]
     o IPv6 address: 0x04, BND.ADDR = 16-byte
   */
  ఒకవేళ(resp[0] != 5) { /*సంస్కరణ*/
    failf(దత్తాంశం, "SOCKS5 reply has wrong version, version should be 5.");
    తిరిగి CURLPX_BAD_సంస్కరణ;
  }
  లేకపోతే ఒకవేళ(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_జవాబు_UNASSIGNED;
    పూర్ణాంకం సంకేతం_2 = resp[1];
    failf(దత్తాంశం, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, సంకేతం_2);
    ఒకవేళ(సంకేతం_2 < 9) {
      /* RFC 1928 section 6 lists: */
      స్థిర స్థిరరాశి CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_జవాబు_GENERAL_SERVER_FAILURE,
        CURLPX_జవాబు_NOT_ALLOWED,
        CURLPX_జవాబు_జాలం_UNREACHABLE,
        CURLPX_జవాబు_HOST_UNREACHABLE,
        CURLPX_జవాబు_CONNECTION_REFUSED,
        CURLPX_జవాబు_TTL_EXPIRED,
        CURLPX_జవాబు_COMMAND_NOT_SUPPORTED,
        CURLPX_జవాబు_చిరునామా_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[సంకేతం_2];
    }
    తిరిగి rc;
  }

  /* Calculate real packet size */
  ఎంపిక(resp[3]) {
  స్థితి 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ఆపు;
  స్థితి 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ఆపు;
  స్థితి 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "SOCKS5 reply has wrong address type.");
    తిరిగి CURLPX_BAD_చిరునామా_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, దత్తాంశం, resp_len, పూర్తి);
  ఒకవేళ(presult)
    తిరిగి presult;
  లేకపోతే ఒకవేళ(!*పూర్తి)
    తిరిగి CURLPX_OK;

  ఒకవేళ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(దత్తాంశం, "SOCKS5 response is incomplete.");
    తిరిగి CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *పూర్తి = TRUE;
  తిరిగి CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
స్థిర CURLproxycode socks5_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం socks_ctx *sx,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLproxycode presult;
  bool పూర్తి;

process_state:
  ఎంపిక(sx->state) {
  స్థితి SOCKS_ST_INIT:
    sx->సంస్కరణ_2 = 5;
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_ప్రారంభం);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_ప్రారంభం:
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->పోర్టు);
    presult = socks5_req0_init(cf, sx, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, దత్తాంశం, 2, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    /* socks5_check_resp0() sets next socks state */
    వెళ్ళు process_state;

  స్థితి SOCKS5_ST_GSSAPI_INIT: {
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode ఫలితం = Curl_SOCKS5_gssapi_negotiate(cf, దత్తాంశం, sx->creds);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Unable to negotiate SOCKS5 GSS-API context.");
      తిరిగి CURLPX_GSSAPI;
    }
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ1_INIT);
    వెళ్ళు process_state;
#లేకపోతే
    failf(దత్తాంశం, "SOCKS5 GSSAPI per-message authentication is not supported.");
    తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_GSSAPI_PERMSG);
#అయితే_ముగింపు
  }

  స్థితి SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, దత్తాంశం, 2, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, దత్తాంశం);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    ఒకవేళ(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ1_SEND);
      వెళ్ళు process_state;
    }
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    లేకపోతే ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ఒకవేళ(cf->conn->socks5_gssapi_enctype) {
      failf(దత్తాంశం, "SOCKS5 GSS-API protection not yet implemented.");
      తిరిగి CURLPX_GSSAPI_PROTECTION;
    }
#అయితే_ముగింపు
    sxstate(sx, cf, దత్తాంశం, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  స్థితి SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(presult)
      తిరిగి socks_failed(sx, cf, దత్తాంశం, presult);
    ఒకవేళ(!పూర్తి)
      తిరిగి CURLPX_OK;
    CURL_TRC_CF(దత్తాంశం, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, దత్తాంశం, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  స్థితి SOCKS_ST_SUCCESS:
    తిరిగి CURLPX_OK;

  స్థితి SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    తిరిగి sx->presult;

  అప్రమేయ:
    DEBUGASSERT(0);
    తిరిగి socks_failed(sx, cf, దత్తాంశం, CURLPX_SEND_REQUEST);
  }
}

స్థిర శూన్య socks_proxy_ctx_free(నిర్మాణం socks_ctx *ctx)
{
  ఒకవేళ(ctx) {
    Curl_peer_unlink(&ctx->dest);
    Curl_creds_unlink(&ctx->creds);
    Curl_bufq_free(&ctx->iobuf);
    curlx_free(ctx);
  }
}

/* After a TCP connection to the proxy has been verified, this function does
   the next magic steps. If 'done' is not set TRUE, it is not done yet and
   must be called again.

   Note: this function's sub-functions call failf() */
స్థిర CURLcode socks_proxy_cf_connect(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *పూర్తి)
{
  నిర్మాణం socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode ఫలితం;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
  ఒకవేళ(ఫలితం || !*పూర్తి)
    తిరిగి ఫలితం;

  ఎంపిక(ctx->proxy_type) {
  స్థితి CURLPROXY_SOCKS5:
  స్థితి CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, దత్తాంశం);
    ఆపు;

  స్థితి CURLPROXY_SOCKS4:
  స్థితి CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, దత్తాంశం);
    ఆపు;

  అప్రమేయ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    ఫలితం = CURLE_COULDNT_CONNECT;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(pxresult) {
    ఫలితం = CURLE_PROXY;
    దత్తాంశం->info.pxcode = (uint8_t)pxresult;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ctx->state != SOCKS_ST_SUCCESS)
    వెళ్ళు బహిర్గతం;

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    నిర్మాణం ip_quadruple ipquad;
    bool is_ipv6;
    ఒకవేళ(!Curl_conn_cf_get_ip_info(cf->తదుపరి, దత్తాంశం, &is_ipv6, &ipquad))
      infof(దత్తాంశం, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_పోర్టు,
            ctx->dest->hostname, ctx->dest->పోర్టు,
            ipquad.remote_ip, ipquad.remote_పోర్టు);
    లేకపోతే
      infof(దత్తాంశం, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#అయితే_ముగింపు
  cf->connected = TRUE;

బహిర్గతం:
  *పూర్తి = (bool)cf->connected;
  ఒకవేళ(*పూర్తి || ఫలితం)
    Curl_creds_unlink(&ctx->creds);
  తిరిగి ఫలితం;
}

స్థిర CURLcode socks_cf_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం socks_ctx *sx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);
    ఎంపిక(sx->state) {
    స్థితి SOCKS4_ST_SEND:
    స్థితి SOCKS5_ST_REQ0_SEND:
    స్థితి SOCKS5_ST_AUTH_SEND:
    స్థితి SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(దత్తాంశం, cf, "adjust pollset out (%d)", (పూర్ణాంకం)sx->state);
      ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, sock);
      ఆపు;
    అప్రమేయ:
      CURL_TRC_CF(దత్తాంశం, cf, "adjust pollset in (%d)", (పూర్ణాంకం)sx->state);
      ఫలితం = Curl_pollset_సమితి_లోపల_only(దత్తాంశం, ps, sock);
      ఆపు;
    }
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య socks_proxy_cf_destroy(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)దత్తాంశం;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ఖాళీ;
}

స్థిర CURLcode socks_cf_query(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం socks_ctx *sx = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_HOST_పోర్టు:
    ఒకవేళ(sx) {
      *pres1 = sx->dest->పోర్టు;
      *((స్థిరరాశి అక్షరం **)pres2) = sx->dest->hostname;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ఖాళీ;
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

నిర్మాణం Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_peer *dest,
                                          uint8_t ip_సంస్కరణ,
                                          uint8_t proxy_type,
                                          నిర్మాణం Curl_creds *creds)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(దత్తాంశం, ip_సంస్కరణ);
  CURLcode ఫలితం;

  ఒకవేళ(!dest)
    తిరిగి CURLE_FAILED_INIT;

  ఎంపిక(proxy_type) {
  స్థితి CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ఆపు;
  స్థితి CURLPROXY_SOCKS5_HOSTNAME:
    ఆపు;
  స్థితి CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ఆపు;
  స్థితి CURLPROXY_SOCKS4A:
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "unknown proxytype %d option given", proxy_type);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_సంస్కరణ = ip_సంస్కరణ;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_కొలత, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ఒకవేళ(!ఫలితం) {
    Curl_conn_cf_చొప్పించు_after(cf_at, cf);
    ఒకవేళ(ctx->resolve_local) {
      ఫలితం = Curl_conn_dns_కూడు_addr_resolve(దత్తాంశం, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  లేకపోతే
    socks_proxy_ctx_free(ctx);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* CURL_DISABLE_PROXY */
