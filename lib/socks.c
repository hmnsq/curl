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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "bufq.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "select.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "socks.h"
#شامل "vdns/cf-dns.h"
#شامل "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
شمار socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_آغاز,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_آغاز,
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

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساکن ثابت حرف * ثابت cf_socks_statename[] = {
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
#اگر_ختم

#تعریف_کرو SOCKS_CHUNK_سائز    1024
#تعریف_کرو SOCKS_CHUNKS        1

ساخت socks_ctx {
  شمار socks_state_t state;
  ساخت bufq iobuf;
  ساخت Curl_peer *dest;
  ساخت Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_نسخہ;
  uint8_t proxy_type;
  غیرعلامتی حرف نسخہ_2;
  ثنائی_ہندسہ(resolve_local);
  ثنائی_ہندسہ(socks4a);
};

#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            حرف *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode نتیجہ;

  *pnread = 0;
  برائے(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(ڈیٹا);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ڈیٹا);

    اگر(timeout_ms < 0) {
      /* we already got the timeout */
      واپس CURLE_OPERATION_TIMEDOUT;
    }
    اگر(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    اگر(SOCKET_READABLE(sock, timeout_ms) <= 0)
      واپس CURLE_OPERATION_TIMEDOUT;
    نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا, buf, blen, &nread);
    اگر(نتیجہ == CURLE_AGAIN)
      جاری;
    ورنہ اگر(نتیجہ)
      واپس نتیجہ;

    اگر(blen == nread) {
      *pnread += nread;
      واپس CURLE_OK;
    }
    اگر(!nread) /* EOF */
      واپس CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#اگر_ختم

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
#تعریف_کرو sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ورنہ
#تعریف_کرو sxstate(x, c, d, y) socksstate(x, c, d, y)
#اگر_ختم

/* always use this function to change state, to make debugging easier */
ساکن خالی socksstate(ساخت socks_ctx *sx,
                       ساخت Curl_cfilter *cf,
                       ساخت Curl_easy *ڈیٹا,
                       شمار socks_state_t state
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , صحیح lineno
#اگر_ختم
)
{
  شمار socks_state_t oldstate = sx->state;

  اگر(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    واپس;

  sx->state = state;

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(ڈیٹا, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ورنہ
  (خالی)cf;
  (خالی)ڈیٹا;
#اگر_ختم
}

ساکن CURLproxycode socks_failed(ساخت socks_ctx *sx,
                                  ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, ڈیٹا, SOCKS_ST_FAILED);
  sx->presult = presult;
  واپس presult;
}

ساکن CURLproxycode socks_flush(ساخت socks_ctx *sx,
                                 ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool *مکمل)
{
  CURLcode نتیجہ;
  size_t nwritten;

  *مکمل = FALSE;
  جب(!Curl_bufq_is_خالی_خانے(&sx->iobuf)) {
    نتیجہ = Curl_cf_send_bufq(cf->اگلا, ڈیٹا, &sx->iobuf, تہی, 0,
                               &nwritten);
    اگر(نتیجہ == CURLE_AGAIN)
      واپس CURLPX_OK;
    ورنہ اگر(نتیجہ) {
      failf(ڈیٹا, "Failed to send SOCKS request: %s",
            curl_easy_strerror(نتیجہ));
      واپس socks_failed(sx, cf, ڈیٹا, CURLPX_SEND_CONNECT);
    }
  }
  *مکمل = TRUE;
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks_recv(ساخت socks_ctx *sx,
                                ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                size_t min_بائٹیں,
                                bool *مکمل)
{
  CURLcode نتیجہ;
  size_t nread;

  *مکمل = FALSE;
  جب(Curl_bufq_len(&sx->iobuf) < min_بائٹیں) {
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &sx->iobuf,
                               min_بائٹیں - Curl_bufq_len(&sx->iobuf),
                               &nread);
    اگر(نتیجہ == CURLE_AGAIN)
      واپس CURLPX_OK;
    ورنہ اگر(نتیجہ) {
      failf(ڈیٹا, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(نتیجہ));
      واپس CURLPX_RECV_CONNECT;
    }
    ورنہ اگر(!nread) { /* EOF */
      اگر(Curl_bufq_len(&sx->iobuf) < min_بائٹیں) {
        failf(ڈیٹا, "Failed to receive SOCKS response, "
              "proxy closed connection");
        واپس CURLPX_RECV_CONNECT;
      }
      توڑو;
    }
  }
  *مکمل = TRUE;
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks4_req_جوڑو_hd(ساخت socks_ctx *sx,
                                       ساخت Curl_easy *ڈیٹا)
{
  غیرعلامتی حرف buf[4];
  size_t nwritten;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (غیرعلامتی حرف)((sx->dest->پورٹ >> 8) & 0xffU); /* MSB */
  buf[3] = (غیرعلامتی حرف)(sx->dest->پورٹ & 0xffU);        /* LSB */

  نتیجہ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  اگر(نتیجہ || (nwritten != 4))
    واپس CURLPX_SEND_REQUEST;
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks4_req_جوڑو_user(ساخت socks_ctx *sx,
                                         ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  size_t nwritten;

  اگر(sx->creds) {
    size_t plen = لڑی_لمبائی(sx->creds->user);
    اگر(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(ڈیٹا, "Too long SOCKS proxy username");
      واپس CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    نتیجہ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    اگر(نتیجہ || (nwritten != (plen + 1)))
      واپس CURLPX_SEND_REQUEST;
  }
  ورنہ {
    /* empty username */
    غیرعلامتی حرف b = 0;
    نتیجہ = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    اگر(نتیجہ || (nwritten != 1))
      واپس CURLPX_SEND_REQUEST;
  }
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks4_resolving(ساخت socks_ctx *sx,
                                      ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *مکمل)
{
  ثابت ساخت Curl_addrinfo *ai = تہی;
  CURLcode نتیجہ;
  size_t nwritten;

  *مکمل = FALSE;
  نتیجہ = Curl_conn_dns_addr_نتیجہ(cf->conn, cf->sockindex, sx->dest);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_AGAIN) {
      failf(ڈیٹا, "error %d resolving SOCKS destination %s:%u",
            (صحیح)نتیجہ, sx->dest->hostname, sx->dest->پورٹ);
      واپس CURLPX_RESOLVE_HOST;
    }
    واپس CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(ڈیٹا, sx->dest, cf->sockindex, AF_INET, 0);
  اگر(ai) {
    ساخت sockaddr_in *saddr_اندر;
    حرف ipbuf[64];

    Curl_printable_پتہ(ai, ipbuf, حجم(ipbuf));
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_اندر = (ساخت sockaddr_in *)(خالی *)ai->ai_addr;
    نتیجہ = Curl_bufq_write(&sx->iobuf,
                             (غیرعلامتی حرف *)&saddr_اندر->sin_addr.s_addr, 4,
                             &nwritten);

    اگر(نتیجہ || (nwritten != 4))
      واپس CURLPX_SEND_REQUEST;
  }
  ورنہ {
    /* No IPv4 address resolved */
    failf(ڈیٹا, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    واپس CURLPX_RESOLVE_HOST;
  }

  *مکمل = TRUE;
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks4_check_resp(ساخت socks_ctx *sx,
                                       ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا)
{
  ثابت غیرعلامتی حرف *resp;
  size_t rlen;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(ڈیٹا, "SOCKS4 reply is incomplete.");
    واپس CURLPX_RECV_CONNECT;
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
  اگر(resp[0]) {
    failf(ڈیٹا, "SOCKS4 reply has wrong version, version should be 0.");
    واپس CURLPX_BAD_نسخہ;
  }

  /*نتیجہ*/
  انتخاب(resp[1]) {
  حالت 90:
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    واپس CURLPX_OK;
  حالت 91:
    failf(ڈیٹا,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (غیرعلامتی صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    واپس CURLPX_REQUEST_FAILED;
  حالت 92:
    failf(ڈیٹا,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (غیرعلامتی صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    واپس CURLPX_IDENTD;
  حالت 93:
    failf(ڈیٹا,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (غیرعلامتی صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    واپس CURLPX_IDENTD_فرق;
  طےشدہ:
    failf(ڈیٹا,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (غیرعلامتی صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    واپس CURLPX_UNKNOWN_FAIL;
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
ساکن CURLproxycode socks4_connect(ساخت Curl_cfilter *cf,
                                    ساخت socks_ctx *sx,
                                    ساخت Curl_easy *ڈیٹا)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode نتیجہ;
  bool مکمل;

process_state:
  انتخاب(sx->state) {
  حالت SOCKS_ST_INIT:
    sx->نسخہ_2 = 4;
    sxstate(sx, cf, ڈیٹا, SOCKS4_ST_آغاز);
    FALLTHROUGH();

  حالت SOCKS4_ST_آغاز:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_نسخہ = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->پورٹ);

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
    presult = socks4_req_جوڑو_hd(sx, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    اگر(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      غیرعلامتی حرف buf[4] = { 0, 0, 0, 1 };
      size_t hlen = لڑی_لمبائی(sx->dest->hostname) + 1; /* including NUL */

      اگر(hlen > 255) {
        failf(ڈیٹا, "SOCKS4: too long hostname");
        واپس socks_failed(sx, cf, ڈیٹا, CURLPX_LONG_HOSTNAME);
      }
      نتیجہ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      اگر(نتیجہ || (nwritten != 4))
        واپس socks_failed(sx, cf, ڈیٹا, CURLPX_SEND_REQUEST);
      presult = socks4_req_جوڑو_user(sx, ڈیٹا);
      اگر(presult)
        واپس socks_failed(sx, cf, ڈیٹا, presult);
      نتیجہ = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      اگر(نتیجہ || (nwritten != hlen))
        واپس socks_failed(sx, cf, ڈیٹا, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, ڈیٹا, SOCKS4_ST_SEND);
      جاؤ process_state;
    }
    sxstate(sx, cf, ڈیٹا, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  حالت SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    اگر(!مکمل)
      واپس CURLPX_OK;
    /* append user */
    presult = socks4_req_جوڑو_user(sx, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    sxstate(sx, cf, ڈیٹا, SOCKS4_ST_SEND);
    FALLTHROUGH();

  حالت SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    sxstate(sx, cf, ڈیٹا, SOCKS4_ST_RECV);
    FALLTHROUGH();

  حالت SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, ڈیٹا, 8, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    presult = socks4_check_resp(sx, cf, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    sxstate(sx, cf, ڈیٹا, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالت SOCKS_ST_SUCCESS:
    واپس CURLPX_OK;

  حالت SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    واپس sx->presult;

  طےشدہ:
    DEBUGASSERT(0);
    واپس socks_failed(sx, cf, ڈیٹا, CURLPX_SEND_REQUEST);
  }
}

ساکن CURLproxycode socks5_req0_init(ساخت Curl_cfilter *cf,
                                      ساخت socks_ctx *sx,
                                      ساخت Curl_easy *ڈیٹا)
{
  ثابت غیرعلامتی حرف auth = ڈیٹا->سیٹ.socks5auth;
  غیرعلامتی حرف req[5]; /* version + len + 3 possible auth methods */
  غیرعلامتی حرف nauths;
  size_t req_len, nwritten;
  CURLcode نتیجہ;

  (خالی)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  اگر(!sx->resolve_local && لڑی_لمبائی(sx->dest->hostname) > 255) {
    failf(ڈیٹا, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    واپس CURLPX_LONG_HOSTNAME;
  }

  اگر(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(ڈیٹا, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  اگر(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  اگر(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->بٹ.socks5_authenticated = FALSE;

  req[0] = 5;   /*نسخہ*/
  nauths = 0;
  اگر(!ڈیٹا->سیٹ.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  اگر(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#اگر_ختم
  اگر(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  اگر(!nauths) {
    failf(ڈیٹا, "SOCKS5: no acceptable authentication method is available.");
    واپس CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  نتیجہ = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  اگر(نتیجہ || (nwritten != req_len))
    واپس CURLPX_SEND_REQUEST;
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks5_check_resp0(ساخت socks_ctx *sx,
                                        ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا)
{
  ثابت غیرعلامتی حرف *resp;
  غیرعلامتی حرف auth_منوال;
  size_t rlen;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ڈیٹا, "SOCKS5 initial reply is incomplete.");
    واپس CURLPX_RECV_CONNECT;
  }

  اگر(resp[0] != 5) {
    failf(ڈیٹا, "Received invalid version in initial SOCKS5 response.");
    واپس CURLPX_BAD_نسخہ;
  }

  auth_منوال = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  انتخاب(auth_منوال) {
  حالت 0:
    اگر(ڈیٹا->سیٹ.socks5_auth_only) {
      failf(ڈیٹا, "SOCKS5 proxy selected no authentication.");
      واپس CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ1_INIT);
    واپس CURLPX_OK;
  حالت 1:
    اگر(ڈیٹا->سیٹ.socks5auth & CURLAUTH_GSSAPI) {
      اگر(cf->sockindex == FIRSTSOCKET)
        cf->conn->بٹ.socks5_authenticated = TRUE;
      sxstate(sx, cf, ڈیٹا, SOCKS5_ST_GSSAPI_INIT);
      واپس CURLPX_OK;
    }
    failf(ڈیٹا, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    واپس CURLPX_GSSAPI_PERMSG;
  حالت 2:
    /* regular name + password authentication */
    اگر(ڈیٹا->سیٹ.socks5auth & CURLAUTH_BASIC) {
      اگر(cf->sockindex == FIRSTSOCKET)
        cf->conn->بٹ.socks5_authenticated = TRUE;
      sxstate(sx, cf, ڈیٹا, SOCKS5_ST_AUTH_INIT);
      واپس CURLPX_OK;
    }
    failf(ڈیٹا, "BASIC authentication proposed but not enabled.");
    واپس CURLPX_NO_AUTH;
  حالت 255:
    failf(ڈیٹا, "No authentication method was acceptable.");
    واپس CURLPX_NO_AUTH;
  طےشدہ:
    failf(ڈیٹا, "Unknown SOCKS5 mode attempted to be used by server.");
    واپس CURLPX_UNKNOWN_منوال;
  }
}

ساکن CURLproxycode socks5_auth_init(ساخت Curl_cfilter *cf,
                                      ساخت socks_ctx *sx,
                                      ساخت Curl_easy *ڈیٹا)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  غیرعلامتی حرف buf[2];
  CURLcode نتیجہ;

  اگر(sx->creds) {
    ulen = لڑی_لمبائی(sx->creds->user);
    plen = لڑی_لمبائی(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    اگر(ulen > 255) {
      failf(ڈیٹا, "Excessive username length for proxy auth");
      واپس CURLPX_LONG_USER;
    }
    اگر(plen > 255) {
      failf(ڈیٹا, "Excessive password length for proxy auth");
      واپس CURLPX_LONG_PASSWD;
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
  buf[1] = (غیرعلامتی حرف)ulen;
  نتیجہ = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  اگر(نتیجہ || (nwritten != 2))
    واپس CURLPX_SEND_REQUEST;
  اگر(ulen) {
    نتیجہ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    اگر(نتیجہ || (nwritten != ulen))
      واپس CURLPX_SEND_REQUEST;
  }
  buf[0] = (غیرعلامتی حرف)plen;
  نتیجہ = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  اگر(نتیجہ || (nwritten != 1))
    واپس CURLPX_SEND_REQUEST;
  اگر(plen) {
    نتیجہ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    اگر(نتیجہ || (nwritten != plen))
      واپس CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, ڈیٹا, SOCKS5_ST_AUTH_SEND);
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks5_check_auth_resp(ساخت socks_ctx *sx,
                                            ساخت Curl_cfilter *cf,
                                            ساخت Curl_easy *ڈیٹا)
{
  ثابت غیرعلامتی حرف *resp;
  غیرعلامتی حرف auth_حالت;
  size_t rlen;

  (خالی)cf;
  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ڈیٹا, "SOCKS5 sub-negotiation response incomplete.");
    واپس CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_حالت = resp[1];
  اگر(auth_حالت) {
    failf(ڈیٹا, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    واپس CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks5_req1_init(ساخت socks_ctx *sx,
                                      ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  غیرعلامتی حرف req[5];
  غیرعلامتی حرف ipbuf[16];
  ثابت غیرعلامتی حرف *destination;
  غیرعلامتی حرف desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode نتیجہ;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  اگر(sx->resolve_local) {
    /* rest of request is added after resolving */
    نتیجہ = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    اگر(نتیجہ || (nwritten != 3))
      واپس CURLPX_SEND_REQUEST;
    واپس CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#اگر_تعریف_شدہ USE_IPV6
  اگر(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    اگر(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      واپس CURLPX_BAD_پتہ_TYPE;
  }
  ورنہ
#اگر_ختم
  اگر(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ورنہ {
    ثابت size_t hostname_len = لڑی_لمبائی(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ثابت غیرعلامتی حرف *)sx->dest->hostname;
    destlen = (غیرعلامتی حرف)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  نتیجہ = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  اگر(نتیجہ || (nwritten != hdlen))
    واپس CURLPX_SEND_REQUEST;
  نتیجہ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  اگر(نتیجہ || (nwritten != destlen))
    واپس CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (غیرعلامتی حرف)((sx->dest->پورٹ >> 8) & 0xff);
  req[1] = (غیرعلامتی حرف)(sx->dest->پورٹ & 0xff);
  نتیجہ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  اگر(نتیجہ || (nwritten != 2))
    واپس CURLPX_SEND_REQUEST;
  CURL_TRC_CF(ڈیٹا, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->پورٹ);
  واپس CURLPX_OK;
}

ساکن CURLproxycode socks5_resolving(ساخت socks_ctx *sx,
                                      ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *مکمل)
{
  ثابت ساخت Curl_addrinfo *ai = تہی;
  حرف dest[MAX_IPADR_LEN];  /* printable address */
  ثابت غیرعلامتی حرف *destination = تہی;
  غیرعلامتی حرف desttype = 1, destlen = 4;
  غیرعلامتی حرف req[2];
  CURLcode نتیجہ;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *مکمل = FALSE;
  نتیجہ = Curl_conn_dns_addr_نتیجہ(cf->conn, cf->sockindex, sx->dest);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_AGAIN) {
      failf(ڈیٹا, "error %d resolving SOCKS destination %s:%u",
            (صحیح)نتیجہ, sx->dest->hostname, sx->dest->پورٹ);
      واپس CURLPX_RESOLVE_HOST;
    }
    واپس CURLPX_OK;
  }

#اگر_تعریف_شدہ USE_IPV6
  اگر(ڈیٹا->سیٹ.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(ڈیٹا, sx->dest, cf->sockindex, AF_INET6, 0);
#اگر_ختم
  اگر(!ai)
    ai = Curl_conn_dns_get_ai(ڈیٹا, sx->dest, cf->sockindex, AF_INET, 0);

  اگر(!ai) {
    failf(ڈیٹا, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    جاؤ برآمد;
  }

  Curl_printable_پتہ(ai, dest, حجم(dest));

  اگر(ai->ai_family == AF_INET) {
    ساخت sockaddr_in *saddr_اندر;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_اندر = (ساخت sockaddr_in *)(خالی *)ai->ai_addr;
    destination = (ثابت غیرعلامتی حرف *)&saddr_اندر->sin_addr.s_addr;
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->پورٹ);
  }
#اگر_تعریف_شدہ USE_IPV6
  ورنہ اگر(ai->ai_family == AF_INET6) {
    ساخت sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ساخت sockaddr_in6 *)(خالی *)ai->ai_addr;
    destination = (ثابت غیرعلامتی حرف *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->پورٹ);
  }
#اگر_ختم

  اگر(!destination) {
    failf(ڈیٹا, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    جاؤ برآمد;
  }

  req[0] = desttype;
  نتیجہ = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  اگر(نتیجہ || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    جاؤ برآمد;
  }
  نتیجہ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  اگر(نتیجہ || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    جاؤ برآمد;
  }
  /* PORT MSB+LSB */
  req[0] = (غیرعلامتی حرف)((sx->dest->پورٹ >> 8) & 0xffU);
  req[1] = (غیرعلامتی حرف)(sx->dest->پورٹ & 0xffU);
  نتیجہ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  اگر(نتیجہ || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    جاؤ برآمد;
  }

برآمد:
  *مکمل = (presult == CURLPX_OK);
  واپس presult;
}

ساکن CURLproxycode socks5_recv_resp1(ساخت socks_ctx *sx,
                                       ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       bool *مکمل)
{
  ثابت غیرعلامتی حرف *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, ڈیٹا, resp_len, مکمل);
  اگر(presult)
    واپس presult;
  ورنہ اگر(!*مکمل)
    واپس CURLPX_OK;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ڈیٹا, "SOCKS5 response is incomplete.");
    واپس CURLPX_RECV_CONNECT;
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
  اگر(resp[0] != 5) { /*نسخہ*/
    failf(ڈیٹا, "SOCKS5 reply has wrong version, version should be 5.");
    واپس CURLPX_BAD_نسخہ;
  }
  ورنہ اگر(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_جواب_UNASSIGNED;
    صحیح رمز = resp[1];
    failf(ڈیٹا, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, رمز);
    اگر(رمز < 9) {
      /* RFC 1928 section 6 lists: */
      ساکن ثابت CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_جواب_GENERAL_SERVER_FAILURE,
        CURLPX_جواب_NOT_ALLOWED,
        CURLPX_جواب_جال_UNREACHABLE,
        CURLPX_جواب_HOST_UNREACHABLE,
        CURLPX_جواب_CONNECTION_REFUSED,
        CURLPX_جواب_TTL_EXPIRED,
        CURLPX_جواب_COMMAND_NOT_SUPPORTED,
        CURLPX_جواب_پتہ_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[رمز];
    }
    واپس rc;
  }

  /* Calculate real packet size */
  انتخاب(resp[3]) {
  حالت 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    توڑو;
  حالت 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    توڑو;
  حالت 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "SOCKS5 reply has wrong address type.");
    واپس CURLPX_BAD_پتہ_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, ڈیٹا, resp_len, مکمل);
  اگر(presult)
    واپس presult;
  ورنہ اگر(!*مکمل)
    واپس CURLPX_OK;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ڈیٹا, "SOCKS5 response is incomplete.");
    واپس CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *مکمل = TRUE;
  واپس CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ساکن CURLproxycode socks5_connect(ساخت Curl_cfilter *cf,
                                    ساخت socks_ctx *sx,
                                    ساخت Curl_easy *ڈیٹا)
{
  CURLproxycode presult;
  bool مکمل;

process_state:
  انتخاب(sx->state) {
  حالت SOCKS_ST_INIT:
    sx->نسخہ_2 = 5;
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_آغاز);
    FALLTHROUGH();

  حالت SOCKS5_ST_آغاز:
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->پورٹ);
    presult = socks5_req0_init(cf, sx, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, ڈیٹا, 2, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    /* socks5_check_resp0() sets next socks state */
    جاؤ process_state;

  حالت SOCKS5_ST_GSSAPI_INIT: {
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode نتیجہ = Curl_SOCKS5_gssapi_negotiate(cf, ڈیٹا, sx->creds);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Unable to negotiate SOCKS5 GSS-API context.");
      واپس CURLPX_GSSAPI;
    }
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ1_INIT);
    جاؤ process_state;
#ورنہ
    failf(ڈیٹا, "SOCKS5 GSSAPI per-message authentication is not supported.");
    واپس socks_failed(sx, cf, ڈیٹا, CURLPX_GSSAPI_PERMSG);
#اگر_ختم
  }

  حالت SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, ڈیٹا, 2, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, ڈیٹا);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    اگر(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ1_SEND);
      جاؤ process_state;
    }
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    اگر(!مکمل)
      واپس CURLPX_OK;
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    ورنہ اگر(!مکمل)
      واپس CURLPX_OK;
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    اگر(cf->conn->socks5_gssapi_enctype) {
      failf(ڈیٹا, "SOCKS5 GSS-API protection not yet implemented.");
      واپس CURLPX_GSSAPI_PROTECTION;
    }
#اگر_ختم
    sxstate(sx, cf, ڈیٹا, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, ڈیٹا, &مکمل);
    اگر(presult)
      واپس socks_failed(sx, cf, ڈیٹا, presult);
    اگر(!مکمل)
      واپس CURLPX_OK;
    CURL_TRC_CF(ڈیٹا, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, ڈیٹا, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالت SOCKS_ST_SUCCESS:
    واپس CURLPX_OK;

  حالت SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    واپس sx->presult;

  طےشدہ:
    DEBUGASSERT(0);
    واپس socks_failed(sx, cf, ڈیٹا, CURLPX_SEND_REQUEST);
  }
}

ساکن خالی socks_proxy_ctx_free(ساخت socks_ctx *ctx)
{
  اگر(ctx) {
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
ساکن CURLcode socks_proxy_cf_connect(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       bool *مکمل)
{
  ساخت socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode نتیجہ;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
  اگر(نتیجہ || !*مکمل)
    واپس نتیجہ;

  انتخاب(ctx->proxy_type) {
  حالت CURLPROXY_SOCKS5:
  حالت CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, ڈیٹا);
    توڑو;

  حالت CURLPROXY_SOCKS4:
  حالت CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, ڈیٹا);
    توڑو;

  طےشدہ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    نتیجہ = CURLE_COULDNT_CONNECT;
    جاؤ برآمد;
  }

  اگر(pxresult) {
    نتیجہ = CURLE_PROXY;
    ڈیٹا->info.pxcode = (uint8_t)pxresult;
    جاؤ برآمد;
  }
  ورنہ اگر(ctx->state != SOCKS_ST_SUCCESS)
    جاؤ برآمد;

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    ساخت ip_quadruple ipquad;
    bool is_ipv6;
    اگر(!Curl_conn_cf_get_ip_info(cf->اگلا, ڈیٹا, &is_ipv6, &ipquad))
      infof(ڈیٹا, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_پورٹ,
            ctx->dest->hostname, ctx->dest->پورٹ,
            ipquad.remote_ip, ipquad.remote_پورٹ);
    ورنہ
      infof(ڈیٹا, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#اگر_ختم
  cf->connected = TRUE;

برآمد:
  *مکمل = (bool)cf->connected;
  اگر(*مکمل || نتیجہ)
    Curl_creds_unlink(&ctx->creds);
  واپس نتیجہ;
}

ساکن CURLcode socks_cf_adjust_pollset(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت easy_pollset *ps)
{
  ساخت socks_ctx *sx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ڈیٹا);
    انتخاب(sx->state) {
    حالت SOCKS4_ST_SEND:
    حالت SOCKS5_ST_REQ0_SEND:
    حالت SOCKS5_ST_AUTH_SEND:
    حالت SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(ڈیٹا, cf, "adjust pollset out (%d)", (صحیح)sx->state);
      نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, sock);
      توڑو;
    طےشدہ:
      CURL_TRC_CF(ڈیٹا, cf, "adjust pollset in (%d)", (صحیح)sx->state);
      نتیجہ = Curl_pollset_سیٹ_اندر_only(ڈیٹا, ps, sock);
      توڑو;
    }
  }
  واپس نتیجہ;
}

ساکن خالی socks_proxy_cf_destroy(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  (خالی)ڈیٹا;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = تہی;
}

ساکن CURLcode socks_cf_query(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت socks_ctx *sx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_HOST_پورٹ:
    اگر(sx) {
      *pres1 = sx->dest->پورٹ;
      *((ثابت حرف **)pres2) = sx->dest->hostname;
      واپس CURLE_OK;
    }
    توڑو;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساخت Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                          ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_peer *dest,
                                          uint8_t ip_نسخہ,
                                          uint8_t proxy_type,
                                          ساخت Curl_creds *creds)
{
  ساخت Curl_cfilter *cf;
  ساخت socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(ڈیٹا, ip_نسخہ);
  CURLcode نتیجہ;

  اگر(!dest)
    واپس CURLE_FAILED_INIT;

  انتخاب(proxy_type) {
  حالت CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    توڑو;
  حالت CURLPROXY_SOCKS5_HOSTNAME:
    توڑو;
  حالت CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    توڑو;
  حالت CURLPROXY_SOCKS4A:
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "unknown proxytype %d option given", proxy_type);
    واپس CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    واپس CURLE_برآمد_کا_حافظہ;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_نسخہ = ip_نسخہ;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_سائز, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  اگر(!نتیجہ) {
    Curl_conn_cf_داخل_کرو_after(cf_at, cf);
    اگر(ctx->resolve_local) {
      نتیجہ = Curl_conn_dns_جوڑو_addr_resolve(ڈیٹا, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ورنہ
    socks_proxy_ctx_free(ctx);
  واپس نتیجہ;
}

#اگر_ختم /* CURL_DISABLE_PROXY */
