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

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
गणना socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_START,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_START,
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

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
स्थिर ध्रुव वर्ण * ध्रुव cf_socks_statename[] = {
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
#यदि_अन्तः

#परिभाषय SOCKS_CHUNK_आकार    1024
#परिभाषय SOCKS_CHUNKS        1

संरचना socks_ctx {
  गणना socks_state_t state;
  संरचना bufq iobuf;
  संरचना Curl_peer *dest;
  संरचना Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_संस्करणम्;
  uint8_t proxy_type;
  अचिह्नित वर्ण संस्करणम्_2;
  BIT(resolve_local);
  BIT(socks4a);
};

#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            वर्ण *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode परिणाम;

  *pnread = 0;
  प्रति(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(दत्त);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, दत्त);

    यदि(timeout_ms < 0) {
      /* we already got the timeout */
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    }
    यदि(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    यदि(SOCKET_READABLE(sock, timeout_ms) <= 0)
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त, buf, blen, &nread);
    यदि(परिणाम == CURLE_AGAIN)
      अनुवर्त;
    अन्यथा यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि(blen == nread) {
      *pnread += nread;
      प्रत्यागम CURLE_OK;
    }
    यदि(!nread) /* EOF */
      प्रत्यागम CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#यदि_अन्तः

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
#परिभाषय sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#अन्यथा
#परिभाषय sxstate(x, c, d, y) socksstate(x, c, d, y)
#यदि_अन्तः

/* always use this function to change state, to make debugging easier */
स्थिर शून्य socksstate(संरचना socks_ctx *sx,
                       संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *दत्त,
                       गणना socks_state_t state
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , पूर्णाङ्क lineno
#यदि_अन्तः
)
{
  गणना socks_state_t oldstate = sx->state;

  यदि(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    प्रत्यागम;

  sx->state = state;

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(दत्त, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#अन्यथा
  (शून्य)cf;
  (शून्य)दत्त;
#यदि_अन्तः
}

स्थिर CURLproxycode socks_failed(संरचना socks_ctx *sx,
                                  संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, दत्त, SOCKS_ST_FAILED);
  sx->presult = presult;
  प्रत्यागम presult;
}

स्थिर CURLproxycode socks_flush(संरचना socks_ctx *sx,
                                 संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool *समाप्तम्)
{
  CURLcode परिणाम;
  size_t nwritten;

  *समाप्तम् = FALSE;
  यावत्(!Curl_bufq_is_रिक्तानि(&sx->iobuf)) {
    परिणाम = Curl_cf_send_bufq(cf->अग्र, दत्त, &sx->iobuf, रिक्त, 0,
                               &nwritten);
    यदि(परिणाम == CURLE_AGAIN)
      प्रत्यागम CURLPX_OK;
    अन्यथा यदि(परिणाम) {
      failf(दत्त, "Failed to send SOCKS request: %s",
            curl_easy_strerror(परिणाम));
      प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_SEND_CONNECT);
    }
  }
  *समाप्तम् = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks_recv(संरचना socks_ctx *sx,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                size_t min_अष्टकानि,
                                bool *समाप्तम्)
{
  CURLcode परिणाम;
  size_t nread;

  *समाप्तम् = FALSE;
  यावत्(Curl_bufq_len(&sx->iobuf) < min_अष्टकानि) {
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &sx->iobuf,
                               min_अष्टकानि - Curl_bufq_len(&sx->iobuf),
                               &nread);
    यदि(परिणाम == CURLE_AGAIN)
      प्रत्यागम CURLPX_OK;
    अन्यथा यदि(परिणाम) {
      failf(दत्त, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(परिणाम));
      प्रत्यागम CURLPX_RECV_CONNECT;
    }
    अन्यथा यदि(!nread) { /* EOF */
      यदि(Curl_bufq_len(&sx->iobuf) < min_अष्टकानि) {
        failf(दत्त, "Failed to receive SOCKS response, "
              "proxy closed connection");
        प्रत्यागम CURLPX_RECV_CONNECT;
      }
      भङ्ग;
    }
  }
  *समाप्तम् = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_req_सङ्कलय_hd(संरचना socks_ctx *sx,
                                       संरचना Curl_easy *दत्त)
{
  अचिह्नित वर्ण buf[4];
  size_t nwritten;
  CURLcode परिणाम;

  (शून्य)दत्त;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (अचिह्नित वर्ण)((sx->dest->पत्तनम् >> 8) & 0xffU); /* MSB */
  buf[3] = (अचिह्नित वर्ण)(sx->dest->पत्तनम् & 0xffU);        /* LSB */

  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  यदि(परिणाम || (nwritten != 4))
    प्रत्यागम CURLPX_SEND_REQUEST;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_req_सङ्कलय_user(संरचना socks_ctx *sx,
                                         संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  size_t nwritten;

  यदि(sx->creds) {
    size_t plen = वाक्यदैर्घ्यम्(sx->creds->user);
    यदि(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(दत्त, "Too long SOCKS proxy username");
      प्रत्यागम CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    यदि(परिणाम || (nwritten != (plen + 1)))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* empty username */
    अचिह्नित वर्ण b = 0;
    परिणाम = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    यदि(परिणाम || (nwritten != 1))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_resolving(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      bool *समाप्तम्)
{
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  CURLcode परिणाम;
  size_t nwritten;

  *समाप्तम् = FALSE;
  परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex, sx->dest);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_AGAIN) {
      failf(दत्त, "error %d resolving SOCKS destination %s:%u",
            (पूर्णाङ्क)परिणाम, sx->dest->hostname, sx->dest->पत्तनम्);
      प्रत्यागम CURLPX_RESOLVE_HOST;
    }
    प्रत्यागम CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(दत्त, sx->dest, cf->sockindex, AF_INET, 0);
  यदि(ai) {
    संरचना sockaddr_in *saddr_अन्तर्;
    वर्ण ipbuf[64];

    Curl_printable_स्थानाङ्कः(ai, ipbuf, परिमाण(ipbuf));
    CURL_TRC_CF(दत्त, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_अन्तर् = (संरचना sockaddr_in *)(शून्य *)ai->ai_addr;
    परिणाम = Curl_bufq_write(&sx->iobuf,
                             (अचिह्नित वर्ण *)&saddr_अन्तर्->sin_addr.s_addr, 4,
                             &nwritten);

    यदि(परिणाम || (nwritten != 4))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* No IPv4 address resolved */
    failf(दत्त, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    प्रत्यागम CURLPX_RESOLVE_HOST;
  }

  *समाप्तम् = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_check_resp(संरचना socks_ctx *sx,
                                       संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त)
{
  ध्रुव अचिह्नित वर्ण *resp;
  size_t rlen;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(दत्त, "SOCKS4 reply is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
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
  यदि(resp[0]) {
    failf(दत्त, "SOCKS4 reply has wrong version, version should be 0.");
    प्रत्यागम CURLPX_BAD_संस्करणम्;
  }

  /*परिणाम*/
  विकल्प(resp[1]) {
  पक्ष 90:
    CURL_TRC_CF(दत्त, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    प्रत्यागम CURLPX_OK;
  पक्ष 91:
    failf(दत्त,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिह्नित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_REQUEST_FAILED;
  पक्ष 92:
    failf(दत्त,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिह्नित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_IDENTD;
  पक्ष 93:
    failf(दत्त,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिह्नित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_IDENTD_भेदः;
  स्वभाव:
    failf(दत्त,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिह्नित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_UNKNOWN_FAIL;
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
स्थिर CURLproxycode socks4_connect(संरचना Curl_cfilter *cf,
                                    संरचना socks_ctx *sx,
                                    संरचना Curl_easy *दत्त)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode परिणाम;
  bool समाप्तम्;

process_state:
  विकल्प(sx->state) {
  पक्ष SOCKS_ST_INIT:
    sx->संस्करणम्_2 = 4;
    sxstate(sx, cf, दत्त, SOCKS4_ST_START);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_START:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_संस्करणम् = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(दत्त, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->पत्तनम्);

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
    presult = socks4_req_सङ्कलय_hd(sx, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    यदि(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      अचिह्नित वर्ण buf[4] = { 0, 0, 0, 1 };
      size_t hlen = वाक्यदैर्घ्यम्(sx->dest->hostname) + 1; /* including NUL */

      यदि(hlen > 255) {
        failf(दत्त, "SOCKS4: too long hostname");
        प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_LONG_HOSTNAME);
      }
      परिणाम = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      यदि(परिणाम || (nwritten != 4))
        प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_SEND_REQUEST);
      presult = socks4_req_सङ्कलय_user(sx, दत्त);
      यदि(presult)
        प्रत्यागम socks_failed(sx, cf, दत्त, presult);
      परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      यदि(परिणाम || (nwritten != hlen))
        प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, दत्त, SOCKS4_ST_SEND);
      गच्छ process_state;
    }
    sxstate(sx, cf, दत्त, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    /* append user */
    presult = socks4_req_सङ्कलय_user(sx, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    sxstate(sx, cf, दत्त, SOCKS4_ST_SEND);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, दत्त, SOCKS4_ST_RECV);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, दत्त, 8, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    presult = socks4_check_resp(sx, cf, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    sxstate(sx, cf, दत्त, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  पक्ष SOCKS_ST_SUCCESS:
    प्रत्यागम CURLPX_OK;

  पक्ष SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    प्रत्यागम sx->presult;

  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_SEND_REQUEST);
  }
}

स्थिर CURLproxycode socks5_req0_init(संरचना Curl_cfilter *cf,
                                      संरचना socks_ctx *sx,
                                      संरचना Curl_easy *दत्त)
{
  ध्रुव अचिह्नित वर्ण auth = दत्त->समुच्चयः.socks5auth;
  अचिह्नित वर्ण req[5]; /* version + len + 3 possible auth methods */
  अचिह्नित वर्ण nauths;
  size_t req_len, nwritten;
  CURLcode परिणाम;

  (शून्य)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  यदि(!sx->resolve_local && वाक्यदैर्घ्यम्(sx->dest->hostname) > 255) {
    failf(दत्त, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    प्रत्यागम CURLPX_LONG_HOSTNAME;
  }

  यदि(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(दत्त, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  यदि(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  यदि(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->द्वयङ्काः.socks5_authenticated = FALSE;

  req[0] = 5;   /*संस्करणम्*/
  nauths = 0;
  यदि(!दत्त->समुच्चयः.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  यदि(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#यदि_अन्तः
  यदि(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  यदि(!nauths) {
    failf(दत्त, "SOCKS5: no acceptable authentication method is available.");
    प्रत्यागम CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  परिणाम = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  यदि(परिणाम || (nwritten != req_len))
    प्रत्यागम CURLPX_SEND_REQUEST;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_check_resp0(संरचना socks_ctx *sx,
                                        संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त)
{
  ध्रुव अचिह्नित वर्ण *resp;
  अचिह्नित वर्ण auth_बहुलकम्;
  size_t rlen;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(दत्त, "SOCKS5 initial reply is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }

  यदि(resp[0] != 5) {
    failf(दत्त, "Received invalid version in initial SOCKS5 response.");
    प्रत्यागम CURLPX_BAD_संस्करणम्;
  }

  auth_बहुलकम् = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  विकल्प(auth_बहुलकम्) {
  पक्ष 0:
    यदि(दत्त->समुच्चयः.socks5_auth_only) {
      failf(दत्त, "SOCKS5 proxy selected no authentication.");
      प्रत्यागम CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, दत्त, SOCKS5_ST_REQ1_INIT);
    प्रत्यागम CURLPX_OK;
  पक्ष 1:
    यदि(दत्त->समुच्चयः.socks5auth & CURLAUTH_GSSAPI) {
      यदि(cf->sockindex == FIRSTSOCKET)
        cf->conn->द्वयङ्काः.socks5_authenticated = TRUE;
      sxstate(sx, cf, दत्त, SOCKS5_ST_GSSAPI_INIT);
      प्रत्यागम CURLPX_OK;
    }
    failf(दत्त, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    प्रत्यागम CURLPX_GSSAPI_PERMSG;
  पक्ष 2:
    /* regular name + password authentication */
    यदि(दत्त->समुच्चयः.socks5auth & CURLAUTH_BASIC) {
      यदि(cf->sockindex == FIRSTSOCKET)
        cf->conn->द्वयङ्काः.socks5_authenticated = TRUE;
      sxstate(sx, cf, दत्त, SOCKS5_ST_AUTH_INIT);
      प्रत्यागम CURLPX_OK;
    }
    failf(दत्त, "BASIC authentication proposed but not enabled.");
    प्रत्यागम CURLPX_NO_AUTH;
  पक्ष 255:
    failf(दत्त, "No authentication method was acceptable.");
    प्रत्यागम CURLPX_NO_AUTH;
  स्वभाव:
    failf(दत्त, "Unknown SOCKS5 mode attempted to be used by server.");
    प्रत्यागम CURLPX_UNKNOWN_बहुलकम्;
  }
}

स्थिर CURLproxycode socks5_auth_init(संरचना Curl_cfilter *cf,
                                      संरचना socks_ctx *sx,
                                      संरचना Curl_easy *दत्त)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  अचिह्नित वर्ण buf[2];
  CURLcode परिणाम;

  यदि(sx->creds) {
    ulen = वाक्यदैर्घ्यम्(sx->creds->user);
    plen = वाक्यदैर्घ्यम्(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    यदि(ulen > 255) {
      failf(दत्त, "Excessive username length for proxy auth");
      प्रत्यागम CURLPX_LONG_USER;
    }
    यदि(plen > 255) {
      failf(दत्त, "Excessive password length for proxy auth");
      प्रत्यागम CURLPX_LONG_PASSWD;
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
  buf[1] = (अचिह्नित वर्ण)ulen;
  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  यदि(परिणाम || (nwritten != 2))
    प्रत्यागम CURLPX_SEND_REQUEST;
  यदि(ulen) {
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    यदि(परिणाम || (nwritten != ulen))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  buf[0] = (अचिह्नित वर्ण)plen;
  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  यदि(परिणाम || (nwritten != 1))
    प्रत्यागम CURLPX_SEND_REQUEST;
  यदि(plen) {
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    यदि(परिणाम || (nwritten != plen))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, दत्त, SOCKS5_ST_AUTH_SEND);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_check_auth_resp(संरचना socks_ctx *sx,
                                            संरचना Curl_cfilter *cf,
                                            संरचना Curl_easy *दत्त)
{
  ध्रुव अचिह्नित वर्ण *resp;
  अचिह्नित वर्ण auth_स्थितिः;
  size_t rlen;

  (शून्य)cf;
  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(दत्त, "SOCKS5 sub-negotiation response incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_स्थितिः = resp[1];
  यदि(auth_स्थितिः) {
    failf(दत्त, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    प्रत्यागम CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_req1_init(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  अचिह्नित वर्ण req[5];
  अचिह्नित वर्ण ipbuf[16];
  ध्रुव अचिह्नित वर्ण *destination;
  अचिह्नित वर्ण desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode परिणाम;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  यदि(sx->resolve_local) {
    /* rest of request is added after resolving */
    परिणाम = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    यदि(परिणाम || (nwritten != 3))
      प्रत्यागम CURLPX_SEND_REQUEST;
    प्रत्यागम CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#यदि_परिभाषितम् USE_IPV6
  यदि(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    यदि(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      प्रत्यागम CURLPX_BAD_स्थानाङ्कः_TYPE;
  }
  अन्यथा
#यदि_अन्तः
  यदि(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  अन्यथा {
    ध्रुव size_t hostname_len = वाक्यदैर्घ्यम्(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ध्रुव अचिह्नित वर्ण *)sx->dest->hostname;
    destlen = (अचिह्नित वर्ण)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  परिणाम = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  यदि(परिणाम || (nwritten != hdlen))
    प्रत्यागम CURLPX_SEND_REQUEST;
  परिणाम = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  यदि(परिणाम || (nwritten != destlen))
    प्रत्यागम CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (अचिह्नित वर्ण)((sx->dest->पत्तनम् >> 8) & 0xff);
  req[1] = (अचिह्नित वर्ण)(sx->dest->पत्तनम् & 0xff);
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  यदि(परिणाम || (nwritten != 2))
    प्रत्यागम CURLPX_SEND_REQUEST;
  CURL_TRC_CF(दत्त, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->पत्तनम्);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_resolving(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      bool *समाप्तम्)
{
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  वर्ण dest[MAX_IPADR_LEN];  /* printable address */
  ध्रुव अचिह्नित वर्ण *destination = रिक्त;
  अचिह्नित वर्ण desttype = 1, destlen = 4;
  अचिह्नित वर्ण req[2];
  CURLcode परिणाम;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *समाप्तम् = FALSE;
  परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex, sx->dest);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_AGAIN) {
      failf(दत्त, "error %d resolving SOCKS destination %s:%u",
            (पूर्णाङ्क)परिणाम, sx->dest->hostname, sx->dest->पत्तनम्);
      प्रत्यागम CURLPX_RESOLVE_HOST;
    }
    प्रत्यागम CURLPX_OK;
  }

#यदि_परिभाषितम् USE_IPV6
  यदि(दत्त->समुच्चयः.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(दत्त, sx->dest, cf->sockindex, AF_INET6, 0);
#यदि_अन्तः
  यदि(!ai)
    ai = Curl_conn_dns_get_ai(दत्त, sx->dest, cf->sockindex, AF_INET, 0);

  यदि(!ai) {
    failf(दत्त, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    गच्छ बहिर्गमनम्;
  }

  Curl_printable_स्थानाङ्कः(ai, dest, परिमाण(dest));

  यदि(ai->ai_family == AF_INET) {
    संरचना sockaddr_in *saddr_अन्तर्;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_अन्तर् = (संरचना sockaddr_in *)(शून्य *)ai->ai_addr;
    destination = (ध्रुव अचिह्नित वर्ण *)&saddr_अन्तर्->sin_addr.s_addr;
    CURL_TRC_CF(दत्त, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->पत्तनम्);
  }
#यदि_परिभाषितम् USE_IPV6
  अन्यथा यदि(ai->ai_family == AF_INET6) {
    संरचना sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (संरचना sockaddr_in6 *)(शून्य *)ai->ai_addr;
    destination = (ध्रुव अचिह्नित वर्ण *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(दत्त, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->पत्तनम्);
  }
#यदि_अन्तः

  यदि(!destination) {
    failf(दत्त, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    गच्छ बहिर्गमनम्;
  }

  req[0] = desttype;
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  यदि(परिणाम || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  यदि(परिणाम || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ बहिर्गमनम्;
  }
  /* PORT MSB+LSB */
  req[0] = (अचिह्नित वर्ण)((sx->dest->पत्तनम् >> 8) & 0xffU);
  req[1] = (अचिह्नित वर्ण)(sx->dest->पत्तनम् & 0xffU);
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  यदि(परिणाम || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ बहिर्गमनम्;
  }

बहिर्गमनम्:
  *समाप्तम् = (presult == CURLPX_OK);
  प्रत्यागम presult;
}

स्थिर CURLproxycode socks5_recv_resp1(संरचना socks_ctx *sx,
                                       संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       bool *समाप्तम्)
{
  ध्रुव अचिह्नित वर्ण *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, दत्त, resp_len, समाप्तम्);
  यदि(presult)
    प्रत्यागम presult;
  अन्यथा यदि(!*समाप्तम्)
    प्रत्यागम CURLPX_OK;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(दत्त, "SOCKS5 response is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
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
  यदि(resp[0] != 5) { /*संस्करणम्*/
    failf(दत्त, "SOCKS5 reply has wrong version, version should be 5.");
    प्रत्यागम CURLPX_BAD_संस्करणम्;
  }
  अन्यथा यदि(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_प्रत्युत्तरम्_UNASSIGNED;
    पूर्णाङ्क संहिता = resp[1];
    failf(दत्त, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, संहिता);
    यदि(संहिता < 9) {
      /* RFC 1928 section 6 lists: */
      स्थिर ध्रुव CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_प्रत्युत्तरम्_GENERAL_SERVER_FAILURE,
        CURLPX_प्रत्युत्तरम्_NOT_ALLOWED,
        CURLPX_प्रत्युत्तरम्_जालम्_UNREACHABLE,
        CURLPX_प्रत्युत्तरम्_HOST_UNREACHABLE,
        CURLPX_प्रत्युत्तरम्_CONNECTION_REFUSED,
        CURLPX_प्रत्युत्तरम्_TTL_EXPIRED,
        CURLPX_प्रत्युत्तरम्_COMMAND_NOT_SUPPORTED,
        CURLPX_प्रत्युत्तरम्_स्थानाङ्कः_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[संहिता];
    }
    प्रत्यागम rc;
  }

  /* Calculate real packet size */
  विकल्प(resp[3]) {
  पक्ष 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    भङ्ग;
  पक्ष 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    भङ्ग;
  पक्ष 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    भङ्ग;
  स्वभाव:
    failf(दत्त, "SOCKS5 reply has wrong address type.");
    प्रत्यागम CURLPX_BAD_स्थानाङ्कः_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, दत्त, resp_len, समाप्तम्);
  यदि(presult)
    प्रत्यागम presult;
  अन्यथा यदि(!*समाप्तम्)
    प्रत्यागम CURLPX_OK;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(दत्त, "SOCKS5 response is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *समाप्तम् = TRUE;
  प्रत्यागम CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
स्थिर CURLproxycode socks5_connect(संरचना Curl_cfilter *cf,
                                    संरचना socks_ctx *sx,
                                    संरचना Curl_easy *दत्त)
{
  CURLproxycode presult;
  bool समाप्तम्;

process_state:
  विकल्प(sx->state) {
  पक्ष SOCKS_ST_INIT:
    sx->संस्करणम्_2 = 5;
    sxstate(sx, cf, दत्त, SOCKS5_ST_START);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_START:
    CURL_TRC_CF(दत्त, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->पत्तनम्);
    presult = socks5_req0_init(cf, sx, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    sxstate(sx, cf, दत्त, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, दत्त, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, दत्त, 2, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    /* socks5_check_resp0() sets next socks state */
    गच्छ process_state;

  पक्ष SOCKS5_ST_GSSAPI_INIT: {
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode परिणाम = Curl_SOCKS5_gssapi_negotiate(cf, दत्त, sx->creds);
    यदि(परिणाम) {
      failf(दत्त, "Unable to negotiate SOCKS5 GSS-API context.");
      प्रत्यागम CURLPX_GSSAPI;
    }
    sxstate(sx, cf, दत्त, SOCKS5_ST_REQ1_INIT);
    गच्छ process_state;
#अन्यथा
    failf(दत्त, "SOCKS5 GSSAPI per-message authentication is not supported.");
    प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_GSSAPI_PERMSG);
#यदि_अन्तः
  }

  पक्ष SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    sxstate(sx, cf, दत्त, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, दत्त, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, दत्त, 2, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, दत्त, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, दत्त);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    यदि(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, दत्त, SOCKS5_ST_REQ1_SEND);
      गच्छ process_state;
    }
    sxstate(sx, cf, दत्त, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, दत्त, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    अन्यथा यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    यदि(cf->conn->socks5_gssapi_enctype) {
      failf(दत्त, "SOCKS5 GSS-API protection not yet implemented.");
      प्रत्यागम CURLPX_GSSAPI_PROTECTION;
    }
#यदि_अन्तः
    sxstate(sx, cf, दत्त, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, दत्त, &समाप्तम्);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, दत्त, presult);
    यदि(!समाप्तम्)
      प्रत्यागम CURLPX_OK;
    CURL_TRC_CF(दत्त, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, दत्त, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  पक्ष SOCKS_ST_SUCCESS:
    प्रत्यागम CURLPX_OK;

  पक्ष SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    प्रत्यागम sx->presult;

  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम socks_failed(sx, cf, दत्त, CURLPX_SEND_REQUEST);
  }
}

स्थिर शून्य socks_proxy_ctx_free(संरचना socks_ctx *ctx)
{
  यदि(ctx) {
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
स्थिर CURLcode socks_proxy_cf_connect(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       bool *समाप्तम्)
{
  संरचना socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode परिणाम;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
  यदि(परिणाम || !*समाप्तम्)
    प्रत्यागम परिणाम;

  विकल्प(ctx->proxy_type) {
  पक्ष CURLPROXY_SOCKS5:
  पक्ष CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, दत्त);
    भङ्ग;

  पक्ष CURLPROXY_SOCKS4:
  पक्ष CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, दत्त);
    भङ्ग;

  स्वभाव:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    परिणाम = CURLE_COULDNT_CONNECT;
    गच्छ बहिर्गमनम्;
  }

  यदि(pxresult) {
    परिणाम = CURLE_PROXY;
    दत्त->info.pxcode = (uint8_t)pxresult;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(ctx->state != SOCKS_ST_SUCCESS)
    गच्छ बहिर्गमनम्;

#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_is_verbose(दत्त)) {
    संरचना ip_quadruple ipquad;
    bool is_ipv6;
    यदि(!Curl_conn_cf_get_ip_info(cf->अग्र, दत्त, &is_ipv6, &ipquad))
      infof(दत्त, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_पत्तनम्,
            ctx->dest->hostname, ctx->dest->पत्तनम्,
            ipquad.remote_ip, ipquad.remote_पत्तनम्);
    अन्यथा
      infof(दत्त, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#यदि_अन्तः
  cf->connected = TRUE;

बहिर्गमनम्:
  *समाप्तम् = (bool)cf->connected;
  यदि(*समाप्तम् || परिणाम)
    Curl_creds_unlink(&ctx->creds);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode socks_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना easy_pollset *ps)
{
  संरचना socks_ctx *sx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, दत्त);
    विकल्प(sx->state) {
    पक्ष SOCKS4_ST_SEND:
    पक्ष SOCKS5_ST_REQ0_SEND:
    पक्ष SOCKS5_ST_AUTH_SEND:
    पक्ष SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(दत्त, cf, "adjust pollset out (%d)", (पूर्णाङ्क)sx->state);
      परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, sock);
      भङ्ग;
    स्वभाव:
      CURL_TRC_CF(दत्त, cf, "adjust pollset in (%d)", (पूर्णाङ्क)sx->state);
      परिणाम = Curl_pollset_समुच्चयः_अन्तर्_only(दत्त, ps, sock);
      भङ्ग;
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य socks_proxy_cf_destroy(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  (शून्य)दत्त;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = रिक्त;
}

स्थिर CURLcode socks_cf_query(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना socks_ctx *sx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_HOST_पत्तनम्:
    यदि(sx) {
      *pres1 = sx->dest->पत्तनम्;
      *((ध्रुव वर्ण **)pres2) = sx->dest->hostname;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

संरचना Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                          संरचना Curl_easy *दत्त,
                                          संरचना Curl_peer *dest,
                                          uint8_t ip_संस्करणम्,
                                          uint8_t proxy_type,
                                          संरचना Curl_creds *creds)
{
  संरचना Curl_cfilter *cf;
  संरचना socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(दत्त, ip_संस्करणम्);
  CURLcode परिणाम;

  यदि(!dest)
    प्रत्यागम CURLE_FAILED_INIT;

  विकल्प(proxy_type) {
  पक्ष CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    भङ्ग;
  पक्ष CURLPROXY_SOCKS5_HOSTNAME:
    भङ्ग;
  पक्ष CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    भङ्ग;
  पक्ष CURLPROXY_SOCKS4A:
    भङ्ग;
  स्वभाव:
    failf(दत्त, "unknown proxytype %d option given", proxy_type);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_संस्करणम् = ip_संस्करणम्;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_आकार, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  यदि(!परिणाम) {
    Curl_conn_cf_निवेशय_after(cf_at, cf);
    यदि(ctx->resolve_local) {
      परिणाम = Curl_conn_dns_सङ्कलय_addr_resolve(दत्त, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  अन्यथा
    socks_proxy_ctx_free(ctx);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* CURL_DISABLE_PROXY */
