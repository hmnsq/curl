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

#ifndef CURL_DISABLE_RTSP

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
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ಸಂರಚನಾ rtsp_conn {
  ಸಂರಚನಾ dynbuf buf;
  ಪೂರ್ಣಾಙ್ಕ rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
ಸಂರಚನಾ RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)((p)[2]))) << 8) | \
                            ((unsigned ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ಸ್ಥಿರ CURLcode rtsp_do_pollset(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ easy_pollset *ps)
{
  /* write mode */
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ಸ್ಥಿರ ಶೂನ್ಯ rtsp_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ RTSP *rtsp = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(rtsp);
}

ಸ್ಥಿರ ಶೂನ್ಯ rtsp_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ rtsp_conn *rtspc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ಸ್ಥಿರ CURLcode rtsp_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ rtsp_conn *rtspc;
  ಸಂರಚನಾ RTSP *rtsp;

  rtspc = curlx_calloc(1, ಪರಿಮಾಣ(*rtspc));
  ಯದಿ(!rtspc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ಯದಿ(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ RTSP));
  ಯದಿ(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ಸ್ಥಿರ bool rtsp_conn_is_dead(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  bool ಒಳಹರಿವು_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ಪ್ರತ್ಯಾಗಮ !Curl_conn_is_alive(data, conn, &ಒಳಹರಿವು_pending);
}

ಸ್ಥಿರ CURLcode rtsp_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  ಯದಿ(!rtspc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ಯದಿ(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  ಯದಿ(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtsp_done(ಸಂರಚನಾ Curl_easy *data,
                          CURLcode status, bool premature)
{
  ಸಂರಚನಾ rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  ಸಂರಚನಾ RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  ಯದಿ(!rtspc || !rtsp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ಯದಿ(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  ಯದಿ(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ಯದಿ((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      ಪ್ರತ್ಯಾಗಮ CURLE_RTSP_CSEQ_ERROR;
    }
    ಯದಿ(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ಯದಿ(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode rtsp_setup_body(ಸಂರಚನಾ Curl_easy *data,
                                unsigned ವರ್ಣ rtspreq,
                                ಸಂರಚನಾ dynbuf *reqp)
{
  CURLcode result;
  ಯದಿ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ಯದಿ(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      ಅನ್ಯಥಾ ಯದಿ(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      ಅನ್ಯಥಾ {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ(req_clen > 0) {
      ಯದಿ(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        ಪ್ರತ್ಯಾಗಮ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ಯದಿ(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }

      ಯದಿ(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ಯದಿ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
        }
      }

      ಯದಿ(rtspreq == RTSPREQ_ANNOUNCE) {
        ಯದಿ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
        }
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  ಅನ್ಯಥಾ
    result = Curl_creader_set_null(data);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ rtspselect {
  ಧ್ರುವ ವರ್ಣ *method;
  bool no_body;
};

ಸ್ಥಿರ CURLcode pick_method(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ unsigned ವರ್ಣ rtspreq,
                            ಧ್ರುವ ವರ್ಣ **p)
{
  ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ rtspselect req[] = {
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_LAST));
  ಯದಿ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Allocate and store a header string. */
ಸ್ಥಿರ CURLcode rtsp_header_alloc(ಧ್ರುವ ವರ್ಣ *header_name,
                                  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                                  ವರ್ಣ **target)
{
  ಯದಿ(!ಮೌಲ್ಯ)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, ಮೌಲ್ಯ);
  ಯದಿ(!*target)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ rtsp_blocks {
  ಧ್ರುವ ವರ್ಣ *request;
  ಧ್ರುವ ವರ್ಣ *session_id;
  ಧ್ರುವ ವರ್ಣ *accept;
  ಧ್ರುವ ವರ್ಣ *range;
  ಧ್ರುವ ವರ್ಣ *stream_uri;
  ಧ್ರುವ ವರ್ಣ *hd_proxy_auth;
  ಧ್ರುವ ವರ್ಣ *hd_auth;
  ವರ್ಣ *referrer;
  ವರ್ಣ *accept_encoding;
  ವರ್ಣ *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

ಸ್ಥಿರ CURLcode rtsp_setup_request(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ rtsp_blocks *b,
                                   ಧ್ರುವ unsigned ವರ್ಣ rtspreq)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ಯದಿ(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  ಅನ್ಯಥಾ
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  ಯದಿ(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    ಯದಿ(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      b->transport_alloc = TRUE;
    }
    ಅನ್ಯಥಾ {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ಯದಿ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      ರಿಕ್ತ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ಯದಿ(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, ರಿಕ್ತ, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  ಯದಿ(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    ಯದಿ(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ಯದಿ(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    ಯದಿ(!result)
      b->range = data->state.rangeline;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ಸ್ಥಿರ CURLcode rtsp_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ unsigned ವರ್ಣ rtspreq = data->set.rtspreq;
  ಧ್ರುವ ವರ್ಣ *str;
  ಸಂರಚನಾ RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  ಸಂರಚನಾ dynbuf req_buffer;
  ಸಂರಚನಾ rtsp_blocks block;
  memset(&block, 0, ಪರಿಮಾಣ(block));

  *done = TRUE;
  ಯದಿ(!rtsp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    ಗಚ್ಛ out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = ರಿಕ್ತ;

  /*
   * Sanity check the custom headers
   */
  ಯದಿ(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    ಗಚ್ಛ out;
  }
  ಯದಿ(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ಯದಿ(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /*
   * Shared HTTP-like options
   */
  result = curlx_dyn_addf(&req_buffer,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          block.transport ? block.transport : "",
                          block.accept ? block.accept : "",
                          block.accept_encoding ? block.accept_encoding : "",
                          block.range ? block.range : "",
                          block.referrer ? block.referrer : "");

  str = CURL_EASY_STR(data, STRING_USERAGENT);
  ಯದಿ(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  ಯದಿ(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  ಯದಿ(result)
    ಗಚ್ಛ out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  ಯದಿ(result) {
    failf(data, "Failed sending RTSP request");
    ಗಚ್ಛ out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  ಯದಿ(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  ಯದಿ(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  ಪ್ರತ್ಯಾಗಮ result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ಸ್ಥಿರ CURLcode rtp_write_body_junk(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ rtsp_conn *rtspc,
                                    ಧ್ರುವ ವರ್ಣ *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ಯದಿ(body_remain) {
    ಯದಿ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ಪ್ರತ್ಯಾಗಮ Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtp_client_write(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *ptr,
                                 size_t len)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  ಶೂನ್ಯ *user_ptr;

  ಯದಿ(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ಯದಿ(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  ಅನ್ಯಥಾ {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((ವರ್ಣ *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  ಯದಿ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }

  ಯದಿ(wrote != len) {
    failf(data, "Failed writing RTP data");
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode rtsp_filter_rtp(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ rtsp_conn *rtspc,
                                ಧ್ರುವ ವರ್ಣ *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ಯಾವತ್(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    ವಿಕಲ್ಪ(rtspc->state) {

    ಪಕ್ಷ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ಯಾವತ್(blen && buf[0] != '$') {
        ಯದಿ(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          ಯದಿ(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ಯದಿ(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            ಗಚ್ಛ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ಯದಿ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ಯದಿ(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ಯದಿ(result)
            ಗಚ್ಛ out;
        }
        ಯದಿ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          ಗಚ್ಛ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ಭಙ್ಗ;
    }

    ಪಕ್ಷ RTP_PARSE_CHANNEL: {
      ಪೂರ್ಣಾಙ್ಕ idx = ((unsigned ವರ್ಣ)buf[0]) / 8;
      ಪೂರ್ಣಾಙ್ಕ off = ((unsigned ವರ್ಣ)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ಯದಿ(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        ಯದಿ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ಯದಿ(result)
            ಗಚ್ಛ out;
        }
        ಅನ್ಯಥಾ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ಭಙ್ಗ;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned ವರ್ಣ)buf[0];
      ಯದಿ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ಭಙ್ಗ;
    }

    ಪಕ್ಷ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ಧ್ರುವ ವರ್ಣ *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ಯದಿ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ಯದಿ(rtp_len == 2)
        ಭಙ್ಗ;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      ಯದಿ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ಯದಿ(result)
          ಗಚ್ಛ out;
        ಭಙ್ಗ;
      }
      rtspc->state = RTP_PARSE_DATA;
      ಭಙ್ಗ;
    }

    ಪಕ್ಷ RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ಯದಿ(needed <= blen) {
        ಯದಿ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          ಗಚ್ಛ out;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          ಗಚ್ಛ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ಭಙ್ಗ;
    }

    ಸ್ವಭಾವ:
      DEBUGASSERT(0);
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
  }
out:
  ಯದಿ(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode rtsp_rtp_write_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಧ್ರುವ ವರ್ಣ *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ಸಂರಚನಾ rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  ಯದಿ(!rtspc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!data->req.header)
    rtspc->in_header = FALSE;
  ಯದಿ(!blen) {
    ಗಚ್ಛ out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ಯದಿ(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ಯದಿ(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ಯದಿ(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    buf += consumed;
    blen -= consumed;

    ಯದಿ(!data->req.header)
      rtspc->in_header = FALSE;

    ಯದಿ(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      ಯದಿ(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  ಯದಿ(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (ಪೂರ್ಣಾಙ್ಕ)rtspc->state,
               data->req.size));
  ಯದಿ(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  ಯದಿ((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode rtsp_rtp_write_resp_hd(ಸಂರಚನಾ Curl_easy *data,
                                       ಧ್ರುವ ವರ್ಣ *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ಪ್ರತ್ಯಾಗಮ rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

ಸ್ಥಿರ CURLcode rtsp_parse_transport(ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ವರ್ಣ *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ಧ್ರುವ ವರ್ಣ *start, *end;
  start = transport;
  ಯಾವತ್(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    ಯದಿ(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ಧ್ರುವ ವರ್ಣ *p = start + 12;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &chan1, 255)) {
        unsigned ವರ್ಣ *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        ಯದಿ(!curlx_str_single(&p, '-') && curlx_str_ಸಂಖ್ಯೆ(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        ಪ್ರತಿ(chan = chan1; chan <= chan2; chan++) {
          ಪೂರ್ಣಾಙ್ಕ idx = (ಪೂರ್ಣಾಙ್ಕ)chan / 8;
          ಪೂರ್ಣಾಙ್ಕ off = (ಪೂರ್ಣಾಙ್ಕ)chan % 8;
          rtp_channel_mask[idx] |= (unsigned ವರ್ಣ)(1 << off);
        }
      }
      ಅನ್ಯಥಾ {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ಭಙ್ಗ;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *header)
{
  ಯದಿ(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    ಸಂರಚನಾ RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    ಧ್ರುವ ವರ್ಣ *p = &header[5];
    ಯದಿ(!rtsp)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      ಪ್ರತ್ಯಾಗಮ CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("Session:", header)) {
    ಧ್ರುವ ವರ್ಣ *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    ಯದಿ(!*start) {
      failf(data, "Got a blank Session ID");
      ಪ್ರತ್ಯಾಗಮ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    ಯಾವತ್((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    ಯದಿ(str) {

      /* If the Session ID is set, then compare */
      ಯದಿ(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        ಪ್ರತ್ಯಾಗಮ CURLE_RTSP_SESSION_ERROR;
      }
    }
    ಅನ್ಯಥಾ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      ಶೂನ್ಯ *mem = curlx_memdup0(start, idlen);
      ಯದಿ(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_rtsp = {
  rtsp_setup_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_done,                            /* done */
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

#endif /* CURL_DISABLE_RTSP */
