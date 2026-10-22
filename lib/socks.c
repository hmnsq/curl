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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socks.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
ਗਿਣਤੀ socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_ਸ਼ੁਰੂ,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_ਸ਼ੁਰੂ,
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

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ cf_socks_statename[] = {
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
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKS_CHUNK_ਮਾਪ    1024
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKS_CHUNKS        1

ਬਣਤਰ socks_ctx {
  ਗਿਣਤੀ socks_state_t state;
  ਬਣਤਰ bufq iobuf;
  ਬਣਤਰ Curl_peer *dest;
  ਬਣਤਰ Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_ਸੰਸਕਰਣ;
  uint8_t proxy_type;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਸੰਸਕਰਣ_2;
  ਦੋਆਧਾਰੀ_ਅੰਕ(resolve_local);
  ਦੋਆਧਾਰੀ_ਅੰਕ(socks4a);
};

#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅੱਖਰ *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode ਨਤੀਜਾ;

  *pnread = 0;
  ਲਈ(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);

    ਜੇ(timeout_ms < 0) {
      /* we already got the timeout */
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
    }
    ਜੇ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ਜੇ(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
    ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਜਾਰੀ;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ(blen == nread) {
      *pnread += nread;
      ਵਾਪਸ CURLE_OK;
    }
    ਜੇ(!nread) /* EOF */
      ਵਾਪਸ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sxstate(x, c, d, y) socksstate(x, c, d, y)
#ਜੇ_ਅੰਤ

/* always use this function to change state, to make debugging easier */
ਸਥਿਰ ਖਾਲੀ socksstate(ਬਣਤਰ socks_ctx *sx,
                       ਬਣਤਰ Curl_cfilter *cf,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਗਿਣਤੀ socks_state_t state
#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , ਪੂਰਨ lineno
#ਜੇ_ਅੰਤ
)
{
  ਗਿਣਤੀ socks_state_t oldstate = sx->state;

  ਜੇ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ਵਾਪਸ;

  sx->state = state;

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLproxycode socks_failed(ਬਣਤਰ socks_ctx *sx,
                                  ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, ਅੰਕੜੇ, SOCKS_ST_FAILED);
  sx->presult = presult;
  ਵਾਪਸ presult;
}

ਸਥਿਰ CURLproxycode socks_flush(ਬਣਤਰ socks_ctx *sx,
                                 ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  size_t nwritten;

  *ਪੂਰਾ = FALSE;
  ਜਦੋਂ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&sx->iobuf)) {
    ਨਤੀਜਾ = Curl_cf_send_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &sx->iobuf, ਨਿਰਾ, 0,
                               &nwritten);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਵਾਪਸ CURLPX_OK;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Failed to send SOCKS request: %s",
            curl_easy_strerror(ਨਤੀਜਾ));
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_SEND_CONNECT);
    }
  }
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks_recv(ਬਣਤਰ socks_ctx *sx,
                                ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                size_t min_ਬਾਈਟਾਂ,
                                bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  size_t nread;

  *ਪੂਰਾ = FALSE;
  ਜਦੋਂ(Curl_bufq_len(&sx->iobuf) < min_ਬਾਈਟਾਂ) {
    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &sx->iobuf,
                               min_ਬਾਈਟਾਂ - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਵਾਪਸ CURLPX_OK;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(ਨਤੀਜਾ));
      ਵਾਪਸ CURLPX_RECV_CONNECT;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!nread) { /* EOF */
      ਜੇ(Curl_bufq_len(&sx->iobuf) < min_ਬਾਈਟਾਂ) {
        failf(ਅੰਕੜੇ, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ਵਾਪਸ CURLPX_RECV_CONNECT;
      }
      ਰੋਕੋ;
    }
  }
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks4_req_ਜੋੜੋ_hd(ਬਣਤਰ socks_ctx *sx,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[4];
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((sx->dest->ਪੋਰਟ >> 8) & 0xffU); /* MSB */
  buf[3] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(sx->dest->ਪੋਰਟ & 0xffU);        /* LSB */

  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 4))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks4_req_ਜੋੜੋ_user(ਬਣਤਰ socks_ctx *sx,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  size_t nwritten;

  ਜੇ(sx->creds) {
    size_t plen = ਲੜੀ_ਲੰਬਾਈ(sx->creds->user);
    ਜੇ(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(ਅੰਕੜੇ, "Too long SOCKS proxy username");
      ਵਾਪਸ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    ਨਤੀਜਾ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != (plen + 1)))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
  }
  ਨਹੀਂਤਾਂ {
    /* empty username */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ b = 0;
    ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != 1))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
  }
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks4_resolving(ਬਣਤਰ socks_ctx *sx,
                                      ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਪੂਰਾ)
{
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  size_t nwritten;

  *ਪੂਰਾ = FALSE;
  ਨਤੀਜਾ = Curl_conn_dns_addr_ਨਤੀਜਾ(cf->conn, cf->sockindex, sx->dest);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
      failf(ਅੰਕੜੇ, "error %d resolving SOCKS destination %s:%u",
            (ਪੂਰਨ)ਨਤੀਜਾ, sx->dest->hostname, sx->dest->ਪੋਰਟ);
      ਵਾਪਸ CURLPX_RESOLVE_HOST;
    }
    ਵਾਪਸ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(ਅੰਕੜੇ, sx->dest, cf->sockindex, AF_INET, 0);
  ਜੇ(ai) {
    ਬਣਤਰ sockaddr_in *saddr_ਵਿੱਚ;
    ਅੱਖਰ ipbuf[64];

    Curl_printable_ਪਤਾ(ai, ipbuf, ਆਕਾਰ(ipbuf));
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_ਵਿੱਚ = (ਬਣਤਰ sockaddr_in *)(ਖਾਲੀ *)ai->ai_addr;
    ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf,
                             (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&saddr_ਵਿੱਚ->sin_addr.s_addr, 4,
                             &nwritten);

    ਜੇ(ਨਤੀਜਾ || (nwritten != 4))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
  }
  ਨਹੀਂਤਾਂ {
    /* No IPv4 address resolved */
    failf(ਅੰਕੜੇ, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ਵਾਪਸ CURLPX_RESOLVE_HOST;
  }

  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks4_check_resp(ਬਣਤਰ socks_ctx *sx,
                                       ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *resp;
  size_t rlen;

  ਜੇ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(ਅੰਕੜੇ, "SOCKS4 reply is incomplete.");
    ਵਾਪਸ CURLPX_RECV_CONNECT;
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
  ਜੇ(resp[0]) {
    failf(ਅੰਕੜੇ, "SOCKS4 reply has wrong version, version should be 0.");
    ਵਾਪਸ CURLPX_BAD_ਸੰਸਕਰਣ;
  }

  /*ਨਤੀਜਾ*/
  ਚੋਣ(resp[1]) {
  ਹਾਲਤ 90:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ਵਾਪਸ CURLPX_OK;
  ਹਾਲਤ 91:
    failf(ਅੰਕੜੇ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((resp[2] << 8) | resp[3]), resp[1]);
    ਵਾਪਸ CURLPX_REQUEST_FAILED;
  ਹਾਲਤ 92:
    failf(ਅੰਕੜੇ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((resp[2] << 8) | resp[3]), resp[1]);
    ਵਾਪਸ CURLPX_IDENTD;
  ਹਾਲਤ 93:
    failf(ਅੰਕੜੇ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((resp[2] << 8) | resp[3]), resp[1]);
    ਵਾਪਸ CURLPX_IDENTD_ਫਰਕ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((resp[2] << 8) | resp[3]), resp[1]);
    ਵਾਪਸ CURLPX_UNKNOWN_FAIL;
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
ਸਥਿਰ CURLproxycode socks4_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ socks_ctx *sx,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode ਨਤੀਜਾ;
  bool ਪੂਰਾ;

process_state:
  ਚੋਣ(sx->state) {
  ਹਾਲਤ SOCKS_ST_INIT:
    sx->ਸੰਸਕਰਣ_2 = 4;
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS4_ST_ਸ਼ੁਰੂ);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS4_ST_ਸ਼ੁਰੂ:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_ਸੰਸਕਰਣ = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->ਪੋਰਟ);

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
    presult = socks4_req_ਜੋੜੋ_hd(sx, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ਜੇ(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[4] = { 0, 0, 0, 1 };
      size_t hlen = ਲੜੀ_ਲੰਬਾਈ(sx->dest->hostname) + 1; /* including NUL */

      ਜੇ(hlen > 255) {
        failf(ਅੰਕੜੇ, "SOCKS4: too long hostname");
        ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_LONG_HOSTNAME);
      }
      ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ਜੇ(ਨਤੀਜਾ || (nwritten != 4))
        ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_SEND_REQUEST);
      presult = socks4_req_ਜੋੜੋ_user(sx, ਅੰਕੜੇ);
      ਜੇ(presult)
        ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
      ਨਤੀਜਾ = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ਜੇ(ਨਤੀਜਾ || (nwritten != hlen))
        ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, ਅੰਕੜੇ, SOCKS4_ST_SEND);
      ਜਾਓ process_state;
    }
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    /* append user */
    presult = socks4_req_ਜੋੜੋ_user(sx, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS4_ST_SEND);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS4_ST_RECV);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, ਅੰਕੜੇ, 8, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS_ST_SUCCESS:
    ਵਾਪਸ CURLPX_OK;

  ਹਾਲਤ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ਵਾਪਸ sx->presult;

  ਮੂਲ:
    DEBUGASSERT(0);
    ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_SEND_REQUEST);
  }
}

ਸਥਿਰ CURLproxycode socks5_req0_init(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ socks_ctx *sx,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ auth = ਅੰਕੜੇ->ਸੈੱਟ.socks5auth;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ req[5]; /* version + len + 3 possible auth methods */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ nauths;
  size_t req_len, nwritten;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ਜੇ(!sx->resolve_local && ਲੜੀ_ਲੰਬਾਈ(sx->dest->hostname) > 255) {
    failf(ਅੰਕੜੇ, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ਵਾਪਸ CURLPX_LONG_HOSTNAME;
  }

  ਜੇ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(ਅੰਕੜੇ, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ਜੇ(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ਜੇ(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->ਬਿੱਟ.socks5_authenticated = FALSE;

  req[0] = 5;   /*ਸੰਸਕਰਣ*/
  nauths = 0;
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਜੇ(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#ਜੇ_ਅੰਤ
  ਜੇ(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ਜੇ(!nauths) {
    failf(ਅੰਕੜੇ, "SOCKS5: no acceptable authentication method is available.");
    ਵਾਪਸ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != req_len))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks5_check_resp0(ਬਣਤਰ socks_ctx *sx,
                                        ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *resp;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ auth_ਬਹੁਲਕ;
  size_t rlen;

  ਜੇ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ਅੰਕੜੇ, "SOCKS5 initial reply is incomplete.");
    ਵਾਪਸ CURLPX_RECV_CONNECT;
  }

  ਜੇ(resp[0] != 5) {
    failf(ਅੰਕੜੇ, "Received invalid version in initial SOCKS5 response.");
    ਵਾਪਸ CURLPX_BAD_ਸੰਸਕਰਣ;
  }

  auth_ਬਹੁਲਕ = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  ਚੋਣ(auth_ਬਹੁਲਕ) {
  ਹਾਲਤ 0:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5_auth_only) {
      failf(ਅੰਕੜੇ, "SOCKS5 proxy selected no authentication.");
      ਵਾਪਸ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ1_INIT);
    ਵਾਪਸ CURLPX_OK;
  ਹਾਲਤ 1:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5auth & CURLAUTH_GSSAPI) {
      ਜੇ(cf->sockindex == FIRSTSOCKET)
        cf->conn->ਬਿੱਟ.socks5_authenticated = TRUE;
      sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_GSSAPI_INIT);
      ਵਾਪਸ CURLPX_OK;
    }
    failf(ਅੰਕੜੇ, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ਵਾਪਸ CURLPX_GSSAPI_PERMSG;
  ਹਾਲਤ 2:
    /* regular name + password authentication */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5auth & CURLAUTH_BASIC) {
      ਜੇ(cf->sockindex == FIRSTSOCKET)
        cf->conn->ਬਿੱਟ.socks5_authenticated = TRUE;
      sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_AUTH_INIT);
      ਵਾਪਸ CURLPX_OK;
    }
    failf(ਅੰਕੜੇ, "BASIC authentication proposed but not enabled.");
    ਵਾਪਸ CURLPX_NO_AUTH;
  ਹਾਲਤ 255:
    failf(ਅੰਕੜੇ, "No authentication method was acceptable.");
    ਵਾਪਸ CURLPX_NO_AUTH;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unknown SOCKS5 mode attempted to be used by server.");
    ਵਾਪਸ CURLPX_UNKNOWN_ਬਹੁਲਕ;
  }
}

ਸਥਿਰ CURLproxycode socks5_auth_init(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ socks_ctx *sx,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[2];
  CURLcode ਨਤੀਜਾ;

  ਜੇ(sx->creds) {
    ulen = ਲੜੀ_ਲੰਬਾਈ(sx->creds->user);
    plen = ਲੜੀ_ਲੰਬਾਈ(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ਜੇ(ulen > 255) {
      failf(ਅੰਕੜੇ, "Excessive username length for proxy auth");
      ਵਾਪਸ CURLPX_LONG_USER;
    }
    ਜੇ(plen > 255) {
      failf(ਅੰਕੜੇ, "Excessive password length for proxy auth");
      ਵਾਪਸ CURLPX_LONG_PASSWD;
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
  buf[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ulen;
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 2))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  ਜੇ(ulen) {
    ਨਤੀਜਾ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != ulen))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
  }
  buf[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)plen;
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 1))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  ਜੇ(plen) {
    ਨਤੀਜਾ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != plen))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_AUTH_SEND);
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks5_check_auth_resp(ਬਣਤਰ socks_ctx *sx,
                                            ਬਣਤਰ Curl_cfilter *cf,
                                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *resp;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ auth_ਸਥਿਤੀ;
  size_t rlen;

  (ਖਾਲੀ)cf;
  ਜੇ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ਅੰਕੜੇ, "SOCKS5 sub-negotiation response incomplete.");
    ਵਾਪਸ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_ਸਥਿਤੀ = resp[1];
  ਜੇ(auth_ਸਥਿਤੀ) {
    failf(ਅੰਕੜੇ, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ਵਾਪਸ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks5_req1_init(ਬਣਤਰ socks_ctx *sx,
                                      ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ req[5];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ipbuf[16];
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *destination;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ਜੇ(sx->resolve_local) {
    /* rest of request is added after resolving */
    ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != 3))
      ਵਾਪਸ CURLPX_SEND_REQUEST;
    ਵਾਪਸ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ਜੇ(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ਵਾਪਸ CURLPX_BAD_ਪਤਾ_TYPE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ size_t hostname_len = ਲੜੀ_ਲੰਬਾਈ(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)sx->dest->hostname;
    destlen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != hdlen))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != destlen))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((sx->dest->ਪੋਰਟ >> 8) & 0xff);
  req[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(sx->dest->ਪੋਰਟ & 0xff);
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 2))
    ਵਾਪਸ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->ਪੋਰਟ);
  ਵਾਪਸ CURLPX_OK;
}

ਸਥਿਰ CURLproxycode socks5_resolving(ਬਣਤਰ socks_ctx *sx,
                                      ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਪੂਰਾ)
{
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
  ਅੱਖਰ dest[MAX_IPADR_LEN];  /* printable address */
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *destination = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ desttype = 1, destlen = 4;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ req[2];
  CURLcode ਨਤੀਜਾ;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *ਪੂਰਾ = FALSE;
  ਨਤੀਜਾ = Curl_conn_dns_addr_ਨਤੀਜਾ(cf->conn, cf->sockindex, sx->dest);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
      failf(ਅੰਕੜੇ, "error %d resolving SOCKS destination %s:%u",
            (ਪੂਰਨ)ਨਤੀਜਾ, sx->dest->hostname, sx->dest->ਪੋਰਟ);
      ਵਾਪਸ CURLPX_RESOLVE_HOST;
    }
    ਵਾਪਸ CURLPX_OK;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(ਅੰਕੜੇ, sx->dest, cf->sockindex, AF_INET6, 0);
#ਜੇ_ਅੰਤ
  ਜੇ(!ai)
    ai = Curl_conn_dns_get_ai(ਅੰਕੜੇ, sx->dest, cf->sockindex, AF_INET, 0);

  ਜੇ(!ai) {
    failf(ਅੰਕੜੇ, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  Curl_printable_ਪਤਾ(ai, dest, ਆਕਾਰ(dest));

  ਜੇ(ai->ai_family == AF_INET) {
    ਬਣਤਰ sockaddr_in *saddr_ਵਿੱਚ;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_ਵਿੱਚ = (ਬਣਤਰ sockaddr_in *)(ਖਾਲੀ *)ai->ai_addr;
    destination = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&saddr_ਵਿੱਚ->sin_addr.s_addr;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->ਪੋਰਟ);
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਨਹੀਂਤਾਂ ਜੇ(ai->ai_family == AF_INET6) {
    ਬਣਤਰ sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ਬਣਤਰ sockaddr_in6 *)(ਖਾਲੀ *)ai->ai_addr;
    destination = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->ਪੋਰਟ);
  }
#ਜੇ_ਅੰਤ

  ਜੇ(!destination) {
    failf(ਅੰਕੜੇ, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  req[0] = desttype;
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* PORT MSB+LSB */
  req[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((sx->dest->ਪੋਰਟ >> 8) & 0xffU);
  req[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(sx->dest->ਪੋਰਟ & 0xffU);
  ਨਤੀਜਾ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

ਬਾਹਰਲਾ:
  *ਪੂਰਾ = (presult == CURLPX_OK);
  ਵਾਪਸ presult;
}

ਸਥਿਰ CURLproxycode socks5_recv_resp1(ਬਣਤਰ socks_ctx *sx,
                                       ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਪੂਰਾ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, ਅੰਕੜੇ, resp_len, ਪੂਰਾ);
  ਜੇ(presult)
    ਵਾਪਸ presult;
  ਨਹੀਂਤਾਂ ਜੇ(!*ਪੂਰਾ)
    ਵਾਪਸ CURLPX_OK;

  ਜੇ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ਅੰਕੜੇ, "SOCKS5 response is incomplete.");
    ਵਾਪਸ CURLPX_RECV_CONNECT;
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
  ਜੇ(resp[0] != 5) { /*ਸੰਸਕਰਣ*/
    failf(ਅੰਕੜੇ, "SOCKS5 reply has wrong version, version should be 5.");
    ਵਾਪਸ CURLPX_BAD_ਸੰਸਕਰਣ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_ਜਵਾਬ_UNASSIGNED;
    ਪੂਰਨ ਕੋਡ = resp[1];
    failf(ਅੰਕੜੇ, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, ਕੋਡ);
    ਜੇ(ਕੋਡ < 9) {
      /* RFC 1928 section 6 lists: */
      ਸਥਿਰ ਅਚਲ CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_ਜਵਾਬ_GENERAL_SERVER_FAILURE,
        CURLPX_ਜਵਾਬ_NOT_ALLOWED,
        CURLPX_ਜਵਾਬ_ਜਾਲ_UNREACHABLE,
        CURLPX_ਜਵਾਬ_HOST_UNREACHABLE,
        CURLPX_ਜਵਾਬ_CONNECTION_REFUSED,
        CURLPX_ਜਵਾਬ_TTL_EXPIRED,
        CURLPX_ਜਵਾਬ_COMMAND_NOT_SUPPORTED,
        CURLPX_ਜਵਾਬ_ਪਤਾ_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[ਕੋਡ];
    }
    ਵਾਪਸ rc;
  }

  /* Calculate real packet size */
  ਚੋਣ(resp[3]) {
  ਹਾਲਤ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ਰੋਕੋ;
  ਹਾਲਤ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ਰੋਕੋ;
  ਹਾਲਤ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "SOCKS5 reply has wrong address type.");
    ਵਾਪਸ CURLPX_BAD_ਪਤਾ_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, ਅੰਕੜੇ, resp_len, ਪੂਰਾ);
  ਜੇ(presult)
    ਵਾਪਸ presult;
  ਨਹੀਂਤਾਂ ਜੇ(!*ਪੂਰਾ)
    ਵਾਪਸ CURLPX_OK;

  ਜੇ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ਅੰਕੜੇ, "SOCKS5 response is incomplete.");
    ਵਾਪਸ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ਸਥਿਰ CURLproxycode socks5_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ socks_ctx *sx,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLproxycode presult;
  bool ਪੂਰਾ;

process_state:
  ਚੋਣ(sx->state) {
  ਹਾਲਤ SOCKS_ST_INIT:
    sx->ਸੰਸਕਰਣ_2 = 5;
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_ਸ਼ੁਰੂ);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_ਸ਼ੁਰੂ:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->ਪੋਰਟ);
    presult = socks5_req0_init(cf, sx, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, ਅੰਕੜੇ, 2, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    /* socks5_check_resp0() sets next socks state */
    ਜਾਓ process_state;

  ਹਾਲਤ SOCKS5_ST_GSSAPI_INIT: {
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode ਨਤੀਜਾ = Curl_SOCKS5_gssapi_negotiate(cf, ਅੰਕੜੇ, sx->creds);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Unable to negotiate SOCKS5 GSS-API context.");
      ਵਾਪਸ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ1_INIT);
    ਜਾਓ process_state;
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_GSSAPI_PERMSG);
#ਜੇ_ਅੰਤ
  }

  ਹਾਲਤ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, ਅੰਕੜੇ, 2, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, ਅੰਕੜੇ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਜੇ(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ1_SEND);
      ਜਾਓ process_state;
    }
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ਜੇ(cf->conn->socks5_gssapi_enctype) {
      failf(ਅੰਕੜੇ, "SOCKS5 GSS-API protection not yet implemented.");
      ਵਾਪਸ CURLPX_GSSAPI_PROTECTION;
    }
#ਜੇ_ਅੰਤ
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(presult)
      ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, presult);
    ਜੇ(!ਪੂਰਾ)
      ਵਾਪਸ CURLPX_OK;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, ਅੰਕੜੇ, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ਹਾਲਤ SOCKS_ST_SUCCESS:
    ਵਾਪਸ CURLPX_OK;

  ਹਾਲਤ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ਵਾਪਸ sx->presult;

  ਮੂਲ:
    DEBUGASSERT(0);
    ਵਾਪਸ socks_failed(sx, cf, ਅੰਕੜੇ, CURLPX_SEND_REQUEST);
  }
}

ਸਥਿਰ ਖਾਲੀ socks_proxy_ctx_free(ਬਣਤਰ socks_ctx *ctx)
{
  ਜੇ(ctx) {
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
ਸਥਿਰ CURLcode socks_proxy_cf_connect(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਪੂਰਾ)
{
  ਬਣਤਰ socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਚੋਣ(ctx->proxy_type) {
  ਹਾਲਤ CURLPROXY_SOCKS5:
  ਹਾਲਤ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, ਅੰਕੜੇ);
    ਰੋਕੋ;

  ਹਾਲਤ CURLPROXY_SOCKS4:
  ਹਾਲਤ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, ਅੰਕੜੇ);
    ਰੋਕੋ;

  ਮੂਲ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(pxresult) {
    ਨਤੀਜਾ = CURLE_PROXY;
    ਅੰਕੜੇ->info.pxcode = (uint8_t)pxresult;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ctx->state != SOCKS_ST_SUCCESS)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    ਬਣਤਰ ip_quadruple ipquad;
    bool is_ipv6;
    ਜੇ(!Curl_conn_cf_get_ip_info(cf->ਅਗਲਾ, ਅੰਕੜੇ, &is_ipv6, &ipquad))
      infof(ਅੰਕੜੇ, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_ਪੋਰਟ,
            ctx->dest->hostname, ctx->dest->ਪੋਰਟ,
            ipquad.remote_ip, ipquad.remote_ਪੋਰਟ);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#ਜੇ_ਅੰਤ
  cf->connected = TRUE;

ਬਾਹਰਲਾ:
  *ਪੂਰਾ = (bool)cf->connected;
  ਜੇ(*ਪੂਰਾ || ਨਤੀਜਾ)
    Curl_creds_unlink(&ctx->creds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode socks_cf_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ socks_ctx *sx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
    ਚੋਣ(sx->state) {
    ਹਾਲਤ SOCKS4_ST_SEND:
    ਹਾਲਤ SOCKS5_ST_REQ0_SEND:
    ਹਾਲਤ SOCKS5_ST_AUTH_SEND:
    ਹਾਲਤ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust pollset out (%d)", (ਪੂਰਨ)sx->state);
      ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, sock);
      ਰੋਕੋ;
    ਮੂਲ:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust pollset in (%d)", (ਪੂਰਨ)sx->state);
      ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਵਿੱਚ_only(ਅੰਕੜੇ, ps, sock);
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ socks_proxy_cf_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ਨਿਰਾ;
}

ਸਥਿਰ CURLcode socks_cf_query(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ socks_ctx *sx = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_HOST_ਪੋਰਟ:
    ਜੇ(sx) {
      *pres1 = sx->dest->ਪੋਰਟ;
      *((ਅਚਲ ਅੱਖਰ **)pres2) = sx->dest->hostname;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਬਣਤਰ Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_peer *dest,
                                          uint8_t ip_ਸੰਸਕਰਣ,
                                          uint8_t proxy_type,
                                          ਬਣਤਰ Curl_creds *creds)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(ਅੰਕੜੇ, ip_ਸੰਸਕਰਣ);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!dest)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਚੋਣ(proxy_type) {
  ਹਾਲਤ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLPROXY_SOCKS5_HOSTNAME:
    ਰੋਕੋ;
  ਹਾਲਤ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ਰੋਕੋ;
  ਹਾਲਤ CURLPROXY_SOCKS4A:
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "unknown proxytype %d option given", proxy_type);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_ਸੰਸਕਰਣ = ip_ਸੰਸਕਰਣ;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_ਮਾਪ, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ਜੇ(!ਨਤੀਜਾ) {
    Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
    ਜੇ(ctx->resolve_local) {
      ਨਤੀਜਾ = Curl_conn_dns_ਜੋੜੋ_addr_resolve(ਅੰਕੜੇ, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ਨਹੀਂਤਾਂ
    socks_proxy_ctx_free(ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */
