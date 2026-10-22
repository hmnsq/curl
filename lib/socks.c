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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "socks.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
എണ്ണൽ socks_state_t {
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

#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം cf_socks_statename[] = {
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

ഘടന socks_ctx {
  എണ്ണൽ socks_state_t state;
  ഘടന bufq iobuf;
  ഘടന Curl_peer *dest;
  ഘടന Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned അക്ഷരം version;
  BIT(resolve_local);
  BIT(socks4a);
};

#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            അക്ഷരം *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  വേണ്ടി(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    എങ്കിൽ(timeout_ms < 0) {
      /* we already got the timeout */
      തിരികെ CURLE_OPERATION_TIMEDOUT;
    }
    എങ്കിൽ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    എങ്കിൽ(SOCKET_READABLE(sock, timeout_ms) <= 0)
      തിരികെ CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    എങ്കിൽ(result == CURLE_AGAIN)
      തുടരുക;
    അല്ലെങ്കിൽ എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ(blen == nread) {
      *pnread += nread;
      തിരികെ CURLE_OK;
    }
    എങ്കിൽ(!nread) /* EOF */
      തിരികെ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#അല്ലെങ്കിൽ
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
സ്ഥിതം ശൂന്യം socksstate(ഘടന socks_ctx *sx,
                       ഘടന Curl_cfilter *cf,
                       ഘടന Curl_easy *data,
                       എണ്ണൽ socks_state_t state
#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , പൂർണ്ണസംഖ്യ lineno
#endif
)
{
  എണ്ണൽ socks_state_t oldstate = sx->state;

  എങ്കിൽ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    തിരികെ;

  sx->state = state;

#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
#endif
}

സ്ഥിതം CURLproxycode socks_failed(ഘടന socks_ctx *sx,
                                  ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  തിരികെ presult;
}

സ്ഥിതം CURLproxycode socks_flush(ഘടന socks_ctx *sx,
                                 ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  ആകുമ്പോൾ(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, ശൂന്യ, 0,
                               &nwritten);
    എങ്കിൽ(result == CURLE_AGAIN)
      തിരികെ CURLPX_OK;
    അല്ലെങ്കിൽ എങ്കിൽ(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      തിരികെ socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks_recv(ഘടന socks_ctx *sx,
                                ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  ആകുമ്പോൾ(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    എങ്കിൽ(result == CURLE_AGAIN)
      തിരികെ CURLPX_OK;
    അല്ലെങ്കിൽ എങ്കിൽ(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      തിരികെ CURLPX_RECV_CONNECT;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!nread) { /* EOF */
      എങ്കിൽ(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        തിരികെ CURLPX_RECV_CONNECT;
      }
      നിർത്തുക;
    }
  }
  *done = TRUE;
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks4_req_add_hd(ഘടന socks_ctx *sx,
                                       ഘടന Curl_easy *data)
{
  unsigned അക്ഷരം buf[4];
  size_t nwritten;
  CURLcode result;

  (ശൂന്യം)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned അക്ഷരം)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned അക്ഷരം)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  എങ്കിൽ(result || (nwritten != 4))
    തിരികെ CURLPX_SEND_REQUEST;
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks4_req_add_user(ഘടന socks_ctx *sx,
                                         ഘടന Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  എങ്കിൽ(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    എങ്കിൽ(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      തിരികെ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    എങ്കിൽ(result || (nwritten != (plen + 1)))
      തിരികെ CURLPX_SEND_REQUEST;
  }
  അല്ലെങ്കിൽ {
    /* empty username */
    unsigned അക്ഷരം b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    എങ്കിൽ(result || (nwritten != 1))
      തിരികെ CURLPX_SEND_REQUEST;
  }
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks4_resolving(ഘടന socks_ctx *sx,
                                      ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      bool *done)
{
  സ്ഥിരം ഘടന Curl_addrinfo *ai = ശൂന്യ;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (പൂർണ്ണസംഖ്യ)result, sx->dest->hostname, sx->dest->port);
      തിരികെ CURLPX_RESOLVE_HOST;
    }
    തിരികെ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  എങ്കിൽ(ai) {
    ഘടന sockaddr_in *saddr_in;
    അക്ഷരം ipbuf[64];

    Curl_printable_address(ai, ipbuf, വലുപ്പം(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (ഘടന sockaddr_in *)(ശൂന്യം *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned അക്ഷരം *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    എങ്കിൽ(result || (nwritten != 4))
      തിരികെ CURLPX_SEND_REQUEST;
  }
  അല്ലെങ്കിൽ {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    തിരികെ CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks4_check_resp(ഘടന socks_ctx *sx,
                                       ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data)
{
  സ്ഥിരം unsigned അക്ഷരം *resp;
  size_t rlen;

  എങ്കിൽ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    തിരികെ CURLPX_RECV_CONNECT;
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
  എങ്കിൽ(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    തിരികെ CURLPX_BAD_VERSION;
  }

  /* Result */
  തിരഞ്ഞെടുപ്പ്(resp[1]) {
  അവസ്ഥ 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    തിരികെ CURLPX_OK;
  അവസ്ഥ 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned പൂർണ്ണസംഖ്യ)((resp[2] << 8) | resp[3]), resp[1]);
    തിരികെ CURLPX_REQUEST_FAILED;
  അവസ്ഥ 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned പൂർണ്ണസംഖ്യ)((resp[2] << 8) | resp[3]), resp[1]);
    തിരികെ CURLPX_IDENTD;
  അവസ്ഥ 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned പൂർണ്ണസംഖ്യ)((resp[2] << 8) | resp[3]), resp[1]);
    തിരികെ CURLPX_IDENTD_DIFFER;
  സ്ഥിരസ്ഥിതി:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned പൂർണ്ണസംഖ്യ)((resp[2] << 8) | resp[3]), resp[1]);
    തിരികെ CURLPX_UNKNOWN_FAIL;
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
സ്ഥിതം CURLproxycode socks4_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന socks_ctx *sx,
                                    ഘടന Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  തിരഞ്ഞെടുപ്പ്(sx->state) {
  അവസ്ഥ SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  അവസ്ഥ SOCKS4_ST_START:
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
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    എങ്കിൽ(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned അക്ഷരം buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      എങ്കിൽ(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        തിരികെ socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      എങ്കിൽ(result || (nwritten != 4))
        തിരികെ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      എങ്കിൽ(presult)
        തിരികെ socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      എങ്കിൽ(result || (nwritten != hlen))
        തിരികെ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      പോകുക process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  അവസ്ഥ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  അവസ്ഥ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  അവസ്ഥ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  അവസ്ഥ SOCKS_ST_SUCCESS:
    തിരികെ CURLPX_OK;

  അവസ്ഥ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    തിരികെ sx->presult;

  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0);
    തിരികെ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

സ്ഥിതം CURLproxycode socks5_req0_init(ഘടന Curl_cfilter *cf,
                                      ഘടന socks_ctx *sx,
                                      ഘടന Curl_easy *data)
{
  സ്ഥിരം unsigned അക്ഷരം auth = data->set.socks5auth;
  unsigned അക്ഷരം req[5]; /* version + len + 3 possible auth methods */
  unsigned അക്ഷരം nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (ശൂന്യം)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  എങ്കിൽ(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    തിരികെ CURLPX_LONG_HOSTNAME;
  }

  എങ്കിൽ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  എങ്കിൽ(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  എങ്കിൽ(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  എങ്കിൽ(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  എങ്കിൽ(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  എങ്കിൽ(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  എങ്കിൽ(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    തിരികെ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  എങ്കിൽ(result || (nwritten != req_len))
    തിരികെ CURLPX_SEND_REQUEST;
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks5_check_resp0(ഘടന socks_ctx *sx,
                                        ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data)
{
  സ്ഥിരം unsigned അക്ഷരം *resp;
  unsigned അക്ഷരം auth_mode;
  size_t rlen;

  എങ്കിൽ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    തിരികെ CURLPX_RECV_CONNECT;
  }

  എങ്കിൽ(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    തിരികെ CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  തിരഞ്ഞെടുപ്പ്(auth_mode) {
  അവസ്ഥ 0:
    എങ്കിൽ(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      തിരികെ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    തിരികെ CURLPX_OK;
  അവസ്ഥ 1:
    എങ്കിൽ(data->set.socks5auth & CURLAUTH_GSSAPI) {
      എങ്കിൽ(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      തിരികെ CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    തിരികെ CURLPX_GSSAPI_PERMSG;
  അവസ്ഥ 2:
    /* regular name + password authentication */
    എങ്കിൽ(data->set.socks5auth & CURLAUTH_BASIC) {
      എങ്കിൽ(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      തിരികെ CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    തിരികെ CURLPX_NO_AUTH;
  അവസ്ഥ 255:
    failf(data, "No authentication method was acceptable.");
    തിരികെ CURLPX_NO_AUTH;
  സ്ഥിരസ്ഥിതി:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    തിരികെ CURLPX_UNKNOWN_MODE;
  }
}

സ്ഥിതം CURLproxycode socks5_auth_init(ഘടന Curl_cfilter *cf,
                                      ഘടന socks_ctx *sx,
                                      ഘടന Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned അക്ഷരം buf[2];
  CURLcode result;

  എങ്കിൽ(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    എങ്കിൽ(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      തിരികെ CURLPX_LONG_USER;
    }
    എങ്കിൽ(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      തിരികെ CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned അക്ഷരം)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  എങ്കിൽ(result || (nwritten != 2))
    തിരികെ CURLPX_SEND_REQUEST;
  എങ്കിൽ(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    എങ്കിൽ(result || (nwritten != ulen))
      തിരികെ CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned അക്ഷരം)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  എങ്കിൽ(result || (nwritten != 1))
    തിരികെ CURLPX_SEND_REQUEST;
  എങ്കിൽ(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    എങ്കിൽ(result || (nwritten != plen))
      തിരികെ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks5_check_auth_resp(ഘടന socks_ctx *sx,
                                            ഘടന Curl_cfilter *cf,
                                            ഘടന Curl_easy *data)
{
  സ്ഥിരം unsigned അക്ഷരം *resp;
  unsigned അക്ഷരം auth_status;
  size_t rlen;

  (ശൂന്യം)cf;
  എങ്കിൽ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    തിരികെ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  എങ്കിൽ(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    തിരികെ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks5_req1_init(ഘടന socks_ctx *sx,
                                      ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  unsigned അക്ഷരം req[5];
  unsigned അക്ഷരം ipbuf[16];
  സ്ഥിരം unsigned അക്ഷരം *destination;
  unsigned അക്ഷരം desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  എങ്കിൽ(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    എങ്കിൽ(result || (nwritten != 3))
      തിരികെ CURLPX_SEND_REQUEST;
    തിരികെ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  എങ്കിൽ(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    എങ്കിൽ(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      തിരികെ CURLPX_BAD_ADDRESS_TYPE;
  }
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (സ്ഥിരം unsigned അക്ഷരം *)sx->dest->hostname;
    destlen = (unsigned അക്ഷരം)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  എങ്കിൽ(result || (nwritten != hdlen))
    തിരികെ CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  എങ്കിൽ(result || (nwritten != destlen))
    തിരികെ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned അക്ഷരം)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned അക്ഷരം)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  എങ്കിൽ(result || (nwritten != 2))
    തിരികെ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  തിരികെ CURLPX_OK;
}

സ്ഥിതം CURLproxycode socks5_resolving(ഘടന socks_ctx *sx,
                                      ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      bool *done)
{
  സ്ഥിരം ഘടന Curl_addrinfo *ai = ശൂന്യ;
  അക്ഷരം dest[MAX_IPADR_LEN];  /* printable address */
  സ്ഥിരം unsigned അക്ഷരം *destination = ശൂന്യ;
  unsigned അക്ഷരം desttype = 1, destlen = 4;
  unsigned അക്ഷരം req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (പൂർണ്ണസംഖ്യ)result, sx->dest->hostname, sx->dest->port);
      തിരികെ CURLPX_RESOLVE_HOST;
    }
    തിരികെ CURLPX_OK;
  }

#ifdef USE_IPV6
  എങ്കിൽ(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  എങ്കിൽ(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  എങ്കിൽ(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    പോകുക out;
  }

  Curl_printable_address(ai, dest, വലുപ്പം(dest));

  എങ്കിൽ(ai->ai_family == AF_INET) {
    ഘടന sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (ഘടന sockaddr_in *)(ശൂന്യം *)ai->ai_addr;
    destination = (സ്ഥിരം unsigned അക്ഷരം *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  അല്ലെങ്കിൽ എങ്കിൽ(ai->ai_family == AF_INET6) {
    ഘടന sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ഘടന sockaddr_in6 *)(ശൂന്യം *)ai->ai_addr;
    destination = (സ്ഥിരം unsigned അക്ഷരം *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  എങ്കിൽ(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    പോകുക out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  എങ്കിൽ(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    പോകുക out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  എങ്കിൽ(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    പോകുക out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned അക്ഷരം)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned അക്ഷരം)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  എങ്കിൽ(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    പോകുക out;
  }

out:
  *done = (presult == CURLPX_OK);
  തിരികെ presult;
}

സ്ഥിതം CURLproxycode socks5_recv_resp1(ഘടന socks_ctx *sx,
                                       ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       bool *done)
{
  സ്ഥിരം unsigned അക്ഷരം *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  എങ്കിൽ(presult)
    തിരികെ presult;
  അല്ലെങ്കിൽ എങ്കിൽ(!*done)
    തിരികെ CURLPX_OK;

  എങ്കിൽ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    തിരികെ CURLPX_RECV_CONNECT;
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
  എങ്കിൽ(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    തിരികെ CURLPX_BAD_VERSION;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    പൂർണ്ണസംഖ്യ code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    എങ്കിൽ(code < 9) {
      /* RFC 1928 section 6 lists: */
      സ്ഥിതം സ്ഥിരം CURLproxycode lookup[] = {
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
    തിരികെ rc;
  }

  /* Calculate real packet size */
  തിരഞ്ഞെടുപ്പ്(resp[3]) {
  അവസ്ഥ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    നിർത്തുക;
  അവസ്ഥ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    നിർത്തുക;
  അവസ്ഥ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "SOCKS5 reply has wrong address type.");
    തിരികെ CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  എങ്കിൽ(presult)
    തിരികെ presult;
  അല്ലെങ്കിൽ എങ്കിൽ(!*done)
    തിരികെ CURLPX_OK;

  എങ്കിൽ(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    തിരികെ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  തിരികെ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
സ്ഥിതം CURLproxycode socks5_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന socks_ctx *sx,
                                    ഘടന Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  തിരഞ്ഞെടുപ്പ്(sx->state) {
  അവസ്ഥ SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    പോകുക process_state;

  അവസ്ഥ SOCKS5_ST_GSSAPI_INIT: {
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    എങ്കിൽ(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      തിരികെ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    പോകുക process_state;
#അല്ലെങ്കിൽ
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    തിരികെ socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  അവസ്ഥ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    എങ്കിൽ(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      പോകുക process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    അല്ലെങ്കിൽ എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    എങ്കിൽ(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      തിരികെ CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  അവസ്ഥ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    എങ്കിൽ(presult)
      തിരികെ socks_failed(sx, cf, data, presult);
    എങ്കിൽ(!done)
      തിരികെ CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  അവസ്ഥ SOCKS_ST_SUCCESS:
    തിരികെ CURLPX_OK;

  അവസ്ഥ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    തിരികെ sx->presult;

  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0);
    തിരികെ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

സ്ഥിതം ശൂന്യം socks_proxy_ctx_free(ഘടന socks_ctx *ctx)
{
  എങ്കിൽ(ctx) {
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
സ്ഥിതം CURLcode socks_proxy_cf_connect(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       bool *done)
{
  ഘടന socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  എങ്കിൽ(result || !*done)
    തിരികെ result;

  തിരഞ്ഞെടുപ്പ്(ctx->proxy_type) {
  അവസ്ഥ CURLPROXY_SOCKS5:
  അവസ്ഥ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    നിർത്തുക;

  അവസ്ഥ CURLPROXY_SOCKS4:
  അവസ്ഥ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    പോകുക out;
  }

  എങ്കിൽ(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->state != SOCKS_ST_SUCCESS)
    പോകുക out;

#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    ഘടന ip_quadruple ipquad;
    bool is_ipv6;
    എങ്കിൽ(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    അല്ലെങ്കിൽ
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  എങ്കിൽ(*done || result)
    Curl_creds_unlink(&ctx->creds);
  തിരികെ result;
}

സ്ഥിതം CURLcode socks_cf_adjust_pollset(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന easy_pollset *ps)
{
  ഘടന socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    തിരഞ്ഞെടുപ്പ്(sx->state) {
    അവസ്ഥ SOCKS4_ST_SEND:
    അവസ്ഥ SOCKS5_ST_REQ0_SEND:
    അവസ്ഥ SOCKS5_ST_AUTH_SEND:
    അവസ്ഥ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (പൂർണ്ണസംഖ്യ)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (പൂർണ്ണസംഖ്യ)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      നിർത്തുക;
    }
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം socks_proxy_cf_destroy(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  (ശൂന്യം)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ശൂന്യ;
}

സ്ഥിതം CURLcode socks_cf_query(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന socks_ctx *sx = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_HOST_PORT:
    എങ്കിൽ(sx) {
      *pres1 = sx->dest->port;
      *((സ്ഥിരം അക്ഷരം **)pres2) = sx->dest->hostname;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ഘടന Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                          ഘടന Curl_easy *data,
                                          ഘടന Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          ഘടന Curl_creds *creds)
{
  ഘടന Curl_cfilter *cf;
  ഘടന socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  എങ്കിൽ(!dest)
    തിരികെ CURLE_FAILED_INIT;

  തിരഞ്ഞെടുപ്പ്(proxy_type) {
  അവസ്ഥ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURLPROXY_SOCKS5_HOSTNAME:
    നിർത്തുക;
  അവസ്ഥ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    നിർത്തുക;
  അവസ്ഥ CURLPROXY_SOCKS4A:
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "unknown proxytype %d option given", proxy_type);
    തിരികെ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  എങ്കിൽ(!result) {
    Curl_conn_cf_തിരുകുക_after(cf_at, cf);
    എങ്കിൽ(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  അല്ലെങ്കിൽ
    socks_proxy_ctx_free(ctx);
  തിരികെ result;
}

#endif /* CURL_DISABLE_PROXY */
