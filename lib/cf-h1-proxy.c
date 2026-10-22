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

#જો !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#અન્તર્ભાવ <curl/curl.h>
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-h1-proxy.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curlx/strparse.h"

પ્રકારવ્યાખ્યા ગણના {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
સંરચના h1_tunnel_state {
  સંરચના Curl_peer *dest;
  સંરચના dynbuf rcvbuf;
  સંરચના dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  સંરચના Curl_chunker ch;
  પૂર્ણાંક httpversion;
  ગણના keeponval {
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
સંરચના cf_h1_proxy_ctx {
  સંરચના Curl_peer *peer;
  સંરચના h1_tunnel_state *ts;
};

સ્થિર bool tunnel_is_established(સંરચના h1_tunnel_state *ts)
{
  પાછા ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

સ્થિર bool tunnel_is_failed(સંરચના h1_tunnel_state *ts)
{
  પાછા ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

સ્થિર bool h1_proxy_is_udp(સંરચના Curl_cfilter *cf)
{
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
  પાછા (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

સ્થિર CURLcode tunnel_reinit(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              સંરચના h1_tunnel_state *ts)
{
  (શૂન્ય)data;
  (શૂન્ય)cf;
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
  પાછા CURLE_OK;
}

સ્થિર CURLcode tunnel_init(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના h1_tunnel_state **pts)
{
  સંરચના h1_tunnel_state *ts;

  જો(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, કદ(*ts));
  જો(!ts)
    પાછા CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  પાછા tunnel_reinit(cf, data, ts);
}

સ્થિર શૂન્ય h1_tunnel_go_state(સંરચના Curl_cfilter *cf,
                               સંરચના h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               સંરચના Curl_easy *data)
{
  જો(ts->tunnel_state == new_state)
    પાછા;
  /* entering this one */
  પસંદગી(new_state) {
  સ્થિતિ H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    તોડો;

  સ્થિતિ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    તોડો;

  સ્થિતિ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    તોડો;

  સ્થિતિ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    તોડો;

  સ્થિતિ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  સ્થિતિ H1_TUNNEL_FAILED:
    જો(new_state == H1_TUNNEL_FAILED)
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
    તોડો;
  }
}

સ્થિર શૂન્ય tunnel_free(સંરચના h1_tunnel_state *ts,
                        સંરચના Curl_easy *data)
{
  જો(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

સ્થિર શૂન્ય cf_tunnel_free(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data)
{
  જો(cf) {
    સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
    સંરચના h1_tunnel_state *ts = pctx ? pctx->ts : ખાલી;
    જો(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = ખાલી;
    }
  }
}

સ્થિર bool tunnel_want_send(સંરચના h1_tunnel_state *ts)
{
  પાછા ts->tunnel_state == H1_TUNNEL_CONNECT;
}

સ્થિર CURLcode start_CONNECT(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              સંરચના h1_tunnel_state *ts)
{
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
  સંરચના httpreq *req = ખાલી;
  પૂર્ણાંક http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  જો(result)
    જાઓ out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  જો(!result)
    result = Curl_creader_set_null(data);

out:
  જો(result)
    failf(data, "Failed sending CONNECT to proxy");
  જો(req)
    Curl_http_req_free(req);
  પાછા result;
}

સ્થિર CURLcode send_CONNECT(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             સંરચના h1_tunnel_state *ts,
                             bool *done)
{
  અચલ uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  જો(blen <= ts->nsent)
    જાઓ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN)
      result = CURLE_OK;
    જાઓ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (અચલ અક્ષર *)buf, nwritten);

out:
  જો(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  પાછા result;
}

સ્થિર CURLcode on_resp_status(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h1_tunnel_state *ts,
                               અચલ અક્ષર *header)
{
  સંરચના SingleRequest *k = &data->req;

  (શૂન્ય)ts;
  જો(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    પાછા CURLE_OK;
  }
  failf(data, "Invalid response header");
  પાછા CURLE_WEIRD_SERVER_REPLY;
}

સ્થિર CURLcode on_resp_header(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h1_tunnel_state *ts,
                               અચલ અક્ષર *header)
{
  CURLcode result = CURLE_OK;
  સંરચના SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  જો(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    અક્ષર *auth = Curl_copy_header_કિંમત(header);
    જો(!auth)
      પાછા CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_આગત_auth(data, proxy, auth);

    curlx_free(auth);

    જો(result)
      પાછા result;
  }
  નહીંતર જો(checkprefix("Content-Length:", header)) {
    જો(k->httpcode < 300) {
      જો(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        પાછા CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    નહીંતર {
      અચલ અક્ષર *p = header + CURL_CSTRLEN("Content-Length:");
      જો(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        પાછા CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  નહીંતર જો(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  નહીંતર જો(checkprefix("Transfer-Encoding:", header)) {
    જો(k->httpcode < 300) {
      જો(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        પાછા CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    નહીંતર જો(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  નહીંતર જો(is_udp && checkprefix("Capsule-protocol:", header)) {
    જો(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  નહીંતર જો(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  પાછા result;
}

સ્થિર CURLcode single_header(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              સંરચના h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t લીટી_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  અચલ સંરચના SingleRequest *k = &data->req;
  પૂર્ણાંક writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, લીટી_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, લીટી_len);
  જો(result)
    પાછા result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, લીટી_len);
  જો(result)
    પાછા result;

  result = Curl_bump_headersize(data, લીટી_len, TRUE);
  જો(result)
    પાછા result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  જો(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    જો(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    નહીંતર જો((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      જો(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      નહીંતર જો(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      નહીંતર {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    નહીંતર {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    પાછા result;
  }

  જો(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  નહીંતર
    result = on_resp_header(cf, data, ts, linep);
  જો(result)
    પાછા result;

  curlx_dyn_reset(&ts->rcvbuf);
  પાછા result;
}

સ્થિર CURLcode recv_CONNECT_resp(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  જ્યારે(ts->keepon) {
    size_t nread;
    અક્ષર byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    જો(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      પાછા CURLE_OK;

    જો(!result)
      result = Curl_pgrsUpdate(data);

    જો(result) {
      ts->keepon = KEEPON_DONE;
      તોડો;
    }

    જો(!nread) {
      જો(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        જો(result)
          પાછા result;
        ts->maybe_folded = FALSE;
      }
      જો(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      નહીંતર {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      તોડો;
    }

    જો(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      જો(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        જો(result)
          પાછા result;
        જો(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      નહીંતર જો(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        જો(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          તોડો;
        }
      }
      ચાલુ;
    }

    જો(ts->maybe_folded) {
      જો(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      નહીંતર {
        result = single_header(cf, data, ts);
        જો(result)
          પાછા result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    જો(ts->leading_unfold) {
      જો(ISBLANK(byte))
        /* skip a bit brother */
        ચાલુ;
      /* non-blank, insert a space then continue the unfolding */
      જો(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        પાછા CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    જો(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      પાછા CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    જો(byte != 0x0a)
      ચાલુ;
    નહીંતર {
      અચલ અક્ષર *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      જો(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        જો(result)
          પાછા result;
      }
      નહીંતર
        ts->maybe_folded = TRUE;
    }

    જો(result)
      પાછા result;
  } /* while there is buffer left and loop is requested */

  જો(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  જો(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  પાછા result;
}

સ્થિર CURLcode H1_CONNECT(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના h1_tunnel_state *ts)
{
  સંરચના connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  જો(tunnel_is_established(ts))
    પાછા CURLE_OK;
  જો(tunnel_is_failed(ts))
    પાછા CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  કરો {

    જો(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      જાઓ out;
    }

    પસંદગી(ts->tunnel_state) {
    સ્થિતિ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      જો(result)
        જાઓ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    સ્થિતિ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      જો(result || !done)
        જાઓ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    સ્થિતિ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      જો(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (પૂર્ણાંક)result);
      જો(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      જો(result || !done)
        જાઓ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    સ્થિતિ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      જો(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        જો(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          પાછા CURLE_AGAIN;
        }
        નહીંતર {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      તોડો;

    મૂળભૂત:
      તોડો;
    }

  } જ્યારે(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  જો(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    જો(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      પાછા CURLE_COULDNT_CONNECT;
    }
  }
  નહીંતર {
    જો(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      પાછા CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  જો(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  પાછા result;
}

સ્થિર CURLcode cf_h1_proxy_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
  સંરચના h1_tunnel_state *ts = pctx->ts;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  જો(result || !*done)
    પાછા result;

  *done = FALSE;
  જો(!ts) {
    result = tunnel_init(cf, data, &ts);
    જો(result)
      પાછા result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  જો(result)
    જાઓ out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  જો(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  પાછા result;
}

સ્થિર CURLcode cf_h1_proxy_adjust_pollset(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના easy_pollset *ps)
{
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
  સંરચના h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  જો(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    જો(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      જો(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      નહીંતર
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    નહીંતર
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  નહીંતર {
    જો(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  પાછા result;
}

સ્થિર bool cf_h1_proxy_data_pending(સંરચના Curl_cfilter *cf,
                                     અચલ સંરચના Curl_easy *data)
{
  પાછા cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સ્થિર શૂન્ય cf_h1_proxy_destroy(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  જો(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

સ્થિર CURLcode cf_h1_proxy_query(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_h1_proxy_ctx *pctx = cf->ctx;
  સંરચના h1_tunnel_state *ts = pctx ? pctx->ts : ખાલી;
  પસંદગી(query) {
  સ્થિતિ CF_QUERY_HOST_PORT:
    જો(!ts || !ts->dest)
      તોડો;
    *pres1 = (પૂર્ણાંક)ts->dest->port;
    *((અચલ અક્ષર **)pres2) = ts->dest->hostname;
    પાછા CURLE_OK;
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

સંરચના Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                       સંરચના Curl_easy *data,
                                       સંરચના Curl_peer *peer,
                                       સંરચના Curl_peer *dest,
                                       પૂર્ણાંક httpversion,
                                       bool udp_tunnel)
{
  સંરચના Curl_cfilter *cf;
  સંરચના cf_h1_proxy_ctx *pctx;
  સંરચના h1_tunnel_state *ts;
  CURLcode result;

  (શૂન્ય)data;
  જો(!dest)
    પાછા CURLE_FAILED_INIT;
  જો((httpversion < 10) || (httpversion >= 20))
    પાછા CURLE_FAILED_INIT;

  ts = curlx_calloc(1, કદ(*ts));
  જો(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, કદ(*pctx));
  જો(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  જો(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    જાઓ out;
  }
  ts = ખાલી;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  પાછા result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
