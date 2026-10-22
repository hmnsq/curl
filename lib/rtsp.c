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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

வகைவரை எண்ணிக்கை {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
கட்டமைப்பு rtsp_conn {
  கட்டமைப்பு dynbuf buf;
  முழு rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
கட்டமைப்பு RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned முழு)((unsigned எழுத்து)((p)[2]))) << 8) | \
                            ((unsigned முழு)((unsigned எழுத்து)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
நிலையான CURLcode rtsp_do_pollset(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு easy_pollset *ps)
{
  /* write mode */
  திரும்பு Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

நிலையான வெறுமை rtsp_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு RTSP *rtsp = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_free(rtsp);
}

நிலையான வெறுமை rtsp_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு rtsp_conn *rtspc = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

நிலையான CURLcode rtsp_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு rtsp_conn *rtspc;
  கட்டமைப்பு RTSP *rtsp;

  rtspc = curlx_calloc(1, அளவு(*rtspc));
  எனில்(!rtspc)
    திரும்பு CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  எனில்(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, அளவு(கட்டமைப்பு RTSP));
  எனில்(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
நிலையான bool rtsp_conn_is_dead(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  bool உள்ளீடு_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  திரும்பு !Curl_conn_is_alive(data, conn, &உள்ளீடு_pending);
}

நிலையான CURLcode rtsp_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  எனில்(!rtspc)
    திரும்பு CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  எனில்(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  எனில்(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode rtsp_done(கட்டமைப்பு Curl_easy *data,
                          CURLcode status, bool premature)
{
  கட்டமைப்பு rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  கட்டமைப்பு RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  எனில்(!rtspc || !rtsp)
    திரும்பு CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  எனில்(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  எனில்(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    எனில்((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      திரும்பு CURLE_RTSP_CSEQ_ERROR;
    }
    எனில்(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    எனில்(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      திரும்பு CURLE_RECV_ERROR;
    }
  }

  திரும்பு result;
}

நிலையான CURLcode rtsp_setup_body(கட்டமைப்பு Curl_easy *data,
                                unsigned எழுத்து rtspreq,
                                கட்டமைப்பு dynbuf *reqp)
{
  CURLcode result;
  எனில்(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    எனில்(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில் {
      எனில்(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      இல்லையெனில் எனில்(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      இல்லையெனில் {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      எனில்(result)
        திரும்பு result;
    }

    எனில்(req_clen > 0) {
      எனில்(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        திரும்பு CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      எனில்(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        எனில்(result)
          திரும்பு result;
      }

      எனில்(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        எனில்(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          எனில்(result)
            திரும்பு result;
        }
      }

      எனில்(rtspreq == RTSPREQ_ANNOUNCE) {
        எனில்(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          எனில்(result)
            திரும்பு result;
        }
      }
    }
    இல்லையெனில் எனில்(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  இல்லையெனில்
    result = Curl_creader_set_null(data);
  திரும்பு result;
}

கட்டமைப்பு rtspselect {
  மாறிலி எழுத்து *method;
  bool no_body;
};

நிலையான CURLcode pick_method(கட்டமைப்பு Curl_easy *data,
                            மாறிலி unsigned எழுத்து rtspreq,
                            மாறிலி எழுத்து **p)
{
  நிலையான மாறிலி கட்டமைப்பு rtspselect req[] = {
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
  எனில்((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  திரும்பு CURLE_OK;
}

/* Allocate and store a header string. */
நிலையான CURLcode rtsp_header_alloc(மாறிலி எழுத்து *header_name,
                                  மாறிலி எழுத்து *மதிப்பு,
                                  எழுத்து **target)
{
  எனில்(!மதிப்பு)
    திரும்பு CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, மதிப்பு);
  எனில்(!*target)
    திரும்பு CURLE_OUT_OF_MEMORY;
  திரும்பு CURLE_OK;
}

கட்டமைப்பு rtsp_blocks {
  மாறிலி எழுத்து *request;
  மாறிலி எழுத்து *session_id;
  மாறிலி எழுத்து *accept;
  மாறிலி எழுத்து *range;
  மாறிலி எழுத்து *stream_uri;
  மாறிலி எழுத்து *hd_proxy_auth;
  மாறிலி எழுத்து *hd_auth;
  எழுத்து *referrer;
  எழுத்து *accept_encoding;
  எழுத்து *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

நிலையான CURLcode rtsp_setup_request(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு rtsp_blocks *b,
                                   மாறிலி unsigned எழுத்து rtspreq)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  எனில்(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  இல்லையெனில்
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  எனில்(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    எனில்(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      எனில்(result)
        திரும்பு result;
      b->transport_alloc = TRUE;
    }
    இல்லையெனில் {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  எனில்(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      வெற்று : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    எனில்(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      எனில்(result)
        திரும்பு result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, வெற்று, FALSE);
  எனில்(result)
    திரும்பு result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  எனில்(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    எனில்(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  எனில்(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    எனில்(!result)
      b->range = data->state.rangeline;
  }
  திரும்பு result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

நிலையான CURLcode rtsp_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  மாறிலி unsigned எழுத்து rtspreq = data->set.rtspreq;
  மாறிலி எழுத்து *str;
  கட்டமைப்பு RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  கட்டமைப்பு dynbuf req_buffer;
  கட்டமைப்பு rtsp_blocks block;
  memset(&block, 0, அளவு(block));

  *done = TRUE;
  எனில்(!rtsp)
    திரும்பு CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  எனில்(result)
    செல் out;

  எனில்(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    செல் out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  எனில்(result)
    செல் out;

  எனில்(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = வெற்று;

  /*
   * Sanity check the custom headers
   */
  எனில்(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    செல் out;
  }
  எனில்(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  எனில்(result)
    செல் out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  எனில்(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    எனில்(result)
      செல் out;
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
  எனில்(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  எனில்(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  எனில்(result)
    செல் out;

  எனில்((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    எனில்(result)
      செல் out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  எனில்(result)
    செல் out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  எனில்(result)
    செல் out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  எனில்(result)
    செல் out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  எனில்(result) {
    failf(data, "Failed sending RTSP request");
    செல் out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  எனில்(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  எனில்(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  திரும்பு result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
நிலையான CURLcode rtp_write_body_junk(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு rtsp_conn *rtspc,
                                    மாறிலி எழுத்து *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  எனில்(body_remain) {
    எனில்((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    திரும்பு Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode rtp_client_write(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *ptr,
                                 size_t len)
{
  கட்டமைப்பு Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  வெறுமை *user_ptr;

  எனில்(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    திரும்பு CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  எனில்(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  இல்லையெனில் {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((எழுத்து *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  எனில்(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    திரும்பு CURLE_WRITE_ERROR;
  }

  எனில்(wrote != len) {
    failf(data, "Failed writing RTP data");
    திரும்பு CURLE_WRITE_ERROR;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode rtsp_filter_rtp(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு rtsp_conn *rtspc,
                                மாறிலி எழுத்து *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  போது(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    தேர்வு(rtspc->state) {

    நிலை RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      போது(blen && buf[0] != '$') {
        எனில்(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          எனில்(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            எனில்(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            செல் out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      எனில்(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        எனில்(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          எனில்(result)
            செல் out;
        }
        எனில்(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          செல் out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      நிறுத்து;
    }

    நிலை RTP_PARSE_CHANNEL: {
      முழு idx = ((unsigned எழுத்து)buf[0]) / 8;
      முழு off = ((unsigned எழுத்து)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      எனில்(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        எனில்(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          எனில்(result)
            செல் out;
        }
        இல்லையெனில் {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        நிறுத்து;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned எழுத்து)buf[0];
      எனில்(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      நிறுத்து;
    }

    நிலை RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      மாறிலி எழுத்து *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      எனில்(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      எனில்(rtp_len == 2)
        நிறுத்து;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      எனில்(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        எனில்(result)
          செல் out;
        நிறுத்து;
      }
      rtspc->state = RTP_PARSE_DATA;
      நிறுத்து;
    }

    நிலை RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      எனில்(needed <= blen) {
        எனில்(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          செல் out;
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
        எனில்(result)
          செல் out;
      }
      இல்லையெனில் {
        எனில்(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          செல் out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      நிறுத்து;
    }

    இயல்பு:
      DEBUGASSERT(0);
      திரும்பு CURLE_RECV_ERROR;
    }
  }
out:
  எனில்(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  திரும்பு result;
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
நிலையான CURLcode rtsp_rtp_write_resp(கட்டமைப்பு Curl_easy *data,
                                    மாறிலி எழுத்து *buf,
                                    size_t blen,
                                    bool is_eos)
{
  கட்டமைப்பு rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  எனில்(!rtspc)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(!data->req.header)
    rtspc->in_header = FALSE;
  எனில்(!blen) {
    செல் out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  எனில்(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    எனில்(result)
      செல் out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    எனில்(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  எனில்(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    எனில்(result)
      செல் out;

    buf += consumed;
    blen -= consumed;

    எனில்(!data->req.header)
      rtspc->in_header = FALSE;

    எனில்(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      எனில்(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      எனில்(result)
        செல் out;
      buf += consumed;
      blen -= consumed;
    }
  }

  எனில்(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (முழு)rtspc->state,
               data->req.size));
  எனில்(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  எனில்((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  திரும்பு result;
}

நிலையான CURLcode rtsp_rtp_write_resp_hd(கட்டமைப்பு Curl_easy *data,
                                       மாறிலி எழுத்து *buf,
                                       size_t blen,
                                       bool is_eos)
{
  திரும்பு rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

நிலையான CURLcode rtsp_parse_transport(கட்டமைப்பு Curl_easy *data,
                                     மாறிலி எழுத்து *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  மாறிலி எழுத்து *start, *end;
  start = transport;
  போது(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    எனில்(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      மாறிலி எழுத்து *p = start + 12;
      எனில்(!curlx_str_எண்(&p, &chan1, 255)) {
        unsigned எழுத்து *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        எனில்(!curlx_str_single(&p, '-') && curlx_str_எண்(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        க்கு(chan = chan1; chan <= chan2; chan++) {
          முழு idx = (முழு)chan / 8;
          முழு off = (முழு)chan % 8;
          rtp_channel_mask[idx] |= (unsigned எழுத்து)(1 << off);
        }
      }
      இல்லையெனில் {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      நிறுத்து;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *header)
{
  எனில்(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    கட்டமைப்பு RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    மாறிலி எழுத்து *p = &header[5];
    எனில்(!rtsp)
      திரும்பு CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    எனில்(curlx_str_எண்(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      திரும்பு CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  இல்லையெனில் எனில்(checkprefix("Session:", header)) {
    மாறிலி எழுத்து *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    எனில்(!*start) {
      failf(data, "Got a blank Session ID");
      திரும்பு CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    போது((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    எனில்(str) {

      /* If the Session ID is set, then compare */
      எனில்(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        திரும்பு CURLE_RTSP_SESSION_ERROR;
      }
    }
    இல்லையெனில் {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      வெறுமை *mem = curlx_memdup0(start, idlen);
      எனில்(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        திரும்பு CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  இல்லையெனில் எனில்(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

/*
 * RTSP handler interface.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_rtsp = {
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
