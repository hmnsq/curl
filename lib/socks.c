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
#include "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "urldata.h"
#include "bufq.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "select.h"
#include "cfilters.h"
#include "connect.h"
#include "socks.h"
#include "vdns/cf-dns.h"
#include "curlx/inet_pton.h"

/* for the (SOCKS) connect state machine */
ga_nanaa socks_state_t {
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

#ᮚᮦᮔ᮪ defined(DEBUGBUILD) && defined(CURLVERBOSE)
static const var_na * const cf_socks_statename[] = {
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

sa_nracanaa socks_ctx {
  ga_nanaa socks_state_t state;
  sa_nracanaa bufq iobuf;
  sa_nracanaa Curl_peer *dest;
  sa_nracanaa Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned var_na version;
  BIT(resolve_local);
  BIT(socks4a);
};

#ᮚᮦᮔ᮪ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            var_na *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  for(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    ᮚᮦᮔ᮪(timeout_ms < 0) {
      /* we already got the timeout */
      ᮝᮜᮤ CURLE_OPERATION_TIMEDOUT;
    }
    ᮚᮦᮔ᮪(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    ᮚᮦᮔ᮪(SOCKET_READABLE(sock, timeout_ms) <= 0)
      ᮝᮜᮤ CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    ᮚᮦᮔ᮪(result == CURLE_AGAIN)
      continue;
    ᮜᮤᮚ ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;

    ᮚᮦᮔ᮪(blen == nread) {
      *pnread += nread;
      ᮝᮜᮤ CURLE_OK;
    }
    ᮚᮦᮔ᮪(!nread) /* EOF */
      ᮝᮜᮤ CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#ᮚᮦᮔ᮪ defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#ᮜᮤᮚ
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
static void socksstate(sa_nracanaa socks_ctx *sx,
                       sa_nracanaa Curl_cfilter *cf,
                       sa_nracanaa Curl_easy *data,
                       ga_nanaa socks_state_t state
#ᮚᮦᮔ᮪ defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , puur_naa_nka lineno
#endif
)
{
  ga_nanaa socks_state_t oldstate = sx->state;

  ᮚᮦᮔ᮪(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ᮝᮜᮤ;

  sx->state = state;

#ᮚᮦᮔ᮪ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#ᮜᮤᮚ
  (void)cf;
  (void)data;
#endif
}

static CURLproxycode socks_failed(sa_nracanaa socks_ctx *sx,
                                  sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  ᮝᮜᮤ presult;
}

static CURLproxycode socks_flush(sa_nracanaa socks_ctx *sx,
                                 sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  ᮞᮨᮓᮨᮜ(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, NULL, 0,
                               &nwritten);
    ᮚᮦᮔ᮪(result == CURLE_AGAIN)
      ᮝᮜᮤ CURLPX_OK;
    ᮜᮤᮚ ᮚᮦᮔ᮪(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks_recv(sa_nracanaa socks_ctx *sx,
                                sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  ᮞᮨᮓᮨᮜ(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    ᮚᮦᮔ᮪(result == CURLE_AGAIN)
      ᮝᮜᮤ CURLPX_OK;
    ᮜᮤᮚ ᮚᮦᮔ᮪(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      ᮝᮜᮤ CURLPX_RECV_CONNECT;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(!nread) { /* EOF */
      ᮚᮦᮔ᮪(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        ᮝᮜᮤ CURLPX_RECV_CONNECT;
      }
      bha_nga;
    }
  }
  *done = TRUE;
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks4_req_add_hd(sa_nracanaa socks_ctx *sx,
                                       sa_nracanaa Curl_easy *data)
{
  unsigned var_na buf[4];
  size_t nwritten;
  CURLcode result;

  (void)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned var_na)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned var_na)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 4))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks4_req_add_user(sa_nracanaa socks_ctx *sx,
                                         sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  ᮚᮦᮔ᮪(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    ᮚᮦᮔ᮪(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      ᮝᮜᮤ CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    ᮚᮦᮔ᮪(result || (nwritten != (plen + 1)))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
  }
  ᮜᮤᮚ {
    /* empty username */
    unsigned var_na b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    ᮚᮦᮔ᮪(result || (nwritten != 1))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
  }
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks4_resolving(sa_nracanaa socks_ctx *sx,
                                      sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *done)
{
  const sa_nracanaa Curl_addrinfo *ai = NULL;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (puur_naa_nka)result, sx->dest->hostname, sx->dest->port);
      ᮝᮜᮤ CURLPX_RESOLVE_HOST;
    }
    ᮝᮜᮤ CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  ᮚᮦᮔ᮪(ai) {
    sa_nracanaa sockaddr_in *saddr_in;
    var_na ipbuf[64];

    Curl_printable_address(ai, ipbuf, parimaa_na(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (sa_nracanaa sockaddr_in *)(void *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned var_na *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    ᮚᮦᮔ᮪(result || (nwritten != 4))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
  }
  ᮜᮤᮚ {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    ᮝᮜᮤ CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks4_check_resp(sa_nracanaa socks_ctx *sx,
                                       sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data)
{
  const unsigned var_na *resp;
  size_t rlen;

  ᮚᮦᮔ᮪(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    ᮝᮜᮤ CURLPX_RECV_CONNECT;
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
  ᮚᮦᮔ᮪(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    ᮝᮜᮤ CURLPX_BAD_VERSION;
  }

  /* Result */
  switch(resp[1]) {
  pak_sa 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    ᮝᮜᮤ CURLPX_OK;
  pak_sa 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned puur_naa_nka)((resp[2] << 8) | resp[3]), resp[1]);
    ᮝᮜᮤ CURLPX_REQUEST_FAILED;
  pak_sa 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned puur_naa_nka)((resp[2] << 8) | resp[3]), resp[1]);
    ᮝᮜᮤ CURLPX_IDENTD;
  pak_sa 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned puur_naa_nka)((resp[2] << 8) | resp[3]), resp[1]);
    ᮝᮜᮤ CURLPX_IDENTD_DIFFER;
  default:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned puur_naa_nka)((resp[2] << 8) | resp[3]), resp[1]);
    ᮝᮜᮤ CURLPX_UNKNOWN_FAIL;
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
static CURLproxycode socks4_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa socks_ctx *sx,
                                    sa_nracanaa Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  switch(sx->state) {
  pak_sa SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  pak_sa SOCKS4_ST_START:
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
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    ᮚᮦᮔ᮪(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned var_na buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      ᮚᮦᮔ᮪(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      ᮚᮦᮔ᮪(result || (nwritten != 4))
        ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      ᮚᮦᮔ᮪(presult)
        ᮝᮜᮤ socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      ᮚᮦᮔ᮪(result || (nwritten != hlen))
        ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      goto process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  pak_sa SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  pak_sa SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  pak_sa SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  pak_sa SOCKS_ST_SUCCESS:
    ᮝᮜᮤ CURLPX_OK;

  pak_sa SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ᮝᮜᮤ sx->presult;

  default:
    DEBUGASSERT(0);
    ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

static CURLproxycode socks5_req0_init(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa socks_ctx *sx,
                                      sa_nracanaa Curl_easy *data)
{
  const unsigned var_na auth = data->set.socks5auth;
  unsigned var_na req[5]; /* version + len + 3 possible auth methods */
  unsigned var_na nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (void)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  ᮚᮦᮔ᮪(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    ᮝᮜᮤ CURLPX_LONG_HOSTNAME;
  }

  ᮚᮦᮔ᮪(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  ᮚᮦᮔ᮪(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  ᮚᮦᮔ᮪(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  ᮚᮦᮔ᮪(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#ᮚᮦᮔ᮪ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ᮚᮦᮔ᮪(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  ᮚᮦᮔ᮪(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  ᮚᮦᮔ᮪(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    ᮝᮜᮤ CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != req_len))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks5_check_resp0(sa_nracanaa socks_ctx *sx,
                                        sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data)
{
  const unsigned var_na *resp;
  unsigned var_na auth_mode;
  size_t rlen;

  ᮚᮦᮔ᮪(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    ᮝᮜᮤ CURLPX_RECV_CONNECT;
  }

  ᮚᮦᮔ᮪(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    ᮝᮜᮤ CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  switch(auth_mode) {
  pak_sa 0:
    ᮚᮦᮔ᮪(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      ᮝᮜᮤ CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    ᮝᮜᮤ CURLPX_OK;
  pak_sa 1:
    ᮚᮦᮔ᮪(data->set.socks5auth & CURLAUTH_GSSAPI) {
      ᮚᮦᮔ᮪(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      ᮝᮜᮤ CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    ᮝᮜᮤ CURLPX_GSSAPI_PERMSG;
  pak_sa 2:
    /* regular name + password authentication */
    ᮚᮦᮔ᮪(data->set.socks5auth & CURLAUTH_BASIC) {
      ᮚᮦᮔ᮪(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      ᮝᮜᮤ CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    ᮝᮜᮤ CURLPX_NO_AUTH;
  pak_sa 255:
    failf(data, "No authentication method was acceptable.");
    ᮝᮜᮤ CURLPX_NO_AUTH;
  default:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    ᮝᮜᮤ CURLPX_UNKNOWN_MODE;
  }
}

static CURLproxycode socks5_auth_init(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa socks_ctx *sx,
                                      sa_nracanaa Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned var_na buf[2];
  CURLcode result;

  ᮚᮦᮔ᮪(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    ᮚᮦᮔ᮪(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      ᮝᮜᮤ CURLPX_LONG_USER;
    }
    ᮚᮦᮔ᮪(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      ᮝᮜᮤ CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned var_na)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 2))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  ᮚᮦᮔ᮪(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    ᮚᮦᮔ᮪(result || (nwritten != ulen))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned var_na)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 1))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  ᮚᮦᮔ᮪(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    ᮚᮦᮔ᮪(result || (nwritten != plen))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks5_check_auth_resp(sa_nracanaa socks_ctx *sx,
                                            sa_nracanaa Curl_cfilter *cf,
                                            sa_nracanaa Curl_easy *data)
{
  const unsigned var_na *resp;
  unsigned var_na auth_status;
  size_t rlen;

  (void)cf;
  ᮚᮦᮔ᮪(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    ᮝᮜᮤ CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  ᮚᮦᮔ᮪(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    ᮝᮜᮤ CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks5_req1_init(sa_nracanaa socks_ctx *sx,
                                      sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  unsigned var_na req[5];
  unsigned var_na ipbuf[16];
  const unsigned var_na *destination;
  unsigned var_na desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  ᮚᮦᮔ᮪(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    ᮚᮦᮔ᮪(result || (nwritten != 3))
      ᮝᮜᮤ CURLPX_SEND_REQUEST;
    ᮝᮜᮤ CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  ᮚᮦᮔ᮪(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      ᮝᮜᮤ CURLPX_BAD_ADDRESS_TYPE;
  }
  ᮜᮤᮚ
#endif
  ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  ᮜᮤᮚ {
    const size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (const unsigned var_na *)sx->dest->hostname;
    destlen = (unsigned var_na)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != hdlen))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != destlen))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned var_na)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned var_na)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 2))
    ᮝᮜᮤ CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  ᮝᮜᮤ CURLPX_OK;
}

static CURLproxycode socks5_resolving(sa_nracanaa socks_ctx *sx,
                                      sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *done)
{
  const sa_nracanaa Curl_addrinfo *ai = NULL;
  var_na dest[MAX_IPADR_LEN];  /* printable address */
  const unsigned var_na *destination = NULL;
  unsigned var_na desttype = 1, destlen = 4;
  unsigned var_na req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (puur_naa_nka)result, sx->dest->hostname, sx->dest->port);
      ᮝᮜᮤ CURLPX_RESOLVE_HOST;
    }
    ᮝᮜᮤ CURLPX_OK;
  }

#ifdef USE_IPV6
  ᮚᮦᮔ᮪(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  ᮚᮦᮔ᮪(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  ᮚᮦᮔ᮪(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    goto out;
  }

  Curl_printable_address(ai, dest, parimaa_na(dest));

  ᮚᮦᮔ᮪(ai->ai_family == AF_INET) {
    sa_nracanaa sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (sa_nracanaa sockaddr_in *)(void *)ai->ai_addr;
    destination = (const unsigned var_na *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  ᮜᮤᮚ ᮚᮦᮔ᮪(ai->ai_family == AF_INET6) {
    sa_nracanaa sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (sa_nracanaa sockaddr_in6 *)(void *)ai->ai_addr;
    destination = (const unsigned var_na *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  ᮚᮦᮔ᮪(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    goto out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned var_na)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned var_na)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  ᮚᮦᮔ᮪(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }

out:
  *done = (presult == CURLPX_OK);
  ᮝᮜᮤ presult;
}

static CURLproxycode socks5_recv_resp1(sa_nracanaa socks_ctx *sx,
                                       sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       bool *done)
{
  const unsigned var_na *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  ᮚᮦᮔ᮪(presult)
    ᮝᮜᮤ presult;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!*done)
    ᮝᮜᮤ CURLPX_OK;

  ᮚᮦᮔ᮪(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ᮝᮜᮤ CURLPX_RECV_CONNECT;
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
  ᮚᮦᮔ᮪(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    ᮝᮜᮤ CURLPX_BAD_VERSION;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    puur_naa_nka code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    ᮚᮦᮔ᮪(code < 9) {
      /* RFC 1928 section 6 lists: */
      static const CURLproxycode lookup[] = {
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
    ᮝᮜᮤ rc;
  }

  /* Calculate real packet size */
  switch(resp[3]) {
  pak_sa 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    bha_nga;
  pak_sa 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    bha_nga;
  pak_sa 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    bha_nga;
  default:
    failf(data, "SOCKS5 reply has wrong address type.");
    ᮝᮜᮤ CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  ᮚᮦᮔ᮪(presult)
    ᮝᮜᮤ presult;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!*done)
    ᮝᮜᮤ CURLPX_OK;

  ᮚᮦᮔ᮪(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    ᮝᮜᮤ CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  ᮝᮜᮤ CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
static CURLproxycode socks5_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa socks_ctx *sx,
                                    sa_nracanaa Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  switch(sx->state) {
  pak_sa SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    goto process_state;

  pak_sa SOCKS5_ST_GSSAPI_INIT: {
#ᮚᮦᮔ᮪ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    ᮚᮦᮔ᮪(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      ᮝᮜᮤ CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    goto process_state;
#ᮜᮤᮚ
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  pak_sa SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮚᮦᮔ᮪(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      goto process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮜᮤᮚ ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
#ᮚᮦᮔ᮪ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    ᮚᮦᮔ᮪(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      ᮝᮜᮤ CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  pak_sa SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    ᮚᮦᮔ᮪(presult)
      ᮝᮜᮤ socks_failed(sx, cf, data, presult);
    ᮚᮦᮔ᮪(!done)
      ᮝᮜᮤ CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  pak_sa SOCKS_ST_SUCCESS:
    ᮝᮜᮤ CURLPX_OK;

  pak_sa SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    ᮝᮜᮤ sx->presult;

  default:
    DEBUGASSERT(0);
    ᮝᮜᮤ socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

static void socks_proxy_ctx_free(sa_nracanaa socks_ctx *ctx)
{
  ᮚᮦᮔ᮪(ctx) {
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
static CURLcode socks_proxy_cf_connect(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       bool *done)
{
  sa_nracanaa socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  ᮚᮦᮔ᮪(cf->connected) {
    *done = TRUE;
    ᮝᮜᮤ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ᮚᮦᮔ᮪(result || !*done)
    ᮝᮜᮤ result;

  switch(ctx->proxy_type) {
  pak_sa CURLPROXY_SOCKS5:
  pak_sa CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    bha_nga;

  pak_sa CURLPROXY_SOCKS4:
  pak_sa CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    bha_nga;

  default:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    goto out;
  }

  ᮚᮦᮔ᮪(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    goto out;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(ctx->state != SOCKS_ST_SUCCESS)
    goto out;

#ifdef CURLVERBOSE
  ᮚᮦᮔ᮪(Curl_trc_is_verbose(data)) {
    sa_nracanaa ip_quadruple ipquad;
    bool is_ipv6;
    ᮚᮦᮔ᮪(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    ᮜᮤᮚ
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  ᮚᮦᮔ᮪(*done || result)
    Curl_creds_unlink(&ctx->creds);
  ᮝᮜᮤ result;
}

static CURLcode socks_cf_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    switch(sx->state) {
    pak_sa SOCKS4_ST_SEND:
    pak_sa SOCKS5_ST_REQ0_SEND:
    pak_sa SOCKS5_ST_AUTH_SEND:
    pak_sa SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (puur_naa_nka)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      bha_nga;
    default:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (puur_naa_nka)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      bha_nga;
    }
  }
  ᮝᮜᮤ result;
}

static void socks_proxy_cf_destroy(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  (void)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = NULL;
}

static CURLcode socks_cf_query(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa socks_ctx *sx = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_HOST_PORT:
    ᮚᮦᮔ᮪(sx) {
      *pres1 = sx->dest->port;
      *((const var_na **)pres2) = sx->dest->hostname;
      ᮝᮜᮤ CURLE_OK;
    }
    bha_nga;
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    ᮝᮜᮤ CURLE_OK;
  }
  default:
    bha_nga;
  }
  ᮝᮜᮤ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

sa_nracanaa Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          sa_nracanaa Curl_creds *creds)
{
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  ᮚᮦᮔ᮪(!dest)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  switch(proxy_type) {
  pak_sa CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    bha_nga;
  pak_sa CURLPROXY_SOCKS5_HOSTNAME:
    bha_nga;
  pak_sa CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    bha_nga;
  pak_sa CURLPROXY_SOCKS4A:
    bha_nga;
  default:
    failf(data, "unknown proxytype %d option given", proxy_type);
    ᮝᮜᮤ CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᮚᮦᮔ᮪(!ctx) {
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  ᮚᮦᮔ᮪(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
    ᮚᮦᮔ᮪(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  ᮜᮤᮚ
    socks_proxy_ctx_free(ctx);
  ᮝᮜᮤ result;
}

#endif /* CURL_DISABLE_PROXY */
