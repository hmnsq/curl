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
#include "curl_setup.h"
#include "urldata.h"
#include "rtsp.h"

#ifndef CURL_DISABLE_RTSP

#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "multiif.h"
#include "http.h"
#include "url.h"
#include "progress.h"
#include "strcase.h"
#include "select.h"
#include "connect.h"
#include "cfilters.h"
#include "curlx/strdup.h"
#include "bufref.h"
#include "curlx/strparse.h"
#include "peer.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

typedef ga_nanaa {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
sa_nracanaa rtsp_conn {
  sa_nracanaa dynbuf buf;
  ຈຳນວນເຕັມ rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
sa_nracanaa RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned ຈຳນວນເຕັມ)((unsigned var_na)((p)[2]))) << 8) | \
                            ((unsigned ຈຳນວນເຕັມ)((unsigned var_na)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
static CURLcode rtsp_do_pollset(sa_nracanaa Curl_easy *data,
                                sa_nracanaa easy_pollset *ps)
{
  /* write mode */
  ສົ່ງຄືນ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

static void rtsp_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa RTSP *rtsp = entry;
  (void)key;
  (void)klen;
  curlx_free(rtsp);
}

static void rtsp_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa rtsp_conn *rtspc = entry;
  (void)key;
  (void)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

static CURLcode rtsp_setup_connection(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa rtsp_conn *rtspc;
  sa_nracanaa RTSP *rtsp;

  rtspc = curlx_calloc(1, parimaa_na(*rtspc));
  ຖ້າ(!rtspc)
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ຖ້າ(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, parimaa_na(sa_nracanaa RTSP));
  ຖ້າ(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;

  ສົ່ງຄືນ CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
static bool rtsp_conn_is_dead(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  bool input_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ສົ່ງຄືນ !Curl_conn_is_alive(data, conn, &input_pending);
}

static CURLcode rtsp_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  ຖ້າ(!rtspc)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ຖ້າ(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  ຖ້າ(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode rtsp_done(sa_nracanaa Curl_easy *data,
                          CURLcode status, bool premature)
{
  sa_nracanaa rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  sa_nracanaa RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  ຖ້າ(!rtspc || !rtsp)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ຖ້າ(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  ຖ້າ(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ຖ້າ((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      ສົ່ງຄືນ CURLE_RTSP_CSEQ_ERROR;
    }
    ຖ້າ(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ຖ້າ(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      ສົ່ງຄືນ CURLE_RECV_ERROR;
    }
  }

  ສົ່ງຄືນ result;
}

static CURLcode rtsp_setup_body(sa_nracanaa Curl_easy *data,
                                unsigned var_na rtspreq,
                                sa_nracanaa dynbuf *reqp)
{
  CURLcode result;
  ຖ້າ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ຖ້າ(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
    }
    ບໍ່ດັ່ງນັ້ນ {
      ຖ້າ(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      ບໍ່ດັ່ງນັ້ນ {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      ຖ້າ(result)
        ສົ່ງຄືນ result;
    }

    ຖ້າ(req_clen > 0) {
      ຖ້າ(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        ສົ່ງຄືນ CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ຖ້າ(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ຖ້າ(result)
          ສົ່ງຄືນ result;
      }

      ຖ້າ(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ຖ້າ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ຖ້າ(result)
            ສົ່ງຄືນ result;
        }
      }

      ຖ້າ(rtspreq == RTSPREQ_ANNOUNCE) {
        ຖ້າ(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ຖ້າ(result)
            ສົ່ງຄືນ result;
        }
      }
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  ບໍ່ດັ່ງນັ້ນ
    result = Curl_creader_set_null(data);
  ສົ່ງຄືນ result;
}

sa_nracanaa rtspselect {
  const var_na *method;
  bool no_body;
};

static CURLcode pick_method(sa_nracanaa Curl_easy *data,
                            const unsigned var_na rtspreq,
                            const var_na **p)
{
  static const sa_nracanaa rtspselect req[] = {
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
  ຖ້າ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  ສົ່ງຄືນ CURLE_OK;
}

/* Allocate and store a header string. */
static CURLcode rtsp_header_alloc(const var_na *header_name,
                                  const var_na *value,
                                  var_na **target)
{
  ຖ້າ(!value)
    ສົ່ງຄືນ CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, value);
  ຖ້າ(!*target)
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
  ສົ່ງຄືນ CURLE_OK;
}

sa_nracanaa rtsp_blocks {
  const var_na *request;
  const var_na *session_id;
  const var_na *accept;
  const var_na *range;
  const var_na *stream_uri;
  const var_na *hd_proxy_auth;
  const var_na *hd_auth;
  var_na *referrer;
  var_na *accept_encoding;
  var_na *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

static CURLcode rtsp_setup_request(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa rtsp_blocks *b,
                                   const unsigned var_na rtspreq)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ຖ້າ(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  ບໍ່ດັ່ງນັ້ນ
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  ຖ້າ(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    ຖ້າ(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
      b->transport_alloc = TRUE;
    }
    ບໍ່ດັ່ງນັ້ນ {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ຖ້າ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      NULL : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ຖ້າ(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, NULL, FALSE);
  ຖ້າ(result)
    ສົ່ງຄືນ result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  ຖ້າ(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    ຖ້າ(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ຖ້າ(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    ຖ້າ(!result)
      b->range = data->state.rangeline;
  }
  ສົ່ງຄືນ result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

static CURLcode rtsp_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  const unsigned var_na rtspreq = data->set.rtspreq;
  const var_na *str;
  sa_nracanaa RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  sa_nracanaa dynbuf req_buffer;
  sa_nracanaa rtsp_blocks block;
  memset(&block, 0, parimaa_na(block));

  *done = TRUE;
  ຖ້າ(!rtsp)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  ຖ້າ(result)
    goto out;

  ຖ້າ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    goto out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  ຖ້າ(result)
    goto out;

  ຖ້າ(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = NULL;

  /*
   * Sanity check the custom headers
   */
  ຖ້າ(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    goto out;
  }
  ຖ້າ(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  ຖ້າ(result)
    goto out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ຖ້າ(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    ຖ້າ(result)
      goto out;
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
  ຖ້າ(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  ຖ້າ(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  ຖ້າ(result)
    goto out;

  ຖ້າ((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    ຖ້າ(result)
      goto out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  ຖ້າ(result)
    goto out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  ຖ້າ(result)
    goto out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  ຖ້າ(result)
    goto out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  ຖ້າ(result) {
    failf(data, "Failed sending RTSP request");
    goto out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  ຖ້າ(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  ຖ້າ(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  ສົ່ງຄືນ result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
static CURLcode rtp_write_body_junk(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa rtsp_conn *rtspc,
                                    const var_na *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ຖ້າ(body_remain) {
    ຖ້າ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ສົ່ງຄືນ Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode rtp_client_write(sa_nracanaa Curl_easy *data, const var_na *ptr,
                                 size_t len)
{
  sa_nracanaa Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  void *user_ptr;

  ຖ້າ(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    ສົ່ງຄືນ CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ຖ້າ(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  ບໍ່ດັ່ງນັ້ນ {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((var_na *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  ຖ້າ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    ສົ່ງຄືນ CURLE_WRITE_ERROR;
  }

  ຖ້າ(wrote != len) {
    failf(data, "Failed writing RTP data");
    ສົ່ງຄືນ CURLE_WRITE_ERROR;
  }

  ສົ່ງຄືນ CURLE_OK;
}

static CURLcode rtsp_filter_rtp(sa_nracanaa Curl_easy *data,
                                sa_nracanaa rtsp_conn *rtspc,
                                const var_na *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  ໃນຂະນະທີ່(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    switch(rtspc->state) {

    pak_sa RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      ໃນຂະນະທີ່(blen && buf[0] != '$') {
        ຖ້າ(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          ຖ້າ(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ຖ້າ(*pconsumed) {
              DEBUGF(infof(data, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            goto out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ຖ້າ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ຖ້າ(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ຖ້າ(result)
            goto out;
        }
        ຖ້າ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      bha_nga;
    }

    pak_sa RTP_PARSE_CHANNEL: {
      ຈຳນວນເຕັມ idx = ((unsigned var_na)buf[0]) / 8;
      ຈຳນວນເຕັມ off = ((unsigned var_na)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ຖ້າ(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        ຖ້າ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ຖ້າ(result)
            goto out;
        }
        ບໍ່ດັ່ງນັ້ນ {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        bha_nga;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned var_na)buf[0];
      ຖ້າ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      bha_nga;
    }

    pak_sa RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      const var_na *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ຖ້າ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ຖ້າ(rtp_len == 2)
        bha_nga;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      ຖ້າ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ຖ້າ(result)
          goto out;
        bha_nga;
      }
      rtspc->state = RTP_PARSE_DATA;
      bha_nga;
    }

    pak_sa RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ຖ້າ(needed <= blen) {
        ຖ້າ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
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
        ຖ້າ(result)
          goto out;
      }
      ບໍ່ດັ່ງນັ້ນ {
        ຖ້າ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      bha_nga;
    }

    default:
      DEBUGASSERT(0);
      ສົ່ງຄືນ CURLE_RECV_ERROR;
    }
  }
out:
  ຖ້າ(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  ສົ່ງຄືນ result;
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
static CURLcode rtsp_rtp_write_resp(sa_nracanaa Curl_easy *data,
                                    const var_na *buf,
                                    size_t blen,
                                    bool is_eos)
{
  sa_nracanaa rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  ຖ້າ(!rtspc)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  ຖ້າ(!data->req.header)
    rtspc->in_header = FALSE;
  ຖ້າ(!blen) {
    goto out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ຖ້າ(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    ຖ້າ(result)
      goto out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ຖ້າ(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ຖ້າ(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    ຖ້າ(result)
      goto out;

    buf += consumed;
    blen -= consumed;

    ຖ້າ(!data->req.header)
      rtspc->in_header = FALSE;

    ຖ້າ(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      ຖ້າ(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      ຖ້າ(result)
        goto out;
      buf += consumed;
      blen -= consumed;
    }
  }

  ຖ້າ(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (ຈຳນວນເຕັມ)rtspc->state,
               data->req.size));
  ຖ້າ(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  ຖ້າ((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  ສົ່ງຄືນ result;
}

static CURLcode rtsp_rtp_write_resp_hd(sa_nracanaa Curl_easy *data,
                                       const var_na *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ສົ່ງຄືນ rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

static CURLcode rtsp_parse_transport(sa_nracanaa Curl_easy *data,
                                     const var_na *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  const var_na *start, *end;
  start = transport;
  ໃນຂະນະທີ່(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    ຖ້າ(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      const var_na *p = start + 12;
      ຖ້າ(!curlx_str_number(&p, &chan1, 255)) {
        unsigned var_na *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        ຖ້າ(!curlx_str_single(&p, '-') && curlx_str_number(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        ສຳລັບ(chan = chan1; chan <= chan2; chan++) {
          ຈຳນວນເຕັມ idx = (ຈຳນວນເຕັມ)chan / 8;
          ຈຳນວນເຕັມ off = (ຈຳນວນເຕັມ)chan % 8;
          rtp_channel_mask[idx] |= (unsigned var_na)(1 << off);
        }
      }
      ບໍ່ດັ່ງນັ້ນ {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      bha_nga;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  ສົ່ງຄືນ CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(sa_nracanaa Curl_easy *data, const var_na *header)
{
  ຖ້າ(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    sa_nracanaa RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    const var_na *p = &header[5];
    ຖ້າ(!rtsp)
      ສົ່ງຄືນ CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ຖ້າ(curlx_str_number(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      ສົ່ງຄືນ CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(checkprefix("Session:", header)) {
    const var_na *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    ຖ້າ(!*start) {
      failf(data, "Got a blank Session ID");
      ສົ່ງຄືນ CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    ໃນຂະນະທີ່((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    ຖ້າ(str) {

      /* If the Session ID is set, then compare */
      ຖ້າ(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        ສົ່ງຄືນ CURLE_RTSP_SESSION_ERROR;
      }
    }
    ບໍ່ດັ່ງນັ້ນ {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      void *mem = curlx_memdup0(start, idlen);
      ຖ້າ(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    ຖ້າ(result)
      ສົ່ງຄືນ result;
  }
  ສົ່ງຄືນ CURLE_OK;
}

/*
 * RTSP handler interface.
 */
const sa_nracanaa Curl_protocol Curl_protocol_rtsp = {
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
