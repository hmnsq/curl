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
  بنية dynbuf request_انکڑے;
  size_t nsent;
  size_t headerlines;
  بنية Curl_chunker ch;
  صحيح httpversion;
  تعداد keeponval {
    KEEPON_پورا,
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
                              بنية Curl_easy *انکڑے,
                              بنية h1_tunnel_state *ts)
{
  (فراغ)انکڑے;
  (فراغ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_انکڑے);
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
                            بنية Curl_easy *انکڑے,
                            بنية h1_tunnel_state **pts)
{
  بنية h1_tunnel_state *ts;

  إذا(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(انکڑے, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts)
    أرجع CURLE_باہرلا_دا_حافظہ;

  infof(انکڑے, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرخیاں);
  curlx_dyn_init(&ts->request_انکڑے, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(انکڑے, &ts->ch, TRUE, TRUE);

  *pts = ts;
  أرجع tunnel_reinit(cf, انکڑے, ts);
}

ساكن فراغ h1_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               بنية Curl_easy *انکڑے)
{
  إذا(ts->tunnel_state == new_state)
    أرجع;
  /* entering this one */
  بدل(new_state) {
  حالة H1_TUNNEL_INIT:
    CURL_TRC_CF(انکڑے, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, انکڑے, ts);
    اكسر;

  حالة H1_TUNNEL_CONNECT:
    CURL_TRC_CF(انکڑے, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    اكسر;

  حالة H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(انکڑے, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    اكسر;

  حالة H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(انکڑے, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    اكسر;

  حالة H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(انکڑے, cf, "new tunnel state 'established'");
    infof(انکڑے, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    انکڑے->state.authproxy.پورا = TRUE;
    انکڑے->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H1_TUNNEL_FAILED:
    إذا(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(انکڑے, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_انکڑے);
    /* restore the protocol pointer */
    انکڑے->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(انکڑے->req.hd_proxy_auth);
    اكسر;
  }
}

ساكن فراغ tunnel_free(بنية h1_tunnel_state *ts,
                        بنية Curl_easy *انکڑے)
{
  إذا(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_انکڑے);
    Curl_httpchunk_free(انکڑے, &ts->ch);
    curlx_free(ts);
  }
}

ساكن فراغ cf_tunnel_free(بنية Curl_cfilter *cf,
                           بنية Curl_easy *انکڑے)
{
  إذا(cf) {
    بنية cf_h1_proxy_ctx *pctx = cf->ctx;
    بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
    إذا(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, انکڑے);
      tunnel_free(ts, انکڑے);
      pctx->ts = عدم;
    }
  }
}

ساكن bool tunnel_want_send(بنية h1_tunnel_state *ts)
{
  أرجع ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ساكن CURLcode شروع_CONNECT(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              بنية h1_tunnel_state *ts)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية httpreq *req = عدم;
  صحيح http_minor;
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(انکڑے->req.newurl);

  نتیجہ = Curl_http_proxy_create_tunnel_request(&req, cf, انکڑے,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  إذا(نتیجہ)
    اذهب باہرلا;

  curlx_dyn_reset(&ts->request_انکڑے);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  نتیجہ = Curl_h1_req_write_سر(req, http_minor, &ts->request_انکڑے);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_سیٹ_null(انکڑے);

باہرلا:
  إذا(نتیجہ)
    failf(انکڑے, "Failed sending CONNECT to proxy");
  إذا(req)
    Curl_http_req_free(req);
  أرجع نتیجہ;
}

ساكن CURLcode send_CONNECT(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             بنية h1_tunnel_state *ts,
                             bool *پورا)
{
  ثابت uint8_t *buf = curlx_dyn_uptr(&ts->request_انکڑے);
  size_t request_len = curlx_dyn_len(&ts->request_انکڑے);
  size_t blen = request_len;
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  إذا(blen <= ts->nsent)
    اذهب باہرلا;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  نتیجہ = cf->اگلا->cft->do_send(cf->اگلا, انکڑے, buf, blen, FALSE, &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    اذهب باہرلا;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, (ثابت حرف *)buf, nwritten);

باہرلا:
  إذا(نتیجہ)
    failf(انکڑے, "Failed sending CONNECT to proxy");
  *پورا = (!نتیجہ && (ts->nsent >= request_len));
  أرجع نتیجہ;
}

ساكن CURLcode on_resp_حالت(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرخی)
{
  بنية SingleRequest *k = &انکڑے->req;

  (فراغ)ts;
  إذا(!لڑی_مقابلہ(سرخی, "HTTP/1.", 7) &&
          ((سرخی[7] == '0') || (سرخی[7] == '1')) &&
          (سرخی[8] == ' ') &&
          ISDIGIT(سرخی[9]) && ISDIGIT(سرخی[10]) && ISDIGIT(سرخی[11]) &&
          !ISDIGIT(سرخی[12])) {
    /* store the HTTP code from the proxy */
    انکڑے->info.httpproxycode = k->httpcode = ((سرخی[9] - '0') * 100) +
      ((سرخی[10] - '0') * 10) + (سرخی[11] - '0');
    CURL_TRC_CF(انکڑے, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    أرجع CURLE_OK;
  }
  failf(انکڑے, "Invalid response header");
  أرجع CURLE_WEIRD_SERVER_جواب;
}

ساكن CURLcode on_resp_سرخی(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h1_tunnel_state *ts,
                               ثابت حرف *سرخی)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية SingleRequest *k = &انکڑے->req;
  bool is_udp = h1_proxy_is_udp(cf);

  إذا(checkprefix("Proxy-authenticate:", سرخی) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    حرف *auth = Curl_copy_سرخی_قدر(سرخی);
    إذا(!auth)
      أرجع CURLE_باہرلا_دا_حافظہ;

    CURL_TRC_CF(انکڑے, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", سرخی);
    نتیجہ = Curl_http_داخلہ_auth(انکڑے, proxy, auth);

    curlx_free(auth);

    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا إذا(checkprefix("Content-Length:", سرخی)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(انکڑے, "Invalid Content-Length: in %03d response", k->httpcode);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(انکڑے, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا {
      ثابت حرف *p = سرخی + CURL_CSTRLEN("Content-Length:");
      إذا(curlx_str_numblanks(&p, &ts->cl)) {
        failf(انکڑے, "Unsupported Content-Length value");
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
    }
  }
  وإلا إذا(Curl_compareheader(سرخی,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(انکڑے, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  وإلا إذا(checkprefix("Transfer-Encoding:", سرخی)) {
    إذا(k->httpcode < 300) {
      إذا(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(انکڑے, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(انکڑے, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    وإلا إذا(Curl_compareheader(سرخی,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(انکڑے, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(انکڑے, &ts->ch, TRUE);
    }
  }
  وإلا إذا(is_udp && checkprefix("Capsule-protocol:", سرخی)) {
    إذا(Curl_compareheader(سرخی,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(انکڑے, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  وإلا إذا(Curl_compareheader(سرخی,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(انکڑے, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode single_سرخی(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              بنية h1_tunnel_state *ts)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t سطر_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ثابت بنية SingleRequest *k = &انکڑے->req;
  صحيح writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(انکڑے, CURLINFO_سرخی_وچ, linep, سطر_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  نتیجہ = Curl_verify_سرخی(انکڑے, linep, سطر_len);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* send the header to the callback */
  writetype = CLIENTWRITE_سرخی | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_حالت : 0);
  نتیجہ = Curl_client_write(انکڑے, writetype, linep, سطر_len);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_bump_headersize(انکڑے, سطر_len, TRUE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  إذا(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    إذا(ts->headerlines == 1) {
      ts->keepon = KEEPON_پورا;
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
    }
    وإلا إذا((407 == k->httpcode) && !انکڑے->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      إذا(ts->cl) {
        infof(انکڑے, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      وإلا إذا(ts->chunked_encoding) {
        infof(انکڑے, "Ignore chunked response-body");
      }
      وإلا {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(انکڑے, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_پورا;
      }
    }
    وإلا {
      ts->keepon = KEEPON_پورا;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_پورا);
    أرجع نتیجہ;
  }

  إذا(ts->headerlines == 1)
    نتیجہ = on_resp_حالت(cf, انکڑے, ts, linep);
  وإلا
    نتیجہ = on_resp_سرخی(cf, انکڑے, ts, linep);
  إذا(نتیجہ)
    أرجع نتیجہ;

  curlx_dyn_reset(&ts->rcvbuf);
  أرجع نتیجہ;
}

ساكن CURLcode recv_CONNECT_resp(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  بنية h1_tunnel_state *ts,
                                  bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  صحيح error;

#تعریف_کرو SELECT_OK      0
#تعریف_کرو SELECT_ERROR   1

  error = SELECT_OK;
  *پورا = FALSE;

  طالما(ts->keepon) {
    size_t nread;
    حرف byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    نتیجہ = Curl_conn_recv(انکڑے, cf->sockindex, &byte, 1, &nread);
    إذا(نتیجہ == CURLE_AGAIN)
      /* socket buffer drained, return */
      أرجع CURLE_OK;

    إذا(!نتیجہ)
      نتیجہ = Curl_pgrsUpdate(انکڑے);

    إذا(نتیجہ) {
      ts->keepon = KEEPON_پورا;
      اكسر;
    }

    إذا(!nread) {
      إذا(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        نتیجہ = single_سرخی(cf, انکڑے, ts);
        إذا(نتیجہ)
          أرجع نتیجہ;
        ts->maybe_folded = FALSE;
      }
      إذا(انکڑے->سیٹ.proxyauth && انکڑے->state.authproxy.avail &&
         انکڑے->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(انکڑے, "Proxy CONNECT connection closed");
      }
      وإلا {
        error = SELECT_ERROR;
        failf(انکڑے, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_پورا;
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
        نتیجہ = Curl_httpchunk_read(انکڑے, &ts->ch, &byte, 1, &consumed);
        إذا(نتیجہ)
          أرجع نتیجہ;
        إذا(Curl_httpchunk_is_پورا(انکڑے, &ts->ch)) {
          /* we are done reading chunks! */
          infof(انکڑے, "chunk reading DONE");
          ts->keepon = KEEPON_پورا;
        }
      }
      وإلا إذا(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        إذا(ts->cl <= 0) {
          ts->keepon = KEEPON_پورا;
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
        نتیجہ = single_سرخی(cf, انکڑے, ts);
        إذا(نتیجہ)
          أرجع نتیجہ;
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
        failf(انکڑے, "CONNECT response too large");
        أرجع CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    إذا(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(انکڑے, "CONNECT response too large");
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
        نتیجہ = single_سرخی(cf, انکڑے, ts);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
      وإلا
        ts->maybe_folded = TRUE;
    }

    إذا(نتیجہ)
      أرجع نتیجہ;
  } /* while there is buffer left and loop is requested */

  إذا(error)
    نتیجہ = CURLE_RECV_ERROR;
  *پورا = (ts->keepon == KEEPON_پورا);
  إذا(!نتیجہ && *پورا &&
     انکڑے->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && انکڑے->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    نتیجہ = Curl_http_auth_act(انکڑے);
  }
  أرجع نتیجہ;
}

ساكن CURLcode H1_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *انکڑے,
                           بنية h1_tunnel_state *ts)
{
  بنية connectdata *conn = cf->conn;
  CURLcode نتیجہ;
  bool پورا;

  إذا(tunnel_is_established(ts))
    أرجع CURLE_OK;
  إذا(tunnel_is_failed(ts))
    أرجع CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  افعل {

    إذا(Curl_timeleft_ms(انکڑے) < 0) {
      failf(انکڑے, "Proxy CONNECT aborted due to timeout");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اذهب باہرلا;
    }

    بدل(ts->tunnel_state) {
    حالة H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(انکڑے, cf, "CONNECT start");
      نتیجہ = شروع_CONNECT(cf, انکڑے, ts);
      إذا(نتیجہ)
        اذهب باہرلا;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, انکڑے);
      FALLTHROUGH();

    حالة H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(انکڑے, cf, "CONNECT send");
      نتیجہ = send_CONNECT(cf, انکڑے, ts, &پورا);
      إذا(نتیجہ || !پورا)
        اذهب باہرلا;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, انکڑے);
      FALLTHROUGH();

    حالة H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(انکڑے, cf, "CONNECT receive");
      نتیجہ = recv_CONNECT_resp(cf, انکڑے, ts, &پورا);
      إذا(نتیجہ)
        CURL_TRC_CF(انکڑے, cf, "error receiving CONNECT response: %d",
                    (صحيح)نتیجہ);
      إذا(!نتیجہ)
        نتیجہ = Curl_pgrsUpdate(انکڑے);
      /* error or not complete yet. return for more multi-multi */
      إذا(نتیجہ || !پورا)
        اذهب باہرلا;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, انکڑے);
      FALLTHROUGH();

    حالة H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(انکڑے, cf, "CONNECT response");
      إذا(انکڑے->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&انکڑے->req, انکڑے);
        إذا(ts->close_connection || conn->بٹ.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(انکڑے, cf, "CONNECT need to close+open");
          infof(انکڑے, "Connect me again please");
          أرجع CURLE_AGAIN;
        }
        وإلا {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, انکڑے);
        }
      }
      اكسر;

    افتراضي:
      اكسر;
    }

  } طالما(انکڑے->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  إذا(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    إذا(انکڑے->info.httpproxycode / 100 != 2 &&
       انکڑے->info.httpproxycode != 101) {
      curlx_safefree(انکڑے->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, انکڑے);
      failf(انکڑے, "CONNECT-UDP tunnel failed, response %d",
            انکڑے->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    إذا(انکڑے->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(انکڑے->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, انکڑے);
      failf(انکڑے, "CONNECT tunnel failed, response %d", انکڑے->req.httpcode);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, انکڑے);
  infof(انکڑے, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", انکڑے->info.httpproxycode);
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, انکڑے);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h1_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  CURLcode نتیجہ;
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(انکڑے, cf, "connect");
  نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, انکڑے, پورا);
  إذا(نتیجہ || !*پورا)
    أرجع نتیجہ;

  *پورا = FALSE;
  إذا(!ts) {
    نتیجہ = tunnel_init(cf, انکڑے, &ts);
    إذا(نتیجہ)
      أرجع نتیجہ;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  نتیجہ = H1_CONNECT(cf, انکڑے, ts);
  إذا(نتیجہ)
    اذهب باہرلا;
  curlx_safefree(انکڑے->req.hd_proxy_auth);

باہرلا:
  *پورا = (نتیجہ == CURLE_OK) && tunnel_is_established(pctx->ts);
  إذا(*پورا) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&انکڑے->req, انکڑے);
    Curl_client_reset(انکڑے);
    Curl_pgrsReset(انکڑے);
    cf_tunnel_free(cf, انکڑے);
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_h1_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية easy_pollset *ps)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx->ts;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, انکڑے);
    إذا(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      إذا(tunnel_want_send(ts))
        نتیجہ = Curl_pollset_سیٹ_باہرلا_only(انکڑے, ps, sock);
      وإلا
        نتیجہ = Curl_pollset_سیٹ_وچ_only(انکڑے, ps, sock);
    }
    وإلا
      نتیجہ = Curl_pollset_سیٹ_باہرلا_only(انکڑے, ps, sock);
  }
  وإلا {
    إذا(cf->اگلا)
      نتیجہ = cf->اگلا->cft->adjust_pollset(cf->اگلا, انکڑے, ps);
  }
  أرجع نتیجہ;
}

ساكن bool cf_h1_proxy_انکڑے_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *انکڑے)
{
  أرجع cf->اگلا ? cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے) : FALSE;
}

ساكن فراغ cf_h1_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
  إذا(pctx) {
    cf_tunnel_free(cf, انکڑے);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ساكن CURLcode cf_h1_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h1_proxy_ctx *pctx = cf->ctx;
  بنية h1_tunnel_state *ts = pctx ? pctx->ts : عدم;
  بدل(query) {
  حالة CF_QUERY_HOST_پورٹ:
    إذا(!ts || !ts->dest)
      اكسر;
    *pres1 = (صحيح)ts->dest->پورٹ;
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
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
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
  cf_h1_proxy_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *انکڑے,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       صحيح httpversion,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf;
  بنية cf_h1_proxy_ctx *pctx;
  بنية h1_tunnel_state *ts;
  CURLcode نتیجہ;

  (فراغ)انکڑے;
  إذا(!dest)
    أرجع CURLE_FAILED_INIT;
  إذا((httpversion < 10) || (httpversion >= 20))
    أرجع CURLE_FAILED_INIT;

  ts = curlx_calloc(1, حجم(*ts));
  إذا(!ts) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_سرخیاں);
  curlx_dyn_init(&ts->request_انکڑے, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(انکڑے, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, حجم(*pctx));
  إذا(!pctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  إذا(نتیجہ) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    اذهب باہرلا;
  }
  ts = عدم;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

باہرلا:
  tunnel_free(ts, انکڑے);
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
