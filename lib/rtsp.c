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
#شامل "urldata.h"
#شامل "rtsp.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "http.h"
#شامل "url.h"
#شامل "progress.h"
#شامل "strcase.h"
#شامل "select.h"
#شامل "connect.h"
#شامل "cfilters.h"
#شامل "curlx/strdup.h"
#شامل "bufref.h"
#شامل "curlx/strparse.h"
#شامل "peer.h"

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

قسمتعریف شمار {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_ڈیٹا
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ساخت rtsp_conn {
  ساخت dynbuf buf;
  صحیح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  ثنائی_ہندسہ(اندر_سرخی);
};

/* RTSP transfer data */
ساخت RTSP {
  uint32_t CSeq_بھیجا; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#تعریف_کرو RTP_PKT_لمبائی(p) ((((غیرعلامتی صحیح)((غیرعلامتی حرف)((p)[2]))) << 8) | \
                            ((غیرعلامتی صحیح)((غیرعلامتی حرف)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ساکن CURLcode rtsp_do_pollset(ساخت Curl_easy *ڈیٹا,
                                ساخت easy_pollset *ps)
{
  /* write mode */
  واپس Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
}

#تعریف_کرو MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ساکن خالی rtsp_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت RTSP *rtsp = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_free(rtsp);
}

ساکن خالی rtsp_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت rtsp_conn *rtspc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ساکن CURLcode rtsp_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت rtsp_conn *rtspc;
  ساخت RTSP *rtsp;

  rtspc = curlx_calloc(1, حجم(*rtspc));
  اگر(!rtspc)
    واپس CURLE_برآمد_کا_حافظہ;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  اگر(Curl_conn_meta_سیٹ(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  rtsp = curlx_calloc(1, حجم(ساخت RTSP));
  اگر(!rtsp ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ساکن bool rtsp_conn_is_dead(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  bool داخلہ_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  واپس !Curl_conn_is_alive(ڈیٹا, conn, &داخلہ_pending);
}

ساکن CURLcode rtsp_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت rtsp_conn *rtspc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_RTSP_CONN);

  اگر(!rtspc)
    واپس CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  اگر(ڈیٹا->state.rtsp_اگلا_client_CSeq == 0)
    ڈیٹا->state.rtsp_اگلا_client_CSeq = 1;
  اگر(ڈیٹا->state.rtsp_اگلا_server_CSeq == 0)
    ڈیٹا->state.rtsp_اگلا_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *مکمل = TRUE;
  واپس CURLE_OK;
}

ساکن CURLcode rtsp_مکمل(ساخت Curl_easy *ڈیٹا,
                          CURLcode حالت_2, bool premature)
{
  ساخت rtsp_conn *rtspc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_RTSP_CONN);
  ساخت RTSP *rtsp = Curl_meta_get(ڈیٹا, CURL_META_RTSP_EASY);
  CURLcode نتیجہ;

  اگر(!rtspc || !rtsp)
    واپس CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  اگر(ڈیٹا->سیٹ.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  نتیجہ = Curl_http_مکمل(ڈیٹا, حالت_2, premature);

  اگر(!حالت_2 && !نتیجہ) {
    /* Check the sequence numbers */
    uint32_t CSeq_بھیجا = rtsp->CSeq_بھیجا;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    اگر((ڈیٹا->سیٹ.rtspreq != RTSPREQ_RECEIVE) && (CSeq_بھیجا != CSeq_recv)) {
      failf(ڈیٹا,
            "The CSeq of this request %u did not match the response %u",
            CSeq_بھیجا, CSeq_recv);
      واپس CURLE_RTSP_CSEQ_ERROR;
    }
    اگر(ڈیٹا->سیٹ.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(ڈیٹا, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    اگر(ڈیٹا->سیٹ.rtspreq == RTSPREQ_RECEIVE &&
       ڈیٹا->req.eos_written) {
      failf(ڈیٹا, "Server prematurely closed the RTSP connection.");
      واپس CURLE_RECV_ERROR;
    }
  }

  واپس نتیجہ;
}

ساکن CURLcode rtsp_تیاری_body(ساخت Curl_easy *ڈیٹا,
                                غیرعلامتی حرف rtspreq,
                                ساخت dynbuf *reqp)
{
  CURLcode نتیجہ;
  اگر(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_سیٹ_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    اگر(ڈیٹا->state.upload) {
      req_clen = ڈیٹا->state.infilesize;
      ڈیٹا->state.httpreq = HTTPREQ_PUT;
      نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, req_clen);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ {
      اگر(ڈیٹا->سیٹ.postfields) {
        size_t plen = (ڈیٹا->سیٹ.postfieldsize >= 0) ?
          (size_t)ڈیٹا->سیٹ.postfieldsize : لڑی_لمبائی(ڈیٹا->سیٹ.postfields);
        req_clen = (curl_off_t)plen;
        نتیجہ = Curl_creader_سیٹ_buf(ڈیٹا, ڈیٹا->سیٹ.postfields, plen);
      }
      ورنہ اگر(ڈیٹا->state.infilesize >= 0) {
        req_clen = ڈیٹا->state.infilesize;
        نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, req_clen);
      }
      ورنہ {
        req_clen = 0;
        نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
      }
      اگر(نتیجہ)
        واپس نتیجہ;
    }

    اگر(req_clen > 0) {
      اگر(ڈیٹا->سیٹ.crlf) {
        failf(ڈیٹا, "Cannot do CRLF conversions on upload");
        واپس CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Content-Length"))) {
        نتیجہ = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        اگر(نتیجہ)
          واپس نتیجہ;
      }

      اگر(rtspreq == RTSPREQ_سیٹ_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Content-Type"))) {
          نتیجہ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          اگر(نتیجہ)
            واپس نتیجہ;
        }
      }

      اگر(rtspreq == RTSPREQ_ANNOUNCE) {
        اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Content-Type"))) {
          نتیجہ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          اگر(نتیجہ)
            واپس نتیجہ;
        }
      }
    }
    ورنہ اگر(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      ڈیٹا->state.httpreq = HTTPREQ_سر;
      ڈیٹا->req.no_body = TRUE;
    }
  }
  ورنہ
    نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
  واپس نتیجہ;
}

ساخت rtspselect {
  ثابت حرف *طریقہ;
  bool no_body;
};

ساکن CURLcode pick_طریقہ(ساخت Curl_easy *ڈیٹا,
                            ثابت غیرعلامتی حرف rtspreq,
                            ثابت حرف **p)
{
  ساکن ثابت ساخت rtspselect req[] = {
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
  اگر((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_آخری))
    واپس CURLE_BAD_FUNCTION_دلیل;
  *p = req[rtspreq - 1].طریقہ;
  ڈیٹا->req.no_body = req[rtspreq - 1].no_body;
  واپس CURLE_OK;
}

/* Allocate and store a header string. */
ساکن CURLcode rtsp_سرخی_alloc(ثابت حرف *سرخی_name,
                                  ثابت حرف *قدر,
                                  حرف **ہدف)
{
  اگر(!قدر)
    واپس CURLE_OK;
  curlx_free(*ہدف);
  *ہدف = curl_maprintf("%s: %s\r\n", سرخی_name, قدر);
  اگر(!*ہدف)
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
}

ساخت rtsp_بلاکیں {
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

ساکن CURLcode rtsp_تیاری_request(ساخت Curl_easy *ڈیٹا,
                                   ساخت rtsp_بلاکیں *b,
                                   ثابت غیرعلامتی حرف rtspreq)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  b->session_id = CURL_EASY_STR(ڈیٹا, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  اگر(CURL_EASY_STR(ڈیٹا, STRING_RTSP_دھارا_URI))
    b->دھارا_uri = CURL_EASY_STR(ڈیٹا, STRING_RTSP_دھارا_URI);
  ورنہ
    b->دھارا_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(ڈیٹا, STRCONST("Transport"));
  اگر(rtspreq == RTSPREQ_تیاری && !b->transport) {
    /* New Transport: setting? */
    اگر(CURL_EASY_STR(ڈیٹا, STRING_RTSP_TRANSPORT)) {
      نتیجہ = rtsp_سرخی_alloc(
        "Transport", CURL_EASY_STR(ڈیٹا, STRING_RTSP_TRANSPORT),
        &b->transport);
      اگر(نتیجہ)
        واپس نتیجہ;
      b->transport_alloc = TRUE;
    }
    ورنہ {
      failf(ڈیٹا,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  اگر(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(ڈیٹا, STRCONST("Accept")) ?
      تہی : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(ڈیٹا, STRING_ENCODING)) {
      نتیجہ = rtsp_سرخی_alloc("Accept-Encoding",
                                 CURL_EASY_STR(ڈیٹا, STRING_ENCODING),
                                 &b->accept_encoding);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  /* setup the authentication headers */
  نتیجہ = Curl_http_output_auth(ڈیٹا, conn, b->request, HTTPREQ_GET,
                                 b->دھارا_uri, تہی, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  b->hd_proxy_auth = ڈیٹا->req.hd_proxy_auth;
#اگر_ختم
  b->hd_auth = ڈیٹا->req.hd_auth;

  /* Referrer */
  اگر(Curl_bufref_ptr(&ڈیٹا->state.referer) &&
     !Curl_checkheaders(ڈیٹا, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&ڈیٹا->state.referer));
    اگر(!b->referrer)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  اگر(!نتیجہ &&
     ڈیٹا->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_ریکارڈ)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(ڈیٹا, STRCONST("Range")) && ڈیٹا->state.range) {
    نتیجہ = rtsp_سرخی_alloc("Range",
                               ڈیٹا->state.range,
                               &ڈیٹا->state.rangeline);
    اگر(!نتیجہ)
      b->range = ڈیٹا->state.rangeline;
  }
  واپس نتیجہ;
}

#تعریف_کرو HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ساکن CURLcode rtsp_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی حرف rtspreq = ڈیٹا->سیٹ.rtspreq;
  ثابت حرف *str;
  ساخت RTSP *rtsp = Curl_meta_get(ڈیٹا, CURL_META_RTSP_EASY);
  ساخت dynbuf req_بفر;
  ساخت rtsp_بلاکیں بلاک;
  memset(&بلاک, 0, حجم(بلاک));

  *مکمل = TRUE;
  اگر(!rtsp)
    واپس CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_بفر, DYN_RTSP_REQ_سرخی);

  rtsp->CSeq_بھیجا = ڈیٹا->state.rtsp_اگلا_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  نتیجہ = pick_طریقہ(ڈیٹا, rtspreq, &بلاک.request);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
    جاؤ برآمد;
  }

  نتیجہ = rtsp_تیاری_request(ڈیٹا, &بلاک,  rtspreq);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(بلاک.session_id && ڈیٹا->state.rtsp_session_origin &&
     !ڈیٹا->سیٹ.allow_auth_مقصد_other_hosts &&
     !Curl_peer_equal(ڈیٹا->state.origin, ڈیٹا->state.rtsp_session_origin))
    بلاک.session_id = تہی;

  /*
   * Sanity check the custom headers
   */
  اگر(Curl_checkheaders(ڈیٹا, STRCONST("CSeq"))) {
    failf(ڈیٹا, "CSeq cannot be set as a custom header.");
    نتیجہ = CURLE_RTSP_CSEQ_ERROR;
    جاؤ برآمد;
  }
  اگر(Curl_checkheaders(ڈیٹا, STRCONST("Session"))) {
    failf(ڈیٹا, "Session ID cannot be set as a custom header.");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }

  نتیجہ =
    curlx_dyn_addf(&req_بفر,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   بلاک.request, بلاک.دھارا_uri, rtsp->CSeq_بھیجا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  اگر(بلاک.session_id) {
    نتیجہ = curlx_dyn_addf(&req_بفر, "Session: %s\r\n", بلاک.session_id);
    اگر(نتیجہ)
      جاؤ برآمد;
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

  str = CURL_EASY_STR(ڈیٹا, STRING_USERAGENT);
  اگر(!نتیجہ && str && *str &&
     !Curl_checkheaders(ڈیٹا, STRCONST("User-Agent")))
    نتیجہ = curlx_dyn_addf(&req_بفر,
                            "User-Agent: %s\r\n", str);

  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addf(&req_بفر,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            بلاک.hd_proxy_auth ? بلاک.hd_proxy_auth : "",
                            بلاک.hd_auth ? بلاک.hd_auth : "");
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر((rtspreq == RTSPREQ_تیاری) || (rtspreq == RTSPREQ_DESCRIBE)) {
    نتیجہ = Curl_جوڑو_timecondition(ڈیٹا, &req_بفر);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  نتیجہ = Curl_جوڑو_custom_سرخیاں(ڈیٹا, FALSE, HTTPVERSION, &req_بفر);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = rtsp_تیاری_body(ڈیٹا, rtspreq, &req_بفر);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* Finish the request buffer */
  نتیجہ = curlx_dyn_addn(&req_بفر, STRCONST("\r\n"));
  اگر(نتیجہ)
    جاؤ برآمد;

  Curl_xfer_تیاری_sendrecv(ڈیٹا, FIRSTSOCKET, -1);

  /* issue the request */
  نتیجہ = Curl_req_send(ڈیٹا, &req_بفر, HTTPVERSION);
  اگر(نتیجہ) {
    failf(ڈیٹا, "Failed sending RTSP request");
    جاؤ برآمد;
  }

  /* Increment the CSeq on success */
  ڈیٹا->state.rtsp_اگلا_client_CSeq++;

  اگر(ڈیٹا->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_سیٹ_Upload_شمار(ڈیٹا, ڈیٹا->req.writebytecount);
    نتیجہ = Curl_pgrsUpdate(ڈیٹا);
  }
برآمد:
  اگر(بلاک.transport_alloc)
    curlx_free(بلاک.transport);
  curlx_free(بلاک.accept_encoding);
  curlx_free(بلاک.referrer);
  curlx_dyn_free(&req_بفر);
  واپس نتیجہ;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ساکن CURLcode rtp_write_body_junk(ساخت Curl_easy *ڈیٹا,
                                    ساخت rtsp_conn *rtspc,
                                    ثابت حرف *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool اندر_body;

  اندر_body = (ڈیٹا->req.headerline && !rtspc->اندر_سرخی) &&
            (ڈیٹا->req.سائز >= 0) &&
            (ڈیٹا->req.bytecount < ڈیٹا->req.سائز);
  body_remain = اندر_body ? (ڈیٹا->req.سائز - ڈیٹا->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  اگر(body_remain) {
    اگر((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    واپس Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, buf, blen);
  }
  واپس CURLE_OK;
}

ساکن CURLcode rtp_client_write(ساخت Curl_easy *ڈیٹا, ثابت حرف *ptr,
                                 size_t len)
{
  ساخت Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  خالی *user_ptr;

  اگر(len == 0) {
    failf(ڈیٹا, "Cannot write a 0 size RTP packet.");
    واپس CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  اگر(ڈیٹا->سیٹ.fwrite_rtp) {
    writeit = ڈیٹا->سیٹ.fwrite_rtp;
    user_ptr = ڈیٹا->سیٹ.rtp_برآمد;
  }
  ورنہ {
    writeit = ڈیٹا->سیٹ.fwrite_func;
    user_ptr = ڈیٹا->سیٹ.برآمد;
  }

  CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fwrite_rtp);
  wrote = writeit((حرف *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_انتہا(&guard);

  اگر(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(ڈیٹا, "Cannot pause RTP");
    واپس CURLE_WRITE_ERROR;
  }

  اگر(wrote != len) {
    failf(ڈیٹا, "Failed writing RTP data");
    واپس CURLE_WRITE_ERROR;
  }

  واپس CURLE_OK;
}

ساکن CURLcode rtsp_filter_rtp(ساخت Curl_easy *ڈیٹا,
                                ساخت rtsp_conn *rtspc,
                                ثابت حرف *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  جب(blen) {
    bool اندر_body = (ڈیٹا->req.headerline && !rtspc->اندر_سرخی) &&
                   (ڈیٹا->req.سائز >= 0) &&
                   (ڈیٹا->req.bytecount < ڈیٹا->req.سائز);
    انتخاب(rtspc->state) {

    حالت RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      جب(blen && buf[0] != '$') {
        اگر(!اندر_body && buf[0] == 'R' &&
           ڈیٹا->سیٹ.rtspreq != RTSPREQ_RECEIVE) {
          اگر(!لڑی_موازنہ(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            اگر(*pconsumed) {
              DEBUGF(infof(ڈیٹا, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->اندر_سرخی = TRUE;
            جاؤ برآمد;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      اگر(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        اگر(skip_len) {
          /* end of junk/BODY bytes, flush */
          نتیجہ = rtp_write_body_junk(ڈیٹا, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          اگر(نتیجہ)
            جاؤ برآمد;
        }
        اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          نتیجہ = CURLE_برآمد_کا_حافظہ;
          جاؤ برآمد;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      توڑو;
    }

    حالت RTP_PARSE_CHANNEL: {
      صحیح idx = ((غیرعلامتی حرف)buf[0]) / 8;
      صحیح off = ((غیرعلامتی حرف)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      اگر(!(ڈیٹا->state.rtp_channel_نقاب[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(ڈیٹا, "RTSP: invalid RTP channel %d, skipping", idx));
        اگر(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          نتیجہ = rtp_write_body_junk(ڈیٹا, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          اگر(نتیجہ)
            جاؤ برآمد;
        }
        ورنہ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        توڑو;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (غیرعلامتی حرف)buf[0];
      اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      توڑو;
    }

    حالت RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ثابت حرف *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      اگر(rtp_len == 2)
        توڑو;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_لمبائی(rtp_buf) + 4;
      اگر(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(ڈیٹا, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجہ = rtp_client_write(ڈیٹا, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        اگر(نتیجہ)
          جاؤ برآمد;
        توڑو;
      }
      rtspc->state = RTP_PARSE_ڈیٹا;
      توڑو;
    }

    حالت RTP_PARSE_ڈیٹا: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      اگر(needed <= blen) {
        اگر(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          نتیجہ = CURLE_برآمد_کا_حافظہ;
          جاؤ برآمد;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(ڈیٹا, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        نتیجہ = rtp_client_write(ڈیٹا, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        اگر(نتیجہ)
          جاؤ برآمد;
      }
      ورنہ {
        اگر(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          نتیجہ = CURLE_برآمد_کا_حافظہ;
          جاؤ برآمد;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      توڑو;
    }

    طےشدہ:
      DEBUGASSERT(0);
      واپس CURLE_RECV_ERROR;
    }
  }
برآمد:
  اگر(!نتیجہ && skip_len)
    نتیجہ = rtp_write_body_junk(ڈیٹا, rtspc, buf - skip_len, skip_len);
  واپس نتیجہ;
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
ساکن CURLcode rtsp_rtp_write_resp(ساخت Curl_easy *ڈیٹا,
                                    ثابت حرف *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ساخت rtsp_conn *rtspc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_RTSP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  size_t consumed = 0;

  اگر(!rtspc)
    واپس CURLE_FAILED_INIT;

  اگر(!ڈیٹا->req.سرخی)
    rtspc->اندر_سرخی = FALSE;
  اگر(!blen) {
    جاؤ برآمد;
  }

  DEBUGF(infof(ڈیٹا, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->اندر_سرخی, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  اگر(!rtspc->اندر_سرخی) {
    نتیجہ = rtsp_filter_rtp(ڈیٹا, rtspc, buf, blen, &consumed);
    اگر(نتیجہ)
      جاؤ برآمد;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    اگر(blen && !ڈیٹا->req.سرخی)
      DEBUGF(infof(ڈیٹا, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  اگر(ڈیٹا->req.سرخی && blen) {
    rtspc->اندر_سرخی = TRUE;
    نتیجہ = Curl_http_write_resp_hds(ڈیٹا, buf, blen, &consumed);
    اگر(نتیجہ)
      جاؤ برآمد;

    buf += consumed;
    blen -= consumed;

    اگر(!ڈیٹا->req.سرخی)
      rtspc->اندر_سرخی = FALSE;

    اگر(!rtspc->اندر_سرخی) {
      /* If header parsing is done, extract interleaved RTP messages */
      اگر(ڈیٹا->req.سائز <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        ڈیٹا->req.سائز = 0;
        ڈیٹا->req.download_مکمل = TRUE;
      }
      نتیجہ = rtsp_filter_rtp(ڈیٹا, rtspc, buf, blen, &consumed);
      اگر(نتیجہ)
        جاؤ برآمد;
      buf += consumed;
      blen -= consumed;
    }
  }

  اگر(rtspc->state != RTP_PARSE_SKIP)
    ڈیٹا->req.مکمل = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(ڈیٹا, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->اندر_سرخی, ڈیٹا->req.مکمل, (صحیح)rtspc->state,
               ڈیٹا->req.سائز));
  اگر(!نتیجہ && (is_eos || blen)) {
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

برآمد:
  اگر((ڈیٹا->سیٹ.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    ڈیٹا->req.download_مکمل = TRUE;
  }
  واپس نتیجہ;
}

ساکن CURLcode rtsp_rtp_write_resp_hd(ساخت Curl_easy *ڈیٹا,
                                       ثابت حرف *buf,
                                       size_t blen,
                                       bool is_eos)
{
  واپس rtsp_rtp_write_resp(ڈیٹا, buf, blen, is_eos);
}

ساکن CURLcode rtsp_parse_transport(ساخت Curl_easy *ڈیٹا,
                                     ثابت حرف *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت حرف *آغاز, *انتہا;
  آغاز = transport;
  جب(آغاز && *آغاز) {
    curlx_str_passblanks(&آغاز);
    انتہا = strchr(آغاز, ';');
    اگر(checkprefix("interleaved=", آغاز)) {
      curl_off_t chan1, chan2, chan;
      ثابت حرف *p = آغاز + 12;
      اگر(!curlx_str_نمبر(&p, &chan1, 255)) {
        غیرعلامتی حرف *rtp_channel_نقاب = ڈیٹا->state.rtp_channel_نقاب;
        chan2 = chan1;
        اگر(!curlx_str_single(&p, '-') && curlx_str_نمبر(&p, &chan2, 255)) {
          infof(ڈیٹا, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        برائے(chan = chan1; chan <= chan2; chan++) {
          صحیح idx = (صحیح)chan / 8;
          صحیح off = (صحیح)chan % 8;
          rtp_channel_نقاب[idx] |= (غیرعلامتی حرف)(1 << off);
        }
      }
      ورنہ {
        infof(ڈیٹا, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      توڑو;
    }
    /* skip to next parameter */
    آغاز = (!انتہا) ? انتہا : (انتہا + 1);
  }
  واپس CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ساخت Curl_easy *ڈیٹا, ثابت حرف *سرخی)
{
  اگر(checkprefix("CSeq:", سرخی)) {
    curl_off_t CSeq = 0;
    ساخت RTSP *rtsp = Curl_meta_get(ڈیٹا, CURL_META_RTSP_EASY);
    ثابت حرف *p = &سرخی[5];
    اگر(!rtsp)
      واپس CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    اگر(curlx_str_نمبر(&p, &CSeq, UINT_MAX)) {
      failf(ڈیٹا, "Unable to read the CSeq header: [%s]", سرخی);
      واپس CURLE_RTSP_CSEQ_ERROR;
    }
    ڈیٹا->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ورنہ اگر(checkprefix("Session:", سرخی)) {
    ثابت حرف *آغاز, *انتہا, *str;
    size_t idlen;

    /* Find the first non-space letter */
    آغاز = سرخی + 8;
    curlx_str_passblanks(&آغاز);

    اگر(!*آغاز) {
      failf(ڈیٹا, "Got a blank Session ID");
      واپس CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    انتہا = آغاز;
    جب((*انتہا > ' ') && (*انتہا != ';'))
      انتہا++;
    idlen = انتہا - آغاز;

    str = CURL_EASY_STR(ڈیٹا, STRING_RTSP_SESSION_ID);
    اگر(str) {

      /* If the Session ID is set, then compare */
      اگر(لڑی_لمبائی(str) != idlen ||
         لڑی_موازنہ(آغاز, str, idlen)) {
        failf(ڈیٹا, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              آغاز, str);
        واپس CURLE_RTSP_SESSION_ERROR;
      }
    }
    ورنہ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      خالی *mem = curlx_memdup0(آغاز, idlen);
      اگر(!mem ||
         CURL_EASY_STR_SETN(ڈیٹا, STRING_RTSP_SESSION_ID, mem))
        واپس CURLE_برآمد_کا_حافظہ;
      Curl_peer_link(&ڈیٹا->state.rtsp_session_origin, ڈیٹا->state.origin);
    }
  }
  ورنہ اگر(checkprefix("Transport:", سرخی)) {
    CURLcode نتیجہ;
    نتیجہ = rtsp_parse_transport(ڈیٹا, سرخی + 10);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت ساخت Curl_protocol Curl_protocol_rtsp = {
  rtsp_تیاری_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_مکمل,                            /* done */
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

#اگر_ختم /* CURL_DISABLE_RTSP */
