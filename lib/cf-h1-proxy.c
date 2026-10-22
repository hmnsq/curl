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

قسمتعریف شمار {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ساخت h1_tunnel_state {
  ساخت Curl_peer *dest;
  ساخت dynbuf rcvbuf;
  ساخت dynbuf request_ڈیٹا;
  size_t nsent;
  size_t headerlines;
  ساخت Curl_chunker ch;
  صحیح httpversion;
  شمار keeponval {
    KEEPON_مکمل,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  ثنائی_ہندسہ(chunked_encoding);
  ثنائی_ہندسہ(close_connection);
  ثنائی_ہندسہ(maybe_folded);
  ثنائی_ہندسہ(leading_unfold);
  ثنائی_ہندسہ(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
ساخت cf_h1_proxy_ctx {
  ساخت Curl_peer *peer;
  ساخت h1_tunnel_state *ts;
};

ساکن bool tunnel_is_established(ساخت h1_tunnel_state *ts)
{
  واپس ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ساکن bool tunnel_is_failed(ساخت h1_tunnel_state *ts)
{
  واپس ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ساکن bool h1_proxy_is_udp(ساخت Curl_cfilter *cf)
{
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
  واپس (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ساکن CURLcode tunnel_reinit(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت h1_tunnel_state *ts)
{
  (خالی)ڈیٹا;
  (خالی)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_ڈیٹا);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  واپس CURLE_OK;
}

ساکن CURLcode tunnel_init(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت h1_tunnel_state **pts)
{
  ساخت h1_tunnel_state *ts;

  اگر(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(ڈیٹا, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, حجم(*ts));
  اگر(!ts)
    واپس CURLE_برآمد_کا_حافظہ;

  infof(ڈیٹا, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرخیاں);
  curlx_dyn_init(&ts->request_ڈیٹا, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ڈیٹا, &ts->ch, TRUE, TRUE);

  *pts = ts;
  واپس tunnel_reinit(cf, ڈیٹا, ts);
}

ساکن خالی h1_tunnel_go_state(ساخت Curl_cfilter *cf,
                               ساخت h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ساخت Curl_easy *ڈیٹا)
{
  اگر(ts->tunnel_state == new_state)
    واپس;
  /* entering this one */
  انتخاب(new_state) {
  حالت H1_TUNNEL_INIT:
    CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, ڈیٹا, ts);
    توڑو;

  حالت H1_TUNNEL_CONNECT:
    CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    توڑو;

  حالت H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    توڑو;

  حالت H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    توڑو;

  حالت H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'established'");
    infof(ڈیٹا, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    ڈیٹا->state.authproxy.مکمل = TRUE;
    ڈیٹا->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالت H1_TUNNEL_FAILED:
    اگر(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(ڈیٹا, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_ڈیٹا);
    /* restore the protocol pointer */
    ڈیٹا->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ڈیٹا->req.hd_proxy_auth);
    توڑو;
  }
}

ساکن خالی tunnel_free(ساخت h1_tunnel_state *ts,
                        ساخت Curl_easy *ڈیٹا)
{
  اگر(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_ڈیٹا);
    Curl_httpchunk_free(ڈیٹا, &ts->ch);
    curlx_free(ts);
  }
}

ساکن خالی cf_tunnel_free(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا)
{
  اگر(cf) {
    ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
    ساخت h1_tunnel_state *ts = pctx ? pctx->ts : تہی;
    اگر(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ڈیٹا);
      tunnel_free(ts, ڈیٹا);
      pctx->ts = تہی;
    }
  }
}

ساکن bool tunnel_want_send(ساخت h1_tunnel_state *ts)
{
  واپس ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ساکن CURLcode آغاز_CONNECT(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت h1_tunnel_state *ts)
{
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
  ساخت httpreq *req = تہی;
  صحیح http_minor;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(ڈیٹا->req.newurl);

  نتیجہ = Curl_http_proxy_create_tunnel_request(&req, cf, ڈیٹا,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  اگر(نتیجہ)
    جاؤ برآمد;

  curlx_dyn_reset(&ts->request_ڈیٹا);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  نتیجہ = Curl_h1_req_write_سر(req, http_minor, &ts->request_ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);

برآمد:
  اگر(نتیجہ)
    failf(ڈیٹا, "Failed sending CONNECT to proxy");
  اگر(req)
    Curl_http_req_free(req);
  واپس نتیجہ;
}

ساکن CURLcode send_CONNECT(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت h1_tunnel_state *ts,
                             bool *مکمل)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_ڈیٹا);
  size_t request_len = curlx_dyn_len(&ts->request_ڈیٹا);
  size_t blen = request_len;
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  اگر(blen <= ts->nsent)
    جاؤ برآمد;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  نتیجہ = cf->اگلا->cft->do_send(cf->اگلا, ڈیٹا, buf, blen, FALSE, &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    جاؤ برآمد;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, (ثابت حرف *)buf, nwritten);

برآمد:
  اگر(نتیجہ)
    failf(ڈیٹا, "Failed sending CONNECT to proxy");
  *مکمل = (!نتیجہ && (ts->nsent >= request_len));
  واپس نتیجہ;
}

ساکن CURLcode on_resp_حالت(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h1_tunnel_state *ts,
                               ثابت حرف *سرخی)
{
  ساخت SingleRequest *k = &ڈیٹا->req;

  (خالی)ts;
  اگر(!لڑی_موازنہ(سرخی, "HTTP/1.", 7) &&
          ((سرخی[7] == '0') || (سرخی[7] == '1')) &&
          (سرخی[8] == ' ') &&
          ISDIGIT(سرخی[9]) && ISDIGIT(سرخی[10]) && ISDIGIT(سرخی[11]) &&
          !ISDIGIT(سرخی[12])) {
    /* store the HTTP code from the proxy */
    ڈیٹا->info.httpproxycode = k->httpcode = ((سرخی[9] - '0') * 100) +
      ((سرخی[10] - '0') * 10) + (سرخی[11] - '0');
    CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    واپس CURLE_OK;
  }
  failf(ڈیٹا, "Invalid response header");
  واپس CURLE_WEIRD_SERVER_جواب;
}

ساکن CURLcode on_resp_سرخی(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h1_tunnel_state *ts,
                               ثابت حرف *سرخی)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت SingleRequest *k = &ڈیٹا->req;
  bool is_udp = h1_proxy_is_udp(cf);

  اگر(checkprefix("Proxy-authenticate:", سرخی) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    حرف *auth = Curl_copy_سرخی_قدر(سرخی);
    اگر(!auth)
      واپس CURLE_برآمد_کا_حافظہ;

    CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", سرخی);
    نتیجہ = Curl_http_داخلہ_auth(ڈیٹا, proxy, auth);

    curlx_free(auth);

    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ اگر(checkprefix("Content-Length:", سرخی)) {
    اگر(k->httpcode < 300) {
      اگر(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ڈیٹا, "Invalid Content-Length: in %03d response", k->httpcode);
        واپس CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ڈیٹا, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    ورنہ {
      ثابت حرف *p = سرخی + CURL_CSTRLEN("Content-Length:");
      اگر(curlx_str_numblanks(&p, &ts->cl)) {
        failf(ڈیٹا, "Unsupported Content-Length value");
        واپس CURLE_WEIRD_SERVER_جواب;
      }
    }
  }
  ورنہ اگر(Curl_compareheader(سرخی,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ورنہ اگر(checkprefix("Transfer-Encoding:", سرخی)) {
    اگر(k->httpcode < 300) {
      اگر(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ڈیٹا, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        واپس CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ڈیٹا, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    ورنہ اگر(Curl_compareheader(سرخی,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(ڈیٹا, &ts->ch, TRUE);
    }
  }
  ورنہ اگر(is_udp && checkprefix("Capsule-protocol:", سرخی)) {
    اگر(Curl_compareheader(سرخی,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  ورنہ اگر(Curl_compareheader(سرخی,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  واپس نتیجہ;
}

ساکن CURLcode single_سرخی(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت h1_tunnel_state *ts)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t سطر_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت ساخت SingleRequest *k = &ڈیٹا->req;
  صحیح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, linep, سطر_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  نتیجہ = Curl_verify_سرخی(ڈیٹا, linep, سطر_len);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* send the header to the callback */
  writetype = CLIENTWRITE_سرخی | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_حالت : 0);
  نتیجہ = Curl_client_write(ڈیٹا, writetype, linep, سطر_len);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_bump_headersize(ڈیٹا, سطر_len, TRUE);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  اگر(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    اگر(ts->headerlines == 1) {
      ts->keepon = KEEPON_مکمل;
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
    }
    ورنہ اگر((407 == k->httpcode) && !ڈیٹا->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      اگر(ts->cl) {
        infof(ڈیٹا, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      ورنہ اگر(ts->chunked_encoding) {
        infof(ڈیٹا, "Ignore chunked response-body");
      }
      ورنہ {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(ڈیٹا, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_مکمل;
      }
    }
    ورنہ {
      ts->keepon = KEEPON_مکمل;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_مکمل);
    واپس نتیجہ;
  }

  اگر(ts->headerlines == 1)
    نتیجہ = on_resp_حالت(cf, ڈیٹا, ts, linep);
  ورنہ
    نتیجہ = on_resp_سرخی(cf, ڈیٹا, ts, linep);
  اگر(نتیجہ)
    واپس نتیجہ;

  curlx_dyn_reset(&ts->rcvbuf);
  واپس نتیجہ;
}

ساکن CURLcode recv_CONNECT_resp(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت h1_tunnel_state *ts,
                                  bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح error;

#تعریف_کرو SELECT_OK      0
#تعریف_کرو SELECT_ERROR   1

  error = SELECT_OK;
  *مکمل = FALSE;

  جب(ts->keepon) {
    size_t nread;
    حرف byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    نتیجہ = Curl_conn_recv(ڈیٹا, cf->sockindex, &byte, 1, &nread);
    اگر(نتیجہ == CURLE_AGAIN)
      /* socket buffer drained, return */
      واپس CURLE_OK;

    اگر(!نتیجہ)
      نتیجہ = Curl_pgrsUpdate(ڈیٹا);

    اگر(نتیجہ) {
      ts->keepon = KEEPON_مکمل;
      توڑو;
    }

    اگر(!nread) {
      اگر(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        نتیجہ = single_سرخی(cf, ڈیٹا, ts);
        اگر(نتیجہ)
          واپس نتیجہ;
        ts->maybe_folded = FALSE;
      }
      اگر(ڈیٹا->سیٹ.proxyauth && ڈیٹا->state.authproxy.avail &&
         ڈیٹا->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(ڈیٹا, "Proxy CONNECT connection closed");
      }
      ورنہ {
        error = SELECT_ERROR;
        failf(ڈیٹا, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_مکمل;
      توڑو;
    }

    اگر(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      اگر(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        نتیجہ = Curl_httpchunk_read(ڈیٹا, &ts->ch, &byte, 1, &consumed);
        اگر(نتیجہ)
          واپس نتیجہ;
        اگر(Curl_httpchunk_is_مکمل(ڈیٹا, &ts->ch)) {
          /* we are done reading chunks! */
          infof(ڈیٹا, "chunk reading DONE");
          ts->keepon = KEEPON_مکمل;
        }
      }
      ورنہ اگر(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        اگر(ts->cl <= 0) {
          ts->keepon = KEEPON_مکمل;
          توڑو;
        }
      }
      جاری;
    }

    اگر(ts->maybe_folded) {
      اگر(ISBLANK(byte)) {
        Curl_http_مقصد_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      ورنہ {
        نتیجہ = single_سرخی(cf, ڈیٹا, ts);
        اگر(نتیجہ)
          واپس نتیجہ;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    اگر(ts->leading_unfold) {
      اگر(ISBLANK(byte))
        /* skip a bit brother */
        جاری;
      /* non-blank, insert a space then continue the unfolding */
      اگر(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(ڈیٹا, "CONNECT response too large");
        واپس CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    اگر(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(ڈیٹا, "CONNECT response too large");
      واپس CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    اگر(byte != 0x0a)
      جاری;
    ورنہ {
      ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      اگر(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        نتیجہ = single_سرخی(cf, ڈیٹا, ts);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
      ورنہ
        ts->maybe_folded = TRUE;
    }

    اگر(نتیجہ)
      واپس نتیجہ;
  } /* while there is buffer left and loop is requested */

  اگر(error)
    نتیجہ = CURLE_RECV_ERROR;
  *مکمل = (ts->keepon == KEEPON_مکمل);
  اگر(!نتیجہ && *مکمل &&
     ڈیٹا->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && ڈیٹا->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    نتیجہ = Curl_http_auth_act(ڈیٹا);
  }
  واپس نتیجہ;
}

ساکن CURLcode H1_CONNECT(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت h1_tunnel_state *ts)
{
  ساخت connectdata *conn = cf->conn;
  CURLcode نتیجہ;
  bool مکمل;

  اگر(tunnel_is_established(ts))
    واپس CURLE_OK;
  اگر(tunnel_is_failed(ts))
    واپس CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  کرو {

    اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
      failf(ڈیٹا, "Proxy CONNECT aborted due to timeout");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      جاؤ برآمد;
    }

    انتخاب(ts->tunnel_state) {
    حالت H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT start");
      نتیجہ = آغاز_CONNECT(cf, ڈیٹا, ts);
      اگر(نتیجہ)
        جاؤ برآمد;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, ڈیٹا);
      FALLTHROUGH();

    حالت H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT send");
      نتیجہ = send_CONNECT(cf, ڈیٹا, ts, &مکمل);
      اگر(نتیجہ || !مکمل)
        جاؤ برآمد;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, ڈیٹا);
      FALLTHROUGH();

    حالت H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT receive");
      نتیجہ = recv_CONNECT_resp(cf, ڈیٹا, ts, &مکمل);
      اگر(نتیجہ)
        CURL_TRC_CF(ڈیٹا, cf, "error receiving CONNECT response: %d",
                    (صحیح)نتیجہ);
      اگر(!نتیجہ)
        نتیجہ = Curl_pgrsUpdate(ڈیٹا);
      /* error or not complete yet. return for more multi-multi */
      اگر(نتیجہ || !مکمل)
        جاؤ برآمد;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, ڈیٹا);
      FALLTHROUGH();

    حالت H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(ڈیٹا, cf, "CONNECT response");
      اگر(ڈیٹا->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&ڈیٹا->req, ڈیٹا);
        اگر(ts->close_connection || conn->بٹ.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(ڈیٹا, cf, "CONNECT need to close+open");
          infof(ڈیٹا, "Connect me again please");
          واپس CURLE_AGAIN;
        }
        ورنہ {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, ڈیٹا);
        }
      }
      توڑو;

    طےشدہ:
      توڑو;
    }

  } جب(ڈیٹا->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  اگر(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    اگر(ڈیٹا->info.httpproxycode / 100 != 2 &&
       ڈیٹا->info.httpproxycode != 101) {
      curlx_safefree(ڈیٹا->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ڈیٹا);
      failf(ڈیٹا, "CONNECT-UDP tunnel failed, response %d",
            ڈیٹا->req.httpcode);
      واپس CURLE_COULDNT_CONNECT;
    }
  }
  ورنہ {
    اگر(ڈیٹا->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(ڈیٹا->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ڈیٹا);
      failf(ڈیٹا, "CONNECT tunnel failed, response %d", ڈیٹا->req.httpcode);
      واپس CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, ڈیٹا);
  infof(ڈیٹا, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", ڈیٹا->info.httpproxycode);
  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ڈیٹا);
  واپس نتیجہ;
}

ساکن CURLcode cf_h1_proxy_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  CURLcode نتیجہ;
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
  ساخت h1_tunnel_state *ts = pctx->ts;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  CURL_TRC_CF(ڈیٹا, cf, "connect");
  نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
  اگر(نتیجہ || !*مکمل)
    واپس نتیجہ;

  *مکمل = FALSE;
  اگر(!ts) {
    نتیجہ = tunnel_init(cf, ڈیٹا, &ts);
    اگر(نتیجہ)
      واپس نتیجہ;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  نتیجہ = H1_CONNECT(cf, ڈیٹا, ts);
  اگر(نتیجہ)
    جاؤ برآمد;
  curlx_safefree(ڈیٹا->req.hd_proxy_auth);

برآمد:
  *مکمل = (نتیجہ == CURLE_OK) && tunnel_is_established(pctx->ts);
  اگر(*مکمل) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ڈیٹا->req, ڈیٹا);
    Curl_client_reset(ڈیٹا);
    Curl_pgrsReset(ڈیٹا);
    cf_tunnel_free(cf, ڈیٹا);
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_h1_proxy_adjust_pollset(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت easy_pollset *ps)
{
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
  ساخت h1_tunnel_state *ts = pctx->ts;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ڈیٹا);
    اگر(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      اگر(tunnel_want_send(ts))
        نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, sock);
      ورنہ
        نتیجہ = Curl_pollset_سیٹ_اندر_only(ڈیٹا, ps, sock);
    }
    ورنہ
      نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, sock);
  }
  ورنہ {
    اگر(cf->اگلا)
      نتیجہ = cf->اگلا->cft->adjust_pollset(cf->اگلا, ڈیٹا, ps);
  }
  واپس نتیجہ;
}

ساکن bool cf_h1_proxy_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                     ثابت ساخت Curl_easy *ڈیٹا)
{
  واپس cf->اگلا ? cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساکن خالی cf_h1_proxy_destroy(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  اگر(pctx) {
    cf_tunnel_free(cf, ڈیٹا);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ساکن CURLcode cf_h1_proxy_query(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_h1_proxy_ctx *pctx = cf->ctx;
  ساخت h1_tunnel_state *ts = pctx ? pctx->ts : تہی;
  انتخاب(query) {
  حالت CF_QUERY_HOST_پورٹ:
    اگر(!ts || !ts->dest)
      توڑو;
    *pres1 = (صحیح)ts->dest->پورٹ;
    *((ثابت حرف **)pres2) = ts->dest->hostname;
    واپس CURLE_OK;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساخت Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *peer,
                                       ساخت Curl_peer *dest,
                                       صحیح httpversion,
                                       bool udp_tunnel)
{
  ساخت Curl_cfilter *cf;
  ساخت cf_h1_proxy_ctx *pctx;
  ساخت h1_tunnel_state *ts;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  اگر(!dest)
    واپس CURLE_FAILED_INIT;
  اگر((httpversion < 10) || (httpversion >= 20))
    واپس CURLE_FAILED_INIT;

  ts = curlx_calloc(1, حجم(*ts));
  اگر(!ts) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرخیاں);
  curlx_dyn_init(&ts->request_ڈیٹا, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ڈیٹا, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, حجم(*pctx));
  اگر(!pctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  اگر(نتیجہ) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    جاؤ برآمد;
  }
  ts = تہی;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

برآمد:
  tunnel_free(ts, ڈیٹا);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
