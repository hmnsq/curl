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

#اگر_تعریف_نشده CURL_DISABLE_RTSP

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
#تعریف_کن CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

تعريفنوع تعداد {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_داده
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
بنية rtsp_conn {
  بنية dynbuf buf;
  صحيح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  بیت(درون_سرآیند);
};

/* RTSP transfer data */
بنية RTSP {
  uint32_t CSeq_فرستاده; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#تعریف_کن RTP_PKT_درازی(p) ((((بیعلامت صحيح)((بیعلامت حرف)((p)[2]))) << 8) | \
                            ((بیعلامت صحيح)((بیعلامت حرف)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ساكن CURLcode rtsp_do_pollset(بنية Curl_easy *داده,
                                بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جمع_کن_بیرون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
}

#تعریف_کن MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ساكن فراغ rtsp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية RTSP *rtsp = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(rtsp);
}

ساكن فراغ rtsp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية rtsp_conn *rtspc = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ساكن CURLcode rtsp_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية rtsp_conn *rtspc;
  بنية RTSP *rtsp;

  rtspc = curlx_calloc(1, حجم(*rtspc));
  إذا(!rtspc)
    أرجع CURLE_بیرون_از_حافظه;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  إذا(Curl_conn_meta_مجموعه(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  rtsp = curlx_calloc(1, حجم(بنية RTSP));
  إذا(!rtsp ||
     Curl_meta_مجموعه(داده, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ساكن bool rtsp_conn_is_dead(بنية Curl_easy *داده,
                              بنية connectdata *conn)
{
  bool درون_داد_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  أرجع !Curl_conn_is_alive(داده, conn, &درون_داد_pending);
}

ساكن CURLcode rtsp_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(داده->conn, CURL_META_RTSP_CONN);

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  إذا(داده->state.rtsp_بعدی_client_CSeq == 0)
    داده->state.rtsp_بعدی_client_CSeq = 1;
  إذا(داده->state.rtsp_بعدی_server_CSeq == 0)
    داده->state.rtsp_بعدی_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *انجامشده = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_انجامشده(بنية Curl_easy *داده,
                          CURLcode وضعیت, bool premature)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(داده->conn, CURL_META_RTSP_CONN);
  بنية RTSP *rtsp = Curl_meta_get(داده, CURL_META_RTSP_EASY);
  CURLcode نتیجه;

  إذا(!rtspc || !rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  إذا(داده->مجموعه.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  نتیجه = Curl_http_انجامشده(داده, وضعیت, premature);

  إذا(!وضعیت && !نتیجه) {
    /* Check the sequence numbers */
    uint32_t CSeq_فرستاده = rtsp->CSeq_فرستاده;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    إذا((داده->مجموعه.rtspreq != RTSPREQ_RECEIVE) && (CSeq_فرستاده != CSeq_recv)) {
      failf(داده,
            "The CSeq of this request %u did not match the response %u",
            CSeq_فرستاده, CSeq_recv);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    إذا(داده->مجموعه.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(داده, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    إذا(داده->مجموعه.rtspreq == RTSPREQ_RECEIVE &&
       داده->req.eos_written) {
      failf(داده, "Server prematurely closed the RTSP connection.");
      أرجع CURLE_RECV_ERROR;
    }
  }

  أرجع نتیجه;
}

ساكن CURLcode rtsp_راهاندازی_body(بنية Curl_easy *داده,
                                بیعلامت حرف rtspreq,
                                بنية dynbuf *reqp)
{
  CURLcode نتیجه;
  إذا(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_مجموعه_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    إذا(داده->state.upload) {
      req_clen = داده->state.infilesize;
      داده->state.httpreq = HTTPREQ_PUT;
      نتیجه = Curl_creader_مجموعه_fread(داده, req_clen);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا {
      إذا(داده->مجموعه.postfields) {
        size_t plen = (داده->مجموعه.postfieldsize >= 0) ?
          (size_t)داده->مجموعه.postfieldsize : طول_رشته(داده->مجموعه.postfields);
        req_clen = (curl_off_t)plen;
        نتیجه = Curl_creader_مجموعه_buf(داده, داده->مجموعه.postfields, plen);
      }
      وإلا إذا(داده->state.infilesize >= 0) {
        req_clen = داده->state.infilesize;
        نتیجه = Curl_creader_مجموعه_fread(داده, req_clen);
      }
      وإلا {
        req_clen = 0;
        نتیجه = Curl_creader_مجموعه_null(داده);
      }
      إذا(نتیجه)
        أرجع نتیجه;
    }

    إذا(req_clen > 0) {
      إذا(داده->مجموعه.crlf) {
        failf(داده, "Cannot do CRLF conversions on upload");
        أرجع CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      إذا(!Curl_checkheaders(داده, STRCONST("Content-Length"))) {
        نتیجه = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        إذا(نتیجه)
          أرجع نتیجه;
      }

      إذا(rtspreq == RTSPREQ_مجموعه_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        إذا(!Curl_checkheaders(داده, STRCONST("Content-Type"))) {
          نتیجه = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          إذا(نتیجه)
            أرجع نتیجه;
        }
      }

      إذا(rtspreq == RTSPREQ_ANNOUNCE) {
        إذا(!Curl_checkheaders(داده, STRCONST("Content-Type"))) {
          نتیجه = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          إذا(نتیجه)
            أرجع نتیجه;
        }
      }
    }
    وإلا إذا(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      داده->state.httpreq = HTTPREQ_سر;
      داده->req.no_body = TRUE;
    }
  }
  وإلا
    نتیجه = Curl_creader_مجموعه_null(داده);
  أرجع نتیجه;
}

بنية rtspselect {
  ثابت حرف *روش;
  bool no_body;
};

ساكن CURLcode pick_روش(بنية Curl_easy *داده,
                            ثابت بیعلامت حرف rtspreq,
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_آخرین));
  إذا((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_آخرین))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  *p = req[rtspreq - 1].روش;
  داده->req.no_body = req[rtspreq - 1].no_body;
  أرجع CURLE_OK;
}

/* Allocate and store a header string. */
ساكن CURLcode rtsp_سرآیند_alloc(ثابت حرف *سرآیند_name,
                                  ثابت حرف *مقدار,
                                  حرف **هدف)
{
  إذا(!مقدار)
    أرجع CURLE_OK;
  curlx_free(*هدف);
  *هدف = curl_maprintf("%s: %s\r\n", سرآیند_name, مقدار);
  إذا(!*هدف)
    أرجع CURLE_بیرون_از_حافظه;
  أرجع CURLE_OK;
}

بنية rtsp_بلوکها {
  ثابت حرف *request;
  ثابت حرف *session_id;
  ثابت حرف *accept;
  ثابت حرف *range;
  ثابت حرف *جریان_uri;
  ثابت حرف *hd_proxy_auth;
  ثابت حرف *hd_auth;
  حرف *referrer;
  حرف *accept_encoding;
  حرف *transport;
  بیت(transport_alloc); /* if 'transport' is allocated */
};

ساكن CURLcode rtsp_راهاندازی_request(بنية Curl_easy *داده,
                                   بنية rtsp_بلوکها *b,
                                   ثابت بیعلامت حرف rtspreq)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  b->session_id = CURL_EASY_STR(داده, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  إذا(CURL_EASY_STR(داده, STRING_RTSP_جریان_URI))
    b->جریان_uri = CURL_EASY_STR(داده, STRING_RTSP_جریان_URI);
  وإلا
    b->جریان_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(داده, STRCONST("Transport"));
  إذا(rtspreq == RTSPREQ_راهاندازی && !b->transport) {
    /* New Transport: setting? */
    إذا(CURL_EASY_STR(داده, STRING_RTSP_TRANSPORT)) {
      نتیجه = rtsp_سرآیند_alloc(
        "Transport", CURL_EASY_STR(داده, STRING_RTSP_TRANSPORT),
        &b->transport);
      إذا(نتیجه)
        أرجع نتیجه;
      b->transport_alloc = TRUE;
    }
    وإلا {
      failf(داده,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  إذا(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(داده, STRCONST("Accept")) ?
      عدم : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    إذا(!Curl_checkheaders(داده, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(داده, STRING_ENCODING)) {
      نتیجه = rtsp_سرآیند_alloc("Accept-Encoding",
                                 CURL_EASY_STR(داده, STRING_ENCODING),
                                 &b->accept_encoding);
      إذا(نتیجه)
        أرجع نتیجه;
    }
  }

  /* setup the authentication headers */
  نتیجه = Curl_http_output_auth(داده, conn, b->request, HTTPREQ_GET,
                                 b->جریان_uri, عدم, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  b->hd_proxy_auth = داده->req.hd_proxy_auth;
#پایان_اگر
  b->hd_auth = داده->req.hd_auth;

  /* Referrer */
  إذا(Curl_bufref_ptr(&داده->state.referer) &&
     !Curl_checkheaders(داده, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&داده->state.referer));
    إذا(!b->referrer)
      نتیجه = CURLE_بیرون_از_حافظه;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  إذا(!نتیجه &&
     داده->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_رکورد)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(داده, STRCONST("Range")) && داده->state.range) {
    نتیجه = rtsp_سرآیند_alloc("Range",
                               داده->state.range,
                               &داده->state.rangeline);
    إذا(!نتیجه)
      b->range = داده->state.rangeline;
  }
  أرجع نتیجه;
}

#تعریف_کن HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ساكن CURLcode rtsp_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت بیعلامت حرف rtspreq = داده->مجموعه.rtspreq;
  ثابت حرف *str;
  بنية RTSP *rtsp = Curl_meta_get(داده, CURL_META_RTSP_EASY);
  بنية dynbuf req_میانگیر;
  بنية rtsp_بلوکها بلوک;
  memset(&بلوک, 0, حجم(بلوک));

  *انجامشده = TRUE;
  إذا(!rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_میانگیر, DYN_RTSP_REQ_سرآیند);

  rtsp->CSeq_فرستاده = داده->state.rtsp_بعدی_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  نتیجه = pick_روش(داده, rtspreq, &بلوک.request);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);
    اذهب بیرون;
  }

  نتیجه = rtsp_راهاندازی_request(داده, &بلوک,  rtspreq);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(بلوک.session_id && داده->state.rtsp_session_origin &&
     !داده->مجموعه.allow_auth_مقصد_other_hosts &&
     !Curl_peer_equal(داده->state.origin, داده->state.rtsp_session_origin))
    بلوک.session_id = عدم;

  /*
   * Sanity check the custom headers
   */
  إذا(Curl_checkheaders(داده, STRCONST("CSeq"))) {
    failf(داده, "CSeq cannot be set as a custom header.");
    نتیجه = CURLE_RTSP_CSEQ_ERROR;
    اذهب بیرون;
  }
  إذا(Curl_checkheaders(داده, STRCONST("Session"))) {
    failf(داده, "Session ID cannot be set as a custom header.");
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }

  نتیجه =
    curlx_dyn_addf(&req_میانگیر,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   بلوک.request, بلوک.جریان_uri, rtsp->CSeq_فرستاده);
  إذا(نتیجه)
    اذهب بیرون;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  إذا(بلوک.session_id) {
    نتیجه = curlx_dyn_addf(&req_میانگیر, "Session: %s\r\n", بلوک.session_id);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /*
   * Shared HTTP-like options
   */
  نتیجه = curlx_dyn_addf(&req_میانگیر,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          بلوک.transport ? بلوک.transport : "",
                          بلوک.accept ? بلوک.accept : "",
                          بلوک.accept_encoding ? بلوک.accept_encoding : "",
                          بلوک.range ? بلوک.range : "",
                          بلوک.referrer ? بلوک.referrer : "");

  str = CURL_EASY_STR(داده, STRING_USERAGENT);
  إذا(!نتیجه && str && *str &&
     !Curl_checkheaders(داده, STRCONST("User-Agent")))
    نتیجه = curlx_dyn_addf(&req_میانگیر,
                            "User-Agent: %s\r\n", str);

  إذا(!نتیجه)
    نتیجه = curlx_dyn_addf(&req_میانگیر,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            بلوک.hd_proxy_auth ? بلوک.hd_proxy_auth : "",
                            بلوک.hd_auth ? بلوک.hd_auth : "");
  إذا(نتیجه)
    اذهب بیرون;

  إذا((rtspreq == RTSPREQ_راهاندازی) || (rtspreq == RTSPREQ_DESCRIBE)) {
    نتیجه = Curl_جمع_کن_timecondition(داده, &req_میانگیر);
    إذا(نتیجه)
      اذهب بیرون;
  }

  نتیجه = Curl_جمع_کن_custom_سرآیندها(داده, FALSE, HTTPVERSION, &req_میانگیر);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = rtsp_راهاندازی_body(داده, rtspreq, &req_میانگیر);
  إذا(نتیجه)
    اذهب بیرون;

  /* Finish the request buffer */
  نتیجه = curlx_dyn_addn(&req_میانگیر, STRCONST("\r\n"));
  إذا(نتیجه)
    اذهب بیرون;

  Curl_xfer_راهاندازی_sendrecv(داده, FIRSTSOCKET, -1);

  /* issue the request */
  نتیجه = Curl_req_send(داده, &req_میانگیر, HTTPVERSION);
  إذا(نتیجه) {
    failf(داده, "Failed sending RTSP request");
    اذهب بیرون;
  }

  /* Increment the CSeq on success */
  داده->state.rtsp_بعدی_client_CSeq++;

  إذا(داده->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_مجموعه_Upload_Counter(داده, داده->req.writebytecount);
    نتیجه = Curl_pgrsUpdate(داده);
  }
بیرون:
  إذا(بلوک.transport_alloc)
    curlx_free(بلوک.transport);
  curlx_free(بلوک.accept_encoding);
  curlx_free(بلوک.referrer);
  curlx_dyn_free(&req_میانگیر);
  أرجع نتیجه;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ساكن CURLcode rtp_write_body_junk(بنية Curl_easy *داده,
                                    بنية rtsp_conn *rtspc,
                                    ثابت حرف *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool درون_body;

  درون_body = (داده->req.headerline && !rtspc->درون_سرآیند) &&
            (داده->req.اندازه >= 0) &&
            (داده->req.bytecount < داده->req.اندازه);
  body_remain = درون_body ? (داده->req.اندازه - داده->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  إذا(body_remain) {
    إذا((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    أرجع Curl_client_write(داده, CLIENTWRITE_BODY, buf, blen);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode rtp_client_write(بنية Curl_easy *داده, ثابت حرف *ptr,
                                 size_t len)
{
  بنية Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  فراغ *user_ptr;

  إذا(len == 0) {
    failf(داده, "Cannot write a 0 size RTP packet.");
    أرجع CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  إذا(داده->مجموعه.fwrite_rtp) {
    writeit = داده->مجموعه.fwrite_rtp;
    user_ptr = داده->مجموعه.rtp_بیرون;
  }
  وإلا {
    writeit = داده->مجموعه.fwrite_func;
    user_ptr = داده->مجموعه.بیرون;
  }

  CURL_CBAPI_شروع(&guard, داده, easy_fwrite_rtp);
  wrote = writeit((حرف *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_انتها(&guard);

  إذا(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(داده, "Cannot pause RTP");
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(wrote != len) {
    failf(داده, "Failed writing RTP data");
    أرجع CURLE_WRITE_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_filter_rtp(بنية Curl_easy *داده,
                                بنية rtsp_conn *rtspc,
                                ثابت حرف *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode نتیجه = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  طالما(blen) {
    bool درون_body = (داده->req.headerline && !rtspc->درون_سرآیند) &&
                   (داده->req.اندازه >= 0) &&
                   (داده->req.bytecount < داده->req.اندازه);
    بدل(rtspc->state) {

    حالة RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      طالما(blen && buf[0] != '$') {
        إذا(!درون_body && buf[0] == 'R' &&
           داده->مجموعه.rtspreq != RTSPREQ_RECEIVE) {
          إذا(!مقایسه_رشته(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            إذا(*pconsumed) {
              DEBUGF(infof(داده, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->درون_سرآیند = TRUE;
            اذهب بیرون;
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
          نتیجه = rtp_write_body_junk(داده, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          إذا(نتیجه)
            اذهب بیرون;
        }
        إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          نتیجه = CURLE_بیرون_از_حافظه;
          اذهب بیرون;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      اكسر;
    }

    حالة RTP_PARSE_CHANNEL: {
      صحيح idx = ((بیعلامت حرف)buf[0]) / 8;
      صحيح off = ((بیعلامت حرف)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      إذا(!(داده->state.rtp_channel_ماسک[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(داده, "RTSP: invalid RTP channel %d, skipping", idx));
        إذا(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          نتیجه = rtp_write_body_junk(داده, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          إذا(نتیجه)
            اذهب بیرون;
        }
        وإلا {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        اكسر;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (بیعلامت حرف)buf[0];
      إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
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
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      إذا(rtp_len == 2)
        اكسر;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_درازی(rtp_buf) + 4;
      إذا(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(داده, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجه = rtp_client_write(داده, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(نتیجه)
          اذهب بیرون;
        اكسر;
      }
      rtspc->state = RTP_PARSE_داده;
      اكسر;
    }

    حالة RTP_PARSE_داده: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      إذا(needed <= blen) {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          نتیجه = CURLE_بیرون_از_حافظه;
          اذهب بیرون;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(داده, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجه = rtp_client_write(داده, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(نتیجه)
          اذهب بیرون;
      }
      وإلا {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          نتیجه = CURLE_بیرون_از_حافظه;
          اذهب بیرون;
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
بیرون:
  إذا(!نتیجه && skip_len)
    نتیجه = rtp_write_body_junk(داده, rtspc, buf - skip_len, skip_len);
  أرجع نتیجه;
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
ساكن CURLcode rtsp_rtp_write_resp(بنية Curl_easy *داده,
                                    ثابت حرف *buf,
                                    size_t blen,
                                    bool is_eos)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(داده->conn, CURL_META_RTSP_CONN);
  CURLcode نتیجه = CURLE_OK;
  size_t consumed = 0;

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  إذا(!داده->req.سرآیند)
    rtspc->درون_سرآیند = FALSE;
  إذا(!blen) {
    اذهب بیرون;
  }

  DEBUGF(infof(داده, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->درون_سرآیند, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  إذا(!rtspc->درون_سرآیند) {
    نتیجه = rtsp_filter_rtp(داده, rtspc, buf, blen, &consumed);
    إذا(نتیجه)
      اذهب بیرون;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    إذا(blen && !داده->req.سرآیند)
      DEBUGF(infof(داده, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  إذا(داده->req.سرآیند && blen) {
    rtspc->درون_سرآیند = TRUE;
    نتیجه = Curl_http_write_resp_hds(داده, buf, blen, &consumed);
    إذا(نتیجه)
      اذهب بیرون;

    buf += consumed;
    blen -= consumed;

    إذا(!داده->req.سرآیند)
      rtspc->درون_سرآیند = FALSE;

    إذا(!rtspc->درون_سرآیند) {
      /* If header parsing is done, extract interleaved RTP messages */
      إذا(داده->req.اندازه <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        داده->req.اندازه = 0;
        داده->req.download_انجامشده = TRUE;
      }
      نتیجه = rtsp_filter_rtp(داده, rtspc, buf, blen, &consumed);
      إذا(نتیجه)
        اذهب بیرون;
      buf += consumed;
      blen -= consumed;
    }
  }

  إذا(rtspc->state != RTP_PARSE_SKIP)
    داده->req.انجامشده = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(داده, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->درون_سرآیند, داده->req.انجامشده, (صحيح)rtspc->state,
               داده->req.اندازه));
  إذا(!نتیجه && (is_eos || blen)) {
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

بیرون:
  إذا((داده->مجموعه.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    داده->req.download_انجامشده = TRUE;
  }
  أرجع نتیجه;
}

ساكن CURLcode rtsp_rtp_write_resp_hd(بنية Curl_easy *داده,
                                       ثابت حرف *buf,
                                       size_t blen,
                                       bool is_eos)
{
  أرجع rtsp_rtp_write_resp(داده, buf, blen, is_eos);
}

ساكن CURLcode rtsp_parse_transport(بنية Curl_easy *داده,
                                     ثابت حرف *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت حرف *شروع, *انتها;
  شروع = transport;
  طالما(شروع && *شروع) {
    curlx_str_passblanks(&شروع);
    انتها = strchr(شروع, ';');
    إذا(checkprefix("interleaved=", شروع)) {
      curl_off_t chan1, chan2, chan;
      ثابت حرف *p = شروع + 12;
      إذا(!curlx_str_عدد(&p, &chan1, 255)) {
        بیعلامت حرف *rtp_channel_ماسک = داده->state.rtp_channel_ماسک;
        chan2 = chan1;
        إذا(!curlx_str_single(&p, '-') && curlx_str_عدد(&p, &chan2, 255)) {
          infof(داده, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        لكل(chan = chan1; chan <= chan2; chan++) {
          صحيح idx = (صحيح)chan / 8;
          صحيح off = (صحيح)chan % 8;
          rtp_channel_ماسک[idx] |= (بیعلامت حرف)(1 << off);
        }
      }
      وإلا {
        infof(داده, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      اكسر;
    }
    /* skip to next parameter */
    شروع = (!انتها) ? انتها : (انتها + 1);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(بنية Curl_easy *داده, ثابت حرف *سرآیند)
{
  إذا(checkprefix("CSeq:", سرآیند)) {
    curl_off_t CSeq = 0;
    بنية RTSP *rtsp = Curl_meta_get(داده, CURL_META_RTSP_EASY);
    ثابت حرف *p = &سرآیند[5];
    إذا(!rtsp)
      أرجع CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    إذا(curlx_str_عدد(&p, &CSeq, UINT_MAX)) {
      failf(داده, "Unable to read the CSeq header: [%s]", سرآیند);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    داده->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  وإلا إذا(checkprefix("Session:", سرآیند)) {
    ثابت حرف *شروع, *انتها, *str;
    size_t idlen;

    /* Find the first non-space letter */
    شروع = سرآیند + 8;
    curlx_str_passblanks(&شروع);

    إذا(!*شروع) {
      failf(داده, "Got a blank Session ID");
      أرجع CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    انتها = شروع;
    طالما((*انتها > ' ') && (*انتها != ';'))
      انتها++;
    idlen = انتها - شروع;

    str = CURL_EASY_STR(داده, STRING_RTSP_SESSION_ID);
    إذا(str) {

      /* If the Session ID is set, then compare */
      إذا(طول_رشته(str) != idlen ||
         مقایسه_رشته(شروع, str, idlen)) {
        failf(داده, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
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
         CURL_EASY_STR_SETN(داده, STRING_RTSP_SESSION_ID, mem))
        أرجع CURLE_بیرون_از_حافظه;
      Curl_peer_link(&داده->state.rtsp_session_origin, داده->state.origin);
    }
  }
  وإلا إذا(checkprefix("Transport:", سرآیند)) {
    CURLcode نتیجه;
    نتیجه = rtsp_parse_transport(داده, سرآیند + 10);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_rtsp = {
  rtsp_راهاندازی_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_انجامشده,                            /* done */
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

#پایان_اگر /* CURL_DISABLE_RTSP */
