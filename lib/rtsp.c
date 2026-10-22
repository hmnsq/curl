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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "peer.h"

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_ದತ್ತಾಂಶ
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ರಚನೆ rtsp_conn {
  ರಚನೆ dynbuf buf;
  ಪೂರ್ಣಾಂಕ rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  ಬಿಟ್(ಒಳ_ಶೀರ್ಷಿಕೆ);
};

/* RTSP transfer data */
ರಚನೆ RTSP {
  uint32_t CSeq_ಕಳುಹಿಸಿದ; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#ವ್ಯಾಖ್ಯಾನಿಸು RTP_PKT_ಉದ್ದಳತೆ(p) ((((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((p)[2]))) << 8) | \
                            ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ಸ್ಥಿರ CURLcode rtsp_do_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ easy_pollset *ps)
{
  /* write mode */
  ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ಸ್ಥಿರ ಶೂನ್ಯ rtsp_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ RTSP *rtsp = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(rtsp);
}

ಸ್ಥಿರ ಶೂನ್ಯ rtsp_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ rtsp_conn *rtspc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ಸ್ಥಿರ CURLcode rtsp_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ rtsp_conn *rtspc;
  ರಚನೆ RTSP *rtsp;

  rtspc = curlx_calloc(1, ಗಾತ್ರ(*rtspc));
  ಆದರೆ(!rtspc)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ಆದರೆ(Curl_conn_meta_ಗಣ(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  rtsp = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ RTSP));
  ಆದರೆ(!rtsp ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ಸ್ಥಿರ bool rtsp_conn_is_dead(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  bool ಒಳಹರಿವು_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ಹಿಂತಿರುಗಿ !Curl_conn_is_alive(ದತ್ತಾಂಶ, conn, &ಒಳಹರಿವು_pending);
}

ಸ್ಥಿರ CURLcode rtsp_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ rtsp_conn *rtspc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_RTSP_CONN);

  ಆದರೆ(!rtspc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ಆದರೆ(ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq == 0)
    ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq = 1;
  ಆದರೆ(ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_server_CSeq == 0)
    ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtsp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          CURLcode ಸ್ಥಿತಿ_2, bool premature)
{
  ರಚನೆ rtsp_conn *rtspc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_RTSP_CONN);
  ರಚನೆ RTSP *rtsp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_RTSP_EASY);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!rtspc || !rtsp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  ಫಲಿತಾಂಶ = Curl_http_ಮುಗಿದ(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, premature);

  ಆದರೆ(!ಸ್ಥಿತಿ_2 && !ಫಲಿತಾಂಶ) {
    /* Check the sequence numbers */
    uint32_t CSeq_ಕಳುಹಿಸಿದ = rtsp->CSeq_ಕಳುಹಿಸಿದ;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.rtspreq != RTSPREQ_RECEIVE) && (CSeq_ಕಳುಹಿಸಿದ != CSeq_recv)) {
      failf(ದತ್ತಾಂಶ,
            "The CSeq of this request %u did not match the response %u",
            CSeq_ಕಳುಹಿಸಿದ, CSeq_recv);
      ಹಿಂತಿರುಗಿ CURLE_RTSP_CSEQ_ERROR;
    }
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(ದತ್ತಾಂಶ, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.rtspreq == RTSPREQ_RECEIVE &&
       ದತ್ತಾಂಶ->req.eos_written) {
      failf(ದತ್ತಾಂಶ, "Server prematurely closed the RTSP connection.");
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode rtsp_ಸಿದ್ಧತೆ_body(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rtspreq,
                                ರಚನೆ dynbuf *reqp)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_ಗಣ_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
      req_clen = ದತ್ತಾಂಶ->state.infilesize;
      ದತ್ತಾಂಶ->state.httpreq = HTTPREQ_PUT;
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, req_clen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postfields) {
        size_t plen = (ದತ್ತಾಂಶ->ಗಣ.postfieldsize >= 0) ?
          (size_t)ದತ್ತಾಂಶ->ಗಣ.postfieldsize : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ->ಗಣ.postfields);
        req_clen = (curl_off_t)plen;
        ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_buf(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->ಗಣ.postfields, plen);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize >= 0) {
        req_clen = ದತ್ತಾಂಶ->state.infilesize;
        ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, req_clen);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        req_clen = 0;
        ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
      }
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ(req_clen > 0) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.crlf) {
        failf(ದತ್ತಾಂಶ, "Cannot do CRLF conversions on upload");
        ಹಿಂತಿರುಗಿ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Length"))) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }

      ಆದರೆ(rtspreq == RTSPREQ_ಗಣ_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Type"))) {
          ಫಲಿತಾಂಶ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }

      ಆದರೆ(rtspreq == RTSPREQ_ANNOUNCE) {
        ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Type"))) {
          ಫಲಿತಾಂಶ = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      ದತ್ತಾಂಶ->state.httpreq = HTTPREQ_ತಲೆ;
      ದತ್ತಾಂಶ->req.no_body = TRUE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ rtspselect {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ;
  bool no_body;
};

ಸ್ಥಿರ CURLcode pick_ವಿಧಾನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rtspreq,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ rtspselect req[] = {
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_ಕೊನೆಯ));
  ಆದರೆ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_ಕೊನೆಯ))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  *p = req[rtspreq - 1].ವಿಧಾನ;
  ದತ್ತಾಂಶ->req.no_body = req[rtspreq - 1].no_body;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Allocate and store a header string. */
ಸ್ಥಿರ CURLcode rtsp_ಶೀರ್ಷಿಕೆ_alloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ_name,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                                  ಅಕ್ಷರ **ಗುರಿ)
{
  ಆದರೆ(!ಮೌಲ್ಯ)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  curlx_free(*ಗುರಿ);
  *ಗುರಿ = curl_maprintf("%s: %s\r\n", ಶೀರ್ಷಿಕೆ_name, ಮೌಲ್ಯ);
  ಆದರೆ(!*ಗುರಿ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ rtsp_ಖಂಡಗಳು {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *session_id;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *range;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹರಿವು_uri;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd_proxy_auth;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd_auth;
  ಅಕ್ಷರ *referrer;
  ಅಕ್ಷರ *accept_encoding;
  ಅಕ್ಷರ *transport;
  ಬಿಟ್(transport_alloc); /* if 'transport' is allocated */
};

ಸ್ಥಿರ CURLcode rtsp_ಸಿದ್ಧತೆ_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ rtsp_ಖಂಡಗಳು *b,
                                   ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rtspreq)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  b->session_id = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_ಹರಿವು_URI))
    b->ಹರಿವು_uri = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_ಹರಿವು_URI);
  ಇಲ್ಲದಿದ್ದರೆ
    b->ಹರಿವು_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Transport"));
  ಆದರೆ(rtspreq == RTSPREQ_ಸಿದ್ಧತೆ && !b->transport) {
    /* New Transport: setting? */
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_TRANSPORT)) {
      ಫಲಿತಾಂಶ = rtsp_ಶೀರ್ಷಿಕೆ_alloc(
        "Transport", CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_TRANSPORT),
        &b->transport);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      b->transport_alloc = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ಆದರೆ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Accept")) ?
      ಖಾಲಿ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ENCODING)) {
      ಫಲಿತಾಂಶ = rtsp_ಶೀರ್ಷಿಕೆ_alloc("Accept-Encoding",
                                 CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ENCODING),
                                 &b->accept_encoding);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  /* setup the authentication headers */
  ಫಲಿತಾಂಶ = Curl_http_output_auth(ದತ್ತಾಂಶ, conn, b->request, HTTPREQ_GET,
                                 b->ಹರಿವು_uri, ಖಾಲಿ, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  b->hd_proxy_auth = ದತ್ತಾಂಶ->req.hd_proxy_auth;
#ಆದರೆ_ಅಂತ್ಯ
  b->hd_auth = ದತ್ತಾಂಶ->req.hd_auth;

  /* Referrer */
  ಆದರೆ(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer) &&
     !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer));
    ಆದರೆ(!b->referrer)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ಆದರೆ(!ಫಲಿತಾಂಶ &&
     ದತ್ತಾಂಶ->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_ದಾಖಲೆ)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Range")) && ದತ್ತಾಂಶ->state.range) {
    ಫಲಿತಾಂಶ = rtsp_ಶೀರ್ಷಿಕೆ_alloc("Range",
                               ದತ್ತಾಂಶ->state.range,
                               &ದತ್ತಾಂಶ->state.rangeline);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      b->range = ದತ್ತಾಂಶ->state.rangeline;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ಸ್ಥಿರ CURLcode rtsp_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rtspreq = ದತ್ತಾಂಶ->ಗಣ.rtspreq;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
  ರಚನೆ RTSP *rtsp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_RTSP_EASY);
  ರಚನೆ dynbuf req_ಬಫರ್;
  ರಚನೆ rtsp_ಖಂಡಗಳು ಖಂಡ;
  memset(&ಖಂಡ, 0, ಗಾತ್ರ(ಖಂಡ));

  *ಮುಗಿದ = TRUE;
  ಆದರೆ(!rtsp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_ಬಫರ್, DYN_RTSP_REQ_ಶೀರ್ಷಿಕೆ);

  rtsp->CSeq_ಕಳುಹಿಸಿದ = ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  ಫಲಿತಾಂಶ = pick_ವಿಧಾನ(ದತ್ತಾಂಶ, rtspreq, &ಖಂಡ.request);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = rtsp_ಸಿದ್ಧತೆ_request(ದತ್ತಾಂಶ, &ಖಂಡ,  rtspreq);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(ಖಂಡ.session_id && ದತ್ತಾಂಶ->state.rtsp_session_origin &&
     !ದತ್ತಾಂಶ->ಗಣ.allow_auth_ಗಮ್ಯ_other_hosts &&
     !Curl_peer_equal(ದತ್ತಾಂಶ->state.origin, ದತ್ತಾಂಶ->state.rtsp_session_origin))
    ಖಂಡ.session_id = ಖಾಲಿ;

  /*
   * Sanity check the custom headers
   */
  ಆದರೆ(Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("CSeq"))) {
    failf(ದತ್ತಾಂಶ, "CSeq cannot be set as a custom header.");
    ಫಲಿತಾಂಶ = CURLE_RTSP_CSEQ_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Session"))) {
    failf(ದತ್ತಾಂಶ, "Session ID cannot be set as a custom header.");
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ =
    curlx_dyn_addf(&req_ಬಫರ್,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   ಖಂಡ.request, ಖಂಡ.ಹರಿವು_uri, rtsp->CSeq_ಕಳುಹಿಸಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ಆದರೆ(ಖಂಡ.session_id) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&req_ಬಫರ್, "Session: %s\r\n", ಖಂಡ.session_id);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /*
   * Shared HTTP-like options
   */
  ಫಲಿತಾಂಶ = curlx_dyn_addf(&req_ಬಫರ್,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          ಖಂಡ.transport ? ಖಂಡ.transport : "",
                          ಖಂಡ.accept ? ಖಂಡ.accept : "",
                          ಖಂಡ.accept_encoding ? ಖಂಡ.accept_encoding : "",
                          ಖಂಡ.range ? ಖಂಡ.range : "",
                          ಖಂಡ.referrer ? ಖಂಡ.referrer : "");

  str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERAGENT);
  ಆದರೆ(!ಫಲಿತಾಂಶ && str && *str &&
     !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("User-Agent")))
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&req_ಬಫರ್,
                            "User-Agent: %s\r\n", str);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&req_ಬಫರ್,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            ಖಂಡ.hd_proxy_auth ? ಖಂಡ.hd_proxy_auth : "",
                            ಖಂಡ.hd_auth ? ಖಂಡ.hd_auth : "");
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ((rtspreq == RTSPREQ_ಸಿದ್ಧತೆ) || (rtspreq == RTSPREQ_DESCRIBE)) {
    ಫಲಿತಾಂಶ = Curl_ಕೂಡಿಸು_timecondition(ದತ್ತಾಂಶ, &req_ಬಫರ್);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_ಕೂಡಿಸು_custom_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, FALSE, HTTPVERSION, &req_ಬಫರ್);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = rtsp_ಸಿದ್ಧತೆ_body(ದತ್ತಾಂಶ, rtspreq, &req_ಬಫರ್);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Finish the request buffer */
  ಫಲಿತಾಂಶ = curlx_dyn_addn(&req_ಬಫರ್, STRCONST("\r\n"));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  Curl_xfer_ಸಿದ್ಧತೆ_sendrecv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);

  /* issue the request */
  ಫಲಿತಾಂಶ = Curl_req_send(ದತ್ತಾಂಶ, &req_ಬಫರ್, HTTPVERSION);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "Failed sending RTSP request");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Increment the CSeq on success */
  ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq++;

  ಆದರೆ(ದತ್ತಾಂಶ->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_ಗಣ_Upload_Counter(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.writebytecount);
    ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
  }
ಹೊರಹೋಗು:
  ಆದರೆ(ಖಂಡ.transport_alloc)
    curlx_free(ಖಂಡ.transport);
  curlx_free(ಖಂಡ.accept_encoding);
  curlx_free(ಖಂಡ.referrer);
  curlx_dyn_free(&req_ಬಫರ್);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ಸ್ಥಿರ CURLcode rtp_write_body_junk(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ rtsp_conn *rtspc,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool ಒಳ_body;

  ಒಳ_body = (ದತ್ತಾಂಶ->req.headerline && !rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ) &&
            (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 >= 0) &&
            (ದತ್ತಾಂಶ->req.bytecount < ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);
  body_remain = ಒಳ_body ? (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ಆದರೆ(body_remain) {
    ಆದರೆ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ಹಿಂತಿರುಗಿ Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, buf, blen);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtp_client_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr,
                                 size_t len)
{
  ರಚನೆ Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  ಶೂನ್ಯ *user_ptr;

  ಆದರೆ(len == 0) {
    failf(ದತ್ತಾಂಶ, "Cannot write a 0 size RTP packet.");
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fwrite_rtp) {
    writeit = ದತ್ತಾಂಶ->ಗಣ.fwrite_rtp;
    user_ptr = ದತ್ತಾಂಶ->ಗಣ.rtp_ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    writeit = ದತ್ತಾಂಶ->ಗಣ.fwrite_func;
    user_ptr = ದತ್ತಾಂಶ->ಗಣ.ಹೊರಹೋಗು;
  }

  CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fwrite_rtp);
  wrote = writeit((ಅಕ್ಷರ *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_ಕೊನೆ(&guard);

  ಆದರೆ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(ದತ್ತಾಂಶ, "Cannot pause RTP");
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }

  ಆದರೆ(wrote != len) {
    failf(ದತ್ತಾಂಶ, "Failed writing RTP data");
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtsp_filter_rtp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ rtsp_conn *rtspc,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ಆಗ(blen) {
    bool ಒಳ_body = (ದತ್ತಾಂಶ->req.headerline && !rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ) &&
                   (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 >= 0) &&
                   (ದತ್ತಾಂಶ->req.bytecount < ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);
    ಆಯ್ಕೆ(rtspc->state) {

    ಸ್ಥಿತಿ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ಆಗ(blen && buf[0] != '$') {
        ಆದರೆ(!ಒಳ_body && buf[0] == 'R' &&
           ದತ್ತಾಂಶ->ಗಣ.rtspreq != RTSPREQ_RECEIVE) {
          ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ಆದರೆ(*pconsumed) {
              DEBUGF(infof(ದತ್ತಾಂಶ, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ = TRUE;
            ಹೋಗು ಹೊರಹೋಗು;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ಆದರೆ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ಆದರೆ(skip_len) {
          /* end of junk/BODY bytes, flush */
          ಫಲಿತಾಂಶ = rtp_write_body_junk(ದತ್ತಾಂಶ, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹೋಗು ಹೊರಹೋಗು;
        }
        ಆದರೆ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ RTP_PARSE_CHANNEL: {
      ಪೂರ್ಣಾಂಕ idx = ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)buf[0]) / 8;
      ಪೂರ್ಣಾಂಕ off = ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ಆದರೆ(!(ದತ್ತಾಂಶ->state.rtp_channel_ಮುಖವಾಡ[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(ದತ್ತಾಂಶ, "RTSP: invalid RTP channel %d, skipping", idx));
        ಆದರೆ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          ಫಲಿತಾಂಶ = rtp_write_body_junk(ದತ್ತಾಂಶ, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹೋಗು ಹೊರಹೋಗು;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ನಿಲ್ಲಿಸು;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)buf[0];
      ಆದರೆ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ಆದರೆ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ಆದರೆ(rtp_len == 2)
        ನಿಲ್ಲಿಸು;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_ಉದ್ದಳತೆ(rtp_buf) + 4;
      ಆದರೆ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(ದತ್ತಾಂಶ, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ಫಲಿತಾಂಶ = rtp_client_write(ದತ್ತಾಂಶ, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
        ನಿಲ್ಲಿಸು;
      }
      rtspc->state = RTP_PARSE_ದತ್ತಾಂಶ;
      ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ RTP_PARSE_ದತ್ತಾಂಶ: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ಆದರೆ(needed <= blen) {
        ಆದರೆ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(ದತ್ತಾಂಶ, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ಫಲಿತಾಂಶ = rtp_client_write(ದತ್ತಾಂಶ, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ನಿಲ್ಲಿಸು;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
      DEBUGASSERT(0);
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
  }
ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ && skip_len)
    ಫಲಿತಾಂಶ = rtp_write_body_junk(ದತ್ತಾಂಶ, rtspc, buf - skip_len, skip_len);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode rtsp_rtp_write_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ರಚನೆ rtsp_conn *rtspc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_RTSP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t consumed = 0;

  ಆದರೆ(!rtspc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ)
    rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ = FALSE;
  ಆದರೆ(!blen) {
    ಹೋಗು ಹೊರಹೋಗು;
  }

  DEBUGF(infof(ದತ್ತಾಂಶ, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ಆದರೆ(!rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ) {
    ಫಲಿತಾಂಶ = rtsp_filter_rtp(ದತ್ತಾಂಶ, rtspc, buf, blen, &consumed);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ಆದರೆ(blen && !ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ)
      DEBUGF(infof(ದತ್ತಾಂಶ, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ಆದರೆ(ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ && blen) {
    rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ = TRUE;
    ಫಲಿತಾಂಶ = Curl_http_write_resp_hds(ದತ್ತಾಂಶ, buf, blen, &consumed);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    buf += consumed;
    blen -= consumed;

    ಆದರೆ(!ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ)
      rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ = FALSE;

    ಆದರೆ(!rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ) {
      /* If header parsing is done, extract interleaved RTP messages */
      ಆದರೆ(ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = 0;
        ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
      }
      ಫಲಿತಾಂಶ = rtsp_filter_rtp(ದತ್ತಾಂಶ, rtspc, buf, blen, &consumed);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      buf += consumed;
      blen -= consumed;
    }
  }

  ಆದರೆ(rtspc->state != RTP_PARSE_SKIP)
    ದತ್ತಾಂಶ->req.ಮುಗಿದ = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(ದತ್ತಾಂಶ, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->ಒಳ_ಶೀರ್ಷಿಕೆ, ದತ್ತಾಂಶ->req.ಮುಗಿದ, (ಪೂರ್ಣಾಂಕ)rtspc->state,
               ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2));
  ಆದರೆ(!ಫಲಿತಾಂಶ && (is_eos || blen)) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

ಹೊರಹೋಗು:
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode rtsp_rtp_write_resp_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ಹಿಂತಿರುಗಿ rtsp_rtp_write_resp(ದತ್ತಾಂಶ, buf, blen, is_eos);
}

ಸ್ಥಿರ CURLcode rtsp_parse_transport(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ, *ಕೊನೆ;
  ಆರಂಭ = transport;
  ಆಗ(ಆರಂಭ && *ಆರಂಭ) {
    curlx_str_passblanks(&ಆರಂಭ);
    ಕೊನೆ = strchr(ಆರಂಭ, ';');
    ಆದರೆ(checkprefix("interleaved=", ಆರಂಭ)) {
      curl_off_t chan1, chan2, chan;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಆರಂಭ + 12;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &chan1, 255)) {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *rtp_channel_ಮುಖವಾಡ = ದತ್ತಾಂಶ->state.rtp_channel_ಮುಖವಾಡ;
        chan2 = chan1;
        ಆದರೆ(!curlx_str_single(&p, '-') && curlx_str_ಸಂಖ್ಯೆ(&p, &chan2, 255)) {
          infof(ದತ್ತಾಂಶ, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        ಗಾಗಿ(chan = chan1; chan <= chan2; chan++) {
          ಪೂರ್ಣಾಂಕ idx = (ಪೂರ್ಣಾಂಕ)chan / 8;
          ಪೂರ್ಣಾಂಕ off = (ಪೂರ್ಣಾಂಕ)chan % 8;
          rtp_channel_ಮುಖವಾಡ[idx] |= (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(1 << off);
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        infof(ದತ್ತಾಂಶ, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ನಿಲ್ಲಿಸು;
    }
    /* skip to next parameter */
    ಆರಂಭ = (!ಕೊನೆ) ? ಕೊನೆ : (ಕೊನೆ + 1);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  ಆದರೆ(checkprefix("CSeq:", ಶೀರ್ಷಿಕೆ)) {
    curl_off_t CSeq = 0;
    ರಚನೆ RTSP *rtsp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_RTSP_EASY);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &ಶೀರ್ಷಿಕೆ[5];
    ಆದರೆ(!rtsp)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &CSeq, UINT_MAX)) {
      failf(ದತ್ತಾಂಶ, "Unable to read the CSeq header: [%s]", ಶೀರ್ಷಿಕೆ);
      ಹಿಂತಿರುಗಿ CURLE_RTSP_CSEQ_ERROR;
    }
    ದತ್ತಾಂಶ->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("Session:", ಶೀರ್ಷಿಕೆ)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ, *ಕೊನೆ, *str;
    size_t idlen;

    /* Find the first non-space letter */
    ಆರಂಭ = ಶೀರ್ಷಿಕೆ + 8;
    curlx_str_passblanks(&ಆರಂಭ);

    ಆದರೆ(!*ಆರಂಭ) {
      failf(ದತ್ತಾಂಶ, "Got a blank Session ID");
      ಹಿಂತಿರುಗಿ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    ಕೊನೆ = ಆರಂಭ;
    ಆಗ((*ಕೊನೆ > ' ') && (*ಕೊನೆ != ';'))
      ಕೊನೆ++;
    idlen = ಕೊನೆ - ಆರಂಭ;

    str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_SESSION_ID);
    ಆದರೆ(str) {

      /* If the Session ID is set, then compare */
      ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) != idlen ||
         ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಆರಂಭ, str, idlen)) {
        failf(ದತ್ತಾಂಶ, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              ಆರಂಭ, str);
        ಹಿಂತಿರುಗಿ CURLE_RTSP_SESSION_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      ಶೂನ್ಯ *mem = curlx_memdup0(ಆರಂಭ, idlen);
      ಆದರೆ(!mem ||
         CURL_EASY_STR_SETN(ದತ್ತಾಂಶ, STRING_RTSP_SESSION_ID, mem))
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      Curl_peer_link(&ದತ್ತಾಂಶ->state.rtsp_session_origin, ದತ್ತಾಂಶ->state.origin);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("Transport:", ಶೀರ್ಷಿಕೆ)) {
    CURLcode ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = rtsp_parse_transport(ದತ್ತಾಂಶ, ಶೀರ್ಷಿಕೆ + 10);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_rtsp = {
  rtsp_ಸಿದ್ಧತೆ_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_ಮುಗಿದ,                            /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_RTSP */
