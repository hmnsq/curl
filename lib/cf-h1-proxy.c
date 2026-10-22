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

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#شامل <curl/curl.h>
#شامل "urldata.h"
#شامل "curlx/dynbuf.h"
#شامل "sendf.h"
#شامل "http.h"
#شامل "http1.h"
#شامل "http_proxy.h"
#شامل "select.h"
#شامل "progress.h"
#شامل "multiif.h"
#شامل "cfilters.h"
#شامل "cf-h1-proxy.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "curlx/strparse.h"

تعریفنوع شمارش {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ساختار h1_tunnel_state {
  ساختار Curl_peer *dest;
  ساختار dynbuf rcvbuf;
  ساختار dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  ساختار Curl_chunker ch;
  صحیح httpversion;
  شمارش keeponval {
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
ساختار cf_h1_proxy_ctx {
  ساختار Curl_peer *peer;
  ساختار h1_tunnel_state *ts;
};

ایستا bool tunnel_is_established(ساختار h1_tunnel_state *ts)
{
  بازگشت ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ایستا bool tunnel_is_failed(ساختار h1_tunnel_state *ts)
{
  بازگشت ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ایستا bool h1_proxy_is_udp(ساختار Curl_cfilter *cf)
{
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
  بازگشت (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ایستا CURLcode tunnel_reinit(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ساختار h1_tunnel_state *ts)
{
  (تهی)data;
  (تهی)cf;
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
  بازگشت CURLE_OK;
}

ایستا CURLcode tunnel_init(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ساختار h1_tunnel_state **pts)
{
  ساختار h1_tunnel_state *ts;

  اگر(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, اندازه(*ts));
  اگر(!ts)
    بازگشت CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  بازگشت tunnel_reinit(cf, data, ts);
}

ایستا تهی h1_tunnel_go_state(ساختار Curl_cfilter *cf,
                               ساختار h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ساختار Curl_easy *data)
{
  اگر(ts->tunnel_state == new_state)
    بازگشت;
  /* entering this one */
  انتخاب(new_state) {
  حالت H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    بشکن;

  حالت H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    بشکن;

  حالت H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    بشکن;

  حالت H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    بشکن;

  حالت H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالت H1_TUNNEL_FAILED:
    اگر(new_state == H1_TUNNEL_FAILED)
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
    بشکن;
  }
}

ایستا تهی tunnel_free(ساختار h1_tunnel_state *ts,
                        ساختار Curl_easy *data)
{
  اگر(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

ایستا تهی cf_tunnel_free(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data)
{
  اگر(cf) {
    ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
    ساختار h1_tunnel_state *ts = pctx ? pctx->ts : پوچ;
    اگر(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = پوچ;
    }
  }
}

ایستا bool tunnel_want_send(ساختار h1_tunnel_state *ts)
{
  بازگشت ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ایستا CURLcode start_CONNECT(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ساختار h1_tunnel_state *ts)
{
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
  ساختار httpreq *req = پوچ;
  صحیح http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  اگر(result)
    برو out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  اگر(!result)
    result = Curl_creader_set_null(data);

out:
  اگر(result)
    failf(data, "Failed sending CONNECT to proxy");
  اگر(req)
    Curl_http_req_free(req);
  بازگشت result;
}

ایستا CURLcode send_CONNECT(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ساختار h1_tunnel_state *ts,
                             bool *done)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  اگر(blen <= ts->nsent)
    برو out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN)
      result = CURLE_OK;
    برو out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ثابت نویسه *)buf, nwritten);

out:
  اگر(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  بازگشت result;
}

ایستا CURLcode on_resp_status(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h1_tunnel_state *ts,
                               ثابت نویسه *header)
{
  ساختار SingleRequest *k = &data->req;

  (تهی)ts;
  اگر(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    بازگشت CURLE_OK;
  }
  failf(data, "Invalid response header");
  بازگشت CURLE_WEIRD_SERVER_REPLY;
}

ایستا CURLcode on_resp_header(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h1_tunnel_state *ts,
                               ثابت نویسه *header)
{
  CURLcode result = CURLE_OK;
  ساختار SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  اگر(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    نویسه *auth = Curl_copy_header_مقدار(header);
    اگر(!auth)
      بازگشت CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_درون_داد_auth(data, proxy, auth);

    curlx_free(auth);

    اگر(result)
      بازگشت result;
  }
  وگرنه اگر(checkprefix("Content-Length:", header)) {
    اگر(k->httpcode < 300) {
      اگر(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    وگرنه {
      ثابت نویسه *p = header + CURL_CSTRLEN("Content-Length:");
      اگر(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  وگرنه اگر(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  وگرنه اگر(checkprefix("Transfer-Encoding:", header)) {
    اگر(k->httpcode < 300) {
      اگر(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    وگرنه اگر(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  وگرنه اگر(is_udp && checkprefix("Capsule-protocol:", header)) {
    اگر(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  وگرنه اگر(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  بازگشت result;
}

ایستا CURLcode single_header(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ساختار h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t سطر_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت ساختار SingleRequest *k = &data->req;
  صحیح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, سطر_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, سطر_len);
  اگر(result)
    بازگشت result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, سطر_len);
  اگر(result)
    بازگشت result;

  result = Curl_bump_headersize(data, سطر_len, TRUE);
  اگر(result)
    بازگشت result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  اگر(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    اگر(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    وگرنه اگر((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      اگر(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      وگرنه اگر(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      وگرنه {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    وگرنه {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    بازگشت result;
  }

  اگر(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  وگرنه
    result = on_resp_header(cf, data, ts, linep);
  اگر(result)
    بازگشت result;

  curlx_dyn_reset(&ts->rcvbuf);
  بازگشت result;
}

ایستا CURLcode recv_CONNECT_resp(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  صحیح error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  وقتی(ts->keepon) {
    size_t nread;
    نویسه byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    اگر(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      بازگشت CURLE_OK;

    اگر(!result)
      result = Curl_pgrsUpdate(data);

    اگر(result) {
      ts->keepon = KEEPON_DONE;
      بشکن;
    }

    اگر(!nread) {
      اگر(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        اگر(result)
          بازگشت result;
        ts->maybe_folded = FALSE;
      }
      اگر(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      وگرنه {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      بشکن;
    }

    اگر(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      اگر(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        اگر(result)
          بازگشت result;
        اگر(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      وگرنه اگر(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        اگر(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          بشکن;
        }
      }
      ادامه;
    }

    اگر(ts->maybe_folded) {
      اگر(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      وگرنه {
        result = single_header(cf, data, ts);
        اگر(result)
          بازگشت result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    اگر(ts->leading_unfold) {
      اگر(ISBLANK(byte))
        /* skip a bit brother */
        ادامه;
      /* non-blank, insert a space then continue the unfolding */
      اگر(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        بازگشت CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    اگر(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      بازگشت CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    اگر(byte != 0x0a)
      ادامه;
    وگرنه {
      ثابت نویسه *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      اگر(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        اگر(result)
          بازگشت result;
      }
      وگرنه
        ts->maybe_folded = TRUE;
    }

    اگر(result)
      بازگشت result;
  } /* while there is buffer left and loop is requested */

  اگر(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  اگر(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  بازگشت result;
}

ایستا CURLcode H1_CONNECT(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار h1_tunnel_state *ts)
{
  ساختار connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  اگر(tunnel_is_established(ts))
    بازگشت CURLE_OK;
  اگر(tunnel_is_failed(ts))
    بازگشت CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  انجامبده {

    اگر(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      برو out;
    }

    انتخاب(ts->tunnel_state) {
    حالت H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      اگر(result)
        برو out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    حالت H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      اگر(result || !done)
        برو out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    حالت H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      اگر(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (صحیح)result);
      اگر(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      اگر(result || !done)
        برو out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    حالت H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      اگر(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        اگر(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          بازگشت CURLE_AGAIN;
        }
        وگرنه {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      بشکن;

    پیشفرض:
      بشکن;
    }

  } وقتی(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  اگر(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    اگر(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      بازگشت CURLE_COULDNT_CONNECT;
    }
  }
  وگرنه {
    اگر(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      بازگشت CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  اگر(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  بازگشت result;
}

ایستا CURLcode cf_h1_proxy_connect(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
  ساختار h1_tunnel_state *ts = pctx->ts;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  اگر(result || !*done)
    بازگشت result;

  *done = FALSE;
  اگر(!ts) {
    result = tunnel_init(cf, data, &ts);
    اگر(result)
      بازگشت result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  اگر(result)
    برو out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  اگر(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  بازگشت result;
}

ایستا CURLcode cf_h1_proxy_adjust_pollset(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data,
                                           ساختار easy_pollset *ps)
{
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
  ساختار h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  اگر(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    اگر(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      اگر(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      وگرنه
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    وگرنه
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  وگرنه {
    اگر(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  بازگشت result;
}

ایستا bool cf_h1_proxy_data_pending(ساختار Curl_cfilter *cf,
                                     ثابت ساختار Curl_easy *data)
{
  بازگشت cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ایستا تهی cf_h1_proxy_destroy(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  اگر(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ایستا CURLcode cf_h1_proxy_query(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_h1_proxy_ctx *pctx = cf->ctx;
  ساختار h1_tunnel_state *ts = pctx ? pctx->ts : پوچ;
  انتخاب(query) {
  حالت CF_QUERY_HOST_PORT:
    اگر(!ts || !ts->dest)
      بشکن;
    *pres1 = (صحیح)ts->dest->port;
    *((ثابت نویسه **)pres2) = ts->dest->hostname;
    بازگشت CURLE_OK;
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

ساختار Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                       ساختار Curl_easy *data,
                                       ساختار Curl_peer *peer,
                                       ساختار Curl_peer *dest,
                                       صحیح httpversion,
                                       bool udp_tunnel)
{
  ساختار Curl_cfilter *cf;
  ساختار cf_h1_proxy_ctx *pctx;
  ساختار h1_tunnel_state *ts;
  CURLcode result;

  (تهی)data;
  اگر(!dest)
    بازگشت CURLE_FAILED_INIT;
  اگر((httpversion < 10) || (httpversion >= 20))
    بازگشت CURLE_FAILED_INIT;

  ts = curlx_calloc(1, اندازه(*ts));
  اگر(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, اندازه(*pctx));
  اگر(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  اگر(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    برو out;
  }
  ts = پوچ;
  Curl_conn_cf_درج_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  بازگشت result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
