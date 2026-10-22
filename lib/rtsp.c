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
#ضمن "urldata.h"
#ضمن "rtsp.h"

#جے_بنا_تعریف CURL_DISABLE_RTSP

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "http.h"
#ضمن "url.h"
#ضمن "progress.h"
#ضمن "strcase.h"
#ضمن "select.h"
#ضمن "connect.h"
#ضمن "cfilters.h"
#ضمن "curlx/strdup.h"
#ضمن "bufref.h"
#ضمن "curlx/strparse.h"
#ضمن "peer.h"

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

تعريفنوع تعداد {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_انکڑے
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
بنية rtsp_conn {
  بنية dynbuf buf;
  صحيح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  ثنائی_ہندسہ(وچ_سرخی);
};

/* RTSP transfer data */
بنية RTSP {
  uint32_t CSeq_گھلیا; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#تعریف_کرو RTP_PKT_لمائی(p) ((((بنانشانی صحيح)((بنانشانی حرف)((p)[2]))) << 8) | \
                            ((بنانشانی صحيح)((بنانشانی حرف)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ساكن CURLcode rtsp_do_pollset(بنية Curl_easy *انکڑے,
                                بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جوڑو_باہرلا(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
}

#تعریف_کرو MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ساكن فراغ rtsp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية RTSP *rtsp = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(rtsp);
}

ساكن فراغ rtsp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية rtsp_conn *rtspc = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ساكن CURLcode rtsp_تیاری_connection(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn)
{
  بنية rtsp_conn *rtspc;
  بنية RTSP *rtsp;

  rtspc = curlx_calloc(1, حجم(*rtspc));
  إذا(!rtspc)
    أرجع CURLE_باہرلا_دا_حافظہ;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  إذا(Curl_conn_meta_سیٹ(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  rtsp = curlx_calloc(1, حجم(بنية RTSP));
  إذا(!rtsp ||
     Curl_meta_سیٹ(انکڑے, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ساكن bool rtsp_conn_is_dead(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn)
{
  bool داخلہ_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  أرجع !Curl_conn_is_alive(انکڑے, conn, &داخلہ_pending);
}

ساكن CURLcode rtsp_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_RTSP_CONN);

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  إذا(انکڑے->state.rtsp_اگلا_client_CSeq == 0)
    انکڑے->state.rtsp_اگلا_client_CSeq = 1;
  إذا(انکڑے->state.rtsp_اگلا_server_CSeq == 0)
    انکڑے->state.rtsp_اگلا_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *پورا = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_پورا(بنية Curl_easy *انکڑے,
                          CURLcode حالت, bool premature)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_RTSP_CONN);
  بنية RTSP *rtsp = Curl_meta_get(انکڑے, CURL_META_RTSP_EASY);
  CURLcode نتیجہ;

  إذا(!rtspc || !rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  إذا(انکڑے->سیٹ.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  نتیجہ = Curl_http_پورا(انکڑے, حالت, premature);

  إذا(!حالت && !نتیجہ) {
    /* Check the sequence numbers */
    uint32_t CSeq_گھلیا = rtsp->CSeq_گھلیا;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    إذا((انکڑے->سیٹ.rtspreq != RTSPREQ_RECEIVE) && (CSeq_گھلیا != CSeq_recv)) {
      failf(انکڑے,
            "The CSeq of this request %u did not match the response %u",
            CSeq_گھلیا, CSeq_recv);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    إذا(انکڑے->سیٹ.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(انکڑے, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    إذا(انکڑے->سیٹ.rtspreq == RTSPREQ_RECEIVE &&
       انکڑے->req.eos_written) {
      failf(انکڑے, "Server prematurely closed the RTSP connection.");
      أرجع CURLE_RECV_ERROR;
    }
  }

  أرجع نتیجہ;
}

ساكن CURLcode rtsp_تیاری_body(بنية Curl_easy *انکڑے,
                                بنانشانی حرف rtspreq,
                                بنية dynbuf *reqp)
{
  CURLcode نتیجہ;
  إذا(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_سیٹ_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    إذا(انکڑے->state.upload) {
      req_clen = انکڑے->state.infilesize;
      انکڑے->state.httpreq = HTTPREQ_PUT;
      نتیجہ = Curl_creader_سیٹ_fread(انکڑے, req_clen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا {
      إذا(انکڑے->سیٹ.postfields) {
        size_t plen = (انکڑے->سیٹ.postfieldsize >= 0) ?
          (size_t)انکڑے->سیٹ.postfieldsize : لڑی_لمبائی(انکڑے->سیٹ.postfields);
        req_clen = (curl_off_t)plen;
        نتیجہ = Curl_creader_سیٹ_buf(انکڑے, انکڑے->سیٹ.postfields, plen);
      }
      وإلا إذا(انکڑے->state.infilesize >= 0) {
        req_clen = انکڑے->state.infilesize;
        نتیجہ = Curl_creader_سیٹ_fread(انکڑے, req_clen);
      }
      وإلا {
        req_clen = 0;
        نتیجہ = Curl_creader_سیٹ_null(انکڑے);
      }
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    إذا(req_clen > 0) {
      إذا(انکڑے->سیٹ.crlf) {
        failf(انکڑے, "Cannot do CRLF conversions on upload");
        أرجع CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      إذا(!Curl_checkheaders(انکڑے, STRCONST("Content-Length"))) {
        نتیجہ = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }

      إذا(rtspreq == RTSPREQ_سیٹ_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        إذا(!Curl_checkheaders(انکڑے, STRCONST("Content-Type"))) {
          نتیجہ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          إذا(نتیجہ)
            أرجع نتیجہ;
        }
      }

      إذا(rtspreq == RTSPREQ_ANNOUNCE) {
        إذا(!Curl_checkheaders(انکڑے, STRCONST("Content-Type"))) {
          نتیجہ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          إذا(نتیجہ)
            أرجع نتیجہ;
        }
      }
    }
    وإلا إذا(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      انکڑے->state.httpreq = HTTPREQ_سر;
      انکڑے->req.no_body = TRUE;
    }
  }
  وإلا
    نتیجہ = Curl_creader_سیٹ_null(انکڑے);
  أرجع نتیجہ;
}

بنية rtspselect {
  ثابت حرف *طریقہ;
  bool no_body;
};

ساكن CURLcode pick_طریقہ(بنية Curl_easy *انکڑے,
                            ثابت بنانشانی حرف rtspreq,
                            ثابت حرف **p)
{
  ساكن ثابت بنية rtspselect req[] = {
    { "OPTIONS",       TRUE },
    { "DESCRIBE",      FALSE },
    { "ANNOUNCE",      TRUE },
    { "SETUP",         TRUE },
    { "PLAY",          TRUE },
    { "PAUSE",         TRUE },
    { "TEARDOWN",      TRUE },
    { "GET_PARAMETER", FALSE },
    { "SET_PARAMETER", TRUE },
    { "RECORD",        TRUE },
    { "", FALSE }, /* RECEIVE: treat interleaved RTP as body */
  };
  /* this is verified already in setopt, this is just added precaution */
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_آخری));
  إذا((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_آخری))
    أرجع CURLE_BAD_FUNCTION_دلیل;
  *p = req[rtspreq - 1].طریقہ;
  انکڑے->req.no_body = req[rtspreq - 1].no_body;
  أرجع CURLE_OK;
}

/* Allocate and store a header string. */
ساكن CURLcode rtsp_سرخی_alloc(ثابت حرف *سرخی_name,
                                  ثابت حرف *قدر,
                                  حرف **نشانہ)
{
  إذا(!قدر)
    أرجع CURLE_OK;
  curlx_free(*نشانہ);
  *نشانہ = curl_maprintf("%s: %s\r\n", سرخی_name, قدر);
  إذا(!*نشانہ)
    أرجع CURLE_باہرلا_دا_حافظہ;
  أرجع CURLE_OK;
}

بنية rtsp_بلاکاں {
  ثابت حرف *request;
  ثابت حرف *session_id;
  ثابت حرف *accept;
  ثابت حرف *range;
  ثابت حرف *دھارا_uri;
  ثابت حرف *hd_proxy_auth;
  ثابت حرف *hd_auth;
  حرف *referrer;
  حرف *accept_encoding;
  حرف *transport;
  ثنائی_ہندسہ(transport_alloc); /* if 'transport' is allocated */
};

ساكن CURLcode rtsp_تیاری_request(بنية Curl_easy *انکڑے,
                                   بنية rtsp_بلاکاں *b,
                                   ثابت بنانشانی حرف rtspreq)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  b->session_id = CURL_EASY_STR(انکڑے, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  إذا(CURL_EASY_STR(انکڑے, STRING_RTSP_دھارا_URI))
    b->دھارا_uri = CURL_EASY_STR(انکڑے, STRING_RTSP_دھارا_URI);
  وإلا
    b->دھارا_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(انکڑے, STRCONST("Transport"));
  إذا(rtspreq == RTSPREQ_تیاری && !b->transport) {
    /* New Transport: setting? */
    إذا(CURL_EASY_STR(انکڑے, STRING_RTSP_TRANSPORT)) {
      نتیجہ = rtsp_سرخی_alloc(
        "Transport", CURL_EASY_STR(انکڑے, STRING_RTSP_TRANSPORT),
        &b->transport);
      إذا(نتیجہ)
        أرجع نتیجہ;
      b->transport_alloc = TRUE;
    }
    وإلا {
      failf(انکڑے,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  إذا(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(انکڑے, STRCONST("Accept")) ?
      عدم : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    إذا(!Curl_checkheaders(انکڑے, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(انکڑے, STRING_ENCODING)) {
      نتیجہ = rtsp_سرخی_alloc("Accept-Encoding",
                                 CURL_EASY_STR(انکڑے, STRING_ENCODING),
                                 &b->accept_encoding);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }

  /* setup the authentication headers */
  نتیجہ = Curl_http_output_auth(انکڑے, conn, b->request, HTTPREQ_GET,
                                 b->دھارا_uri, عدم, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  b->hd_proxy_auth = انکڑے->req.hd_proxy_auth;
#جے_انت
  b->hd_auth = انکڑے->req.hd_auth;

  /* Referrer */
  إذا(Curl_bufref_ptr(&انکڑے->state.referer) &&
     !Curl_checkheaders(انکڑے, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&انکڑے->state.referer));
    إذا(!b->referrer)
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  إذا(!نتیجہ &&
     انکڑے->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_ریکارڈ)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(انکڑے, STRCONST("Range")) && انکڑے->state.range) {
    نتیجہ = rtsp_سرخی_alloc("Range",
                               انکڑے->state.range,
                               &انکڑے->state.rangeline);
    إذا(!نتیجہ)
      b->range = انکڑے->state.rangeline;
  }
  أرجع نتیجہ;
}

#تعریف_کرو HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ساكن CURLcode rtsp_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنانشانی حرف rtspreq = انکڑے->سیٹ.rtspreq;
  ثابت حرف *str;
  بنية RTSP *rtsp = Curl_meta_get(انکڑے, CURL_META_RTSP_EASY);
  بنية dynbuf req_بفر;
  بنية rtsp_بلاکاں بلاک;
  memset(&بلاک, 0, حجم(بلاک));

  *پورا = TRUE;
  إذا(!rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_بفر, DYN_RTSP_REQ_سرخی);

  rtsp->CSeq_گھلیا = انکڑے->state.rtsp_اگلا_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  نتیجہ = pick_طریقہ(انکڑے, rtspreq, &بلاک.request);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
    اذهب باہرلا;
  }

  نتیجہ = rtsp_تیاری_request(انکڑے, &بلاک,  rtspreq);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(بلاک.session_id && انکڑے->state.rtsp_session_origin &&
     !انکڑے->سیٹ.allow_auth_منزل_other_hosts &&
     !Curl_peer_equal(انکڑے->state.origin, انکڑے->state.rtsp_session_origin))
    بلاک.session_id = عدم;

  /*
   * Sanity check the custom headers
   */
  إذا(Curl_checkheaders(انکڑے, STRCONST("CSeq"))) {
    failf(انکڑے, "CSeq cannot be set as a custom header.");
    نتیجہ = CURLE_RTSP_CSEQ_ERROR;
    اذهب باہرلا;
  }
  إذا(Curl_checkheaders(انکڑے, STRCONST("Session"))) {
    failf(انکڑے, "Session ID cannot be set as a custom header.");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }

  نتیجہ =
    curlx_dyn_addf(&req_بفر,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   بلاک.request, بلاک.دھارا_uri, rtsp->CSeq_گھلیا);
  إذا(نتیجہ)
    اذهب باہرلا;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  إذا(بلاک.session_id) {
    نتیجہ = curlx_dyn_addf(&req_بفر, "Session: %s\r\n", بلاک.session_id);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /*
   * Shared HTTP-like options
   */
  نتیجہ = curlx_dyn_addf(&req_بفر,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          بلاک.transport ? بلاک.transport : "",
                          بلاک.accept ? بلاک.accept : "",
                          بلاک.accept_encoding ? بلاک.accept_encoding : "",
                          بلاک.range ? بلاک.range : "",
                          بلاک.referrer ? بلاک.referrer : "");

  str = CURL_EASY_STR(انکڑے, STRING_USERAGENT);
  إذا(!نتیجہ && str && *str &&
     !Curl_checkheaders(انکڑے, STRCONST("User-Agent")))
    نتیجہ = curlx_dyn_addf(&req_بفر,
                            "User-Agent: %s\r\n", str);

  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addf(&req_بفر,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            بلاک.hd_proxy_auth ? بلاک.hd_proxy_auth : "",
                            بلاک.hd_auth ? بلاک.hd_auth : "");
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا((rtspreq == RTSPREQ_تیاری) || (rtspreq == RTSPREQ_DESCRIBE)) {
    نتیجہ = Curl_جوڑو_timecondition(انکڑے, &req_بفر);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  نتیجہ = Curl_جوڑو_custom_سرخیاں(انکڑے, FALSE, HTTPVERSION, &req_بفر);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = rtsp_تیاری_body(انکڑے, rtspreq, &req_بفر);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* Finish the request buffer */
  نتیجہ = curlx_dyn_addn(&req_بفر, STRCONST("\r\n"));
  إذا(نتیجہ)
    اذهب باہرلا;

  Curl_xfer_تیاری_sendrecv(انکڑے, FIRSTSOCKET, -1);

  /* issue the request */
  نتیجہ = Curl_req_send(انکڑے, &req_بفر, HTTPVERSION);
  إذا(نتیجہ) {
    failf(انکڑے, "Failed sending RTSP request");
    اذهب باہرلا;
  }

  /* Increment the CSeq on success */
  انکڑے->state.rtsp_اگلا_client_CSeq++;

  إذا(انکڑے->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_سیٹ_Upload_Counter(انکڑے, انکڑے->req.writebytecount);
    نتیجہ = Curl_pgrsUpdate(انکڑے);
  }
باہرلا:
  إذا(بلاک.transport_alloc)
    curlx_free(بلاک.transport);
  curlx_free(بلاک.accept_encoding);
  curlx_free(بلاک.referrer);
  curlx_dyn_free(&req_بفر);
  أرجع نتیجہ;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ساكن CURLcode rtp_write_body_junk(بنية Curl_easy *انکڑے,
                                    بنية rtsp_conn *rtspc,
                                    ثابت حرف *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool وچ_body;

  وچ_body = (انکڑے->req.headerline && !rtspc->وچ_سرخی) &&
            (انکڑے->req.ناپ >= 0) &&
            (انکڑے->req.bytecount < انکڑے->req.ناپ);
  body_remain = وچ_body ? (انکڑے->req.ناپ - انکڑے->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  إذا(body_remain) {
    إذا((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    أرجع Curl_client_write(انکڑے, CLIENTWRITE_BODY, buf, blen);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode rtp_client_write(بنية Curl_easy *انکڑے, ثابت حرف *ptr,
                                 size_t len)
{
  بنية Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  فراغ *user_ptr;

  إذا(len == 0) {
    failf(انکڑے, "Cannot write a 0 size RTP packet.");
    أرجع CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  إذا(انکڑے->سیٹ.fwrite_rtp) {
    writeit = انکڑے->سیٹ.fwrite_rtp;
    user_ptr = انکڑے->سیٹ.rtp_باہرلا;
  }
  وإلا {
    writeit = انکڑے->سیٹ.fwrite_func;
    user_ptr = انکڑے->سیٹ.باہرلا;
  }

  CURL_CBAPI_شروع(&guard, انکڑے, easy_fwrite_rtp);
  wrote = writeit((حرف *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_سرا(&guard);

  إذا(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(انکڑے, "Cannot pause RTP");
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(wrote != len) {
    failf(انکڑے, "Failed writing RTP data");
    أرجع CURLE_WRITE_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_filter_rtp(بنية Curl_easy *انکڑے,
                                بنية rtsp_conn *rtspc,
                                ثابت حرف *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  طالما(blen) {
    bool وچ_body = (انکڑے->req.headerline && !rtspc->وچ_سرخی) &&
                   (انکڑے->req.ناپ >= 0) &&
                   (انکڑے->req.bytecount < انکڑے->req.ناپ);
    بدل(rtspc->state) {

    حالة RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      طالما(blen && buf[0] != '$') {
        إذا(!وچ_body && buf[0] == 'R' &&
           انکڑے->سیٹ.rtspreq != RTSPREQ_RECEIVE) {
          إذا(!لڑی_مقابلہ(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            إذا(*pconsumed) {
              DEBUGF(infof(انکڑے, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->وچ_سرخی = TRUE;
            اذهب باہرلا;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      إذا(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        إذا(skip_len) {
          /* end of junk/BODY bytes, flush */
          نتیجہ = rtp_write_body_junk(انکڑے, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          إذا(نتیجہ)
            اذهب باہرلا;
        }
        إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
          اذهب باہرلا;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      اكسر;
    }

    حالة RTP_PARSE_CHANNEL: {
      صحيح idx = ((بنانشانی حرف)buf[0]) / 8;
      صحيح off = ((بنانشانی حرف)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      إذا(!(انکڑے->state.rtp_channel_نقاب[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(انکڑے, "RTSP: invalid RTP channel %d, skipping", idx));
        إذا(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          نتیجہ = rtp_write_body_junk(انکڑے, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          إذا(نتیجہ)
            اذهب باہرلا;
        }
        وإلا {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        اكسر;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (بنانشانی حرف)buf[0];
      إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      اكسر;
    }

    حالة RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ثابت حرف *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      إذا(rtp_len == 2)
        اكسر;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_لمائی(rtp_buf) + 4;
      إذا(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(انکڑے, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجہ = rtp_client_write(انکڑے, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(نتیجہ)
          اذهب باہرلا;
        اكسر;
      }
      rtspc->state = RTP_PARSE_انکڑے;
      اكسر;
    }

    حالة RTP_PARSE_انکڑے: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      إذا(needed <= blen) {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
          اذهب باہرلا;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(انکڑے, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجہ = rtp_client_write(انکڑے, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(نتیجہ)
          اذهب باہرلا;
      }
      وإلا {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
          اذهب باہرلا;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      اكسر;
    }

    افتراضي:
      DEBUGASSERT(0);
      أرجع CURLE_RECV_ERROR;
    }
  }
باہرلا:
  إذا(!نتیجہ && skip_len)
    نتیجہ = rtp_write_body_junk(انکڑے, rtspc, buf - skip_len, skip_len);
  أرجع نتیجہ;
}

/*
 * Parse and write out an RTSP response.
 * @param data     the transfer
 * @param conn     the connection
 * @param buf      data read from connection
 * @param blen     amount of data in buf
 * @param is_eos   TRUE iff this is the last write
 * @param readmore out, TRUE iff complete buf was consumed and more data
 *                 is needed
 */
ساكن CURLcode rtsp_rtp_write_resp(بنية Curl_easy *انکڑے,
                                    ثابت حرف *buf,
                                    size_t blen,
                                    bool is_eos)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_RTSP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  size_t consumed = 0;

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  إذا(!انکڑے->req.سرخی)
    rtspc->وچ_سرخی = FALSE;
  إذا(!blen) {
    اذهب باہرلا;
  }

  DEBUGF(infof(انکڑے, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->وچ_سرخی, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  إذا(!rtspc->وچ_سرخی) {
    نتیجہ = rtsp_filter_rtp(انکڑے, rtspc, buf, blen, &consumed);
    إذا(نتیجہ)
      اذهب باہرلا;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    إذا(blen && !انکڑے->req.سرخی)
      DEBUGF(infof(انکڑے, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  إذا(انکڑے->req.سرخی && blen) {
    rtspc->وچ_سرخی = TRUE;
    نتیجہ = Curl_http_write_resp_hds(انکڑے, buf, blen, &consumed);
    إذا(نتیجہ)
      اذهب باہرلا;

    buf += consumed;
    blen -= consumed;

    إذا(!انکڑے->req.سرخی)
      rtspc->وچ_سرخی = FALSE;

    إذا(!rtspc->وچ_سرخی) {
      /* If header parsing is done, extract interleaved RTP messages */
      إذا(انکڑے->req.ناپ <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        انکڑے->req.ناپ = 0;
        انکڑے->req.download_پورا = TRUE;
      }
      نتیجہ = rtsp_filter_rtp(انکڑے, rtspc, buf, blen, &consumed);
      إذا(نتیجہ)
        اذهب باہرلا;
      buf += consumed;
      blen -= consumed;
    }
  }

  إذا(rtspc->state != RTP_PARSE_SKIP)
    انکڑے->req.پورا = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(انکڑے, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->وچ_سرخی, انکڑے->req.پورا, (صحيح)rtspc->state,
               انکڑے->req.ناپ));
  إذا(!نتیجہ && (is_eos || blen)) {
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

باہرلا:
  إذا((انکڑے->سیٹ.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    انکڑے->req.download_پورا = TRUE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode rtsp_rtp_write_resp_hd(بنية Curl_easy *انکڑے,
                                       ثابت حرف *buf,
                                       size_t blen,
                                       bool is_eos)
{
  أرجع rtsp_rtp_write_resp(انکڑے, buf, blen, is_eos);
}

ساكن CURLcode rtsp_parse_transport(بنية Curl_easy *انکڑے,
                                     ثابت حرف *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت حرف *شروع, *سرا;
  شروع = transport;
  طالما(شروع && *شروع) {
    curlx_str_passblanks(&شروع);
    سرا = strchr(شروع, ';');
    إذا(checkprefix("interleaved=", شروع)) {
      curl_off_t chan1, chan2, chan;
      ثابت حرف *p = شروع + 12;
      إذا(!curlx_str_نمبر(&p, &chan1, 255)) {
        بنانشانی حرف *rtp_channel_نقاب = انکڑے->state.rtp_channel_نقاب;
        chan2 = chan1;
        إذا(!curlx_str_single(&p, '-') && curlx_str_نمبر(&p, &chan2, 255)) {
          infof(انکڑے, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        لكل(chan = chan1; chan <= chan2; chan++) {
          صحيح idx = (صحيح)chan / 8;
          صحيح off = (صحيح)chan % 8;
          rtp_channel_نقاب[idx] |= (بنانشانی حرف)(1 << off);
        }
      }
      وإلا {
        infof(انکڑے, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      اكسر;
    }
    /* skip to next parameter */
    شروع = (!سرا) ? سرا : (سرا + 1);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(بنية Curl_easy *انکڑے, ثابت حرف *سرخی)
{
  إذا(checkprefix("CSeq:", سرخی)) {
    curl_off_t CSeq = 0;
    بنية RTSP *rtsp = Curl_meta_get(انکڑے, CURL_META_RTSP_EASY);
    ثابت حرف *p = &سرخی[5];
    إذا(!rtsp)
      أرجع CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    إذا(curlx_str_نمبر(&p, &CSeq, UINT_MAX)) {
      failf(انکڑے, "Unable to read the CSeq header: [%s]", سرخی);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    انکڑے->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  وإلا إذا(checkprefix("Session:", سرخی)) {
    ثابت حرف *شروع, *سرا, *str;
    size_t idlen;

    /* Find the first non-space letter */
    شروع = سرخی + 8;
    curlx_str_passblanks(&شروع);

    إذا(!*شروع) {
      failf(انکڑے, "Got a blank Session ID");
      أرجع CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    سرا = شروع;
    طالما((*سرا > ' ') && (*سرا != ';'))
      سرا++;
    idlen = سرا - شروع;

    str = CURL_EASY_STR(انکڑے, STRING_RTSP_SESSION_ID);
    إذا(str) {

      /* If the Session ID is set, then compare */
      إذا(لڑی_لمبائی(str) != idlen ||
         لڑی_مقابلہ(شروع, str, idlen)) {
        failf(انکڑے, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              شروع, str);
        أرجع CURLE_RTSP_SESSION_ERROR;
      }
    }
    وإلا {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      فراغ *mem = curlx_memdup0(شروع, idlen);
      إذا(!mem ||
         CURL_EASY_STR_SETN(انکڑے, STRING_RTSP_SESSION_ID, mem))
        أرجع CURLE_باہرلا_دا_حافظہ;
      Curl_peer_link(&انکڑے->state.rtsp_session_origin, انکڑے->state.origin);
    }
  }
  وإلا إذا(checkprefix("Transport:", سرخی)) {
    CURLcode نتیجہ;
    نتیجہ = rtsp_parse_transport(انکڑے, سرخی + 10);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_rtsp = {
  rtsp_تیاری_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_پورا,                            /* done */
  ZERO_NULL,                            /* do_more */
  rtsp_connect,                         /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  rtsp_do_pollset,                      /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_http_perform_pollset,            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  rtsp_rtp_write_resp,                  /* write_resp */
  rtsp_rtp_write_resp_hd,               /* write_resp_hd */
  rtsp_conn_is_dead,                    /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  Curl_http_follow,                     /* follow */
};

#جے_انت /* CURL_DISABLE_RTSP */
