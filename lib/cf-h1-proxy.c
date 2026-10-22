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

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#ضمن <curl/curl.h>
#ضمن "urldata.h"
#ضمن "curlx/dynbuf.h"
#ضمن "sendf.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "http_proxy.h"
#ضمن "select.h"
#ضمن "progress.h"
#ضمن "multiif.h"
#ضمن "cfilters.h"
#ضمن "cf-h1-proxy.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "curlx/strparse.h"

تعريفنوع تعداد {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
بنية h1_tunnel_state {
  بنية Curl_peer *dest;
  بنية dynbuf rcvbuf;
  بنية dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  بنية Curl_chunker ch;
  صحيح httpversion;
  تعداد keeponval {
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
بنية cf_h1_proxy_ctx {
  بنية Curl_peer *peer;
  بنية h1_tunnel_state *ts;
};

ساكن bool tunnel_is_established(بنية h1_tunnel_state *ts)
{
  أرجع ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ساكن bool tunnel_is_failed(بنية h1_tunnel_state *ts)
{
  أرجع ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ساكن bool h1_proxy_is_udp(بنية Curl_cfilter *cf)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  أرجع (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ساكن CURLcode tunnel_reinit(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              بنية h1_tunnel_state *ts)
{
  (فراغ)data;
  (فراغ)cf;
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
  أرجع CURLE_OK;
}

ساكن CURLcode tunnel_init(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            بنية h1_tunnel_state **pts)
{
  بنية h1_tunnel_state *ts;

  إذا(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts)
    أرجع CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  أرجع tunnel_reinit(cf, data, ts);
}

ساكن فراغ h1_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               بنية Curl_easy *data)
{
  إذا(ts->tunnel_state == new_state)
    أرجع;
  /* entering this one */
  بدل(new_state) {
  حالة H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    اكسر;

  حالة H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    اكسر;

  حالة H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    اكسر;

  حالة H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    اكسر;

  حالة H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H1_TUNNEL_FAILED:
    إذا(new_state == H1_TUNNEL_FAILED)
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
    اكسر;
  }
}

ساكن فراغ tunnel_free(بنية h1_tunnel_state *ts,
                        بنية Curl_easy *data)
{
  إذا(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

ساكن فراغ cf_tunnel_free(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data)
{
  إذا(cf) {
    بنية cf_h1_proxy_ctx *pctx = cf->ctx;
    بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
    إذا(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = عدم;
    }
  }
}

ساكن bool tunnel_want_send(بنية h1_tunnel_state *ts)
{
  أرجع ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ساكن CURLcode start_CONNECT(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              بنية h1_tunnel_state *ts)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية httpreq *req = عدم;
  صحيح http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  إذا(result)
    اذهب out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  إذا(!result)
    result = Curl_creader_set_null(data);

out:
  إذا(result)
    failf(data, "Failed sending CONNECT to proxy");
  إذا(req)
    Curl_http_req_free(req);
  أرجع result;
}

ساكن CURLcode send_CONNECT(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             بنية h1_tunnel_state *ts,
                             bool *done)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  إذا(blen <= ts->nsent)
    اذهب out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  إذا(result) {
    إذا(result == CURLE_AGAIN)
      result = CURLE_OK;
    اذهب out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ثابت حرف *)buf, nwritten);

out:
  إذا(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  أرجع result;
}

ساكن CURLcode on_resp_status(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *header)
{
  بنية SingleRequest *k = &data->req;

  (فراغ)ts;
  إذا(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    أرجع CURLE_OK;
  }
  failf(data, "Invalid response header");
  أرجع CURLE_WEIRD_SERVER_REPLY;
}

ساكن CURLcode on_resp_header(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *header)
{
  CURLcode result = CURLE_OK;
  بنية SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  إذا(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    حرف *auth = Curl_copy_header_بەها(header);
    إذا(!auth)
      أرجع CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_تێخستن_auth(data, proxy, auth);

    curlx_free(auth);

    إذا(result)
      أرجع result;
  }
  وإلا إذا(checkprefix("Content-Length:", header)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا {
      ثابت حرف *p = header + CURL_CSTRLEN("Content-Length:");
      إذا(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  وإلا إذا(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  وإلا إذا(checkprefix("Transfer-Encoding:", header)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا إذا(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  وإلا إذا(is_udp && checkprefix("Capsule-protocol:", header)) {
    إذا(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  وإلا إذا(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  أرجع result;
}

ساكن CURLcode single_header(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              بنية h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t هێڵ_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت بنية SingleRequest *k = &data->req;
  صحيح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, هێڵ_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, هێڵ_len);
  إذا(result)
    أرجع result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, هێڵ_len);
  إذا(result)
    أرجع result;

  result = Curl_bump_headersize(data, هێڵ_len, TRUE);
  إذا(result)
    أرجع result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  إذا(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    إذا(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    وإلا إذا((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      إذا(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      وإلا إذا(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      وإلا {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    وإلا {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    أرجع result;
  }

  إذا(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  وإلا
    result = on_resp_header(cf, data, ts, linep);
  إذا(result)
    أرجع result;

  curlx_dyn_reset(&ts->rcvbuf);
  أرجع result;
}

ساكن CURLcode recv_CONNECT_resp(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  صحيح error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  طالما(ts->keepon) {
    size_t nread;
    حرف byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    إذا(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      أرجع CURLE_OK;

    إذا(!result)
      result = Curl_pgrsUpdate(data);

    إذا(result) {
      ts->keepon = KEEPON_DONE;
      اكسر;
    }

    إذا(!nread) {
      إذا(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        إذا(result)
          أرجع result;
        ts->maybe_folded = FALSE;
      }
      إذا(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      وإلا {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      اكسر;
    }

    إذا(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      إذا(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        إذا(result)
          أرجع result;
        إذا(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      وإلا إذا(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        إذا(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          اكسر;
        }
      }
      استمر;
    }

    إذا(ts->maybe_folded) {
      إذا(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      وإلا {
        result = single_header(cf, data, ts);
        إذا(result)
          أرجع result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    إذا(ts->leading_unfold) {
      إذا(ISBLANK(byte))
        /* skip a bit brother */
        استمر;
      /* non-blank, insert a space then continue the unfolding */
      إذا(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        أرجع CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    إذا(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      أرجع CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    إذا(byte != 0x0a)
      استمر;
    وإلا {
      ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      إذا(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        إذا(result)
          أرجع result;
      }
      وإلا
        ts->maybe_folded = TRUE;
    }

    إذا(result)
      أرجع result;
  } /* while there is buffer left and loop is requested */

  إذا(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  إذا(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  أرجع result;
}

ساكن CURLcode H1_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية h1_tunnel_state *ts)
{
  بنية connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  إذا(tunnel_is_established(ts))
    أرجع CURLE_OK;
  إذا(tunnel_is_failed(ts))
    أرجع CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  افعل {

    إذا(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      اذهب out;
    }

    بدل(ts->tunnel_state) {
    حالة H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      إذا(result)
        اذهب out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    حالة H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      إذا(result || !done)
        اذهب out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    حالة H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      إذا(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (صحيح)result);
      إذا(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      إذا(result || !done)
        اذهب out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    حالة H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      إذا(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        إذا(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          أرجع CURLE_AGAIN;
        }
        وإلا {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      اكسر;

    افتراضي:
      اكسر;
    }

  } طالما(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  إذا(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    إذا(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    إذا(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  إذا(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  أرجع result;
}

ساكن CURLcode cf_h1_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  إذا(result || !*done)
    أرجع result;

  *done = FALSE;
  إذا(!ts) {
    result = tunnel_init(cf, data, &ts);
    إذا(result)
      أرجع result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  إذا(result)
    اذهب out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  إذا(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  أرجع result;
}

ساكن CURLcode cf_h1_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data,
                                           بنية easy_pollset *ps)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  إذا(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    إذا(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      إذا(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      وإلا
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    وإلا
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  وإلا {
    إذا(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  أرجع result;
}

ساكن bool cf_h1_proxy_data_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *data)
{
  أرجع cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ساكن فراغ cf_h1_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  إذا(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ساكن CURLcode cf_h1_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
  بدل(query) {
  حالة CF_QUERY_HOST_PORT:
    إذا(!ts || !ts->dest)
      اكسر;
    *pres1 = (صحيح)ts->dest->port;
    *((ثابت حرف **)pres2) = ts->dest->hostname;
    أرجع CURLE_OK;
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

بنية Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_زیادکردن_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *data,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       صحيح httpversion,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf;
  بنية cf_h1_proxy_ctx *pctx;
  بنية h1_tunnel_state *ts;
  CURLcode result;

  (فراغ)data;
  إذا(!dest)
    أرجع CURLE_FAILED_INIT;
  إذا((httpversion < 10) || (httpversion >= 20))
    أرجع CURLE_FAILED_INIT;

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, حجم(*pctx));
  إذا(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  إذا(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    اذهب out;
  }
  ts = عدم;
  Curl_conn_cf_زیادکردن_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  أرجع result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
