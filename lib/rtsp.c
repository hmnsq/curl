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

#ifndef CURL_DISABLE_RTSP

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
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

تعريفنوع تعداد {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
بنية rtsp_conn {
  بنية dynbuf buf;
  صحيح rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
بنية RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned صحيح)((unsigned حرف)((p)[2]))) << 8) | \
                            ((unsigned صحيح)((unsigned حرف)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
ساكن CURLcode rtsp_do_pollset(بنية Curl_easy *data,
                                بنية easy_pollset *ps)
{
  /* write mode */
  החזר Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

ساكن فراغ rtsp_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية RTSP *rtsp = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_free(rtsp);
}

ساكن فراغ rtsp_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية rtsp_conn *rtspc = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

ساكن CURLcode rtsp_setup_connection(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  بنية rtsp_conn *rtspc;
  بنية RTSP *rtsp;

  rtspc = curlx_calloc(1, حجم(*rtspc));
  אם(!rtspc)
    החזר CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  אם(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    החזר CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, حجم(بنية RTSP));
  אם(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    החזר CURLE_OUT_OF_MEMORY;

  החזר CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
ساكن bool rtsp_conn_is_dead(بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  bool input_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  החזר !Curl_conn_is_alive(data, conn, &input_pending);
}

ساكن CURLcode rtsp_connect(بنية Curl_easy *data, bool *done)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  אם(!rtspc)
    החזר CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  אם(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  אם(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  החזר CURLE_OK;
}

ساكن CURLcode rtsp_done(بنية Curl_easy *data,
                          CURLcode status, bool premature)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  بنية RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  אם(!rtspc || !rtsp)
    החזר CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  אם(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  אם(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    אם((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      החזר CURLE_RTSP_CSEQ_ERROR;
    }
    אם(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    אם(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      החזר CURLE_RECV_ERROR;
    }
  }

  החזר result;
}

ساكن CURLcode rtsp_setup_body(بنية Curl_easy *data,
                                unsigned حرف rtspreq,
                                بنية dynbuf *reqp)
{
  CURLcode result;
  אם(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    אם(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      אם(result)
        החזר result;
    }
    אחרת {
      אם(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      אחרת אם(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      אחרת {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      אם(result)
        החזר result;
    }

    אם(req_clen > 0) {
      אם(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        החזר CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      אם(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        אם(result)
          החזר result;
      }

      אם(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        אם(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          אם(result)
            החזר result;
        }
      }

      אם(rtspreq == RTSPREQ_ANNOUNCE) {
        אם(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          אם(result)
            החזר result;
        }
      }
    }
    אחרת אם(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  אחרת
    result = Curl_creader_set_null(data);
  החזר result;
}

بنية rtspselect {
  ثابت حرف *method;
  bool no_body;
};

ساكن CURLcode pick_method(بنية Curl_easy *data,
                            ثابت unsigned حرف rtspreq,
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_LAST));
  אם((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  החזר CURLE_OK;
}

/* Allocate and store a header string. */
ساكن CURLcode rtsp_header_alloc(ثابت حرف *header_name,
                                  ثابت حرف *value,
                                  حرف **target)
{
  אם(!value)
    החזר CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, value);
  אם(!*target)
    החזר CURLE_OUT_OF_MEMORY;
  החזר CURLE_OK;
}

بنية rtsp_blocks {
  ثابت حرف *request;
  ثابت حرف *session_id;
  ثابت حرف *accept;
  ثابت حرف *range;
  ثابت حرف *stream_uri;
  ثابت حرف *hd_proxy_auth;
  ثابت حرف *hd_auth;
  حرف *referrer;
  حرف *accept_encoding;
  حرف *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

ساكن CURLcode rtsp_setup_request(بنية Curl_easy *data,
                                   بنية rtsp_blocks *b,
                                   ثابت unsigned حرف rtspreq)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  אם(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  אחרת
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  אם(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    אם(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      אם(result)
        החזר result;
      b->transport_alloc = TRUE;
    }
    אחרת {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  אם(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      عدم : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    אם(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      אם(result)
        החזר result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, عدم, FALSE);
  אם(result)
    החזר result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  אם(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    אם(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  אם(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    אם(!result)
      b->range = data->state.rangeline;
  }
  החזר result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

ساكن CURLcode rtsp_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ثابت unsigned حرف rtspreq = data->set.rtspreq;
  ثابت حرف *str;
  بنية RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  بنية dynbuf req_buffer;
  بنية rtsp_blocks block;
  memset(&block, 0, حجم(block));

  *done = TRUE;
  אם(!rtsp)
    החזר CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  אם(result)
    اذهب out;

  אם(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    اذهب out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  אם(result)
    اذهب out;

  אם(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = عدم;

  /*
   * Sanity check the custom headers
   */
  אם(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    اذهب out;
  }
  אם(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  אם(result)
    اذهب out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  אם(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    אם(result)
      اذهب out;
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
  אם(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  אם(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  אם(result)
    اذهب out;

  אם((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    אם(result)
      اذهب out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  אם(result)
    اذهب out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  אם(result)
    اذهب out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  אם(result)
    اذهب out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  אם(result) {
    failf(data, "Failed sending RTSP request");
    اذهب out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  אם(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  אם(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  החזר result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
ساكن CURLcode rtp_write_body_junk(بنية Curl_easy *data,
                                    بنية rtsp_conn *rtspc,
                                    ثابت حرف *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  אם(body_remain) {
    אם((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    החזר Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  החזר CURLE_OK;
}

ساكن CURLcode rtp_client_write(بنية Curl_easy *data, ثابت حرف *ptr,
                                 size_t len)
{
  بنية Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  فراغ *user_ptr;

  אם(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    החזר CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  אם(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  אחרת {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((حرف *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  אם(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    החזר CURLE_WRITE_ERROR;
  }

  אם(wrote != len) {
    failf(data, "Failed writing RTP data");
    החזר CURLE_WRITE_ERROR;
  }

  החזר CURLE_OK;
}

ساكن CURLcode rtsp_filter_rtp(بنية Curl_easy *data,
                                بنية rtsp_conn *rtspc,
                                ثابت حرف *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  כלעוד(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    بدل(rtspc->state) {

    حالة RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      כלעוד(blen && buf[0] != '$') {
        אם(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          אם(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            אם(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            اذهب out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      אם(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        אם(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          אם(result)
            اذهب out;
        }
        אם(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          اذهب out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      اكسر;
    }

    حالة RTP_PARSE_CHANNEL: {
      صحيح idx = ((unsigned حرف)buf[0]) / 8;
      صحيح off = ((unsigned حرف)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      אם(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        אם(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          אם(result)
            اذهب out;
        }
        אחרת {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        اكسر;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned حرف)buf[0];
      אם(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
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
      אם(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      אם(rtp_len == 2)
        اكسر;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      אם(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        אם(result)
          اذهب out;
        اكسر;
      }
      rtspc->state = RTP_PARSE_DATA;
      اكسر;
    }

    حالة RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      אם(needed <= blen) {
        אם(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          اذهب out;
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
        אם(result)
          اذهب out;
      }
      אחרת {
        אם(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          اذهب out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      اكسر;
    }

    افتراضي:
      DEBUGASSERT(0);
      החזר CURLE_RECV_ERROR;
    }
  }
out:
  אם(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  החזר result;
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
ساكن CURLcode rtsp_rtp_write_resp(بنية Curl_easy *data,
                                    ثابت حرف *buf,
                                    size_t blen,
                                    bool is_eos)
{
  بنية rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  אם(!rtspc)
    החזר CURLE_FAILED_INIT;

  אם(!data->req.header)
    rtspc->in_header = FALSE;
  אם(!blen) {
    اذهب out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  אם(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    אם(result)
      اذهب out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    אם(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  אם(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    אם(result)
      اذهب out;

    buf += consumed;
    blen -= consumed;

    אם(!data->req.header)
      rtspc->in_header = FALSE;

    אם(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      אם(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      אם(result)
        اذهب out;
      buf += consumed;
      blen -= consumed;
    }
  }

  אם(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (صحيح)rtspc->state,
               data->req.size));
  אם(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  אם((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  החזר result;
}

ساكن CURLcode rtsp_rtp_write_resp_hd(بنية Curl_easy *data,
                                       ثابت حرف *buf,
                                       size_t blen,
                                       bool is_eos)
{
  החזר rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

ساكن CURLcode rtsp_parse_transport(بنية Curl_easy *data,
                                     ثابت حرف *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ثابت حرف *start, *end;
  start = transport;
  כלעוד(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    אם(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ثابت حرف *p = start + 12;
      אם(!curlx_str_number(&p, &chan1, 255)) {
        unsigned حرف *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        אם(!curlx_str_single(&p, '-') && curlx_str_number(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        עבור(chan = chan1; chan <= chan2; chan++) {
          صحيح idx = (صحيح)chan / 8;
          صحيح off = (صحيح)chan % 8;
          rtp_channel_mask[idx] |= (unsigned حرف)(1 << off);
        }
      }
      אחרת {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      اكسر;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  החזר CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(بنية Curl_easy *data, ثابت حرف *header)
{
  אם(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    بنية RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    ثابت حرف *p = &header[5];
    אם(!rtsp)
      החזר CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    אם(curlx_str_number(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      החזר CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  אחרת אם(checkprefix("Session:", header)) {
    ثابت حرف *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    אם(!*start) {
      failf(data, "Got a blank Session ID");
      החזר CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    כלעוד((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    אם(str) {

      /* If the Session ID is set, then compare */
      אם(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        החזר CURLE_RTSP_SESSION_ERROR;
      }
    }
    אחרת {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      فراغ *mem = curlx_memdup0(start, idlen);
      אם(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        החזר CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  אחרת אם(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    אם(result)
      החזר result;
  }
  החזר CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_rtsp = {
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
