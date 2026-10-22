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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "socks.h"
#ضمن "vdns/cf-dns.h"
#ضمن "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
تعداد socks_state_t {
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

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن ثابت حرف * ثابت cf_socks_statename[] = {
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

بنية socks_ctx {
  تعداد socks_state_t state;
  بنية bufq iobuf;
  بنية Curl_peer *dest;
  بنية Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned حرف version;
  BIT(resolve_local);
  BIT(socks4a);
};

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            حرف *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  لكل(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    إذا(timeout_ms < 0) {
      /* we already got the timeout */
      أرجع CURLE_OPERATION_TIMEDOUT;
    }
    إذا(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    إذا(SOCKET_READABLE(sock, timeout_ms) <= 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    إذا(result == CURLE_AGAIN)
      استمر;
    وإلا إذا(result)
      أرجع result;

    إذا(blen == nread) {
      *pnread += nread;
      أرجع CURLE_OK;
    }
    إذا(!nread) /* EOF */
      أرجع CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#وإلا
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
ساكن فراغ socksstate(بنية socks_ctx *sx,
                       بنية Curl_cfilter *cf,
                       بنية Curl_easy *data,
                       تعداد socks_state_t state
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , صحيح lineno
#endif
)
{
  تعداد socks_state_t oldstate = sx->state;

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

  sx->state = state;

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#وإلا
  (فراغ)cf;
  (فراغ)data;
#endif
}

ساكن CURLproxycode socks_failed(بنية socks_ctx *sx,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  أرجع presult;
}

ساكن CURLproxycode socks_flush(بنية socks_ctx *sx,
                                 بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  طالما(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, عدم, 0,
                               &nwritten);
    إذا(result == CURLE_AGAIN)
      أرجع CURLPX_OK;
    وإلا إذا(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      أرجع socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks_recv(بنية socks_ctx *sx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  طالما(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    إذا(result == CURLE_AGAIN)
      أرجع CURLPX_OK;
    وإلا إذا(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      أرجع CURLPX_RECV_CONNECT;
    }
    وإلا إذا(!nread) { /* EOF */
      إذا(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        أرجع CURLPX_RECV_CONNECT;
      }
      اكسر;
    }
  }
  *done = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_req_add_hd(بنية socks_ctx *sx,
                                       بنية Curl_easy *data)
{
  unsigned حرف buf[4];
  size_t nwritten;
  CURLcode result;

  (فراغ)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned حرف)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned حرف)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  إذا(result || (nwritten != 4))
    أرجع CURLPX_SEND_REQUEST;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_req_add_user(بنية socks_ctx *sx,
                                         بنية Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  إذا(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    إذا(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      أرجع CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    إذا(result || (nwritten != (plen + 1)))
      أرجع CURLPX_SEND_REQUEST;
  }
  وإلا {
    /* empty username */
    unsigned حرف b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    إذا(result || (nwritten != 1))
      أرجع CURLPX_SEND_REQUEST;
  }
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_resolving(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *done)
{
  ثابت بنية Curl_addrinfo *ai = عدم;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  إذا(result) {
    إذا(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (صحيح)result, sx->dest->hostname, sx->dest->port);
      أرجع CURLPX_RESOLVE_HOST;
    }
    أرجع CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  إذا(ai) {
    بنية sockaddr_in *saddr_in;
    حرف ipbuf[64];

    Curl_printable_address(ai, ipbuf, حجم(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (بنية sockaddr_in *)(فراغ *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned حرف *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    إذا(result || (nwritten != 4))
      أرجع CURLPX_SEND_REQUEST;
  }
  وإلا {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    أرجع CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks4_check_resp(بنية socks_ctx *sx,
                                       بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data)
{
  ثابت unsigned حرف *resp;
  size_t rlen;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
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
  إذا(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    أرجع CURLPX_BAD_VERSION;
  }

  /* Result */
  بدل(resp[1]) {
  حالة 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    أرجع CURLPX_OK;
  حالة 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_REQUEST_FAILED;
  حالة 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_IDENTD;
  حالة 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_IDENTD_DIFFER;
  افتراضي:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned صحيح)((resp[2] << 8) | resp[3]), resp[1]);
    أرجع CURLPX_UNKNOWN_FAIL;
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
ساكن CURLproxycode socks4_connect(بنية Curl_cfilter *cf,
                                    بنية socks_ctx *sx,
                                    بنية Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  بدل(sx->state) {
  حالة SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  حالة SOCKS4_ST_START:
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
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    إذا(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned حرف buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      إذا(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        أرجع socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      إذا(result || (nwritten != 4))
        أرجع socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      إذا(presult)
        أرجع socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      إذا(result || (nwritten != hlen))
        أرجع socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      اذهب process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  حالة SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    إذا(!done)
      أرجع CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  حالة SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  حالة SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالة SOCKS_ST_SUCCESS:
    أرجع CURLPX_OK;

  حالة SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    أرجع sx->presult;

  افتراضي:
    DEBUGASSERT(0);
    أرجع socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ساكن CURLproxycode socks5_req0_init(بنية Curl_cfilter *cf,
                                      بنية socks_ctx *sx,
                                      بنية Curl_easy *data)
{
  ثابت unsigned حرف auth = data->set.socks5auth;
  unsigned حرف req[5]; /* version + len + 3 possible auth methods */
  unsigned حرف nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (فراغ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  إذا(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    أرجع CURLPX_LONG_HOSTNAME;
  }

  إذا(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  إذا(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  إذا(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  إذا(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  إذا(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  إذا(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  إذا(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    أرجع CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  إذا(result || (nwritten != req_len))
    أرجع CURLPX_SEND_REQUEST;
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_check_resp0(بنية socks_ctx *sx,
                                        بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data)
{
  ثابت unsigned حرف *resp;
  unsigned حرف auth_mode;
  size_t rlen;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }

  إذا(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    أرجع CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  بدل(auth_mode) {
  حالة 0:
    إذا(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      أرجع CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    أرجع CURLPX_OK;
  حالة 1:
    إذا(data->set.socks5auth & CURLAUTH_GSSAPI) {
      إذا(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      أرجع CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    أرجع CURLPX_GSSAPI_PERMSG;
  حالة 2:
    /* regular name + password authentication */
    إذا(data->set.socks5auth & CURLAUTH_BASIC) {
      إذا(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      أرجع CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    أرجع CURLPX_NO_AUTH;
  حالة 255:
    failf(data, "No authentication method was acceptable.");
    أرجع CURLPX_NO_AUTH;
  افتراضي:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    أرجع CURLPX_UNKNOWN_MODE;
  }
}

ساكن CURLproxycode socks5_auth_init(بنية Curl_cfilter *cf,
                                      بنية socks_ctx *sx,
                                      بنية Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned حرف buf[2];
  CURLcode result;

  إذا(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    إذا(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      أرجع CURLPX_LONG_USER;
    }
    إذا(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      أرجع CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned حرف)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  إذا(result || (nwritten != 2))
    أرجع CURLPX_SEND_REQUEST;
  إذا(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    إذا(result || (nwritten != ulen))
      أرجع CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned حرف)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  إذا(result || (nwritten != 1))
    أرجع CURLPX_SEND_REQUEST;
  إذا(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    إذا(result || (nwritten != plen))
      أرجع CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_check_auth_resp(بنية socks_ctx *sx,
                                            بنية Curl_cfilter *cf,
                                            بنية Curl_easy *data)
{
  ثابت unsigned حرف *resp;
  unsigned حرف auth_status;
  size_t rlen;

  (فراغ)cf;
  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  إذا(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    أرجع CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_req1_init(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  unsigned حرف req[5];
  unsigned حرف ipbuf[16];
  ثابت unsigned حرف *destination;
  unsigned حرف desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  إذا(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    إذا(result || (nwritten != 3))
      أرجع CURLPX_SEND_REQUEST;
    أرجع CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  إذا(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    إذا(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      أرجع CURLPX_BAD_ADDRESS_TYPE;
  }
  وإلا
#endif
  إذا(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  وإلا {
    ثابت size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ثابت unsigned حرف *)sx->dest->hostname;
    destlen = (unsigned حرف)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  إذا(result || (nwritten != hdlen))
    أرجع CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  إذا(result || (nwritten != destlen))
    أرجع CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned حرف)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned حرف)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  إذا(result || (nwritten != 2))
    أرجع CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  أرجع CURLPX_OK;
}

ساكن CURLproxycode socks5_resolving(بنية socks_ctx *sx,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *done)
{
  ثابت بنية Curl_addrinfo *ai = عدم;
  حرف dest[MAX_IPADR_LEN];  /* printable address */
  ثابت unsigned حرف *destination = عدم;
  unsigned حرف desttype = 1, destlen = 4;
  unsigned حرف req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  إذا(result) {
    إذا(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (صحيح)result, sx->dest->hostname, sx->dest->port);
      أرجع CURLPX_RESOLVE_HOST;
    }
    أرجع CURLPX_OK;
  }

#ifdef USE_IPV6
  إذا(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  إذا(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  إذا(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    اذهب out;
  }

  Curl_printable_address(ai, dest, حجم(dest));

  إذا(ai->ai_family == AF_INET) {
    بنية sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (بنية sockaddr_in *)(فراغ *)ai->ai_addr;
    destination = (ثابت unsigned حرف *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  وإلا إذا(ai->ai_family == AF_INET6) {
    بنية sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (بنية sockaddr_in6 *)(فراغ *)ai->ai_addr;
    destination = (ثابت unsigned حرف *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  إذا(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    اذهب out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  إذا(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  إذا(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned حرف)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned حرف)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  إذا(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    اذهب out;
  }

out:
  *done = (presult == CURLPX_OK);
  أرجع presult;
}

ساكن CURLproxycode socks5_recv_resp1(بنية socks_ctx *sx,
                                       بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       bool *done)
{
  ثابت unsigned حرف *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  إذا(presult)
    أرجع presult;
  وإلا إذا(!*done)
    أرجع CURLPX_OK;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
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
  إذا(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    أرجع CURLPX_BAD_VERSION;
  }
  وإلا إذا(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    صحيح code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    إذا(code < 9) {
      /* RFC 1928 section 6 lists: */
      ساكن ثابت CURLproxycode lookup[] = {
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
    أرجع rc;
  }

  /* Calculate real packet size */
  بدل(resp[3]) {
  حالة 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    اكسر;
  حالة 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    اكسر;
  حالة 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    اكسر;
  افتراضي:
    failf(data, "SOCKS5 reply has wrong address type.");
    أرجع CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  إذا(presult)
    أرجع presult;
  وإلا إذا(!*done)
    أرجع CURLPX_OK;

  إذا(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    أرجع CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  أرجع CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ساكن CURLproxycode socks5_connect(بنية Curl_cfilter *cf,
                                    بنية socks_ctx *sx,
                                    بنية Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  بدل(sx->state) {
  حالة SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  حالة SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    اذهب process_state;

  حالة SOCKS5_ST_GSSAPI_INIT: {
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    إذا(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      أرجع CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    اذهب process_state;
#وإلا
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    أرجع socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  حالة SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    إذا(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      اذهب process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    إذا(!done)
      أرجع CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  حالة SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    وإلا إذا(!done)
      أرجع CURLPX_OK;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    إذا(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      أرجع CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  حالة SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    إذا(presult)
      أرجع socks_failed(sx, cf, data, presult);
    إذا(!done)
      أرجع CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  حالة SOCKS_ST_SUCCESS:
    أرجع CURLPX_OK;

  حالة SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    أرجع sx->presult;

  افتراضي:
    DEBUGASSERT(0);
    أرجع socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ساكن فراغ socks_proxy_ctx_free(بنية socks_ctx *ctx)
{
  إذا(ctx) {
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
ساكن CURLcode socks_proxy_cf_connect(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       bool *done)
{
  بنية socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  إذا(result || !*done)
    أرجع result;

  بدل(ctx->proxy_type) {
  حالة CURLPROXY_SOCKS5:
  حالة CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    اكسر;

  حالة CURLPROXY_SOCKS4:
  حالة CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    اكسر;

  افتراضي:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    اذهب out;
  }

  إذا(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    اذهب out;
  }
  وإلا إذا(ctx->state != SOCKS_ST_SUCCESS)
    اذهب out;

#ifdef CURLVERBOSE
  إذا(Curl_trc_is_verbose(data)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    إذا(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    وإلا
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  إذا(*done || result)
    Curl_creds_unlink(&ctx->creds);
  أرجع result;
}

ساكن CURLcode socks_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        بنية easy_pollset *ps)
{
  بنية socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    بدل(sx->state) {
    حالة SOCKS4_ST_SEND:
    حالة SOCKS5_ST_REQ0_SEND:
    حالة SOCKS5_ST_AUTH_SEND:
    حالة SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (صحيح)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      اكسر;
    افتراضي:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (صحيح)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      اكسر;
    }
  }
  أرجع result;
}

ساكن فراغ socks_proxy_cf_destroy(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  (فراغ)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = عدم;
}

ساكن CURLcode socks_cf_query(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية socks_ctx *sx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_PORT:
    إذا(sx) {
      *pres1 = sx->dest->port;
      *((ثابت حرف **)pres2) = sx->dest->hostname;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

بنية Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_قىستۇرۇش_after(بنية Curl_cfilter *cf_at,
                                          بنية Curl_easy *data,
                                          بنية Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          بنية Curl_creds *creds)
{
  بنية Curl_cfilter *cf;
  بنية socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  إذا(!dest)
    أرجع CURLE_FAILED_INIT;

  بدل(proxy_type) {
  حالة CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    اكسر;
  حالة CURLPROXY_SOCKS5_HOSTNAME:
    اكسر;
  حالة CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    اكسر;
  حالة CURLPROXY_SOCKS4A:
    اكسر;
  افتراضي:
    failf(data, "unknown proxytype %d option given", proxy_type);
    أرجع CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    أرجع CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  إذا(!result) {
    Curl_conn_cf_قىستۇرۇش_after(cf_at, cf);
    إذا(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  وإلا
    socks_proxy_ctx_free(ctx);
  أرجع result;
}

#endif /* CURL_DISABLE_PROXY */
