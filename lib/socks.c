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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "socks.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
ଗଣନା socks_state_t {
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

#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ cf_socks_statename[] = {
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

ସଂରଚନା socks_ctx {
  ଗଣନା socks_state_t state;
  ସଂରଚନା bufq iobuf;
  ସଂରଚନା Curl_peer *dest;
  ସଂରଚନା Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned ଅକ୍ଷର version;
  BIT(resolve_local);
  BIT(socks4a);
};

#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ଅକ୍ଷର *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  ପାଇଁ(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    ଯଦି(timeout_ms < 0) {
      /* we already got the timeout */
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
    }
    ଯଦି(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ଯଦି(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    ଯଦି(result == CURLE_AGAIN)
      ଜାରି;
    ଅନ୍ୟଥା ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯଦି(blen == nread) {
      *pnread += nread;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଯଦି(!nread) /* EOF */
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ଅନ୍ୟଥା
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
ସ୍ଥିର ଶୂନ୍ୟ socksstate(ସଂରଚନା socks_ctx *sx,
                       ସଂରଚନା Curl_cfilter *cf,
                       ସଂରଚନା Curl_easy *data,
                       ଗଣନା socks_state_t state
#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno
#endif
)
{
  ଗଣନା socks_state_t oldstate = sx->state;

  ଯଦି(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ଫେରନ୍ତୁ;

  sx->state = state;

#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
#endif
}

ସ୍ଥିର CURLproxycode socks_failed(ସଂରଚନା socks_ctx *sx,
                                  ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  ଫେରନ୍ତୁ presult;
}

ସ୍ଥିର CURLproxycode socks_flush(ସଂରଚନା socks_ctx *sx,
                                 ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  ଯେତେବେଳେ(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, ଖାଲି, 0,
                               &nwritten);
    ଯଦି(result == CURLE_AGAIN)
      ଫେରନ୍ତୁ CURLPX_OK;
    ଅନ୍ୟଥା ଯଦି(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks_recv(ସଂରଚନା socks_ctx *sx,
                                ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  ଯେତେବେଳେ(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ଯଦି(result == CURLE_AGAIN)
      ଫେରନ୍ତୁ CURLPX_OK;
    ଅନ୍ୟଥା ଯଦି(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
    }
    ଅନ୍ୟଥା ଯଦି(!nread) { /* EOF */
      ଯଦି(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
      }
      ଭାଙ୍ଗ;
    }
  }
  *done = TRUE;
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks4_req_add_hd(ସଂରଚନା socks_ctx *sx,
                                       ସଂରଚନା Curl_easy *data)
{
  unsigned ଅକ୍ଷର buf[4];
  size_t nwritten;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned ଅକ୍ଷର)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned ଅକ୍ଷର)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ଯଦି(result || (nwritten != 4))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks4_req_add_user(ସଂରଚନା socks_ctx *sx,
                                         ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  ଯଦି(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    ଯଦି(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      ଫେରନ୍ତୁ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ଯଦି(result || (nwritten != (plen + 1)))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  }
  ଅନ୍ୟଥା {
    /* empty username */
    unsigned ଅକ୍ଷର b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ଯଦି(result || (nwritten != 1))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  }
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks4_resolving(ସଂରଚନା socks_ctx *sx,
                                      ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *done)
{
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai = ଖାଲି;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ଯଦି(result) {
    ଯଦି(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, sx->dest->hostname, sx->dest->port);
      ଫେରନ୍ତୁ CURLPX_RESOLVE_HOST;
    }
    ଫେରନ୍ତୁ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  ଯଦି(ai) {
    ସଂରଚନା sockaddr_in *saddr_in;
    ଅକ୍ଷର ipbuf[64];

    Curl_printable_address(ai, ipbuf, ଆକାର(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (ସଂରଚନା sockaddr_in *)(ଶୂନ୍ୟ *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned ଅକ୍ଷର *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    ଯଦି(result || (nwritten != 4))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  }
  ଅନ୍ୟଥା {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ଫେରନ୍ତୁ CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks4_check_resp(ସଂରଚନା socks_ctx *sx,
                                       ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ unsigned ଅକ୍ଷର *resp;
  size_t rlen;

  ଯଦି(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
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
  ଯଦି(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    ଫେରନ୍ତୁ CURLPX_BAD_VERSION;
  }

  /* Result */
  ବାଛ(resp[1]) {
  ସ୍ଥିତି 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ଫେରନ୍ତୁ CURLPX_OK;
  ସ୍ଥିତି 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((resp[2] << 8) | resp[3]), resp[1]);
    ଫେରନ୍ତୁ CURLPX_REQUEST_FAILED;
  ସ୍ଥିତି 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((resp[2] << 8) | resp[3]), resp[1]);
    ଫେରନ୍ତୁ CURLPX_IDENTD;
  ସ୍ଥିତି 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((resp[2] << 8) | resp[3]), resp[1]);
    ଫେରନ୍ତୁ CURLPX_IDENTD_DIFFER;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((resp[2] << 8) | resp[3]), resp[1]);
    ଫେରନ୍ତୁ CURLPX_UNKNOWN_FAIL;
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
ସ୍ଥିର CURLproxycode socks4_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା socks_ctx *sx,
                                    ସଂରଚନା Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  ବାଛ(sx->state) {
  ସ୍ଥିତି SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS4_ST_START:
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
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ଯଦି(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned ଅକ୍ଷର buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      ଯଦି(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ଯଦି(result || (nwritten != 4))
        ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      ଯଦି(presult)
        ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ଯଦି(result || (nwritten != hlen))
        ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      ଯାଅ process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS_ST_SUCCESS:
    ଫେରନ୍ତୁ CURLPX_OK;

  ସ୍ଥିତି SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ଫେରନ୍ତୁ sx->presult;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ସ୍ଥିର CURLproxycode socks5_req0_init(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା socks_ctx *sx,
                                      ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ unsigned ଅକ୍ଷର auth = data->set.socks5auth;
  unsigned ଅକ୍ଷର req[5]; /* version + len + 3 possible auth methods */
  unsigned ଅକ୍ଷର nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (ଶୂନ୍ୟ)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ଯଦି(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ଫେରନ୍ତୁ CURLPX_LONG_HOSTNAME;
  }

  ଯଦି(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ଯଦି(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ଯଦି(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  ଯଦି(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ଯଦି(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  ଯଦି(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ଯଦି(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    ଫେରନ୍ତୁ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ଯଦି(result || (nwritten != req_len))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks5_check_resp0(ସଂରଚନା socks_ctx *sx,
                                        ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ unsigned ଅକ୍ଷର *resp;
  unsigned ଅକ୍ଷର auth_mode;
  size_t rlen;

  ଯଦି(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
  }

  ଯଦି(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    ଫେରନ୍ତୁ CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  ବାଛ(auth_mode) {
  ସ୍ଥିତି 0:
    ଯଦି(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      ଫେରନ୍ତୁ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    ଫେରନ୍ତୁ CURLPX_OK;
  ସ୍ଥିତି 1:
    ଯଦି(data->set.socks5auth & CURLAUTH_GSSAPI) {
      ଯଦି(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      ଫେରନ୍ତୁ CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ଫେରନ୍ତୁ CURLPX_GSSAPI_PERMSG;
  ସ୍ଥିତି 2:
    /* regular name + password authentication */
    ଯଦି(data->set.socks5auth & CURLAUTH_BASIC) {
      ଯଦି(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      ଫେରନ୍ତୁ CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    ଫେରନ୍ତୁ CURLPX_NO_AUTH;
  ସ୍ଥିତି 255:
    failf(data, "No authentication method was acceptable.");
    ଫେରନ୍ତୁ CURLPX_NO_AUTH;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    ଫେରନ୍ତୁ CURLPX_UNKNOWN_MODE;
  }
}

ସ୍ଥିର CURLproxycode socks5_auth_init(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା socks_ctx *sx,
                                      ସଂରଚନା Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned ଅକ୍ଷର buf[2];
  CURLcode result;

  ଯଦି(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ଯଦି(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      ଫେରନ୍ତୁ CURLPX_LONG_USER;
    }
    ଯଦି(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      ଫେରନ୍ତୁ CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned ଅକ୍ଷର)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ଯଦି(result || (nwritten != 2))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  ଯଦି(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ଯଦି(result || (nwritten != ulen))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned ଅକ୍ଷର)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ଯଦି(result || (nwritten != 1))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  ଯଦି(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ଯଦି(result || (nwritten != plen))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks5_check_auth_resp(ସଂରଚନା socks_ctx *sx,
                                            ସଂରଚନା Curl_cfilter *cf,
                                            ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ unsigned ଅକ୍ଷର *resp;
  unsigned ଅକ୍ଷର auth_status;
  size_t rlen;

  (ଶୂନ୍ୟ)cf;
  ଯଦି(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  ଯଦି(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ଫେରନ୍ତୁ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks5_req1_init(ସଂରଚନା socks_ctx *sx,
                                      ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  unsigned ଅକ୍ଷର req[5];
  unsigned ଅକ୍ଷର ipbuf[16];
  ଅଚଳ unsigned ଅକ୍ଷର *destination;
  unsigned ଅକ୍ଷର desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ଯଦି(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ଯଦି(result || (nwritten != 3))
      ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
    ଫେରନ୍ତୁ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  ଯଦି(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ଯଦି(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ଫେରନ୍ତୁ CURLPX_BAD_ADDRESS_TYPE;
  }
  ଅନ୍ୟଥା
#endif
  ଯଦି(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ଅନ୍ୟଥା {
    ଅଚଳ size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ଅଚଳ unsigned ଅକ୍ଷର *)sx->dest->hostname;
    destlen = (unsigned ଅକ୍ଷର)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ଯଦି(result || (nwritten != hdlen))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ଯଦି(result || (nwritten != destlen))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned ଅକ୍ଷର)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned ଅକ୍ଷର)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ଯଦି(result || (nwritten != 2))
    ଫେରନ୍ତୁ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  ଫେରନ୍ତୁ CURLPX_OK;
}

ସ୍ଥିର CURLproxycode socks5_resolving(ସଂରଚନା socks_ctx *sx,
                                      ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *done)
{
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai = ଖାଲି;
  ଅକ୍ଷର dest[MAX_IPADR_LEN];  /* printable address */
  ଅଚଳ unsigned ଅକ୍ଷର *destination = ଖାଲି;
  unsigned ଅକ୍ଷର desttype = 1, destlen = 4;
  unsigned ଅକ୍ଷର req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ଯଦି(result) {
    ଯଦି(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, sx->dest->hostname, sx->dest->port);
      ଫେରନ୍ତୁ CURLPX_RESOLVE_HOST;
    }
    ଫେରନ୍ତୁ CURLPX_OK;
  }

#ifdef USE_IPV6
  ଯଦି(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  ଯଦି(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  ଯଦି(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    ଯାଅ out;
  }

  Curl_printable_address(ai, dest, ଆକାର(dest));

  ଯଦି(ai->ai_family == AF_INET) {
    ସଂରଚନା sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (ସଂରଚନା sockaddr_in *)(ଶୂନ୍ୟ *)ai->ai_addr;
    destination = (ଅଚଳ unsigned ଅକ୍ଷର *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  ଅନ୍ୟଥା ଯଦି(ai->ai_family == AF_INET6) {
    ସଂରଚନା sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (ସଂରଚନା sockaddr_in6 *)(ଶୂନ୍ୟ *)ai->ai_addr;
    destination = (ଅଚଳ unsigned ଅକ୍ଷର *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  ଯଦି(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    ଯାଅ out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ଯଦି(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    ଯାଅ out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ଯଦି(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    ଯାଅ out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned ଅକ୍ଷର)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned ଅକ୍ଷର)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ଯଦି(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    ଯାଅ out;
  }

out:
  *done = (presult == CURLPX_OK);
  ଫେରନ୍ତୁ presult;
}

ସ୍ଥିର CURLproxycode socks5_recv_resp1(ସଂରଚନା socks_ctx *sx,
                                       ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       bool *done)
{
  ଅଚଳ unsigned ଅକ୍ଷର *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  ଯଦି(presult)
    ଫେରନ୍ତୁ presult;
  ଅନ୍ୟଥା ଯଦି(!*done)
    ଫେରନ୍ତୁ CURLPX_OK;

  ଯଦି(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
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
  ଯଦି(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    ଫେରନ୍ତୁ CURLPX_BAD_VERSION;
  }
  ଅନ୍ୟଥା ଯଦି(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    ଯଦି(code < 9) {
      /* RFC 1928 section 6 lists: */
      ସ୍ଥିର ଅଚଳ CURLproxycode lookup[] = {
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
    ଫେରନ୍ତୁ rc;
  }

  /* Calculate real packet size */
  ବାଛ(resp[3]) {
  ସ୍ଥିତି 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "SOCKS5 reply has wrong address type.");
    ଫେରନ୍ତୁ CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  ଯଦି(presult)
    ଫେରନ୍ତୁ presult;
  ଅନ୍ୟଥା ଯଦି(!*done)
    ଫେରନ୍ତୁ CURLPX_OK;

  ଯଦି(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ଫେରନ୍ତୁ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  ଫେରନ୍ତୁ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
ସ୍ଥିର CURLproxycode socks5_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା socks_ctx *sx,
                                    ସଂରଚନା Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  ବାଛ(sx->state) {
  ସ୍ଥିତି SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    ଯାଅ process_state;

  ସ୍ଥିତି SOCKS5_ST_GSSAPI_INIT: {
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    ଯଦି(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      ଫେରନ୍ତୁ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    ଯାଅ process_state;
#ଅନ୍ୟଥା
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  ସ୍ଥିତି SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଯଦି(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      ଯାଅ process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଅନ୍ୟଥା ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ଯଦି(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      ଫେରନ୍ତୁ CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    ଯଦି(presult)
      ଫେରନ୍ତୁ socks_failed(sx, cf, data, presult);
    ଯଦି(!done)
      ଫେରନ୍ତୁ CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ସ୍ଥିତି SOCKS_ST_SUCCESS:
    ଫେରନ୍ତୁ CURLPX_OK;

  ସ୍ଥିତି SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ଫେରନ୍ତୁ sx->presult;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ socks_proxy_ctx_free(ସଂରଚନା socks_ctx *ctx)
{
  ଯଦି(ctx) {
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
ସ୍ଥିର CURLcode socks_proxy_cf_connect(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       bool *done)
{
  ସଂରଚନା socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ଯଦି(result || !*done)
    ଫେରନ୍ତୁ result;

  ବାଛ(ctx->proxy_type) {
  ସ୍ଥିତି CURLPROXY_SOCKS5:
  ସ୍ଥିତି CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLPROXY_SOCKS4:
  ସ୍ଥିତି CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    ଯାଅ out;
  }

  ଯଦି(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(ctx->state != SOCKS_ST_SUCCESS)
    ଯାଅ out;

#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_is_verbose(data)) {
    ସଂରଚନା ip_quadruple ipquad;
    bool is_ipv6;
    ଯଦି(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    ଅନ୍ୟଥା
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  ଯଦି(*done || result)
    Curl_creds_unlink(&ctx->creds);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode socks_cf_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ବାଛ(sx->state) {
    ସ୍ଥିତି SOCKS4_ST_SEND:
    ସ୍ଥିତି SOCKS5_ST_REQ0_SEND:
    ସ୍ଥିତି SOCKS5_ST_AUTH_SEND:
    ସ୍ଥିତି SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ socks_proxy_cf_destroy(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ଖାଲି;
}

ସ୍ଥିର CURLcode socks_cf_query(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା socks_ctx *sx = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_HOST_PORT:
    ଯଦି(sx) {
      *pres1 = sx->dest->port;
      *((ଅଚଳ ଅକ୍ଷର **)pres2) = sx->dest->hostname;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସଂରଚନା Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                          ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          ସଂରଚନା Curl_creds *creds)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  ଯଦି(!dest)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ବାଛ(proxy_type) {
  ସ୍ଥିତି CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLPROXY_SOCKS5_HOSTNAME:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLPROXY_SOCKS4A:
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "unknown proxytype %d option given", proxy_type);
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ଯଦି(!result) {
    Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
    ଯଦି(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ଅନ୍ୟଥା
    socks_proxy_ctx_free(ctx);
  ଫେରନ୍ତୁ result;
}

#endif /* CURL_DISABLE_PROXY */
