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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "socks.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
ગણના socks_state_t {
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

#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
સ્થિર અચલ અક્ષર * અચલ cf_socks_statename[] = {
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

સંરચના socks_ctx {
  ગણના socks_state_t state;
  સંરચના bufq iobuf;
  સંરચના Curl_peer *dest;
  સંરચના Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned અક્ષર version;
  BIT(resolve_local);
  BIT(socks4a);
};

#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            અક્ષર *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  માટે(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    જો(timeout_ms < 0) {
      /* we already got the timeout */
      પાછા CURLE_OPERATION_TIMEDOUT;
    }
    જો(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    જો(SOCKET_READABLE(sock, timeout_ms) <= 0)
      પાછા CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    જો(result == CURLE_AGAIN)
      ચાલુ;
    નહીંતર જો(result)
      પાછા result;

    જો(blen == nread) {
      *pnread += nread;
      પાછા CURLE_OK;
    }
    જો(!nread) /* EOF */
      પાછા CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#નહીંતર
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
સ્થિર શૂન્ય socksstate(સંરચના socks_ctx *sx,
                       સંરચના Curl_cfilter *cf,
                       સંરચના Curl_easy *data,
                       ગણના socks_state_t state
#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , પૂર્ણાંક lineno
#endif
)
{
  ગણના socks_state_t oldstate = sx->state;

  જો(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    પાછા;

  sx->state = state;

#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#નહીંતર
  (શૂન્ય)cf;
  (શૂન્ય)data;
#endif
}

સ્થિર CURLproxycode socks_failed(સંરચના socks_ctx *sx,
                                  સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  પાછા presult;
}

સ્થિર CURLproxycode socks_flush(સંરચના socks_ctx *sx,
                                 સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  જ્યારે(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, ખાલી, 0,
                               &nwritten);
    જો(result == CURLE_AGAIN)
      પાછા CURLPX_OK;
    નહીંતર જો(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      પાછા socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks_recv(સંરચના socks_ctx *sx,
                                સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  જ્યારે(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    જો(result == CURLE_AGAIN)
      પાછા CURLPX_OK;
    નહીંતર જો(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      પાછા CURLPX_RECV_CONNECT;
    }
    નહીંતર જો(!nread) { /* EOF */
      જો(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        પાછા CURLPX_RECV_CONNECT;
      }
      તોડો;
    }
  }
  *done = TRUE;
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks4_req_add_hd(સંરચના socks_ctx *sx,
                                       સંરચના Curl_easy *data)
{
  unsigned અક્ષર buf[4];
  size_t nwritten;
  CURLcode result;

  (શૂન્ય)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned અક્ષર)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned અક્ષર)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  જો(result || (nwritten != 4))
    પાછા CURLPX_SEND_REQUEST;
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks4_req_add_user(સંરચના socks_ctx *sx,
                                         સંરચના Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  જો(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    જો(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      પાછા CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    જો(result || (nwritten != (plen + 1)))
      પાછા CURLPX_SEND_REQUEST;
  }
  નહીંતર {
    /* empty username */
    unsigned અક્ષર b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    જો(result || (nwritten != 1))
      પાછા CURLPX_SEND_REQUEST;
  }
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks4_resolving(સંરચના socks_ctx *sx,
                                      સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *done)
{
  અચલ સંરચના Curl_addrinfo *ai = ખાલી;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  જો(result) {
    જો(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (પૂર્ણાંક)result, sx->dest->hostname, sx->dest->port);
      પાછા CURLPX_RESOLVE_HOST;
    }
    પાછા CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  જો(ai) {
    સંરચના sockaddr_in *saddr_in;
    અક્ષર ipbuf[64];

    Curl_printable_address(ai, ipbuf, કદ(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (સંરચના sockaddr_in *)(શૂન્ય *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned અક્ષર *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    જો(result || (nwritten != 4))
      પાછા CURLPX_SEND_REQUEST;
  }
  નહીંતર {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    પાછા CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks4_check_resp(સંરચના socks_ctx *sx,
                                       સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data)
{
  અચલ unsigned અક્ષર *resp;
  size_t rlen;

  જો(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    પાછા CURLPX_RECV_CONNECT;
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
  જો(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    પાછા CURLPX_BAD_VERSION;
  }

  /* Result */
  પસંદગી(resp[1]) {
  સ્થિતિ 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    પાછા CURLPX_OK;
  સ્થિતિ 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned પૂર્ણાંક)((resp[2] << 8) | resp[3]), resp[1]);
    પાછા CURLPX_REQUEST_FAILED;
  સ્થિતિ 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned પૂર્ણાંક)((resp[2] << 8) | resp[3]), resp[1]);
    પાછા CURLPX_IDENTD;
  સ્થિતિ 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned પૂર્ણાંક)((resp[2] << 8) | resp[3]), resp[1]);
    પાછા CURLPX_IDENTD_DIFFER;
  મૂળભૂત:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned પૂર્ણાંક)((resp[2] << 8) | resp[3]), resp[1]);
    પાછા CURLPX_UNKNOWN_FAIL;
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
સ્થિર CURLproxycode socks4_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના socks_ctx *sx,
                                    સંરચના Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  પસંદગી(sx->state) {
  સ્થિતિ SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  સ્થિતિ SOCKS4_ST_START:
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
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    જો(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned અક્ષર buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      જો(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        પાછા socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      જો(result || (nwritten != 4))
        પાછા socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      જો(presult)
        પાછા socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      જો(result || (nwritten != hlen))
        પાછા socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      જાઓ process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  સ્થિતિ SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    જો(!done)
      પાછા CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  સ્થિતિ SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  સ્થિતિ SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  સ્થિતિ SOCKS_ST_SUCCESS:
    પાછા CURLPX_OK;

  સ્થિતિ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    પાછા sx->presult;

  મૂળભૂત:
    DEBUGASSERT(0);
    પાછા socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

સ્થિર CURLproxycode socks5_req0_init(સંરચના Curl_cfilter *cf,
                                      સંરચના socks_ctx *sx,
                                      સંરચના Curl_easy *data)
{
  અચલ unsigned અક્ષર auth = data->set.socks5auth;
  unsigned અક્ષર req[5]; /* version + len + 3 possible auth methods */
  unsigned અક્ષર nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (શૂન્ય)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  જો(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    પાછા CURLPX_LONG_HOSTNAME;
  }

  જો(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  જો(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  જો(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  જો(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  જો(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  જો(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  જો(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    પાછા CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  જો(result || (nwritten != req_len))
    પાછા CURLPX_SEND_REQUEST;
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks5_check_resp0(સંરચના socks_ctx *sx,
                                        સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data)
{
  અચલ unsigned અક્ષર *resp;
  unsigned અક્ષર auth_mode;
  size_t rlen;

  જો(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    પાછા CURLPX_RECV_CONNECT;
  }

  જો(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    પાછા CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  પસંદગી(auth_mode) {
  સ્થિતિ 0:
    જો(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      પાછા CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    પાછા CURLPX_OK;
  સ્થિતિ 1:
    જો(data->set.socks5auth & CURLAUTH_GSSAPI) {
      જો(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      પાછા CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    પાછા CURLPX_GSSAPI_PERMSG;
  સ્થિતિ 2:
    /* regular name + password authentication */
    જો(data->set.socks5auth & CURLAUTH_BASIC) {
      જો(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      પાછા CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    પાછા CURLPX_NO_AUTH;
  સ્થિતિ 255:
    failf(data, "No authentication method was acceptable.");
    પાછા CURLPX_NO_AUTH;
  મૂળભૂત:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    પાછા CURLPX_UNKNOWN_MODE;
  }
}

સ્થિર CURLproxycode socks5_auth_init(સંરચના Curl_cfilter *cf,
                                      સંરચના socks_ctx *sx,
                                      સંરચના Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned અક્ષર buf[2];
  CURLcode result;

  જો(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    જો(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      પાછા CURLPX_LONG_USER;
    }
    જો(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      પાછા CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned અક્ષર)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  જો(result || (nwritten != 2))
    પાછા CURLPX_SEND_REQUEST;
  જો(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    જો(result || (nwritten != ulen))
      પાછા CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned અક્ષર)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  જો(result || (nwritten != 1))
    પાછા CURLPX_SEND_REQUEST;
  જો(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    જો(result || (nwritten != plen))
      પાછા CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks5_check_auth_resp(સંરચના socks_ctx *sx,
                                            સંરચના Curl_cfilter *cf,
                                            સંરચના Curl_easy *data)
{
  અચલ unsigned અક્ષર *resp;
  unsigned અક્ષર auth_status;
  size_t rlen;

  (શૂન્ય)cf;
  જો(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    પાછા CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  જો(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    પાછા CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks5_req1_init(સંરચના socks_ctx *sx,
                                      સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  unsigned અક્ષર req[5];
  unsigned અક્ષર ipbuf[16];
  અચલ unsigned અક્ષર *destination;
  unsigned અક્ષર desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  જો(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    જો(result || (nwritten != 3))
      પાછા CURLPX_SEND_REQUEST;
    પાછા CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  જો(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    જો(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      પાછા CURLPX_BAD_ADDRESS_TYPE;
  }
  નહીંતર
#endif
  જો(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  નહીંતર {
    અચલ size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (અચલ unsigned અક્ષર *)sx->dest->hostname;
    destlen = (unsigned અક્ષર)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  જો(result || (nwritten != hdlen))
    પાછા CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  જો(result || (nwritten != destlen))
    પાછા CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned અક્ષર)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned અક્ષર)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  જો(result || (nwritten != 2))
    પાછા CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  પાછા CURLPX_OK;
}

સ્થિર CURLproxycode socks5_resolving(સંરચના socks_ctx *sx,
                                      સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *done)
{
  અચલ સંરચના Curl_addrinfo *ai = ખાલી;
  અક્ષર dest[MAX_IPADR_LEN];  /* printable address */
  અચલ unsigned અક્ષર *destination = ખાલી;
  unsigned અક્ષર desttype = 1, destlen = 4;
  unsigned અક્ષર req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  જો(result) {
    જો(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (પૂર્ણાંક)result, sx->dest->hostname, sx->dest->port);
      પાછા CURLPX_RESOLVE_HOST;
    }
    પાછા CURLPX_OK;
  }

#ifdef USE_IPV6
  જો(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  જો(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  જો(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    જાઓ out;
  }

  Curl_printable_address(ai, dest, કદ(dest));

  જો(ai->ai_family == AF_INET) {
    સંરચના sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (સંરચના sockaddr_in *)(શૂન્ય *)ai->ai_addr;
    destination = (અચલ unsigned અક્ષર *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  નહીંતર જો(ai->ai_family == AF_INET6) {
    સંરચના sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (સંરચના sockaddr_in6 *)(શૂન્ય *)ai->ai_addr;
    destination = (અચલ unsigned અક્ષર *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  જો(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    જાઓ out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  જો(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    જાઓ out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  જો(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    જાઓ out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned અક્ષર)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned અક્ષર)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  જો(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    જાઓ out;
  }

out:
  *done = (presult == CURLPX_OK);
  પાછા presult;
}

સ્થિર CURLproxycode socks5_recv_resp1(સંરચના socks_ctx *sx,
                                       સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       bool *done)
{
  અચલ unsigned અક્ષર *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  જો(presult)
    પાછા presult;
  નહીંતર જો(!*done)
    પાછા CURLPX_OK;

  જો(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    પાછા CURLPX_RECV_CONNECT;
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
  જો(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    પાછા CURLPX_BAD_VERSION;
  }
  નહીંતર જો(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    પૂર્ણાંક code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    જો(code < 9) {
      /* RFC 1928 section 6 lists: */
      સ્થિર અચલ CURLproxycode lookup[] = {
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
    પાછા rc;
  }

  /* Calculate real packet size */
  પસંદગી(resp[3]) {
  સ્થિતિ 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    તોડો;
  સ્થિતિ 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    તોડો;
  સ્થિતિ 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    તોડો;
  મૂળભૂત:
    failf(data, "SOCKS5 reply has wrong address type.");
    પાછા CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  જો(presult)
    પાછા presult;
  નહીંતર જો(!*done)
    પાછા CURLPX_OK;

  જો(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    પાછા CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  પાછા CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
સ્થિર CURLproxycode socks5_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના socks_ctx *sx,
                                    સંરચના Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  પસંદગી(sx->state) {
  સ્થિતિ SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    જાઓ process_state;

  સ્થિતિ SOCKS5_ST_GSSAPI_INIT: {
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    જો(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      પાછા CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    જાઓ process_state;
#નહીંતર
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    પાછા socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  સ્થિતિ SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    જો(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      જાઓ process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    જો(!done)
      પાછા CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    નહીંતર જો(!done)
      પાછા CURLPX_OK;
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    જો(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      પાછા CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  સ્થિતિ SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    જો(presult)
      પાછા socks_failed(sx, cf, data, presult);
    જો(!done)
      પાછા CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  સ્થિતિ SOCKS_ST_SUCCESS:
    પાછા CURLPX_OK;

  સ્થિતિ SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    પાછા sx->presult;

  મૂળભૂત:
    DEBUGASSERT(0);
    પાછા socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

સ્થિર શૂન્ય socks_proxy_ctx_free(સંરચના socks_ctx *ctx)
{
  જો(ctx) {
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
સ્થિર CURLcode socks_proxy_cf_connect(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       bool *done)
{
  સંરચના socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  જો(result || !*done)
    પાછા result;

  પસંદગી(ctx->proxy_type) {
  સ્થિતિ CURLPROXY_SOCKS5:
  સ્થિતિ CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    તોડો;

  સ્થિતિ CURLPROXY_SOCKS4:
  સ્થિતિ CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    તોડો;

  મૂળભૂત:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    જાઓ out;
  }

  જો(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    જાઓ out;
  }
  નહીંતર જો(ctx->state != SOCKS_ST_SUCCESS)
    જાઓ out;

#ifdef CURLVERBOSE
  જો(Curl_trc_is_verbose(data)) {
    સંરચના ip_quadruple ipquad;
    bool is_ipv6;
    જો(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    નહીંતર
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  જો(*done || result)
    Curl_creds_unlink(&ctx->creds);
  પાછા result;
}

સ્થિર CURLcode socks_cf_adjust_pollset(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના easy_pollset *ps)
{
  સંરચના socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    પસંદગી(sx->state) {
    સ્થિતિ SOCKS4_ST_SEND:
    સ્થિતિ SOCKS5_ST_REQ0_SEND:
    સ્થિતિ SOCKS5_ST_AUTH_SEND:
    સ્થિતિ SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (પૂર્ણાંક)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      તોડો;
    મૂળભૂત:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (પૂર્ણાંક)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      તોડો;
    }
  }
  પાછા result;
}

સ્થિર શૂન્ય socks_proxy_cf_destroy(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  (શૂન્ય)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ખાલી;
}

સ્થિર CURLcode socks_cf_query(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના socks_ctx *sx = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_HOST_PORT:
    જો(sx) {
      *pres1 = sx->dest->port;
      *((અચલ અક્ષર **)pres2) = sx->dest->hostname;
      પાછા CURLE_OK;
    }
    તોડો;
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ખાલી;
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સંરચના Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                          સંરચના Curl_easy *data,
                                          સંરચના Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          સંરચના Curl_creds *creds)
{
  સંરચના Curl_cfilter *cf;
  સંરચના socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  જો(!dest)
    પાછા CURLE_FAILED_INIT;

  પસંદગી(proxy_type) {
  સ્થિતિ CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    તોડો;
  સ્થિતિ CURLPROXY_SOCKS5_HOSTNAME:
    તોડો;
  સ્થિતિ CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    તોડો;
  સ્થિતિ CURLPROXY_SOCKS4A:
    તોડો;
  મૂળભૂત:
    failf(data, "unknown proxytype %d option given", proxy_type);
    પાછા CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    પાછા CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  જો(!result) {
    Curl_conn_cf_ઉમેરો_after(cf_at, cf);
    જો(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  નહીંતર
    socks_proxy_ctx_free(ctx);
  પાછા result;
}

#endif /* CURL_DISABLE_PROXY */
