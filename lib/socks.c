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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "socks.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
எண்ணிக்கை socks_state_t {
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

#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
நிலையான மாறிலி எழுத்து * மாறிலி cf_socks_statename[] = {
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

கட்டமைப்பு socks_ctx {
  எண்ணிக்கை socks_state_t state;
  கட்டமைப்பு bufq iobuf;
  கட்டமைப்பு Curl_peer *dest;
  கட்டமைப்பு Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned எழுத்து version;
  BIT(resolve_local);
  BIT(socks4a);
};

#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            எழுத்து *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  க்கு(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    எனில்(timeout_ms < 0) {
      /* we already got the timeout */
      திரும்பு CURLE_OPERATION_TIMEDOUT;
    }
    எனில்(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    எனில்(SOCKET_READABLE(sock, timeout_ms) <= 0)
      திரும்பு CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    எனில்(result == CURLE_AGAIN)
      தொடர்;
    இல்லையெனில் எனில்(result)
      திரும்பு result;

    எனில்(blen == nread) {
      *pnread += nread;
      திரும்பு CURLE_OK;
    }
    எனில்(!nread) /* EOF */
      திரும்பு CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#இல்லையெனில்
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
நிலையான வெறுமை socksstate(கட்டமைப்பு socks_ctx *sx,
                       கட்டமைப்பு Curl_cfilter *cf,
                       கட்டமைப்பு Curl_easy *data,
                       எண்ணிக்கை socks_state_t state
#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , முழு lineno
#endif
)
{
  எண்ணிக்கை socks_state_t oldstate = sx->state;

  எனில்(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    திரும்பு;

  sx->state = state;

#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#இல்லையெனில்
  (வெறுமை)cf;
  (வெறுமை)data;
#endif
}

நிலையான CURLproxycode socks_failed(கட்டமைப்பு socks_ctx *sx,
                                  கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  திரும்பு presult;
}

நிலையான CURLproxycode socks_flush(கட்டமைப்பு socks_ctx *sx,
                                 கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  போது(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, வெற்று, 0,
                               &nwritten);
    எனில்(result == CURLE_AGAIN)
      திரும்பு CURLPX_OK;
    இல்லையெனில் எனில்(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      திரும்பு socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks_recv(கட்டமைப்பு socks_ctx *sx,
                                கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  போது(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    எனில்(result == CURLE_AGAIN)
      திரும்பு CURLPX_OK;
    இல்லையெனில் எனில்(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      திரும்பு CURLPX_RECV_CONNECT;
    }
    இல்லையெனில் எனில்(!nread) { /* EOF */
      எனில்(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        திரும்பு CURLPX_RECV_CONNECT;
      }
      நிறுத்து;
    }
  }
  *done = TRUE;
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks4_req_add_hd(கட்டமைப்பு socks_ctx *sx,
                                       கட்டமைப்பு Curl_easy *data)
{
  unsigned எழுத்து buf[4];
  size_t nwritten;
  CURLcode result;

  (வெறுமை)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned எழுத்து)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned எழுத்து)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  எனில்(result || (nwritten != 4))
    திரும்பு CURLPX_SEND_REQUEST;
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks4_req_add_user(கட்டமைப்பு socks_ctx *sx,
                                         கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  எனில்(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    எனில்(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      திரும்பு CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    எனில்(result || (nwritten != (plen + 1)))
      திரும்பு CURLPX_SEND_REQUEST;
  }
  இல்லையெனில் {
    /* empty username */
    unsigned எழுத்து b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    எனில்(result || (nwritten != 1))
      திரும்பு CURLPX_SEND_REQUEST;
  }
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks4_resolving(கட்டமைப்பு socks_ctx *sx,
                                      கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *done)
{
  மாறிலி கட்டமைப்பு Curl_addrinfo *ai = வெற்று;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  எனில்(result) {
    எனில்(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (முழு)result, sx->dest->hostname, sx->dest->port);
      திரும்பு CURLPX_RESOLVE_HOST;
    }
    திரும்பு CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  எனில்(ai) {
    கட்டமைப்பு sockaddr_in *saddr_in;
    எழுத்து ipbuf[64];

    Curl_printable_address(ai, ipbuf, அளவு(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (கட்டமைப்பு sockaddr_in *)(வெறுமை *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned எழுத்து *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    எனில்(result || (nwritten != 4))
      திரும்பு CURLPX_SEND_REQUEST;
  }
  இல்லையெனில் {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    திரும்பு CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks4_check_resp(கட்டமைப்பு socks_ctx *sx,
                                       கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data)
{
  மாறிலி unsigned எழுத்து *resp;
  size_t rlen;

  எனில்(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    திரும்பு CURLPX_RECV_CONNECT;
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
  எனில்(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    திரும்பு CURLPX_BAD_VERSION;
  }

  /* Result */
  தேர்வு(resp[1]) {
  நிலை 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    திரும்பு CURLPX_OK;
  நிலை 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned முழு)((resp[2] << 8) | resp[3]), resp[1]);
    திரும்பு CURLPX_REQUEST_FAILED;
  நிலை 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned முழு)((resp[2] << 8) | resp[3]), resp[1]);
    திரும்பு CURLPX_IDENTD;
  நிலை 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned முழு)((resp[2] << 8) | resp[3]), resp[1]);
    திரும்பு CURLPX_IDENTD_DIFFER;
  இயல்பு:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned முழு)((resp[2] << 8) | resp[3]), resp[1]);
    திரும்பு CURLPX_UNKNOWN_FAIL;
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
நிலையான CURLproxycode socks4_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு socks_ctx *sx,
                                    கட்டமைப்பு Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  தேர்வு(sx->state) {
  நிலை SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  நிலை SOCKS4_ST_START:
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
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    எனில்(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned எழுத்து buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      எனில்(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        திரும்பு socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      எனில்(result || (nwritten != 4))
        திரும்பு socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      எனில்(presult)
        திரும்பு socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      எனில்(result || (nwritten != hlen))
        திரும்பு socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      செல் process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  நிலை SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    எனில்(!done)
      திரும்பு CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  நிலை SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  நிலை SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  நிலை SOCKS_ST_SUCCESS:
    திரும்பு CURLPX_OK;

  நிலை SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    திரும்பு sx->presult;

  இயல்பு:
    DEBUGASSERT(0);
    திரும்பு socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

நிலையான CURLproxycode socks5_req0_init(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு socks_ctx *sx,
                                      கட்டமைப்பு Curl_easy *data)
{
  மாறிலி unsigned எழுத்து auth = data->set.socks5auth;
  unsigned எழுத்து req[5]; /* version + len + 3 possible auth methods */
  unsigned எழுத்து nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (வெறுமை)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  எனில்(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    திரும்பு CURLPX_LONG_HOSTNAME;
  }

  எனில்(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  எனில்(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  எனில்(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  எனில்(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  எனில்(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  எனில்(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  எனில்(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    திரும்பு CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  எனில்(result || (nwritten != req_len))
    திரும்பு CURLPX_SEND_REQUEST;
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks5_check_resp0(கட்டமைப்பு socks_ctx *sx,
                                        கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data)
{
  மாறிலி unsigned எழுத்து *resp;
  unsigned எழுத்து auth_mode;
  size_t rlen;

  எனில்(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    திரும்பு CURLPX_RECV_CONNECT;
  }

  எனில்(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    திரும்பு CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  தேர்வு(auth_mode) {
  நிலை 0:
    எனில்(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      திரும்பு CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    திரும்பு CURLPX_OK;
  நிலை 1:
    எனில்(data->set.socks5auth & CURLAUTH_GSSAPI) {
      எனில்(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      திரும்பு CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    திரும்பு CURLPX_GSSAPI_PERMSG;
  நிலை 2:
    /* regular name + password authentication */
    எனில்(data->set.socks5auth & CURLAUTH_BASIC) {
      எனில்(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      திரும்பு CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    திரும்பு CURLPX_NO_AUTH;
  நிலை 255:
    failf(data, "No authentication method was acceptable.");
    திரும்பு CURLPX_NO_AUTH;
  இயல்பு:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    திரும்பு CURLPX_UNKNOWN_MODE;
  }
}

நிலையான CURLproxycode socks5_auth_init(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு socks_ctx *sx,
                                      கட்டமைப்பு Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned எழுத்து buf[2];
  CURLcode result;

  எனில்(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    எனில்(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      திரும்பு CURLPX_LONG_USER;
    }
    எனில்(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      திரும்பு CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned எழுத்து)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  எனில்(result || (nwritten != 2))
    திரும்பு CURLPX_SEND_REQUEST;
  எனில்(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    எனில்(result || (nwritten != ulen))
      திரும்பு CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned எழுத்து)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  எனில்(result || (nwritten != 1))
    திரும்பு CURLPX_SEND_REQUEST;
  எனில்(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    எனில்(result || (nwritten != plen))
      திரும்பு CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks5_check_auth_resp(கட்டமைப்பு socks_ctx *sx,
                                            கட்டமைப்பு Curl_cfilter *cf,
                                            கட்டமைப்பு Curl_easy *data)
{
  மாறிலி unsigned எழுத்து *resp;
  unsigned எழுத்து auth_status;
  size_t rlen;

  (வெறுமை)cf;
  எனில்(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    திரும்பு CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  எனில்(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    திரும்பு CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks5_req1_init(கட்டமைப்பு socks_ctx *sx,
                                      கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  unsigned எழுத்து req[5];
  unsigned எழுத்து ipbuf[16];
  மாறிலி unsigned எழுத்து *destination;
  unsigned எழுத்து desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  எனில்(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    எனில்(result || (nwritten != 3))
      திரும்பு CURLPX_SEND_REQUEST;
    திரும்பு CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  எனில்(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    எனில்(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      திரும்பு CURLPX_BAD_ADDRESS_TYPE;
  }
  இல்லையெனில்
#endif
  எனில்(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  இல்லையெனில் {
    மாறிலி size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (மாறிலி unsigned எழுத்து *)sx->dest->hostname;
    destlen = (unsigned எழுத்து)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  எனில்(result || (nwritten != hdlen))
    திரும்பு CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  எனில்(result || (nwritten != destlen))
    திரும்பு CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned எழுத்து)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned எழுத்து)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  எனில்(result || (nwritten != 2))
    திரும்பு CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  திரும்பு CURLPX_OK;
}

நிலையான CURLproxycode socks5_resolving(கட்டமைப்பு socks_ctx *sx,
                                      கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *done)
{
  மாறிலி கட்டமைப்பு Curl_addrinfo *ai = வெற்று;
  எழுத்து dest[MAX_IPADR_LEN];  /* printable address */
  மாறிலி unsigned எழுத்து *destination = வெற்று;
  unsigned எழுத்து desttype = 1, destlen = 4;
  unsigned எழுத்து req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  எனில்(result) {
    எனில்(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (முழு)result, sx->dest->hostname, sx->dest->port);
      திரும்பு CURLPX_RESOLVE_HOST;
    }
    திரும்பு CURLPX_OK;
  }

#ifdef USE_IPV6
  எனில்(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  எனில்(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  எனில்(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    செல் out;
  }

  Curl_printable_address(ai, dest, அளவு(dest));

  எனில்(ai->ai_family == AF_INET) {
    கட்டமைப்பு sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (கட்டமைப்பு sockaddr_in *)(வெறுமை *)ai->ai_addr;
    destination = (மாறிலி unsigned எழுத்து *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  இல்லையெனில் எனில்(ai->ai_family == AF_INET6) {
    கட்டமைப்பு sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (கட்டமைப்பு sockaddr_in6 *)(வெறுமை *)ai->ai_addr;
    destination = (மாறிலி unsigned எழுத்து *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  எனில்(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    செல் out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  எனில்(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    செல் out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  எனில்(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    செல் out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned எழுத்து)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned எழுத்து)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  எனில்(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    செல் out;
  }

out:
  *done = (presult == CURLPX_OK);
  திரும்பு presult;
}

நிலையான CURLproxycode socks5_recv_resp1(கட்டமைப்பு socks_ctx *sx,
                                       கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       bool *done)
{
  மாறிலி unsigned எழுத்து *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  எனில்(presult)
    திரும்பு presult;
  இல்லையெனில் எனில்(!*done)
    திரும்பு CURLPX_OK;

  எனில்(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    திரும்பு CURLPX_RECV_CONNECT;
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
  எனில்(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    திரும்பு CURLPX_BAD_VERSION;
  }
  இல்லையெனில் எனில்(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    முழு code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    எனில்(code < 9) {
      /* RFC 1928 section 6 lists: */
      நிலையான மாறிலி CURLproxycode lookup[] = {
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
    திரும்பு rc;
  }

  /* Calculate real packet size */
  தேர்வு(resp[3]) {
  நிலை 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    நிறுத்து;
  நிலை 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    நிறுத்து;
  நிலை 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    நிறுத்து;
  இயல்பு:
    failf(data, "SOCKS5 reply has wrong address type.");
    திரும்பு CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  எனில்(presult)
    திரும்பு presult;
  இல்லையெனில் எனில்(!*done)
    திரும்பு CURLPX_OK;

  எனில்(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    திரும்பு CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  திரும்பு CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
நிலையான CURLproxycode socks5_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு socks_ctx *sx,
                                    கட்டமைப்பு Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  தேர்வு(sx->state) {
  நிலை SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  நிலை SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  நிலை SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  நிலை SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    செல் process_state;

  நிலை SOCKS5_ST_GSSAPI_INIT: {
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    எனில்(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      திரும்பு CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    செல் process_state;
#இல்லையெனில்
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    திரும்பு socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  நிலை SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  நிலை SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  நிலை SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  நிலை SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    எனில்(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      செல் process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  நிலை SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    எனில்(!done)
      திரும்பு CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  நிலை SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    இல்லையெனில் எனில்(!done)
      திரும்பு CURLPX_OK;
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    எனில்(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      திரும்பு CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  நிலை SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    எனில்(presult)
      திரும்பு socks_failed(sx, cf, data, presult);
    எனில்(!done)
      திரும்பு CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  நிலை SOCKS_ST_SUCCESS:
    திரும்பு CURLPX_OK;

  நிலை SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    திரும்பு sx->presult;

  இயல்பு:
    DEBUGASSERT(0);
    திரும்பு socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

நிலையான வெறுமை socks_proxy_ctx_free(கட்டமைப்பு socks_ctx *ctx)
{
  எனில்(ctx) {
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
நிலையான CURLcode socks_proxy_cf_connect(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       bool *done)
{
  கட்டமைப்பு socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  எனில்(result || !*done)
    திரும்பு result;

  தேர்வு(ctx->proxy_type) {
  நிலை CURLPROXY_SOCKS5:
  நிலை CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    நிறுத்து;

  நிலை CURLPROXY_SOCKS4:
  நிலை CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    நிறுத்து;

  இயல்பு:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    செல் out;
  }

  எனில்(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    செல் out;
  }
  இல்லையெனில் எனில்(ctx->state != SOCKS_ST_SUCCESS)
    செல் out;

#ifdef CURLVERBOSE
  எனில்(Curl_trc_is_verbose(data)) {
    கட்டமைப்பு ip_quadruple ipquad;
    bool is_ipv6;
    எனில்(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    இல்லையெனில்
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  எனில்(*done || result)
    Curl_creds_unlink(&ctx->creds);
  திரும்பு result;
}

நிலையான CURLcode socks_cf_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    தேர்வு(sx->state) {
    நிலை SOCKS4_ST_SEND:
    நிலை SOCKS5_ST_REQ0_SEND:
    நிலை SOCKS5_ST_AUTH_SEND:
    நிலை SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (முழு)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      நிறுத்து;
    இயல்பு:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (முழு)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      நிறுத்து;
    }
  }
  திரும்பு result;
}

நிலையான வெறுமை socks_proxy_cf_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = வெற்று;
}

நிலையான CURLcode socks_cf_query(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு socks_ctx *sx = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_HOST_PORT:
    எனில்(sx) {
      *pres1 = sx->dest->port;
      *((மாறிலி எழுத்து **)pres2) = sx->dest->hostname;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

கட்டமைப்பு Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                          கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          கட்டமைப்பு Curl_creds *creds)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  எனில்(!dest)
    திரும்பு CURLE_FAILED_INIT;

  தேர்வு(proxy_type) {
  நிலை CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    நிறுத்து;
  நிலை CURLPROXY_SOCKS5_HOSTNAME:
    நிறுத்து;
  நிலை CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    நிறுத்து;
  நிலை CURLPROXY_SOCKS4A:
    நிறுத்து;
  இயல்பு:
    failf(data, "unknown proxytype %d option given", proxy_type);
    திரும்பு CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  எனில்(!result) {
    Curl_conn_cf_செருகு_after(cf_at, cf);
    எனில்(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  இல்லையெனில்
    socks_proxy_ctx_free(ctx);
  திரும்பு result;
}

#endif /* CURL_DISABLE_PROXY */
