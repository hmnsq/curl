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
enum socks_state_t {
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

#만약 defined(DEBUGBUILD) && defined(CURLVERBOSE)
static const char * const cf_socks_statename[] = {
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

struct socks_ctx {
  enum socks_state_t state;
  struct bufq iobuf;
  struct Curl_peer *dest;
  struct Curl_creds *creds;
  CURLproxycode presult;
  uint32_t resolv_id;
  uint8_t ip_version;
  uint8_t proxy_type;
  unsigned char version;
  BIT(resolve_local);
  BIT(socks4a);
};

#만약 defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior. Only used by the SOCKS GSSAPI functions.
 */
CURLcode Curl_blockread_all(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            char *buf,             /* store read data here */
                            size_t blen,           /* space in buf */
                            size_t *pnread)        /* amount bytes read */
{
  size_t nread = 0;
  CURLcode result;

  *pnread = 0;
  위해(;;) {
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);

    만약(timeout_ms < 0) {
      /* we already got the timeout */
      반환 CURLE_OPERATION_TIMEDOUT;
    }
    만약(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;
    만약(SOCKET_READABLE(sock, timeout_ms) <= 0)
      반환 CURLE_OPERATION_TIMEDOUT;
    result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
    만약(result == CURLE_AGAIN)
      continue;
    아니면 만약(result)
      반환 result;

    만약(blen == nread) {
      *pnread += nread;
      반환 CURLE_OK;
    }
    만약(!nread) /* EOF */
      반환 CURLE_RECV_ERROR;

    buf += nread;
    blen -= nread;
    *pnread += nread;
  }
}
#endif

#만약 defined(DEBUGBUILD) && defined(CURLVERBOSE)
#define sxstate(x, c, d, y) socksstate(x, c, d, y, __LINE__)
#아니면
#define sxstate(x, c, d, y) socksstate(x, c, d, y)
#endif

/* always use this function to change state, to make debugging easier */
static void socksstate(struct socks_ctx *sx,
                       struct Curl_cfilter *cf,
                       struct Curl_easy *data,
                       enum socks_state_t state
#만약 defined(DEBUGBUILD) && defined(CURLVERBOSE)
                       , 정수 lineno
#endif
)
{
  enum socks_state_t oldstate = sx->state;

  만약(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    반환;

  sx->state = state;

#만약 defined(DEBUGBUILD) && defined(CURLVERBOSE)
  CURL_TRC_CF(data, cf, "[%s] -> [%s] (line %d)",
              cf_socks_statename[oldstate],
              cf_socks_statename[sx->state], lineno);
#아니면
  (void)cf;
  (void)data;
#endif
}

static CURLproxycode socks_failed(struct socks_ctx *sx,
                                  struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  CURLproxycode presult)
{
  sxstate(sx, cf, data, SOCKS_ST_FAILED);
  sx->presult = presult;
  반환 presult;
}

static CURLproxycode socks_flush(struct socks_ctx *sx,
                                 struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 bool *done)
{
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  동안(!Curl_bufq_is_empty(&sx->iobuf)) {
    result = Curl_cf_send_bufq(cf->next, data, &sx->iobuf, NULL, 0,
                               &nwritten);
    만약(result == CURLE_AGAIN)
      반환 CURLPX_OK;
    아니면 만약(result) {
      failf(data, "Failed to send SOCKS request: %s",
            curl_easy_strerror(result));
      반환 socks_failed(sx, cf, data, CURLPX_SEND_CONNECT);
    }
  }
  *done = TRUE;
  반환 CURLPX_OK;
}

static CURLproxycode socks_recv(struct socks_ctx *sx,
                                struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                size_t min_bytes,
                                bool *done)
{
  CURLcode result;
  size_t nread;

  *done = FALSE;
  동안(Curl_bufq_len(&sx->iobuf) < min_bytes) {
    result = Curl_cf_recv_bufq(cf->next, data, &sx->iobuf,
                               min_bytes - Curl_bufq_len(&sx->iobuf),
                               &nread);
    만약(result == CURLE_AGAIN)
      반환 CURLPX_OK;
    아니면 만약(result) {
      failf(data, "Failed to receive SOCKS response: %s",
            curl_easy_strerror(result));
      반환 CURLPX_RECV_CONNECT;
    }
    아니면 만약(!nread) { /* EOF */
      만약(Curl_bufq_len(&sx->iobuf) < min_bytes) {
        failf(data, "Failed to receive SOCKS response, "
              "proxy closed connection");
        반환 CURLPX_RECV_CONNECT;
      }
      break;
    }
  }
  *done = TRUE;
  반환 CURLPX_OK;
}

static CURLproxycode socks4_req_add_hd(struct socks_ctx *sx,
                                       struct Curl_easy *data)
{
  unsigned char buf[4];
  size_t nwritten;
  CURLcode result;

  (void)data;
  buf[0] = 4; /* version (SOCKS4) */
  buf[1] = 1; /* connect */
  buf[2] = (unsigned char)((sx->dest->port >> 8) & 0xffU); /* MSB */
  buf[3] = (unsigned char)(sx->dest->port & 0xffU);        /* LSB */

  result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
  만약(result || (nwritten != 4))
    반환 CURLPX_SEND_REQUEST;
  반환 CURLPX_OK;
}

static CURLproxycode socks4_req_add_user(struct socks_ctx *sx,
                                         struct Curl_easy *data)
{
  CURLcode result;
  size_t nwritten;

  만약(sx->creds) {
    size_t plen = strlen(sx->creds->user);
    만약(plen > 255) {
      /* there is no real size limit to this field in the protocol, but
         SOCKS5 limits the proxy user field to 255 bytes and it seems likely
         that a longer field is either a mistake or malicious input */
      failf(data, "Too long SOCKS proxy username");
      반환 CURLPX_LONG_USER;
    }
    /* add proxy name WITH null-terminator */
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, plen + 1,
                              &nwritten);
    만약(result || (nwritten != (plen + 1)))
      반환 CURLPX_SEND_REQUEST;
  }
  아니면 {
    /* empty username */
    unsigned char b = 0;
    result = Curl_bufq_write(&sx->iobuf, &b, 1, &nwritten);
    만약(result || (nwritten != 1))
      반환 CURLPX_SEND_REQUEST;
  }
  반환 CURLPX_OK;
}

static CURLproxycode socks4_resolving(struct socks_ctx *sx,
                                      struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      bool *done)
{
  const struct Curl_addrinfo *ai = NULL;
  CURLcode result;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  만약(result) {
    만약(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (정수)result, sx->dest->hostname, sx->dest->port);
      반환 CURLPX_RESOLVE_HOST;
    }
    반환 CURLPX_OK;
  }

  ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);
  만약(ai) {
    struct sockaddr_in *saddr_in;
    char ipbuf[64];

    Curl_printable_address(ai, ipbuf, sizeof(ipbuf));
    CURL_TRC_CF(data, cf, "SOCKS4 connect to IPv4 %s (locally resolved)",
                ipbuf);

    saddr_in = (struct sockaddr_in *)(void *)ai->ai_addr;
    result = Curl_bufq_write(&sx->iobuf,
                             (unsigned char *)&saddr_in->sin_addr.s_addr, 4,
                             &nwritten);

    만약(result || (nwritten != 4))
      반환 CURLPX_SEND_REQUEST;
  }
  아니면 {
    /* No IPv4 address resolved */
    failf(data, "SOCKS4 connection to %s not supported", sx->dest->hostname);
    반환 CURLPX_RESOLVE_HOST;
  }

  *done = TRUE;
  반환 CURLPX_OK;
}

static CURLproxycode socks4_check_resp(struct socks_ctx *sx,
                                       struct Curl_cfilter *cf,
                                       struct Curl_easy *data)
{
  const unsigned char *resp;
  size_t rlen;

  만약(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 8) {
    failf(data, "SOCKS4 reply is incomplete.");
    반환 CURLPX_RECV_CONNECT;
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
  만약(resp[0]) {
    failf(data, "SOCKS4 reply has wrong version, version should be 0.");
    반환 CURLPX_BAD_VERSION;
  }

  /* Result */
  switch(resp[1]) {
  case 90:
    CURL_TRC_CF(data, cf, "SOCKS4%s request granted.", sx->socks4a ? "a" : "");
    Curl_bufq_skip(&sx->iobuf, 8);
    반환 CURLPX_OK;
  case 91:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected or failed.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned 정수)((resp[2] << 8) | resp[3]), resp[1]);
    반환 CURLPX_REQUEST_FAILED;
  case 92:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because SOCKS server cannot connect to "
          "identd on the client.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned 정수)((resp[2] << 8) | resp[3]), resp[1]);
    반환 CURLPX_IDENTD;
  case 93:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", request rejected because the client program and identd "
          "report different user-ids.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned 정수)((resp[2] << 8) | resp[3]), resp[1]);
    반환 CURLPX_IDENTD_DIFFER;
  default:
    failf(data,
          "[SOCKS] cannot complete SOCKS4 connection to %u.%u.%u.%u:%u. (%u)"
          ", Unknown.",
          resp[4], resp[5], resp[6], resp[7],
          (unsigned 정수)((resp[2] << 8) | resp[3]), resp[1]);
    반환 CURLPX_UNKNOWN_FAIL;
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
static CURLproxycode socks4_connect(struct Curl_cfilter *cf,
                                    struct socks_ctx *sx,
                                    struct Curl_easy *data)
{
  size_t nwritten;
  CURLproxycode presult;
  CURLcode result;
  bool done;

process_state:
  switch(sx->state) {
  case SOCKS_ST_INIT:
    sx->version = 4;
    sxstate(sx, cf, data, SOCKS4_ST_START);
    FALLTHROUGH();

  case SOCKS4_ST_START:
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
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);

    /* DNS resolve only for SOCKS4, not SOCKS4a */
    만약(!sx->resolve_local) {
      /* socks4a, not resolving locally, sends the hostname.
       * add an invalid address + user + hostname */
      unsigned char buf[4] = { 0, 0, 0, 1 };
      size_t hlen = strlen(sx->dest->hostname) + 1; /* including NUL */

      만약(hlen > 255) {
        failf(data, "SOCKS4: too long hostname");
        반환 socks_failed(sx, cf, data, CURLPX_LONG_HOSTNAME);
      }
      result = Curl_bufq_write(&sx->iobuf, buf, 4, &nwritten);
      만약(result || (nwritten != 4))
        반환 socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      presult = socks4_req_add_user(sx, data);
      만약(presult)
        반환 socks_failed(sx, cf, data, presult);
      result = Curl_bufq_cwrite(&sx->iobuf, sx->dest->hostname, hlen,
                                &nwritten);
      만약(result || (nwritten != hlen))
        반환 socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
      /* request complete */
      sxstate(sx, cf, data, SOCKS4_ST_SEND);
      goto process_state;
    }
    sxstate(sx, cf, data, SOCKS4_ST_RESOLVING);
    FALLTHROUGH();

  case SOCKS4_ST_RESOLVING:
    presult = socks4_resolving(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    만약(!done)
      반환 CURLPX_OK;
    /* append user */
    presult = socks4_req_add_user(sx, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS4_ST_SEND);
    FALLTHROUGH();

  case SOCKS4_ST_SEND:
    presult = socks_flush(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    sxstate(sx, cf, data, SOCKS4_ST_RECV);
    FALLTHROUGH();

  case SOCKS4_ST_RECV:
    /* Receive 8-byte response */
    presult = socks_recv(sx, cf, data, 8, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    presult = socks4_check_resp(sx, cf, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  case SOCKS_ST_SUCCESS:
    반환 CURLPX_OK;

  case SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    반환 sx->presult;

  default:
    DEBUGASSERT(0);
    반환 socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

static CURLproxycode socks5_req0_init(struct Curl_cfilter *cf,
                                      struct socks_ctx *sx,
                                      struct Curl_easy *data)
{
  const unsigned char auth = data->set.socks5auth;
  unsigned char req[5]; /* version + len + 3 possible auth methods */
  unsigned char nauths;
  size_t req_len, nwritten;
  CURLcode result;

  (void)cf;
  /* RFC1928 chapter 5 specifies max 255 chars for domain name in packet */
  만약(!sx->resolve_local && strlen(sx->dest->hostname) > 255) {
    failf(data, "SOCKS5: the destination hostname is too long to be "
          "resolved remotely by the proxy.");
    반환 CURLPX_LONG_HOSTNAME;
  }

  만약(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI))
    infof(data, "warning: unsupported value passed to "
          "CURLOPT_SOCKS5_AUTH: %u", auth);
  만약(!(auth & CURLAUTH_BASIC))
    /* disable username/password auth */
    Curl_creds_unlink(&sx->creds);

  만약(cf->sockindex == FIRSTSOCKET)
    /* the reuse of this connection depends on how this negotiation ends */
    cf->conn->bits.socks5_authenticated = FALSE;

  req[0] = 5;   /* version */
  nauths = 0;
  만약(!data->set.socks5_auth_only) {
    ++nauths;
    req[1 + nauths] = 0; /* no authentication */
  }
#만약 defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  만약(auth & CURLAUTH_GSSAPI) {
    ++nauths;
    req[1 + nauths] = 1; /* GSS-API */
  }
#endif
  만약(sx->creds) {
    ++nauths;
    req[1 + nauths] = 2; /* username/password */
  }
  만약(!nauths) {
    failf(data, "SOCKS5: no acceptable authentication method is available.");
    반환 CURLPX_NO_AUTH;
  }
  req[1] = nauths;
  req_len = 2 + nauths;

  result = Curl_bufq_write(&sx->iobuf, req, req_len, &nwritten);
  만약(result || (nwritten != req_len))
    반환 CURLPX_SEND_REQUEST;
  반환 CURLPX_OK;
}

static CURLproxycode socks5_check_resp0(struct socks_ctx *sx,
                                        struct Curl_cfilter *cf,
                                        struct Curl_easy *data)
{
  const unsigned char *resp;
  unsigned char auth_mode;
  size_t rlen;

  만약(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 initial reply is incomplete.");
    반환 CURLPX_RECV_CONNECT;
  }

  만약(resp[0] != 5) {
    failf(data, "Received invalid version in initial SOCKS5 response.");
    반환 CURLPX_BAD_VERSION;
  }

  auth_mode = resp[1];
  Curl_bufq_skip(&sx->iobuf, 2);

  switch(auth_mode) {
  case 0:
    만약(data->set.socks5_auth_only) {
      failf(data, "SOCKS5 proxy selected no authentication.");
      반환 CURLPX_NO_AUTH;
    }
    /* DONE! No authentication needed. Send request. */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    반환 CURLPX_OK;
  case 1:
    만약(data->set.socks5auth & CURLAUTH_GSSAPI) {
      만약(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_GSSAPI_INIT);
      반환 CURLPX_OK;
    }
    failf(data, "SOCKS5 GSSAPI per-message authentication is not enabled.");
    반환 CURLPX_GSSAPI_PERMSG;
  case 2:
    /* regular name + password authentication */
    만약(data->set.socks5auth & CURLAUTH_BASIC) {
      만약(cf->sockindex == FIRSTSOCKET)
        cf->conn->bits.socks5_authenticated = TRUE;
      sxstate(sx, cf, data, SOCKS5_ST_AUTH_INIT);
      반환 CURLPX_OK;
    }
    failf(data, "BASIC authentication proposed but not enabled.");
    반환 CURLPX_NO_AUTH;
  case 255:
    failf(data, "No authentication method was acceptable.");
    반환 CURLPX_NO_AUTH;
  default:
    failf(data, "Unknown SOCKS5 mode attempted to be used by server.");
    반환 CURLPX_UNKNOWN_MODE;
  }
}

static CURLproxycode socks5_auth_init(struct Curl_cfilter *cf,
                                      struct socks_ctx *sx,
                                      struct Curl_easy *data)
{
  /* Needs username and password */
  size_t ulen = 0, plen = 0, nwritten;
  unsigned char buf[2];
  CURLcode result;

  만약(sx->creds) {
    ulen = strlen(sx->creds->user);
    plen = strlen(sx->creds->passwd);
    /* the lengths must fit in a single byte */
    만약(ulen > 255) {
      failf(data, "Excessive username length for proxy auth");
      반환 CURLPX_LONG_USER;
    }
    만약(plen > 255) {
      failf(data, "Excessive password length for proxy auth");
      반환 CURLPX_LONG_PASSWD;
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
  buf[1] = (unsigned char)ulen;
  result = Curl_bufq_write(&sx->iobuf, buf, 2, &nwritten);
  만약(result || (nwritten != 2))
    반환 CURLPX_SEND_REQUEST;
  만약(ulen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->user, ulen, &nwritten);
    만약(result || (nwritten != ulen))
      반환 CURLPX_SEND_REQUEST;
  }
  buf[0] = (unsigned char)plen;
  result = Curl_bufq_write(&sx->iobuf, buf, 1, &nwritten);
  만약(result || (nwritten != 1))
    반환 CURLPX_SEND_REQUEST;
  만약(plen) {
    result = Curl_bufq_cwrite(&sx->iobuf, sx->creds->passwd, plen, &nwritten);
    만약(result || (nwritten != plen))
      반환 CURLPX_SEND_REQUEST;
  }
  sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
  반환 CURLPX_OK;
}

static CURLproxycode socks5_check_auth_resp(struct socks_ctx *sx,
                                            struct Curl_cfilter *cf,
                                            struct Curl_easy *data)
{
  const unsigned char *resp;
  unsigned char auth_status;
  size_t rlen;

  (void)cf;
  만약(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < 2) {
    failf(data, "SOCKS5 sub-negotiation response incomplete.");
    반환 CURLPX_RECV_CONNECT;
  }

  /* ignore the first (VER) byte */
  auth_status = resp[1];
  만약(auth_status) {
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          resp[0], resp[1]);
    반환 CURLPX_USER_REJECTED;
  }
  Curl_bufq_skip(&sx->iobuf, 2);
  반환 CURLPX_OK;
}

static CURLproxycode socks5_req1_init(struct socks_ctx *sx,
                                      struct Curl_cfilter *cf,
                                      struct Curl_easy *data)
{
  unsigned char req[5];
  unsigned char ipbuf[16];
  const unsigned char *destination;
  unsigned char desttype, destlen, hdlen;
  size_t nwritten;
  CURLcode result;

  req[0] = 5; /* version (SOCKS5) */
  req[1] = 1; /* connect */
  req[2] = 0; /* must be zero */
  만약(sx->resolve_local) {
    /* rest of request is added after resolving */
    result = Curl_bufq_write(&sx->iobuf, req, 3, &nwritten);
    만약(result || (nwritten != 3))
      반환 CURLPX_SEND_REQUEST;
    반환 CURLPX_OK;
  }

  /* remote resolving, send what type+addr/string to resolve */
#ifdef USE_IPV6
  만약(strchr(sx->dest->hostname, ':')) {
    desttype = 4;
    destination = ipbuf;
    destlen = 16;
    만약(curlx_inet_pton(AF_INET6, sx->dest->hostname, ipbuf) != 1)
      반환 CURLPX_BAD_ADDRESS_TYPE;
  }
  아니면
#endif
  만약(curlx_inet_pton(AF_INET, sx->dest->hostname, ipbuf) == 1) {
    desttype = 1;
    destination = ipbuf;
    destlen = 4;
  }
  아니면 {
    const size_t hostname_len = strlen(sx->dest->hostname);
    /* socks5_req0_init() already rejects hostnames longer than 255 bytes, so
       this cast to unsigned char is safe. Assert to guard against future
       refactoring that might remove or reorder that earlier check. */
    DEBUGASSERT(hostname_len <= 255);
    desttype = 3;
    destination = (const unsigned char *)sx->dest->hostname;
    destlen = (unsigned char)hostname_len; /* 1-byte length */
  }

  req[3] = desttype;
  req[4] = destlen;
  hdlen = (desttype == 3) ? 5 : 4; /* no length byte for ip addresses */
  result = Curl_bufq_write(&sx->iobuf, req, hdlen, &nwritten);
  만약(result || (nwritten != hdlen))
    반환 CURLPX_SEND_REQUEST;
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  만약(result || (nwritten != destlen))
    반환 CURLPX_SEND_REQUEST;
  /* PORT MSB+LSB */
  req[0] = (unsigned char)((sx->dest->port >> 8) & 0xff);
  req[1] = (unsigned char)(sx->dest->port & 0xff);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  만약(result || (nwritten != 2))
    반환 CURLPX_SEND_REQUEST;
  CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (remotely resolved)",
              sx->dest->hostname, sx->dest->port);
  반환 CURLPX_OK;
}

static CURLproxycode socks5_resolving(struct socks_ctx *sx,
                                      struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      bool *done)
{
  const struct Curl_addrinfo *ai = NULL;
  char dest[MAX_IPADR_LEN];  /* printable address */
  const unsigned char *destination = NULL;
  unsigned char desttype = 1, destlen = 4;
  unsigned char req[2];
  CURLcode result;
  CURLproxycode presult = CURLPX_OK;
  size_t nwritten;

  *done = FALSE;
  result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex, sx->dest);
  만약(result) {
    만약(result != CURLE_AGAIN) {
      failf(data, "error %d resolving SOCKS destination %s:%u",
            (정수)result, sx->dest->hostname, sx->dest->port);
      반환 CURLPX_RESOLVE_HOST;
    }
    반환 CURLPX_OK;
  }

#ifdef USE_IPV6
  만약(data->set.ipver != CURL_IPRESOLVE_V4)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET6, 0);
#endif
  만약(!ai)
    ai = Curl_conn_dns_get_ai(data, sx->dest, cf->sockindex, AF_INET, 0);

  만약(!ai) {
    failf(data, "Failed to resolve \"%s\" for SOCKS5 connect.",
          sx->dest->hostname);
    presult = CURLPX_RESOLVE_HOST;
    goto out;
  }

  Curl_printable_address(ai, dest, sizeof(dest));

  만약(ai->ai_family == AF_INET) {
    struct sockaddr_in *saddr_in;
    desttype = 1; /* ATYP: IPv4 = 1 */
    destlen = 4;
    saddr_in = (struct sockaddr_in *)(void *)ai->ai_addr;
    destination = (const unsigned char *)&saddr_in->sin_addr.s_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to %s:%u (locally resolved)",
                dest, sx->dest->port);
  }
#ifdef USE_IPV6
  아니면 만약(ai->ai_family == AF_INET6) {
    struct sockaddr_in6 *saddr_in6;
    desttype = 4; /* ATYP: IPv6 = 4 */
    destlen = 16;
    saddr_in6 = (struct sockaddr_in6 *)(void *)ai->ai_addr;
    destination = (const unsigned char *)&saddr_in6->sin6_addr.s6_addr;
    CURL_TRC_CF(data, cf, "SOCKS5 connect to [%s]:%u (locally resolved)",
                dest, sx->dest->port);
  }
#endif

  만약(!destination) {
    failf(data, "SOCKS5 connection to %s not supported", dest);
    presult = CURLPX_RESOLVE_HOST;
    goto out;
  }

  req[0] = desttype;
  result = Curl_bufq_write(&sx->iobuf, req, 1, &nwritten);
  만약(result || (nwritten != 1)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }
  result = Curl_bufq_write(&sx->iobuf, destination, destlen, &nwritten);
  만약(result || (nwritten != destlen)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }
  /* PORT MSB+LSB */
  req[0] = (unsigned char)((sx->dest->port >> 8) & 0xffU);
  req[1] = (unsigned char)(sx->dest->port & 0xffU);
  result = Curl_bufq_write(&sx->iobuf, req, 2, &nwritten);
  만약(result || (nwritten != 2)) {
    presult = CURLPX_SEND_REQUEST;
    goto out;
  }

out:
  *done = (presult == CURLPX_OK);
  반환 presult;
}

static CURLproxycode socks5_recv_resp1(struct socks_ctx *sx,
                                       struct Curl_cfilter *cf,
                                       struct Curl_easy *data,
                                       bool *done)
{
  const unsigned char *resp;
  size_t rlen, resp_len = 8; /* minimum response length */
  CURLproxycode presult;

  presult = socks_recv(sx, cf, data, resp_len, done);
  만약(presult)
    반환 presult;
  아니면 만약(!*done)
    반환 CURLPX_OK;

  만약(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    반환 CURLPX_RECV_CONNECT;
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
  만약(resp[0] != 5) { /* version */
    failf(data, "SOCKS5 reply has wrong version, version should be 5.");
    반환 CURLPX_BAD_VERSION;
  }
  아니면 만약(resp[1]) { /* Anything besides 0 is an error */
    CURLproxycode rc = CURLPX_REPLY_UNASSIGNED;
    정수 code = resp[1];
    failf(data, "cannot complete SOCKS5 connection to %s. (%d)",
          sx->dest->hostname, code);
    만약(code < 9) {
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
    반환 rc;
  }

  /* Calculate real packet size */
  switch(resp[3]) {
  case 1: /* IPv4 */
    resp_len = 4 + 4 + 2;
    break;
  case 3: /* domain name */
    resp_len = 4 + 1 + resp[4] + 2; /* header, var length, var bytes, port */
    break;
  case 4: /* IPv6 */
    resp_len = 4 + 16 + 2;
    break;
  default:
    failf(data, "SOCKS5 reply has wrong address type.");
    반환 CURLPX_BAD_ADDRESS_TYPE;
  }

  /* receive the rest of the response */
  presult = socks_recv(sx, cf, data, resp_len, done);
  만약(presult)
    반환 presult;
  아니면 만약(!*done)
    반환 CURLPX_OK;

  만약(!Curl_bufq_peek(&sx->iobuf, &resp, &rlen) || rlen < resp_len) {
    failf(data, "SOCKS5 response is incomplete.");
    반환 CURLPX_RECV_CONNECT;
  }
  /* got it all */
  *done = TRUE;
  반환 CURLPX_OK;
}

/*
 * This function logs in to a SOCKS5 proxy and sends the specifics to the final
 * destination server.
 */
static CURLproxycode socks5_connect(struct Curl_cfilter *cf,
                                    struct socks_ctx *sx,
                                    struct Curl_easy *data)
{
  CURLproxycode presult;
  bool done;

process_state:
  switch(sx->state) {
  case SOCKS_ST_INIT:
    sx->version = 5;
    sxstate(sx, cf, data, SOCKS5_ST_START);
    FALLTHROUGH();

  case SOCKS5_ST_START:
    CURL_TRC_CF(data, cf, "SOCKS5: connecting to %s:%u",
                sx->dest->hostname, sx->dest->port);
    presult = socks5_req0_init(cf, sx, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_REQ0_SEND);
    FALLTHROUGH();

  case SOCKS5_ST_REQ0_SEND:
    presult = socks_flush(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    /* done sending! */
    sxstate(sx, cf, data, SOCKS5_ST_RESP0_RECV);
    FALLTHROUGH();

  case SOCKS5_ST_RESP0_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    presult = socks5_check_resp0(sx, cf, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    /* socks5_check_resp0() sets next socks state */
    goto process_state;

  case SOCKS5_ST_GSSAPI_INIT: {
#만약 defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    /* GSSAPI stuff done non-blocking */
    CURLcode result = Curl_SOCKS5_gssapi_negotiate(cf, data, sx->creds);
    만약(result) {
      failf(data, "Unable to negotiate SOCKS5 GSS-API context.");
      반환 CURLPX_GSSAPI;
    }
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    goto process_state;
#아니면
    failf(data, "SOCKS5 GSSAPI per-message authentication is not supported.");
    반환 socks_failed(sx, cf, data, CURLPX_GSSAPI_PERMSG);
#endif
  }

  case SOCKS5_ST_AUTH_INIT:
    presult = socks5_auth_init(cf, sx, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_SEND);
    FALLTHROUGH();

  case SOCKS5_ST_AUTH_SEND:
    presult = socks_flush(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_AUTH_RECV);
    FALLTHROUGH();

  case SOCKS5_ST_AUTH_RECV:
    presult = socks_recv(sx, cf, data, 2, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
    presult = socks5_check_auth_resp(sx, cf, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    /* Everything is good so far, user was authenticated! */
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_INIT);
    FALLTHROUGH();

  case SOCKS5_ST_REQ1_INIT:
    presult = socks5_req1_init(sx, cf, data);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    만약(!sx->resolve_local) {
      /* we do not resolve, request is complete */
      sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
      goto process_state;
    }
    sxstate(sx, cf, data, SOCKS5_ST_RESOLVING);
    FALLTHROUGH();

  case SOCKS5_ST_RESOLVING:
    presult = socks5_resolving(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    만약(!done)
      반환 CURLPX_OK;
    sxstate(sx, cf, data, SOCKS5_ST_REQ1_SEND);
    FALLTHROUGH();

  case SOCKS5_ST_REQ1_SEND:
    presult = socks_flush(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    아니면 만약(!done)
      반환 CURLPX_OK;
#만약 defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
    만약(cf->conn->socks5_gssapi_enctype) {
      failf(data, "SOCKS5 GSS-API protection not yet implemented.");
      반환 CURLPX_GSSAPI_PROTECTION;
    }
#endif
    sxstate(sx, cf, data, SOCKS5_ST_RESP1_RECV);
    FALLTHROUGH();

  case SOCKS5_ST_RESP1_RECV:
    presult = socks5_recv_resp1(sx, cf, data, &done);
    만약(presult)
      반환 socks_failed(sx, cf, data, presult);
    만약(!done)
      반환 CURLPX_OK;
    CURL_TRC_CF(data, cf, "SOCKS5 request granted.");
    sxstate(sx, cf, data, SOCKS_ST_SUCCESS);
    FALLTHROUGH();

  case SOCKS_ST_SUCCESS:
    반환 CURLPX_OK;

  case SOCKS_ST_FAILED:
    DEBUGASSERT(sx->presult);
    반환 sx->presult;

  default:
    DEBUGASSERT(0);
    반환 socks_failed(sx, cf, data, CURLPX_SEND_REQUEST);
  }
}

static void socks_proxy_ctx_free(struct socks_ctx *ctx)
{
  만약(ctx) {
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
static CURLcode socks_proxy_cf_connect(struct Curl_cfilter *cf,
                                       struct Curl_easy *data,
                                       bool *done)
{
  struct socks_ctx *ctx = cf->ctx;
  CURLproxycode pxresult = CURLPX_OK;
  CURLcode result;

  만약(cf->connected) {
    *done = TRUE;
    반환 CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  만약(result || !*done)
    반환 result;

  switch(ctx->proxy_type) {
  case CURLPROXY_SOCKS5:
  case CURLPROXY_SOCKS5_HOSTNAME:
    pxresult = socks5_connect(cf, ctx, data);
    break;

  case CURLPROXY_SOCKS4:
  case CURLPROXY_SOCKS4A:
    pxresult = socks4_connect(cf, ctx, data);
    break;

  default:
    DEBUGASSERT(0); /* should not come here, checked it at creation time */
    result = CURLE_COULDNT_CONNECT;
    goto out;
  }

  만약(pxresult) {
    result = CURLE_PROXY;
    data->info.pxcode = (uint8_t)pxresult;
    goto out;
  }
  아니면 만약(ctx->state != SOCKS_ST_SUCCESS)
    goto out;

#ifdef CURLVERBOSE
  만약(Curl_trc_is_verbose(data)) {
    struct ip_quadruple ipquad;
    bool is_ipv6;
    만약(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad))
      infof(data, "Opened %sSOCKS connection from %s port %d to %s port %d "
            "(via %s port %u)",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
            ipquad.local_ip, ipquad.local_port,
            ctx->dest->hostname, ctx->dest->port,
            ipquad.remote_ip, ipquad.remote_port);
    아니면
      infof(data, "Opened %sSOCKS connection",
            (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "");
  }
#endif
  cf->connected = TRUE;

out:
  *done = (bool)cf->connected;
  만약(*done || result)
    Curl_creds_unlink(&ctx->creds);
  반환 result;
}

static CURLcode socks_cf_adjust_pollset(struct Curl_cfilter *cf,
                                        struct Curl_easy *data,
                                        struct easy_pollset *ps)
{
  struct socks_ctx *sx = cf->ctx;
  CURLcode result = CURLE_OK;

  만약(!cf->connected && sx) {
    /* If we are not connected, the filter below is and has nothing
     * to wait on, we determine what to wait for. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    switch(sx->state) {
    case SOCKS4_ST_SEND:
    case SOCKS5_ST_REQ0_SEND:
    case SOCKS5_ST_AUTH_SEND:
    case SOCKS5_ST_REQ1_SEND:
      CURL_TRC_CF(data, cf, "adjust pollset out (%d)", (정수)sx->state);
      result = Curl_pollset_set_out_only(data, ps, sock);
      break;
    default:
      CURL_TRC_CF(data, cf, "adjust pollset in (%d)", (정수)sx->state);
      result = Curl_pollset_set_in_only(data, ps, sock);
      break;
    }
  }
  반환 result;
}

static void socks_proxy_cf_destroy(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  (void)data;
  socks_proxy_ctx_free(cf->ctx);
  cf->ctx = NULL;
}

static CURLcode socks_cf_query(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               정수 query, 정수 *pres1, void *pres2)
{
  struct socks_ctx *sx = cf->ctx;

  switch(query) {
  case CF_QUERY_HOST_PORT:
    만약(sx) {
      *pres1 = sx->dest->port;
      *((const char **)pres2) = sx->dest->hostname;
      반환 CURLE_OK;
    }
    break;
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    반환 CURLE_OK;
  }
  default:
    break;
  }
  반환 cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

struct Curl_cftype Curl_cft_socks_proxy = {
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

CURLcode Curl_cf_socks_proxy_insert_after(struct Curl_cfilter *cf_at,
                                          struct Curl_easy *data,
                                          struct Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          struct Curl_creds *creds)
{
  struct Curl_cfilter *cf;
  struct socks_ctx *ctx;
  bool resolve_local = FALSE;
  uint8_t dns_queries = Curl_resolv_dns_queries(data, ip_version);
  CURLcode result;

  만약(!dest)
    반환 CURLE_FAILED_INIT;

  switch(proxy_type) {
  case CURLPROXY_SOCKS5:
    resolve_local = TRUE;
    break;
  case CURLPROXY_SOCKS5_HOSTNAME:
    break;
  case CURLPROXY_SOCKS4:
    resolve_local = TRUE;
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_AAAA);
    break;
  case CURLPROXY_SOCKS4A:
    break;
  default:
    failf(data, "unknown proxytype %d option given", proxy_type);
    반환 CURLE_COULDNT_CONNECT;
  }

  /* NUL byte already part of struct size */
  ctx = curlx_calloc(1, sizeof(*ctx));
  만약(!ctx) {
    반환 CURLE_OUT_OF_MEMORY;
  }

  Curl_peer_link(&ctx->dest, dest);
  ctx->ip_version = ip_version;
  ctx->proxy_type = proxy_type;
  ctx->resolve_local = resolve_local;
  Curl_creds_link(&ctx->creds, creds);
  Curl_bufq_init2(&ctx->iobuf, SOCKS_CHUNK_SIZE, SOCKS_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_socks_proxy, ctx);
  만약(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
    만약(ctx->resolve_local) {
      result = Curl_conn_dns_add_addr_resolve(data, cf_at->conn,
                                              cf_at->sockindex,
                                              ctx->dest, dns_queries,
                                              TRNSPRT_TCP);
    }
  }
  아니면
    socks_proxy_ctx_free(ctx);
  반환 result;
}

#endif /* CURL_DISABLE_PROXY */
