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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#அந்தர்பாவ <curl/curl.h>
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-h1-proxy.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "curlx/strparse.h"

வகைவரை எண்ணிக்கை {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
கட்டமைப்பு h1_tunnel_state {
  கட்டமைப்பு Curl_peer *dest;
  கட்டமைப்பு dynbuf rcvbuf;
  கட்டமைப்பு dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  கட்டமைப்பு Curl_chunker ch;
  முழு httpversion;
  எண்ணிக்கை keeponval {
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
கட்டமைப்பு cf_h1_proxy_ctx {
  கட்டமைப்பு Curl_peer *peer;
  கட்டமைப்பு h1_tunnel_state *ts;
};

நிலையான bool tunnel_is_established(கட்டமைப்பு h1_tunnel_state *ts)
{
  திரும்பு ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

நிலையான bool tunnel_is_failed(கட்டமைப்பு h1_tunnel_state *ts)
{
  திரும்பு ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

நிலையான bool h1_proxy_is_udp(கட்டமைப்பு Curl_cfilter *cf)
{
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
  திரும்பு (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

நிலையான CURLcode tunnel_reinit(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு h1_tunnel_state *ts)
{
  (வெறுமை)data;
  (வெறுமை)cf;
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
  திரும்பு CURLE_OK;
}

நிலையான CURLcode tunnel_init(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு h1_tunnel_state **pts)
{
  கட்டமைப்பு h1_tunnel_state *ts;

  எனில்(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, அளவு(*ts));
  எனில்(!ts)
    திரும்பு CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  திரும்பு tunnel_reinit(cf, data, ts);
}

நிலையான வெறுமை h1_tunnel_go_state(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               கட்டமைப்பு Curl_easy *data)
{
  எனில்(ts->tunnel_state == new_state)
    திரும்பு;
  /* entering this one */
  தேர்வு(new_state) {
  நிலை H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    நிறுத்து;

  நிலை H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    நிறுத்து;

  நிலை H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    நிறுத்து;

  நிலை H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    நிறுத்து;

  நிலை H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  நிலை H1_TUNNEL_FAILED:
    எனில்(new_state == H1_TUNNEL_FAILED)
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
    நிறுத்து;
  }
}

நிலையான வெறுமை tunnel_free(கட்டமைப்பு h1_tunnel_state *ts,
                        கட்டமைப்பு Curl_easy *data)
{
  எனில்(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

நிலையான வெறுமை cf_tunnel_free(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data)
{
  எனில்(cf) {
    கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
    கட்டமைப்பு h1_tunnel_state *ts = pctx ? pctx->ts : வெற்று;
    எனில்(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = வெற்று;
    }
  }
}

நிலையான bool tunnel_want_send(கட்டமைப்பு h1_tunnel_state *ts)
{
  திரும்பு ts->tunnel_state == H1_TUNNEL_CONNECT;
}

நிலையான CURLcode start_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு h1_tunnel_state *ts)
{
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு httpreq *req = வெற்று;
  முழு http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  எனில்(result)
    செல் out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  எனில்(!result)
    result = Curl_creader_set_null(data);

out:
  எனில்(result)
    failf(data, "Failed sending CONNECT to proxy");
  எனில்(req)
    Curl_http_req_free(req);
  திரும்பு result;
}

நிலையான CURLcode send_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு h1_tunnel_state *ts,
                             bool *done)
{
  மாறிலி uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  எனில்(blen <= ts->nsent)
    செல் out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN)
      result = CURLE_OK;
    செல் out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (மாறிலி எழுத்து *)buf, nwritten);

out:
  எனில்(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  திரும்பு result;
}

நிலையான CURLcode on_resp_status(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h1_tunnel_state *ts,
                               மாறிலி எழுத்து *header)
{
  கட்டமைப்பு SingleRequest *k = &data->req;

  (வெறுமை)ts;
  எனில்(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    திரும்பு CURLE_OK;
  }
  failf(data, "Invalid response header");
  திரும்பு CURLE_WEIRD_SERVER_REPLY;
}

நிலையான CURLcode on_resp_header(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h1_tunnel_state *ts,
                               மாறிலி எழுத்து *header)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  எனில்(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    எழுத்து *auth = Curl_copy_header_மதிப்பு(header);
    எனில்(!auth)
      திரும்பு CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_உள்ளீடு_auth(data, proxy, auth);

    curlx_free(auth);

    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில் எனில்(checkprefix("Content-Length:", header)) {
    எனில்(k->httpcode < 300) {
      எனில்(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    இல்லையெனில் {
      மாறிலி எழுத்து *p = header + CURL_CSTRLEN("Content-Length:");
      எனில்(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  இல்லையெனில் எனில்(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  இல்லையெனில் எனில்(checkprefix("Transfer-Encoding:", header)) {
    எனில்(k->httpcode < 300) {
      எனில்(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    இல்லையெனில் எனில்(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  இல்லையெனில் எனில்(is_udp && checkprefix("Capsule-protocol:", header)) {
    எனில்(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  இல்லையெனில் எனில்(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  திரும்பு result;
}

நிலையான CURLcode single_header(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t வரி_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  மாறிலி கட்டமைப்பு SingleRequest *k = &data->req;
  முழு writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, வரி_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, வரி_len);
  எனில்(result)
    திரும்பு result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, வரி_len);
  எனில்(result)
    திரும்பு result;

  result = Curl_bump_headersize(data, வரி_len, TRUE);
  எனில்(result)
    திரும்பு result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  எனில்(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    எனில்(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    இல்லையெனில் எனில்((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      எனில்(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      இல்லையெனில் எனில்(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      இல்லையெனில் {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    இல்லையெனில் {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    திரும்பு result;
  }

  எனில்(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  இல்லையெனில்
    result = on_resp_header(cf, data, ts, linep);
  எனில்(result)
    திரும்பு result;

  curlx_dyn_reset(&ts->rcvbuf);
  திரும்பு result;
}

நிலையான CURLcode recv_CONNECT_resp(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  முழு error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  போது(ts->keepon) {
    size_t nread;
    எழுத்து byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    எனில்(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      திரும்பு CURLE_OK;

    எனில்(!result)
      result = Curl_pgrsUpdate(data);

    எனில்(result) {
      ts->keepon = KEEPON_DONE;
      நிறுத்து;
    }

    எனில்(!nread) {
      எனில்(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        எனில்(result)
          திரும்பு result;
        ts->maybe_folded = FALSE;
      }
      எனில்(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      இல்லையெனில் {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      நிறுத்து;
    }

    எனில்(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      எனில்(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        எனில்(result)
          திரும்பு result;
        எனில்(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      இல்லையெனில் எனில்(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        எனில்(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          நிறுத்து;
        }
      }
      தொடர்;
    }

    எனில்(ts->maybe_folded) {
      எனில்(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      இல்லையெனில் {
        result = single_header(cf, data, ts);
        எனில்(result)
          திரும்பு result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    எனில்(ts->leading_unfold) {
      எனில்(ISBLANK(byte))
        /* skip a bit brother */
        தொடர்;
      /* non-blank, insert a space then continue the unfolding */
      எனில்(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        திரும்பு CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    எனில்(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      திரும்பு CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    எனில்(byte != 0x0a)
      தொடர்;
    இல்லையெனில் {
      மாறிலி எழுத்து *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      எனில்(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        எனில்(result)
          திரும்பு result;
      }
      இல்லையெனில்
        ts->maybe_folded = TRUE;
    }

    எனில்(result)
      திரும்பு result;
  } /* while there is buffer left and loop is requested */

  எனில்(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  எனில்(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  திரும்பு result;
}

நிலையான CURLcode H1_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு h1_tunnel_state *ts)
{
  கட்டமைப்பு connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  எனில்(tunnel_is_established(ts))
    திரும்பு CURLE_OK;
  எனில்(tunnel_is_failed(ts))
    திரும்பு CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  செய் {

    எனில்(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      செல் out;
    }

    தேர்வு(ts->tunnel_state) {
    நிலை H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      எனில்(result)
        செல் out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    நிலை H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      எனில்(result || !done)
        செல் out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    நிலை H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      எனில்(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (முழு)result);
      எனில்(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      எனில்(result || !done)
        செல் out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    நிலை H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      எனில்(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        எனில்(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          திரும்பு CURLE_AGAIN;
        }
        இல்லையெனில் {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      நிறுத்து;

    இயல்பு:
      நிறுத்து;
    }

  } போது(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  எனில்(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    எனில்(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      திரும்பு CURLE_COULDNT_CONNECT;
    }
  }
  இல்லையெனில் {
    எனில்(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      திரும்பு CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  எனில்(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  திரும்பு result;
}

நிலையான CURLcode cf_h1_proxy_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு h1_tunnel_state *ts = pctx->ts;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  எனில்(result || !*done)
    திரும்பு result;

  *done = FALSE;
  எனில்(!ts) {
    result = tunnel_init(cf, data, &ts);
    எனில்(result)
      திரும்பு result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  எனில்(result)
    செல் out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  எனில்(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  திரும்பு result;
}

நிலையான CURLcode cf_h1_proxy_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  எனில்(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    எனில்(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      எனில்(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      இல்லையெனில்
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    இல்லையெனில்
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  இல்லையெனில் {
    எனில்(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  திரும்பு result;
}

நிலையான bool cf_h1_proxy_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                     மாறிலி கட்டமைப்பு Curl_easy *data)
{
  திரும்பு cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

நிலையான வெறுமை cf_h1_proxy_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  எனில்(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

நிலையான CURLcode cf_h1_proxy_query(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_h1_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு h1_tunnel_state *ts = pctx ? pctx->ts : வெற்று;
  தேர்வு(query) {
  நிலை CF_QUERY_HOST_PORT:
    எனில்(!ts || !ts->dest)
      நிறுத்து;
    *pres1 = (முழு)ts->dest->port;
    *((மாறிலி எழுத்து **)pres2) = ts->dest->hostname;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

கட்டமைப்பு Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_peer *peer,
                                       கட்டமைப்பு Curl_peer *dest,
                                       முழு httpversion,
                                       bool udp_tunnel)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு cf_h1_proxy_ctx *pctx;
  கட்டமைப்பு h1_tunnel_state *ts;
  CURLcode result;

  (வெறுமை)data;
  எனில்(!dest)
    திரும்பு CURLE_FAILED_INIT;
  எனில்((httpversion < 10) || (httpversion >= 20))
    திரும்பு CURLE_FAILED_INIT;

  ts = curlx_calloc(1, அளவு(*ts));
  எனில்(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, அளவு(*pctx));
  எனில்(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  எனில்(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    செல் out;
  }
  ts = வெற்று;
  Curl_conn_cf_செருகு_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  திரும்பு result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
