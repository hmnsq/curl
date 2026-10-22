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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

തരനിർവചനം എണ്ണൽ {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
ഘടന rtsp_conn {
  ഘടന dynbuf buf;
  പൂർണ്ണസംഖ്യ rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
ഘടന RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)((p)[2]))) << 8) | \
                            ((unsigned പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
സ്ഥിതം CURLcode rtsp_do_pollset(ഘടന Curl_easy *data,
                                ഘടന easy_pollset *ps)
{
  /* write mode */
  തിരികെ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

സ്ഥിതം ശൂന്യം rtsp_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന RTSP *rtsp = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_free(rtsp);
}

സ്ഥിതം ശൂന്യം rtsp_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന rtsp_conn *rtspc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

സ്ഥിതം CURLcode rtsp_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന rtsp_conn *rtspc;
  ഘടന RTSP *rtsp;

  rtspc = curlx_calloc(1, വലുപ്പം(*rtspc));
  എങ്കിൽ(!rtspc)
    തിരികെ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  എങ്കിൽ(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, വലുപ്പം(ഘടന RTSP));
  എങ്കിൽ(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
സ്ഥിതം bool rtsp_conn_is_dead(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn)
{
  bool നിവേശം_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  തിരികെ !Curl_conn_is_alive(data, conn, &നിവേശം_pending);
}

സ്ഥിതം CURLcode rtsp_connect(ഘടന Curl_easy *data, bool *done)
{
  ഘടന rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  എങ്കിൽ(!rtspc)
    തിരികെ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  എങ്കിൽ(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  എങ്കിൽ(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode rtsp_done(ഘടന Curl_easy *data,
                          CURLcode status, bool premature)
{
  ഘടന rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  ഘടന RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  എങ്കിൽ(!rtspc || !rtsp)
    തിരികെ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  എങ്കിൽ(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  എങ്കിൽ(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    എങ്കിൽ((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      തിരികെ CURLE_RTSP_CSEQ_ERROR;
    }
    എങ്കിൽ(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    എങ്കിൽ(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      തിരികെ CURLE_RECV_ERROR;
    }
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode rtsp_setup_body(ഘടന Curl_easy *data,
                                unsigned അക്ഷരം rtspreq,
                                ഘടന dynbuf *reqp)
{
  CURLcode result;
  എങ്കിൽ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    എങ്കിൽ(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      അല്ലെങ്കിൽ {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      എങ്കിൽ(result)
        തിരികെ result;
    }

    എങ്കിൽ(req_clen > 0) {
      എങ്കിൽ(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        തിരികെ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        എങ്കിൽ(result)
          തിരികെ result;
      }

      എങ്കിൽ(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          എങ്കിൽ(result)
            തിരികെ result;
        }
      }

      എങ്കിൽ(rtspreq == RTSPREQ_ANNOUNCE) {
        എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          എങ്കിൽ(result)
            തിരികെ result;
        }
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  അല്ലെങ്കിൽ
    result = Curl_creader_set_null(data);
  തിരികെ result;
}

ഘടന rtspselect {
  സ്ഥിരം അക്ഷരം *method;
  bool no_body;
};

സ്ഥിതം CURLcode pick_method(ഘടന Curl_easy *data,
                            സ്ഥിരം unsigned അക്ഷരം rtspreq,
                            സ്ഥിരം അക്ഷരം **p)
{
  സ്ഥിതം സ്ഥിരം ഘടന rtspselect req[] = {
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
  എങ്കിൽ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  തിരികെ CURLE_OK;
}

/* Allocate and store a header string. */
സ്ഥിതം CURLcode rtsp_header_alloc(സ്ഥിരം അക്ഷരം *header_name,
                                  സ്ഥിരം അക്ഷരം *മൂല്യം,
                                  അക്ഷരം **target)
{
  എങ്കിൽ(!മൂല്യം)
    തിരികെ CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, മൂല്യം);
  എങ്കിൽ(!*target)
    തിരികെ CURLE_OUT_OF_MEMORY;
  തിരികെ CURLE_OK;
}

ഘടന rtsp_blocks {
  സ്ഥിരം അക്ഷരം *request;
  സ്ഥിരം അക്ഷരം *session_id;
  സ്ഥിരം അക്ഷരം *accept;
  സ്ഥിരം അക്ഷരം *range;
  സ്ഥിരം അക്ഷരം *stream_uri;
  സ്ഥിരം അക്ഷരം *hd_proxy_auth;
  സ്ഥിരം അക്ഷരം *hd_auth;
  അക്ഷരം *referrer;
  അക്ഷരം *accept_encoding;
  അക്ഷരം *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

സ്ഥിതം CURLcode rtsp_setup_request(ഘടന Curl_easy *data,
                                   ഘടന rtsp_blocks *b,
                                   സ്ഥിരം unsigned അക്ഷരം rtspreq)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  എങ്കിൽ(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  അല്ലെങ്കിൽ
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  എങ്കിൽ(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    എങ്കിൽ(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      എങ്കിൽ(result)
        തിരികെ result;
      b->transport_alloc = TRUE;
    }
    അല്ലെങ്കിൽ {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  എങ്കിൽ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      ശൂന്യ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, ശൂന്യ, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  എങ്കിൽ(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    എങ്കിൽ(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  എങ്കിൽ(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    എങ്കിൽ(!result)
      b->range = data->state.rangeline;
  }
  തിരികെ result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

സ്ഥിതം CURLcode rtsp_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം unsigned അക്ഷരം rtspreq = data->set.rtspreq;
  സ്ഥിരം അക്ഷരം *str;
  ഘടന RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  ഘടന dynbuf req_buffer;
  ഘടന rtsp_blocks block;
  memset(&block, 0, വലുപ്പം(block));

  *done = TRUE;
  എങ്കിൽ(!rtsp)
    തിരികെ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    പോകുക out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = ശൂന്യ;

  /*
   * Sanity check the custom headers
   */
  എങ്കിൽ(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    പോകുക out;
  }
  എങ്കിൽ(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  എങ്കിൽ(result)
    പോകുക out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  എങ്കിൽ(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    എങ്കിൽ(result)
      പോകുക out;
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
  എങ്കിൽ(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  എങ്കിൽ(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    എങ്കിൽ(result)
      പോകുക out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  എങ്കിൽ(result)
    പോകുക out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  എങ്കിൽ(result)
    പോകുക out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  എങ്കിൽ(result)
    പോകുക out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  എങ്കിൽ(result) {
    failf(data, "Failed sending RTSP request");
    പോകുക out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  എങ്കിൽ(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  എങ്കിൽ(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  തിരികെ result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
സ്ഥിതം CURLcode rtp_write_body_junk(ഘടന Curl_easy *data,
                                    ഘടന rtsp_conn *rtspc,
                                    സ്ഥിരം അക്ഷരം *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  എങ്കിൽ(body_remain) {
    എങ്കിൽ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    തിരികെ Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode rtp_client_write(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *ptr,
                                 size_t len)
{
  ഘടന Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  ശൂന്യം *user_ptr;

  എങ്കിൽ(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    തിരികെ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  എങ്കിൽ(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  അല്ലെങ്കിൽ {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((അക്ഷരം *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  എങ്കിൽ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    തിരികെ CURLE_WRITE_ERROR;
  }

  എങ്കിൽ(wrote != len) {
    failf(data, "Failed writing RTP data");
    തിരികെ CURLE_WRITE_ERROR;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode rtsp_filter_rtp(ഘടന Curl_easy *data,
                                ഘടന rtsp_conn *rtspc,
                                സ്ഥിരം അക്ഷരം *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ആകുമ്പോൾ(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    തിരഞ്ഞെടുപ്പ്(rtspc->state) {

    അവസ്ഥ RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ആകുമ്പോൾ(blen && buf[0] != '$') {
        എങ്കിൽ(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          എങ്കിൽ(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            എങ്കിൽ(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            പോകുക out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      എങ്കിൽ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        എങ്കിൽ(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          എങ്കിൽ(result)
            പോകുക out;
        }
        എങ്കിൽ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          പോകുക out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      നിർത്തുക;
    }

    അവസ്ഥ RTP_PARSE_CHANNEL: {
      പൂർണ്ണസംഖ്യ idx = ((unsigned അക്ഷരം)buf[0]) / 8;
      പൂർണ്ണസംഖ്യ off = ((unsigned അക്ഷരം)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      എങ്കിൽ(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        എങ്കിൽ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          എങ്കിൽ(result)
            പോകുക out;
        }
        അല്ലെങ്കിൽ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        നിർത്തുക;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned അക്ഷരം)buf[0];
      എങ്കിൽ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      നിർത്തുക;
    }

    അവസ്ഥ RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      സ്ഥിരം അക്ഷരം *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      എങ്കിൽ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      എങ്കിൽ(rtp_len == 2)
        നിർത്തുക;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      എങ്കിൽ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        എങ്കിൽ(result)
          പോകുക out;
        നിർത്തുക;
      }
      rtspc->state = RTP_PARSE_DATA;
      നിർത്തുക;
    }

    അവസ്ഥ RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      എങ്കിൽ(needed <= blen) {
        എങ്കിൽ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          പോകുക out;
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
        എങ്കിൽ(result)
          പോകുക out;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          പോകുക out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      നിർത്തുക;
    }

    സ്ഥിരസ്ഥിതി:
      DEBUGASSERT(0);
      തിരികെ CURLE_RECV_ERROR;
    }
  }
out:
  എങ്കിൽ(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  തിരികെ result;
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
സ്ഥിതം CURLcode rtsp_rtp_write_resp(ഘടന Curl_easy *data,
                                    സ്ഥിരം അക്ഷരം *buf,
                                    size_t blen,
                                    bool is_eos)
{
  ഘടന rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  എങ്കിൽ(!rtspc)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!data->req.header)
    rtspc->in_header = FALSE;
  എങ്കിൽ(!blen) {
    പോകുക out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  എങ്കിൽ(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    എങ്കിൽ(result)
      പോകുക out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    എങ്കിൽ(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  എങ്കിൽ(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    എങ്കിൽ(result)
      പോകുക out;

    buf += consumed;
    blen -= consumed;

    എങ്കിൽ(!data->req.header)
      rtspc->in_header = FALSE;

    എങ്കിൽ(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      എങ്കിൽ(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      എങ്കിൽ(result)
        പോകുക out;
      buf += consumed;
      blen -= consumed;
    }
  }

  എങ്കിൽ(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (പൂർണ്ണസംഖ്യ)rtspc->state,
               data->req.size));
  എങ്കിൽ(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  എങ്കിൽ((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode rtsp_rtp_write_resp_hd(ഘടന Curl_easy *data,
                                       സ്ഥിരം അക്ഷരം *buf,
                                       size_t blen,
                                       bool is_eos)
{
  തിരികെ rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

സ്ഥിതം CURLcode rtsp_parse_transport(ഘടന Curl_easy *data,
                                     സ്ഥിരം അക്ഷരം *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  സ്ഥിരം അക്ഷരം *start, *end;
  start = transport;
  ആകുമ്പോൾ(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    എങ്കിൽ(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      സ്ഥിരം അക്ഷരം *p = start + 12;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &chan1, 255)) {
        unsigned അക്ഷരം *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        എങ്കിൽ(!curlx_str_single(&p, '-') && curlx_str_സംഖ്യ(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        വേണ്ടി(chan = chan1; chan <= chan2; chan++) {
          പൂർണ്ണസംഖ്യ idx = (പൂർണ്ണസംഖ്യ)chan / 8;
          പൂർണ്ണസംഖ്യ off = (പൂർണ്ണസംഖ്യ)chan % 8;
          rtp_channel_mask[idx] |= (unsigned അക്ഷരം)(1 << off);
        }
      }
      അല്ലെങ്കിൽ {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      നിർത്തുക;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *header)
{
  എങ്കിൽ(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    ഘടന RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    സ്ഥിരം അക്ഷരം *p = &header[5];
    എങ്കിൽ(!rtsp)
      തിരികെ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      തിരികെ CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("Session:", header)) {
    സ്ഥിരം അക്ഷരം *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    എങ്കിൽ(!*start) {
      failf(data, "Got a blank Session ID");
      തിരികെ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    ആകുമ്പോൾ((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    എങ്കിൽ(str) {

      /* If the Session ID is set, then compare */
      എങ്കിൽ(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        തിരികെ CURLE_RTSP_SESSION_ERROR;
      }
    }
    അല്ലെങ്കിൽ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      ശൂന്യം *mem = curlx_memdup0(start, idlen);
      എങ്കിൽ(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        തിരികെ CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_rtsp = {
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
