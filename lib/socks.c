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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "socks.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
গণনা socks_state_t {
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

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
স্থির ধ্রুব বর্ণ * ধ্রুব cf_socks_statename[] = {
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

সংরচনা socks_ctx {
  গণনা socks_state_t state;
  সংরচনা bufq iobuf;
  সংরচনা Curl_peer *dest;
  সংরচনা Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned বর্ণ version;
  BIT(resolve_local);
  BIT(socks4a);
};

#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            বর্ণ *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  প্রতি(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    যদি(timeout_ms < 0) {
      /* we already got the timeout */
      প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
    }
    যদি(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    যদি(SOCKET_READABLE(sock, timeout_ms) <= 0)
      প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    যদি(result == CURLE_AGAIN)
      অনুবর্ত;
    অন্যথা যদি(result)
      প্রত্যাগম result;

    যদি(blen == nread) {
      *pnread += nread;
      প্রত্যাগম CURLE_OK;
    }
    যদি(!nread) /* EOF */
      প্রত্যাগম CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#অন্যথা
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
স্থির শূন্য socksstate(সংরচনা socks_ctx *sx,
                       সংরচনা Curl_cfilter *cf,
                       সংরচনা Curl_easy *data,
                       গণনা socks_state_t state
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , পূর্ণাঙ্ক lineno
#endif
)
{
  গণনা socks_state_t oldstate = sx->state;

  যদি(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    প্রত্যাগম;

  sx->state = state;

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#অন্যথা
  (শূন্য)cf;
  (শূন্য)data;
#endif
}

স্থির CURLproxycode socks_failed(সংরচনা socks_ctx *sx,
                                  সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  প্রত্যাগম presult;
}

স্থির CURLproxycode socks_flush(সংরচনা socks_ctx *sx,
                                 সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  যাবত্(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, রিক্ত, 0,
                               &nwritten);
    যদি(result == CURLE_AGAIN)
      প্রত্যাগম CURLPX_OK;
    অন্যথা যদি(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      প্রত্যাগম socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks_recv(সংরচনা socks_ctx *sx,
                                সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  যাবত্(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    যদি(result == CURLE_AGAIN)
      প্রত্যাগম CURLPX_OK;
    অন্যথা যদি(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      প্রত্যাগম CURLPX_RECV_CONNECT;
    }
    অন্যথা যদি(!nread) { /* EOF */
      যদি(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        প্রত্যাগম CURLPX_RECV_CONNECT;
      }
      ভঙ্গ;
    }
  }
  *done = TRUE;
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks4_req_add_hd(সংরচনা socks_ctx *sx,
                                       সংরচনা Curl_easy *data)
{
  unsigned বর্ণ buf[4];
  size_t nwritten;
  CURLcode result;

  (শূন্য)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned বর্ণ)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned বর্ণ)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  যদি(result || (nwritten != 4))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks4_req_add_user(সংরচনা socks_ctx *sx,
                                         সংরচনা Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  যদি(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    যদি(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      প্রত্যাগম CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    যদি(result || (nwritten != (plen + 1)))
      প্রত্যাগম CURLPX_SEND_REQUEST;
  }
  অন্যথা {
    /* empty username */
    unsigned বর্ণ b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    যদি(result || (nwritten != 1))
      প্রত্যাগম CURLPX_SEND_REQUEST;
  }
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks4_resolving(সংরচনা socks_ctx *sx,
                                      সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      bool *done)
{
  ধ্রুব সংরচনা Curl_addrinfo *ai = রিক্ত;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  যদি(result) {
    যদি(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (পূর্ণাঙ্ক)result, sx->dest->hostname, sx->dest->port);
      প্রত্যাগম CURLPX_RESOLVE_HOST;
    }
    প্রত্যাগম CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  যদি(ai) {
    সংরচনা sockaddr_in *saddr_in;
    বর্ণ ipbuf[64];

    Curl_printable_address(ai, ipbuf, পরিমাণ(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (সংরচনা sockaddr_in *)(শূন্য *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned বর্ণ *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    যদি(result || (nwritten != 4))
      প্রত্যাগম CURLPX_SEND_REQUEST;
  }
  অন্যথা {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    প্রত্যাগম CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks4_check_resp(সংরচনা socks_ctx *sx,
                                       সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data)
{
  ধ্রুব unsigned বর্ণ *resp;
  size_t rlen;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    প্রত্যাগম CURLPX_RECV_CONNECT;
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
  যদি(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    প্রত্যাগম CURLPX_BAD_VERSION;
  }

  /* Result */
  বিকল্প(resp[1]) {
  পক্ষ 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    প্রত্যাগম CURLPX_OK;
  পক্ষ 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণাঙ্ক)((resp[2] << 8) | resp[3]), resp[1]);
    প্রত্যাগম CURLPX_REQUEST_FAILED;
  পক্ষ 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণাঙ্ক)((resp[2] << 8) | resp[3]), resp[1]);
    প্রত্যাগম CURLPX_IDENTD;
  পক্ষ 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণাঙ্ক)((resp[2] << 8) | resp[3]), resp[1]);
    প্রত্যাগম CURLPX_IDENTD_DIFFER;
  স্বভাব:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণাঙ্ক)((resp[2] << 8) | resp[3]), resp[1]);
    প্রত্যাগম CURLPX_UNKNOWN_FAIL;
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
স্থির CURLproxycode socks4_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা socks_ctx *sx,
                                    সংরচনা Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  বিকল্প(sx->state) {
  পক্ষ SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  পক্ষ SOCKS4_ST_START:
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
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    যদি(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned বর্ণ buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      যদি(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        প্রত্যাগম socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      যদি(result || (nwritten != 4))
        প্রত্যাগম socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      যদি(presult)
        প্রত্যাগম socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      যদি(result || (nwritten != hlen))
        প্রত্যাগম socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      গচ্ছ process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  পক্ষ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    যদি(!done)
      প্রত্যাগম CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  পক্ষ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  পক্ষ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  পক্ষ SOCKS_ST_SUCCESS:
    প্রত্যাগম CURLPX_OK;

  পক্ষ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    প্রত্যাগম sx->presult;

  স্বভাব:
    DEBUGASSERT(0);
    প্রত্যাগম socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

স্থির CURLproxycode socks5_req0_init(সংরচনা Curl_cfilter *cf,
                                      সংরচনা socks_ctx *sx,
                                      সংরচনা Curl_easy *data)
{
  ধ্রুব unsigned বর্ণ auth = data->set.socks5auth;
  unsigned বর্ণ req[5]; /* version + len + 3 possible auth methods */
  unsigned বর্ণ nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (শূন্য)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  যদি(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    প্রত্যাগম CURLPX_LONG_HOSTNAME;
  }

  যদি(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  যদি(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  যদি(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  যদি(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  যদি(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  যদি(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  যদি(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    প্রত্যাগম CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  যদি(result || (nwritten != req_len))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks5_check_resp0(সংরচনা socks_ctx *sx,
                                        সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data)
{
  ধ্রুব unsigned বর্ণ *resp;
  unsigned বর্ণ auth_mode;
  size_t rlen;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    প্রত্যাগম CURLPX_RECV_CONNECT;
  }

  যদি(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    প্রত্যাগম CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  বিকল্প(auth_mode) {
  পক্ষ 0:
    যদি(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      প্রত্যাগম CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    প্রত্যাগম CURLPX_OK;
  পক্ষ 1:
    যদি(data->set.socks5auth & CURLAUTH_GSSAPI) {
      যদি(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      প্রত্যাগম CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    প্রত্যাগম CURLPX_GSSAPI_PERMSG;
  পক্ষ 2:
    /* regular name + password authentication */
    যদি(data->set.socks5auth & CURLAUTH_BASIC) {
      যদি(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      প্রত্যাগম CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    প্রত্যাগম CURLPX_NO_AUTH;
  পক্ষ 255:
    failf(data, "No authentication method was acceptable.");
    প্রত্যাগম CURLPX_NO_AUTH;
  স্বভাব:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    প্রত্যাগম CURLPX_UNKNOWN_MODE;
  }
}

স্থির CURLproxycode socks5_auth_init(সংরচনা Curl_cfilter *cf,
                                      সংরচনা socks_ctx *sx,
                                      সংরচনা Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned বর্ণ buf[2];
  CURLcode result;

  যদি(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    যদি(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      প্রত্যাগম CURLPX_LONG_USER;
    }
    যদি(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      প্রত্যাগম CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned বর্ণ)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  যদি(result || (nwritten != 2))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  যদি(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    যদি(result || (nwritten != ulen))
      প্রত্যাগম CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned বর্ণ)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  যদি(result || (nwritten != 1))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  যদি(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    যদি(result || (nwritten != plen))
      প্রত্যাগম CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks5_check_auth_resp(সংরচনা socks_ctx *sx,
                                            সংরচনা Curl_cfilter *cf,
                                            সংরচনা Curl_easy *data)
{
  ধ্রুব unsigned বর্ণ *resp;
  unsigned বর্ণ auth_status;
  size_t rlen;

  (শূন্য)cf;
  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    প্রত্যাগম CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  যদি(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    প্রত্যাগম CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks5_req1_init(সংরচনা socks_ctx *sx,
                                      সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  unsigned বর্ণ req[5];
  unsigned বর্ণ ipbuf[16];
  ধ্রুব unsigned বর্ণ *destination;
  unsigned বর্ণ desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  যদি(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    যদি(result || (nwritten != 3))
      প্রত্যাগম CURLPX_SEND_REQUEST;
    প্রত্যাগম CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  যদি(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    যদি(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      প্রত্যাগম CURLPX_BAD_ADDRESS_TYPE;
  }
  অন্যথা
#endif
  যদি(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  অন্যথা {
    ধ্রুব size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ধ্রুব unsigned বর্ণ *)sx->dest->hostname;
    destlen = (unsigned বর্ণ)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  যদি(result || (nwritten != hdlen))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  যদি(result || (nwritten != destlen))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned বর্ণ)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned বর্ণ)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  যদি(result || (nwritten != 2))
    প্রত্যাগম CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  প্রত্যাগম CURLPX_OK;
}

স্থির CURLproxycode socks5_resolving(সংরচনা socks_ctx *sx,
                                      সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      bool *done)
{
  ধ্রুব সংরচনা Curl_addrinfo *ai = রিক্ত;
  বর্ণ dest[MAX_IPADR_LEN];  /* printable address */
  ধ্রুব unsigned বর্ণ *destination = রিক্ত;
  unsigned বর্ণ desttype = 1, destlen = 4;
  unsigned বর্ণ req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  যদি(result) {
    যদি(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (পূর্ণাঙ্ক)result, sx->dest->hostname, sx->dest->port);
      প্রত্যাগম CURLPX_RESOLVE_HOST;
    }
    প্রত্যাগম CURLPX_OK;
  }

#ifdef USE_IPV6
  যদি(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  যদি(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  যদি(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    গচ্ছ out;
  }

  Curl_printable_address(ai, dest, পরিমাণ(dest));

  যদি(ai->ai_family == AF_INET) {
    সংরচনা sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (সংরচনা sockaddr_in *)(শূন্য *)ai->ai_addr;
    destination = (ধ্রুব unsigned বর্ণ *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  অন্যথা যদি(ai->ai_family == AF_INET6) {
    সংরচনা sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (সংরচনা sockaddr_in6 *)(শূন্য *)ai->ai_addr;
    destination = (ধ্রুব unsigned বর্ণ *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  যদি(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    গচ্ছ out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  যদি(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    গচ্ছ out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  যদি(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    গচ্ছ out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned বর্ণ)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned বর্ণ)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  যদি(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    গচ্ছ out;
  }

out:
  *done = (presult == CURLPX_OK);
  প্রত্যাগম presult;
}

স্থির CURLproxycode socks5_recv_resp1(সংরচনা socks_ctx *sx,
                                       সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       bool *done)
{
  ধ্রুব unsigned বর্ণ *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  যদি(presult)
    প্রত্যাগম presult;
  অন্যথা যদি(!*done)
    প্রত্যাগম CURLPX_OK;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    প্রত্যাগম CURLPX_RECV_CONNECT;
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
  যদি(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    প্রত্যাগম CURLPX_BAD_VERSION;
  }
  অন্যথা যদি(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    পূর্ণাঙ্ক code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    যদি(code < 9) {
      /* RFC 1928 section 6 lists: */
      স্থির ধ্রুব CURLproxycode lookup[] = {
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
    প্রত্যাগম rc;
  }

  /* Calculate real packet size */
  বিকল্প(resp[3]) {
  পক্ষ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ভঙ্গ;
  পক্ষ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ভঙ্গ;
  পক্ষ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ভঙ্গ;
  স্বভাব:
    failf(data, "SOCKS5 reply has wrong address type.");
    প্রত্যাগম CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  যদি(presult)
    প্রত্যাগম presult;
  অন্যথা যদি(!*done)
    প্রত্যাগম CURLPX_OK;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    প্রত্যাগম CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  প্রত্যাগম CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
স্থির CURLproxycode socks5_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা socks_ctx *sx,
                                    সংরচনা Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  বিকল্প(sx->state) {
  পক্ষ SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    গচ্ছ process_state;

  পক্ষ SOCKS5_ST_GSSAPI_INIT: {
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    যদি(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      প্রত্যাগম CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    গচ্ছ process_state;
#অন্যথা
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    প্রত্যাগম socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  পক্ষ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    যদি(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      গচ্ছ process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    যদি(!done)
      প্রত্যাগম CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    অন্যথা যদি(!done)
      প্রত্যাগম CURLPX_OK;
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    যদি(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      প্রত্যাগম CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  পক্ষ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    যদি(presult)
      প্রত্যাগম socks_failed(sx, cf, data, presult);
    যদি(!done)
      প্রত্যাগম CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  পক্ষ SOCKS_ST_SUCCESS:
    প্রত্যাগম CURLPX_OK;

  পক্ষ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    প্রত্যাগম sx->presult;

  স্বভাব:
    DEBUGASSERT(0);
    প্রত্যাগম socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

স্থির শূন্য socks_proxy_ctx_free(সংরচনা socks_ctx *ctx)
{
  যদি(ctx) {
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
স্থির CURLcode socks_proxy_cf_connect(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       bool *done)
{
  সংরচনা socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  যদি(result || !*done)
    প্রত্যাগম result;

  বিকল্প(ctx->proxy_type) {
  পক্ষ CURLPROXY_SOCKS5:
  পক্ষ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    ভঙ্গ;

  পক্ষ CURLPROXY_SOCKS4:
  পক্ষ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    ভঙ্গ;

  স্বভাব:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    গচ্ছ out;
  }

  যদি(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    গচ্ছ out;
  }
  অন্যথা যদি(ctx->state != SOCKS_ST_SUCCESS)
    গচ্ছ out;

#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(data)) {
    সংরচনা ip_quadruple ipquad;
    bool is_ipv6;
    যদি(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    অন্যথা
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  যদি(*done || result)
    Curl_creds_unlink(&ctx->creds);
  প্রত্যাগম result;
}

স্থির CURLcode socks_cf_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা easy_pollset *ps)
{
  সংরচনা socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    বিকল্প(sx->state) {
    পক্ষ SOCKS4_ST_SEND:
    পক্ষ SOCKS5_ST_REQ0_SEND:
    পক্ষ SOCKS5_ST_AUTH_SEND:
    পক্ষ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (পূর্ণাঙ্ক)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      ভঙ্গ;
    স্বভাব:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (পূর্ণাঙ্ক)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      ভঙ্গ;
    }
  }
  প্রত্যাগম result;
}

স্থির শূন্য socks_proxy_cf_destroy(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  (শূন্য)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = রিক্ত;
}

স্থির CURLcode socks_cf_query(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা socks_ctx *sx = cf->ctx;

  বিকল্প(query) {
  পক্ষ CF_QUERY_HOST_PORT:
    যদি(sx) {
      *pres1 = sx->dest->port;
      *((ধ্রুব বর্ণ **)pres2) = sx->dest->hostname;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = রিক্ত;
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

সংরচনা Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                          সংরচনা Curl_easy *data,
                                          সংরচনা Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          সংরচনা Curl_creds *creds)
{
  সংরচনা Curl_cfilter *cf;
  সংরচনা socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  যদি(!dest)
    প্রত্যাগম CURLE_FAILED_INIT;

  বিকল্প(proxy_type) {
  পক্ষ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ভঙ্গ;
  পক্ষ CURLPROXY_SOCKS5_HOSTNAME:
    ভঙ্গ;
  পক্ষ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ভঙ্গ;
  পক্ষ CURLPROXY_SOCKS4A:
    ভঙ্গ;
  স্বভাব:
    failf(data, "unknown proxytype %d option given", proxy_type);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  যদি(!result) {
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
    যদি(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  অন্যথা
    socks_proxy_ctx_free(ctx);
  প্রত্যাগম result;
}

#endif /* CURL_DISABLE_PROXY */
