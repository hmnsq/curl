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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_PROXY

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "bufq.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "select.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "socks.h"
#समावेश "vdns/cf-dns.h"
#समावेश "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
क्रमागत socks_state_t {
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

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
अटल स्थिर अक्षर * स्थिर cf_socks_statename[] = {
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
#पूर्ण_अगर

#घोषणा SOCKS_CHUNK_आकार    1024
#घोषणा SOCKS_CHUNKS        1

काष्ठा socks_ctx {
  क्रमागत socks_state_t state;
  काष्ठा bufq iobuf;
  काष्ठा Curl_peer *dest;
  काष्ठा Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  अचिन्हित अक्षर version;
  BIT(resolve_local);
  BIT(socks4a);
};

#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            अक्षर *buf,             /* store read data here */
                            माप_प्रकार blen,           /* space in buf */
                            माप_प्रकार *pnread)        /* amount bytes read */
{
  माप_प्रकार nread = 0;
  CURLcode परिणाम;

  *pnread = 0;
  क्रम(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(तथ्य);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, तथ्य);

    अगर(timeout_ms < 0) {
      /* we already got the timeout */
      वापस CURLE_OPERATION_TIMEDOUT;
    }
    अगर(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    अगर(SOCKET_READABLE(sock, timeout_ms) <= 0)
      वापस CURLE_OPERATION_TIMEDOUT;
    परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य, buf, blen, &nread);
    अगर(परिणाम == CURLE_AGAIN)
      जारी;
    अन्यथा अगर(परिणाम)
      वापस परिणाम;

    अगर(blen == nread) {
      *pnread += nread;
      वापस CURLE_OK;
    }
    अगर(!nread) /* EOF */
      वापस CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#पूर्ण_अगर

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
#घोषणा sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#अन्यथा
#घोषणा sxstate(x, c, d, y) socksstate(x, c, d, y)
#पूर्ण_अगर

/* always use this function to change state, to make debugging easier */
अटल व्योम socksstate(काष्ठा socks_ctx *sx,
                       काष्ठा Curl_cfilter *cf,
                       काष्ठा Curl_easy *तथ्य,
                       क्रमागत socks_state_t state
#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , पूर्णांक lineno
#पूर्ण_अगर
)
{
  क्रमागत socks_state_t oldstate = sx->state;

  अगर(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    वापस;

  sx->state = state;

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(तथ्य, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#अन्यथा
  (व्योम)cf;
  (व्योम)तथ्य;
#पूर्ण_अगर
}

अटल CURLproxycode socks_failed(काष्ठा socks_ctx *sx,
                                  काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, तथ्य, SOCKS_ST_FAILED);
  sx->presult = presult;
  वापस presult;
}

अटल CURLproxycode socks_flush(काष्ठा socks_ctx *sx,
                                 काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool *done)
{
  CURLcode परिणाम;
  माप_प्रकार nwritten;

  *done = FALSE;
  जबतक(!Curl_bufq_is_empty(&sx->iobuf)) {
    परिणाम = Curl_cf_send_bufq(cf->अग्र, तथ्य, &sx->iobuf, शून्य, 0,
                               &nwritten);
    अगर(परिणाम == CURLE_AGAIN)
      वापस CURLPX_OK;
    अन्यथा अगर(परिणाम) {
      failf(तथ्य, "Failed to send SOCKS request: %s",
            curl_easy_strerror(परिणाम));
      वापस socks_failed(sx, cf, तथ्य, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  वापस CURLPX_OK;
}

अटल CURLproxycode socks_recv(काष्ठा socks_ctx *sx,
                                काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                माप_प्रकार min_bytes,
                                bool *done)
{
  CURLcode परिणाम;
  माप_प्रकार nread;

  *done = FALSE;
  जबतक(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    अगर(परिणाम == CURLE_AGAIN)
      वापस CURLPX_OK;
    अन्यथा अगर(परिणाम) {
      failf(तथ्य, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(परिणाम));
      वापस CURLPX_RECV_CONNECT;
    }
    अन्यथा अगर(!nread) { /* EOF */
      अगर(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(तथ्य, "Failed to receive SOCKS response, "
              "proxy closed connection");
        वापस CURLPX_RECV_CONNECT;
      }
      अवरोध;
    }
  }
  *done = TRUE;
  वापस CURLPX_OK;
}

अटल CURLproxycode socks4_req_add_hd(काष्ठा socks_ctx *sx,
                                       काष्ठा Curl_easy *तथ्य)
{
  अचिन्हित अक्षर buf[4];
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (अचिन्हित अक्षर)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (अचिन्हित अक्षर)(sx->dest->port & 0xffU);        /* LSB */

  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  अगर(परिणाम || (nwritten != 4))
    वापस CURLPX_SEND_REQUEST;
  वापस CURLPX_OK;
}

अटल CURLproxycode socks4_req_add_user(काष्ठा socks_ctx *sx,
                                         काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  माप_प्रकार nwritten;

  अगर(sx->creds) {
    माप_प्रकार plen = म_माप(sx->creds->user);
    अगर(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(तथ्य, "Too long SOCKS proxy username");
      वापस CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    अगर(परिणाम || (nwritten != (plen + 1)))
      वापस CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* empty username */
    अचिन्हित अक्षर b = 0;
    परिणाम = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    अगर(परिणाम || (nwritten != 1))
      वापस CURLPX_SEND_REQUEST;
  }
  वापस CURLPX_OK;
}

अटल CURLproxycode socks4_resolving(काष्ठा socks_ctx *sx,
                                      काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *done)
{
  स्थिर काष्ठा Curl_addrinfo *ai = शून्य;
  CURLcode परिणाम;
  माप_प्रकार nwritten;

  *done = FALSE;
  परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex, sx->dest);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_AGAIN) {
      failf(तथ्य, "error %d resolving SOCKS destination %s:%u",
            (पूर्णांक)परिणाम, sx->dest->hostname, sx->dest->port);
      वापस CURLPX_RESOLVE_HOST;
    }
    वापस CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(तथ्य, sx->dest, cf->sockindex, AF_INET, 0);
  अगर(ai) {
    काष्ठा sockaddr_in *saddr_in;
    अक्षर ipbuf[64];

    Curl_printable_address(ai, ipbuf, माप(ipbuf));
    CURL_TRC_CF(तथ्य, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (काष्ठा sockaddr_in *)(व्योम *)ai->ai_addr;
    परिणाम = Curl_bufq_write(&sx->iobuf,
                             (अचिन्हित अक्षर *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    अगर(परिणाम || (nwritten != 4))
      वापस CURLPX_SEND_REQUEST;
  }
  अन्यथा {
    /* No IPv4 address resolved */
    failf(तथ्य, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    वापस CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  वापस CURLPX_OK;
}

अटल CURLproxycode socks4_check_resp(काष्ठा socks_ctx *sx,
                                       काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य)
{
  स्थिर अचिन्हित अक्षर *resp;
  माप_प्रकार rlen;

  अगर(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(तथ्य, "SOCKS4 reply is incomplete.");
    वापस CURLPX_RECV_CONNECT;
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
  अगर(resp[0]) {
    failf(तथ्य, "SOCKS4 reply has wrong version, version should be 0.");
    वापस CURLPX_BAD_VERSION;
  }

  /*परिणाम*/
  चयन(resp[1]) {
  हाल 90:
    CURL_TRC_CF(तथ्य, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    वापस CURLPX_OK;
  हाल 91:
    failf(तथ्य,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णांक)((resp[2] << 8) | resp[3]), resp[1]);
    वापस CURLPX_REQUEST_FAILED;
  हाल 92:
    failf(तथ्य,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णांक)((resp[2] << 8) | resp[3]), resp[1]);
    वापस CURLPX_IDENTD;
  हाल 93:
    failf(तथ्य,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णांक)((resp[2] << 8) | resp[3]), resp[1]);
    वापस CURLPX_IDENTD_DIFFER;
  शेष:
    failf(तथ्य,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (अचिन्हित पूर्णांक)((resp[2] << 8) | resp[3]), resp[1]);
    वापस CURLPX_UNKNOWN_FAIL;
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
अटल CURLproxycode socks4_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा socks_ctx *sx,
                                    काष्ठा Curl_easy *तथ्य)
{
  माप_प्रकार nwritten;
  CURLproxycode presult;
  CURLcode परिणाम;
  bool done;

process_state:
  चयन(sx->state) {
  हाल SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, तथ्य, SOCKS4_ST_START);
    FALLTHROUGH();

  हाल SOCKS4_ST_START:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_version = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(तथ्य, cf, "SOCKS4%s connecting to %s:%u",
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
    presult = socks4_req_add_hd(sx, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    अगर(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      अचिन्हित अक्षर buf[4] = { 0, 0, 0, 1 };
      माप_प्रकार hlen = म_माप(sx->dest->hostname) + 1; /* including NUL */

      अगर(hlen > 255) {
        failf(तथ्य, "SOCKS4: too long hostname");
        वापस socks_failed(sx, cf, तथ्य, CURLPX_LONG_HOSTNAME);
      }
      परिणाम = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      अगर(परिणाम || (nwritten != 4))
        वापस socks_failed(sx, cf, तथ्य, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, तथ्य);
      अगर(presult)
        वापस socks_failed(sx, cf, तथ्य, presult);
      परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      अगर(परिणाम || (nwritten != hlen))
        वापस socks_failed(sx, cf, तथ्य, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, तथ्य, SOCKS4_ST_SEND);
      जाओ process_state;
    }
    sxstate(sx, cf, तथ्य, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  हाल SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अगर(!done)
      वापस CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    sxstate(sx, cf, तथ्य, SOCKS4_ST_SEND);
    FALLTHROUGH();

  हाल SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    sxstate(sx, cf, तथ्य, SOCKS4_ST_RECV);
    FALLTHROUGH();

  हाल SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, तथ्य, 8, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    presult = socks4_check_resp(sx, cf, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    sxstate(sx, cf, तथ्य, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  हाल SOCKS_ST_SUCCESS:
    वापस CURLPX_OK;

  हाल SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    वापस sx->presult;

  शेष:
    DEBUGASSERT(0);
    वापस socks_failed(sx, cf, तथ्य, CURLPX_SEND_REQUEST);
  }
}

अटल CURLproxycode socks5_req0_init(काष्ठा Curl_cfilter *cf,
                                      काष्ठा socks_ctx *sx,
                                      काष्ठा Curl_easy *तथ्य)
{
  स्थिर अचिन्हित अक्षर auth = तथ्य->set.socks5auth;
  अचिन्हित अक्षर req[5]; /* version + len + 3 possible auth methods */
  अचिन्हित अक्षर nauths;
  माप_प्रकार req_len, nwritten;
  CURLcode परिणाम;

  (व्योम)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  अगर(!sx->resolve_local && म_माप(sx->dest->hostname) > 255) {
    failf(तथ्य, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    वापस CURLPX_LONG_HOSTNAME;
  }

  अगर(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(तथ्य, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  अगर(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  अगर(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  अगर(!तथ्य->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  अगर(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#पूर्ण_अगर
  अगर(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  अगर(!nauths) {
    failf(तथ्य, "SOCKS5: no acceptable authentication method is available.");
    वापस CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  परिणाम = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  अगर(परिणाम || (nwritten != req_len))
    वापस CURLPX_SEND_REQUEST;
  वापस CURLPX_OK;
}

अटल CURLproxycode socks5_check_resp0(काष्ठा socks_ctx *sx,
                                        काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य)
{
  स्थिर अचिन्हित अक्षर *resp;
  अचिन्हित अक्षर auth_mode;
  माप_प्रकार rlen;

  अगर(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(तथ्य, "SOCKS5 initial reply is incomplete.");
    वापस CURLPX_RECV_CONNECT;
  }

  अगर(resp[0] != 5) {
    failf(तथ्य, "Received invalid version in initial SOCKS5 response.");
    वापस CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  चयन(auth_mode) {
  हाल 0:
    अगर(तथ्य->set.socks5_auth_only) {
      failf(तथ्य, "SOCKS5 proxy selected no authentication.");
      वापस CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ1_INIT);
    वापस CURLPX_OK;
  हाल 1:
    अगर(तथ्य->set.socks5auth & CURLAUTH_GSSAPI) {
      अगर(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, तथ्य, SOCKS5_ST_GSSAPI_INIT);
      वापस CURLPX_OK;
    }
    failf(तथ्य, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    वापस CURLPX_GSSAPI_PERMSG;
  हाल 2:
    /* regular name + password authentication */
    अगर(तथ्य->set.socks5auth & CURLAUTH_BASIC) {
      अगर(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, तथ्य, SOCKS5_ST_AUTH_INIT);
      वापस CURLPX_OK;
    }
    failf(तथ्य, "BASIC authentication proposed but not enabled.");
    वापस CURLPX_NO_AUTH;
  हाल 255:
    failf(तथ्य, "No authentication method was acceptable.");
    वापस CURLPX_NO_AUTH;
  शेष:
    failf(तथ्य, "Unknown SOCKS5 mode attempted to be used by server.");
    वापस CURLPX_UNKNOWN_MODE;
  }
}

अटल CURLproxycode socks5_auth_init(काष्ठा Curl_cfilter *cf,
                                      काष्ठा socks_ctx *sx,
                                      काष्ठा Curl_easy *तथ्य)
{
  /* Needs username and password */
  माप_प्रकार ulen = 0, plen = 0, nwritten;
  अचिन्हित अक्षर buf[2];
  CURLcode परिणाम;

  अगर(sx->creds) {
    ulen = म_माप(sx->creds->user);
    plen = म_माप(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    अगर(ulen > 255) {
      failf(तथ्य, "Excessive username length for proxy auth");
      वापस CURLPX_LONG_USER;
    }
    अगर(plen > 255) {
      failf(तथ्य, "Excessive password length for proxy auth");
      वापस CURLPX_LONG_PASSWD;
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
  buf[1] = (अचिन्हित अक्षर)ulen;
  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  अगर(परिणाम || (nwritten != 2))
    वापस CURLPX_SEND_REQUEST;
  अगर(ulen) {
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    अगर(परिणाम || (nwritten != ulen))
      वापस CURLPX_SEND_REQUEST;
  }
  buf[0] = (अचिन्हित अक्षर)plen;
  परिणाम = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  अगर(परिणाम || (nwritten != 1))
    वापस CURLPX_SEND_REQUEST;
  अगर(plen) {
    परिणाम = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    अगर(परिणाम || (nwritten != plen))
      वापस CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, तथ्य, SOCKS5_ST_AUTH_SEND);
  वापस CURLPX_OK;
}

अटल CURLproxycode socks5_check_auth_resp(काष्ठा socks_ctx *sx,
                                            काष्ठा Curl_cfilter *cf,
                                            काष्ठा Curl_easy *तथ्य)
{
  स्थिर अचिन्हित अक्षर *resp;
  अचिन्हित अक्षर auth_status;
  माप_प्रकार rlen;

  (व्योम)cf;
  अगर(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(तथ्य, "SOCKS5 sub-negotiation response incomplete.");
    वापस CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  अगर(auth_status) {
    failf(तथ्य, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    वापस CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  वापस CURLPX_OK;
}

अटल CURLproxycode socks5_req1_init(काष्ठा socks_ctx *sx,
                                      काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  अचिन्हित अक्षर req[5];
  अचिन्हित अक्षर ipbuf[16];
  स्थिर अचिन्हित अक्षर *destination;
  अचिन्हित अक्षर desttype, destlen, hdlen;
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  अगर(sx->resolve_local) {
    /* rest of request is added after resolving */
    परिणाम = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    अगर(परिणाम || (nwritten != 3))
      वापस CURLPX_SEND_REQUEST;
    वापस CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#अगर_घोषित USE_IPV6
  अगर(म_अक्षर(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    अगर(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      वापस CURLPX_BAD_ADDRESS_TYPE;
  }
  अन्यथा
#पूर्ण_अगर
  अगर(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  अन्यथा {
    स्थिर माप_प्रकार hostname_len = म_माप(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (स्थिर अचिन्हित अक्षर *)sx->dest->hostname;
    destlen = (अचिन्हित अक्षर)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  परिणाम = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  अगर(परिणाम || (nwritten != hdlen))
    वापस CURLPX_SEND_REQUEST;
  परिणाम = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  अगर(परिणाम || (nwritten != destlen))
    वापस CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (अचिन्हित अक्षर)((sx->dest->port >> 8) & 0xff);
  req[1] = (अचिन्हित अक्षर)(sx->dest->port & 0xff);
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  अगर(परिणाम || (nwritten != 2))
    वापस CURLPX_SEND_REQUEST;
  CURL_TRC_CF(तथ्य, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  वापस CURLPX_OK;
}

अटल CURLproxycode socks5_resolving(काष्ठा socks_ctx *sx,
                                      काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *done)
{
  स्थिर काष्ठा Curl_addrinfo *ai = शून्य;
  अक्षर dest[MAX_IPADR_LEN];  /* printable address */
  स्थिर अचिन्हित अक्षर *destination = शून्य;
  अचिन्हित अक्षर desttype = 1, destlen = 4;
  अचिन्हित अक्षर req[2];
  CURLcode परिणाम;
  CURLproxycode presult = CURLPX_OK;
  माप_प्रकार nwritten;

  *done = FALSE;
  परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex, sx->dest);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_AGAIN) {
      failf(तथ्य, "error %d resolving SOCKS destination %s:%u",
            (पूर्णांक)परिणाम, sx->dest->hostname, sx->dest->port);
      वापस CURLPX_RESOLVE_HOST;
    }
    वापस CURLPX_OK;
  }

#अगर_घोषित USE_IPV6
  अगर(तथ्य->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(तथ्य, sx->dest, cf->sockindex, AF_INET6, 0);
#पूर्ण_अगर
  अगर(!ai)
    ai = Curl_conn_dns_get_ai(तथ्य, sx->dest, cf->sockindex, AF_INET, 0);

  अगर(!ai) {
    failf(तथ्य, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    जाओ out;
  }

  Curl_printable_address(ai, dest, माप(dest));

  अगर(ai->ai_family == AF_INET) {
    काष्ठा sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (काष्ठा sockaddr_in *)(व्योम *)ai->ai_addr;
    destination = (स्थिर अचिन्हित अक्षर *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(तथ्य, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#अगर_घोषित USE_IPV6
  अन्यथा अगर(ai->ai_family == AF_INET6) {
    काष्ठा sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (काष्ठा sockaddr_in6 *)(व्योम *)ai->ai_addr;
    destination = (स्थिर अचिन्हित अक्षर *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(तथ्य, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#पूर्ण_अगर

  अगर(!destination) {
    failf(तथ्य, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    जाओ out;
  }

  req[0] = desttype;
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  अगर(परिणाम || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    जाओ out;
  }
  परिणाम = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  अगर(परिणाम || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    जाओ out;
  }
  /* PORT MSB+LSB */
  req[0] = (अचिन्हित अक्षर)((sx->dest->port >> 8) & 0xffU);
  req[1] = (अचिन्हित अक्षर)(sx->dest->port & 0xffU);
  परिणाम = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  अगर(परिणाम || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    जाओ out;
  }

out:
  *done = (presult == CURLPX_OK);
  वापस presult;
}

अटल CURLproxycode socks5_recv_resp1(काष्ठा socks_ctx *sx,
                                       काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       bool *done)
{
  स्थिर अचिन्हित अक्षर *resp;
  माप_प्रकार rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, तथ्य, resp_len, done);
  अगर(presult)
    वापस presult;
  अन्यथा अगर(!*done)
    वापस CURLPX_OK;

  अगर(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(तथ्य, "SOCKS5 response is incomplete.");
    वापस CURLPX_RECV_CONNECT;
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
  अगर(resp[0] != 5) { /* version */
    failf(तथ्य, "SOCKS5 reply has wrong version, version should be 5.");
    वापस CURLPX_BAD_VERSION;
  }
  अन्यथा अगर(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    पूर्णांक code = resp[1];
    failf(तथ्य, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    अगर(code < 9) {
      /* RFC 1928 section 6 lists: */
      अटल स्थिर CURLproxycode lookup[] = {
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
    वापस rc;
  }

  /* Calculate real packet size */
  चयन(resp[3]) {
  हाल 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    अवरोध;
  हाल 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    अवरोध;
  हाल 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    अवरोध;
  शेष:
    failf(तथ्य, "SOCKS5 reply has wrong address type.");
    वापस CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, तथ्य, resp_len, done);
  अगर(presult)
    वापस presult;
  अन्यथा अगर(!*done)
    वापस CURLPX_OK;

  अगर(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(तथ्य, "SOCKS5 response is incomplete.");
    वापस CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  वापस CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
अटल CURLproxycode socks5_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा socks_ctx *sx,
                                    काष्ठा Curl_easy *तथ्य)
{
  CURLproxycode presult;
  bool done;

process_state:
  चयन(sx->state) {
  हाल SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, तथ्य, SOCKS5_ST_START);
    FALLTHROUGH();

  हाल SOCKS5_ST_START:
    CURL_TRC_CF(तथ्य, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  हाल SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, तथ्य, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  हाल SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, तथ्य, 2, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    /* socks5_check_resp0() sets next socks state */
    जाओ process_state;

  हाल SOCKS5_ST_GSSAPI_INIT: {
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode परिणाम = Curl_SOCKS5_gssapi_negotiate(cf, तथ्य, sx->creds);
    अगर(परिणाम) {
      failf(तथ्य, "Unable to negotiate SOCKS5 GSS-API context.");
      वापस CURLPX_GSSAPI;
    }
    sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ1_INIT);
    जाओ process_state;
#अन्यथा
    failf(तथ्य, "SOCKS5 GSSAPI per-message authentication is not supported.");
    वापस socks_failed(sx, cf, तथ्य, CURLPX_GSSAPI_PERMSG);
#पूर्ण_अगर
  }

  हाल SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    sxstate(sx, cf, तथ्य, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  हाल SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    sxstate(sx, cf, तथ्य, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  हाल SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, तथ्य, 2, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  हाल SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, तथ्य);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अगर(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ1_SEND);
      जाओ process_state;
    }
    sxstate(sx, cf, तथ्य, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  हाल SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अगर(!done)
      वापस CURLPX_OK;
    sxstate(sx, cf, तथ्य, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  हाल SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अन्यथा अगर(!done)
      वापस CURLPX_OK;
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    अगर(cf->conn->socks5_gssapi_enctype) {
      failf(तथ्य, "SOCKS5 GSS-API protection not yet implemented.");
      वापस CURLPX_GSSAPI_PROTECTION;
    }
#पूर्ण_अगर
    sxstate(sx, cf, तथ्य, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  हाल SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, तथ्य, &done);
    अगर(presult)
      वापस socks_failed(sx, cf, तथ्य, presult);
    अगर(!done)
      वापस CURLPX_OK;
    CURL_TRC_CF(तथ्य, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, तथ्य, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  हाल SOCKS_ST_SUCCESS:
    वापस CURLPX_OK;

  हाल SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    वापस sx->presult;

  शेष:
    DEBUGASSERT(0);
    वापस socks_failed(sx, cf, तथ्य, CURLPX_SEND_REQUEST);
  }
}

अटल व्योम socks_proxy_ctx_free(काष्ठा socks_ctx *ctx)
{
  अगर(ctx) {
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
अटल CURLcode socks_proxy_cf_connect(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       bool *done)
{
  काष्ठा socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode परिणाम;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
  अगर(परिणाम || !*done)
    वापस परिणाम;

  चयन(ctx->proxy_type) {
  हाल CURLPROXY_SOCKS5:
  हाल CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, तथ्य);
    अवरोध;

  हाल CURLPROXY_SOCKS4:
  हाल CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, तथ्य);
    अवरोध;

  शेष:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    परिणाम = CURLE_COULDNT_CONNECT;
    जाओ out;
  }

  अगर(pxresult) {
    परिणाम = CURLE_PROXY;
    तथ्य->info.pxcode = (uint8_t)pxresult;
    जाओ out;
  }
  अन्यथा अगर(ctx->state != SOCKS_ST_SUCCESS)
    जाओ out;

#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_is_verbose(तथ्य)) {
    काष्ठा ip_quadruple ipquad;
    bool is_ipv6;
    अगर(!Curl_conn_cf_get_ip_info(cf->अग्र, तथ्य, &is_ipv6, &ipquad))
      infof(तथ्य, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    अन्यथा
      infof(तथ्य, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#पूर्ण_अगर
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  अगर(*done || परिणाम)
    Curl_creds_unlink(&ctx->creds);
  वापस परिणाम;
}

अटल CURLcode socks_cf_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा easy_pollset *ps)
{
  काष्ठा socks_ctx *sx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, तथ्य);
    चयन(sx->state) {
    हाल SOCKS4_ST_SEND:
    हाल SOCKS5_ST_REQ0_SEND:
    हाल SOCKS5_ST_AUTH_SEND:
    हाल SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(तथ्य, cf, "adjust pollset out (%d)", (पूर्णांक)sx->state);
      परिणाम = Curl_pollset_set_out_only(तथ्य, ps, sock);
      अवरोध;
    शेष:
      CURL_TRC_CF(तथ्य, cf, "adjust pollset in (%d)", (पूर्णांक)sx->state);
      परिणाम = Curl_pollset_set_in_only(तथ्य, ps, sock);
      अवरोध;
    }
  }
  वापस परिणाम;
}

अटल व्योम socks_proxy_cf_destroy(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  (व्योम)तथ्य;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = शून्य;
}

अटल CURLcode socks_cf_query(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा socks_ctx *sx = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_HOST_PORT:
    अगर(sx) {
      *pres1 = sx->dest->port;
      *((स्थिर अक्षर **)pres2) = sx->dest->hostname;
      वापस CURLE_OK;
    }
    अवरोध;
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

काष्ठा Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          काष्ठा Curl_creds *creds)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(तथ्य, ip_version);
  CURLcode परिणाम;

  अगर(!dest)
    वापस CURLE_FAILED_INIT;

  चयन(proxy_type) {
  हाल CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    अवरोध;
  हाल CURLPROXY_SOCKS5_HOSTNAME:
    अवरोध;
  हाल CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    अवरोध;
  हाल CURLPROXY_SOCKS4A:
    अवरोध;
  शेष:
    failf(तथ्य, "unknown proxytype %d option given", proxy_type);
    वापस CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    वापस CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_आकार, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  अगर(!परिणाम) {
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
    अगर(ctx->resolve_local) {
      परिणाम = Curl_conn_dns_add_addr_resolve(तथ्य, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  अन्यथा
    socks_proxy_ctx_free(ctx);
  वापस परिणाम;
}

#पूर्ण_अगर /* CURL_DISABLE_PROXY */
