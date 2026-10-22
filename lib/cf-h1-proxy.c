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

#만약 !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#include <curl/curl.h>
#include "urldata.h"
#include "curlx/dynbuf.h"
#include "sendf.h"
#include "http.h"
#include "http1.h"
#include "http_proxy.h"
#include "select.h"
#include "progress.h"
#include "multiif.h"
#include "cfilters.h"
#include "cf-h1-proxy.h"
#include "connect.h"
#include "curl_trc.h"
#include "strcase.h"
#include "curlx/strparse.h"

typedef enum {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
struct h1_tunnel_state {
  struct Curl_peer *dest;
  struct dynbuf rcvbuf;
  struct dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  struct Curl_chunker ch;
  정수 httpversion;
  enum keeponval {
    KEEPON_DONE,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  BIT(chunked_encoding);
  BIT(close_connection);
  BIT(maybe_folded);
  BIT(leading_unfold);
  BIT(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
struct cf_h1_proxy_ctx {
  struct Curl_peer *peer;
  struct h1_tunnel_state *ts;
};

static bool tunnel_is_established(struct h1_tunnel_state *ts)
{
  반환 ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

static bool tunnel_is_failed(struct h1_tunnel_state *ts)
{
  반환 ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

static bool h1_proxy_is_udp(struct Curl_cfilter *cf)
{
  struct cf_h1_proxy_ctx *pctx = cf->ctx;
  반환 (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

static CURLcode tunnel_reinit(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              struct h1_tunnel_state *ts)
{
  (void)data;
  (void)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_data);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  반환 CURLE_OK;
}

static CURLcode tunnel_init(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            struct h1_tunnel_state **pts)
{
  struct h1_tunnel_state *ts;

  만약(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    반환 CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, sizeof(*ts));
  만약(!ts)
    반환 CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  반환 tunnel_reinit(cf, data, ts);
}

static void h1_tunnel_go_state(struct Curl_cfilter *cf,
                               struct h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               struct Curl_easy *data)
{
  만약(ts->tunnel_state == new_state)
    반환;
  /* entering this one */
  switch(new_state) {
  case H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    break;

  case H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    break;

  case H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    break;

  case H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    break;

  case H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  case H1_TUNNEL_FAILED:
    만약(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_data);
    /* restore the protocol pointer */
    data->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    break;
  }
}

static void tunnel_free(struct h1_tunnel_state *ts,
                        struct Curl_easy *data)
{
  만약(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

static void cf_tunnel_free(struct Curl_cfilter *cf,
                           struct Curl_easy *data)
{
  만약(cf) {
    struct cf_h1_proxy_ctx *pctx = cf->ctx;
    struct h1_tunnel_state *ts = pctx ? pctx->ts : NULL;
    만약(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = NULL;
    }
  }
}

static bool tunnel_want_send(struct h1_tunnel_state *ts)
{
  반환 ts->tunnel_state == H1_TUNNEL_CONNECT;
}

static CURLcode start_CONNECT(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              struct h1_tunnel_state *ts)
{
  struct cf_h1_proxy_ctx *pctx = cf->ctx;
  struct httpreq *req = NULL;
  정수 http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  만약(result)
    goto out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  만약(!result)
    result = Curl_creader_set_null(data);

out:
  만약(result)
    failf(data, "Failed sending CONNECT to proxy");
  만약(req)
    Curl_http_req_free(req);
  반환 result;
}

static CURLcode send_CONNECT(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             struct h1_tunnel_state *ts,
                             bool *done)
{
  const uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  만약(blen <= ts->nsent)
    goto out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  만약(result) {
    만약(result == CURLE_AGAIN)
      result = CURLE_OK;
    goto out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (const char *)buf, nwritten);

out:
  만약(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  반환 result;
}

static CURLcode on_resp_status(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               struct h1_tunnel_state *ts,
                               const char *header)
{
  struct SingleRequest *k = &data->req;

  (void)ts;
  만약(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    반환 CURLE_OK;
  }
  failf(data, "Invalid response header");
  반환 CURLE_WEIRD_SERVER_REPLY;
}

static CURLcode on_resp_header(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               struct h1_tunnel_state *ts,
                               const char *header)
{
  CURLcode result = CURLE_OK;
  struct SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  만약(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    char *auth = Curl_copy_header_value(header);
    만약(!auth)
      반환 CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_input_auth(data, proxy, auth);

    curlx_free(auth);

    만약(result)
      반환 result;
  }
  아니면 만약(checkprefix("Content-Length:", header)) {
    만약(k->httpcode < 300) {
      만약(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    아니면 {
      const char *p = header + CURL_CSTRLEN("Content-Length:");
      만약(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  아니면 만약(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  아니면 만약(checkprefix("Transfer-Encoding:", header)) {
    만약(k->httpcode < 300) {
      만약(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    아니면 만약(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  아니면 만약(is_udp && checkprefix("Capsule-protocol:", header)) {
    만약(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  아니면 만약(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  반환 result;
}

static CURLcode single_header(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              struct h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  const char *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t line_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  const struct SingleRequest *k = &data->req;
  정수 writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, line_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, line_len);
  만약(result)
    반환 result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, line_len);
  만약(result)
    반환 result;

  result = Curl_bump_headersize(data, line_len, TRUE);
  만약(result)
    반환 result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  만약(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    만약(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    아니면 만약((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      만약(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      아니면 만약(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      아니면 {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    아니면 {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    반환 result;
  }

  만약(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  아니면
    result = on_resp_header(cf, data, ts, linep);
  만약(result)
    반환 result;

  curlx_dyn_reset(&ts->rcvbuf);
  반환 result;
}

static CURLcode recv_CONNECT_resp(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  struct h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  정수 error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  동안(ts->keepon) {
    size_t nread;
    char byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    만약(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      반환 CURLE_OK;

    만약(!result)
      result = Curl_pgrsUpdate(data);

    만약(result) {
      ts->keepon = KEEPON_DONE;
      break;
    }

    만약(!nread) {
      만약(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        만약(result)
          반환 result;
        ts->maybe_folded = FALSE;
      }
      만약(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      아니면 {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      break;
    }

    만약(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      만약(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        만약(result)
          반환 result;
        만약(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      아니면 만약(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        만약(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          break;
        }
      }
      continue;
    }

    만약(ts->maybe_folded) {
      만약(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      아니면 {
        result = single_header(cf, data, ts);
        만약(result)
          반환 result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    만약(ts->leading_unfold) {
      만약(ISBLANK(byte))
        /* skip a bit brother */
        continue;
      /* non-blank, insert a space then continue the unfolding */
      만약(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        반환 CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    만약(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      반환 CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    만약(byte != 0x0a)
      continue;
    아니면 {
      const char *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      만약(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        만약(result)
          반환 result;
      }
      아니면
        ts->maybe_folded = TRUE;
    }

    만약(result)
      반환 result;
  } /* while there is buffer left and loop is requested */

  만약(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  만약(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  반환 result;
}

static CURLcode H1_CONNECT(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           struct h1_tunnel_state *ts)
{
  struct connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  만약(tunnel_is_established(ts))
    반환 CURLE_OK;
  만약(tunnel_is_failed(ts))
    반환 CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  do {

    만약(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }

    switch(ts->tunnel_state) {
    case H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      만약(result)
        goto out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    case H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      만약(result || !done)
        goto out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    case H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      만약(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (정수)result);
      만약(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      만약(result || !done)
        goto out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    case H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      만약(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        만약(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          반환 CURLE_AGAIN;
        }
        아니면 {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      break;

    default:
      break;
    }

  } 동안(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  만약(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    만약(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      반환 CURLE_COULDNT_CONNECT;
    }
  }
  아니면 {
    만약(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      반환 CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  만약(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  반환 result;
}

static CURLcode cf_h1_proxy_connect(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  struct cf_h1_proxy_ctx *pctx = cf->ctx;
  struct h1_tunnel_state *ts = pctx->ts;

  만약(cf->connected) {
    *done = TRUE;
    반환 CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  만약(result || !*done)
    반환 result;

  *done = FALSE;
  만약(!ts) {
    result = tunnel_init(cf, data, &ts);
    만약(result)
      반환 result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  만약(result)
    goto out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  만약(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  반환 result;
}

static CURLcode cf_h1_proxy_adjust_pollset(struct Curl_cfilter *cf,
                                           struct Curl_easy *data,
                                           struct easy_pollset *ps)
{
  struct cf_h1_proxy_ctx *pctx = cf->ctx;
  struct h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  만약(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    만약(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      만약(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      아니면
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    아니면
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  아니면 {
    만약(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  반환 result;
}

static bool cf_h1_proxy_data_pending(struct Curl_cfilter *cf,
                                     const struct Curl_easy *data)
{
  반환 cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static void cf_h1_proxy_destroy(struct Curl_cfilter *cf,
                                struct Curl_easy *data)
{
  struct cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  만약(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

static CURLcode cf_h1_proxy_query(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  정수 query, 정수 *pres1, void *pres2)
{
  struct cf_h1_proxy_ctx *pctx = cf->ctx;
  struct h1_tunnel_state *ts = pctx ? pctx->ts : NULL;
  switch(query) {
  case CF_QUERY_HOST_PORT:
    만약(!ts || !ts->dest)
      break;
    *pres1 = (정수)ts->dest->port;
    *((const char **)pres2) = ts->dest->hostname;
    반환 CURLE_OK;
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

struct Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_insert_after(struct Curl_cfilter *cf_at,
                                       struct Curl_easy *data,
                                       struct Curl_peer *peer,
                                       struct Curl_peer *dest,
                                       정수 httpversion,
                                       bool udp_tunnel)
{
  struct Curl_cfilter *cf;
  struct cf_h1_proxy_ctx *pctx;
  struct h1_tunnel_state *ts;
  CURLcode result;

  (void)data;
  만약(!dest)
    반환 CURLE_FAILED_INIT;
  만약((httpversion < 10) || (httpversion >= 20))
    반환 CURLE_FAILED_INIT;

  ts = curlx_calloc(1, sizeof(*ts));
  만약(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, sizeof(*pctx));
  만약(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  만약(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    goto out;
  }
  ts = NULL;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  반환 result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
