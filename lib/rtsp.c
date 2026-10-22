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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

प्रकारनिर्देश गणना {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
संरचना rtsp_conn {
  संरचना dynbuf buf;
  पूर्णाङ्क rtp_channel;
  माप_प्रकार rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
संरचना RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((अचिन्हित पूर्णाङ्क)((अचिन्हित वर्ण)((p)[2]))) << 8) | \
                            ((अचिन्हित पूर्णाङ्क)((अचिन्हित वर्ण)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
स्थिर CURLcode rtsp_do_pollset(संरचना Curl_easy *data,
                                संरचना easy_pollset *ps)
{
  /* write mode */
  फर्काऊ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

स्थिर शून्य rtsp_easy_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना RTSP *rtsp = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  curlx_free(rtsp);
}

स्थिर शून्य rtsp_conn_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना rtsp_conn *rtspc = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

स्थिर CURLcode rtsp_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना rtsp_conn *rtspc;
  संरचना RTSP *rtsp;

  rtspc = curlx_calloc(1, परिमाण(*rtspc));
  यदि(!rtspc)
    फर्काऊ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  यदि(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    फर्काऊ CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, परिमाण(संरचना RTSP));
  यदि(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    फर्काऊ CURLE_OUT_OF_MEMORY;

  फर्काऊ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
स्थिर bool rtsp_conn_is_dead(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  bool आगत_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  फर्काऊ !Curl_conn_is_alive(data, conn, &आगत_pending);
}

स्थिर CURLcode rtsp_connect(संरचना Curl_easy *data, bool *done)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  यदि(!rtspc)
    फर्काऊ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  यदि(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  यदि(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode rtsp_done(संरचना Curl_easy *data,
                          CURLcode status, bool premature)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  संरचना RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  यदि(!rtspc || !rtsp)
    फर्काऊ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  यदि(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  यदि(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    यदि((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      फर्काऊ CURLE_RTSP_CSEQ_ERROR;
    }
    यदि(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    यदि(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      फर्काऊ CURLE_RECV_ERROR;
    }
  }

  फर्काऊ result;
}

स्थिर CURLcode rtsp_setup_body(संरचना Curl_easy *data,
                                अचिन्हित वर्ण rtspreq,
                                संरचना dynbuf *reqp)
{
  CURLcode result;
  यदि(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    यदि(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      यदि(result)
        फर्काऊ result;
    }
    अन्यथा {
      यदि(data->set.postfields) {
        माप_प्रकार plen = (data->set.postfieldsize >= 0) ?
          (माप_प्रकार)data->set.postfieldsize : म_माप(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      अन्यथा यदि(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      अन्यथा {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      यदि(result)
        फर्काऊ result;
    }

    यदि(req_clen > 0) {
      यदि(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        फर्काऊ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      यदि(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        यदि(result)
          फर्काऊ result;
      }

      यदि(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        यदि(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          यदि(result)
            फर्काऊ result;
        }
      }

      यदि(rtspreq == RTSPREQ_ANNOUNCE) {
        यदि(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          यदि(result)
            फर्काऊ result;
        }
      }
    }
    अन्यथा यदि(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  अन्यथा
    result = Curl_creader_set_null(data);
  फर्काऊ result;
}

संरचना rtspselect {
  अचर वर्ण *method;
  bool no_body;
};

स्थिर CURLcode pick_method(संरचना Curl_easy *data,
                            अचर अचिन्हित वर्ण rtspreq,
                            अचर वर्ण **p)
{
  स्थिर अचर संरचना rtspselect req[] = {
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
  यदि((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  फर्काऊ CURLE_OK;
}

/* Allocate and store a header string. */
स्थिर CURLcode rtsp_header_alloc(अचर वर्ण *header_name,
                                  अचर वर्ण *मान,
                                  वर्ण **target)
{
  यदि(!मान)
    फर्काऊ CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, मान);
  यदि(!*target)
    फर्काऊ CURLE_OUT_OF_MEMORY;
  फर्काऊ CURLE_OK;
}

संरचना rtsp_blocks {
  अचर वर्ण *request;
  अचर वर्ण *session_id;
  अचर वर्ण *accept;
  अचर वर्ण *range;
  अचर वर्ण *stream_uri;
  अचर वर्ण *hd_proxy_auth;
  अचर वर्ण *hd_auth;
  वर्ण *referrer;
  वर्ण *accept_encoding;
  वर्ण *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

स्थिर CURLcode rtsp_setup_request(संरचना Curl_easy *data,
                                   संरचना rtsp_blocks *b,
                                   अचर अचिन्हित वर्ण rtspreq)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  यदि(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  अन्यथा
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  यदि(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    यदि(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      यदि(result)
        फर्काऊ result;
      b->transport_alloc = TRUE;
    }
    अन्यथा {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  यदि(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      रिक्त : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    यदि(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      यदि(result)
        फर्काऊ result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, रिक्त, FALSE);
  यदि(result)
    फर्काऊ result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  यदि(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    यदि(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  यदि(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    यदि(!result)
      b->range = data->state.rangeline;
  }
  फर्काऊ result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

स्थिर CURLcode rtsp_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  अचर अचिन्हित वर्ण rtspreq = data->set.rtspreq;
  अचर वर्ण *str;
  संरचना RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  संरचना dynbuf req_buffer;
  संरचना rtsp_blocks block;
  स_रखो(&block, 0, परिमाण(block));

  *done = TRUE;
  यदि(!rtsp)
    फर्काऊ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  यदि(result)
    गच्छ out;

  यदि(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    गच्छ out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  यदि(result)
    गच्छ out;

  यदि(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = रिक्त;

  /*
   * Sanity check the custom headers
   */
  यदि(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    गच्छ out;
  }
  यदि(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  यदि(result)
    गच्छ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  यदि(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    यदि(result)
      गच्छ out;
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
  यदि(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  यदि(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  यदि(result)
    गच्छ out;

  यदि((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    यदि(result)
      गच्छ out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  यदि(result)
    गच्छ out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  यदि(result)
    गच्छ out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  यदि(result)
    गच्छ out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  यदि(result) {
    failf(data, "Failed sending RTSP request");
    गच्छ out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  यदि(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  यदि(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  फर्काऊ result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
स्थिर CURLcode rtp_write_body_junk(संरचना Curl_easy *data,
                                    संरचना rtsp_conn *rtspc,
                                    अचर वर्ण *buf,
                                    माप_प्रकार blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  यदि(body_remain) {
    यदि((curl_off_t)blen > body_remain)
      blen = (माप_प्रकार)body_remain;
    फर्काऊ Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode rtp_client_write(संरचना Curl_easy *data, अचर वर्ण *ptr,
                                 माप_प्रकार len)
{
  संरचना Curl_mapi_guard guard;
  माप_प्रकार wrote;
  curl_write_callback writeit;
  शून्य *user_ptr;

  यदि(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    फर्काऊ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  यदि(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  अन्यथा {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((वर्ण *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  यदि(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    फर्काऊ CURLE_WRITE_ERROR;
  }

  यदि(wrote != len) {
    failf(data, "Failed writing RTP data");
    फर्काऊ CURLE_WRITE_ERROR;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode rtsp_filter_rtp(संरचना Curl_easy *data,
                                संरचना rtsp_conn *rtspc,
                                अचर वर्ण *buf,
                                माप_प्रकार blen,
                                माप_प्रकार *pconsumed)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार skip_len = 0;

  *pconsumed = 0;
  जबसम्म(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    विकल्प(rtspc->state) {

    पक्ष RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      जबसम्म(blen && buf[0] != '$') {
        यदि(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          यदि(!म_भेदन(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            यदि(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            गच्छ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      यदि(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        यदि(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          यदि(result)
            गच्छ out;
        }
        यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          गच्छ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      रोक;
    }

    पक्ष RTP_PARSE_CHANNEL: {
      पूर्णाङ्क idx = ((अचिन्हित वर्ण)buf[0]) / 8;
      पूर्णाङ्क off = ((अचिन्हित वर्ण)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      यदि(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        यदि(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          यदि(result)
            गच्छ out;
        }
        अन्यथा {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        रोक;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (अचिन्हित वर्ण)buf[0];
      यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      रोक;
    }

    पक्ष RTP_PARSE_LEN: {
      माप_प्रकार rtp_len = curlx_dyn_len(&rtspc->buf);
      अचर वर्ण *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      यदि(rtp_len == 2)
        रोक;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      यदि(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        यदि(result)
          गच्छ out;
        रोक;
      }
      rtspc->state = RTP_PARSE_DATA;
      रोक;
    }

    पक्ष RTP_PARSE_DATA: {
      माप_प्रकार rtp_len = curlx_dyn_len(&rtspc->buf);
      माप_प्रकार needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      यदि(needed <= blen) {
        यदि(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          गच्छ out;
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
        यदि(result)
          गच्छ out;
      }
      अन्यथा {
        यदि(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          गच्छ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      रोक;
    }

    स्वभाव:
      DEBUGASSERT(0);
      फर्काऊ CURLE_RECV_ERROR;
    }
  }
out:
  यदि(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  फर्काऊ result;
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
स्थिर CURLcode rtsp_rtp_write_resp(संरचना Curl_easy *data,
                                    अचर वर्ण *buf,
                                    माप_प्रकार blen,
                                    bool is_eos)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  माप_प्रकार consumed = 0;

  यदि(!rtspc)
    फर्काऊ CURLE_FAILED_INIT;

  यदि(!data->req.header)
    rtspc->in_header = FALSE;
  यदि(!blen) {
    गच्छ out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  यदि(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    यदि(result)
      गच्छ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    यदि(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  यदि(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    यदि(result)
      गच्छ out;

    buf += consumed;
    blen -= consumed;

    यदि(!data->req.header)
      rtspc->in_header = FALSE;

    यदि(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      यदि(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      यदि(result)
        गच्छ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  यदि(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (पूर्णाङ्क)rtspc->state,
               data->req.size));
  यदि(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  यदि((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  फर्काऊ result;
}

स्थिर CURLcode rtsp_rtp_write_resp_hd(संरचना Curl_easy *data,
                                       अचर वर्ण *buf,
                                       माप_प्रकार blen,
                                       bool is_eos)
{
  फर्काऊ rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

स्थिर CURLcode rtsp_parse_transport(संरचना Curl_easy *data,
                                     अचर वर्ण *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  अचर वर्ण *start, *end;
  start = transport;
  जबसम्म(start && *start) {
    curlx_str_passblanks(&start);
    end = म_अक्षर(start, ';');
    यदि(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      अचर वर्ण *p = start + 12;
      यदि(!curlx_str_सङ्ख्या(&p, &chan1, 255)) {
        अचिन्हित वर्ण *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        यदि(!curlx_str_single(&p, '-') && curlx_str_सङ्ख्या(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        लागि(chan = chan1; chan <= chan2; chan++) {
          पूर्णाङ्क idx = (पूर्णाङ्क)chan / 8;
          पूर्णाङ्क off = (पूर्णाङ्क)chan % 8;
          rtp_channel_mask[idx] |= (अचिन्हित वर्ण)(1 << off);
        }
      }
      अन्यथा {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      रोक;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  फर्काऊ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(संरचना Curl_easy *data, अचर वर्ण *header)
{
  यदि(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    संरचना RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    अचर वर्ण *p = &header[5];
    यदि(!rtsp)
      फर्काऊ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    यदि(curlx_str_सङ्ख्या(&p, &CSeq, अच_पूर्णांक_उच्च)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      फर्काऊ CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  अन्यथा यदि(checkprefix("Session:", header)) {
    अचर वर्ण *start, *end, *str;
    माप_प्रकार idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    यदि(!*start) {
      failf(data, "Got a blank Session ID");
      फर्काऊ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    जबसम्म((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    यदि(str) {

      /* If the Session ID is set, then compare */
      यदि(म_माप(str) != idlen ||
         म_भेदन(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        फर्काऊ CURLE_RTSP_SESSION_ERROR;
      }
    }
    अन्यथा {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      शून्य *mem = curlx_memdup0(start, idlen);
      यदि(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        फर्काऊ CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  अन्यथा यदि(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    यदि(result)
      फर्काऊ result;
  }
  फर्काऊ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
अचर संरचना Curl_protocol Curl_protocol_rtsp = {
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
