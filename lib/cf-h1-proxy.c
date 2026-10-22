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
  بنية dynbuf request_داده;
  size_t nsent;
  size_t headerlines;
  بنية Curl_chunker ch;
  صحيح httpversion;
  تعداد keeponval {
    KEEPON_انجامشده,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  بیت(chunked_encoding);
  بیت(close_connection);
  بیت(maybe_folded);
  بیت(leading_unfold);
  بیت(udp_tunnel);
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
                              بنية Curl_easy *داده,
                              بنية h1_tunnel_state *ts)
{
  (فراغ)داده;
  (فراغ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_داده);
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
                            بنية Curl_easy *داده,
                            بنية h1_tunnel_state **pts)
{
  بنية h1_tunnel_state *ts;

  إذا(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(داده, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts)
    أرجع CURLE_بیرون_از_حافظه;

  infof(داده, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرآیندها);
  curlx_dyn_init(&ts->request_داده, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(داده, &ts->ch, TRUE, TRUE);

  *pts = ts;
  أرجع tunnel_reinit(cf, داده, ts);
}

ساكن فراغ h1_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               بنية Curl_easy *داده)
{
  إذا(ts->tunnel_state == new_state)
    أرجع;
  /* entering this one */
  بدل(new_state) {
  حالة H1_TUNNEL_INIT:
    CURL_TRC_CF(داده, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, داده, ts);
    اكسر;

  حالة H1_TUNNEL_CONNECT:
    CURL_TRC_CF(داده, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    اكسر;

  حالة H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(داده, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    اكسر;

  حالة H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(داده, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    اكسر;

  حالة H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(داده, cf, "new tunnel state 'established'");
    infof(داده, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    داده->state.authproxy.انجامشده = TRUE;
    داده->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H1_TUNNEL_FAILED:
    إذا(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(داده, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_داده);
    /* restore the protocol pointer */
    داده->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(داده->req.hd_proxy_auth);
    اكسر;
  }
}

ساكن فراغ tunnel_free(بنية h1_tunnel_state *ts,
                        بنية Curl_easy *داده)
{
  إذا(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_داده);
    Curl_httpchunk_free(داده, &ts->ch);
    curlx_free(ts);
  }
}

ساكن فراغ cf_tunnel_free(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده)
{
  إذا(cf) {
    بنية cf_h1_proxy_ctx *pctx = cf->ctx;
    بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
    إذا(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, داده);
      tunnel_free(ts, داده);
      pctx->ts = عدم;
    }
  }
}

ساكن bool tunnel_want_send(بنية h1_tunnel_state *ts)
{
  أرجع ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ساكن CURLcode شروع_CONNECT(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              بنية h1_tunnel_state *ts)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية httpreq *req = عدم;
  صحيح http_minor;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(داده->req.newurl);

  نتیجه = Curl_http_proxy_create_tunnel_request(&req, cf, داده,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  إذا(نتیجه)
    اذهب بیرون;

  curlx_dyn_reset(&ts->request_داده);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  نتیجه = Curl_h1_req_write_سر(req, http_minor, &ts->request_داده);
  إذا(!نتیجه)
    نتیجه = Curl_creader_مجموعه_null(داده);

بیرون:
  إذا(نتیجه)
    failf(داده, "Failed sending CONNECT to proxy");
  إذا(req)
    Curl_http_req_free(req);
  أرجع نتیجه;
}

ساكن CURLcode send_CONNECT(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             بنية h1_tunnel_state *ts,
                             bool *انجامشده)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_داده);
  size_t request_len = curlx_dyn_len(&ts->request_داده);
  size_t blen = request_len;
  CURLcode نتیجه = CURLE_OK;
  size_t nwritten;

  إذا(blen <= ts->nsent)
    اذهب بیرون;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  نتیجه = cf->بعدی->cft->do_send(cf->بعدی, داده, buf, blen, FALSE, &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN)
      نتیجه = CURLE_OK;
    اذهب بیرون;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(داده, CURLINFO_سرآیند_بیرون, (ثابت حرف *)buf, nwritten);

بیرون:
  إذا(نتیجه)
    failf(داده, "Failed sending CONNECT to proxy");
  *انجامشده = (!نتیجه && (ts->nsent >= request_len));
  أرجع نتیجه;
}

ساكن CURLcode on_resp_وضعیت(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرآیند)
{
  بنية SingleRequest *k = &داده->req;

  (فراغ)ts;
  إذا(!مقایسه_رشته(سرآیند, "HTTP/1.", 7) &&
          ((سرآیند[7] == '0') || (سرآیند[7] == '1')) &&
          (سرآیند[8] == ' ') &&
          ISDIGIT(سرآیند[9]) && ISDIGIT(سرآیند[10]) && ISDIGIT(سرآیند[11]) &&
          !ISDIGIT(سرآیند[12])) {
    /* store the HTTP code from the proxy */
    داده->info.httpproxycode = k->httpcode = ((سرآیند[9] - '0') * 100) +
      ((سرآیند[10] - '0') * 10) + (سرآیند[11] - '0');
    CURL_TRC_CF(داده, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    أرجع CURLE_OK;
  }
  failf(داده, "Invalid response header");
  أرجع CURLE_WEIRD_SERVER_پاسخ;
}

ساكن CURLcode on_resp_سرآیند(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرآیند)
{
  CURLcode نتیجه = CURLE_OK;
  بنية SingleRequest *k = &داده->req;
  bool is_udp = h1_proxy_is_udp(cf);

  إذا(checkprefix("Proxy-authenticate:", سرآیند) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    حرف *auth = Curl_copy_سرآیند_مقدار(سرآیند);
    إذا(!auth)
      أرجع CURLE_بیرون_از_حافظه;

    CURL_TRC_CF(داده, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", سرآیند);
    نتیجه = Curl_http_درون_داد_auth(داده, proxy, auth);

    curlx_free(auth);

    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا إذا(checkprefix("Content-Length:", سرآیند)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(داده, "Invalid Content-Length: in %03d response", k->httpcode);
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(داده, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا {
      ثابت حرف *p = سرآیند + CURL_CSTRLEN("Content-Length:");
      إذا(curlx_str_numblanks(&p, &ts->cl)) {
        failf(داده, "Unsupported Content-Length value");
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
    }
  }
  وإلا إذا(Curl_compareheader(سرآیند,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(داده, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  وإلا إذا(checkprefix("Transfer-Encoding:", سرآیند)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(داده, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(داده, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا إذا(Curl_compareheader(سرآیند,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(داده, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(داده, &ts->ch, TRUE);
    }
  }
  وإلا إذا(is_udp && checkprefix("Capsule-protocol:", سرآیند)) {
    إذا(Curl_compareheader(سرآیند,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(داده, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  وإلا إذا(Curl_compareheader(سرآیند,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(داده, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  أرجع نتیجه;
}

ساكن CURLcode single_سرآیند(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              بنية h1_tunnel_state *ts)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t سطر_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت بنية SingleRequest *k = &داده->req;
  صحيح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(داده, CURLINFO_سرآیند_درون, linep, سطر_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  نتیجه = Curl_verify_سرآیند(داده, linep, سطر_len);
  إذا(نتیجه)
    أرجع نتیجه;

  /* send the header to the callback */
  writetype = CLIENTWRITE_سرآیند | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_وضعیت : 0);
  نتیجه = Curl_client_write(داده, writetype, linep, سطر_len);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_bump_headersize(داده, سطر_len, TRUE);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  إذا(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    إذا(ts->headerlines == 1) {
      ts->keepon = KEEPON_انجامشده;
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
    }
    وإلا إذا((407 == k->httpcode) && !داده->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      إذا(ts->cl) {
        infof(داده, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      وإلا إذا(ts->chunked_encoding) {
        infof(داده, "Ignore chunked response-body");
      }
      وإلا {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(داده, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_انجامشده;
      }
    }
    وإلا {
      ts->keepon = KEEPON_انجامشده;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_انجامشده);
    أرجع نتیجه;
  }

  إذا(ts->headerlines == 1)
    نتیجه = on_resp_وضعیت(cf, داده, ts, linep);
  وإلا
    نتیجه = on_resp_سرآیند(cf, داده, ts, linep);
  إذا(نتیجه)
    أرجع نتیجه;

  curlx_dyn_reset(&ts->rcvbuf);
  أرجع نتیجه;
}

ساكن CURLcode recv_CONNECT_resp(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية h1_tunnel_state *ts,
                                  bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح error;

#تعریف_کن SELECT_OK      0
#تعریف_کن SELECT_ERROR   1

  error = SELECT_OK;
  *انجامشده = FALSE;

  طالما(ts->keepon) {
    size_t nread;
    حرف byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    نتیجه = Curl_conn_recv(داده, cf->sockindex, &byte, 1, &nread);
    إذا(نتیجه == CURLE_AGAIN)
      /* socket buffer drained, return */
      أرجع CURLE_OK;

    إذا(!نتیجه)
      نتیجه = Curl_pgrsUpdate(داده);

    إذا(نتیجه) {
      ts->keepon = KEEPON_انجامشده;
      اكسر;
    }

    إذا(!nread) {
      إذا(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        نتیجه = single_سرآیند(cf, داده, ts);
        إذا(نتیجه)
          أرجع نتیجه;
        ts->maybe_folded = FALSE;
      }
      إذا(داده->مجموعه.proxyauth && داده->state.authproxy.avail &&
         داده->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(داده, "Proxy CONNECT connection closed");
      }
      وإلا {
        error = SELECT_ERROR;
        failf(داده, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_انجامشده;
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
        نتیجه = Curl_httpchunk_read(داده, &ts->ch, &byte, 1, &consumed);
        إذا(نتیجه)
          أرجع نتیجه;
        إذا(Curl_httpchunk_is_انجامشده(داده, &ts->ch)) {
          /* we are done reading chunks! */
          infof(داده, "chunk reading DONE");
          ts->keepon = KEEPON_انجامشده;
        }
      }
      وإلا إذا(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        إذا(ts->cl <= 0) {
          ts->keepon = KEEPON_انجامشده;
          اكسر;
        }
      }
      استمر;
    }

    إذا(ts->maybe_folded) {
      إذا(ISBLANK(byte)) {
        Curl_http_مقصد_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      وإلا {
        نتیجه = single_سرآیند(cf, داده, ts);
        إذا(نتیجه)
          أرجع نتیجه;
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
        failf(داده, "CONNECT response too large");
        أرجع CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    إذا(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(داده, "CONNECT response too large");
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
        نتیجه = single_سرآیند(cf, داده, ts);
        إذا(نتیجه)
          أرجع نتیجه;
      }
      وإلا
        ts->maybe_folded = TRUE;
    }

    إذا(نتیجه)
      أرجع نتیجه;
  } /* while there is buffer left and loop is requested */

  إذا(error)
    نتیجه = CURLE_RECV_ERROR;
  *انجامشده = (ts->keepon == KEEPON_انجامشده);
  إذا(!نتیجه && *انجامشده &&
     داده->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && داده->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    نتیجه = Curl_http_auth_act(داده);
  }
  أرجع نتیجه;
}

ساكن CURLcode H1_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية h1_tunnel_state *ts)
{
  بنية connectdata *conn = cf->conn;
  CURLcode نتیجه;
  bool انجامشده;

  إذا(tunnel_is_established(ts))
    أرجع CURLE_OK;
  إذا(tunnel_is_failed(ts))
    أرجع CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  افعل {

    إذا(Curl_timeleft_ms(داده) < 0) {
      failf(داده, "Proxy CONNECT aborted due to timeout");
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اذهب بیرون;
    }

    بدل(ts->tunnel_state) {
    حالة H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(داده, cf, "CONNECT start");
      نتیجه = شروع_CONNECT(cf, داده, ts);
      إذا(نتیجه)
        اذهب بیرون;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, داده);
      FALLTHROUGH();

    حالة H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(داده, cf, "CONNECT send");
      نتیجه = send_CONNECT(cf, داده, ts, &انجامشده);
      إذا(نتیجه || !انجامشده)
        اذهب بیرون;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, داده);
      FALLTHROUGH();

    حالة H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(داده, cf, "CONNECT receive");
      نتیجه = recv_CONNECT_resp(cf, داده, ts, &انجامشده);
      إذا(نتیجه)
        CURL_TRC_CF(داده, cf, "error receiving CONNECT response: %d",
                    (صحيح)نتیجه);
      إذا(!نتیجه)
        نتیجه = Curl_pgrsUpdate(داده);
      /* error or not complete yet. return for more multi-multi */
      إذا(نتیجه || !انجامشده)
        اذهب بیرون;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, داده);
      FALLTHROUGH();

    حالة H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(داده, cf, "CONNECT response");
      إذا(داده->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&داده->req, داده);
        إذا(ts->close_connection || conn->بیتها.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(داده, cf, "CONNECT need to close+open");
          infof(داده, "Connect me again please");
          أرجع CURLE_AGAIN;
        }
        وإلا {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, داده);
        }
      }
      اكسر;

    افتراضي:
      اكسر;
    }

  } طالما(داده->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  إذا(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    إذا(داده->info.httpproxycode / 100 != 2 &&
       داده->info.httpproxycode != 101) {
      curlx_safefree(داده->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, داده);
      failf(داده, "CONNECT-UDP tunnel failed, response %d",
            داده->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    إذا(داده->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(داده->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, داده);
      failf(داده, "CONNECT tunnel failed, response %d", داده->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, داده);
  infof(داده, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", داده->info.httpproxycode);
  نتیجه = CURLE_OK;

بیرون:
  إذا(نتیجه)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, داده);
  أرجع نتیجه;
}

ساكن CURLcode cf_h1_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  CURLcode نتیجه;
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(داده, cf, "connect");
  نتیجه = cf->بعدی->cft->do_connect(cf->بعدی, داده, انجامشده);
  إذا(نتیجه || !*انجامشده)
    أرجع نتیجه;

  *انجامشده = FALSE;
  إذا(!ts) {
    نتیجه = tunnel_init(cf, داده, &ts);
    إذا(نتیجه)
      أرجع نتیجه;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  نتیجه = H1_CONNECT(cf, داده, ts);
  إذا(نتیجه)
    اذهب بیرون;
  curlx_safefree(داده->req.hd_proxy_auth);

بیرون:
  *انجامشده = (نتیجه == CURLE_OK) && tunnel_is_established(pctx->ts);
  إذا(*انجامشده) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&داده->req, داده);
    Curl_client_reset(داده);
    Curl_pgrsReset(داده);
    cf_tunnel_free(cf, داده);
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_h1_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية easy_pollset *ps)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;
  CURLcode نتیجه = CURLE_OK;

  إذا(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, داده);
    إذا(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      إذا(tunnel_want_send(ts))
        نتیجه = Curl_pollset_مجموعه_بیرون_only(داده, ps, sock);
      وإلا
        نتیجه = Curl_pollset_مجموعه_درون_only(داده, ps, sock);
    }
    وإلا
      نتیجه = Curl_pollset_مجموعه_بیرون_only(داده, ps, sock);
  }
  وإلا {
    إذا(cf->بعدی)
      نتیجه = cf->بعدی->cft->adjust_pollset(cf->بعدی, داده, ps);
  }
  أرجع نتیجه;
}

ساكن bool cf_h1_proxy_داده_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *داده)
{
  أرجع cf->بعدی ? cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

ساكن فراغ cf_h1_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  إذا(pctx) {
    cf_tunnel_free(cf, داده);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ساكن CURLcode cf_h1_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
  بدل(query) {
  حالة CF_QUERY_HOST_پورت:
    إذا(!ts || !ts->dest)
      اكسر;
    *pres1 = (صحيح)ts->dest->پورت;
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
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
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
  cf_h1_proxy_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_درج_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *داده,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       صحيح httpversion,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf;
  بنية cf_h1_proxy_ctx *pctx;
  بنية h1_tunnel_state *ts;
  CURLcode نتیجه;

  (فراغ)داده;
  إذا(!dest)
    أرجع CURLE_FAILED_INIT;
  إذا((httpversion < 10) || (httpversion >= 20))
    أرجع CURLE_FAILED_INIT;

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرآیندها);
  curlx_dyn_init(&ts->request_داده, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(داده, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, حجم(*pctx));
  إذا(!pctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  نتیجه = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  إذا(نتیجه) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    اذهب بیرون;
  }
  ts = عدم;
  Curl_conn_cf_درج_after(cf_at, cf);

بیرون:
  tunnel_free(ts, داده);
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
