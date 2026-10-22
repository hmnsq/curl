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
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

প্রকারনির্দেশ গণনা {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
সংরচনা rtsp_conn {
  সংরচনা dynbuf buf;
  পূর্ণাঙ্ক rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
সংরচনা RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned পূর্ণাঙ্ক)((unsigned বর্ণ)((p)[2]))) << 8) | \
                            ((unsigned পূর্ণাঙ্ক)((unsigned বর্ণ)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
স্থির CURLcode rtsp_do_pollset(সংরচনা Curl_easy *data,
                                সংরচনা easy_pollset *ps)
{
  /* write mode */
  প্রত্যাগম Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

স্থির শূন্য rtsp_easy_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা RTSP *rtsp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(rtsp);
}

স্থির শূন্য rtsp_conn_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা rtsp_conn *rtspc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

স্থির CURLcode rtsp_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা rtsp_conn *rtspc;
  সংরচনা RTSP *rtsp;

  rtspc = curlx_calloc(1, পরিমাণ(*rtspc));
  যদি(!rtspc)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  যদি(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, পরিমাণ(সংরচনা RTSP));
  যদি(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  প্রত্যাগম CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
স্থির bool rtsp_conn_is_dead(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn)
{
  bool প্রবেশ_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  প্রত্যাগম !Curl_conn_is_alive(data, conn, &প্রবেশ_pending);
}

স্থির CURLcode rtsp_connect(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  যদি(!rtspc)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  যদি(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  যদি(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode rtsp_done(সংরচনা Curl_easy *data,
                          CURLcode status, bool premature)
{
  সংরচনা rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  সংরচনা RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  যদি(!rtspc || !rtsp)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  যদি(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  যদি(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    যদি((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      প্রত্যাগম CURLE_RTSP_CSEQ_ERROR;
    }
    যদি(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    যদি(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      প্রত্যাগম CURLE_RECV_ERROR;
    }
  }

  প্রত্যাগম result;
}

স্থির CURLcode rtsp_setup_body(সংরচনা Curl_easy *data,
                                unsigned বর্ণ rtspreq,
                                সংরচনা dynbuf *reqp)
{
  CURLcode result;
  যদি(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    যদি(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      যদি(result)
        প্রত্যাগম result;
    }
    অন্যথা {
      যদি(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      অন্যথা যদি(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      অন্যথা {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      যদি(result)
        প্রত্যাগম result;
    }

    যদি(req_clen > 0) {
      যদি(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        প্রত্যাগম CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      যদি(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        যদি(result)
          প্রত্যাগম result;
      }

      যদি(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        যদি(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          যদি(result)
            প্রত্যাগম result;
        }
      }

      যদি(rtspreq == RTSPREQ_ANNOUNCE) {
        যদি(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          যদি(result)
            প্রত্যাগম result;
        }
      }
    }
    অন্যথা যদি(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  অন্যথা
    result = Curl_creader_set_null(data);
  প্রত্যাগম result;
}

সংরচনা rtspselect {
  ধ্রুব বর্ণ *method;
  bool no_body;
};

স্থির CURLcode pick_method(সংরচনা Curl_easy *data,
                            ধ্রুব unsigned বর্ণ rtspreq,
                            ধ্রুব বর্ণ **p)
{
  স্থির ধ্রুব সংরচনা rtspselect req[] = {
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
  যদি((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  প্রত্যাগম CURLE_OK;
}

/* Allocate and store a header string. */
স্থির CURLcode rtsp_header_alloc(ধ্রুব বর্ণ *header_name,
                                  ধ্রুব বর্ণ *মান,
                                  বর্ণ **target)
{
  যদি(!মান)
    প্রত্যাগম CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, মান);
  যদি(!*target)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  প্রত্যাগম CURLE_OK;
}

সংরচনা rtsp_blocks {
  ধ্রুব বর্ণ *request;
  ধ্রুব বর্ণ *session_id;
  ধ্রুব বর্ণ *accept;
  ধ্রুব বর্ণ *range;
  ধ্রুব বর্ণ *stream_uri;
  ধ্রুব বর্ণ *hd_proxy_auth;
  ধ্রুব বর্ণ *hd_auth;
  বর্ণ *referrer;
  বর্ণ *accept_encoding;
  বর্ণ *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

স্থির CURLcode rtsp_setup_request(সংরচনা Curl_easy *data,
                                   সংরচনা rtsp_blocks *b,
                                   ধ্রুব unsigned বর্ণ rtspreq)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  যদি(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  অন্যথা
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  যদি(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    যদি(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      যদি(result)
        প্রত্যাগম result;
      b->transport_alloc = TRUE;
    }
    অন্যথা {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  যদি(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      রিক্ত : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    যদি(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      যদি(result)
        প্রত্যাগম result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, রিক্ত, FALSE);
  যদি(result)
    প্রত্যাগম result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  যদি(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    যদি(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  যদি(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    যদি(!result)
      b->range = data->state.rangeline;
  }
  প্রত্যাগম result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

স্থির CURLcode rtsp_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ধ্রুব unsigned বর্ণ rtspreq = data->set.rtspreq;
  ধ্রুব বর্ণ *str;
  সংরচনা RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  সংরচনা dynbuf req_buffer;
  সংরচনা rtsp_blocks block;
  memset(&block, 0, পরিমাণ(block));

  *done = TRUE;
  যদি(!rtsp)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  যদি(result)
    গচ্ছ out;

  যদি(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    গচ্ছ out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  যদি(result)
    গচ্ছ out;

  যদি(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = রিক্ত;

  /*
   * Sanity check the custom headers
   */
  যদি(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    গচ্ছ out;
  }
  যদি(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  যদি(result)
    গচ্ছ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  যদি(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    যদি(result)
      গচ্ছ out;
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
  যদি(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  যদি(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  যদি(result)
    গচ্ছ out;

  যদি((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    যদি(result)
      গচ্ছ out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  যদি(result)
    গচ্ছ out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  যদি(result)
    গচ্ছ out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  যদি(result)
    গচ্ছ out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  যদি(result) {
    failf(data, "Failed sending RTSP request");
    গচ্ছ out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  যদি(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  যদি(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  প্রত্যাগম result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
স্থির CURLcode rtp_write_body_junk(সংরচনা Curl_easy *data,
                                    সংরচনা rtsp_conn *rtspc,
                                    ধ্রুব বর্ণ *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  যদি(body_remain) {
    যদি((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    প্রত্যাগম Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode rtp_client_write(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *ptr,
                                 size_t len)
{
  সংরচনা Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  শূন্য *user_ptr;

  যদি(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    প্রত্যাগম CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  যদি(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  অন্যথা {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((বর্ণ *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  যদি(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    প্রত্যাগম CURLE_WRITE_ERROR;
  }

  যদি(wrote != len) {
    failf(data, "Failed writing RTP data");
    প্রত্যাগম CURLE_WRITE_ERROR;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode rtsp_filter_rtp(সংরচনা Curl_easy *data,
                                সংরচনা rtsp_conn *rtspc,
                                ধ্রুব বর্ণ *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  যাবত্(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    বিকল্প(rtspc->state) {

    পক্ষ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      যাবত্(blen && buf[0] != '$') {
        যদি(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          যদি(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            যদি(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            গচ্ছ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      যদি(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        যদি(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          যদি(result)
            গচ্ছ out;
        }
        যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          গচ্ছ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ভঙ্গ;
    }

    পক্ষ RTP_PARSE_CHANNEL: {
      পূর্ণাঙ্ক idx = ((unsigned বর্ণ)buf[0]) / 8;
      পূর্ণাঙ্ক off = ((unsigned বর্ণ)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      যদি(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        যদি(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          যদি(result)
            গচ্ছ out;
        }
        অন্যথা {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ভঙ্গ;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned বর্ণ)buf[0];
      যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ভঙ্গ;
    }

    পক্ষ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ধ্রুব বর্ণ *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      যদি(rtp_len == 2)
        ভঙ্গ;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      যদি(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        যদি(result)
          গচ্ছ out;
        ভঙ্গ;
      }
      rtspc->state = RTP_PARSE_DATA;
      ভঙ্গ;
    }

    পক্ষ RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      যদি(needed <= blen) {
        যদি(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          গচ্ছ out;
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
        যদি(result)
          গচ্ছ out;
      }
      অন্যথা {
        যদি(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          গচ্ছ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ভঙ্গ;
    }

    স্বভাব:
      DEBUGASSERT(0);
      প্রত্যাগম CURLE_RECV_ERROR;
    }
  }
out:
  যদি(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  প্রত্যাগম result;
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
স্থির CURLcode rtsp_rtp_write_resp(সংরচনা Curl_easy *data,
                                    ধ্রুব বর্ণ *buf,
                                    size_t blen,
                                    bool is_eos)
{
  সংরচনা rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  যদি(!rtspc)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(!data->req.header)
    rtspc->in_header = FALSE;
  যদি(!blen) {
    গচ্ছ out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  যদি(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    যদি(result)
      গচ্ছ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    যদি(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  যদি(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    যদি(result)
      গচ্ছ out;

    buf += consumed;
    blen -= consumed;

    যদি(!data->req.header)
      rtspc->in_header = FALSE;

    যদি(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      যদি(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      যদি(result)
        গচ্ছ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  যদি(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (পূর্ণাঙ্ক)rtspc->state,
               data->req.size));
  যদি(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  যদি((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  প্রত্যাগম result;
}

স্থির CURLcode rtsp_rtp_write_resp_hd(সংরচনা Curl_easy *data,
                                       ধ্রুব বর্ণ *buf,
                                       size_t blen,
                                       bool is_eos)
{
  প্রত্যাগম rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

স্থির CURLcode rtsp_parse_transport(সংরচনা Curl_easy *data,
                                     ধ্রুব বর্ণ *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ধ্রুব বর্ণ *start, *end;
  start = transport;
  যাবত্(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    যদি(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ধ্রুব বর্ণ *p = start + 12;
      যদি(!curlx_str_সংখ্যা(&p, &chan1, 255)) {
        unsigned বর্ণ *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        যদি(!curlx_str_single(&p, '-') && curlx_str_সংখ্যা(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        প্রতি(chan = chan1; chan <= chan2; chan++) {
          পূর্ণাঙ্ক idx = (পূর্ণাঙ্ক)chan / 8;
          পূর্ণাঙ্ক off = (পূর্ণাঙ্ক)chan % 8;
          rtp_channel_mask[idx] |= (unsigned বর্ণ)(1 << off);
        }
      }
      অন্যথা {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ভঙ্গ;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *header)
{
  যদি(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    সংরচনা RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    ধ্রুব বর্ণ *p = &header[5];
    যদি(!rtsp)
      প্রত্যাগম CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    যদি(curlx_str_সংখ্যা(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      প্রত্যাগম CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  অন্যথা যদি(checkprefix("Session:", header)) {
    ধ্রুব বর্ণ *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    যদি(!*start) {
      failf(data, "Got a blank Session ID");
      প্রত্যাগম CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    যাবত্((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    যদি(str) {

      /* If the Session ID is set, then compare */
      যদি(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        প্রত্যাগম CURLE_RTSP_SESSION_ERROR;
      }
    }
    অন্যথা {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      শূন্য *mem = curlx_memdup0(start, idlen);
      যদি(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  অন্যথা যদি(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ধ্রুব সংরচনা Curl_protocol Curl_protocol_rtsp = {
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
