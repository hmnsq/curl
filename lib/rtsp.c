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

typedef enum {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_DATA
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
struct rtsp_conn {
  struct dynbuf buf;
  정수 rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
struct RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_LENGTH(p) ((((unsigned 정수)((unsigned char)((p)[2]))) << 8) | \
                            ((unsigned 정수)((unsigned char)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
static CURLcode rtsp_do_pollset(struct Curl_easy *data,
                                struct easy_pollset *ps)
{
  /* write mode */
  반환 Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

static void rtsp_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct RTSP *rtsp = entry;
  (void)key;
  (void)klen;
  curlx_free(rtsp);
}

static void rtsp_conn_dtor(const void *key, size_t klen, void *entry)
{
  struct rtsp_conn *rtspc = entry;
  (void)key;
  (void)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

static CURLcode rtsp_setup_connection(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct rtsp_conn *rtspc;
  struct RTSP *rtsp;

  rtspc = curlx_calloc(1, sizeof(*rtspc));
  만약(!rtspc)
    반환 CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  만약(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    반환 CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, sizeof(struct RTSP));
  만약(!rtsp ||
     Curl_meta_set(data, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    반환 CURLE_OUT_OF_MEMORY;

  반환 CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
static bool rtsp_conn_is_dead(struct Curl_easy *data,
                              struct connectdata *conn)
{
  bool input_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  반환 !Curl_conn_is_alive(data, conn, &input_pending);
}

static CURLcode rtsp_connect(struct Curl_easy *data, bool *done)
{
  struct rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);

  만약(!rtspc)
    반환 CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  만약(data->state.rtsp_next_client_CSeq == 0)
    data->state.rtsp_next_client_CSeq = 1;
  만약(data->state.rtsp_next_server_CSeq == 0)
    data->state.rtsp_next_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  반환 CURLE_OK;
}

static CURLcode rtsp_done(struct Curl_easy *data,
                          CURLcode status, bool premature)
{
  struct rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  struct RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  CURLcode result;

  만약(!rtspc || !rtsp)
    반환 CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  만약(data->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  result = Curl_http_done(data, status, premature);

  만약(!status && !result) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    만약((data->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(data,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      반환 CURLE_RTSP_CSEQ_ERROR;
    }
    만약(data->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(data, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    만약(data->set.rtspreq == RTSPREQ_RECEIVE &&
       data->req.eos_written) {
      failf(data, "Server prematurely closed the RTSP connection.");
      반환 CURLE_RECV_ERROR;
    }
  }

  반환 result;
}

static CURLcode rtsp_setup_body(struct Curl_easy *data,
                                unsigned char rtspreq,
                                struct dynbuf *reqp)
{
  CURLcode result;
  만약(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    만약(data->state.upload) {
      req_clen = data->state.infilesize;
      data->state.httpreq = HTTPREQ_PUT;
      result = Curl_creader_set_fread(data, req_clen);
      만약(result)
        반환 result;
    }
    아니면 {
      만약(data->set.postfields) {
        size_t plen = (data->set.postfieldsize >= 0) ?
          (size_t)data->set.postfieldsize : strlen(data->set.postfields);
        req_clen = (curl_off_t)plen;
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      }
      아니면 만약(data->state.infilesize >= 0) {
        req_clen = data->state.infilesize;
        result = Curl_creader_set_fread(data, req_clen);
      }
      아니면 {
        req_clen = 0;
        result = Curl_creader_set_null(data);
      }
      만약(result)
        반환 result;
    }

    만약(req_clen > 0) {
      만약(data->set.crlf) {
        failf(data, "Cannot do CRLF conversions on upload");
        반환 CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      만약(!Curl_checkheaders(data, STRCONST("Content-Length"))) {
        result = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        만약(result)
          반환 result;
      }

      만약(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        만약(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          만약(result)
            반환 result;
        }
      }

      만약(rtspreq == RTSPREQ_ANNOUNCE) {
        만약(!Curl_checkheaders(data, STRCONST("Content-Type"))) {
          result = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          만약(result)
            반환 result;
        }
      }
    }
    아니면 만약(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      data->state.httpreq = HTTPREQ_HEAD;
      data->req.no_body = TRUE;
    }
  }
  아니면
    result = Curl_creader_set_null(data);
  반환 result;
}

struct rtspselect {
  const char *method;
  bool no_body;
};

static CURLcode pick_method(struct Curl_easy *data,
                            const unsigned char rtspreq,
                            const char **p)
{
  static const struct rtspselect req[] = {
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
  만약((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  data->req.no_body = req[rtspreq - 1].no_body;
  반환 CURLE_OK;
}

/* Allocate and store a header string. */
static CURLcode rtsp_header_alloc(const char *header_name,
                                  const char *value,
                                  char **target)
{
  만약(!value)
    반환 CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, value);
  만약(!*target)
    반환 CURLE_OUT_OF_MEMORY;
  반환 CURLE_OK;
}

struct rtsp_blocks {
  const char *request;
  const char *session_id;
  const char *accept;
  const char *range;
  const char *stream_uri;
  const char *hd_proxy_auth;
  const char *hd_auth;
  char *referrer;
  char *accept_encoding;
  char *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

static CURLcode rtsp_setup_request(struct Curl_easy *data,
                                   struct rtsp_blocks *b,
                                   const unsigned char rtspreq)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;

  b->session_id = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  만약(CURL_EASY_STR(data, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(data, STRING_RTSP_STREAM_URI);
  아니면
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(data, STRCONST("Transport"));
  만약(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    만약(CURL_EASY_STR(data, STRING_RTSP_TRANSPORT)) {
      result = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(data, STRING_RTSP_TRANSPORT),
        &b->transport);
      만약(result)
        반환 result;
      b->transport_alloc = TRUE;
    }
    아니면 {
      failf(data,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  만약(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(data, STRCONST("Accept")) ?
      NULL : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    만약(!Curl_checkheaders(data, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(data, STRING_ENCODING)) {
      result = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(data, STRING_ENCODING),
                                 &b->accept_encoding);
      만약(result)
        반환 result;
    }
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(data, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, NULL, FALSE);
  만약(result)
    반환 result;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = data->req.hd_proxy_auth;
#endif
  b->hd_auth = data->req.hd_auth;

  /* Referrer */
  만약(Curl_bufref_ptr(&data->state.referer) &&
     !Curl_checkheaders(data, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&data->state.referer));
    만약(!b->referrer)
      result = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  만약(!result &&
     data->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(data, STRCONST("Range")) && data->state.range) {
    result = rtsp_header_alloc("Range",
                               data->state.range,
                               &data->state.rangeline);
    만약(!result)
      b->range = data->state.rangeline;
  }
  반환 result;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

static CURLcode rtsp_do(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  const unsigned char rtspreq = data->set.rtspreq;
  const char *str;
  struct RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
  struct dynbuf req_buffer;
  struct rtsp_blocks block;
  memset(&block, 0, sizeof(block));

  *done = TRUE;
  만약(!rtsp)
    반환 CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_buffer, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = data->state.rtsp_next_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  result = pick_method(data, rtspreq, &block.request);
  만약(result)
    goto out;

  만약(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    goto out;
  }

  result = rtsp_setup_request(data, &block,  rtspreq);
  만약(result)
    goto out;

  만약(block.session_id && data->state.rtsp_session_origin &&
     !data->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(data->state.origin, data->state.rtsp_session_origin))
    block.session_id = NULL;

  /*
   * Sanity check the custom headers
   */
  만약(Curl_checkheaders(data, STRCONST("CSeq"))) {
    failf(data, "CSeq cannot be set as a custom header.");
    result = CURLE_RTSP_CSEQ_ERROR;
    goto out;
  }
  만약(Curl_checkheaders(data, STRCONST("Session"))) {
    failf(data, "Session ID cannot be set as a custom header.");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result =
    curlx_dyn_addf(&req_buffer,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  만약(result)
    goto out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  만약(block.session_id) {
    result = curlx_dyn_addf(&req_buffer, "Session: %s\r\n", block.session_id);
    만약(result)
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
  만약(!result && str && *str &&
     !Curl_checkheaders(data, STRCONST("User-Agent")))
    result = curlx_dyn_addf(&req_buffer,
                            "User-Agent: %s\r\n", str);

  만약(!result)
    result = curlx_dyn_addf(&req_buffer,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  만약(result)
    goto out;

  만약((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    result = Curl_add_timecondition(data, &req_buffer);
    만약(result)
      goto out;
  }

  result = Curl_add_custom_headers(data, FALSE, HTTPVERSION, &req_buffer);
  만약(result)
    goto out;

  result = rtsp_setup_body(data, rtspreq, &req_buffer);
  만약(result)
    goto out;

  /* Finish the request buffer */
  result = curlx_dyn_addn(&req_buffer, STRCONST("\r\n"));
  만약(result)
    goto out;

  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);

  /* issue the request */
  result = Curl_req_send(data, &req_buffer, HTTPVERSION);
  만약(result) {
    failf(data, "Failed sending RTSP request");
    goto out;
  }

  /* Increment the CSeq on success */
  data->state.rtsp_next_client_CSeq++;

  만약(data->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrsSetUploadCounter(data, data->req.writebytecount);
    result = Curl_pgrsUpdate(data);
  }
out:
  만약(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_buffer);
  반환 result;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
static CURLcode rtp_write_body_junk(struct Curl_easy *data,
                                    struct rtsp_conn *rtspc,
                                    const char *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (data->req.headerline && !rtspc->in_header) &&
            (data->req.size >= 0) &&
            (data->req.bytecount < data->req.size);
  body_remain = in_body ? (data->req.size - data->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  만약(body_remain) {
    만약((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    반환 Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
  }
  반환 CURLE_OK;
}

static CURLcode rtp_client_write(struct Curl_easy *data, const char *ptr,
                                 size_t len)
{
  struct Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  void *user_ptr;

  만약(len == 0) {
    failf(data, "Cannot write a 0 size RTP packet.");
    반환 CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  만약(data->set.fwrite_rtp) {
    writeit = data->set.fwrite_rtp;
    user_ptr = data->set.rtp_out;
  }
  아니면 {
    writeit = data->set.fwrite_func;
    user_ptr = data->set.out;
  }

  CURL_CBAPI_START(&guard, data, easy_fwrite_rtp);
  wrote = writeit((char *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  만약(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(data, "Cannot pause RTP");
    반환 CURLE_WRITE_ERROR;
  }

  만약(wrote != len) {
    failf(data, "Failed writing RTP data");
    반환 CURLE_WRITE_ERROR;
  }

  반환 CURLE_OK;
}

static CURLcode rtsp_filter_rtp(struct Curl_easy *data,
                                struct rtsp_conn *rtspc,
                                const char *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  동안(blen) {
    bool in_body = (data->req.headerline && !rtspc->in_header) &&
                   (data->req.size >= 0) &&
                   (data->req.bytecount < data->req.size);
    switch(rtspc->state) {

    case RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      동안(blen && buf[0] != '$') {
        만약(!in_body && buf[0] == 'R' &&
           data->set.rtspreq != RTSPREQ_RECEIVE) {
          만약(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            만약(*pconsumed) {
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
      만약(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        만약(skip_len) {
          /* end of junk/BODY bytes, flush */
          result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          만약(result)
            goto out;
        }
        만약(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      break;
    }

    case RTP_PARSE_CHANNEL: {
      정수 idx = ((unsigned char)buf[0]) / 8;
      정수 off = ((unsigned char)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      만약(!(data->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(data, "RTSP: invalid RTP channel %d, skipping", idx));
        만약(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          result = rtp_write_body_junk(data, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          만약(result)
            goto out;
        }
        아니면 {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        break;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned char)buf[0];
      만약(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      break;
    }

    case RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      const char *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      만약(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      만약(rtp_len == 2)
        break;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_LENGTH(rtp_buf) + 4;
      만약(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(data, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        result = rtp_client_write(data, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        만약(result)
          goto out;
        break;
      }
      rtspc->state = RTP_PARSE_DATA;
      break;
    }

    case RTP_PARSE_DATA: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      만약(needed <= blen) {
        만약(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
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
        만약(result)
          goto out;
      }
      아니면 {
        만약(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      break;
    }

    default:
      DEBUGASSERT(0);
      반환 CURLE_RECV_ERROR;
    }
  }
out:
  만약(!result && skip_len)
    result = rtp_write_body_junk(data, rtspc, buf - skip_len, skip_len);
  반환 result;
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
static CURLcode rtsp_rtp_write_resp(struct Curl_easy *data,
                                    const char *buf,
                                    size_t blen,
                                    bool is_eos)
{
  struct rtsp_conn *rtspc =
    Curl_conn_meta_get(data->conn, CURL_META_RTSP_CONN);
  CURLcode result = CURLE_OK;
  size_t consumed = 0;

  만약(!rtspc)
    반환 CURLE_FAILED_INIT;

  만약(!data->req.header)
    rtspc->in_header = FALSE;
  만약(!blen) {
    goto out;
  }

  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  만약(!rtspc->in_header) {
    result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
    만약(result)
      goto out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    만약(blen && !data->req.header)
      DEBUGF(infof(data, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  만약(data->req.header && blen) {
    rtspc->in_header = TRUE;
    result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
    만약(result)
      goto out;

    buf += consumed;
    blen -= consumed;

    만약(!data->req.header)
      rtspc->in_header = FALSE;

    만약(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      만약(data->req.size <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        data->req.size = 0;
        data->req.download_done = TRUE;
      }
      result = rtsp_filter_rtp(data, rtspc, buf, blen, &consumed);
      만약(result)
        goto out;
      buf += consumed;
      blen -= consumed;
    }
  }

  만약(rtspc->state != RTP_PARSE_SKIP)
    data->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(data, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, data->req.done, (정수)rtspc->state,
               data->req.size));
  만약(!result && (is_eos || blen)) {
    result = Curl_client_write(data, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  만약((data->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    data->req.download_done = TRUE;
  }
  반환 result;
}

static CURLcode rtsp_rtp_write_resp_hd(struct Curl_easy *data,
                                       const char *buf,
                                       size_t blen,
                                       bool is_eos)
{
  반환 rtsp_rtp_write_resp(data, buf, blen, is_eos);
}

static CURLcode rtsp_parse_transport(struct Curl_easy *data,
                                     const char *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  const char *start, *end;
  start = transport;
  동안(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    만약(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      const char *p = start + 12;
      만약(!curlx_str_number(&p, &chan1, 255)) {
        unsigned char *rtp_channel_mask = data->state.rtp_channel_mask;
        chan2 = chan1;
        만약(!curlx_str_single(&p, '-') && curlx_str_number(&p, &chan2, 255)) {
          infof(data, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        위해(chan = chan1; chan <= chan2; chan++) {
          정수 idx = (정수)chan / 8;
          정수 off = (정수)chan % 8;
          rtp_channel_mask[idx] |= (unsigned char)(1 << off);
        }
      }
      아니면 {
        infof(data, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      break;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  반환 CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(struct Curl_easy *data, const char *header)
{
  만약(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    struct RTSP *rtsp = Curl_meta_get(data, CURL_META_RTSP_EASY);
    const char *p = &header[5];
    만약(!rtsp)
      반환 CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    만약(curlx_str_number(&p, &CSeq, UINT_MAX)) {
      failf(data, "Unable to read the CSeq header: [%s]", header);
      반환 CURLE_RTSP_CSEQ_ERROR;
    }
    data->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  아니면 만약(checkprefix("Session:", header)) {
    const char *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    만약(!*start) {
      failf(data, "Got a blank Session ID");
      반환 CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    동안((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
    만약(str) {

      /* If the Session ID is set, then compare */
      만약(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(data, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        반환 CURLE_RTSP_SESSION_ERROR;
      }
    }
    아니면 {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      void *mem = curlx_memdup0(start, idlen);
      만약(!mem ||
         CURL_EASY_STR_SETN(data, STRING_RTSP_SESSION_ID, mem))
        반환 CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&data->state.rtsp_session_origin, data->state.origin);
    }
  }
  아니면 만약(checkprefix("Transport:", header)) {
    CURLcode result;
    result = rtsp_parse_transport(data, header + 10);
    만약(result)
      반환 result;
  }
  반환 CURLE_OK;
}

/*
 * RTSP handler interface.
 */
const struct Curl_protocol Curl_protocol_rtsp = {
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
