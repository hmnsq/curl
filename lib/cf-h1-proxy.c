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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#অন্তর্ভাব <curl/curl.h>
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-h1-proxy.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curlx/strparse.h"

প্রকারলেখ গণনা {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
কাঠামো h1_tunnel_state {
  কাঠামো Curl_peer *dest;
  কাঠামো dynbuf rcvbuf;
  কাঠামো dynbuf request_তথ্য;
  size_t nsent;
  size_t headerlines;
  কাঠামো Curl_chunker ch;
  পূর্ণসংখ্যা httpversion;
  গণনা keeponval {
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
কাঠামো cf_h1_proxy_ctx {
  কাঠামো Curl_peer *peer;
  কাঠামো h1_tunnel_state *ts;
};

স্থির bool tunnel_is_established(কাঠামো h1_tunnel_state *ts)
{
  ফেরত ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

স্থির bool tunnel_is_failed(কাঠামো h1_tunnel_state *ts)
{
  ফেরত ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

স্থির bool h1_proxy_is_udp(কাঠামো Curl_cfilter *cf)
{
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
  ফেরত (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

স্থির CURLcode tunnel_reinit(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো h1_tunnel_state *ts)
{
  (শূন্য)তথ্য;
  (শূন্য)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_তথ্য);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  ফেরত CURLE_OK;
}

স্থির CURLcode tunnel_init(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো h1_tunnel_state **pts)
{
  কাঠামো h1_tunnel_state *ts;

  যদি(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(তথ্য, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, আকার(*ts));
  যদি(!ts)
    ফেরত CURLE_OUT_OF_MEMORY;

  infof(তথ্য, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_তথ্য, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(তথ্য, &ts->ch, TRUE, TRUE);

  *pts = ts;
  ফেরত tunnel_reinit(cf, তথ্য, ts);
}

স্থির শূন্য h1_tunnel_go_state(কাঠামো Curl_cfilter *cf,
                               কাঠামো h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               কাঠামো Curl_easy *তথ্য)
{
  যদি(ts->tunnel_state == new_state)
    ফেরত;
  /* entering this one */
  নির্বাচন(new_state) {
  ক্ষেত্র H1_TUNNEL_INIT:
    CURL_TRC_CF(তথ্য, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, তথ্য, ts);
    বিরতি;

  ক্ষেত্র H1_TUNNEL_CONNECT:
    CURL_TRC_CF(তথ্য, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    বিরতি;

  ক্ষেত্র H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(তথ্য, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    বিরতি;

  ক্ষেত্র H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(তথ্য, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    বিরতি;

  ক্ষেত্র H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(তথ্য, cf, "new tunnel state 'established'");
    infof(তথ্য, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    তথ্য->state.authproxy.done = TRUE;
    তথ্য->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ক্ষেত্র H1_TUNNEL_FAILED:
    যদি(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(তথ্য, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_তথ্য);
    /* restore the protocol pointer */
    তথ্য->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(তথ্য->req.hd_proxy_auth);
    বিরতি;
  }
}

স্থির শূন্য tunnel_free(কাঠামো h1_tunnel_state *ts,
                        কাঠামো Curl_easy *তথ্য)
{
  যদি(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_তথ্য);
    Curl_httpchunk_free(তথ্য, &ts->ch);
    curlx_free(ts);
  }
}

স্থির শূন্য cf_tunnel_free(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য)
{
  যদি(cf) {
    কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
    কাঠামো h1_tunnel_state *ts = pctx ? pctx->ts : ফাঁকা;
    যদি(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, তথ্য);
      tunnel_free(ts, তথ্য);
      pctx->ts = ফাঁকা;
    }
  }
}

স্থির bool tunnel_want_send(কাঠামো h1_tunnel_state *ts)
{
  ফেরত ts->tunnel_state == H1_TUNNEL_CONNECT;
}

স্থির CURLcode start_CONNECT(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো h1_tunnel_state *ts)
{
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
  কাঠামো httpreq *req = ফাঁকা;
  পূর্ণসংখ্যা http_minor;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(তথ্য->req.newurl);

  ফলাফল = Curl_http_proxy_create_tunnel_request(&req, cf, তথ্য,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  যদি(ফলাফল)
    যাও out;

  curlx_dyn_reset(&ts->request_তথ্য);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  ফলাফল = Curl_h1_req_write_শীর্ষ(req, http_minor, &ts->request_তথ্য);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_set_null(তথ্য);

out:
  যদি(ফলাফল)
    failf(তথ্য, "Failed sending CONNECT to proxy");
  যদি(req)
    Curl_http_req_free(req);
  ফেরত ফলাফল;
}

স্থির CURLcode send_CONNECT(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো h1_tunnel_state *ts,
                             bool *done)
{
  ধ্রুবক uint8_t *buf = curlx_dyn_uptr(&ts->request_তথ্য);
  size_t request_len = curlx_dyn_len(&ts->request_তথ্য);
  size_t blen = request_len;
  CURLcode ফলাফল = CURLE_OK;
  size_t nwritten;

  যদি(blen <= ts->nsent)
    যাও out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  ফলাফল = cf->পরবর্তী->cft->do_send(cf->পরবর্তী, তথ্য, buf, blen, FALSE, &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN)
      ফলাফল = CURLE_OK;
    যাও out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(তথ্য, CURLINFO_HEADER_OUT, (ধ্রুবক অক্ষর *)buf, nwritten);

out:
  যদি(ফলাফল)
    failf(তথ্য, "Failed sending CONNECT to proxy");
  *done = (!ফলাফল && (ts->nsent >= request_len));
  ফেরত ফলাফল;
}

স্থির CURLcode on_resp_status(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h1_tunnel_state *ts,
                               ধ্রুবক অক্ষর *header)
{
  কাঠামো SingleRequest *k = &তথ্য->req;

  (শূন্য)ts;
  যদি(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    তথ্য->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(তথ্য, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    ফেরত CURLE_OK;
  }
  failf(তথ্য, "Invalid response header");
  ফেরত CURLE_WEIRD_SERVER_REPLY;
}

স্থির CURLcode on_resp_header(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h1_tunnel_state *ts,
                               ধ্রুবক অক্ষর *header)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SingleRequest *k = &তথ্য->req;
  bool is_udp = h1_proxy_is_udp(cf);

  যদি(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    অক্ষর *auth = Curl_copy_header_মান(header);
    যদি(!auth)
      ফেরত CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(তথ্য, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    ফলাফল = Curl_http_প্রবেশ_auth(তথ্য, proxy, auth);

    curlx_free(auth);

    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা যদি(checkprefix("Content-Length:", header)) {
    যদি(k->httpcode < 300) {
      যদি(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(তথ্য, "Invalid Content-Length: in %03d response", k->httpcode);
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(তথ্য, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    অন্যথা {
      ধ্রুবক অক্ষর *p = header + CURL_CSTRLEN("Content-Length:");
      যদি(curlx_str_numblanks(&p, &ts->cl)) {
        failf(তথ্য, "Unsupported Content-Length value");
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  অন্যথা যদি(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(তথ্য, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  অন্যথা যদি(checkprefix("Transfer-Encoding:", header)) {
    যদি(k->httpcode < 300) {
      যদি(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(তথ্য, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(তথ্য, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    অন্যথা যদি(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(তথ্য, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(তথ্য, &ts->ch, TRUE);
    }
  }
  অন্যথা যদি(is_udp && checkprefix("Capsule-protocol:", header)) {
    যদি(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(তথ্য, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  অন্যথা যদি(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(তথ্য, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode single_header(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো h1_tunnel_state *ts)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t সারি_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ধ্রুবক কাঠামো SingleRequest *k = &তথ্য->req;
  পূর্ণসংখ্যা writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(তথ্য, CURLINFO_HEADER_IN, linep, সারি_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  ফলাফল = Curl_verify_header(তথ্য, linep, সারি_len);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  ফলাফল = Curl_client_write(তথ্য, writetype, linep, সারি_len);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_bump_headersize(তথ্য, সারি_len, TRUE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  যদি(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    যদি(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
    }
    অন্যথা যদি((407 == k->httpcode) && !তথ্য->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      যদি(ts->cl) {
        infof(তথ্য, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      অন্যথা যদি(ts->chunked_encoding) {
        infof(তথ্য, "Ignore chunked response-body");
      }
      অন্যথা {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(তথ্য, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    অন্যথা {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    ফেরত ফলাফল;
  }

  যদি(ts->headerlines == 1)
    ফলাফল = on_resp_status(cf, তথ্য, ts, linep);
  অন্যথা
    ফলাফল = on_resp_header(cf, তথ্য, ts, linep);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  curlx_dyn_reset(&ts->rcvbuf);
  ফেরত ফলাফল;
}

স্থির CURLcode recv_CONNECT_resp(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  যখন(ts->keepon) {
    size_t nread;
    অক্ষর byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    ফলাফল = Curl_conn_recv(তথ্য, cf->sockindex, &byte, 1, &nread);
    যদি(ফলাফল == CURLE_AGAIN)
      /* socket buffer drained, return */
      ফেরত CURLE_OK;

    যদি(!ফলাফল)
      ফলাফল = Curl_pgrsUpdate(তথ্য);

    যদি(ফলাফল) {
      ts->keepon = KEEPON_DONE;
      বিরতি;
    }

    যদি(!nread) {
      যদি(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        ফলাফল = single_header(cf, তথ্য, ts);
        যদি(ফলাফল)
          ফেরত ফলাফল;
        ts->maybe_folded = FALSE;
      }
      যদি(তথ্য->set.proxyauth && তথ্য->state.authproxy.avail &&
         তথ্য->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(তথ্য, "Proxy CONNECT connection closed");
      }
      অন্যথা {
        error = SELECT_ERROR;
        failf(তথ্য, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      বিরতি;
    }

    যদি(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      যদি(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        ফলাফল = Curl_httpchunk_read(তথ্য, &ts->ch, &byte, 1, &consumed);
        যদি(ফলাফল)
          ফেরত ফলাফল;
        যদি(Curl_httpchunk_is_done(তথ্য, &ts->ch)) {
          /* we are done reading chunks! */
          infof(তথ্য, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      অন্যথা যদি(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        যদি(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          বিরতি;
        }
      }
      চালু;
    }

    যদি(ts->maybe_folded) {
      যদি(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      অন্যথা {
        ফলাফল = single_header(cf, তথ্য, ts);
        যদি(ফলাফল)
          ফেরত ফলাফল;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    যদি(ts->leading_unfold) {
      যদি(ISBLANK(byte))
        /* skip a bit brother */
        চালু;
      /* non-blank, insert a space then continue the unfolding */
      যদি(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(তথ্য, "CONNECT response too large");
        ফেরত CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    যদি(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(তথ্য, "CONNECT response too large");
      ফেরত CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    যদি(byte != 0x0a)
      চালু;
    অন্যথা {
      ধ্রুবক অক্ষর *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      যদি(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        ফলাফল = single_header(cf, তথ্য, ts);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
      অন্যথা
        ts->maybe_folded = TRUE;
    }

    যদি(ফলাফল)
      ফেরত ফলাফল;
  } /* while there is buffer left and loop is requested */

  যদি(error)
    ফলাফল = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  যদি(!ফলাফল && *done &&
     তথ্য->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && তথ্য->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    ফলাফল = Curl_http_auth_act(তথ্য);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode H1_CONNECT(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো h1_tunnel_state *ts)
{
  কাঠামো connectdata *conn = cf->conn;
  CURLcode ফলাফল;
  bool done;

  যদি(tunnel_is_established(ts))
    ফেরত CURLE_OK;
  যদি(tunnel_is_failed(ts))
    ফেরত CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  করো {

    যদি(Curl_timeleft_ms(তথ্য) < 0) {
      failf(তথ্য, "Proxy CONNECT aborted due to timeout");
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      যাও out;
    }

    নির্বাচন(ts->tunnel_state) {
    ক্ষেত্র H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(তথ্য, cf, "CONNECT start");
      ফলাফল = start_CONNECT(cf, তথ্য, ts);
      যদি(ফলাফল)
        যাও out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, তথ্য);
      FALLTHROUGH();

    ক্ষেত্র H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(তথ্য, cf, "CONNECT send");
      ফলাফল = send_CONNECT(cf, তথ্য, ts, &done);
      যদি(ফলাফল || !done)
        যাও out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, তথ্য);
      FALLTHROUGH();

    ক্ষেত্র H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(তথ্য, cf, "CONNECT receive");
      ফলাফল = recv_CONNECT_resp(cf, তথ্য, ts, &done);
      যদি(ফলাফল)
        CURL_TRC_CF(তথ্য, cf, "error receiving CONNECT response: %d",
                    (পূর্ণসংখ্যা)ফলাফল);
      যদি(!ফলাফল)
        ফলাফল = Curl_pgrsUpdate(তথ্য);
      /* error or not complete yet. return for more multi-multi */
      যদি(ফলাফল || !done)
        যাও out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, তথ্য);
      FALLTHROUGH();

    ক্ষেত্র H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(তথ্য, cf, "CONNECT response");
      যদি(তথ্য->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&তথ্য->req, তথ্য);
        যদি(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(তথ্য, cf, "CONNECT need to close+open");
          infof(তথ্য, "Connect me again please");
          ফেরত CURLE_AGAIN;
        }
        অন্যথা {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, তথ্য);
        }
      }
      বিরতি;

    পূর্বনির্ধারিত:
      বিরতি;
    }

  } যখন(তথ্য->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  যদি(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    যদি(তথ্য->info.httpproxycode / 100 != 2 &&
       তথ্য->info.httpproxycode != 101) {
      curlx_safefree(তথ্য->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, তথ্য);
      failf(তথ্য, "CONNECT-UDP tunnel failed, response %d",
            তথ্য->req.httpcode);
      ফেরত CURLE_COULDNT_CONNECT;
    }
  }
  অন্যথা {
    যদি(তথ্য->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(তথ্য->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, তথ্য);
      failf(তথ্য, "CONNECT tunnel failed, response %d", তথ্য->req.httpcode);
      ফেরত CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, তথ্য);
  infof(তথ্য, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", তথ্য->info.httpproxycode);
  ফলাফল = CURLE_OK;

out:
  যদি(ফলাফল)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, তথ্য);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h1_proxy_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  CURLcode ফলাফল;
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
  কাঠামো h1_tunnel_state *ts = pctx->ts;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  CURL_TRC_CF(তথ্য, cf, "connect");
  ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
  যদি(ফলাফল || !*done)
    ফেরত ফলাফল;

  *done = FALSE;
  যদি(!ts) {
    ফলাফল = tunnel_init(cf, তথ্য, &ts);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  ফলাফল = H1_CONNECT(cf, তথ্য, ts);
  যদি(ফলাফল)
    যাও out;
  curlx_safefree(তথ্য->req.hd_proxy_auth);

out:
  *done = (ফলাফল == CURLE_OK) && tunnel_is_established(pctx->ts);
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&তথ্য->req, তথ্য);
    Curl_client_reset(তথ্য);
    Curl_pgrsReset(তথ্য);
    cf_tunnel_free(cf, তথ্য);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h1_proxy_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো easy_pollset *ps)
{
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
  কাঠামো h1_tunnel_state *ts = pctx->ts;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, তথ্য);
    যদি(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      যদি(tunnel_want_send(ts))
        ফলাফল = Curl_pollset_set_out_only(তথ্য, ps, sock);
      অন্যথা
        ফলাফল = Curl_pollset_set_in_only(তথ্য, ps, sock);
    }
    অন্যথা
      ফলাফল = Curl_pollset_set_out_only(তথ্য, ps, sock);
  }
  অন্যথা {
    যদি(cf->পরবর্তী)
      ফলাফল = cf->পরবর্তী->cft->adjust_pollset(cf->পরবর্তী, তথ্য, ps);
  }
  ফেরত ফলাফল;
}

স্থির bool cf_h1_proxy_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                     ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ফেরত cf->পরবর্তী ? cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

স্থির শূন্য cf_h1_proxy_destroy(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  যদি(pctx) {
    cf_tunnel_free(cf, তথ্য);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

স্থির CURLcode cf_h1_proxy_query(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_h1_proxy_ctx *pctx = cf->ctx;
  কাঠামো h1_tunnel_state *ts = pctx ? pctx->ts : ফাঁকা;
  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_HOST_PORT:
    যদি(!ts || !ts->dest)
      বিরতি;
    *pres1 = (পূর্ণসংখ্যা)ts->dest->port;
    *((ধ্রুবক অক্ষর **)pres2) = ts->dest->hostname;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

কাঠামো Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *peer,
                                       কাঠামো Curl_peer *dest,
                                       পূর্ণসংখ্যা httpversion,
                                       bool udp_tunnel)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো cf_h1_proxy_ctx *pctx;
  কাঠামো h1_tunnel_state *ts;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  যদি(!dest)
    ফেরত CURLE_FAILED_INIT;
  যদি((httpversion < 10) || (httpversion >= 20))
    ফেরত CURLE_FAILED_INIT;

  ts = curlx_calloc(1, আকার(*ts));
  যদি(!ts) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_তথ্য, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(তথ্য, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, আকার(*pctx));
  যদি(!pctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  ফলাফল = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  যদি(ফলাফল) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    যাও out;
  }
  ts = ফাঁকা;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  tunnel_free(ts, তথ্য);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
