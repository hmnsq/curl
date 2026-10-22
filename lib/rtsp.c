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

#ifndef CURL_DISABLE_RTSP

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
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

تعریفنوع شمارش {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ساختار rtsp_conn {
  ساختار dynbuf buf;
  صحیح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
ساختار RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned صحیح)((unsigned نویسه)((p)[2]))) << 8) | \
                            ((unsigned صحیح)((unsigned نویسه)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ایستا CURLcode rtsp_do_pollset(ساختار Curl_easy *data,
                                ساختار easy_pollset *ps)
{
  /* write mode */
  بازگشت Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ایستا تهی rtsp_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار RTSP *rtsp = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_free(rtsp);
}

ایستا تهی rtsp_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار rtsp_conn *rtspc = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ایستا CURLcode rtsp_setup_connection(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار rtsp_conn *rtspc;
  ساختار RTSP *rtsp;

  rtspc = curlx_calloc(1, اندازه(*rtspc));
  اگر(!rtspc)
    بازگشت CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  اگر(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, اندازه(ساختار RTSP));
  اگر(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ایستا bool rtsp_conn_is_dead(ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  bool درون_داد_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  بازگشت !Curl_conn_is_alive(data, conn, &درون_داد_pending);
}

ایستا CURLcode rtsp_connect(ساختار Curl_easy *data, bool *done)
{
  ساختار rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  اگر(!rtspc)
    بازگشت CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  اگر(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  اگر(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  بازگشت CURLE_OK;
}

ایستا CURLcode rtsp_done(ساختار Curl_easy *data,
                          CURLcode status, bool premature)
{
  ساختار rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  ساختار RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  اگر(!rtspc || !rtsp)
    بازگشت CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  اگر(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  اگر(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    اگر((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      بازگشت CURLE_RTSP_CSEQ_ERROR;
    }
    اگر(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    اگر(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      بازگشت CURLE_RECV_ERROR;
    }
  }

  بازگشت result;
}

ایستا CURLcode rtsp_setup_body(ساختار Curl_easy *data,
                                unsigned نویسه rtspreq,
                                ساختار dynbuf *reqp)
{
  CURLcode result;
  اگر(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    اگر(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      اگر(result)
        بازگشت result;
    }
    وگرنه {
      اگر(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      وگرنه اگر(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      وگرنه {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      اگر(result)
        بازگشت result;
    }

    اگر(req_clen > 0) {
      اگر(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        بازگشت CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      اگر(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        اگر(result)
          بازگشت result;
      }

      اگر(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        اگر(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          اگر(result)
            بازگشت result;
        }
      }

      اگر(rtspreq == RTSPREQ_ANNOUNCE) {
        اگر(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          اگر(result)
            بازگشت result;
        }
      }
    }
    وگرنه اگر(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  وگرنه
    result = Curl_creader_set_null(data);
  بازگشت result;
}

ساختار rtspselect {
  ثابت نویسه *method;
  bool no_body;
};

ایستا CURLcode pick_method(ساختار Curl_easy *data,
                            ثابت unsigned نویسه rtspreq,
                            ثابت نویسه **p)
{
  ایستا ثابت ساختار rtspselect req[] = {
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
  اگر((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  بازگشت CURLE_OK;
}

/* Allocate and store a header string. */
ایستا CURLcode rtsp_header_alloc(ثابت نویسه *header_name,
                                  ثابت نویسه *مقدار,
                                  نویسه **target)
{
  اگر(!مقدار)
    بازگشت CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, مقدار);
  اگر(!*target)
    بازگشت CURLE_OUT_OF_MEMORY;
  بازگشت CURLE_OK;
}

ساختار rtsp_blocks {
  ثابت نویسه *request;
  ثابت نویسه *session_id;
  ثابت نویسه *accept;
  ثابت نویسه *range;
  ثابت نویسه *stream_uri;
  ثابت نویسه *hd_proxy_auth;
  ثابت نویسه *hd_auth;
  نویسه *referrer;
  نویسه *accept_encoding;
  نویسه *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

ایستا CURLcode rtsp_setup_request(ساختار Curl_easy *data,
                                   ساختار rtsp_blocks *b,
                                   ثابت unsigned نویسه rtspreq)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  اگر(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  وگرنه
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  اگر(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    اگر(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      اگر(result)
        بازگشت result;
      b->transport_alloc = TRUE;
    }
    وگرنه {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  اگر(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      پوچ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    اگر(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      اگر(result)
        بازگشت result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, پوچ, FALSE);
  اگر(result)
    بازگشت result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  اگر(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    اگر(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  اگر(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    اگر(!result)
      b->range = data->state.rangeline;
  }
  بازگشت result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ایستا CURLcode rtsp_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ثابت unsigned نویسه rtspreq = data->set.rtspreq;
  ثابت نویسه *str;
  ساختار RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  ساختار dynbuf req_buffer;
  ساختار rtsp_blocks block;
  memset(&block, 0, اندازه(block));

  *done = TRUE;
  اگر(!rtsp)
    بازگشت CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  اگر(result)
    برو out;

  اگر(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    برو out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  اگر(result)
    برو out;

  اگر(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = پوچ;

  /*
   * Sanity check the custom headers
   */
  اگر(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    برو out;
  }
  اگر(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  اگر(result)
    برو out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  اگر(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    اگر(result)
      برو out;
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
  اگر(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  اگر(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  اگر(result)
    برو out;

  اگر((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    اگر(result)
      برو out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  اگر(result)
    برو out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  اگر(result)
    برو out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  اگر(result)
    برو out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  اگر(result) {
    failf(data, "Failed sending RTSP request");
    برو out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  اگر(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  اگر(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  بازگشت result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ایستا CURLcode rtp_write_body_junk(ساختار Curl_easy *data,
                                    ساختار rtsp_conn *rtspc,
                                    ثابت نویسه *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  اگر(body_remain) {
    اگر((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    بازگشت Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode rtp_client_write(ساختار Curl_easy *data, ثابت نویسه *ptr,
                                 size_t len)
{
  ساختار Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  تهی *user_ptr;

  اگر(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    بازگشت CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  اگر(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  وگرنه {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((نویسه *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  اگر(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    بازگشت CURLE_WRITE_ERROR;
  }

  اگر(wrote != len) {
    failf(data, "Failed writing RTP data");
    بازگشت CURLE_WRITE_ERROR;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode rtsp_filter_rtp(ساختار Curl_easy *data,
                                ساختار rtsp_conn *rtspc,
                                ثابت نویسه *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  وقتی(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    انتخاب(rtspc->state) {

    حالت RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      وقتی(blen && buf[0] != '$') {
        اگر(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          اگر(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            اگر(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            برو out;
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
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          اگر(result)
            برو out;
        }
        اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          برو out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      بشکن;
    }

    حالت RTP_PARSE_CHANNEL: {
      صحیح idx = ((unsigned نویسه)buf[0]) / 8;
      صحیح off = ((unsigned نویسه)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      اگر(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        اگر(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          اگر(result)
            برو out;
        }
        وگرنه {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        بشکن;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned نویسه)buf[0];
      اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      بشکن;
    }

    حالت RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ثابت نویسه *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      اگر(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      اگر(rtp_len == 2)
        بشکن;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      اگر(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        اگر(result)
          برو out;
        بشکن;
      }
      rtspc->state = RTP_PARSE_DATA;
      بشکن;
    }

    حالت RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      اگر(needed <= blen) {
        اگر(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          برو out;
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
        اگر(result)
          برو out;
      }
      وگرنه {
        اگر(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          برو out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      بشکن;
    }

    پیشفرض:
      DEBUGASSERT(0);
      بازگشت CURLE_RECV_ERROR;
    }
  }
out:
  اگر(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  بازگشت result;
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
ایستا CURLcode rtsp_rtp_write_resp(ساختار Curl_easy *data,
                                    ثابت نویسه *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ساختار rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  اگر(!rtspc)
    بازگشت CURLE_FAILED_INIT;

  اگر(!data->req.header)
    rtspc->in_header = FALSE;
  اگر(!blen) {
    برو out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  اگر(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    اگر(result)
      برو out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    اگر(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  اگر(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    اگر(result)
      برو out;

    buf += consumed;
    blen -= consumed;

    اگر(!data->req.header)
      rtspc->in_header = FALSE;

    اگر(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      اگر(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      اگر(result)
        برو out;
      buf += consumed;
      blen -= consumed;
    }
  }

  اگر(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (صحیح)rtspc->state,
               data->req.size));
  اگر(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  اگر((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  بازگشت result;
}

ایستا CURLcode rtsp_rtp_write_resp_hd(ساختار Curl_easy *data,
                                       ثابت نویسه *buf,
                                       size_t blen,
                                       bool is_eos)
{
  بازگشت rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

ایستا CURLcode rtsp_parse_transport(ساختار Curl_easy *data,
                                     ثابت نویسه *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت نویسه *start, *end;
  start = transport;
  وقتی(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    اگر(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ثابت نویسه *p = start + 12;
      اگر(!curlx_str_عدد(&p, &chan1, 255)) {
        unsigned نویسه *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        اگر(!curlx_str_single(&p, '-') && curlx_str_عدد(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        برای(chan = chan1; chan <= chan2; chan++) {
          صحیح idx = (صحیح)chan / 8;
          صحیح off = (صحیح)chan % 8;
          rtp_channel_mask[idx] |= (unsigned نویسه)(1 << off);
        }
      }
      وگرنه {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      بشکن;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ساختار Curl_easy *data, ثابت نویسه *header)
{
  اگر(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    ساختار RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    ثابت نویسه *p = &header[5];
    اگر(!rtsp)
      بازگشت CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    اگر(curlx_str_عدد(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      بازگشت CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  وگرنه اگر(checkprefix("Session:", header)) {
    ثابت نویسه *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    اگر(!*start) {
      failf(data, "Got a blank Session ID");
      بازگشت CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    وقتی((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    اگر(str) {

      /* If the Session ID is set, then compare */
      اگر(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        بازگشت CURLE_RTSP_SESSION_ERROR;
      }
    }
    وگرنه {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      تهی *mem = curlx_memdup0(start, idlen);
      اگر(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        بازگشت CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  وگرنه اگر(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    اگر(result)
      بازگشت result;
  }
  بازگشت CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت ساختار Curl_protocol Curl_protocol_rtsp = {
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
