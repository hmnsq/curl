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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rtsp.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP

#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "peer.h"

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_ਅੰਕੜੇ
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ਬਣਤਰ rtsp_conn {
  ਬਣਤਰ dynbuf buf;
  ਪੂਰਨ rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਚ_ਸਿਰਲੇਖ);
};

/* RTSP transfer data */
ਬਣਤਰ RTSP {
  uint32_t CSeq_ਭੇਜਿਆ; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTP_PKT_ਲੰਮਾਈ(p) ((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((p)[2]))) << 8) | \
                            ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ਸਥਿਰ CURLcode rtsp_do_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ easy_pollset *ps)
{
  /* write mode */
  ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ਸਥਿਰ ਖਾਲੀ rtsp_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ RTSP *rtsp = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_free(rtsp);
}

ਸਥਿਰ ਖਾਲੀ rtsp_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ rtsp_conn *rtspc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ਸਥਿਰ CURLcode rtsp_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ rtsp_conn *rtspc;
  ਬਣਤਰ RTSP *rtsp;

  rtspc = curlx_calloc(1, ਆਕਾਰ(*rtspc));
  ਜੇ(!rtspc)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ਜੇ(Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  rtsp = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ RTSP));
  ਜੇ(!rtsp ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ਸਥਿਰ bool rtsp_conn_is_dead(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  bool ਨਿਵੇਸ਼_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ਵਾਪਸ !Curl_conn_is_alive(ਅੰਕੜੇ, conn, &ਨਿਵੇਸ਼_pending);
}

ਸਥਿਰ CURLcode rtsp_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ rtsp_conn *rtspc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_RTSP_CONN);

  ਜੇ(!rtspc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ਜੇ(ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq == 0)
    ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq = 1;
  ਜੇ(ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_server_CSeq == 0)
    ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode rtsp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          CURLcode ਸਥਿਤੀ, bool premature)
{
  ਬਣਤਰ rtsp_conn *rtspc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_RTSP_CONN);
  ਬਣਤਰ RTSP *rtsp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_RTSP_EASY);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!rtspc || !rtsp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  ਨਤੀਜਾ = Curl_http_ਪੂਰਾ(ਅੰਕੜੇ, ਸਥਿਤੀ, premature);

  ਜੇ(!ਸਥਿਤੀ && !ਨਤੀਜਾ) {
    /* Check the sequence numbers */
    uint32_t CSeq_ਭੇਜਿਆ = rtsp->CSeq_ਭੇਜਿਆ;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.rtspreq != RTSPREQ_RECEIVE) && (CSeq_ਭੇਜਿਆ != CSeq_recv)) {
      failf(ਅੰਕੜੇ,
            "The CSeq of this request %u did not match the response %u",
            CSeq_ਭੇਜਿਆ, CSeq_recv);
      ਵਾਪਸ CURLE_RTSP_CSEQ_ERROR;
    }
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(ਅੰਕੜੇ, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.rtspreq == RTSPREQ_RECEIVE &&
       ਅੰਕੜੇ->req.eos_written) {
      failf(ਅੰਕੜੇ, "Server prematurely closed the RTSP connection.");
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode rtsp_ਤਿਆਰੀ_body(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rtspreq,
                                ਬਣਤਰ dynbuf *reqp)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_ਸੈੱਟ_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ਜੇ(ਅੰਕੜੇ->state.upload) {
      req_clen = ਅੰਕੜੇ->state.infilesize;
      ਅੰਕੜੇ->state.httpreq = HTTPREQ_PUT;
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, req_clen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postfields) {
        size_t plen = (ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize >= 0) ?
          (size_t)ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize : ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ->ਸੈੱਟ.postfields);
        req_clen = (curl_off_t)plen;
        ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_buf(ਅੰਕੜੇ, ਅੰਕੜੇ->ਸੈੱਟ.postfields, plen);
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.infilesize >= 0) {
        req_clen = ਅੰਕੜੇ->state.infilesize;
        ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, req_clen);
      }
      ਨਹੀਂਤਾਂ {
        req_clen = 0;
        ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
      }
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ(req_clen > 0) {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.crlf) {
        failf(ਅੰਕੜੇ, "Cannot do CRLF conversions on upload");
        ਵਾਪਸ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Length"))) {
        ਨਤੀਜਾ = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }

      ਜੇ(rtspreq == RTSPREQ_ਸੈੱਟ_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Type"))) {
          ਨਤੀਜਾ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
        }
      }

      ਜੇ(rtspreq == RTSPREQ_ANNOUNCE) {
        ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Type"))) {
          ਨਤੀਜਾ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      ਅੰਕੜੇ->state.httpreq = HTTPREQ_ਸਿਰ;
      ਅੰਕੜੇ->req.no_body = TRUE;
    }
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ rtspselect {
  ਅਚਲ ਅੱਖਰ *ਵਿਧੀ;
  bool no_body;
};

ਸਥਿਰ CURLcode pick_ਵਿਧੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rtspreq,
                            ਅਚਲ ਅੱਖਰ **p)
{
  ਸਥਿਰ ਅਚਲ ਬਣਤਰ rtspselect req[] = {
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_ਆਖਰੀ));
  ਜੇ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_ਆਖਰੀ))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  *p = req[rtspreq - 1].ਵਿਧੀ;
  ਅੰਕੜੇ->req.no_body = req[rtspreq - 1].no_body;
  ਵਾਪਸ CURLE_OK;
}

/* Allocate and store a header string. */
ਸਥਿਰ CURLcode rtsp_ਸਿਰਲੇਖ_alloc(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ_name,
                                  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                                  ਅੱਖਰ **ਨਿਸ਼ਾਨਾ)
{
  ਜੇ(!ਮੁੱਲ)
    ਵਾਪਸ CURLE_OK;
  curlx_free(*ਨਿਸ਼ਾਨਾ);
  *ਨਿਸ਼ਾਨਾ = curl_maprintf("%s: %s\r\n", ਸਿਰਲੇਖ_name, ਮੁੱਲ);
  ਜੇ(!*ਨਿਸ਼ਾਨਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ rtsp_ਖੰਡਾਂ {
  ਅਚਲ ਅੱਖਰ *request;
  ਅਚਲ ਅੱਖਰ *session_id;
  ਅਚਲ ਅੱਖਰ *accept;
  ਅਚਲ ਅੱਖਰ *range;
  ਅਚਲ ਅੱਖਰ *ਧਾਰਾ_uri;
  ਅਚਲ ਅੱਖਰ *hd_proxy_auth;
  ਅਚਲ ਅੱਖਰ *hd_auth;
  ਅੱਖਰ *referrer;
  ਅੱਖਰ *accept_encoding;
  ਅੱਖਰ *transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(transport_alloc); /* if 'transport' is allocated */
};

ਸਥਿਰ CURLcode rtsp_ਤਿਆਰੀ_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ rtsp_ਖੰਡਾਂ *b,
                                   ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rtspreq)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  b->session_id = CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_ਧਾਰਾ_URI))
    b->ਧਾਰਾ_uri = CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_ਧਾਰਾ_URI);
  ਨਹੀਂਤਾਂ
    b->ਧਾਰਾ_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Transport"));
  ਜੇ(rtspreq == RTSPREQ_ਤਿਆਰੀ && !b->transport) {
    /* New Transport: setting? */
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_TRANSPORT)) {
      ਨਤੀਜਾ = rtsp_ਸਿਰਲੇਖ_alloc(
        "Transport", CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_TRANSPORT),
        &b->transport);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      b->transport_alloc = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ਜੇ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Accept")) ?
      ਨਿਰਾ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(ਅੰਕੜੇ, STRING_ENCODING)) {
      ਨਤੀਜਾ = rtsp_ਸਿਰਲੇਖ_alloc("Accept-Encoding",
                                 CURL_EASY_STR(ਅੰਕੜੇ, STRING_ENCODING),
                                 &b->accept_encoding);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  /* setup the authentication headers */
  ਨਤੀਜਾ = Curl_http_output_auth(ਅੰਕੜੇ, conn, b->request, HTTPREQ_GET,
                                 b->ਧਾਰਾ_uri, ਨਿਰਾ, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  b->hd_proxy_auth = ਅੰਕੜੇ->req.hd_proxy_auth;
#ਜੇ_ਅੰਤ
  b->hd_auth = ਅੰਕੜੇ->req.hd_auth;

  /* Referrer */
  ਜੇ(Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer) &&
     !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer));
    ਜੇ(!b->referrer)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ਜੇ(!ਨਤੀਜਾ &&
     ਅੰਕੜੇ->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_ਰਿਕਾਰਡ)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Range")) && ਅੰਕੜੇ->state.range) {
    ਨਤੀਜਾ = rtsp_ਸਿਰਲੇਖ_alloc("Range",
                               ਅੰਕੜੇ->state.range,
                               &ਅੰਕੜੇ->state.rangeline);
    ਜੇ(!ਨਤੀਜਾ)
      b->range = ਅੰਕੜੇ->state.rangeline;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ਸਥਿਰ CURLcode rtsp_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rtspreq = ਅੰਕੜੇ->ਸੈੱਟ.rtspreq;
  ਅਚਲ ਅੱਖਰ *str;
  ਬਣਤਰ RTSP *rtsp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_RTSP_EASY);
  ਬਣਤਰ dynbuf req_ਬਫਰ;
  ਬਣਤਰ rtsp_ਖੰਡਾਂ ਖੰਡ;
  memset(&ਖੰਡ, 0, ਆਕਾਰ(ਖੰਡ));

  *ਪੂਰਾ = TRUE;
  ਜੇ(!rtsp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_ਬਫਰ, DYN_RTSP_REQ_ਸਿਰਲੇਖ);

  rtsp->CSeq_ਭੇਜਿਆ = ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  ਨਤੀਜਾ = pick_ਵਿਧੀ(ਅੰਕੜੇ, rtspreq, &ਖੰਡ.request);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = rtsp_ਤਿਆਰੀ_request(ਅੰਕੜੇ, &ਖੰਡ,  rtspreq);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(ਖੰਡ.session_id && ਅੰਕੜੇ->state.rtsp_session_origin &&
     !ਅੰਕੜੇ->ਸੈੱਟ.allow_auth_ਮੰਜ਼ਿਲ_other_hosts &&
     !Curl_peer_equal(ਅੰਕੜੇ->state.origin, ਅੰਕੜੇ->state.rtsp_session_origin))
    ਖੰਡ.session_id = ਨਿਰਾ;

  /*
   * Sanity check the custom headers
   */
  ਜੇ(Curl_checkheaders(ਅੰਕੜੇ, STRCONST("CSeq"))) {
    failf(ਅੰਕੜੇ, "CSeq cannot be set as a custom header.");
    ਨਤੀਜਾ = CURLE_RTSP_CSEQ_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Session"))) {
    failf(ਅੰਕੜੇ, "Session ID cannot be set as a custom header.");
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ =
    curlx_dyn_addf(&req_ਬਫਰ,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   ਖੰਡ.request, ਖੰਡ.ਧਾਰਾ_uri, rtsp->CSeq_ਭੇਜਿਆ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ਜੇ(ਖੰਡ.session_id) {
    ਨਤੀਜਾ = curlx_dyn_addf(&req_ਬਫਰ, "Session: %s\r\n", ਖੰਡ.session_id);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /*
   * Shared HTTP-like options
   */
  ਨਤੀਜਾ = curlx_dyn_addf(&req_ਬਫਰ,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          ਖੰਡ.transport ? ਖੰਡ.transport : "",
                          ਖੰਡ.accept ? ਖੰਡ.accept : "",
                          ਖੰਡ.accept_encoding ? ਖੰਡ.accept_encoding : "",
                          ਖੰਡ.range ? ਖੰਡ.range : "",
                          ਖੰਡ.referrer ? ਖੰਡ.referrer : "");

  str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERAGENT);
  ਜੇ(!ਨਤੀਜਾ && str && *str &&
     !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("User-Agent")))
    ਨਤੀਜਾ = curlx_dyn_addf(&req_ਬਫਰ,
                            "User-Agent: %s\r\n", str);

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addf(&req_ਬਫਰ,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            ਖੰਡ.hd_proxy_auth ? ਖੰਡ.hd_proxy_auth : "",
                            ਖੰਡ.hd_auth ? ਖੰਡ.hd_auth : "");
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ((rtspreq == RTSPREQ_ਤਿਆਰੀ) || (rtspreq == RTSPREQ_DESCRIBE)) {
    ਨਤੀਜਾ = Curl_ਜੋੜੋ_timecondition(ਅੰਕੜੇ, &req_ਬਫਰ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_ਜੋੜੋ_custom_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, FALSE, HTTPVERSION, &req_ਬਫਰ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = rtsp_ਤਿਆਰੀ_body(ਅੰਕੜੇ, rtspreq, &req_ਬਫਰ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Finish the request buffer */
  ਨਤੀਜਾ = curlx_dyn_addn(&req_ਬਫਰ, STRCONST("\r\n"));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  Curl_xfer_ਤਿਆਰੀ_sendrecv(ਅੰਕੜੇ, FIRSTSOCKET, -1);

  /* issue the request */
  ਨਤੀਜਾ = Curl_req_send(ਅੰਕੜੇ, &req_ਬਫਰ, HTTPVERSION);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "Failed sending RTSP request");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Increment the CSeq on success */
  ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq++;

  ਜੇ(ਅੰਕੜੇ->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_ਸੈੱਟ_Upload_Counter(ਅੰਕੜੇ, ਅੰਕੜੇ->req.writebytecount);
    ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
  }
ਬਾਹਰਲਾ:
  ਜੇ(ਖੰਡ.transport_alloc)
    curlx_free(ਖੰਡ.transport);
  curlx_free(ਖੰਡ.accept_encoding);
  curlx_free(ਖੰਡ.referrer);
  curlx_dyn_free(&req_ਬਫਰ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ਸਥਿਰ CURLcode rtp_write_body_junk(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ rtsp_conn *rtspc,
                                    ਅਚਲ ਅੱਖਰ *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool ਵਿੱਚ_body;

  ਵਿੱਚ_body = (ਅੰਕੜੇ->req.headerline && !rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ) &&
            (ਅੰਕੜੇ->req.ਮਾਪ >= 0) &&
            (ਅੰਕੜੇ->req.bytecount < ਅੰਕੜੇ->req.ਮਾਪ);
  body_remain = ਵਿੱਚ_body ? (ਅੰਕੜੇ->req.ਮਾਪ - ਅੰਕੜੇ->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ਜੇ(body_remain) {
    ਜੇ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ਵਾਪਸ Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, buf, blen);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode rtp_client_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ptr,
                                 size_t len)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  ਖਾਲੀ *user_ptr;

  ਜੇ(len == 0) {
    failf(ਅੰਕੜੇ, "Cannot write a 0 size RTP packet.");
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fwrite_rtp) {
    writeit = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_rtp;
    user_ptr = ਅੰਕੜੇ->ਸੈੱਟ.rtp_ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    writeit = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_func;
    user_ptr = ਅੰਕੜੇ->ਸੈੱਟ.ਬਾਹਰਲਾ;
  }

  CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fwrite_rtp);
  wrote = writeit((ਅੱਖਰ *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_ਸਿਰਾ(&guard);

  ਜੇ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(ਅੰਕੜੇ, "Cannot pause RTP");
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }

  ਜੇ(wrote != len) {
    failf(ਅੰਕੜੇ, "Failed writing RTP data");
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode rtsp_filter_rtp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ rtsp_conn *rtspc,
                                ਅਚਲ ਅੱਖਰ *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ਜਦੋਂ(blen) {
    bool ਵਿੱਚ_body = (ਅੰਕੜੇ->req.headerline && !rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ) &&
                   (ਅੰਕੜੇ->req.ਮਾਪ >= 0) &&
                   (ਅੰਕੜੇ->req.bytecount < ਅੰਕੜੇ->req.ਮਾਪ);
    ਚੋਣ(rtspc->state) {

    ਹਾਲਤ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ਜਦੋਂ(blen && buf[0] != '$') {
        ਜੇ(!ਵਿੱਚ_body && buf[0] == 'R' &&
           ਅੰਕੜੇ->ਸੈੱਟ.rtspreq != RTSPREQ_RECEIVE) {
          ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ਜੇ(*pconsumed) {
              DEBUGF(infof(ਅੰਕੜੇ, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ = TRUE;
            ਜਾਓ ਬਾਹਰਲਾ;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ਜੇ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ਜੇ(skip_len) {
          /* end of junk/BODY bytes, flush */
          ਨਤੀਜਾ = rtp_write_body_junk(ਅੰਕੜੇ, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ਜੇ(ਨਤੀਜਾ)
            ਜਾਓ ਬਾਹਰਲਾ;
        }
        ਜੇ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ਰੋਕੋ;
    }

    ਹਾਲਤ RTP_PARSE_CHANNEL: {
      ਪੂਰਨ idx = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)buf[0]) / 8;
      ਪੂਰਨ off = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ਜੇ(!(ਅੰਕੜੇ->state.rtp_channel_ਨਕਾਬ[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(ਅੰਕੜੇ, "RTSP: invalid RTP channel %d, skipping", idx));
        ਜੇ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          ਨਤੀਜਾ = rtp_write_body_junk(ਅੰਕੜੇ, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ਜੇ(ਨਤੀਜਾ)
            ਜਾਓ ਬਾਹਰਲਾ;
        }
        ਨਹੀਂਤਾਂ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ਰੋਕੋ;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)buf[0];
      ਜੇ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ਰੋਕੋ;
    }

    ਹਾਲਤ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ਅਚਲ ਅੱਖਰ *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ਜੇ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ਜੇ(rtp_len == 2)
        ਰੋਕੋ;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_ਲੰਮਾਈ(rtp_buf) + 4;
      ਜੇ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(ਅੰਕੜੇ, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ਨਤੀਜਾ = rtp_client_write(ਅੰਕੜੇ, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਰੋਕੋ;
      }
      rtspc->state = RTP_PARSE_ਅੰਕੜੇ;
      ਰੋਕੋ;
    }

    ਹਾਲਤ RTP_PARSE_ਅੰਕੜੇ: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ਜੇ(needed <= blen) {
        ਜੇ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(ਅੰਕੜੇ, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ਨਤੀਜਾ = rtp_client_write(ਅੰਕੜੇ, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ਰੋਕੋ;
    }

    ਮੂਲ:
      DEBUGASSERT(0);
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
  }
ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ && skip_len)
    ਨਤੀਜਾ = rtp_write_body_junk(ਅੰਕੜੇ, rtspc, buf - skip_len, skip_len);
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode rtsp_rtp_write_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਅੱਖਰ *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ਬਣਤਰ rtsp_conn *rtspc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_RTSP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t consumed = 0;

  ਜੇ(!rtspc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!ਅੰਕੜੇ->req.ਸਿਰਲੇਖ)
    rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ = FALSE;
  ਜੇ(!blen) {
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  DEBUGF(infof(ਅੰਕੜੇ, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ਜੇ(!rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ) {
    ਨਤੀਜਾ = rtsp_filter_rtp(ਅੰਕੜੇ, rtspc, buf, blen, &consumed);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ਜੇ(blen && !ਅੰਕੜੇ->req.ਸਿਰਲੇਖ)
      DEBUGF(infof(ਅੰਕੜੇ, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ਜੇ(ਅੰਕੜੇ->req.ਸਿਰਲੇਖ && blen) {
    rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ = TRUE;
    ਨਤੀਜਾ = Curl_http_write_resp_hds(ਅੰਕੜੇ, buf, blen, &consumed);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    buf += consumed;
    blen -= consumed;

    ਜੇ(!ਅੰਕੜੇ->req.ਸਿਰਲੇਖ)
      rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ = FALSE;

    ਜੇ(!rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ) {
      /* If header parsing is done, extract interleaved RTP messages */
      ਜੇ(ਅੰਕੜੇ->req.ਮਾਪ <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        ਅੰਕੜੇ->req.ਮਾਪ = 0;
        ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
      }
      ਨਤੀਜਾ = rtsp_filter_rtp(ਅੰਕੜੇ, rtspc, buf, blen, &consumed);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      buf += consumed;
      blen -= consumed;
    }
  }

  ਜੇ(rtspc->state != RTP_PARSE_SKIP)
    ਅੰਕੜੇ->req.ਪੂਰਾ = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(ਅੰਕੜੇ, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->ਵਿੱਚ_ਸਿਰਲੇਖ, ਅੰਕੜੇ->req.ਪੂਰਾ, (ਪੂਰਨ)rtspc->state,
               ਅੰਕੜੇ->req.ਮਾਪ));
  ਜੇ(!ਨਤੀਜਾ && (is_eos || blen)) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

ਬਾਹਰਲਾ:
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode rtsp_rtp_write_resp_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਅਚਲ ਅੱਖਰ *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ਵਾਪਸ rtsp_rtp_write_resp(ਅੰਕੜੇ, buf, blen, is_eos);
}

ਸਥਿਰ CURLcode rtsp_parse_transport(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ ਅੱਖਰ *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ, *ਸਿਰਾ;
  ਸ਼ੁਰੂ = transport;
  ਜਦੋਂ(ਸ਼ੁਰੂ && *ਸ਼ੁਰੂ) {
    curlx_str_passblanks(&ਸ਼ੁਰੂ);
    ਸਿਰਾ = strchr(ਸ਼ੁਰੂ, ';');
    ਜੇ(checkprefix("interleaved=", ਸ਼ੁਰੂ)) {
      curl_off_t chan1, chan2, chan;
      ਅਚਲ ਅੱਖਰ *p = ਸ਼ੁਰੂ + 12;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &chan1, 255)) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rtp_channel_ਨਕਾਬ = ਅੰਕੜੇ->state.rtp_channel_ਨਕਾਬ;
        chan2 = chan1;
        ਜੇ(!curlx_str_single(&p, '-') && curlx_str_ਸੰਖਿਆ(&p, &chan2, 255)) {
          infof(ਅੰਕੜੇ, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        ਲਈ(chan = chan1; chan <= chan2; chan++) {
          ਪੂਰਨ idx = (ਪੂਰਨ)chan / 8;
          ਪੂਰਨ off = (ਪੂਰਨ)chan % 8;
          rtp_channel_ਨਕਾਬ[idx] |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(1 << off);
        }
      }
      ਨਹੀਂਤਾਂ {
        infof(ਅੰਕੜੇ, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ਰੋਕੋ;
    }
    /* skip to next parameter */
    ਸ਼ੁਰੂ = (!ਸਿਰਾ) ? ਸਿਰਾ : (ਸਿਰਾ + 1);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  ਜੇ(checkprefix("CSeq:", ਸਿਰਲੇਖ)) {
    curl_off_t CSeq = 0;
    ਬਣਤਰ RTSP *rtsp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_RTSP_EASY);
    ਅਚਲ ਅੱਖਰ *p = &ਸਿਰਲੇਖ[5];
    ਜੇ(!rtsp)
      ਵਾਪਸ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &CSeq, UINT_MAX)) {
      failf(ਅੰਕੜੇ, "Unable to read the CSeq header: [%s]", ਸਿਰਲੇਖ);
      ਵਾਪਸ CURLE_RTSP_CSEQ_ERROR;
    }
    ਅੰਕੜੇ->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("Session:", ਸਿਰਲੇਖ)) {
    ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ, *ਸਿਰਾ, *str;
    size_t idlen;

    /* Find the first non-space letter */
    ਸ਼ੁਰੂ = ਸਿਰਲੇਖ + 8;
    curlx_str_passblanks(&ਸ਼ੁਰੂ);

    ਜੇ(!*ਸ਼ੁਰੂ) {
      failf(ਅੰਕੜੇ, "Got a blank Session ID");
      ਵਾਪਸ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    ਸਿਰਾ = ਸ਼ੁਰੂ;
    ਜਦੋਂ((*ਸਿਰਾ > ' ') && (*ਸਿਰਾ != ';'))
      ਸਿਰਾ++;
    idlen = ਸਿਰਾ - ਸ਼ੁਰੂ;

    str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_SESSION_ID);
    ਜੇ(str) {

      /* If the Session ID is set, then compare */
      ਜੇ(ਲੜੀ_ਲੰਬਾਈ(str) != idlen ||
         ਲੜੀ_ਤੁਲਨਾ(ਸ਼ੁਰੂ, str, idlen)) {
        failf(ਅੰਕੜੇ, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              ਸ਼ੁਰੂ, str);
        ਵਾਪਸ CURLE_RTSP_SESSION_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      ਖਾਲੀ *mem = curlx_memdup0(ਸ਼ੁਰੂ, idlen);
      ਜੇ(!mem ||
         CURL_EASY_STR_SETN(ਅੰਕੜੇ, STRING_RTSP_SESSION_ID, mem))
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      Curl_peer_link(&ਅੰਕੜੇ->state.rtsp_session_origin, ਅੰਕੜੇ->state.origin);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("Transport:", ਸਿਰਲੇਖ)) {
    CURLcode ਨਤੀਜਾ;
    ਨਤੀਜਾ = rtsp_parse_transport(ਅੰਕੜੇ, ਸਿਰਲੇਖ + 10);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_rtsp = {
  rtsp_ਤਿਆਰੀ_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_ਪੂਰਾ,                            /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_RTSP */
