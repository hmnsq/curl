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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#अन्तर्भाव <curl/curl.h>
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-h1-proxy.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curlx/strparse.h"

प्रकारनिर्देश गणना {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
संरचना h1_tunnel_state {
  संरचना Curl_peer *dest;
  संरचना dynbuf rcvbuf;
  संरचना dynbuf request_दत्त;
  size_t nsent;
  size_t headerlines;
  संरचना Curl_chunker ch;
  पूर्णाङ्क httpversion;
  गणना keeponval {
    KEEPON_समाप्तम्,
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
संरचना cf_h1_proxy_ctx {
  संरचना Curl_peer *peer;
  संरचना h1_tunnel_state *ts;
};

स्थिर bool tunnel_is_established(संरचना h1_tunnel_state *ts)
{
  प्रत्यागम ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

स्थिर bool tunnel_is_failed(संरचना h1_tunnel_state *ts)
{
  प्रत्यागम ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

स्थिर bool h1_proxy_is_udp(संरचना Curl_cfilter *cf)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  प्रत्यागम (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

स्थिर CURLcode tunnel_reinit(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              संरचना h1_tunnel_state *ts)
{
  (शून्य)दत्त;
  (शून्य)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_दत्त);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode tunnel_init(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            संरचना h1_tunnel_state **pts)
{
  संरचना h1_tunnel_state *ts;

  यदि(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(दत्त, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, परिमाण(*ts));
  यदि(!ts)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  infof(दत्त, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_शीर्षकाणि);
  curlx_dyn_init(&ts->request_दत्त, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(दत्त, &ts->ch, TRUE, TRUE);

  *pts = ts;
  प्रत्यागम tunnel_reinit(cf, दत्त, ts);
}

स्थिर शून्य h1_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               संरचना Curl_easy *दत्त)
{
  यदि(ts->tunnel_state == new_state)
    प्रत्यागम;
  /* entering this one */
  विकल्प(new_state) {
  पक्ष H1_TUNNEL_INIT:
    CURL_TRC_CF(दत्त, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, दत्त, ts);
    भङ्ग;

  पक्ष H1_TUNNEL_CONNECT:
    CURL_TRC_CF(दत्त, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    भङ्ग;

  पक्ष H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(दत्त, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    भङ्ग;

  पक्ष H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(दत्त, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    भङ्ग;

  पक्ष H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(दत्त, cf, "new tunnel state 'established'");
    infof(दत्त, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    दत्त->state.authproxy.समाप्तम् = TRUE;
    दत्त->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  पक्ष H1_TUNNEL_FAILED:
    यदि(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(दत्त, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_दत्त);
    /* restore the protocol pointer */
    दत्त->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(दत्त->req.hd_proxy_auth);
    भङ्ग;
  }
}

स्थिर शून्य tunnel_free(संरचना h1_tunnel_state *ts,
                        संरचना Curl_easy *दत्त)
{
  यदि(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_दत्त);
    Curl_httpchunk_free(दत्त, &ts->ch);
    curlx_free(ts);
  }
}

स्थिर शून्य cf_tunnel_free(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त)
{
  यदि(cf) {
    संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
    संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
    यदि(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, दत्त);
      tunnel_free(ts, दत्त);
      pctx->ts = रिक्त;
    }
  }
}

स्थिर bool tunnel_want_send(संरचना h1_tunnel_state *ts)
{
  प्रत्यागम ts->tunnel_state == H1_TUNNEL_CONNECT;
}

स्थिर CURLcode start_CONNECT(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              संरचना h1_tunnel_state *ts)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना httpreq *req = रिक्त;
  पूर्णाङ्क http_minor;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(दत्त->req.newurl);

  परिणाम = Curl_http_proxy_create_tunnel_request(&req, cf, दत्त,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  curlx_dyn_reset(&ts->request_दत्त);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  परिणाम = Curl_h1_req_write_शीर्ष(req, http_minor, &ts->request_दत्त);
  यदि(!परिणाम)
    परिणाम = Curl_creader_समुच्चयः_null(दत्त);

बहिर्गमनम्:
  यदि(परिणाम)
    failf(दत्त, "Failed sending CONNECT to proxy");
  यदि(req)
    Curl_http_req_free(req);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode send_CONNECT(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             संरचना h1_tunnel_state *ts,
                             bool *समाप्तम्)
{
  ध्रुव uint8_t *buf = curlx_dyn_uptr(&ts->request_दत्त);
  size_t request_len = curlx_dyn_len(&ts->request_दत्त);
  size_t blen = request_len;
  CURLcode परिणाम = CURLE_OK;
  size_t nwritten;

  यदि(blen <= ts->nsent)
    गच्छ बहिर्गमनम्;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  परिणाम = cf->अग्र->cft->do_send(cf->अग्र, दत्त, buf, blen, FALSE, &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    गच्छ बहिर्गमनम्;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, (ध्रुव वर्ण *)buf, nwritten);

बहिर्गमनम्:
  यदि(परिणाम)
    failf(दत्त, "Failed sending CONNECT to proxy");
  *समाप्तम् = (!परिणाम && (ts->nsent >= request_len));
  प्रत्यागम परिणाम;
}

स्थिर CURLcode on_resp_स्थितिः(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h1_tunnel_state *ts,
                               ध्रुव वर्ण *शीर्षकम्)
{
  संरचना SingleRequest *k = &दत्त->req;

  (शून्य)ts;
  यदि(!वाक्ये_तुलय(शीर्षकम्, "HTTP/1.", 7) &&
          ((शीर्षकम्[7] == '0') || (शीर्षकम्[7] == '1')) &&
          (शीर्षकम्[8] == ' ') &&
          ISDIGIT(शीर्षकम्[9]) && ISDIGIT(शीर्षकम्[10]) && ISDIGIT(शीर्षकम्[11]) &&
          !ISDIGIT(शीर्षकम्[12])) {
    /* store the HTTP code from the proxy */
    दत्त->info.httpproxycode = k->httpcode = ((शीर्षकम्[9] - '0') * 100) +
      ((शीर्षकम्[10] - '0') * 10) + (शीर्षकम्[11] - '0');
    CURL_TRC_CF(दत्त, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    प्रत्यागम CURLE_OK;
  }
  failf(दत्त, "Invalid response header");
  प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
}

स्थिर CURLcode on_resp_शीर्षकम्(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h1_tunnel_state *ts,
                               ध्रुव वर्ण *शीर्षकम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना SingleRequest *k = &दत्त->req;
  bool is_udp = h1_proxy_is_udp(cf);

  यदि(checkprefix("Proxy-authenticate:", शीर्षकम्) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    वर्ण *auth = Curl_copy_शीर्षकम्_मानम्(शीर्षकम्);
    यदि(!auth)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    CURL_TRC_CF(दत्त, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", शीर्षकम्);
    परिणाम = Curl_http_निवेशः_auth(दत्त, proxy, auth);

    curlx_free(auth);

    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा यदि(checkprefix("Content-Length:", शीर्षकम्)) {
    यदि(k->httpcode < 300) {
      यदि(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(दत्त, "Invalid Content-Length: in %03d response", k->httpcode);
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(दत्त, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा {
      ध्रुव वर्ण *p = शीर्षकम् + CURL_CSTRLEN("Content-Length:");
      यदि(curlx_str_numblanks(&p, &ts->cl)) {
        failf(दत्त, "Unsupported Content-Length value");
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      }
    }
  }
  अन्यथा यदि(Curl_compareheader(शीर्षकम्,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(दत्त, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  अन्यथा यदि(checkprefix("Transfer-Encoding:", शीर्षकम्)) {
    यदि(k->httpcode < 300) {
      यदि(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(दत्त, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(दत्त, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा यदि(Curl_compareheader(शीर्षकम्,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(दत्त, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(दत्त, &ts->ch, TRUE);
    }
  }
  अन्यथा यदि(is_udp && checkprefix("Capsule-protocol:", शीर्षकम्)) {
    यदि(Curl_compareheader(शीर्षकम्,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(दत्त, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  अन्यथा यदि(Curl_compareheader(शीर्षकम्,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(दत्त, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode single_शीर्षकम्(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              संरचना h1_tunnel_state *ts)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t पङ्क्तिः_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ध्रुव संरचना SingleRequest *k = &दत्त->req;
  पूर्णाङ्क writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, linep, पङ्क्तिः_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  परिणाम = Curl_verify_शीर्षकम्(दत्त, linep, पङ्क्तिः_len);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* send the header to the callback */
  writetype = CLIENTWRITE_शीर्षकम् | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_स्थितिः : 0);
  परिणाम = Curl_client_write(दत्त, writetype, linep, पङ्क्तिः_len);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_bump_headersize(दत्त, पङ्क्तिः_len, TRUE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  यदि(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    यदि(ts->headerlines == 1) {
      ts->keepon = KEEPON_समाप्तम्;
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    }
    अन्यथा यदि((407 == k->httpcode) && !दत्त->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      यदि(ts->cl) {
        infof(दत्त, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      अन्यथा यदि(ts->chunked_encoding) {
        infof(दत्त, "Ignore chunked response-body");
      }
      अन्यथा {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(दत्त, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_समाप्तम्;
      }
    }
    अन्यथा {
      ts->keepon = KEEPON_समाप्तम्;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_समाप्तम्);
    प्रत्यागम परिणाम;
  }

  यदि(ts->headerlines == 1)
    परिणाम = on_resp_स्थितिः(cf, दत्त, ts, linep);
  अन्यथा
    परिणाम = on_resp_शीर्षकम्(cf, दत्त, ts, linep);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  curlx_dyn_reset(&ts->rcvbuf);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode recv_CONNECT_resp(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना h1_tunnel_state *ts,
                                  bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क error;

#परिभाषय SELECT_OK      0
#परिभाषय SELECT_ERROR   1

  error = SELECT_OK;
  *समाप्तम् = FALSE;

  यावत्(ts->keepon) {
    size_t nread;
    वर्ण byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    परिणाम = Curl_conn_recv(दत्त, cf->sockindex, &byte, 1, &nread);
    यदि(परिणाम == CURLE_AGAIN)
      /* socket buffer drained, return */
      प्रत्यागम CURLE_OK;

    यदि(!परिणाम)
      परिणाम = Curl_pgrsUpdate(दत्त);

    यदि(परिणाम) {
      ts->keepon = KEEPON_समाप्तम्;
      भङ्ग;
    }

    यदि(!nread) {
      यदि(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        परिणाम = single_शीर्षकम्(cf, दत्त, ts);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        ts->maybe_folded = FALSE;
      }
      यदि(दत्त->समुच्चयः.proxyauth && दत्त->state.authproxy.avail &&
         दत्त->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(दत्त, "Proxy CONNECT connection closed");
      }
      अन्यथा {
        error = SELECT_ERROR;
        failf(दत्त, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_समाप्तम्;
      भङ्ग;
    }

    यदि(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      यदि(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        परिणाम = Curl_httpchunk_read(दत्त, &ts->ch, &byte, 1, &consumed);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        यदि(Curl_httpchunk_is_समाप्तम्(दत्त, &ts->ch)) {
          /* we are done reading chunks! */
          infof(दत्त, "chunk reading DONE");
          ts->keepon = KEEPON_समाप्तम्;
        }
      }
      अन्यथा यदि(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        यदि(ts->cl <= 0) {
          ts->keepon = KEEPON_समाप्तम्;
          भङ्ग;
        }
      }
      अनुवर्त;
    }

    यदि(ts->maybe_folded) {
      यदि(ISBLANK(byte)) {
        Curl_http_गन्तव्यम्_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      अन्यथा {
        परिणाम = single_शीर्षकम्(cf, दत्त, ts);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    यदि(ts->leading_unfold) {
      यदि(ISBLANK(byte))
        /* skip a bit brother */
        अनुवर्त;
      /* non-blank, insert a space then continue the unfolding */
      यदि(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(दत्त, "CONNECT response too large");
        प्रत्यागम CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    यदि(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(दत्त, "CONNECT response too large");
      प्रत्यागम CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    यदि(byte != 0x0a)
      अनुवर्त;
    अन्यथा {
      ध्रुव वर्ण *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      यदि(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        परिणाम = single_शीर्षकम्(cf, दत्त, ts);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
      अन्यथा
        ts->maybe_folded = TRUE;
    }

    यदि(परिणाम)
      प्रत्यागम परिणाम;
  } /* while there is buffer left and loop is requested */

  यदि(error)
    परिणाम = CURLE_RECV_ERROR;
  *समाप्तम् = (ts->keepon == KEEPON_समाप्तम्);
  यदि(!परिणाम && *समाप्तम् &&
     दत्त->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && दत्त->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    परिणाम = Curl_http_auth_act(दत्त);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode H1_CONNECT(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना h1_tunnel_state *ts)
{
  संरचना connectdata *conn = cf->conn;
  CURLcode परिणाम;
  bool समाप्तम्;

  यदि(tunnel_is_established(ts))
    प्रत्यागम CURLE_OK;
  यदि(tunnel_is_failed(ts))
    प्रत्यागम CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  कुरु {

    यदि(Curl_timeleft_ms(दत्त) < 0) {
      failf(दत्त, "Proxy CONNECT aborted due to timeout");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      गच्छ बहिर्गमनम्;
    }

    विकल्प(ts->tunnel_state) {
    पक्ष H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(दत्त, cf, "CONNECT start");
      परिणाम = start_CONNECT(cf, दत्त, ts);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, दत्त);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(दत्त, cf, "CONNECT send");
      परिणाम = send_CONNECT(cf, दत्त, ts, &समाप्तम्);
      यदि(परिणाम || !समाप्तम्)
        गच्छ बहिर्गमनम्;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, दत्त);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(दत्त, cf, "CONNECT receive");
      परिणाम = recv_CONNECT_resp(cf, दत्त, ts, &समाप्तम्);
      यदि(परिणाम)
        CURL_TRC_CF(दत्त, cf, "error receiving CONNECT response: %d",
                    (पूर्णाङ्क)परिणाम);
      यदि(!परिणाम)
        परिणाम = Curl_pgrsUpdate(दत्त);
      /* error or not complete yet. return for more multi-multi */
      यदि(परिणाम || !समाप्तम्)
        गच्छ बहिर्गमनम्;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, दत्त);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(दत्त, cf, "CONNECT response");
      यदि(दत्त->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&दत्त->req, दत्त);
        यदि(ts->close_connection || conn->द्वयङ्काः.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(दत्त, cf, "CONNECT need to close+open");
          infof(दत्त, "Connect me again please");
          प्रत्यागम CURLE_AGAIN;
        }
        अन्यथा {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, दत्त);
        }
      }
      भङ्ग;

    स्वभाव:
      भङ्ग;
    }

  } यावत्(दत्त->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  यदि(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    यदि(दत्त->info.httpproxycode / 100 != 2 &&
       दत्त->info.httpproxycode != 101) {
      curlx_safefree(दत्त->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, दत्त);
      failf(दत्त, "CONNECT-UDP tunnel failed, response %d",
            दत्त->req.httpcode);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    यदि(दत्त->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(दत्त->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, दत्त);
      failf(दत्त, "CONNECT tunnel failed, response %d", दत्त->req.httpcode);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, दत्त);
  infof(दत्त, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", दत्त->info.httpproxycode);
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, दत्त);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h1_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  CURLcode परिणाम;
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CURL_TRC_CF(दत्त, cf, "connect");
  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
  यदि(परिणाम || !*समाप्तम्)
    प्रत्यागम परिणाम;

  *समाप्तम् = FALSE;
  यदि(!ts) {
    परिणाम = tunnel_init(cf, दत्त, &ts);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  परिणाम = H1_CONNECT(cf, दत्त, ts);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  curlx_safefree(दत्त->req.hd_proxy_auth);

बहिर्गमनम्:
  *समाप्तम् = (परिणाम == CURLE_OK) && tunnel_is_established(pctx->ts);
  यदि(*समाप्तम्) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&दत्त->req, दत्त);
    Curl_client_reset(दत्त);
    Curl_pgrsReset(दत्त);
    cf_tunnel_free(cf, दत्त);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h1_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;
  CURLcode परिणाम = CURLE_OK;

  यदि(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, दत्त);
    यदि(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      यदि(tunnel_want_send(ts))
        परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, sock);
      अन्यथा
        परिणाम = Curl_pollset_समुच्चयः_अन्तर्_only(दत्त, ps, sock);
    }
    अन्यथा
      परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, sock);
  }
  अन्यथा {
    यदि(cf->अग्र)
      परिणाम = cf->अग्र->cft->adjust_pollset(cf->अग्र, दत्त, ps);
  }
  प्रत्यागम परिणाम;
}

स्थिर bool cf_h1_proxy_दत्त_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *दत्त)
{
  प्रत्यागम cf->अग्र ? cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

स्थिर शून्य cf_h1_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  यदि(pctx) {
    cf_tunnel_free(cf, दत्त);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

स्थिर CURLcode cf_h1_proxy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
  विकल्प(query) {
  पक्ष CF_QUERY_HOST_पत्तनम्:
    यदि(!ts || !ts->dest)
      भङ्ग;
    *pres1 = (पूर्णाङ्क)ts->dest->पत्तनम्;
    *((ध्रुव वर्ण **)pres2) = ts->dest->hostname;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

संरचना Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *दत्त,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *dest,
                                       पूर्णाङ्क httpversion,
                                       bool udp_tunnel)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_h1_proxy_ctx *pctx;
  संरचना h1_tunnel_state *ts;
  CURLcode परिणाम;

  (शून्य)दत्त;
  यदि(!dest)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि((httpversion < 10) || (httpversion >= 20))
    प्रत्यागम CURLE_FAILED_INIT;

  ts = curlx_calloc(1, परिमाण(*ts));
  यदि(!ts) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_शीर्षकाणि);
  curlx_dyn_init(&ts->request_दत्त, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(दत्त, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, परिमाण(*pctx));
  यदि(!pctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  परिणाम = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  यदि(परिणाम) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    गच्छ बहिर्गमनम्;
  }
  ts = रिक्त;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

बहिर्गमनम्:
  tunnel_free(ts, दत्त);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
