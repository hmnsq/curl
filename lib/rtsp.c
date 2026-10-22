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

#که_نه_تعریف_شوی CURL_DISABLE_RTSP

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
#تعریف_کړه CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کړه CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

تعريفنوع تعداد {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_معلومات
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
بنية rtsp_conn {
  بنية dynbuf buf;
  صحيح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  بټ(دننه_سرلیک);
};

/* RTSP transfer data */
بنية RTSP {
  uint32_t CSeq_لېږل_شوی; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#تعریف_کړه RTP_PKT_اوږدتیا(p) ((((بینښه صحيح)((بینښه حرف)((p)[2]))) << 8) | \
                            ((بینښه صحيح)((بینښه حرف)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ساكن CURLcode rtsp_do_pollset(بنية Curl_easy *معلومات,
                                بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جمع_کړه_بهر(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
}

#تعریف_کړه MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ساكن فراغ rtsp_easy_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية RTSP *rtsp = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  curlx_free(rtsp);
}

ساكن فراغ rtsp_conn_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية rtsp_conn *rtspc = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ساكن CURLcode rtsp_تیاری_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية rtsp_conn *rtspc;
  بنية RTSP *rtsp;

  rtspc = curlx_calloc(1, حجم(*rtspc));
  إذا(!rtspc)
    أرجع CURLE_بهر_د_حافظه;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  إذا(Curl_conn_meta_سېټ(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    أرجع CURLE_بهر_د_حافظه;

  rtsp = curlx_calloc(1, حجم(بنية RTSP));
  إذا(!rtsp ||
     Curl_meta_سېټ(معلومات, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    أرجع CURLE_بهر_د_حافظه;

  أرجع CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ساكن bool rtsp_conn_is_dead(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  bool ننوت_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  أرجع !Curl_conn_is_alive(معلومات, conn, &ننوت_pending);
}

ساكن CURLcode rtsp_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_RTSP_CONN);

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  إذا(معلومات->state.rtsp_بل_client_CSeq == 0)
    معلومات->state.rtsp_بل_client_CSeq = 1;
  إذا(معلومات->state.rtsp_بل_server_CSeq == 0)
    معلومات->state.rtsp_بل_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *بشپړ = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_بشپړ(بنية Curl_easy *معلومات,
                          CURLcode حالت, bool premature)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_RTSP_CONN);
  بنية RTSP *rtsp = Curl_meta_get(معلومات, CURL_META_RTSP_EASY);
  CURLcode پایله;

  إذا(!rtspc || !rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  إذا(معلومات->سېټ.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  پایله = Curl_http_بشپړ(معلومات, حالت, premature);

  إذا(!حالت && !پایله) {
    /* Check the sequence numbers */
    uint32_t CSeq_لېږل_شوی = rtsp->CSeq_لېږل_شوی;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    إذا((معلومات->سېټ.rtspreq != RTSPREQ_RECEIVE) && (CSeq_لېږل_شوی != CSeq_recv)) {
      failf(معلومات,
            "The CSeq of this request %u did not match the response %u",
            CSeq_لېږل_شوی, CSeq_recv);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    إذا(معلومات->سېټ.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(معلومات, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    إذا(معلومات->سېټ.rtspreq == RTSPREQ_RECEIVE &&
       معلومات->req.eos_written) {
      failf(معلومات, "Server prematurely closed the RTSP connection.");
      أرجع CURLE_RECV_ERROR;
    }
  }

  أرجع پایله;
}

ساكن CURLcode rtsp_تیاری_body(بنية Curl_easy *معلومات,
                                بینښه حرف rtspreq,
                                بنية dynbuf *reqp)
{
  CURLcode پایله;
  إذا(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_سېټ_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    إذا(معلومات->state.upload) {
      req_clen = معلومات->state.infilesize;
      معلومات->state.httpreq = HTTPREQ_PUT;
      پایله = Curl_creader_سېټ_fread(معلومات, req_clen);
      إذا(پایله)
        أرجع پایله;
    }
    وإلا {
      إذا(معلومات->سېټ.postfields) {
        size_t plen = (معلومات->سېټ.postfieldsize >= 0) ?
          (size_t)معلومات->سېټ.postfieldsize : مزي_اوږدوالی(معلومات->سېټ.postfields);
        req_clen = (curl_off_t)plen;
        پایله = Curl_creader_سېټ_buf(معلومات, معلومات->سېټ.postfields, plen);
      }
      وإلا إذا(معلومات->state.infilesize >= 0) {
        req_clen = معلومات->state.infilesize;
        پایله = Curl_creader_سېټ_fread(معلومات, req_clen);
      }
      وإلا {
        req_clen = 0;
        پایله = Curl_creader_سېټ_null(معلومات);
      }
      إذا(پایله)
        أرجع پایله;
    }

    إذا(req_clen > 0) {
      إذا(معلومات->سېټ.crlf) {
        failf(معلومات, "Cannot do CRLF conversions on upload");
        أرجع CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      إذا(!Curl_checkheaders(معلومات, STRCONST("Content-Length"))) {
        پایله = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        إذا(پایله)
          أرجع پایله;
      }

      إذا(rtspreq == RTSPREQ_سېټ_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        إذا(!Curl_checkheaders(معلومات, STRCONST("Content-Type"))) {
          پایله = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          إذا(پایله)
            أرجع پایله;
        }
      }

      إذا(rtspreq == RTSPREQ_ANNOUNCE) {
        إذا(!Curl_checkheaders(معلومات, STRCONST("Content-Type"))) {
          پایله = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          إذا(پایله)
            أرجع پایله;
        }
      }
    }
    وإلا إذا(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      معلومات->state.httpreq = HTTPREQ_سر;
      معلومات->req.no_body = TRUE;
    }
  }
  وإلا
    پایله = Curl_creader_سېټ_null(معلومات);
  أرجع پایله;
}

بنية rtspselect {
  ثابت حرف *لاره;
  bool no_body;
};

ساكن CURLcode pick_لاره(بنية Curl_easy *معلومات,
                            ثابت بینښه حرف rtspreq,
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_وروستی));
  إذا((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_وروستی))
    أرجع CURLE_BAD_FUNCTION_دلیل;
  *p = req[rtspreq - 1].لاره;
  معلومات->req.no_body = req[rtspreq - 1].no_body;
  أرجع CURLE_OK;
}

/* Allocate and store a header string. */
ساكن CURLcode rtsp_سرلیک_alloc(ثابت حرف *سرلیک_name,
                                  ثابت حرف *ارزښت,
                                  حرف **موخه)
{
  إذا(!ارزښت)
    أرجع CURLE_OK;
  curlx_free(*موخه);
  *موخه = curl_maprintf("%s: %s\r\n", سرلیک_name, ارزښت);
  إذا(!*موخه)
    أرجع CURLE_بهر_د_حافظه;
  أرجع CURLE_OK;
}

بنية rtsp_بلاکونه {
  ثابت حرف *request;
  ثابت حرف *session_id;
  ثابت حرف *accept;
  ثابت حرف *range;
  ثابت حرف *بهیر_uri;
  ثابت حرف *hd_proxy_auth;
  ثابت حرف *hd_auth;
  حرف *referrer;
  حرف *accept_encoding;
  حرف *transport;
  بټ(transport_alloc); /* if 'transport' is allocated */
};

ساكن CURLcode rtsp_تیاری_request(بنية Curl_easy *معلومات,
                                   بنية rtsp_بلاکونه *b,
                                   ثابت بینښه حرف rtspreq)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  b->session_id = CURL_EASY_STR(معلومات, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  إذا(CURL_EASY_STR(معلومات, STRING_RTSP_بهیر_URI))
    b->بهیر_uri = CURL_EASY_STR(معلومات, STRING_RTSP_بهیر_URI);
  وإلا
    b->بهیر_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(معلومات, STRCONST("Transport"));
  إذا(rtspreq == RTSPREQ_تیاری && !b->transport) {
    /* New Transport: setting? */
    إذا(CURL_EASY_STR(معلومات, STRING_RTSP_TRANSPORT)) {
      پایله = rtsp_سرلیک_alloc(
        "Transport", CURL_EASY_STR(معلومات, STRING_RTSP_TRANSPORT),
        &b->transport);
      إذا(پایله)
        أرجع پایله;
      b->transport_alloc = TRUE;
    }
    وإلا {
      failf(معلومات,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  إذا(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(معلومات, STRCONST("Accept")) ?
      عدم : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    إذا(!Curl_checkheaders(معلومات, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(معلومات, STRING_ENCODING)) {
      پایله = rtsp_سرلیک_alloc("Accept-Encoding",
                                 CURL_EASY_STR(معلومات, STRING_ENCODING),
                                 &b->accept_encoding);
      إذا(پایله)
        أرجع پایله;
    }
  }

  /* setup the authentication headers */
  پایله = Curl_http_output_auth(معلومات, conn, b->request, HTTPREQ_GET,
                                 b->بهیر_uri, عدم, FALSE);
  إذا(پایله)
    أرجع پایله;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  b->hd_proxy_auth = معلومات->req.hd_proxy_auth;
#د_که_پای
  b->hd_auth = معلومات->req.hd_auth;

  /* Referrer */
  إذا(Curl_bufref_ptr(&معلومات->state.referer) &&
     !Curl_checkheaders(معلومات, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&معلومات->state.referer));
    إذا(!b->referrer)
      پایله = CURLE_بهر_د_حافظه;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  إذا(!پایله &&
     معلومات->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_ریکارډ)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(معلومات, STRCONST("Range")) && معلومات->state.range) {
    پایله = rtsp_سرلیک_alloc("Range",
                               معلومات->state.range,
                               &معلومات->state.rangeline);
    إذا(!پایله)
      b->range = معلومات->state.rangeline;
  }
  أرجع پایله;
}

#تعریف_کړه HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ساكن CURLcode rtsp_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  ثابت بینښه حرف rtspreq = معلومات->سېټ.rtspreq;
  ثابت حرف *str;
  بنية RTSP *rtsp = Curl_meta_get(معلومات, CURL_META_RTSP_EASY);
  بنية dynbuf req_بفر;
  بنية rtsp_بلاکونه بلاک;
  memset(&بلاک, 0, حجم(بلاک));

  *بشپړ = TRUE;
  إذا(!rtsp)
    أرجع CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_بفر, DYN_RTSP_REQ_سرلیک);

  rtsp->CSeq_لېږل_شوی = معلومات->state.rtsp_بل_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  پایله = pick_لاره(معلومات, rtspreq, &بلاک.request);
  إذا(پایله)
    اذهب بهر;

  إذا(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, -1);
    اذهب بهر;
  }

  پایله = rtsp_تیاری_request(معلومات, &بلاک,  rtspreq);
  إذا(پایله)
    اذهب بهر;

  إذا(بلاک.session_id && معلومات->state.rtsp_session_origin &&
     !معلومات->سېټ.allow_auth_منزل_other_hosts &&
     !Curl_peer_equal(معلومات->state.origin, معلومات->state.rtsp_session_origin))
    بلاک.session_id = عدم;

  /*
   * Sanity check the custom headers
   */
  إذا(Curl_checkheaders(معلومات, STRCONST("CSeq"))) {
    failf(معلومات, "CSeq cannot be set as a custom header.");
    پایله = CURLE_RTSP_CSEQ_ERROR;
    اذهب بهر;
  }
  إذا(Curl_checkheaders(معلومات, STRCONST("Session"))) {
    failf(معلومات, "Session ID cannot be set as a custom header.");
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }

  پایله =
    curlx_dyn_addf(&req_بفر,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   بلاک.request, بلاک.بهیر_uri, rtsp->CSeq_لېږل_شوی);
  إذا(پایله)
    اذهب بهر;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  إذا(بلاک.session_id) {
    پایله = curlx_dyn_addf(&req_بفر, "Session: %s\r\n", بلاک.session_id);
    إذا(پایله)
      اذهب بهر;
  }

  /*
   * Shared HTTP-like options
   */
  پایله = curlx_dyn_addf(&req_بفر,
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

  str = CURL_EASY_STR(معلومات, STRING_USERAGENT);
  إذا(!پایله && str && *str &&
     !Curl_checkheaders(معلومات, STRCONST("User-Agent")))
    پایله = curlx_dyn_addf(&req_بفر,
                            "User-Agent: %s\r\n", str);

  إذا(!پایله)
    پایله = curlx_dyn_addf(&req_بفر,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            بلاک.hd_proxy_auth ? بلاک.hd_proxy_auth : "",
                            بلاک.hd_auth ? بلاک.hd_auth : "");
  إذا(پایله)
    اذهب بهر;

  إذا((rtspreq == RTSPREQ_تیاری) || (rtspreq == RTSPREQ_DESCRIBE)) {
    پایله = Curl_جمع_کړه_timecondition(معلومات, &req_بفر);
    إذا(پایله)
      اذهب بهر;
  }

  پایله = Curl_جمع_کړه_custom_سرلیکونه(معلومات, FALSE, HTTPVERSION, &req_بفر);
  إذا(پایله)
    اذهب بهر;

  پایله = rtsp_تیاری_body(معلومات, rtspreq, &req_بفر);
  إذا(پایله)
    اذهب بهر;

  /* Finish the request buffer */
  پایله = curlx_dyn_addn(&req_بفر, STRCONST("\r\n"));
  إذا(پایله)
    اذهب بهر;

  Curl_xfer_تیاری_sendrecv(معلومات, FIRSTSOCKET, -1);

  /* issue the request */
  پایله = Curl_req_send(معلومات, &req_بفر, HTTPVERSION);
  إذا(پایله) {
    failf(معلومات, "Failed sending RTSP request");
    اذهب بهر;
  }

  /* Increment the CSeq on success */
  معلومات->state.rtsp_بل_client_CSeq++;

  إذا(معلومات->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_سېټ_Upload_Counter(معلومات, معلومات->req.writebytecount);
    پایله = Curl_pgrsUpdate(معلومات);
  }
بهر:
  إذا(بلاک.transport_alloc)
    curlx_free(بلاک.transport);
  curlx_free(بلاک.accept_encoding);
  curlx_free(بلاک.referrer);
  curlx_dyn_free(&req_بفر);
  أرجع پایله;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ساكن CURLcode rtp_write_body_junk(بنية Curl_easy *معلومات,
                                    بنية rtsp_conn *rtspc,
                                    ثابت حرف *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool دننه_body;

  دننه_body = (معلومات->req.headerline && !rtspc->دننه_سرلیک) &&
            (معلومات->req.اندازه >= 0) &&
            (معلومات->req.bytecount < معلومات->req.اندازه);
  body_remain = دننه_body ? (معلومات->req.اندازه - معلومات->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  إذا(body_remain) {
    إذا((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    أرجع Curl_client_write(معلومات, CLIENTWRITE_BODY, buf, blen);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode rtp_client_write(بنية Curl_easy *معلومات, ثابت حرف *ptr,
                                 size_t len)
{
  بنية Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  فراغ *user_ptr;

  إذا(len == 0) {
    failf(معلومات, "Cannot write a 0 size RTP packet.");
    أرجع CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  إذا(معلومات->سېټ.fwrite_rtp) {
    writeit = معلومات->سېټ.fwrite_rtp;
    user_ptr = معلومات->سېټ.rtp_بهر;
  }
  وإلا {
    writeit = معلومات->سېټ.fwrite_func;
    user_ptr = معلومات->سېټ.بهر;
  }

  CURL_CBAPI_پیل(&guard, معلومات, easy_fwrite_rtp);
  wrote = writeit((حرف *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_پای_ټکی(&guard);

  إذا(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(معلومات, "Cannot pause RTP");
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(wrote != len) {
    failf(معلومات, "Failed writing RTP data");
    أرجع CURLE_WRITE_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode rtsp_filter_rtp(بنية Curl_easy *معلومات,
                                بنية rtsp_conn *rtspc,
                                ثابت حرف *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode پایله = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  طالما(blen) {
    bool دننه_body = (معلومات->req.headerline && !rtspc->دننه_سرلیک) &&
                   (معلومات->req.اندازه >= 0) &&
                   (معلومات->req.bytecount < معلومات->req.اندازه);
    بدل(rtspc->state) {

    حالة RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      طالما(blen && buf[0] != '$') {
        إذا(!دننه_body && buf[0] == 'R' &&
           معلومات->سېټ.rtspreq != RTSPREQ_RECEIVE) {
          إذا(!مزي_پرتله(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            إذا(*pconsumed) {
              DEBUGF(infof(معلومات, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->دننه_سرلیک = TRUE;
            اذهب بهر;
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
          پایله = rtp_write_body_junk(معلومات, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          إذا(پایله)
            اذهب بهر;
        }
        إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          پایله = CURLE_بهر_د_حافظه;
          اذهب بهر;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      اكسر;
    }

    حالة RTP_PARSE_CHANNEL: {
      صحيح idx = ((بینښه حرف)buf[0]) / 8;
      صحيح off = ((بینښه حرف)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      إذا(!(معلومات->state.rtp_channel_ماسک[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(معلومات, "RTSP: invalid RTP channel %d, skipping", idx));
        إذا(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          پایله = rtp_write_body_junk(معلومات, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          إذا(پایله)
            اذهب بهر;
        }
        وإلا {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        اكسر;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (بینښه حرف)buf[0];
      إذا(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
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
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      إذا(rtp_len == 2)
        اكسر;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_اوږدتیا(rtp_buf) + 4;
      إذا(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(معلومات, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        پایله = rtp_client_write(معلومات, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(پایله)
          اذهب بهر;
        اكسر;
      }
      rtspc->state = RTP_PARSE_معلومات;
      اكسر;
    }

    حالة RTP_PARSE_معلومات: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      إذا(needed <= blen) {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          پایله = CURLE_بهر_د_حافظه;
          اذهب بهر;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(معلومات, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        پایله = rtp_client_write(معلومات, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        إذا(پایله)
          اذهب بهر;
      }
      وإلا {
        إذا(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          پایله = CURLE_بهر_د_حافظه;
          اذهب بهر;
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
بهر:
  إذا(!پایله && skip_len)
    پایله = rtp_write_body_junk(معلومات, rtspc, buf - skip_len, skip_len);
  أرجع پایله;
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
ساكن CURLcode rtsp_rtp_write_resp(بنية Curl_easy *معلومات,
                                    ثابت حرف *buf,
                                    size_t blen,
                                    bool is_eos)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(معلومات->conn, CURL_META_RTSP_CONN);
  CURLcode پایله = CURLE_OK;
  size_t consumed = 0;

  إذا(!rtspc)
    أرجع CURLE_FAILED_INIT;

  إذا(!معلومات->req.سرلیک)
    rtspc->دننه_سرلیک = FALSE;
  إذا(!blen) {
    اذهب بهر;
  }

  DEBUGF(infof(معلومات, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->دننه_سرلیک, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  إذا(!rtspc->دننه_سرلیک) {
    پایله = rtsp_filter_rtp(معلومات, rtspc, buf, blen, &consumed);
    إذا(پایله)
      اذهب بهر;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    إذا(blen && !معلومات->req.سرلیک)
      DEBUGF(infof(معلومات, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  إذا(معلومات->req.سرلیک && blen) {
    rtspc->دننه_سرلیک = TRUE;
    پایله = Curl_http_write_resp_hds(معلومات, buf, blen, &consumed);
    إذا(پایله)
      اذهب بهر;

    buf += consumed;
    blen -= consumed;

    إذا(!معلومات->req.سرلیک)
      rtspc->دننه_سرلیک = FALSE;

    إذا(!rtspc->دننه_سرلیک) {
      /* If header parsing is done, extract interleaved RTP messages */
      إذا(معلومات->req.اندازه <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        معلومات->req.اندازه = 0;
        معلومات->req.download_بشپړ = TRUE;
      }
      پایله = rtsp_filter_rtp(معلومات, rtspc, buf, blen, &consumed);
      إذا(پایله)
        اذهب بهر;
      buf += consumed;
      blen -= consumed;
    }
  }

  إذا(rtspc->state != RTP_PARSE_SKIP)
    معلومات->req.بشپړ = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(معلومات, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->دننه_سرلیک, معلومات->req.بشپړ, (صحيح)rtspc->state,
               معلومات->req.اندازه));
  إذا(!پایله && (is_eos || blen)) {
    پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

بهر:
  إذا((معلومات->سېټ.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    معلومات->req.download_بشپړ = TRUE;
  }
  أرجع پایله;
}

ساكن CURLcode rtsp_rtp_write_resp_hd(بنية Curl_easy *معلومات,
                                       ثابت حرف *buf,
                                       size_t blen,
                                       bool is_eos)
{
  أرجع rtsp_rtp_write_resp(معلومات, buf, blen, is_eos);
}

ساكن CURLcode rtsp_parse_transport(بنية Curl_easy *معلومات,
                                     ثابت حرف *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت حرف *پیل, *پای_ټکی;
  پیل = transport;
  طالما(پیل && *پیل) {
    curlx_str_passblanks(&پیل);
    پای_ټکی = strchr(پیل, ';');
    إذا(checkprefix("interleaved=", پیل)) {
      curl_off_t chan1, chan2, chan;
      ثابت حرف *p = پیل + 12;
      إذا(!curlx_str_شمېره(&p, &chan1, 255)) {
        بینښه حرف *rtp_channel_ماسک = معلومات->state.rtp_channel_ماسک;
        chan2 = chan1;
        إذا(!curlx_str_single(&p, '-') && curlx_str_شمېره(&p, &chan2, 255)) {
          infof(معلومات, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        لكل(chan = chan1; chan <= chan2; chan++) {
          صحيح idx = (صحيح)chan / 8;
          صحيح off = (صحيح)chan % 8;
          rtp_channel_ماسک[idx] |= (بینښه حرف)(1 << off);
        }
      }
      وإلا {
        infof(معلومات, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      اكسر;
    }
    /* skip to next parameter */
    پیل = (!پای_ټکی) ? پای_ټکی : (پای_ټکی + 1);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(بنية Curl_easy *معلومات, ثابت حرف *سرلیک)
{
  إذا(checkprefix("CSeq:", سرلیک)) {
    curl_off_t CSeq = 0;
    بنية RTSP *rtsp = Curl_meta_get(معلومات, CURL_META_RTSP_EASY);
    ثابت حرف *p = &سرلیک[5];
    إذا(!rtsp)
      أرجع CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    إذا(curlx_str_شمېره(&p, &CSeq, UINT_MAX)) {
      failf(معلومات, "Unable to read the CSeq header: [%s]", سرلیک);
      أرجع CURLE_RTSP_CSEQ_ERROR;
    }
    معلومات->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  وإلا إذا(checkprefix("Session:", سرلیک)) {
    ثابت حرف *پیل, *پای_ټکی, *str;
    size_t idlen;

    /* Find the first non-space letter */
    پیل = سرلیک + 8;
    curlx_str_passblanks(&پیل);

    إذا(!*پیل) {
      failf(معلومات, "Got a blank Session ID");
      أرجع CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    پای_ټکی = پیل;
    طالما((*پای_ټکی > ' ') && (*پای_ټکی != ';'))
      پای_ټکی++;
    idlen = پای_ټکی - پیل;

    str = CURL_EASY_STR(معلومات, STRING_RTSP_SESSION_ID);
    إذا(str) {

      /* If the Session ID is set, then compare */
      إذا(مزي_اوږدوالی(str) != idlen ||
         مزي_پرتله(پیل, str, idlen)) {
        failf(معلومات, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              پیل, str);
        أرجع CURLE_RTSP_SESSION_ERROR;
      }
    }
    وإلا {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      فراغ *mem = curlx_memdup0(پیل, idlen);
      إذا(!mem ||
         CURL_EASY_STR_SETN(معلومات, STRING_RTSP_SESSION_ID, mem))
        أرجع CURLE_بهر_د_حافظه;
      Curl_peer_link(&معلومات->state.rtsp_session_origin, معلومات->state.origin);
    }
  }
  وإلا إذا(checkprefix("Transport:", سرلیک)) {
    CURLcode پایله;
    پایله = rtsp_parse_transport(معلومات, سرلیک + 10);
    إذا(پایله)
      أرجع پایله;
  }
  أرجع CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_rtsp = {
  rtsp_تیاری_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_بشپړ,                            /* done */
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

#د_که_پای /* CURL_DISABLE_RTSP */
