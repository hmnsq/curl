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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#ಅನ್ತರ್ಭಾವ <curl/curl.h>
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-h1-proxy.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ಸಂರಚನಾ h1_tunnel_state {
  ಸಂರಚನಾ Curl_peer *dest;
  ಸಂರಚನಾ dynbuf rcvbuf;
  ಸಂರಚನಾ dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  ಸಂರಚನಾ Curl_chunker ch;
  ಪೂರ್ಣಾಙ್ಕ httpversion;
  ಗಣನಾ keeponval {
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
ಸಂರಚನಾ cf_h1_proxy_ctx {
  ಸಂರಚನಾ Curl_peer *peer;
  ಸಂರಚನಾ h1_tunnel_state *ts;
};

ಸ್ಥಿರ bool tunnel_is_established(ಸಂರಚನಾ h1_tunnel_state *ts)
{
  ಪ್ರತ್ಯಾಗಮ ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ಸ್ಥಿರ bool tunnel_is_failed(ಸಂರಚನಾ h1_tunnel_state *ts)
{
  ಪ್ರತ್ಯಾಗಮ ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ಸ್ಥಿರ bool h1_proxy_is_udp(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಪ್ರತ್ಯಾಗಮ (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ಸ್ಥಿರ CURLcode tunnel_reinit(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ h1_tunnel_state *ts)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)cf;
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
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tunnel_init(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ h1_tunnel_state **pts)
{
  ಸಂರಚನಾ h1_tunnel_state *ts;

  ಯದಿ(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, ಪರಿಮಾಣ(*ts));
  ಯದಿ(!ts)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  ಪ್ರತ್ಯಾಗಮ tunnel_reinit(cf, data, ts);
}

ಸ್ಥಿರ ಶೂನ್ಯ h1_tunnel_go_state(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(ts->tunnel_state == new_state)
    ಪ್ರತ್ಯಾಗಮ;
  /* entering this one */
  ವಿಕಲ್ಪ(new_state) {
  ಪಕ್ಷ H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ಪಕ್ಷ H1_TUNNEL_FAILED:
    ಯದಿ(new_state == H1_TUNNEL_FAILED)
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
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_free(ಸಂರಚನಾ h1_tunnel_state *ts,
                        ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_tunnel_free(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(cf) {
    ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
    ಸಂರಚನಾ h1_tunnel_state *ts = pctx ? pctx->ts : ರಿಕ್ತ;
    ಯದಿ(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = ರಿಕ್ತ;
    }
  }
}

ಸ್ಥಿರ bool tunnel_want_send(ಸಂರಚನಾ h1_tunnel_state *ts)
{
  ಪ್ರತ್ಯಾಗಮ ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ಸ್ಥಿರ CURLcode start_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ h1_tunnel_state *ts)
{
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ httpreq *req = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  ಯದಿ(result)
    ಗಚ್ಛ out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  ಯದಿ(!result)
    result = Curl_creader_set_null(data);

out:
  ಯದಿ(result)
    failf(data, "Failed sending CONNECT to proxy");
  ಯದಿ(req)
    Curl_http_req_free(req);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode send_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ h1_tunnel_state *ts,
                             bool *done)
{
  ಧ್ರುವ uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ಯದಿ(blen <= ts->nsent)
    ಗಚ್ಛ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN)
      result = CURLE_OK;
    ಗಚ್ಛ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ಧ್ರುವ ವರ್ಣ *)buf, nwritten);

out:
  ಯದಿ(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode on_resp_status(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h1_tunnel_state *ts,
                               ಧ್ರುವ ವರ್ಣ *header)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;

  (ಶೂನ್ಯ)ts;
  ಯದಿ(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  failf(data, "Invalid response header");
  ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
}

ಸ್ಥಿರ CURLcode on_resp_header(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h1_tunnel_state *ts,
                               ಧ್ರುವ ವರ್ಣ *header)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  ಯದಿ(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    ವರ್ಣ *auth = Curl_copy_header_ಮೌಲ್ಯ(header);
    ಯದಿ(!auth)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_ಒಳಹರಿವು_auth(data, proxy, auth);

    curlx_free(auth);

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("Content-Length:", header)) {
    ಯದಿ(k->httpcode < 300) {
      ಯದಿ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    ಅನ್ಯಥಾ {
      ಧ್ರುವ ವರ್ಣ *p = header + CURL_CSTRLEN("Content-Length:");
      ಯದಿ(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("Transfer-Encoding:", header)) {
    ಯದಿ(k->httpcode < 300) {
      ಯದಿ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    ಅನ್ಯಥಾ ಯದಿ(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(is_udp && checkprefix("Capsule-protocol:", header)) {
    ಯದಿ(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode single_header(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t ಸಾಲು_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ಧ್ರುವ ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಪೂರ್ಣಾಙ್ಕ writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, ಸಾಲು_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, ಸಾಲು_len);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, ಸಾಲು_len);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_bump_headersize(data, ಸಾಲು_len, TRUE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  ಯದಿ(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    ಯದಿ(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    ಅನ್ಯಥಾ ಯದಿ((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      ಯದಿ(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      ಅನ್ಯಥಾ ಯದಿ(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      ಅನ್ಯಥಾ {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    ಅನ್ಯಥಾ {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  ಅನ್ಯಥಾ
    result = on_resp_header(cf, data, ts, linep);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  curlx_dyn_reset(&ts->rcvbuf);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode recv_CONNECT_resp(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  ಯಾವತ್(ts->keepon) {
    size_t nread;
    ವರ್ಣ byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    ಯದಿ(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    ಯದಿ(!result)
      result = Curl_pgrsUpdate(data);

    ಯದಿ(result) {
      ts->keepon = KEEPON_DONE;
      ಭಙ್ಗ;
    }

    ಯದಿ(!nread) {
      ಯದಿ(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        ts->maybe_folded = FALSE;
      }
      ಯದಿ(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      ಅನ್ಯಥಾ {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      ಭಙ್ಗ;
    }

    ಯದಿ(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      ಯದಿ(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        ಯದಿ(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        ಯದಿ(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          ಭಙ್ಗ;
        }
      }
      ಅನುವರ್ತ;
    }

    ಯದಿ(ts->maybe_folded) {
      ಯದಿ(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      ಅನ್ಯಥಾ {
        result = single_header(cf, data, ts);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    ಯದಿ(ts->leading_unfold) {
      ಯದಿ(ISBLANK(byte))
        /* skip a bit brother */
        ಅನುವರ್ತ;
      /* non-blank, insert a space then continue the unfolding */
      ಯದಿ(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    ಯದಿ(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    ಯದಿ(byte != 0x0a)
      ಅನುವರ್ತ;
    ಅನ್ಯಥಾ {
      ಧ್ರುವ ವರ್ಣ *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      ಯದಿ(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ
        ts->maybe_folded = TRUE;
    }

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  } /* while there is buffer left and loop is requested */

  ಯದಿ(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  ಯದಿ(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode H1_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ h1_tunnel_state *ts)
{
  ಸಂರಚನಾ connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  ಯದಿ(tunnel_is_established(ts))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(tunnel_is_failed(ts))
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  ಕುರು {

    ಯದಿ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      ಗಚ್ಛ out;
    }

    ವಿಕಲ್ಪ(ts->tunnel_state) {
    ಪಕ್ಷ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    ಪಕ್ಷ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      ಯದಿ(result || !done)
        ಗಚ್ಛ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    ಪಕ್ಷ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      ಯದಿ(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
      ಯದಿ(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      ಯದಿ(result || !done)
        ಗಚ್ಛ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    ಪಕ್ಷ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      ಯದಿ(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        ಯದಿ(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
        }
        ಅನ್ಯಥಾ {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  } ಯಾವತ್(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  ಯದಿ(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    ಯದಿ(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
    }
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  ಯದಿ(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ h1_tunnel_state *ts = pctx->ts;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  ಯದಿ(result || !*done)
    ಪ್ರತ್ಯಾಗಮ result;

  *done = FALSE;
  ಯದಿ(!ts) {
    result = tunnel_init(cf, data, &ts);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  ಯದಿ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  ಯದಿ(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ಯದಿ(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      ಯದಿ(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      ಅನ್ಯಥಾ
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    ಅನ್ಯಥಾ
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_h1_proxy_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h1_proxy_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ಯದಿ(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ h1_tunnel_state *ts = pctx ? pctx->ts : ರಿಕ್ತ;
  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_HOST_PORT:
    ಯದಿ(!ts || !ts->dest)
      ಭಙ್ಗ;
    *pres1 = (ಪೂರ್ಣಾಙ್ಕ)ts->dest->port;
    *((ಧ್ರುವ ವರ್ಣ **)pres2) = ts->dest->hostname;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_peer *peer,
                                       ಸಂರಚನಾ Curl_peer *dest,
                                       ಪೂರ್ಣಾಙ್ಕ httpversion,
                                       bool udp_tunnel)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ cf_h1_proxy_ctx *pctx;
  ಸಂರಚನಾ h1_tunnel_state *ts;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  ಯದಿ(!dest)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ((httpversion < 10) || (httpversion >= 20))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ts = curlx_calloc(1, ಪರಿಮಾಣ(*ts));
  ಯದಿ(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, ಪರಿಮಾಣ(*pctx));
  ಯದಿ(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  ಯದಿ(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    ಗಚ್ಛ out;
  }
  ts = ರಿಕ್ತ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
