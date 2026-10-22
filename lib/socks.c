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

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

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
#endif

#define SOCKS_CHUNK_SIZE    1024
#define SOCKS_CHUNKS        1

संरचना socks_ctx {
  गणना socks_state_t state;
  संरचना bufq iobuf;
  संरचना Curl_peer *dest;
  संरचना Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  अचिन्हित वर्ण version;
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
                            संरचना Curl_easy *data,
                            वर्ण *buf,             /* store read data here */
                            माप_प्रकार blen,           /* space in buf */
                            माप_प्रकार *pnread)        /* amount bytes read */
{
  माप_प्रकार nread = 0;
  CURLcode result;

  *pnread = 0;
  प्रति(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    यदि(timeout_ms < 0) {
      /* we already got the timeout */
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    }
    यदि(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    यदि(SOCKET_READABLE(sock, timeout_ms) <= 0)
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    यदि(result == CURLE_AGAIN)
      अनुवर्त;
    अन्यथा यदि(result)
      प्रत्यागम result;

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
#endif

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#अन्यथा
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
स्थिर शून्य socksstate(संरचना socks_ctx *sx,
                       संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *data,
                       गणना socks_state_t state
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , पूर्णाङ्क lineno
#endif
)
{
  गणना socks_state_t oldstate = sx->state;

  यदि(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    प्रत्यागम;

  sx->state = state;

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#अन्यथा
  (शून्य)cf;
  (शून्य)data;
#endif
}

स्थिर CURLproxycode socks_failed(संरचना socks_ctx *sx,
                                  संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  प्रत्यागम presult;
}

स्थिर CURLproxycode socks_flush(संरचना socks_ctx *sx,
                                 संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  माप_प्रकार nwritten;

  *done = FALSE;
  यावत्(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, रिक्त, 0,
                               &nwritten);
    यदि(result == CURLE_AGAIN)
      प्रत्यागम CURLPX_OK;
    अन्यथा यदि(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      प्रत्यागम socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks_recv(संरचना socks_ctx *sx,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                माप_प्रकार min_bytes,
                                bool *done)
{
  CURLcode result;
  माप_प्रकार nread;

  *done = FALSE;
  यावत्(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    यदि(result == CURLE_AGAIN)
      प्रत्यागम CURLPX_OK;
    अन्यथा यदि(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      प्रत्यागम CURLPX_RECV_CONNECT;
    }
    अन्यथा यदि(!nread) { /* EOF */
      यदि(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        प्रत्यागम CURLPX_RECV_CONNECT;
      }
      भङ्ग;
    }
  }
  *done = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_req_add_hd(संरचना socks_ctx *sx,
                                       संरचना Curl_easy *data)
{
  अचिन्हित वर्ण buf[4];
  माप_प्रकार nwritten;
  CURLcode result;

  (शून्य)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (अचिन्हित वर्ण)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (अचिन्हित वर्ण)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  यदि(result || (nwritten != 4))
    प्रत्यागम CURLPX_SEND_REQUEST;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_req_add_user(संरचना socks_ctx *sx,
                                         संरचना Curl_easy *data)
{
  CURLcode result;
  माप_प्रकार nwritten;

  यदि(sx->creds) {
    माप_प्रकार plen = म_माप(sx->creds->user);
    यदि(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      प्रत्यागम CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    यदि(result || (nwritten != (plen + 1)))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* empty username */
    अचिन्हित वर्ण b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    यदि(result || (nwritten != 1))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_resolving(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  CURLcode result;
  माप_प्रकार nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  यदि(result) {
    यदि(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (पूर्णाङ्क)result, sx->dest->hostname, sx->dest->port);
      प्रत्यागम CURLPX_RESOLVE_HOST;
    }
    प्रत्यागम CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  यदि(ai) {
    संरचना sockaddr_in *saddr_in;
    वर्ण ipbuf[64];

    Curl_printable_address(ai, ipbuf, परिमाण(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (संरचना sockaddr_in *)(शून्य *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (अचिन्हित वर्ण *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    यदि(result || (nwritten != 4))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    प्रत्यागम CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks4_check_resp(संरचना socks_ctx *sx,
                                       संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  ध्रुव अचिन्हित वर्ण *resp;
  माप_प्रकार rlen;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
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
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    प्रत्यागम CURLPX_BAD_VERSION;
  }

  /* Result */
  विकल्प(resp[1]) {
  पक्ष 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    प्रत्यागम CURLPX_OK;
  पक्ष 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_REQUEST_FAILED;
  पक्ष 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_IDENTD;
  पक्ष 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
    प्रत्यागम CURLPX_IDENTD_DIFFER;
  स्वभाव:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णाङ्क)((resp[2] << 8) | resp[3]), resp[1]);
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
                                    संरचना Curl_easy *data)
{
  माप_प्रकार nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  विकल्प(sx->state) {
  पक्ष SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_START:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_version = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(data, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->port);

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
    presult = socks4_req_add_hd(sx, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    यदि(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      अचिन्हित वर्ण buf[4] = { 0, 0, 0, 1 };
      माप_प्रकार hlen = म_माप(sx->dest->hostname) + 1; /* including NUL */

      यदि(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        प्रत्यागम socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      यदि(result || (nwritten != 4))
        प्रत्यागम socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      यदि(presult)
        प्रत्यागम socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      यदि(result || (nwritten != hlen))
        प्रत्यागम socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      गच्छ process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    यदि(!done)
      प्रत्यागम CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  पक्ष SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  पक्ष SOCKS_ST_SUCCESS:
    प्रत्यागम CURLPX_OK;

  पक्ष SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    प्रत्यागम sx->presult;

  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

स्थिर CURLproxycode socks5_req0_init(संरचना Curl_cfilter *cf,
                                      संरचना socks_ctx *sx,
                                      संरचना Curl_easy *data)
{
  ध्रुव अचिन्हित वर्ण auth = data->set.socks5auth;
  अचिन्हित वर्ण req[5]; /* version + len + 3 possible auth methods */
  अचिन्हित वर्ण nauths;
  माप_प्रकार req_len, nwritten;
  CURLcode result;

  (शून्य)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  यदि(!sx->resolve_local && म_माप(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    प्रत्यागम CURLPX_LONG_HOSTNAME;
  }

  यदि(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  यदि(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  यदि(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  यदि(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  यदि(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  यदि(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  यदि(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    प्रत्यागम CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  यदि(result || (nwritten != req_len))
    प्रत्यागम CURLPX_SEND_REQUEST;
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_check_resp0(संरचना socks_ctx *sx,
                                        संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data)
{
  ध्रुव अचिन्हित वर्ण *resp;
  अचिन्हित वर्ण auth_mode;
  माप_प्रकार rlen;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }

  यदि(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    प्रत्यागम CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  विकल्प(auth_mode) {
  पक्ष 0:
    यदि(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      प्रत्यागम CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    प्रत्यागम CURLPX_OK;
  पक्ष 1:
    यदि(data->set.socks5auth & CURLAUTH_GSSAPI) {
      यदि(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      प्रत्यागम CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    प्रत्यागम CURLPX_GSSAPI_PERMSG;
  पक्ष 2:
    /* regular name + password authentication */
    यदि(data->set.socks5auth & CURLAUTH_BASIC) {
      यदि(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      प्रत्यागम CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    प्रत्यागम CURLPX_NO_AUTH;
  पक्ष 255:
    failf(data, "No authentication method was acceptable.");
    प्रत्यागम CURLPX_NO_AUTH;
  स्वभाव:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    प्रत्यागम CURLPX_UNKNOWN_MODE;
  }
}

स्थिर CURLproxycode socks5_auth_init(संरचना Curl_cfilter *cf,
                                      संरचना socks_ctx *sx,
                                      संरचना Curl_easy *data)
{
  /* Needs username and password */
  माप_प्रकार ulen = 0, plen = 0, nwritten;
  अचिन्हित वर्ण buf[2];
  CURLcode result;

  यदि(sx->creds) {
    ulen = म_माप(sx->creds->user);
    plen = म_माप(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    यदि(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      प्रत्यागम CURLPX_LONG_USER;
    }
    यदि(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
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
  buf[1] = (अचिन्हित वर्ण)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  यदि(result || (nwritten != 2))
    प्रत्यागम CURLPX_SEND_REQUEST;
  यदि(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    यदि(result || (nwritten != ulen))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  buf[0] = (अचिन्हित वर्ण)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  यदि(result || (nwritten != 1))
    प्रत्यागम CURLPX_SEND_REQUEST;
  यदि(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    यदि(result || (nwritten != plen))
      प्रत्यागम CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_check_auth_resp(संरचना socks_ctx *sx,
                                            संरचना Curl_cfilter *cf,
                                            संरचना Curl_easy *data)
{
  ध्रुव अचिन्हित वर्ण *resp;
  अचिन्हित वर्ण auth_status;
  माप_प्रकार rlen;

  (शून्य)cf;
  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  यदि(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    प्रत्यागम CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_req1_init(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  अचिन्हित वर्ण req[5];
  अचिन्हित वर्ण ipbuf[16];
  ध्रुव अचिन्हित वर्ण *destination;
  अचिन्हित वर्ण desttype, destlen, hdlen;
  माप_प्रकार nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  यदि(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    यदि(result || (nwritten != 3))
      प्रत्यागम CURLPX_SEND_REQUEST;
    प्रत्यागम CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  यदि(म_अक्षर(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    यदि(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      प्रत्यागम CURLPX_BAD_ADDRESS_TYPE;
  }
  अन्यथा
#endif
  यदि(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  अन्यथा {
    ध्रुव माप_प्रकार hostname_len = म_माप(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ध्रुव अचिन्हित वर्ण *)sx->dest->hostname;
    destlen = (अचिन्हित वर्ण)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  यदि(result || (nwritten != hdlen))
    प्रत्यागम CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  यदि(result || (nwritten != destlen))
    प्रत्यागम CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (अचिन्हित वर्ण)((sx->dest->port >> 8) & 0xff);
  req[1] = (अचिन्हित वर्ण)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  यदि(result || (nwritten != 2))
    प्रत्यागम CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  प्रत्यागम CURLPX_OK;
}

स्थिर CURLproxycode socks5_resolving(संरचना socks_ctx *sx,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  वर्ण dest[MAX_IPADR_LEN];  /* printable address */
  ध्रुव अचिन्हित वर्ण *destination = रिक्त;
  अचिन्हित वर्ण desttype = 1, destlen = 4;
  अचिन्हित वर्ण req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  माप_प्रकार nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  यदि(result) {
    यदि(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (पूर्णाङ्क)result, sx->dest->hostname, sx->dest->port);
      प्रत्यागम CURLPX_RESOLVE_HOST;
    }
    प्रत्यागम CURLPX_OK;
  }

#ifdef USE_IPV6
  यदि(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  यदि(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  यदि(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    गच्छ out;
  }

  Curl_printable_address(ai, dest, परिमाण(dest));

  यदि(ai->ai_family == AF_INET) {
    संरचना sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (संरचना sockaddr_in *)(शून्य *)ai->ai_addr;
    destination = (ध्रुव अचिन्हित वर्ण *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  अन्यथा यदि(ai->ai_family == AF_INET6) {
    संरचना sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (संरचना sockaddr_in6 *)(शून्य *)ai->ai_addr;
    destination = (ध्रुव अचिन्हित वर्ण *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  यदि(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    गच्छ out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  यदि(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  यदि(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ out;
  }
  /* PORT MSB+LSB */
  req[0] = (अचिन्हित वर्ण)((sx->dest->port >> 8) & 0xffU);
  req[1] = (अचिन्हित वर्ण)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  यदि(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    गच्छ out;
  }

out:
  *done = (presult == CURLPX_OK);
  प्रत्यागम presult;
}

स्थिर CURLproxycode socks5_recv_resp1(संरचना socks_ctx *sx,
                                       संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       bool *done)
{
  ध्रुव अचिन्हित वर्ण *resp;
  माप_प्रकार rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  यदि(presult)
    प्रत्यागम presult;
  अन्यथा यदि(!*done)
    प्रत्यागम CURLPX_OK;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
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
  यदि(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    प्रत्यागम CURLPX_BAD_VERSION;
  }
  अन्यथा यदि(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    पूर्णाङ्क code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    यदि(code < 9) {
      /* RFC 1928 section 6 lists: */
      स्थिर ध्रुव CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_REPLY_GENERAL_SERVER_FAILURE,
        CURLPX_REPLY_NOT_ALLOWED,
        CURLPX_REPLY_NETWORK_UNREACHABLE,
        CURLPX_REPLY_HOST_UNREACHABLE,
        CURLPX_REPLY_CONNECTION_REFUSED,
        CURLPX_REPLY_TTL_EXPIRED,
        CURLPX_REPLY_COMMAND_NOT_SUPPORTED,
        CURLPX_REPLY_ADDRESS_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[code];
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
    failf(data, "SOCKS5 reply has wrong address type.");
    प्रत्यागम CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  यदि(presult)
    प्रत्यागम presult;
  अन्यथा यदि(!*done)
    प्रत्यागम CURLPX_OK;

  यदि(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    प्रत्यागम CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  प्रत्यागम CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
स्थिर CURLproxycode socks5_connect(संरचना Curl_cfilter *cf,
                                    संरचना socks_ctx *sx,
                                    संरचना Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  विकल्प(sx->state) {
  पक्ष SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    गच्छ process_state;

  पक्ष SOCKS5_ST_GSSAPI_INIT: {
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    यदि(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      प्रत्यागम CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    गच्छ process_state;
#अन्यथा
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    प्रत्यागम socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  पक्ष SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    यदि(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      गच्छ process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    यदि(!done)
      प्रत्यागम CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    अन्यथा यदि(!done)
      प्रत्यागम CURLPX_OK;
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    यदि(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      प्रत्यागम CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  पक्ष SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    यदि(presult)
      प्रत्यागम socks_failed(sx, cf, data, presult);
    यदि(!done)
      प्रत्यागम CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  पक्ष SOCKS_ST_SUCCESS:
    प्रत्यागम CURLPX_OK;

  पक्ष SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    प्रत्यागम sx->presult;

  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
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
                                       संरचना Curl_easy *data,
                                       bool *done)
{
  संरचना socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  यदि(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  यदि(result || !*done)
    प्रत्यागम result;

  विकल्प(ctx->proxy_type) {
  पक्ष CURLPROXY_SOCKS5:
  पक्ष CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    भङ्ग;

  पक्ष CURLPROXY_SOCKS4:
  पक्ष CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    भङ्ग;

  स्वभाव:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    गच्छ out;
  }

  यदि(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    गच्छ out;
  }
  अन्यथा यदि(ctx->state != SOCKS_ST_SUCCESS)
    गच्छ out;

#ifdef CURLVERBOSE
  यदि(Curl_trc_is_verbose(data)) {
    संरचना ip_quadruple ipquad;
    bool is_ipv6;
    यदि(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    अन्यथा
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  यदि(*done || result)
    Curl_creds_unlink(&ctx->creds);
  प्रत्यागम result;
}

स्थिर CURLcode socks_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना easy_pollset *ps)
{
  संरचना socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  यदि(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    विकल्प(sx->state) {
    पक्ष SOCKS4_ST_SEND:
    पक्ष SOCKS5_ST_REQ0_SEND:
    पक्ष SOCKS5_ST_AUTH_SEND:
    पक्ष SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (पूर्णाङ्क)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      भङ्ग;
    स्वभाव:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (पूर्णाङ्क)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      भङ्ग;
    }
  }
  प्रत्यागम result;
}

स्थिर शून्य socks_proxy_cf_destroy(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  (शून्य)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = रिक्त;
}

स्थिर CURLcode socks_cf_query(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना socks_ctx *sx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_HOST_PORT:
    यदि(sx) {
      *pres1 = sx->dest->port;
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
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
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
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_जोड़ो_after(संरचना Curl_cfilter *cf_at,
                                          संरचना Curl_easy *data,
                                          संरचना Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          संरचना Curl_creds *creds)
{
  संरचना Curl_cfilter *cf;
  संरचना socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

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
    failf(data, "unknown proxytype %d option given", proxy_type);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  यदि(!result) {
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
    यदि(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  अन्यथा
    socks_proxy_ctx_free(ctx);
  प्रत्यागम result;
}

#endif /* CURL_DISABLE_PROXY */
