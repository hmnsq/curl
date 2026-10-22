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

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

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
شمارش socks_state_t {
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

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
ایستا ثابت نویسه * ثابت cf_socks_statename[] = {
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

ساختار socks_ctx {
  شمارش socks_state_t state;
  ساختار bufq iobuf;
  ساختار Curl_peer *dest;
  ساختار Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned نویسه version;
  BIT(resolve_local);
  BIT(socks4a);
};

#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            نویسه *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  برای(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    اگر(timeout_ms < 0) {
      /* we already got the timeout */
      بازگشت CURLE_OPERATION_TIMEDOUT;
    }
    اگر(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    اگر(SOCKET_READABLE(sock, timeout_ms) <= 0)
      بازگشت CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    اگر(result == CURLE_AGAIN)
      ادامه;
    وگرنه اگر(result)
      بازگشت result;

    اگر(blen == nread) {
      *pnread += nread;
      بازگشت CURLE_OK;
    }
    اگر(!nread) /* EOF */
      بازگشت CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#وگرنه
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
ایستا تهی socksstate(ساختار socks_ctx *sx,
                       ساختار Curl_cfilter *cf,
                       ساختار Curl_easy *data,
                       شمارش socks_state_t state
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , صحیح lineno
#endif
)
{
  شمارش socks_state_t oldstate = sx->state;

  اگر(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    بازگشت;

  sx->state = state;

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#وگرنه
  (تهی)cf;
  (تهی)data;
#endif
}

ایستا CURLproxycode socks_failed(ساختار socks_ctx *sx,
                                  ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  بازگشت presult;
}

ایستا CURLproxycode socks_flush(ساختار socks_ctx *sx,
                                 ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  وقتی(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, پوچ, 0,
                               &nwritten);
    اگر(result == CURLE_AGAIN)
      بازگشت CURLPX_OK;
    وگرنه اگر(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      بازگشت socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks_recv(ساختار socks_ctx *sx,
                                ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  وقتی(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    اگر(result == CURLE_AGAIN)
      بازگشت CURLPX_OK;
    وگرنه اگر(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      بازگشت CURLPX_RECV_CONNECT;
    }
    وگرنه اگر(!nread) { /* EOF */
      اگر(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        بازگشت CURLPX_RECV_CONNECT;
      }
      بشکن;
    }
  }
  *done = TRUE;
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks4_req_add_hd(ساختار socks_ctx *sx,
                                       ساختار Curl_easy *data)
{
  unsigned نویسه buf[4];
  size_t nwritten;
  CURLcode result;

  (تهی)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned نویسه)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned نویسه)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  اگر(result || (nwritten != 4))
    بازگشت CURLPX_SEND_REQUEST;
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks4_req_add_user(ساختار socks_ctx *sx,
                                         ساختار Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  اگر(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    اگر(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      بازگشت CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    اگر(result || (nwritten != (plen + 1)))
      بازگشت CURLPX_SEND_REQUEST;
  }
  وگرنه {
    /* empty username */
    unsigned نویسه b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    اگر(result || (nwritten != 1))
      بازگشت CURLPX_SEND_REQUEST;
  }
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks4_resolving(ساختار socks_ctx *sx,
                                      ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      bool *done)
{
  ثابت ساختار Curl_addrinfo *ai = پوچ;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  اگر(result) {
    اگر(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (صحیح)result, sx->dest->hostname, sx->dest->port);
      بازگشت CURLPX_RESOLVE_HOST;
    }
    بازگشت CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  اگر(ai) {
    ساختار sockaddr_in *saddr_in;
    نویسه ipbuf[64];

    Curl_printable_address(ai, ipbuf, اندازه(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (ساختار sockaddr_in *)(تهی *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned نویسه *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    اگر(result || (nwritten != 4))
      بازگشت CURLPX_SEND_REQUEST;
  }
  وگرنه {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    بازگشت CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks4_check_resp(ساختار socks_ctx *sx,
                                       ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data)
{
  ثابت unsigned نویسه *resp;
  size_t rlen;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    بازگشت CURLPX_RECV_CONNECT;
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
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    بازگشت CURLPX_BAD_VERSION;
  }

  /* Result */
  انتخاب(resp[1]) {
  حالت 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    بازگشت CURLPX_OK;
  حالت 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    بازگشت CURLPX_REQUEST_FAILED;
  حالت 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    بازگشت CURLPX_IDENTD;
  حالت 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    بازگشت CURLPX_IDENTD_DIFFER;
  پیشفرض:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحیح)((resp[2] << 8) | resp[3]), resp[1]);
    بازگشت CURLPX_UNKNOWN_FAIL;
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
ایستا CURLproxycode socks4_connect(ساختار Curl_cfilter *cf,
                                    ساختار socks_ctx *sx,
                                    ساختار Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  انتخاب(sx->state) {
  حالت SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  حالت SOCKS4_ST_START:
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
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    اگر(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned نویسه buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      اگر(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        بازگشت socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      اگر(result || (nwritten != 4))
        بازگشت socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      اگر(presult)
        بازگشت socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      اگر(result || (nwritten != hlen))
        بازگشت socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      برو process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  حالت SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    اگر(!done)
      بازگشت CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  حالت SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  حالت SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالت SOCKS_ST_SUCCESS:
    بازگشت CURLPX_OK;

  حالت SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    بازگشت sx->presult;

  پیشفرض:
    DEBUGASSERT(0);
    بازگشت socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ایستا CURLproxycode socks5_req0_init(ساختار Curl_cfilter *cf,
                                      ساختار socks_ctx *sx,
                                      ساختار Curl_easy *data)
{
  ثابت unsigned نویسه auth = data->set.socks5auth;
  unsigned نویسه req[5]; /* version + len + 3 possible auth methods */
  unsigned نویسه nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (تهی)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  اگر(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    بازگشت CURLPX_LONG_HOSTNAME;
  }

  اگر(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  اگر(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  اگر(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  اگر(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  اگر(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  اگر(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  اگر(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    بازگشت CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  اگر(result || (nwritten != req_len))
    بازگشت CURLPX_SEND_REQUEST;
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks5_check_resp0(ساختار socks_ctx *sx,
                                        ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data)
{
  ثابت unsigned نویسه *resp;
  unsigned نویسه auth_mode;
  size_t rlen;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    بازگشت CURLPX_RECV_CONNECT;
  }

  اگر(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    بازگشت CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  انتخاب(auth_mode) {
  حالت 0:
    اگر(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      بازگشت CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    بازگشت CURLPX_OK;
  حالت 1:
    اگر(data->set.socks5auth & CURLAUTH_GSSAPI) {
      اگر(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      بازگشت CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    بازگشت CURLPX_GSSAPI_PERMSG;
  حالت 2:
    /* regular name + password authentication */
    اگر(data->set.socks5auth & CURLAUTH_BASIC) {
      اگر(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      بازگشت CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    بازگشت CURLPX_NO_AUTH;
  حالت 255:
    failf(data, "No authentication method was acceptable.");
    بازگشت CURLPX_NO_AUTH;
  پیشفرض:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    بازگشت CURLPX_UNKNOWN_MODE;
  }
}

ایستا CURLproxycode socks5_auth_init(ساختار Curl_cfilter *cf,
                                      ساختار socks_ctx *sx,
                                      ساختار Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned نویسه buf[2];
  CURLcode result;

  اگر(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    اگر(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      بازگشت CURLPX_LONG_USER;
    }
    اگر(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      بازگشت CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned نویسه)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  اگر(result || (nwritten != 2))
    بازگشت CURLPX_SEND_REQUEST;
  اگر(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    اگر(result || (nwritten != ulen))
      بازگشت CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned نویسه)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  اگر(result || (nwritten != 1))
    بازگشت CURLPX_SEND_REQUEST;
  اگر(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    اگر(result || (nwritten != plen))
      بازگشت CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks5_check_auth_resp(ساختار socks_ctx *sx,
                                            ساختار Curl_cfilter *cf,
                                            ساختار Curl_easy *data)
{
  ثابت unsigned نویسه *resp;
  unsigned نویسه auth_status;
  size_t rlen;

  (تهی)cf;
  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    بازگشت CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  اگر(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    بازگشت CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks5_req1_init(ساختار socks_ctx *sx,
                                      ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  unsigned نویسه req[5];
  unsigned نویسه ipbuf[16];
  ثابت unsigned نویسه *destination;
  unsigned نویسه desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  اگر(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    اگر(result || (nwritten != 3))
      بازگشت CURLPX_SEND_REQUEST;
    بازگشت CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  اگر(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    اگر(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      بازگشت CURLPX_BAD_ADDRESS_TYPE;
  }
  وگرنه
#endif
  اگر(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  وگرنه {
    ثابت size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ثابت unsigned نویسه *)sx->dest->hostname;
    destlen = (unsigned نویسه)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  اگر(result || (nwritten != hdlen))
    بازگشت CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  اگر(result || (nwritten != destlen))
    بازگشت CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned نویسه)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned نویسه)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  اگر(result || (nwritten != 2))
    بازگشت CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  بازگشت CURLPX_OK;
}

ایستا CURLproxycode socks5_resolving(ساختار socks_ctx *sx,
                                      ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      bool *done)
{
  ثابت ساختار Curl_addrinfo *ai = پوچ;
  نویسه dest[MAX_IPADR_LEN];  /* printable address */
  ثابت unsigned نویسه *destination = پوچ;
  unsigned نویسه desttype = 1, destlen = 4;
  unsigned نویسه req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  اگر(result) {
    اگر(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (صحیح)result, sx->dest->hostname, sx->dest->port);
      بازگشت CURLPX_RESOLVE_HOST;
    }
    بازگشت CURLPX_OK;
  }

#ifdef USE_IPV6
  اگر(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  اگر(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  اگر(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    برو out;
  }

  Curl_printable_address(ai, dest, اندازه(dest));

  اگر(ai->ai_family == AF_INET) {
    ساختار sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (ساختار sockaddr_in *)(تهی *)ai->ai_addr;
    destination = (ثابت unsigned نویسه *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  وگرنه اگر(ai->ai_family == AF_INET6) {
    ساختار sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ساختار sockaddr_in6 *)(تهی *)ai->ai_addr;
    destination = (ثابت unsigned نویسه *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  اگر(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    برو out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  اگر(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    برو out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  اگر(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    برو out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned نویسه)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned نویسه)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  اگر(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    برو out;
  }

out:
  *done = (presult == CURLPX_OK);
  بازگشت presult;
}

ایستا CURLproxycode socks5_recv_resp1(ساختار socks_ctx *sx,
                                       ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       bool *done)
{
  ثابت unsigned نویسه *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  اگر(presult)
    بازگشت presult;
  وگرنه اگر(!*done)
    بازگشت CURLPX_OK;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    بازگشت CURLPX_RECV_CONNECT;
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
  اگر(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    بازگشت CURLPX_BAD_VERSION;
  }
  وگرنه اگر(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    صحیح code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    اگر(code < 9) {
      /* RFC 1928 section 6 lists: */
      ایستا ثابت CURLproxycode lookup[] = {
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
    بازگشت rc;
  }

  /* Calculate real packet size */
  انتخاب(resp[3]) {
  حالت 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    بشکن;
  حالت 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    بشکن;
  حالت 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    بشکن;
  پیشفرض:
    failf(data, "SOCKS5 reply has wrong address type.");
    بازگشت CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  اگر(presult)
    بازگشت presult;
  وگرنه اگر(!*done)
    بازگشت CURLPX_OK;

  اگر(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    بازگشت CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  بازگشت CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ایستا CURLproxycode socks5_connect(ساختار Curl_cfilter *cf,
                                    ساختار socks_ctx *sx,
                                    ساختار Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  انتخاب(sx->state) {
  حالت SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  حالت SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    برو process_state;

  حالت SOCKS5_ST_GSSAPI_INIT: {
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    اگر(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      بازگشت CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    برو process_state;
#وگرنه
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    بازگشت socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  حالت SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    اگر(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      برو process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    اگر(!done)
      بازگشت CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  حالت SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    وگرنه اگر(!done)
      بازگشت CURLPX_OK;
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    اگر(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      بازگشت CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  حالت SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    اگر(presult)
      بازگشت socks_failed(sx, cf, data, presult);
    اگر(!done)
      بازگشت CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالت SOCKS_ST_SUCCESS:
    بازگشت CURLPX_OK;

  حالت SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    بازگشت sx->presult;

  پیشفرض:
    DEBUGASSERT(0);
    بازگشت socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ایستا تهی socks_proxy_ctx_free(ساختار socks_ctx *ctx)
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
ایستا CURLcode socks_proxy_cf_connect(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       bool *done)
{
  ساختار socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  اگر(result || !*done)
    بازگشت result;

  انتخاب(ctx->proxy_type) {
  حالت CURLPROXY_SOCKS5:
  حالت CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    بشکن;

  حالت CURLPROXY_SOCKS4:
  حالت CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    بشکن;

  پیشفرض:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    برو out;
  }

  اگر(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    برو out;
  }
  وگرنه اگر(ctx->state != SOCKS_ST_SUCCESS)
    برو out;

#ifdef CURLVERBOSE
  اگر(Curl_trc_is_verbose(data)) {
    ساختار ip_quadruple ipquad;
    bool is_ipv6;
    اگر(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    وگرنه
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  اگر(*done || result)
    Curl_creds_unlink(&ctx->creds);
  بازگشت result;
}

ایستا CURLcode socks_cf_adjust_pollset(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ساختار easy_pollset *ps)
{
  ساختار socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    انتخاب(sx->state) {
    حالت SOCKS4_ST_SEND:
    حالت SOCKS5_ST_REQ0_SEND:
    حالت SOCKS5_ST_AUTH_SEND:
    حالت SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (صحیح)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      بشکن;
    پیشفرض:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (صحیح)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      بشکن;
    }
  }
  بازگشت result;
}

ایستا تهی socks_proxy_cf_destroy(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  (تهی)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = پوچ;
}

ایستا CURLcode socks_cf_query(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار socks_ctx *sx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_HOST_PORT:
    اگر(sx) {
      *pres1 = sx->dest->port;
      *((ثابت نویسه **)pres2) = sx->dest->hostname;
      بازگشت CURLE_OK;
    }
    بشکن;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = پوچ;
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساختار Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                          ساختار Curl_easy *data,
                                          ساختار Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          ساختار Curl_creds *creds)
{
  ساختار Curl_cfilter *cf;
  ساختار socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  اگر(!dest)
    بازگشت CURLE_FAILED_INIT;

  انتخاب(proxy_type) {
  حالت CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    بشکن;
  حالت CURLPROXY_SOCKS5_HOSTNAME:
    بشکن;
  حالت CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    بشکن;
  حالت CURLPROXY_SOCKS4A:
    بشکن;
  پیشفرض:
    failf(data, "unknown proxytype %d option given", proxy_type);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  اگر(!result) {
    Curl_conn_cf_درج_after(cf_at, cf);
    اگر(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  وگرنه
    socks_proxy_ctx_free(ctx);
  بازگشت result;
}

#endif /* CURL_DISABLE_PROXY */
