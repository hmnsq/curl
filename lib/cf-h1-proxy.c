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
  بنية dynbuf request_معلومات;
  size_t nsent;
  size_t headerlines;
  بنية Curl_chunker ch;
  صحيح httpversion;
  تعداد keeponval {
    KEEPON_پورو,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  ٻٽو_انگ(chunked_encoding);
  ٻٽو_انگ(close_connection);
  ٻٽو_انگ(maybe_folded);
  ٻٽو_انگ(leading_unfold);
  ٻٽو_انگ(udp_tunnel);
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
                              بنية Curl_easy *معلومات,
                              بنية h1_tunnel_state *ts)
{
  (فراغ)معلومات;
  (فراغ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_معلومات);
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
                            بنية Curl_easy *معلومات,
                            بنية h1_tunnel_state **pts)
{
  بنية h1_tunnel_state *ts;

  إذا(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(معلومات, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  infof(معلومات, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرا);
  curlx_dyn_init(&ts->request_معلومات, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(معلومات, &ts->ch, TRUE, TRUE);

  *pts = ts;
  أرجع tunnel_reinit(cf, معلومات, ts);
}

ساكن فراغ h1_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               بنية Curl_easy *معلومات)
{
  إذا(ts->tunnel_state == new_state)
    أرجع;
  /* entering this one */
  بدل(new_state) {
  حالة H1_TUNNEL_INIT:
    CURL_TRC_CF(معلومات, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, معلومات, ts);
    اكسر;

  حالة H1_TUNNEL_CONNECT:
    CURL_TRC_CF(معلومات, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    اكسر;

  حالة H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(معلومات, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    اكسر;

  حالة H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(معلومات, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    اكسر;

  حالة H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(معلومات, cf, "new tunnel state 'established'");
    infof(معلومات, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    معلومات->state.authproxy.پورو = TRUE;
    معلومات->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H1_TUNNEL_FAILED:
    إذا(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(معلومات, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_معلومات);
    /* restore the protocol pointer */
    معلومات->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(معلومات->req.hd_proxy_auth);
    اكسر;
  }
}

ساكن فراغ tunnel_free(بنية h1_tunnel_state *ts,
                        بنية Curl_easy *معلومات)
{
  إذا(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_معلومات);
    Curl_httpchunk_free(معلومات, &ts->ch);
    curlx_free(ts);
  }
}

ساكن فراغ cf_tunnel_free(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات)
{
  إذا(cf) {
    بنية cf_h1_proxy_ctx *pctx = cf->ctx;
    بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
    إذا(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, معلومات);
      tunnel_free(ts, معلومات);
      pctx->ts = عدم;
    }
  }
}

ساكن bool tunnel_want_send(بنية h1_tunnel_state *ts)
{
  أرجع ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ساكن CURLcode شروعات_CONNECT(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              بنية h1_tunnel_state *ts)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية httpreq *req = عدم;
  صحيح http_minor;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(معلومات->req.newurl);

  نتيجو = Curl_http_proxy_create_tunnel_request(&req, cf, معلومات,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  إذا(نتيجو)
    اذهب ٻاهريون;

  curlx_dyn_reset(&ts->request_معلومات);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  نتيجو = Curl_h1_req_write_سر(req, http_minor, &ts->request_معلومات);
  إذا(!نتيجو)
    نتيجو = Curl_creader_سيٽ_null(معلومات);

ٻاهريون:
  إذا(نتيجو)
    failf(معلومات, "Failed sending CONNECT to proxy");
  إذا(req)
    Curl_http_req_free(req);
  أرجع نتيجو;
}

ساكن CURLcode send_CONNECT(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             بنية h1_tunnel_state *ts,
                             bool *پورو)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_معلومات);
  size_t request_len = curlx_dyn_len(&ts->request_معلومات);
  size_t blen = request_len;
  CURLcode نتيجو = CURLE_OK;
  size_t nwritten;

  إذا(blen <= ts->nsent)
    اذهب ٻاهريون;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  نتيجو = cf->ايندڙ->cft->do_send(cf->ايندڙ, معلومات, buf, blen, FALSE, &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN)
      نتيجو = CURLE_OK;
    اذهب ٻاهريون;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(معلومات, CURLINFO_سرو_ٻاهريون, (ثابت حرف *)buf, nwritten);

ٻاهريون:
  إذا(نتيجو)
    failf(معلومات, "Failed sending CONNECT to proxy");
  *پورو = (!نتيجو && (ts->nsent >= request_len));
  أرجع نتيجو;
}

ساكن CURLcode on_resp_حالت(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرو)
{
  بنية SingleRequest *k = &معلومات->req;

  (فراغ)ts;
  إذا(!لڙي_ڀيٽ(سرو, "HTTP/1.", 7) &&
          ((سرو[7] == '0') || (سرو[7] == '1')) &&
          (سرو[8] == ' ') &&
          ISDIGIT(سرو[9]) && ISDIGIT(سرو[10]) && ISDIGIT(سرو[11]) &&
          !ISDIGIT(سرو[12])) {
    /* store the HTTP code from the proxy */
    معلومات->info.httpproxycode = k->httpcode = ((سرو[9] - '0') * 100) +
      ((سرو[10] - '0') * 10) + (سرو[11] - '0');
    CURL_TRC_CF(معلومات, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    أرجع CURLE_OK;
  }
  failf(معلومات, "Invalid response header");
  أرجع CURLE_WEIRD_SERVER_جواب;
}

ساكن CURLcode on_resp_سرو(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية SingleRequest *k = &معلومات->req;
  bool is_udp = h1_proxy_is_udp(cf);

  إذا(checkprefix("Proxy-authenticate:", سرو) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    حرف *auth = Curl_copy_سرو_قدر(سرو);
    إذا(!auth)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    CURL_TRC_CF(معلومات, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", سرو);
    نتيجو = Curl_http_داخلا_auth(معلومات, proxy, auth);

    curlx_free(auth);

    إذا(نتيجو)
      أرجع نتيجو;
  }
  وإلا إذا(checkprefix("Content-Length:", سرو)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(معلومات, "Invalid Content-Length: in %03d response", k->httpcode);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(معلومات, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا {
      ثابت حرف *p = سرو + CURL_CSTRLEN("Content-Length:");
      إذا(curlx_str_numblanks(&p, &ts->cl)) {
        failf(معلومات, "Unsupported Content-Length value");
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
    }
  }
  وإلا إذا(Curl_compareheader(سرو,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(معلومات, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  وإلا إذا(checkprefix("Transfer-Encoding:", سرو)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(معلومات, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(معلومات, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا إذا(Curl_compareheader(سرو,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(معلومات, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(معلومات, &ts->ch, TRUE);
    }
  }
  وإلا إذا(is_udp && checkprefix("Capsule-protocol:", سرو)) {
    إذا(Curl_compareheader(سرو,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(معلومات, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  وإلا إذا(Curl_compareheader(سرو,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(معلومات, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  أرجع نتيجو;
}

ساكن CURLcode single_سرو(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              بنية h1_tunnel_state *ts)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t سٽ_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت بنية SingleRequest *k = &معلومات->req;
  صحيح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(معلومات, CURLINFO_سرو_اندريون, linep, سٽ_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  نتيجو = Curl_verify_سرو(معلومات, linep, سٽ_len);
  إذا(نتيجو)
    أرجع نتيجو;

  /* send the header to the callback */
  writetype = CLIENTWRITE_سرو | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_حالت : 0);
  نتيجو = Curl_client_write(معلومات, writetype, linep, سٽ_len);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_bump_headersize(معلومات, سٽ_len, TRUE);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  إذا(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    إذا(ts->headerlines == 1) {
      ts->keepon = KEEPON_پورو;
      نتيجو = CURLE_WEIRD_SERVER_جواب;
    }
    وإلا إذا((407 == k->httpcode) && !معلومات->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      إذا(ts->cl) {
        infof(معلومات, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      وإلا إذا(ts->chunked_encoding) {
        infof(معلومات, "Ignore chunked response-body");
      }
      وإلا {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(معلومات, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_پورو;
      }
    }
    وإلا {
      ts->keepon = KEEPON_پورو;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_پورو);
    أرجع نتيجو;
  }

  إذا(ts->headerlines == 1)
    نتيجو = on_resp_حالت(cf, معلومات, ts, linep);
  وإلا
    نتيجو = on_resp_سرو(cf, معلومات, ts, linep);
  إذا(نتيجو)
    أرجع نتيجو;

  curlx_dyn_reset(&ts->rcvbuf);
  أرجع نتيجو;
}

ساكن CURLcode recv_CONNECT_resp(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية h1_tunnel_state *ts,
                                  bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  صحيح error;

#وصف_ڪريو SELECT_OK      0
#وصف_ڪريو SELECT_ERROR   1

  error = SELECT_OK;
  *پورو = FALSE;

  طالما(ts->keepon) {
    size_t nread;
    حرف byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    نتيجو = Curl_conn_recv(معلومات, cf->sockindex, &byte, 1, &nread);
    إذا(نتيجو == CURLE_AGAIN)
      /* socket buffer drained, return */
      أرجع CURLE_OK;

    إذا(!نتيجو)
      نتيجو = Curl_pgrsUpdate(معلومات);

    إذا(نتيجو) {
      ts->keepon = KEEPON_پورو;
      اكسر;
    }

    إذا(!nread) {
      إذا(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        نتيجو = single_سرو(cf, معلومات, ts);
        إذا(نتيجو)
          أرجع نتيجو;
        ts->maybe_folded = FALSE;
      }
      إذا(معلومات->سيٽ.proxyauth && معلومات->state.authproxy.avail &&
         معلومات->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(معلومات, "Proxy CONNECT connection closed");
      }
      وإلا {
        error = SELECT_ERROR;
        failf(معلومات, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_پورو;
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
        نتيجو = Curl_httpchunk_read(معلومات, &ts->ch, &byte, 1, &consumed);
        إذا(نتيجو)
          أرجع نتيجو;
        إذا(Curl_httpchunk_is_پورو(معلومات, &ts->ch)) {
          /* we are done reading chunks! */
          infof(معلومات, "chunk reading DONE");
          ts->keepon = KEEPON_پورو;
        }
      }
      وإلا إذا(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        إذا(ts->cl <= 0) {
          ts->keepon = KEEPON_پورو;
          اكسر;
        }
      }
      استمر;
    }

    إذا(ts->maybe_folded) {
      إذا(ISBLANK(byte)) {
        Curl_http_منزل_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      وإلا {
        نتيجو = single_سرو(cf, معلومات, ts);
        إذا(نتيجو)
          أرجع نتيجو;
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
        failf(معلومات, "CONNECT response too large");
        أرجع CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    إذا(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(معلومات, "CONNECT response too large");
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
        نتيجو = single_سرو(cf, معلومات, ts);
        إذا(نتيجو)
          أرجع نتيجو;
      }
      وإلا
        ts->maybe_folded = TRUE;
    }

    إذا(نتيجو)
      أرجع نتيجو;
  } /* while there is buffer left and loop is requested */

  إذا(error)
    نتيجو = CURLE_RECV_ERROR;
  *پورو = (ts->keepon == KEEPON_پورو);
  إذا(!نتيجو && *پورو &&
     معلومات->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && معلومات->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    نتيجو = Curl_http_auth_act(معلومات);
  }
  أرجع نتيجو;
}

ساكن CURLcode H1_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية h1_tunnel_state *ts)
{
  بنية connectdata *conn = cf->conn;
  CURLcode نتيجو;
  bool پورو;

  إذا(tunnel_is_established(ts))
    أرجع CURLE_OK;
  إذا(tunnel_is_failed(ts))
    أرجع CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  افعل {

    إذا(Curl_timeleft_ms(معلومات) < 0) {
      failf(معلومات, "Proxy CONNECT aborted due to timeout");
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اذهب ٻاهريون;
    }

    بدل(ts->tunnel_state) {
    حالة H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(معلومات, cf, "CONNECT start");
      نتيجو = شروعات_CONNECT(cf, معلومات, ts);
      إذا(نتيجو)
        اذهب ٻاهريون;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, معلومات);
      FALLTHROUGH();

    حالة H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(معلومات, cf, "CONNECT send");
      نتيجو = send_CONNECT(cf, معلومات, ts, &پورو);
      إذا(نتيجو || !پورو)
        اذهب ٻاهريون;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, معلومات);
      FALLTHROUGH();

    حالة H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(معلومات, cf, "CONNECT receive");
      نتيجو = recv_CONNECT_resp(cf, معلومات, ts, &پورو);
      إذا(نتيجو)
        CURL_TRC_CF(معلومات, cf, "error receiving CONNECT response: %d",
                    (صحيح)نتيجو);
      إذا(!نتيجو)
        نتيجو = Curl_pgrsUpdate(معلومات);
      /* error or not complete yet. return for more multi-multi */
      إذا(نتيجو || !پورو)
        اذهب ٻاهريون;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, معلومات);
      FALLTHROUGH();

    حالة H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(معلومات, cf, "CONNECT response");
      إذا(معلومات->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&معلومات->req, معلومات);
        إذا(ts->close_connection || conn->بٽ.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(معلومات, cf, "CONNECT need to close+open");
          infof(معلومات, "Connect me again please");
          أرجع CURLE_AGAIN;
        }
        وإلا {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, معلومات);
        }
      }
      اكسر;

    افتراضي:
      اكسر;
    }

  } طالما(معلومات->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  إذا(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    إذا(معلومات->info.httpproxycode / 100 != 2 &&
       معلومات->info.httpproxycode != 101) {
      curlx_safefree(معلومات->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, معلومات);
      failf(معلومات, "CONNECT-UDP tunnel failed, response %d",
            معلومات->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    إذا(معلومات->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(معلومات->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, معلومات);
      failf(معلومات, "CONNECT tunnel failed, response %d", معلومات->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, معلومات);
  infof(معلومات, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", معلومات->info.httpproxycode);
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, معلومات);
  أرجع نتيجو;
}

ساكن CURLcode cf_h1_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *پورو)
{
  CURLcode نتيجو;
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(معلومات, cf, "connect");
  نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
  إذا(نتيجو || !*پورو)
    أرجع نتيجو;

  *پورو = FALSE;
  إذا(!ts) {
    نتيجو = tunnel_init(cf, معلومات, &ts);
    إذا(نتيجو)
      أرجع نتيجو;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  نتيجو = H1_CONNECT(cf, معلومات, ts);
  إذا(نتيجو)
    اذهب ٻاهريون;
  curlx_safefree(معلومات->req.hd_proxy_auth);

ٻاهريون:
  *پورو = (نتيجو == CURLE_OK) && tunnel_is_established(pctx->ts);
  إذا(*پورو) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&معلومات->req, معلومات);
    Curl_client_reset(معلومات);
    Curl_pgrsReset(معلومات);
    cf_tunnel_free(cf, معلومات);
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_h1_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;
  CURLcode نتيجو = CURLE_OK;

  إذا(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
    إذا(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      إذا(tunnel_want_send(ts))
        نتيجو = Curl_pollset_سيٽ_ٻاهريون_only(معلومات, ps, sock);
      وإلا
        نتيجو = Curl_pollset_سيٽ_اندريون_only(معلومات, ps, sock);
    }
    وإلا
      نتيجو = Curl_pollset_سيٽ_ٻاهريون_only(معلومات, ps, sock);
  }
  وإلا {
    إذا(cf->ايندڙ)
      نتيجو = cf->ايندڙ->cft->adjust_pollset(cf->ايندڙ, معلومات, ps);
  }
  أرجع نتيجو;
}

ساكن bool cf_h1_proxy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  أرجع cf->ايندڙ ? cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات) : FALSE;
}

ساكن فراغ cf_h1_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  إذا(pctx) {
    cf_tunnel_free(cf, معلومات);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ساكن CURLcode cf_h1_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
  بدل(query) {
  حالة CF_QUERY_HOST_پورٽ:
    إذا(!ts || !ts->dest)
      اكسر;
    *pres1 = (صحيح)ts->dest->پورٽ;
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
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
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
  cf_h1_proxy_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       صحيح httpversion,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf;
  بنية cf_h1_proxy_ctx *pctx;
  بنية h1_tunnel_state *ts;
  CURLcode نتيجو;

  (فراغ)معلومات;
  إذا(!dest)
    أرجع CURLE_FAILED_INIT;
  إذا((httpversion < 10) || (httpversion >= 20))
    أرجع CURLE_FAILED_INIT;

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرا);
  curlx_dyn_init(&ts->request_معلومات, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(معلومات, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, حجم(*pctx));
  إذا(!pctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  نتيجو = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  إذا(نتيجو) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    اذهب ٻاهريون;
  }
  ts = عدم;
  Curl_conn_cf_وجهو_after(cf_at, cf);

ٻاهريون:
  tunnel_free(ts, معلومات);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
