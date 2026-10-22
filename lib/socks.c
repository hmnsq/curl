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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "socks.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
ಎಣಿಕೆ socks_state_t {
  SOCKS_ST_INIT,
  /* SOCKS Version 4 states */
  SOCKS4_ST_ಆರಂಭ,
  SOCKS4_ST_RESOLVING,
  SOCKS4_ST_SEND,
  SOCKS4_ST_RECV,
  /* SOCKS Version 5 states */
  SOCKS5_ST_ಆರಂಭ,
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

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ cf_socks_statename[] = {
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
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು SOCKS_CHUNK_ಗಾತ್ರ    1024
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKS_CHUNKS        1

ರಚನೆ socks_ctx {
  ಎಣಿಕೆ socks_state_t state;
  ರಚನೆ bufq iobuf;
  ರಚನೆ Curl_peer *dest;
  ರಚನೆ Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_ಆವೃತ್ತಿ;
  uint8_t proxy_type;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಆವೃತ್ತಿ_2;
  ಬಿಟ್(resolve_local);
  ಬಿಟ್(socks4a);
};

#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಅಕ್ಷರ *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode ಫಲಿತಾಂಶ;

  *pnread = 0;
  ಗಾಗಿ(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);

    ಆದರೆ(timeout_ms < 0) {
      /* we already got the timeout */
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
    }
    ಆದರೆ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ಆದರೆ(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
    ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಮುಂದುವರಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ(blen == nread) {
      *pnread += nread;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಆದರೆ(!nread) /* EOF */
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#ವ್ಯಾಖ್ಯಾನಿಸು sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು sxstate(x, c, d, y) socksstate(x, c, d, y)
#ಆದರೆ_ಅಂತ್ಯ

/* always use this function to change state, to make debugging easier */
ಸ್ಥಿರ ಶೂನ್ಯ socksstate(ರಚನೆ socks_ctx *sx,
                       ರಚನೆ Curl_cfilter *cf,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ಎಣಿಕೆ socks_state_t state
#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , ಪೂರ್ಣಾಂಕ lineno
#ಆದರೆ_ಅಂತ್ಯ
)
{
  ಎಣಿಕೆ socks_state_t oldstate = sx->state;

  ಆದರೆ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ಹಿಂತಿರುಗಿ;

  sx->state = state;

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLproxycode socks_failed(ರಚನೆ socks_ctx *sx,
                                  ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS_ST_FAILED);
  sx->presult = presult;
  ಹಿಂತಿರುಗಿ presult;
}

ಸ್ಥಿರ CURLproxycode socks_flush(ರಚನೆ socks_ctx *sx,
                                 ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t nwritten;

  *ಮುಗಿದ = FALSE;
  ಆಗ(!Curl_bufq_is_ಖಾಲಿಗಳು(&sx->iobuf)) {
    ಫಲಿತಾಂಶ = Curl_cf_send_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &sx->iobuf, ಖಾಲಿ, 0,
                               &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Failed to send SOCKS request: %s",
            curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_SEND_CONNECT);
    }
  }
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks_recv(ರಚನೆ socks_ctx *sx,
                                ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                size_t min_ಬೈಟುಗಳು,
                                bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t nread;

  *ಮುಗಿದ = FALSE;
  ಆಗ(Curl_bufq_len(&sx->iobuf) < min_ಬೈಟುಗಳು) {
    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &sx->iobuf,
                               min_ಬೈಟುಗಳು - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) { /* EOF */
      ಆದರೆ(Curl_bufq_len(&sx->iobuf) < min_ಬೈಟುಗಳು) {
        failf(ದತ್ತಾಂಶ, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_req_ಕೂಡಿಸು_hd(ರಚನೆ socks_ctx *sx,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[4];
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((sx->dest->ಪೋರ್ಟ್ >> 8) & 0xffU); /* MSB */
  buf[3] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(sx->dest->ಪೋರ್ಟ್ & 0xffU);        /* LSB */

  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 4))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_req_ಕೂಡಿಸು_user(ರಚನೆ socks_ctx *sx,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t nwritten;

  ಆದರೆ(sx->creds) {
    size_t plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->creds->user);
    ಆದರೆ(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(ದತ್ತಾಂಶ, "Too long SOCKS proxy username");
      ಹಿಂತಿರುಗಿ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != (plen + 1)))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* empty username */
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ b = 0;
    ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 1))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  }
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_resolving(ರಚನೆ socks_ctx *sx,
                                      ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಮುಗಿದ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  size_t nwritten;

  *ಮುಗಿದ = FALSE;
  ಫಲಿತಾಂಶ = Curl_conn_dns_addr_ಫಲಿತಾಂಶ(cf->conn, cf->sockindex, sx->dest);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
      failf(ದತ್ತಾಂಶ, "error %d resolving SOCKS destination %s:%u",
            (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, sx->dest->hostname, sx->dest->ಪೋರ್ಟ್);
      ಹಿಂತಿರುಗಿ CURLPX_RESOLVE_HOST;
    }
    ಹಿಂತಿರುಗಿ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(ದತ್ತಾಂಶ, sx->dest, cf->sockindex, AF_INET, 0);
  ಆದರೆ(ai) {
    ರಚನೆ sockaddr_in *saddr_ಒಳ;
    ಅಕ್ಷರ ipbuf[64];

    Curl_printable_ವಿಳಾಸ(ai, ipbuf, ಗಾತ್ರ(ipbuf));
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_ಒಳ = (ರಚನೆ sockaddr_in *)(ಶೂನ್ಯ *)ai->ai_addr;
    ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf,
                             (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&saddr_ಒಳ->sin_addr.s_addr, 4,
                             &nwritten);

    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 4))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* No IPv4 address resolved */
    failf(ದತ್ತಾಂಶ, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ಹಿಂತಿರುಗಿ CURLPX_RESOLVE_HOST;
  }

  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_check_resp(ರಚನೆ socks_ctx *sx,
                                       ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *resp;
  size_t rlen;

  ಆದರೆ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(ದತ್ತಾಂಶ, "SOCKS4 reply is incomplete.");
    ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
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
  ಆದರೆ(resp[0]) {
    failf(ದತ್ತಾಂಶ, "SOCKS4 reply has wrong version, version should be 0.");
    ಹಿಂತಿರುಗಿ CURLPX_BAD_ಆವೃತ್ತಿ;
  }

  /*ಫಲಿತಾಂಶ*/
  ಆಯ್ಕೆ(resp[1]) {
  ಸ್ಥಿತಿ 90:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ಹಿಂತಿರುಗಿ CURLPX_OK;
  ಸ್ಥಿತಿ 91:
    failf(ದತ್ತಾಂಶ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಹಿಂತಿರುಗಿ CURLPX_REQUEST_FAILED;
  ಸ್ಥಿತಿ 92:
    failf(ದತ್ತಾಂಶ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಹಿಂತಿರುಗಿ CURLPX_IDENTD;
  ಸ್ಥಿತಿ 93:
    failf(ದತ್ತಾಂಶ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಹಿಂತಿರುಗಿ CURLPX_IDENTD_ವ್ಯತ್ಯಾಸ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಹಿಂತಿರುಗಿ CURLPX_UNKNOWN_FAIL;
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
ಸ್ಥಿರ CURLproxycode socks4_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ socks_ctx *sx,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode ಫಲಿತಾಂಶ;
  bool ಮುಗಿದ;

process_state:
  ಆಯ್ಕೆ(sx->state) {
  ಸ್ಥಿತಿ SOCKS_ST_INIT:
    sx->ಆವೃತ್ತಿ_2 = 4;
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS4_ST_ಆರಂಭ);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS4_ST_ಆರಂಭ:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_ಆವೃತ್ತಿ = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS4%s connecting to %s:%u",
                sx->socks4a ? "a" : "",
                sx->dest->hostname, sx->dest->ಪೋರ್ಟ್);

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
    presult = socks4_req_ಕೂಡಿಸು_hd(sx, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ಆದರೆ(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[4] = { 0, 0, 0, 1 };
      size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->dest->hostname) + 1; /* including NUL */

      ಆದರೆ(hlen > 255) {
        failf(ದತ್ತಾಂಶ, "SOCKS4: too long hostname");
        ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_LONG_HOSTNAME);
      }
      ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 4))
        ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_SEND_REQUEST);
      presult = socks4_req_ಕೂಡಿಸು_user(sx, ದತ್ತಾಂಶ);
      ಆದರೆ(presult)
        ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != hlen))
        ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS4_ST_SEND);
      ಹೋಗು process_state;
    }
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    /* append user */
    presult = socks4_req_ಕೂಡಿಸು_user(sx, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS4_ST_SEND);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS4_ST_RECV);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, ದತ್ತಾಂಶ, 8, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS_ST_SUCCESS:
    ಹಿಂತಿರುಗಿ CURLPX_OK;

  ಸ್ಥಿತಿ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ಹಿಂತಿರುಗಿ sx->presult;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_SEND_REQUEST);
  }
}

ಸ್ಥಿರ CURLproxycode socks5_req0_init(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ socks_ctx *sx,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ auth = ದತ್ತಾಂಶ->ಗಣ.socks5auth;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ req[5]; /* version + len + 3 possible auth methods */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ nauths;
  size_t req_len, nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ಆದರೆ(!sx->resolve_local && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->dest->hostname) > 255) {
    failf(ದತ್ತಾಂಶ, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ಹಿಂತಿರುಗಿ CURLPX_LONG_HOSTNAME;
  }

  ಆದರೆ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(ದತ್ತಾಂಶ, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ಆದರೆ(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ಆದರೆ(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->ಬಿಟ್ಟುಗಳು.socks5_authenticated = FALSE;

  req[0] = 5;   /*ಆವೃತ್ತಿ*/
  nauths = 0;
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಆದರೆ(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ಆದರೆ(!nauths) {
    failf(ದತ್ತಾಂಶ, "SOCKS5: no acceptable authentication method is available.");
    ಹಿಂತಿರುಗಿ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != req_len))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_check_resp0(ರಚನೆ socks_ctx *sx,
                                        ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *resp;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ auth_ಬಹುಲಕ;
  size_t rlen;

  ಆದರೆ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ದತ್ತಾಂಶ, "SOCKS5 initial reply is incomplete.");
    ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
  }

  ಆದರೆ(resp[0] != 5) {
    failf(ದತ್ತಾಂಶ, "Received invalid version in initial SOCKS5 response.");
    ಹಿಂತಿರುಗಿ CURLPX_BAD_ಆವೃತ್ತಿ;
  }

  auth_ಬಹುಲಕ = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  ಆಯ್ಕೆ(auth_ಬಹುಲಕ) {
  ಸ್ಥಿತಿ 0:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5_auth_only) {
      failf(ದತ್ತಾಂಶ, "SOCKS5 proxy selected no authentication.");
      ಹಿಂತಿರುಗಿ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ1_INIT);
    ಹಿಂತಿರುಗಿ CURLPX_OK;
  ಸ್ಥಿತಿ 1:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5auth & CURLAUTH_GSSAPI) {
      ಆದರೆ(cf->sockindex == FIRSTSOCKET)
        cf->conn->ಬಿಟ್ಟುಗಳು.socks5_authenticated = TRUE;
      sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_GSSAPI_INIT);
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    }
    failf(ದತ್ತಾಂಶ, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ಹಿಂತಿರುಗಿ CURLPX_GSSAPI_PERMSG;
  ಸ್ಥಿತಿ 2:
    /* regular name + password authentication */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5auth & CURLAUTH_BASIC) {
      ಆದರೆ(cf->sockindex == FIRSTSOCKET)
        cf->conn->ಬಿಟ್ಟುಗಳು.socks5_authenticated = TRUE;
      sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_AUTH_INIT);
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    }
    failf(ದತ್ತಾಂಶ, "BASIC authentication proposed but not enabled.");
    ಹಿಂತಿರುಗಿ CURLPX_NO_AUTH;
  ಸ್ಥಿತಿ 255:
    failf(ದತ್ತಾಂಶ, "No authentication method was acceptable.");
    ಹಿಂತಿರುಗಿ CURLPX_NO_AUTH;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unknown SOCKS5 mode attempted to be used by server.");
    ಹಿಂತಿರುಗಿ CURLPX_UNKNOWN_ಬಹುಲಕ;
  }
}

ಸ್ಥಿರ CURLproxycode socks5_auth_init(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ socks_ctx *sx,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[2];
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(sx->creds) {
    ulen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->creds->user);
    plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ಆದರೆ(ulen > 255) {
      failf(ದತ್ತಾಂಶ, "Excessive username length for proxy auth");
      ಹಿಂತಿರುಗಿ CURLPX_LONG_USER;
    }
    ಆದರೆ(plen > 255) {
      failf(ದತ್ತಾಂಶ, "Excessive password length for proxy auth");
      ಹಿಂತಿರುಗಿ CURLPX_LONG_PASSWD;
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
  buf[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ulen;
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 2))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  ಆದರೆ(ulen) {
    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != ulen))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  }
  buf[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)plen;
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 1))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  ಆದರೆ(plen) {
    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != plen))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_AUTH_SEND);
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_check_auth_resp(ರಚನೆ socks_ctx *sx,
                                            ರಚನೆ Curl_cfilter *cf,
                                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *resp;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ auth_ಸ್ಥಿತಿ;
  size_t rlen;

  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(ದತ್ತಾಂಶ, "SOCKS5 sub-negotiation response incomplete.");
    ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_ಸ್ಥಿತಿ = resp[1];
  ಆದರೆ(auth_ಸ್ಥಿತಿ) {
    failf(ದತ್ತಾಂಶ, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ಹಿಂತಿರುಗಿ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_req1_init(ರಚನೆ socks_ctx *sx,
                                      ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ req[5];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ipbuf[16];
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *destination;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ಆದರೆ(sx->resolve_local) {
    /* rest of request is added after resolving */
    ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 3))
      ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
    ಹಿಂತಿರುಗಿ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ಆದರೆ(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ಹಿಂತಿರುಗಿ CURLPX_BAD_ವಿಳಾಸ_TYPE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ size_t hostname_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)sx->dest->hostname;
    destlen = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != hdlen))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != destlen))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((sx->dest->ಪೋರ್ಟ್ >> 8) & 0xff);
  req[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(sx->dest->ಪೋರ್ಟ್ & 0xff);
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 2))
    ಹಿಂತಿರುಗಿ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->ಪೋರ್ಟ್);
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_resolving(ರಚನೆ socks_ctx *sx,
                                      ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಮುಗಿದ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai = ಖಾಲಿ;
  ಅಕ್ಷರ dest[MAX_IPADR_LEN];  /* printable address */
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *destination = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ desttype = 1, destlen = 4;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ req[2];
  CURLcode ಫಲಿತಾಂಶ;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *ಮುಗಿದ = FALSE;
  ಫಲಿತಾಂಶ = Curl_conn_dns_addr_ಫಲಿತಾಂಶ(cf->conn, cf->sockindex, sx->dest);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
      failf(ದತ್ತಾಂಶ, "error %d resolving SOCKS destination %s:%u",
            (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, sx->dest->hostname, sx->dest->ಪೋರ್ಟ್);
      ಹಿಂತಿರುಗಿ CURLPX_RESOLVE_HOST;
    }
    ಹಿಂತಿರುಗಿ CURLPX_OK;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(ದತ್ತಾಂಶ, sx->dest, cf->sockindex, AF_INET6, 0);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ai)
    ai = Curl_conn_dns_get_ai(ದತ್ತಾಂಶ, sx->dest, cf->sockindex, AF_INET, 0);

  ಆದರೆ(!ai) {
    failf(ದತ್ತಾಂಶ, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  Curl_printable_ವಿಳಾಸ(ai, dest, ಗಾತ್ರ(dest));

  ಆದರೆ(ai->ai_family == AF_INET) {
    ರಚನೆ sockaddr_in *saddr_ಒಳ;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_ಒಳ = (ರಚನೆ sockaddr_in *)(ಶೂನ್ಯ *)ai->ai_addr;
    destination = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&saddr_ಒಳ->sin_addr.s_addr;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->ಪೋರ್ಟ್);
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ai->ai_family == AF_INET6) {
    ರಚನೆ sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ರಚನೆ sockaddr_in6 *)(ಶೂನ್ಯ *)ai->ai_addr;
    destination = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->ಪೋರ್ಟ್);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!destination) {
    failf(ದತ್ತಾಂಶ, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  req[0] = desttype;
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* PORT MSB+LSB */
  req[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((sx->dest->ಪೋರ್ಟ್ >> 8) & 0xffU);
  req[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(sx->dest->ಪೋರ್ಟ್ & 0xffU);
  ಫಲಿತಾಂಶ = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    ಹೋಗು ಹೊರಹೋಗು;
  }

ಹೊರಹೋಗು:
  *ಮುಗಿದ = (presult == CURLPX_OK);
  ಹಿಂತಿರುಗಿ presult;
}

ಸ್ಥಿರ CURLproxycode socks5_recv_resp1(ರಚನೆ socks_ctx *sx,
                                       ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಮುಗಿದ)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, ದತ್ತಾಂಶ, resp_len, ಮುಗಿದ);
  ಆದರೆ(presult)
    ಹಿಂತಿರುಗಿ presult;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLPX_OK;

  ಆದರೆ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ದತ್ತಾಂಶ, "SOCKS5 response is incomplete.");
    ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
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
  ಆದರೆ(resp[0] != 5) { /*ಆವೃತ್ತಿ*/
    failf(ದತ್ತಾಂಶ, "SOCKS5 reply has wrong version, version should be 5.");
    ಹಿಂತಿರುಗಿ CURLPX_BAD_ಆವೃತ್ತಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_ಉತ್ತರ_UNASSIGNED;
    ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2 = resp[1];
    failf(ದತ್ತಾಂಶ, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, ಸಂಕೇತ_2);
    ಆದರೆ(ಸಂಕೇತ_2 < 9) {
      /* RFC 1928 section 6 lists: */
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ CURLproxycode lookup[] = {
        CURLPX_OK,
        CURLPX_ಉತ್ತರ_GENERAL_SERVER_FAILURE,
        CURLPX_ಉತ್ತರ_NOT_ALLOWED,
        CURLPX_ಉತ್ತರ_ಜಾಲ_UNREACHABLE,
        CURLPX_ಉತ್ತರ_HOST_UNREACHABLE,
        CURLPX_ಉತ್ತರ_CONNECTION_REFUSED,
        CURLPX_ಉತ್ತರ_TTL_EXPIRED,
        CURLPX_ಉತ್ತರ_COMMAND_NOT_SUPPORTED,
        CURLPX_ಉತ್ತರ_ವಿಳಾಸ_TYPE_NOT_SUPPORTED,
      };
      rc = lookup[ಸಂಕೇತ_2];
    }
    ಹಿಂತಿರುಗಿ rc;
  }

  /* Calculate real packet size */
  ಆಯ್ಕೆ(resp[3]) {
  ಸ್ಥಿತಿ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "SOCKS5 reply has wrong address type.");
    ಹಿಂತಿರುಗಿ CURLPX_BAD_ವಿಳಾಸ_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, ದತ್ತಾಂಶ, resp_len, ಮುಗಿದ);
  ಆದರೆ(presult)
    ಹಿಂತಿರುಗಿ presult;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLPX_OK;

  ಆದರೆ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(ದತ್ತಾಂಶ, "SOCKS5 response is incomplete.");
    ಹಿಂತಿರುಗಿ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ಸ್ಥಿರ CURLproxycode socks5_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ socks_ctx *sx,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLproxycode presult;
  bool ಮುಗಿದ;

process_state:
  ಆಯ್ಕೆ(sx->state) {
  ಸ್ಥಿತಿ SOCKS_ST_INIT:
    sx->ಆವೃತ್ತಿ_2 = 5;
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_ಆರಂಭ);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_ಆರಂಭ:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->ಪೋರ್ಟ್);
    presult = socks5_req0_init(cf, sx, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, ದತ್ತಾಂಶ, 2, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    /* socks5_check_resp0() sets next socks state */
    ಹೋಗು process_state;

  ಸ್ಥಿತಿ SOCKS5_ST_GSSAPI_INIT: {
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode ಫಲಿತಾಂಶ = Curl_SOCKS5_gssapi_negotiate(cf, ದತ್ತಾಂಶ, sx->creds);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Unable to negotiate SOCKS5 GSS-API context.");
      ಹಿಂತಿರುಗಿ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ1_INIT);
    ಹೋಗು process_state;
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_GSSAPI_PERMSG);
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಸ್ಥಿತಿ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, ದತ್ತಾಂಶ, 2, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಆದರೆ(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ1_SEND);
      ಹೋಗು process_state;
    }
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ಆದರೆ(cf->conn->socks5_gssapi_enctype) {
      failf(ದತ್ತಾಂಶ, "SOCKS5 GSS-API protection not yet implemented.");
      ಹಿಂತಿರುಗಿ CURLPX_GSSAPI_PROTECTION;
    }
#ಆದರೆ_ಅಂತ್ಯ
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(presult)
      ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, presult);
    ಆದರೆ(!ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ CURLPX_OK;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, ದತ್ತಾಂಶ, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ಸ್ಥಿತಿ SOCKS_ST_SUCCESS:
    ಹಿಂತಿರುಗಿ CURLPX_OK;

  ಸ್ಥಿತಿ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ಹಿಂತಿರುಗಿ sx->presult;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ socks_failed(sx, cf, ದತ್ತಾಂಶ, CURLPX_SEND_REQUEST);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ socks_proxy_ctx_free(ರಚನೆ socks_ctx *ctx)
{
  ಆದರೆ(ctx) {
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
ಸ್ಥಿರ CURLcode socks_proxy_cf_connect(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಮುಗಿದ)
{
  ರಚನೆ socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆಯ್ಕೆ(ctx->proxy_type) {
  ಸ್ಥಿತಿ CURLPROXY_SOCKS5:
  ಸ್ಥಿತಿ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLPROXY_SOCKS4:
  ಸ್ಥಿತಿ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(pxresult) {
    ಫಲಿತಾಂಶ = CURLE_PROXY;
    ದತ್ತಾಂಶ->info.pxcode = (uint8_t)pxresult;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->state != SOCKS_ST_SUCCESS)
    ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    ರಚನೆ ip_quadruple ipquad;
    bool is_ipv6;
    ಆದರೆ(!Curl_conn_cf_get_ip_info(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &is_ipv6, &ipquad))
      infof(ದತ್ತಾಂಶ, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_ಪೋರ್ಟ್,
            ctx->dest->hostname, ctx->dest->ಪೋರ್ಟ್,
            ipquad.remote_ip, ipquad.remote_ಪೋರ್ಟ್);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#ಆದರೆ_ಅಂತ್ಯ
  cf->connected = TRUE;

ಹೊರಹೋಗು:
  *ಮುಗಿದ = (bool)cf->connected;
  ಆದರೆ(*ಮುಗಿದ || ಫಲಿತಾಂಶ)
    Curl_creds_unlink(&ctx->creds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode socks_cf_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ socks_ctx *sx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
    ಆಯ್ಕೆ(sx->state) {
    ಸ್ಥಿತಿ SOCKS4_ST_SEND:
    ಸ್ಥಿತಿ SOCKS5_ST_REQ0_SEND:
    ಸ್ಥಿತಿ SOCKS5_ST_AUTH_SEND:
    ಸ್ಥಿತಿ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust pollset out (%d)", (ಪೂರ್ಣಾಂಕ)sx->state);
      ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, sock);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust pollset in (%d)", (ಪೂರ್ಣಾಂಕ)sx->state);
      ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಒಳ_only(ದತ್ತಾಂಶ, ps, sock);
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ socks_proxy_cf_destroy(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ಖಾಲಿ;
}

ಸ್ಥಿರ CURLcode socks_cf_query(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ socks_ctx *sx = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_HOST_ಪೋರ್ಟ್:
    ಆದರೆ(sx) {
      *pres1 = sx->dest->ಪೋರ್ಟ್;
      *((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)pres2) = sx->dest->hostname;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ರಚನೆ Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_peer *dest,
                                          uint8_t ip_ಆವೃತ್ತಿ,
                                          uint8_t proxy_type,
                                          ರಚನೆ Curl_creds *creds)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(ದತ್ತಾಂಶ, ip_ಆವೃತ್ತಿ);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!dest)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಯ್ಕೆ(proxy_type) {
  ಸ್ಥಿತಿ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLPROXY_SOCKS5_HOSTNAME:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLPROXY_SOCKS4A:
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "unknown proxytype %d option given", proxy_type);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_ಆವೃತ್ತಿ = ip_ಆವೃತ್ತಿ;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_ಗಾತ್ರ, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
    ಆದರೆ(ctx->resolve_local) {
      ಫಲಿತಾಂಶ = Curl_conn_dns_ಕೂಡಿಸು_addr_resolve(ದತ್ತಾಂಶ, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    socks_proxy_ctx_free(ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */
