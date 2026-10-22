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
স্থির ধ্রুবক অক্ষর * ধ্রুবক cf_socks_statename[] = {
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

#define SOCKS_CHUNK_আকার    1024
#define SOCKS_CHUNKS        1

কাঠামো socks_ctx {
  গণনা socks_state_t state;
  কাঠামো bufq iobuf;
  কাঠামো Curl_peer *dest;
  কাঠামো Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned অক্ষর version;
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
CURLcode Curl_blockread_all(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            অক্ষর *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode ফলাফল;

  *pnread = 0;
  জন্য(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(তথ্য);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, তথ্য);

    যদি(timeout_ms < 0) {
      /* we already got the timeout */
      ফেরত CURLE_OPERATION_TIMEDOUT;
    }
    যদি(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    যদি(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ফেরত CURLE_OPERATION_TIMEDOUT;
    ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য, buf, blen, &nread);
    যদি(ফলাফল == CURLE_AGAIN)
      চালু;
    অন্যথা যদি(ফলাফল)
      ফেরত ফলাফল;

    যদি(blen == nread) {
      *pnread += nread;
      ফেরত CURLE_OK;
    }
    যদি(!nread) /* EOF */
      ফেরত CURLE_RECV_ERROR;

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
স্থির শূন্য socksstate(কাঠামো socks_ctx *sx,
                       কাঠামো Curl_cfilter *cf,
                       কাঠামো Curl_easy *তথ্য,
                       গণনা socks_state_t state
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , পূর্ণসংখ্যা lineno
#endif
)
{
  গণনা socks_state_t oldstate = sx->state;

  যদি(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ফেরত;

  sx->state = state;

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(তথ্য, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#অন্যথা
  (শূন্য)cf;
  (শূন্য)তথ্য;
#endif
}

স্থির CURLproxycode socks_failed(কাঠামো socks_ctx *sx,
                                  কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, তথ্য, SOCKS_ST_FAILED);
  sx->presult = presult;
  ফেরত presult;
}

স্থির CURLproxycode socks_flush(কাঠামো socks_ctx *sx,
                                 কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool *done)
{
  CURLcode ফলাফল;
  size_t nwritten;

  *done = FALSE;
  যখন(!Curl_bufq_is_empty(&sx->iobuf)) {
    ফলাফল = Curl_cf_send_bufq(cf->পরবর্তী, তথ্য, &sx->iobuf, ফাঁকা, 0,
                               &nwritten);
    যদি(ফলাফল == CURLE_AGAIN)
      ফেরত CURLPX_OK;
    অন্যথা যদি(ফলাফল) {
      failf(তথ্য, "Failed to send SOCKS request: %s",
            curl_easy_strerror(ফলাফল));
      ফেরত socks_failed(sx, cf, তথ্য, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks_recv(কাঠামো socks_ctx *sx,
                                কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode ফলাফল;
  size_t nread;

  *done = FALSE;
  যখন(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    যদি(ফলাফল == CURLE_AGAIN)
      ফেরত CURLPX_OK;
    অন্যথা যদি(ফলাফল) {
      failf(তথ্য, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(ফলাফল));
      ফেরত CURLPX_RECV_CONNECT;
    }
    অন্যথা যদি(!nread) { /* EOF */
      যদি(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(তথ্য, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ফেরত CURLPX_RECV_CONNECT;
      }
      বিরতি;
    }
  }
  *done = TRUE;
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks4_req_add_hd(কাঠামো socks_ctx *sx,
                                       কাঠামো Curl_easy *তথ্য)
{
  unsigned অক্ষর buf[4];
  size_t nwritten;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned অক্ষর)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned অক্ষর)(sx->dest->port & 0xffU);        /* LSB */

  ফলাফল = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  যদি(ফলাফল || (nwritten != 4))
    ফেরত CURLPX_SEND_REQUEST;
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks4_req_add_user(কাঠামো socks_ctx *sx,
                                         কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  size_t nwritten;

  যদি(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    যদি(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(তথ্য, "Too long SOCKS proxy username");
      ফেরত CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    ফলাফল = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    যদি(ফলাফল || (nwritten != (plen + 1)))
      ফেরত CURLPX_SEND_REQUEST;
  }
  অন্যথা {
    /* empty username */
    unsigned অক্ষর b = 0;
    ফলাফল = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    যদি(ফলাফল || (nwritten != 1))
      ফেরত CURLPX_SEND_REQUEST;
  }
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks4_resolving(কাঠামো socks_ctx *sx,
                                      কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *done)
{
  ধ্রুবক কাঠামো Curl_addrinfo *ai = ফাঁকা;
  CURLcode ফলাফল;
  size_t nwritten;

  *done = FALSE;
  ফলাফল = Curl_conn_dns_addr_ফলাফল(cf->conn, cf->sockindex, sx->dest);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_AGAIN) {
      failf(তথ্য, "error %d resolving SOCKS destination %s:%u",
            (পূর্ণসংখ্যা)ফলাফল, sx->dest->hostname, sx->dest->port);
      ফেরত CURLPX_RESOLVE_HOST;
    }
    ফেরত CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(তথ্য, sx->dest, cf->sockindex, AF_INET, 0);
  যদি(ai) {
    কাঠামো sockaddr_in *saddr_in;
    অক্ষর ipbuf[64];

    Curl_printable_address(ai, ipbuf, আকার(ipbuf));
    CURL_TRC_CF(তথ্য, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (কাঠামো sockaddr_in *)(শূন্য *)ai->ai_addr;
    ফলাফল = Curl_bufq_write(&sx->iobuf,
                             (unsigned অক্ষর *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    যদি(ফলাফল || (nwritten != 4))
      ফেরত CURLPX_SEND_REQUEST;
  }
  অন্যথা {
    /* No IPv4 address resolved */
    failf(তথ্য, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ফেরত CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks4_check_resp(কাঠামো socks_ctx *sx,
                                       কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক unsigned অক্ষর *resp;
  size_t rlen;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(তথ্য, "SOCKS4 reply is incomplete.");
    ফেরত CURLPX_RECV_CONNECT;
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
    failf(তথ্য, "SOCKS4 reply has wrong version, version should be 0.");
    ফেরত CURLPX_BAD_VERSION;
  }

  /*ফলাফল*/
  নির্বাচন(resp[1]) {
  ক্ষেত্র 90:
    CURL_TRC_CF(তথ্য, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ফেরত CURLPX_OK;
  ক্ষেত্র 91:
    failf(তথ্য,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণসংখ্যা)((resp[2] << 8) | resp[3]), resp[1]);
    ফেরত CURLPX_REQUEST_FAILED;
  ক্ষেত্র 92:
    failf(তথ্য,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণসংখ্যা)((resp[2] << 8) | resp[3]), resp[1]);
    ফেরত CURLPX_IDENTD;
  ক্ষেত্র 93:
    failf(তথ্য,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণসংখ্যা)((resp[2] << 8) | resp[3]), resp[1]);
    ফেরত CURLPX_IDENTD_DIFFER;
  পূর্বনির্ধারিত:
    failf(তথ্য,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned পূর্ণসংখ্যা)((resp[2] << 8) | resp[3]), resp[1]);
    ফেরত CURLPX_UNKNOWN_FAIL;
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
স্থির CURLproxycode socks4_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো socks_ctx *sx,
                                    কাঠামো Curl_easy *তথ্য)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode ফলাফল;
  bool done;

process_state:
  নির্বাচন(sx->state) {
  ক্ষেত্র SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, তথ্য, SOCKS4_ST_START);
    FALLTHROUGH();

  ক্ষেত্র SOCKS4_ST_START:
    Curl_bufq_reset(&sx->iobuf);
    sx->socks4a = (sx->proxy_type == CURLPROXY_SOCKS4A);
    sx->presult = CURLPX_OK;

    /* SOCKS4 can only do IPv4, insist! */
    sx->ip_version = CURL_IPRESOLVE_V4;
    CURL_TRC_CF(তথ্য, cf, "SOCKS4%s connecting to %s:%u",
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
    presult = socks4_req_add_hd(sx, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    যদি(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned অক্ষর buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      যদি(hlen > 255) {
        failf(তথ্য, "SOCKS4: too long hostname");
        ফেরত socks_failed(sx, cf, তথ্য, CURLPX_LONG_HOSTNAME);
      }
      ফলাফল = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      যদি(ফলাফল || (nwritten != 4))
        ফেরত socks_failed(sx, cf, তথ্য, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, তথ্য);
      যদি(presult)
        ফেরত socks_failed(sx, cf, তথ্য, presult);
      ফলাফল = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      যদি(ফলাফল || (nwritten != hlen))
        ফেরত socks_failed(sx, cf, তথ্য, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, তথ্য, SOCKS4_ST_SEND);
      যাও process_state;
    }
    sxstate(sx, cf, তথ্য, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  ক্ষেত্র SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    যদি(!done)
      ফেরত CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    sxstate(sx, cf, তথ্য, SOCKS4_ST_SEND);
    FALLTHROUGH();

  ক্ষেত্র SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    sxstate(sx, cf, তথ্য, SOCKS4_ST_RECV);
    FALLTHROUGH();

  ক্ষেত্র SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, তথ্য, 8, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    presult = socks4_check_resp(sx, cf, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    sxstate(sx, cf, তথ্য, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ক্ষেত্র SOCKS_ST_SUCCESS:
    ফেরত CURLPX_OK;

  ক্ষেত্র SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ফেরত sx->presult;

  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    ফেরত socks_failed(sx, cf, তথ্য, CURLPX_SEND_REQUEST);
  }
}

স্থির CURLproxycode socks5_req0_init(কাঠামো Curl_cfilter *cf,
                                      কাঠামো socks_ctx *sx,
                                      কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক unsigned অক্ষর auth = তথ্য->set.socks5auth;
  unsigned অক্ষর req[5]; /* version + len + 3 possible auth methods */
  unsigned অক্ষর nauths;
  size_t req_len, nwritten;
  CURLcode ফলাফল;

  (শূন্য)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  যদি(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(তথ্য, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ফেরত CURLPX_LONG_HOSTNAME;
  }

  যদি(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(তথ্য, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  যদি(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  যদি(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  যদি(!তথ্য->set.socks5_auth_only) {
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
    failf(তথ্য, "SOCKS5: no acceptable authentication method is available.");
    ফেরত CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  ফলাফল = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  যদি(ফলাফল || (nwritten != req_len))
    ফেরত CURLPX_SEND_REQUEST;
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks5_check_resp0(কাঠামো socks_ctx *sx,
                                        কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক unsigned অক্ষর *resp;
  unsigned অক্ষর auth_mode;
  size_t rlen;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(তথ্য, "SOCKS5 initial reply is incomplete.");
    ফেরত CURLPX_RECV_CONNECT;
  }

  যদি(resp[0] != 5) {
    failf(তথ্য, "Received invalid version in initial SOCKS5 response.");
    ফেরত CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  নির্বাচন(auth_mode) {
  ক্ষেত্র 0:
    যদি(তথ্য->set.socks5_auth_only) {
      failf(তথ্য, "SOCKS5 proxy selected no authentication.");
      ফেরত CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ1_INIT);
    ফেরত CURLPX_OK;
  ক্ষেত্র 1:
    যদি(তথ্য->set.socks5auth & CURLAUTH_GSSAPI) {
      যদি(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, তথ্য, SOCKS5_ST_GSSAPI_INIT);
      ফেরত CURLPX_OK;
    }
    failf(তথ্য, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ফেরত CURLPX_GSSAPI_PERMSG;
  ক্ষেত্র 2:
    /* regular name + password authentication */
    যদি(তথ্য->set.socks5auth & CURLAUTH_BASIC) {
      যদি(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, তথ্য, SOCKS5_ST_AUTH_INIT);
      ফেরত CURLPX_OK;
    }
    failf(তথ্য, "BASIC authentication proposed but not enabled.");
    ফেরত CURLPX_NO_AUTH;
  ক্ষেত্র 255:
    failf(তথ্য, "No authentication method was acceptable.");
    ফেরত CURLPX_NO_AUTH;
  পূর্বনির্ধারিত:
    failf(তথ্য, "Unknown SOCKS5 mode attempted to be used by server.");
    ফেরত CURLPX_UNKNOWN_MODE;
  }
}

স্থির CURLproxycode socks5_auth_init(কাঠামো Curl_cfilter *cf,
                                      কাঠামো socks_ctx *sx,
                                      কাঠামো Curl_easy *তথ্য)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned অক্ষর buf[2];
  CURLcode ফলাফল;

  যদি(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    যদি(ulen > 255) {
      failf(তথ্য, "Excessive username length for proxy auth");
      ফেরত CURLPX_LONG_USER;
    }
    যদি(plen > 255) {
      failf(তথ্য, "Excessive password length for proxy auth");
      ফেরত CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned অক্ষর)ulen;
  ফলাফল = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  যদি(ফলাফল || (nwritten != 2))
    ফেরত CURLPX_SEND_REQUEST;
  যদি(ulen) {
    ফলাফল = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    যদি(ফলাফল || (nwritten != ulen))
      ফেরত CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned অক্ষর)plen;
  ফলাফল = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  যদি(ফলাফল || (nwritten != 1))
    ফেরত CURLPX_SEND_REQUEST;
  যদি(plen) {
    ফলাফল = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    যদি(ফলাফল || (nwritten != plen))
      ফেরত CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, তথ্য, SOCKS5_ST_AUTH_SEND);
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks5_check_auth_resp(কাঠামো socks_ctx *sx,
                                            কাঠামো Curl_cfilter *cf,
                                            কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক unsigned অক্ষর *resp;
  unsigned অক্ষর auth_status;
  size_t rlen;

  (শূন্য)cf;
  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(তথ্য, "SOCKS5 sub-negotiation response incomplete.");
    ফেরত CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  যদি(auth_status) {
    failf(তথ্য, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ফেরত CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks5_req1_init(কাঠামো socks_ctx *sx,
                                      কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  unsigned অক্ষর req[5];
  unsigned অক্ষর ipbuf[16];
  ধ্রুবক unsigned অক্ষর *destination;
  unsigned অক্ষর desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode ফলাফল;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  যদি(sx->resolve_local) {
    /* rest of request is added after resolving */
    ফলাফল = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    যদি(ফলাফল || (nwritten != 3))
      ফেরত CURLPX_SEND_REQUEST;
    ফেরত CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  যদি(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    যদি(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ফেরত CURLPX_BAD_ADDRESS_TYPE;
  }
  অন্যথা
#endif
  যদি(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  অন্যথা {
    ধ্রুবক size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (ধ্রুবক unsigned অক্ষর *)sx->dest->hostname;
    destlen = (unsigned অক্ষর)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  ফলাফল = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  যদি(ফলাফল || (nwritten != hdlen))
    ফেরত CURLPX_SEND_REQUEST;
  ফলাফল = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  যদি(ফলাফল || (nwritten != destlen))
    ফেরত CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned অক্ষর)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned অক্ষর)(sx->dest->port & 0xff);
  ফলাফল = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  যদি(ফলাফল || (nwritten != 2))
    ফেরত CURLPX_SEND_REQUEST;
  CURL_TRC_CF(তথ্য, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  ফেরত CURLPX_OK;
}

স্থির CURLproxycode socks5_resolving(কাঠামো socks_ctx *sx,
                                      কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *done)
{
  ধ্রুবক কাঠামো Curl_addrinfo *ai = ফাঁকা;
  অক্ষর dest[MAX_IPADR_LEN];  /* printable address */
  ধ্রুবক unsigned অক্ষর *destination = ফাঁকা;
  unsigned অক্ষর desttype = 1, destlen = 4;
  unsigned অক্ষর req[2];
  CURLcode ফলাফল;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  ফলাফল = Curl_conn_dns_addr_ফলাফল(cf->conn, cf->sockindex, sx->dest);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_AGAIN) {
      failf(তথ্য, "error %d resolving SOCKS destination %s:%u",
            (পূর্ণসংখ্যা)ফলাফল, sx->dest->hostname, sx->dest->port);
      ফেরত CURLPX_RESOLVE_HOST;
    }
    ফেরত CURLPX_OK;
  }

#ifdef USE_IPV6
  যদি(তথ্য->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(তথ্য, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  যদি(!ai)
    ai = Curl_conn_dns_get_ai(তথ্য, sx->dest, cf->sockindex, AF_INET, 0);

  যদি(!ai) {
    failf(তথ্য, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    যাও out;
  }

  Curl_printable_address(ai, dest, আকার(dest));

  যদি(ai->ai_family == AF_INET) {
    কাঠামো sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (কাঠামো sockaddr_in *)(শূন্য *)ai->ai_addr;
    destination = (ধ্রুবক unsigned অক্ষর *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(তথ্য, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  অন্যথা যদি(ai->ai_family == AF_INET6) {
    কাঠামো sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (কাঠামো sockaddr_in6 *)(শূন্য *)ai->ai_addr;
    destination = (ধ্রুবক unsigned অক্ষর *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(তথ্য, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  যদি(!destination) {
    failf(তথ্য, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    যাও out;
  }

  req[0] = desttype;
  ফলাফল = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  যদি(ফলাফল || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    যাও out;
  }
  ফলাফল = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  যদি(ফলাফল || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    যাও out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned অক্ষর)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned অক্ষর)(sx->dest->port & 0xffU);
  ফলাফল = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  যদি(ফলাফল || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    যাও out;
  }

out:
  *done = (presult == CURLPX_OK);
  ফেরত presult;
}

স্থির CURLproxycode socks5_recv_resp1(কাঠামো socks_ctx *sx,
                                       কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       bool *done)
{
  ধ্রুবক unsigned অক্ষর *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, তথ্য, resp_len, done);
  যদি(presult)
    ফেরত presult;
  অন্যথা যদি(!*done)
    ফেরত CURLPX_OK;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(তথ্য, "SOCKS5 response is incomplete.");
    ফেরত CURLPX_RECV_CONNECT;
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
    failf(তথ্য, "SOCKS5 reply has wrong version, version should be 5.");
    ফেরত CURLPX_BAD_VERSION;
  }
  অন্যথা যদি(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    পূর্ণসংখ্যা code = resp[1];
    failf(তথ্য, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    যদি(code < 9) {
      /* RFC 1928 section 6 lists: */
      স্থির ধ্রুবক CURLproxycode lookup[] = {
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
    ফেরত rc;
  }

  /* Calculate real packet size */
  নির্বাচন(resp[3]) {
  ক্ষেত্র 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    বিরতি;
  ক্ষেত্র 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    বিরতি;
  ক্ষেত্র 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "SOCKS5 reply has wrong address type.");
    ফেরত CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, তথ্য, resp_len, done);
  যদি(presult)
    ফেরত presult;
  অন্যথা যদি(!*done)
    ফেরত CURLPX_OK;

  যদি(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(তথ্য, "SOCKS5 response is incomplete.");
    ফেরত CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  ফেরত CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
স্থির CURLproxycode socks5_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো socks_ctx *sx,
                                    কাঠামো Curl_easy *তথ্য)
{
  CURLproxycode presult;
  bool done;

process_state:
  নির্বাচন(sx->state) {
  ক্ষেত্র SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, তথ্য, SOCKS5_ST_START);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_START:
    CURL_TRC_CF(তথ্য, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, তথ্য, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, তথ্য, 2, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    /* socks5_check_resp0() sets next socks state */
    যাও process_state;

  ক্ষেত্র SOCKS5_ST_GSSAPI_INIT: {
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode ফলাফল = Curl_SOCKS5_gssapi_negotiate(cf, তথ্য, sx->creds);
    যদি(ফলাফল) {
      failf(তথ্য, "Unable to negotiate SOCKS5 GSS-API context.");
      ফেরত CURLPX_GSSAPI;
    }
    sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ1_INIT);
    যাও process_state;
#অন্যথা
    failf(তথ্য, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ফেরত socks_failed(sx, cf, তথ্য, CURLPX_GSSAPI_PERMSG);
#endif
  }

  ক্ষেত্র SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    sxstate(sx, cf, তথ্য, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    sxstate(sx, cf, তথ্য, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, তথ্য, 2, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, তথ্য);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    যদি(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ1_SEND);
      যাও process_state;
    }
    sxstate(sx, cf, তথ্য, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    যদি(!done)
      ফেরত CURLPX_OK;
    sxstate(sx, cf, তথ্য, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    অন্যথা যদি(!done)
      ফেরত CURLPX_OK;
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    যদি(cf->conn->socks5_gssapi_enctype) {
      failf(তথ্য, "SOCKS5 GSS-API protection not yet implemented.");
      ফেরত CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, তথ্য, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  ক্ষেত্র SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, তথ্য, &done);
    যদি(presult)
      ফেরত socks_failed(sx, cf, তথ্য, presult);
    যদি(!done)
      ফেরত CURLPX_OK;
    CURL_TRC_CF(তথ্য, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, তথ্য, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  ক্ষেত্র SOCKS_ST_SUCCESS:
    ফেরত CURLPX_OK;

  ক্ষেত্র SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ফেরত sx->presult;

  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    ফেরত socks_failed(sx, cf, তথ্য, CURLPX_SEND_REQUEST);
  }
}

স্থির শূন্য socks_proxy_ctx_free(কাঠামো socks_ctx *ctx)
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
স্থির CURLcode socks_proxy_cf_connect(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       bool *done)
{
  কাঠামো socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode ফলাফল;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
  যদি(ফলাফল || !*done)
    ফেরত ফলাফল;

  নির্বাচন(ctx->proxy_type) {
  ক্ষেত্র CURLPROXY_SOCKS5:
  ক্ষেত্র CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, তথ্য);
    বিরতি;

  ক্ষেত্র CURLPROXY_SOCKS4:
  ক্ষেত্র CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, তথ্য);
    বিরতি;

  পূর্বনির্ধারিত:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    ফলাফল = CURLE_COULDNT_CONNECT;
    যাও out;
  }

  যদি(pxresult) {
    ফলাফল = CURLE_PROXY;
    তথ্য->info.pxcode = (uint8_t)pxresult;
    যাও out;
  }
  অন্যথা যদি(ctx->state != SOCKS_ST_SUCCESS)
    যাও out;

#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(তথ্য)) {
    কাঠামো ip_quadruple ipquad;
    bool is_ipv6;
    যদি(!Curl_conn_cf_get_ip_info(cf->পরবর্তী, তথ্য, &is_ipv6, &ipquad))
      infof(তথ্য, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    অন্যথা
      infof(তথ্য, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  যদি(*done || ফলাফল)
    Curl_creds_unlink(&ctx->creds);
  ফেরত ফলাফল;
}

স্থির CURLcode socks_cf_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো easy_pollset *ps)
{
  কাঠামো socks_ctx *sx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, তথ্য);
    নির্বাচন(sx->state) {
    ক্ষেত্র SOCKS4_ST_SEND:
    ক্ষেত্র SOCKS5_ST_REQ0_SEND:
    ক্ষেত্র SOCKS5_ST_AUTH_SEND:
    ক্ষেত্র SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(তথ্য, cf, "adjust pollset out (%d)", (পূর্ণসংখ্যা)sx->state);
      ফলাফল = Curl_pollset_set_out_only(তথ্য, ps, sock);
      বিরতি;
    পূর্বনির্ধারিত:
      CURL_TRC_CF(তথ্য, cf, "adjust pollset in (%d)", (পূর্ণসংখ্যা)sx->state);
      ফলাফল = Curl_pollset_set_in_only(তথ্য, ps, sock);
      বিরতি;
    }
  }
  ফেরত ফলাফল;
}

স্থির শূন্য socks_proxy_cf_destroy(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)তথ্য;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = ফাঁকা;
}

স্থির CURLcode socks_cf_query(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো socks_ctx *sx = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_HOST_PORT:
    যদি(sx) {
      *pres1 = sx->dest->port;
      *((ধ্রুবক অক্ষর **)pres2) = sx->dest->hostname;
      ফেরত CURLE_OK;
    }
    বিরতি;
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

কাঠামো Curl_cftype Curl_cft_socks_proxy = {
  "SOCKS",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  socks_proxy_cf_destroy,
  socks_proxy_cf_connect,
  Curl_cf_def_shutdown,
  socks_cf_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  socks_cf_query,
};

CURLcode Curl_cf_socks_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                          কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          কাঠামো Curl_creds *creds)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(তথ্য, ip_version);
  CURLcode ফলাফল;

  যদি(!dest)
    ফেরত CURLE_FAILED_INIT;

  নির্বাচন(proxy_type) {
  ক্ষেত্র CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    বিরতি;
  ক্ষেত্র CURLPROXY_SOCKS5_HOSTNAME:
    বিরতি;
  ক্ষেত্র CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    বিরতি;
  ক্ষেত্র CURLPROXY_SOCKS4A:
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "unknown proxytype %d option given", proxy_type);
    ফেরত CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_আকার, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  যদি(!ফলাফল) {
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
    যদি(ctx->resolve_local) {
      ফলাফল = Curl_conn_dns_add_addr_resolve(তথ্য, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  অন্যথা
    socks_proxy_ctx_free(ctx);
  ফেরত ফলাফল;
}

#endif /* CURL_DISABLE_PROXY */
