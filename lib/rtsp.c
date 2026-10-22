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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ସଂରଚନା rtsp_conn {
  ସଂରଚନା dynbuf buf;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
ସଂରଚନା RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)((p)[2]))) << 8) | \
                            ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ସ୍ଥିର CURLcode rtsp_do_pollset(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା easy_pollset *ps)
{
  /* write mode */
  ଫେରନ୍ତୁ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ସ୍ଥିର ଶୂନ୍ୟ rtsp_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା RTSP *rtsp = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_free(rtsp);
}

ସ୍ଥିର ଶୂନ୍ୟ rtsp_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା rtsp_conn *rtspc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ସ୍ଥିର CURLcode rtsp_setup_connection(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା rtsp_conn *rtspc;
  ସଂରଚନା RTSP *rtsp;

  rtspc = curlx_calloc(1, ଆକାର(*rtspc));
  ଯଦି(!rtspc)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ଯଦି(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, ଆକାର(ସଂରଚନା RTSP));
  ଯଦି(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ସ୍ଥିର bool rtsp_conn_is_dead(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  bool ନିବେଶ_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ଫେରନ୍ତୁ !Curl_conn_is_alive(data, conn, &ନିବେଶ_pending);
}

ସ୍ଥିର CURLcode rtsp_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  ଯଦି(!rtspc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ଯଦି(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  ଯଦି(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode rtsp_done(ସଂରଚନା Curl_easy *data,
                          CURLcode status, bool premature)
{
  ସଂରଚନା rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  ସଂରଚନା RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  ଯଦି(!rtspc || !rtsp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ଯଦି(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  ଯଦି(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ଯଦି((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      ଫେରନ୍ତୁ CURLE_RTSP_CSEQ_ERROR;
    }
    ଯଦି(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ଯଦି(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode rtsp_setup_body(ସଂରଚନା Curl_easy *data,
                                unsigned ଅକ୍ଷର rtspreq,
                                ସଂରଚନା dynbuf *reqp)
{
  CURLcode result;
  ଯଦି(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ଯଦି(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା {
      ଯଦି(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      ଅନ୍ୟଥା ଯଦି(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      ଅନ୍ୟଥା {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }

    ଯଦି(req_clen > 0) {
      ଯଦି(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        ଫେରନ୍ତୁ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ଯଦି(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }

      ଯଦି(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ଯଦି(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
        }
      }

      ଯଦି(rtspreq == RTSPREQ_ANNOUNCE) {
        ଯଦି(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
        }
      }
    }
    ଅନ୍ୟଥା ଯଦି(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  ଅନ୍ୟଥା
    result = Curl_creader_set_null(data);
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା rtspselect {
  ଅଚଳ ଅକ୍ଷର *method;
  bool no_body;
};

ସ୍ଥିର CURLcode pick_method(ସଂରଚନା Curl_easy *data,
                            ଅଚଳ unsigned ଅକ୍ଷର rtspreq,
                            ଅଚଳ ଅକ୍ଷର **p)
{
  ସ୍ଥିର ଅଚଳ ସଂରଚନା rtspselect req[] = {
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
  ଯଦି((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Allocate and store a header string. */
ସ୍ଥିର CURLcode rtsp_header_alloc(ଅଚଳ ଅକ୍ଷର *header_name,
                                  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                                  ଅକ୍ଷର **target)
{
  ଯଦି(!ମୂଲ୍ୟ)
    ଫେରନ୍ତୁ CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, ମୂଲ୍ୟ);
  ଯଦି(!*target)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା rtsp_blocks {
  ଅଚଳ ଅକ୍ଷର *request;
  ଅଚଳ ଅକ୍ଷର *session_id;
  ଅଚଳ ଅକ୍ଷର *accept;
  ଅଚଳ ଅକ୍ଷର *range;
  ଅଚଳ ଅକ୍ଷର *stream_uri;
  ଅଚଳ ଅକ୍ଷର *hd_proxy_auth;
  ଅଚଳ ଅକ୍ଷର *hd_auth;
  ଅକ୍ଷର *referrer;
  ଅକ୍ଷର *accept_encoding;
  ଅକ୍ଷର *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

ସ୍ଥିର CURLcode rtsp_setup_request(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା rtsp_blocks *b,
                                   ଅଚଳ unsigned ଅକ୍ଷର rtspreq)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ଯଦି(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  ଅନ୍ୟଥା
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  ଯଦି(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    ଯଦି(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      b->transport_alloc = TRUE;
    }
    ଅନ୍ୟଥା {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ଯଦି(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      ଖାଲି : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ଯଦି(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, ଖାଲି, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  ଯଦି(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    ଯଦି(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ଯଦି(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    ଯଦି(!result)
      b->range = data->state.rangeline;
  }
  ଫେରନ୍ତୁ result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ସ୍ଥିର CURLcode rtsp_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ unsigned ଅକ୍ଷର rtspreq = data->set.rtspreq;
  ଅଚଳ ଅକ୍ଷର *str;
  ସଂରଚନା RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  ସଂରଚନା dynbuf req_buffer;
  ସଂରଚନା rtsp_blocks block;
  memset(&block, 0, ଆକାର(block));

  *done = TRUE;
  ଯଦି(!rtsp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    ଯାଅ out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = ଖାଲି;

  /*
   * Sanity check the custom headers
   */
  ଯଦି(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    ଯାଅ out;
  }
  ଯଦି(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  ଯଦି(result)
    ଯାଅ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ଯଦି(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    ଯଦି(result)
      ଯାଅ out;
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
  ଯଦି(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  ଯଦି(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    ଯଦି(result)
      ଯାଅ out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  ଯଦି(result)
    ଯାଅ out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  ଯଦି(result)
    ଯାଅ out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  ଯଦି(result)
    ଯାଅ out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  ଯଦି(result) {
    failf(data, "Failed sending RTSP request");
    ଯାଅ out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  ଯଦି(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  ଯଦି(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  ଫେରନ୍ତୁ result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ସ୍ଥିର CURLcode rtp_write_body_junk(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା rtsp_conn *rtspc,
                                    ଅଚଳ ଅକ୍ଷର *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ଯଦି(body_remain) {
    ଯଦି((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ଫେରନ୍ତୁ Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode rtp_client_write(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ptr,
                                 size_t len)
{
  ସଂରଚନା Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  ଶୂନ୍ୟ *user_ptr;

  ଯଦି(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ଯଦି(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  ଅନ୍ୟଥା {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((ଅକ୍ଷର *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  ଯଦି(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }

  ଯଦି(wrote != len) {
    failf(data, "Failed writing RTP data");
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode rtsp_filter_rtp(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା rtsp_conn *rtspc,
                                ଅଚଳ ଅକ୍ଷର *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ଯେତେବେଳେ(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    ବାଛ(rtspc->state) {

    ସ୍ଥିତି RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ଯେତେବେଳେ(blen && buf[0] != '$') {
        ଯଦି(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          ଯଦି(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ଯଦି(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            ଯାଅ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ଯଦି(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ଯଦି(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ଯଦି(result)
            ଯାଅ out;
        }
        ଯଦି(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          ଯାଅ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି RTP_PARSE_CHANNEL: {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = ((unsigned ଅକ୍ଷର)buf[0]) / 8;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ((unsigned ଅକ୍ଷର)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ଯଦି(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        ଯଦି(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ଯଦି(result)
            ଯାଅ out;
        }
        ଅନ୍ୟଥା {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ଭାଙ୍ଗ;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned ଅକ୍ଷର)buf[0];
      ଯଦି(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ଅଚଳ ଅକ୍ଷର *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ଯଦି(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ଯଦି(rtp_len == 2)
        ଭାଙ୍ଗ;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      ଯଦି(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ଯଦି(result)
          ଯାଅ out;
        ଭାଙ୍ଗ;
      }
      rtspc->state = RTP_PARSE_DATA;
      ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ଯଦି(needed <= blen) {
        ଯଦି(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          ଯାଅ out;
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
        ଯଦି(result)
          ଯାଅ out;
      }
      ଅନ୍ୟଥା {
        ଯଦି(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          ଯାଅ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ଭାଙ୍ଗ;
    }

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      DEBUGASSERT(0);
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
  }
out:
  ଯଦି(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode rtsp_rtp_write_resp(ସଂରଚନା Curl_easy *data,
                                    ଅଚଳ ଅକ୍ଷର *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ସଂରଚନା rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  ଯଦି(!rtspc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(!data->req.header)
    rtspc->in_header = FALSE;
  ଯଦି(!blen) {
    ଯାଅ out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ଯଦି(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    ଯଦି(result)
      ଯାଅ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ଯଦି(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ଯଦି(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    ଯଦି(result)
      ଯାଅ out;

    buf += consumed;
    blen -= consumed;

    ଯଦି(!data->req.header)
      rtspc->in_header = FALSE;

    ଯଦି(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      ଯଦି(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      ଯଦି(result)
        ଯାଅ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  ଯଦି(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rtspc->state,
               data->req.size));
  ଯଦି(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  ଯଦି((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode rtsp_rtp_write_resp_hd(ସଂରଚନା Curl_easy *data,
                                       ଅଚଳ ଅକ୍ଷର *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ଫେରନ୍ତୁ rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

ସ୍ଥିର CURLcode rtsp_parse_transport(ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ ଅକ୍ଷର *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ଅଚଳ ଅକ୍ଷର *start, *end;
  start = transport;
  ଯେତେବେଳେ(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    ଯଦି(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ଅଚଳ ଅକ୍ଷର *p = start + 12;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &chan1, 255)) {
        unsigned ଅକ୍ଷର *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        ଯଦି(!curlx_str_single(&p, '-') && curlx_str_ସଂଖ୍ୟା(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        ପାଇଁ(chan = chan1; chan <= chan2; chan++) {
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)chan / 8;
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)chan % 8;
          rtp_channel_mask[idx] |= (unsigned ଅକ୍ଷର)(1 << off);
        }
      }
      ଅନ୍ୟଥା {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ଭାଙ୍ଗ;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *header)
{
  ଯଦି(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    ସଂରଚନା RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    ଅଚଳ ଅକ୍ଷର *p = &header[5];
    ଯଦି(!rtsp)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      ଫେରନ୍ତୁ CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ଅନ୍ୟଥା ଯଦି(checkprefix("Session:", header)) {
    ଅଚଳ ଅକ୍ଷର *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    ଯଦି(!*start) {
      failf(data, "Got a blank Session ID");
      ଫେରନ୍ତୁ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    ଯେତେବେଳେ((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    ଯଦି(str) {

      /* If the Session ID is set, then compare */
      ଯଦି(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        ଫେରନ୍ତୁ CURLE_RTSP_SESSION_ERROR;
      }
    }
    ଅନ୍ୟଥା {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      ଶୂନ୍ୟ *mem = curlx_memdup0(start, idlen);
      ଯଦି(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  ଅନ୍ୟଥା ଯଦି(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_rtsp = {
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
