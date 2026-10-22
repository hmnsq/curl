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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

પ્રકારવ્યાખ્યા ગણના {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
સંરચના rtsp_conn {
  સંરચના dynbuf buf;
  પૂર્ણાંક rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
સંરચના RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned પૂર્ણાંક)((unsigned અક્ષર)((p)[2]))) << 8) | \
                            ((unsigned પૂર્ણાંક)((unsigned અક્ષર)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
સ્થિર CURLcode rtsp_do_pollset(સંરચના Curl_easy *data,
                                સંરચના easy_pollset *ps)
{
  /* write mode */
  પાછા Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

સ્થિર શૂન્ય rtsp_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના RTSP *rtsp = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_free(rtsp);
}

સ્થિર શૂન્ય rtsp_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના rtsp_conn *rtspc = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

સ્થિર CURLcode rtsp_setup_connection(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  સંરચના rtsp_conn *rtspc;
  સંરચના RTSP *rtsp;

  rtspc = curlx_calloc(1, કદ(*rtspc));
  જો(!rtspc)
    પાછા CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  જો(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, કદ(સંરચના RTSP));
  જો(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
સ્થિર bool rtsp_conn_is_dead(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn)
{
  bool આગત_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  પાછા !Curl_conn_is_alive(data, conn, &આગત_pending);
}

સ્થિર CURLcode rtsp_connect(સંરચના Curl_easy *data, bool *done)
{
  સંરચના rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  જો(!rtspc)
    પાછા CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  જો(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  જો(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  પાછા CURLE_OK;
}

સ્થિર CURLcode rtsp_done(સંરચના Curl_easy *data,
                          CURLcode status, bool premature)
{
  સંરચના rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  સંરચના RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  જો(!rtspc || !rtsp)
    પાછા CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  જો(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  જો(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    જો((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      પાછા CURLE_RTSP_CSEQ_ERROR;
    }
    જો(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    જો(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      પાછા CURLE_RECV_ERROR;
    }
  }

  પાછા result;
}

સ્થિર CURLcode rtsp_setup_body(સંરચના Curl_easy *data,
                                unsigned અક્ષર rtspreq,
                                સંરચના dynbuf *reqp)
{
  CURLcode result;
  જો(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    જો(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      જો(result)
        પાછા result;
    }
    નહીંતર {
      જો(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      નહીંતર જો(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      નહીંતર {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      જો(result)
        પાછા result;
    }

    જો(req_clen > 0) {
      જો(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        પાછા CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      જો(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        જો(result)
          પાછા result;
      }

      જો(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        જો(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          જો(result)
            પાછા result;
        }
      }

      જો(rtspreq == RTSPREQ_ANNOUNCE) {
        જો(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          જો(result)
            પાછા result;
        }
      }
    }
    નહીંતર જો(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  નહીંતર
    result = Curl_creader_set_null(data);
  પાછા result;
}

સંરચના rtspselect {
  અચલ અક્ષર *method;
  bool no_body;
};

સ્થિર CURLcode pick_method(સંરચના Curl_easy *data,
                            અચલ unsigned અક્ષર rtspreq,
                            અચલ અક્ષર **p)
{
  સ્થિર અચલ સંરચના rtspselect req[] = {
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
  જો((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  પાછા CURLE_OK;
}

/* Allocate and store a header string. */
સ્થિર CURLcode rtsp_header_alloc(અચલ અક્ષર *header_name,
                                  અચલ અક્ષર *કિંમત,
                                  અક્ષર **target)
{
  જો(!કિંમત)
    પાછા CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, કિંમત);
  જો(!*target)
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
}

સંરચના rtsp_blocks {
  અચલ અક્ષર *request;
  અચલ અક્ષર *session_id;
  અચલ અક્ષર *accept;
  અચલ અક્ષર *range;
  અચલ અક્ષર *stream_uri;
  અચલ અક્ષર *hd_proxy_auth;
  અચલ અક્ષર *hd_auth;
  અક્ષર *referrer;
  અક્ષર *accept_encoding;
  અક્ષર *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

સ્થિર CURLcode rtsp_setup_request(સંરચના Curl_easy *data,
                                   સંરચના rtsp_blocks *b,
                                   અચલ unsigned અક્ષર rtspreq)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  જો(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  નહીંતર
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  જો(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    જો(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      જો(result)
        પાછા result;
      b->transport_alloc = TRUE;
    }
    નહીંતર {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  જો(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      ખાલી : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    જો(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      જો(result)
        પાછા result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, ખાલી, FALSE);
  જો(result)
    પાછા result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  જો(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    જો(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  જો(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    જો(!result)
      b->range = data->state.rangeline;
  }
  પાછા result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

સ્થિર CURLcode rtsp_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  અચલ unsigned અક્ષર rtspreq = data->set.rtspreq;
  અચલ અક્ષર *str;
  સંરચના RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  સંરચના dynbuf req_buffer;
  સંરચના rtsp_blocks block;
  memset(&block, 0, કદ(block));

  *done = TRUE;
  જો(!rtsp)
    પાછા CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  જો(result)
    જાઓ out;

  જો(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    જાઓ out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  જો(result)
    જાઓ out;

  જો(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = ખાલી;

  /*
   * Sanity check the custom headers
   */
  જો(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    જાઓ out;
  }
  જો(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  જો(result)
    જાઓ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  જો(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    જો(result)
      જાઓ out;
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
  જો(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  જો(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  જો(result)
    જાઓ out;

  જો((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    જો(result)
      જાઓ out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  જો(result)
    જાઓ out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  જો(result)
    જાઓ out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  જો(result)
    જાઓ out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  જો(result) {
    failf(data, "Failed sending RTSP request");
    જાઓ out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  જો(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  જો(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  પાછા result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
સ્થિર CURLcode rtp_write_body_junk(સંરચના Curl_easy *data,
                                    સંરચના rtsp_conn *rtspc,
                                    અચલ અક્ષર *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  જો(body_remain) {
    જો((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    પાછા Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode rtp_client_write(સંરચના Curl_easy *data, અચલ અક્ષર *ptr,
                                 size_t len)
{
  સંરચના Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  શૂન્ય *user_ptr;

  જો(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    પાછા CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  જો(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  નહીંતર {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((અક્ષર *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  જો(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    પાછા CURLE_WRITE_ERROR;
  }

  જો(wrote != len) {
    failf(data, "Failed writing RTP data");
    પાછા CURLE_WRITE_ERROR;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode rtsp_filter_rtp(સંરચના Curl_easy *data,
                                સંરચના rtsp_conn *rtspc,
                                અચલ અક્ષર *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  જ્યારે(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    પસંદગી(rtspc->state) {

    સ્થિતિ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      જ્યારે(blen && buf[0] != '$') {
        જો(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          જો(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            જો(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            જાઓ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      જો(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        જો(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          જો(result)
            જાઓ out;
        }
        જો(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          જાઓ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      તોડો;
    }

    સ્થિતિ RTP_PARSE_CHANNEL: {
      પૂર્ણાંક idx = ((unsigned અક્ષર)buf[0]) / 8;
      પૂર્ણાંક off = ((unsigned અક્ષર)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      જો(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        જો(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          જો(result)
            જાઓ out;
        }
        નહીંતર {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        તોડો;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned અક્ષર)buf[0];
      જો(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      તોડો;
    }

    સ્થિતિ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      અચલ અક્ષર *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      જો(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      જો(rtp_len == 2)
        તોડો;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      જો(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        જો(result)
          જાઓ out;
        તોડો;
      }
      rtspc->state = RTP_PARSE_DATA;
      તોડો;
    }

    સ્થિતિ RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      જો(needed <= blen) {
        જો(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          જાઓ out;
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
        જો(result)
          જાઓ out;
      }
      નહીંતર {
        જો(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          જાઓ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      તોડો;
    }

    મૂળભૂત:
      DEBUGASSERT(0);
      પાછા CURLE_RECV_ERROR;
    }
  }
out:
  જો(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  પાછા result;
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
સ્થિર CURLcode rtsp_rtp_write_resp(સંરચના Curl_easy *data,
                                    અચલ અક્ષર *buf,
                                    size_t blen,
                                    bool is_eos)
{
  સંરચના rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  જો(!rtspc)
    પાછા CURLE_FAILED_INIT;

  જો(!data->req.header)
    rtspc->in_header = FALSE;
  જો(!blen) {
    જાઓ out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  જો(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    જો(result)
      જાઓ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    જો(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  જો(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    જો(result)
      જાઓ out;

    buf += consumed;
    blen -= consumed;

    જો(!data->req.header)
      rtspc->in_header = FALSE;

    જો(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      જો(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      જો(result)
        જાઓ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  જો(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (પૂર્ણાંક)rtspc->state,
               data->req.size));
  જો(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  જો((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  પાછા result;
}

સ્થિર CURLcode rtsp_rtp_write_resp_hd(સંરચના Curl_easy *data,
                                       અચલ અક્ષર *buf,
                                       size_t blen,
                                       bool is_eos)
{
  પાછા rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

સ્થિર CURLcode rtsp_parse_transport(સંરચના Curl_easy *data,
                                     અચલ અક્ષર *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  અચલ અક્ષર *start, *end;
  start = transport;
  જ્યારે(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    જો(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      અચલ અક્ષર *p = start + 12;
      જો(!curlx_str_સંખ્યા(&p, &chan1, 255)) {
        unsigned અક્ષર *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        જો(!curlx_str_single(&p, '-') && curlx_str_સંખ્યા(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        માટે(chan = chan1; chan <= chan2; chan++) {
          પૂર્ણાંક idx = (પૂર્ણાંક)chan / 8;
          પૂર્ણાંક off = (પૂર્ણાંક)chan % 8;
          rtp_channel_mask[idx] |= (unsigned અક્ષર)(1 << off);
        }
      }
      નહીંતર {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      તોડો;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(સંરચના Curl_easy *data, અચલ અક્ષર *header)
{
  જો(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    સંરચના RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    અચલ અક્ષર *p = &header[5];
    જો(!rtsp)
      પાછા CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    જો(curlx_str_સંખ્યા(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      પાછા CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  નહીંતર જો(checkprefix("Session:", header)) {
    અચલ અક્ષર *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    જો(!*start) {
      failf(data, "Got a blank Session ID");
      પાછા CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    જ્યારે((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    જો(str) {

      /* If the Session ID is set, then compare */
      જો(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        પાછા CURLE_RTSP_SESSION_ERROR;
      }
    }
    નહીંતર {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      શૂન્ય *mem = curlx_memdup0(start, idlen);
      જો(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        પાછા CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  નહીંતર જો(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    જો(result)
      પાછા result;
  }
  પાછા CURLE_OK;
}

/*
 * RTSP handler interface.
 */
અચલ સંરચના Curl_protocol Curl_protocol_rtsp = {
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
