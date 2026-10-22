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

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

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
ಗಣನಾ socks_state_t {
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

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ cf_socks_statename[] = {
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

ಸಂರಚನಾ socks_ctx {
  ಗಣನಾ socks_state_t state;
  ಸಂರಚನಾ bufq iobuf;
  ಸಂರಚನಾ Curl_peer *dest;
  ಸಂರಚನಾ Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned ವರ್ಣ version;
  BIT(resolve_local);
  BIT(socks4a);
};

#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ವರ್ಣ *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  ಪ್ರತಿ(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    ಯದಿ(timeout_ms < 0) {
      /* we already got the timeout */
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
    }
    ಯದಿ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ಯದಿ(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    ಯದಿ(result == CURLE_AGAIN)
      ಅನುವರ್ತ;
    ಅನ್ಯಥಾ ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ(blen == nread) {
      *pnread += nread;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಯದಿ(!nread) /* EOF */
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ಅನ್ಯಥಾ
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
ಸ್ಥಿರ ಶೂನ್ಯ socksstate(ಸಂರಚನಾ socks_ctx *sx,
                       ಸಂರಚನಾ Curl_cfilter *cf,
                       ಸಂರಚನಾ Curl_easy *data,
                       ಗಣನಾ socks_state_t state
#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , ಪೂರ್ಣಾಙ್ಕ lineno
#endif
)
{
  ಗಣನಾ socks_state_t oldstate = sx->state;

  ಯದಿ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ಪ್ರತ್ಯಾಗಮ;

  sx->state = state;

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
#endif
}

ಸ್ಥಿರ CURLproxycode socks_failed(ಸಂರಚನಾ socks_ctx *sx,
                                  ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  ಪ್ರತ್ಯಾಗಮ presult;
}

ಸ್ಥಿರ CURLproxycode socks_flush(ಸಂರಚನಾ socks_ctx *sx,
                                 ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  ಯಾವತ್(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, ರಿಕ್ತ, 0,
                               &nwritten);
    ಯದಿ(result == CURLE_AGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    ಅನ್ಯಥಾ ಯದಿ(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks_recv(ಸಂರಚನಾ socks_ctx *sx,
                                ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  ಯಾವತ್(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ಯದಿ(result == CURLE_AGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    ಅನ್ಯಥಾ ಯದಿ(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
    }
    ಅನ್ಯಥಾ ಯದಿ(!nread) { /* EOF */
      ಯದಿ(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
      }
      ಭಙ್ಗ;
    }
  }
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_req_add_hd(ಸಂರಚನಾ socks_ctx *sx,
                                       ಸಂರಚನಾ Curl_easy *data)
{
  unsigned ವರ್ಣ buf[4];
  size_t nwritten;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned ವರ್ಣ)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned ವರ್ಣ)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ಯದಿ(result || (nwritten != 4))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_req_add_user(ಸಂರಚನಾ socks_ctx *sx,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  ಯದಿ(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    ಯದಿ(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      ಪ್ರತ್ಯಾಗಮ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ಯದಿ(result || (nwritten != (plen + 1)))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  }
  ಅನ್ಯಥಾ {
    /* empty username */
    unsigned ವರ್ಣ b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ಯದಿ(result || (nwritten != 1))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  }
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_resolving(ಸಂರಚನಾ socks_ctx *sx,
                                      ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *done)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai = ರಿಕ್ತ;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (ಪೂರ್ಣಾಙ್ಕ)result, sx->dest->hostname, sx->dest->port);
      ಪ್ರತ್ಯಾಗಮ CURLPX_RESOLVE_HOST;
    }
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  ಯದಿ(ai) {
    ಸಂರಚನಾ sockaddr_in *saddr_in;
    ವರ್ಣ ipbuf[64];

    Curl_printable_address(ai, ipbuf, ಪರಿಮಾಣ(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (ಸಂರಚನಾ sockaddr_in *)(ಶೂನ್ಯ *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned ವರ್ಣ *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    ಯದಿ(result || (nwritten != 4))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  }
  ಅನ್ಯಥಾ {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ಪ್ರತ್ಯಾಗಮ CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks4_check_resp(ಸಂರಚನಾ socks_ctx *sx,
                                       ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ unsigned ವರ್ಣ *resp;
  size_t rlen;

  ಯದಿ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
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
  ಯದಿ(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_BAD_VERSION;
  }

  /* Result */
  ವಿಕಲ್ಪ(resp[1]) {
  ಪಕ್ಷ 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
  ಪಕ್ಷ 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ಪೂರ್ಣಾಙ್ಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಪ್ರತ್ಯಾಗಮ CURLPX_REQUEST_FAILED;
  ಪಕ್ಷ 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ಪೂರ್ಣಾಙ್ಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಪ್ರತ್ಯಾಗಮ CURLPX_IDENTD;
  ಪಕ್ಷ 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ಪೂರ್ಣಾಙ್ಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಪ್ರತ್ಯಾಗಮ CURLPX_IDENTD_DIFFER;
  ಸ್ವಭಾವ:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ಪೂರ್ಣಾಙ್ಕ)((resp[2] << 8) | resp[3]), resp[1]);
    ಪ್ರತ್ಯಾಗಮ CURLPX_UNKNOWN_FAIL;
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
ಸ್ಥಿರ CURLproxycode socks4_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ socks_ctx *sx,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  ವಿಕಲ್ಪ(sx->state) {
  ಪಕ್ಷ SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS4_ST_START:
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
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ಯದಿ(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned ವರ್ಣ buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      ಯದಿ(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ಯದಿ(result || (nwritten != 4))
        ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      ಯದಿ(presult)
        ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ಯದಿ(result || (nwritten != hlen))
        ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      ಗಚ್ಛ process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS_ST_SUCCESS:
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;

  ಪಕ್ಷ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ಪ್ರತ್ಯಾಗಮ sx->presult;

  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ಸ್ಥಿರ CURLproxycode socks5_req0_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ socks_ctx *sx,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ unsigned ವರ್ಣ auth = data->set.socks5auth;
  unsigned ವರ್ಣ req[5]; /* version + len + 3 possible auth methods */
  unsigned ವರ್ಣ nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (ಶೂನ್ಯ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ಯದಿ(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_LONG_HOSTNAME;
  }

  ಯದಿ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ಯದಿ(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ಯದಿ(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  ಯದಿ(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಯದಿ(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  ಯದಿ(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ಯದಿ(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ಯದಿ(result || (nwritten != req_len))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_check_resp0(ಸಂರಚನಾ socks_ctx *sx,
                                        ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ unsigned ವರ್ಣ *resp;
  unsigned ವರ್ಣ auth_mode;
  size_t rlen;

  ಯದಿ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
  }

  ಯದಿ(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  ವಿಕಲ್ಪ(auth_mode) {
  ಪಕ್ಷ 0:
    ಯದಿ(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      ಪ್ರತ್ಯಾಗಮ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
  ಪಕ್ಷ 1:
    ಯದಿ(data->set.socks5auth & CURLAUTH_GSSAPI) {
      ಯದಿ(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_GSSAPI_PERMSG;
  ಪಕ್ಷ 2:
    /* regular name + password authentication */
    ಯದಿ(data->set.socks5auth & CURLAUTH_BASIC) {
      ಯದಿ(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_NO_AUTH;
  ಪಕ್ಷ 255:
    failf(data, "No authentication method was acceptable.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_NO_AUTH;
  ಸ್ವಭಾವ:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_UNKNOWN_MODE;
  }
}

ಸ್ಥಿರ CURLproxycode socks5_auth_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ socks_ctx *sx,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned ವರ್ಣ buf[2];
  CURLcode result;

  ಯದಿ(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ಯದಿ(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      ಪ್ರತ್ಯಾಗಮ CURLPX_LONG_USER;
    }
    ಯದಿ(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      ಪ್ರತ್ಯಾಗಮ CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned ವರ್ಣ)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ಯದಿ(result || (nwritten != 2))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  ಯದಿ(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ಯದಿ(result || (nwritten != ulen))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned ವರ್ಣ)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ಯದಿ(result || (nwritten != 1))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  ಯದಿ(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ಯದಿ(result || (nwritten != plen))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_check_auth_resp(ಸಂರಚನಾ socks_ctx *sx,
                                            ಸಂರಚನಾ Curl_cfilter *cf,
                                            ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ unsigned ವರ್ಣ *resp;
  unsigned ವರ್ಣ auth_status;
  size_t rlen;

  (ಶೂನ್ಯ)cf;
  ಯದಿ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  ಯದಿ(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ಪ್ರತ್ಯಾಗಮ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_req1_init(ಸಂರಚನಾ socks_ctx *sx,
                                      ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  unsigned ವರ್ಣ req[5];
  unsigned ವರ್ಣ ipbuf[16];
  ಧ್ರುವ unsigned ವರ್ಣ *destination;
  unsigned ವರ್ಣ desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ಯದಿ(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ಯದಿ(result || (nwritten != 3))
      ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  ಯದಿ(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ಯದಿ(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ಪ್ರತ್ಯಾಗಮ CURLPX_BAD_ADDRESS_TYPE;
  }
  ಅನ್ಯಥಾ
#endif
  ಯದಿ(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ಧ್ರುವ unsigned ವರ್ಣ *)sx->dest->hostname;
    destlen = (unsigned ವರ್ಣ)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ಯದಿ(result || (nwritten != hdlen))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ಯದಿ(result || (nwritten != destlen))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned ವರ್ಣ)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned ವರ್ಣ)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ಯದಿ(result || (nwritten != 2))
    ಪ್ರತ್ಯಾಗಮ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

ಸ್ಥಿರ CURLproxycode socks5_resolving(ಸಂರಚನಾ socks_ctx *sx,
                                      ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *done)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai = ರಿಕ್ತ;
  ವರ್ಣ dest[MAX_IPADR_LEN];  /* printable address */
  ಧ್ರುವ unsigned ವರ್ಣ *destination = ರಿಕ್ತ;
  unsigned ವರ್ಣ desttype = 1, destlen = 4;
  unsigned ವರ್ಣ req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (ಪೂರ್ಣಾಙ್ಕ)result, sx->dest->hostname, sx->dest->port);
      ಪ್ರತ್ಯಾಗಮ CURLPX_RESOLVE_HOST;
    }
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
  }

#ifdef USE_IPV6
  ಯದಿ(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  ಯದಿ(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  ಯದಿ(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    ಗಚ್ಛ out;
  }

  Curl_printable_address(ai, dest, ಪರಿಮಾಣ(dest));

  ಯದಿ(ai->ai_family == AF_INET) {
    ಸಂರಚನಾ sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (ಸಂರಚನಾ sockaddr_in *)(ಶೂನ್ಯ *)ai->ai_addr;
    destination = (ಧ್ರುವ unsigned ವರ್ಣ *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  ಅನ್ಯಥಾ ಯದಿ(ai->ai_family == AF_INET6) {
    ಸಂರಚನಾ sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ಸಂರಚನಾ sockaddr_in6 *)(ಶೂನ್ಯ *)ai->ai_addr;
    destination = (ಧ್ರುವ unsigned ವರ್ಣ *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  ಯದಿ(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    ಗಚ್ಛ out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ಯದಿ(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    ಗಚ್ಛ out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ಯದಿ(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    ಗಚ್ಛ out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned ವರ್ಣ)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned ವರ್ಣ)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ಯದಿ(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    ಗಚ್ಛ out;
  }

out:
  *done = (presult == CURLPX_OK);
  ಪ್ರತ್ಯಾಗಮ presult;
}

ಸ್ಥಿರ CURLproxycode socks5_recv_resp1(ಸಂರಚನಾ socks_ctx *sx,
                                       ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       bool *done)
{
  ಧ್ರುವ unsigned ವರ್ಣ *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  ಯದಿ(presult)
    ಪ್ರತ್ಯಾಗಮ presult;
  ಅನ್ಯಥಾ ಯದಿ(!*done)
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;

  ಯದಿ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
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
  ಯದಿ(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_BAD_VERSION;
  }
  ಅನ್ಯಥಾ ಯದಿ(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    ಪೂರ್ಣಾಙ್ಕ code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    ಯದಿ(code < 9) {
      /* RFC 1928 section 6 lists: */
      ಸ್ಥಿರ ಧ್ರುವ CURLproxycode lookup[] = {
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
    ಪ್ರತ್ಯಾಗಮ rc;
  }

  /* Calculate real packet size */
  ವಿಕಲ್ಪ(resp[3]) {
  ಪಕ್ಷ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ಭಙ್ಗ;
  ಪಕ್ಷ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ಭಙ್ಗ;
  ಪಕ್ಷ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "SOCKS5 reply has wrong address type.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  ಯದಿ(presult)
    ಪ್ರತ್ಯಾಗಮ presult;
  ಅನ್ಯಥಾ ಯದಿ(!*done)
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;

  ಯದಿ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ಪ್ರತ್ಯಾಗಮ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ಸ್ಥಿರ CURLproxycode socks5_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ socks_ctx *sx,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  ವಿಕಲ್ಪ(sx->state) {
  ಪಕ್ಷ SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    ಗಚ್ಛ process_state;

  ಪಕ್ಷ SOCKS5_ST_GSSAPI_INIT: {
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    ಯದಿ(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      ಪ್ರತ್ಯಾಗಮ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    ಗಚ್ಛ process_state;
#ಅನ್ಯಥಾ
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  ಪಕ್ಷ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಯದಿ(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      ಗಚ್ಛ process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಅನ್ಯಥಾ ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ಯದಿ(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      ಪ್ರತ್ಯಾಗಮ CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    ಯದಿ(presult)
      ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, presult);
    ಯದಿ(!done)
      ಪ್ರತ್ಯಾಗಮ CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ಪಕ್ಷ SOCKS_ST_SUCCESS:
    ಪ್ರತ್ಯಾಗಮ CURLPX_OK;

  ಪಕ್ಷ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ಪ್ರತ್ಯಾಗಮ sx->presult;

  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ socks_proxy_ctx_free(ಸಂರಚನಾ socks_ctx *ctx)
{
  ಯದಿ(ctx) {
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
ಸ್ಥಿರ CURLcode socks_proxy_cf_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       bool *done)
{
  ಸಂರಚನಾ socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ಯದಿ(result || !*done)
    ಪ್ರತ್ಯಾಗಮ result;

  ವಿಕಲ್ಪ(ctx->proxy_type) {
  ಪಕ್ಷ CURLPROXY_SOCKS5:
  ಪಕ್ಷ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLPROXY_SOCKS4:
  ಪಕ್ಷ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    ಗಚ್ಛ out;
  }

  ಯದಿ(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(ctx->state != SOCKS_ST_SUCCESS)
    ಗಚ್ಛ out;

#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_is_verbose(data)) {
    ಸಂರಚನಾ ip_quadruple ipquad;
    bool is_ipv6;
    ಯದಿ(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    ಅನ್ಯಥಾ
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  ಯದಿ(*done || result)
    Curl_creds_unlink(&ctx->creds);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode socks_cf_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ವಿಕಲ್ಪ(sx->state) {
    ಪಕ್ಷ SOCKS4_ST_SEND:
    ಪಕ್ಷ SOCKS5_ST_REQ0_SEND:
    ಪಕ್ಷ SOCKS5_ST_AUTH_SEND:
    ಪಕ್ಷ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (ಪೂರ್ಣಾಙ್ಕ)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (ಪೂರ್ಣಾಙ್ಕ)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ socks_proxy_cf_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ರಿಕ್ತ;
}

ಸ್ಥಿರ CURLcode socks_cf_query(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ socks_ctx *sx = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_HOST_PORT:
    ಯದಿ(sx) {
      *pres1 = sx->dest->port;
      *((ಧ್ರುವ ವರ್ಣ **)pres2) = sx->dest->hostname;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          ಸಂರಚನಾ Curl_creds *creds)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  ಯದಿ(!dest)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ವಿಕಲ್ಪ(proxy_type) {
  ಪಕ್ಷ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLPROXY_SOCKS5_HOSTNAME:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLPROXY_SOCKS4A:
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "unknown proxytype %d option given", proxy_type);
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ಯದಿ(!result) {
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
    ಯದಿ(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ಅನ್ಯಥಾ
    socks_proxy_ctx_free(ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* CURL_DISABLE_PROXY */
