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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "socks.h"
#ضمن "vdns/cf-dns.h"
#ضمن "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
تعداد socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_شروعات,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_شروعات,
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

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن ثابت حرف * ثابت cf_socks_statename[] = {
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
#جيڪڏهن_پورو

#وصف_ڪريو SOCKS_CHUNK_سائيز    1024
#وصف_ڪريو SOCKS_CHUNKS        1

بنية socks_ctx {
  تعداد socks_state_t state;
  بنية bufq iobuf;
  بنية Curl_peer *dest;
  بنية Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_نسخو;
  uint8_t proxy_type;
  بنانشان حرف نسخو_2;
  ٻٽو_انگ(resolve_local);
  ٻٽو_انگ(socks4a);
};

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            حرف *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode نتيجو;

  *pnread = 0;
  لكل(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(معلومات);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);

    إذا(timeout_ms < 0) {
      /* we already got the timeout */
      أرجع CURLE_OPERATION_TIMEDOUT;
    }
    إذا(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    إذا(SOCKET_READABLE(sock, timeout_ms) <= 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات, buf, blen, &nread);
    إذا(نتيجو == CURLE_AGAIN)
      استمر;
    وإلا إذا(نتيجو)
      أرجع نتيجو;

    إذا(blen == nread) {
      *pnread += nread;
      أرجع CURLE_OK;
    }
    إذا(!nread) /* EOF */
      أرجع CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#جيڪڏهن_پورو

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
#وصف_ڪريو sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#وإلا
#وصف_ڪريو sxstate(x, c, d, y) socksstate(x, c, d, y)
#جيڪڏهن_پورو

/* always use this function to change state, to make debugging easier */
ساكن فراغ socksstate(بنية socks_ctx *sx,
                       بنية Curl_cfilter *cf,
                       بنية Curl_easy *معلومات,
                       تعداد socks_state_t state
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , صحيح lineno
#جيڪڏهن_پورو
)
{
  تعداد socks_state_t oldstate = sx->state;

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

  sx->state = state;

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(معلومات, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#وإلا
  (فراغ)cf;
  (فراغ)معلومات;
#جيڪڏهن_پورو
}

ساكن CURLproxycode socks_failed(بنية socks_ctx *sx,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, معلومات, SOCKS_ST_FAILED);
  sx->presult = presult;
  أرجع presult;
}

ساكن CURLproxycode socks_flush(بنية socks_ctx *sx,
                                 بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *پورو)
{
  CURLcode نتيجو;
  size_t nwritten;

  *پورو = FALSE;
  طالما(!Curl_bufq_is_خالي_جايون(&sx->iobuf)) {
    نتيجو = Curl_cf_send_bufq(cf->ايندڙ, معلومات, &sx->iobuf, عدم, 0,
                               &nwritten);
    إذا(نتيجو == CURLE_AGAIN)
      أرجع CURLPX_OK;
    وإلا إذا(نتيجو) {
      failf(معلومات, "Failed to send SOCKS request: %s",
            curl_easy_strerror(نتيجو));
      أرجع socks_failed(sx, cf, معلومات, CURLPX_SEND_CONNECT);
    }
  }
  *پورو = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks_recv(بنية socks_ctx *sx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                size_t min_بائيٽون,
                                bool *پورو)
{
  CURLcode نتيجو;
  size_t nread;

  *پورو = FALSE;
  طالما(Curl_bufq_len(&sx->iobuf) < min_بائيٽون) {
    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &sx->iobuf,
                               min_بائيٽون - Curl_bufq_len(&sx->iobuf),
                               &nread);
    إذا(نتيجو == CURLE_AGAIN)
      أرجع CURLPX_OK;
    وإلا إذا(نتيجو) {
      failf(معلومات, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(نتيجو));
      أرجع CURLPX_RECV_CONNECT;
    }
    وإلا إذا(!nread) { /* EOF */
      إذا(Curl_bufq_len(&sx->iobuf) < min_بائيٽون) {
        failf(معلومات, "Failed to receive SOCKS response, "
              "proxy closed connection");
        أرجع CURLPX_RECV_CONNECT;
      }
      اكسر;
    }
  }
  *پورو = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_req_جوڙيو_hd(بنية socks_ctx *sx,
                                       بنية Curl_easy *معلومات)
{
  بنانشان حرف buf[4];
  size_t nwritten;
  CURLcode نتيجو;

  (فراغ)معلومات;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (بنانشان حرف)((sx->dest->پورٽ >> 8) & 0xffU); /* MSB */
  buf[3] = (بنانشان حرف)(sx->dest->پورٽ & 0xffU);        /* LSB */

  نتيجو = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  إذا(نتيجو || (nwritten != 4))
    أرجع CURLPX_SEND_REQUEST;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_req_جوڙيو_user(بنية socks_ctx *sx,
                                         بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  size_t nwritten;

  إذا(sx->creds) {
    size_t plen = لڙي_ڊيگهه(sx->creds->user);
    إذا(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(معلومات, "Too long SOCKS proxy username");
      أرجع CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    نتيجو = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    إذا(نتيجو || (nwritten != (plen + 1)))
      أرجع CURLPX_SEND_REQUEST;
  }
  وإلا {
    /* empty username */
    بنانشان حرف b = 0;
    نتيجو = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    إذا(نتيجو || (nwritten != 1))
      أرجع CURLPX_SEND_REQUEST;
  }
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_resolving(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *پورو)
{
  ثابت بنية Curl_addrinfo *ai = عدم;
  CURLcode نتيجو;
  size_t nwritten;

  *پورو = FALSE;
  نتيجو = Curl_conn_dns_addr_نتيجو(cf->conn, cf->sockindex, sx->dest);
  إذا(نتيجو) {
    إذا(نتيجو != CURLE_AGAIN) {
      failf(معلومات, "error %d resolving SOCKS destination %s:%u",
            (صحيح)نتيجو, sx->dest->hostname, sx->dest->پورٽ);
      أرجع CURLPX_RESOLVE_HOST;
    }
    أرجع CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(معلومات, sx->dest, cf->sockindex, AF_INET, 0);
  إذا(ai) {
    بنية sockaddr_in *saddr_اندريون;
    حرف ipbuf[64];

    Curl_printable_پتو(ai, ipbuf, حجم(ipbuf));
    CURL_TRC_CF(معلومات, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_اندريون = (بنية sockaddr_in *)(فراغ *)ai->ai_addr;
    نتيجو = Curl_bufq_write(&sx->iobuf,
                             (بنانشان حرف *)&saddr_اندريون->sin_addr.s_addr, 4,
                             &nwritten);

    إذا(نتيجو || (nwritten != 4))
      أرجع CURLPX_SEND_REQUEST;
  }
  وإلا {
    /* No IPv4 address resolved */
    failf(معلومات, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    أرجع CURLPX_RESOLVE_HOST;
  }

  *پورو = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_check_resp(بنية socks_ctx *sx,
                                       بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات)
{
  ثابت بنانشان حرف *resp;
  size_t rlen;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(معلومات, "SOCKS4 reply is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
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
  إذا(resp[0]) {
    failf(معلومات, "SOCKS4 reply has wrong version, version should be 0.");
    أرجع CURLPX_BAD_نسخو;
  }

  /*نتيجو*/
  بدل(resp[1]) {
  حالة 90:
    CURL_TRC_CF(معلومات, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    أرجع CURLPX_OK;
  حالة 91:
    failf(معلومات,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (بنانشان صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_REQUEST_FAILED;
  حالة 92:
    failf(معلومات,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (بنانشان صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_IDENTD;
  حالة 93:
    failf(معلومات,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (بنانشان صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_IDENTD_فرق;
  افتراضي:
    failf(معلومات,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (بنانشان صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_UNKNOWN_FAIL;
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
ساكن CURLproxycode socks4_connect(بنية Curl_cfilter *cf,
                                    بنية socks_ctx *sx,
                                    بنية Curl_easy *معلومات)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode نتيجو;
  bool پورو;

process_state:
  بدل(sx->state) {
  حالة SOCKS_ST_INIT:
    sx->نسخو_2 = 4;
    sxstate(sx, cf, معلومات, SOCKS4_ST_شروعات);
    FALLTHROUGH();

  حالة SOCKS4_ST_شروعات:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_نسخو = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(معلومات, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->پورٽ);

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
    presult = socks4_req_جوڙيو_hd(sx, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    إذا(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      بنانشان حرف buf[4] = { 0, 0, 0, 1 };
      size_t hlen = لڙي_ڊيگهه(sx->dest->hostname) + 1; /* including NUL */

      إذا(hlen > 255) {
        failf(معلومات, "SOCKS4: too long hostname");
        أرجع socks_failed(sx, cf, معلومات, CURLPX_LONG_HOSTNAME);
      }
      نتيجو = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      إذا(نتيجو || (nwritten != 4))
        أرجع socks_failed(sx, cf, معلومات, CURLPX_SEND_REQUEST);
      presult = socks4_req_جوڙيو_user(sx, معلومات);
      إذا(presult)
        أرجع socks_failed(sx, cf, معلومات, presult);
      نتيجو = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      إذا(نتيجو || (nwritten != hlen))
        أرجع socks_failed(sx, cf, معلومات, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, معلومات, SOCKS4_ST_SEND);
      اذهب process_state;
    }
    sxstate(sx, cf, معلومات, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  حالة SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    إذا(!پورو)
      أرجع CURLPX_OK;
    /* append user */
    presult = socks4_req_جوڙيو_user(sx, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    sxstate(sx, cf, معلومات, SOCKS4_ST_SEND);
    FALLTHROUGH();

  حالة SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    sxstate(sx, cf, معلومات, SOCKS4_ST_RECV);
    FALLTHROUGH();

  حالة SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, معلومات, 8, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    presult = socks4_check_resp(sx, cf, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    sxstate(sx, cf, معلومات, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالة SOCKS_ST_SUCCESS:
    أرجع CURLPX_OK;

  حالة SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    أرجع sx->presult;

  افتراضي:
    DEBUGASSERT(0);
    أرجع socks_failed(sx, cf, معلومات, CURLPX_SEND_REQUEST);
  }
}

ساكن CURLproxycode socks5_req0_init(بنية Curl_cfilter *cf,
                                      بنية socks_ctx *sx,
                                      بنية Curl_easy *معلومات)
{
  ثابت بنانشان حرف auth = معلومات->سيٽ.socks5auth;
  بنانشان حرف req[5]; /* version + len + 3 possible auth methods */
  بنانشان حرف nauths;
  size_t req_len, nwritten;
  CURLcode نتيجو;

  (فراغ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  إذا(!sx->resolve_local && لڙي_ڊيگهه(sx->dest->hostname) > 255) {
    failf(معلومات, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    أرجع CURLPX_LONG_HOSTNAME;
  }

  إذا(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(معلومات, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  إذا(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  إذا(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->بٽ.socks5_authenticated = FALSE;

  req[0] = 5;   /*نسخو*/
  nauths = 0;
  إذا(!معلومات->سيٽ.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  إذا(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#جيڪڏهن_پورو
  إذا(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  إذا(!nauths) {
    failf(معلومات, "SOCKS5: no acceptable authentication method is available.");
    أرجع CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  نتيجو = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  إذا(نتيجو || (nwritten != req_len))
    أرجع CURLPX_SEND_REQUEST;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_check_resp0(بنية socks_ctx *sx,
                                        بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات)
{
  ثابت بنانشان حرف *resp;
  بنانشان حرف auth_گهڻائي;
  size_t rlen;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(معلومات, "SOCKS5 initial reply is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }

  إذا(resp[0] != 5) {
    failf(معلومات, "Received invalid version in initial SOCKS5 response.");
    أرجع CURLPX_BAD_نسخو;
  }

  auth_گهڻائي = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  بدل(auth_گهڻائي) {
  حالة 0:
    إذا(معلومات->سيٽ.socks5_auth_only) {
      failf(معلومات, "SOCKS5 proxy selected no authentication.");
      أرجع CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, معلومات, SOCKS5_ST_REQ1_INIT);
    أرجع CURLPX_OK;
  حالة 1:
    إذا(معلومات->سيٽ.socks5auth & CURLAUTH_GSSAPI) {
      إذا(cf->sockindex == FIRSTSOCKET)
        cf->conn->بٽ.socks5_authenticated = TRUE;
      sxstate(sx, cf, معلومات, SOCKS5_ST_GSSAPI_INIT);
      أرجع CURLPX_OK;
    }
    failf(معلومات, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    أرجع CURLPX_GSSAPI_PERMSG;
  حالة 2:
    /* regular name + password authentication */
    إذا(معلومات->سيٽ.socks5auth & CURLAUTH_BASIC) {
      إذا(cf->sockindex == FIRSTSOCKET)
        cf->conn->بٽ.socks5_authenticated = TRUE;
      sxstate(sx, cf, معلومات, SOCKS5_ST_AUTH_INIT);
      أرجع CURLPX_OK;
    }
    failf(معلومات, "BASIC authentication proposed but not enabled.");
    أرجع CURLPX_NO_AUTH;
  حالة 255:
    failf(معلومات, "No authentication method was acceptable.");
    أرجع CURLPX_NO_AUTH;
  افتراضي:
    failf(معلومات, "Unknown SOCKS5 mode attempted to be used by server.");
    أرجع CURLPX_UNKNOWN_گهڻائي;
  }
}

ساكن CURLproxycode socks5_auth_init(بنية Curl_cfilter *cf,
                                      بنية socks_ctx *sx,
                                      بنية Curl_easy *معلومات)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  بنانشان حرف buf[2];
  CURLcode نتيجو;

  إذا(sx->creds) {
    ulen = لڙي_ڊيگهه(sx->creds->user);
    plen = لڙي_ڊيگهه(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    إذا(ulen > 255) {
      failf(معلومات, "Excessive username length for proxy auth");
      أرجع CURLPX_LONG_USER;
    }
    إذا(plen > 255) {
      failf(معلومات, "Excessive password length for proxy auth");
      أرجع CURLPX_LONG_PASSWD;
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
  buf[1] = (بنانشان حرف)ulen;
  نتيجو = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  إذا(نتيجو || (nwritten != 2))
    أرجع CURLPX_SEND_REQUEST;
  إذا(ulen) {
    نتيجو = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    إذا(نتيجو || (nwritten != ulen))
      أرجع CURLPX_SEND_REQUEST;
  }
  buf[0] = (بنانشان حرف)plen;
  نتيجو = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  إذا(نتيجو || (nwritten != 1))
    أرجع CURLPX_SEND_REQUEST;
  إذا(plen) {
    نتيجو = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    إذا(نتيجو || (nwritten != plen))
      أرجع CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, معلومات, SOCKS5_ST_AUTH_SEND);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_check_auth_resp(بنية socks_ctx *sx,
                                            بنية Curl_cfilter *cf,
                                            بنية Curl_easy *معلومات)
{
  ثابت بنانشان حرف *resp;
  بنانشان حرف auth_حالت;
  size_t rlen;

  (فراغ)cf;
  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(معلومات, "SOCKS5 sub-negotiation response incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_حالت = resp[1];
  إذا(auth_حالت) {
    failf(معلومات, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    أرجع CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_req1_init(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنانشان حرف req[5];
  بنانشان حرف ipbuf[16];
  ثابت بنانشان حرف *destination;
  بنانشان حرف desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode نتيجو;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  إذا(sx->resolve_local) {
    /* rest of request is added after resolving */
    نتيجو = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    إذا(نتيجو || (nwritten != 3))
      أرجع CURLPX_SEND_REQUEST;
    أرجع CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#جيڪڏهن_وصف_ٿيل USE_IPV6
  إذا(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    إذا(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      أرجع CURLPX_BAD_پتو_TYPE;
  }
  وإلا
#جيڪڏهن_پورو
  إذا(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  وإلا {
    ثابت size_t hostname_len = لڙي_ڊيگهه(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ثابت بنانشان حرف *)sx->dest->hostname;
    destlen = (بنانشان حرف)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  نتيجو = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  إذا(نتيجو || (nwritten != hdlen))
    أرجع CURLPX_SEND_REQUEST;
  نتيجو = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  إذا(نتيجو || (nwritten != destlen))
    أرجع CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (بنانشان حرف)((sx->dest->پورٽ >> 8) & 0xff);
  req[1] = (بنانشان حرف)(sx->dest->پورٽ & 0xff);
  نتيجو = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  إذا(نتيجو || (nwritten != 2))
    أرجع CURLPX_SEND_REQUEST;
  CURL_TRC_CF(معلومات, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->پورٽ);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_resolving(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *پورو)
{
  ثابت بنية Curl_addrinfo *ai = عدم;
  حرف dest[MAX_IPADR_LEN];  /* printable address */
  ثابت بنانشان حرف *destination = عدم;
  بنانشان حرف desttype = 1, destlen = 4;
  بنانشان حرف req[2];
  CURLcode نتيجو;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *پورو = FALSE;
  نتيجو = Curl_conn_dns_addr_نتيجو(cf->conn, cf->sockindex, sx->dest);
  إذا(نتيجو) {
    إذا(نتيجو != CURLE_AGAIN) {
      failf(معلومات, "error %d resolving SOCKS destination %s:%u",
            (صحيح)نتيجو, sx->dest->hostname, sx->dest->پورٽ);
      أرجع CURLPX_RESOLVE_HOST;
    }
    أرجع CURLPX_OK;
  }

#جيڪڏهن_وصف_ٿيل USE_IPV6
  إذا(معلومات->سيٽ.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(معلومات, sx->dest, cf->sockindex, AF_INET6, 0);
#جيڪڏهن_پورو
  إذا(!ai)
    ai = Curl_conn_dns_get_ai(معلومات, sx->dest, cf->sockindex, AF_INET, 0);

  إذا(!ai) {
    failf(معلومات, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    اذهب ٻاهريون;
  }

  Curl_printable_پتو(ai, dest, حجم(dest));

  إذا(ai->ai_family == AF_INET) {
    بنية sockaddr_in *saddr_اندريون;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_اندريون = (بنية sockaddr_in *)(فراغ *)ai->ai_addr;
    destination = (ثابت بنانشان حرف *)&saddr_اندريون->sin_addr.s_addr;
    CURL_TRC_CF(معلومات, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->پورٽ);
  }
#جيڪڏهن_وصف_ٿيل USE_IPV6
  وإلا إذا(ai->ai_family == AF_INET6) {
    بنية sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (بنية sockaddr_in6 *)(فراغ *)ai->ai_addr;
    destination = (ثابت بنانشان حرف *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(معلومات, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->پورٽ);
  }
#جيڪڏهن_پورو

  إذا(!destination) {
    failf(معلومات, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    اذهب ٻاهريون;
  }

  req[0] = desttype;
  نتيجو = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  إذا(نتيجو || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب ٻاهريون;
  }
  نتيجو = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  إذا(نتيجو || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب ٻاهريون;
  }
  /* PORT MSB+LSB */
  req[0] = (بنانشان حرف)((sx->dest->پورٽ >> 8) & 0xffU);
  req[1] = (بنانشان حرف)(sx->dest->پورٽ & 0xffU);
  نتيجو = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  إذا(نتيجو || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب ٻاهريون;
  }

ٻاهريون:
  *پورو = (presult == CURLPX_OK);
  أرجع presult;
}

ساكن CURLproxycode socks5_recv_resp1(بنية socks_ctx *sx,
                                       بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       bool *پورو)
{
  ثابت بنانشان حرف *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, معلومات, resp_len, پورو);
  إذا(presult)
    أرجع presult;
  وإلا إذا(!*پورو)
    أرجع CURLPX_OK;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(معلومات, "SOCKS5 response is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
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
  إذا(resp[0] != 5) { /*نسخو*/
    failf(معلومات, "SOCKS5 reply has wrong version, version should be 5.");
    أرجع CURLPX_BAD_نسخو;
  }
  وإلا إذا(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_جواب_UNASSIGNED;
    صحيح ڪوڊ = resp[1];
    failf(معلومات, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, ڪوڊ);
    إذا(ڪوڊ < 9) {
      /* RFC 1928 section 6 lists: */
      ساكن ثابت CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_جواب_GENERAL_SERVER_FAILURE,
        CURLPX_جواب_NOT_ALLOWED,
        CURLPX_جواب_ڄار_UNREACHABLE,
        CURLPX_جواب_HOST_UNREACHABLE,
        CURLPX_جواب_CONNECTION_REFUSED,
        CURLPX_جواب_TTL_EXPIRED,
        CURLPX_جواب_COMMAND_NOT_SUPPORTED,
        CURLPX_جواب_پتو_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[ڪوڊ];
    }
    أرجع rc;
  }

  /* Calculate real packet size */
  بدل(resp[3]) {
  حالة 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    اكسر;
  حالة 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    اكسر;
  حالة 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    اكسر;
  افتراضي:
    failf(معلومات, "SOCKS5 reply has wrong address type.");
    أرجع CURLPX_BAD_پتو_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, معلومات, resp_len, پورو);
  إذا(presult)
    أرجع presult;
  وإلا إذا(!*پورو)
    أرجع CURLPX_OK;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(معلومات, "SOCKS5 response is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *پورو = TRUE;
  أرجع CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ساكن CURLproxycode socks5_connect(بنية Curl_cfilter *cf,
                                    بنية socks_ctx *sx,
                                    بنية Curl_easy *معلومات)
{
  CURLproxycode presult;
  bool پورو;

process_state:
  بدل(sx->state) {
  حالة SOCKS_ST_INIT:
    sx->نسخو_2 = 5;
    sxstate(sx, cf, معلومات, SOCKS5_ST_شروعات);
    FALLTHROUGH();

  حالة SOCKS5_ST_شروعات:
    CURL_TRC_CF(معلومات, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->پورٽ);
    presult = socks5_req0_init(cf, sx, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    sxstate(sx, cf, معلومات, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, معلومات, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, معلومات, 2, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    /* socks5_check_resp0() sets next socks state */
    اذهب process_state;

  حالة SOCKS5_ST_GSSAPI_INIT: {
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode نتيجو = Curl_SOCKS5_gssapi_negotiate(cf, معلومات, sx->creds);
    إذا(نتيجو) {
      failf(معلومات, "Unable to negotiate SOCKS5 GSS-API context.");
      أرجع CURLPX_GSSAPI;
    }
    sxstate(sx, cf, معلومات, SOCKS5_ST_REQ1_INIT);
    اذهب process_state;
#وإلا
    failf(معلومات, "SOCKS5 GSSAPI per-message authentication is not supported.");
    أرجع socks_failed(sx, cf, معلومات, CURLPX_GSSAPI_PERMSG);
#جيڪڏهن_پورو
  }

  حالة SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    sxstate(sx, cf, معلومات, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    sxstate(sx, cf, معلومات, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, معلومات, 2, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, معلومات, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, معلومات);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    إذا(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, معلومات, SOCKS5_ST_REQ1_SEND);
      اذهب process_state;
    }
    sxstate(sx, cf, معلومات, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    إذا(!پورو)
      أرجع CURLPX_OK;
    sxstate(sx, cf, معلومات, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    وإلا إذا(!پورو)
      أرجع CURLPX_OK;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    إذا(cf->conn->socks5_gssapi_enctype) {
      failf(معلومات, "SOCKS5 GSS-API protection not yet implemented.");
      أرجع CURLPX_GSSAPI_PROTECTION;
    }
#جيڪڏهن_پورو
    sxstate(sx, cf, معلومات, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, معلومات, &پورو);
    إذا(presult)
      أرجع socks_failed(sx, cf, معلومات, presult);
    إذا(!پورو)
      أرجع CURLPX_OK;
    CURL_TRC_CF(معلومات, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, معلومات, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالة SOCKS_ST_SUCCESS:
    أرجع CURLPX_OK;

  حالة SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    أرجع sx->presult;

  افتراضي:
    DEBUGASSERT(0);
    أرجع socks_failed(sx, cf, معلومات, CURLPX_SEND_REQUEST);
  }
}

ساكن فراغ socks_proxy_ctx_free(بنية socks_ctx *ctx)
{
  إذا(ctx) {
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
ساكن CURLcode socks_proxy_cf_connect(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       bool *پورو)
{
  بنية socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode نتيجو;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
  إذا(نتيجو || !*پورو)
    أرجع نتيجو;

  بدل(ctx->proxy_type) {
  حالة CURLPROXY_SOCKS5:
  حالة CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, معلومات);
    اكسر;

  حالة CURLPROXY_SOCKS4:
  حالة CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, معلومات);
    اكسر;

  افتراضي:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    نتيجو = CURLE_COULDNT_CONNECT;
    اذهب ٻاهريون;
  }

  إذا(pxresult) {
    نتيجو = CURLE_PROXY;
    معلومات->info.pxcode = (uint8_t)pxresult;
    اذهب ٻاهريون;
  }
  وإلا إذا(ctx->state != SOCKS_ST_SUCCESS)
    اذهب ٻاهريون;

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_is_verbose(معلومات)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    إذا(!Curl_conn_cf_get_ip_info(cf->ايندڙ, معلومات, &is_ipv6, &ipquad))
      infof(معلومات, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_پورٽ,
            ctx->dest->hostname, ctx->dest->پورٽ,
            ipquad.remote_ip, ipquad.remote_پورٽ);
    وإلا
      infof(معلومات, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#جيڪڏهن_پورو
  cf->connected = TRUE;

ٻاهريون:
  *پورو = (bool)cf->connected;
  إذا(*پورو || نتيجو)
    Curl_creds_unlink(&ctx->creds);
  أرجع نتيجو;
}

ساكن CURLcode socks_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية easy_pollset *ps)
{
  بنية socks_ctx *sx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
    بدل(sx->state) {
    حالة SOCKS4_ST_SEND:
    حالة SOCKS5_ST_REQ0_SEND:
    حالة SOCKS5_ST_AUTH_SEND:
    حالة SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(معلومات, cf, "adjust pollset out (%d)", (صحيح)sx->state);
      نتيجو = Curl_pollset_سيٽ_ٻاهريون_only(معلومات, ps, sock);
      اكسر;
    افتراضي:
      CURL_TRC_CF(معلومات, cf, "adjust pollset in (%d)", (صحيح)sx->state);
      نتيجو = Curl_pollset_سيٽ_اندريون_only(معلومات, ps, sock);
      اكسر;
    }
  }
  أرجع نتيجو;
}

ساكن فراغ socks_proxy_cf_destroy(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  (فراغ)معلومات;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = عدم;
}

ساكن CURLcode socks_cf_query(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية socks_ctx *sx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_پورٽ:
    إذا(sx) {
      *pres1 = sx->dest->پورٽ;
      *((ثابت حرف **)pres2) = sx->dest->hostname;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

بنية Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                          بنية Curl_easy *معلومات,
                                          بنية Curl_peer *dest,
                                          uint8_t ip_نسخو,
                                          uint8_t proxy_type,
                                          بنية Curl_creds *creds)
{
  بنية Curl_cfilter *cf;
  بنية socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(معلومات, ip_نسخو);
  CURLcode نتيجو;

  إذا(!dest)
    أرجع CURLE_FAILED_INIT;

  بدل(proxy_type) {
  حالة CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    اكسر;
  حالة CURLPROXY_SOCKS5_HOSTNAME:
    اكسر;
  حالة CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    اكسر;
  حالة CURLPROXY_SOCKS4A:
    اكسر;
  افتراضي:
    failf(معلومات, "unknown proxytype %d option given", proxy_type);
    أرجع CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_نسخو = ip_نسخو;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_سائيز, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);

  نتيجو = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  إذا(!نتيجو) {
    Curl_conn_cf_وجهو_after(cf_at, cf);
    إذا(ctx->resolve_local) {
      نتيجو = Curl_conn_dns_جوڙيو_addr_resolve(معلومات, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  وإلا
    socks_proxy_ctx_free(ctx);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */
