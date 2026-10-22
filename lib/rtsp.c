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

প্রকারলেখ গণনা {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_তথ্য
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
কাঠামো rtsp_conn {
  কাঠামো dynbuf buf;
  পূর্ণসংখ্যা rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
কাঠামো RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#define RTP_PKT_দৈর্ঘ্য(p) ((((unsigned পূর্ণসংখ্যা)((unsigned অক্ষর)((p)[2]))) << 8) | \
                            ((unsigned পূর্ণসংখ্যা)((unsigned অক্ষর)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
স্থির CURLcode rtsp_do_pollset(কাঠামো Curl_easy *তথ্য,
                                কাঠামো easy_pollset *ps)
{
  /* write mode */
  ফেরত Curl_pollset_add_out(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
}

#define MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

স্থির শূন্য rtsp_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো RTSP *rtsp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(rtsp);
}

স্থির শূন্য rtsp_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো rtsp_conn *rtspc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

স্থির CURLcode rtsp_setup_connection(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  কাঠামো rtsp_conn *rtspc;
  কাঠামো RTSP *rtsp;

  rtspc = curlx_calloc(1, আকার(*rtspc));
  যদি(!rtspc)
    ফেরত CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  যদি(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, আকার(কাঠামো RTSP));
  যদি(!rtsp ||
     Curl_meta_set(তথ্য, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
স্থির bool rtsp_conn_is_dead(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  bool প্রবেশ_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  ফেরত !Curl_conn_is_alive(তথ্য, conn, &প্রবেশ_pending);
}

স্থির CURLcode rtsp_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো rtsp_conn *rtspc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_RTSP_CONN);

  যদি(!rtspc)
    ফেরত CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  যদি(তথ্য->state.rtsp_পরবর্তী_client_CSeq == 0)
    তথ্য->state.rtsp_পরবর্তী_client_CSeq = 1;
  যদি(তথ্য->state.rtsp_পরবর্তী_server_CSeq == 0)
    তথ্য->state.rtsp_পরবর্তী_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  ফেরত CURLE_OK;
}

স্থির CURLcode rtsp_done(কাঠামো Curl_easy *তথ্য,
                          CURLcode status, bool premature)
{
  কাঠামো rtsp_conn *rtspc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_RTSP_CONN);
  কাঠামো RTSP *rtsp = Curl_meta_get(তথ্য, CURL_META_RTSP_EASY);
  CURLcode ফলাফল;

  যদি(!rtspc || !rtsp)
    ফেরত CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  যদি(তথ্য->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  ফলাফল = Curl_http_done(তথ্য, status, premature);

  যদি(!status && !ফলাফল) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    যদি((তথ্য->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(তথ্য,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      ফেরত CURLE_RTSP_CSEQ_ERROR;
    }
    যদি(তথ্য->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(তথ্য, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    যদি(তথ্য->set.rtspreq == RTSPREQ_RECEIVE &&
       তথ্য->req.eos_written) {
      failf(তথ্য, "Server prematurely closed the RTSP connection.");
      ফেরত CURLE_RECV_ERROR;
    }
  }

  ফেরত ফলাফল;
}

স্থির CURLcode rtsp_setup_body(কাঠামো Curl_easy *তথ্য,
                                unsigned অক্ষর rtspreq,
                                কাঠামো dynbuf *reqp)
{
  CURLcode ফলাফল;
  যদি(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    যদি(তথ্য->state.upload) {
      req_clen = তথ্য->state.infilesize;
      তথ্য->state.httpreq = HTTPREQ_PUT;
      ফলাফল = Curl_creader_set_fread(তথ্য, req_clen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা {
      যদি(তথ্য->set.postfields) {
        size_t plen = (তথ্য->set.postfieldsize >= 0) ?
          (size_t)তথ্য->set.postfieldsize : strlen(তথ্য->set.postfields);
        req_clen = (curl_off_t)plen;
        ফলাফল = Curl_creader_set_buf(তথ্য, তথ্য->set.postfields, plen);
      }
      অন্যথা যদি(তথ্য->state.infilesize >= 0) {
        req_clen = তথ্য->state.infilesize;
        ফলাফল = Curl_creader_set_fread(তথ্য, req_clen);
      }
      অন্যথা {
        req_clen = 0;
        ফলাফল = Curl_creader_set_null(তথ্য);
      }
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }

    যদি(req_clen > 0) {
      যদি(তথ্য->set.crlf) {
        failf(তথ্য, "Cannot do CRLF conversions on upload");
        ফেরত CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      যদি(!Curl_checkheaders(তথ্য, STRCONST("Content-Length"))) {
        ফলাফল = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }

      যদি(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        যদি(!Curl_checkheaders(তথ্য, STRCONST("Content-Type"))) {
          ফলাফল = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          যদি(ফলাফল)
            ফেরত ফলাফল;
        }
      }

      যদি(rtspreq == RTSPREQ_ANNOUNCE) {
        যদি(!Curl_checkheaders(তথ্য, STRCONST("Content-Type"))) {
          ফলাফল = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          যদি(ফলাফল)
            ফেরত ফলাফল;
        }
      }
    }
    অন্যথা যদি(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      তথ্য->state.httpreq = HTTPREQ_শীর্ষ;
      তথ্য->req.no_body = TRUE;
    }
  }
  অন্যথা
    ফলাফল = Curl_creader_set_null(তথ্য);
  ফেরত ফলাফল;
}

কাঠামো rtspselect {
  ধ্রুবক অক্ষর *method;
  bool no_body;
};

স্থির CURLcode pick_method(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক unsigned অক্ষর rtspreq,
                            ধ্রুবক অক্ষর **p)
{
  স্থির ধ্রুবক কাঠামো rtspselect req[] = {
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
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  তথ্য->req.no_body = req[rtspreq - 1].no_body;
  ফেরত CURLE_OK;
}

/* Allocate and store a header string. */
স্থির CURLcode rtsp_header_alloc(ধ্রুবক অক্ষর *header_name,
                                  ধ্রুবক অক্ষর *মান,
                                  অক্ষর **target)
{
  যদি(!মান)
    ফেরত CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, মান);
  যদি(!*target)
    ফেরত CURLE_OUT_OF_MEMORY;
  ফেরত CURLE_OK;
}

কাঠামো rtsp_blocks {
  ধ্রুবক অক্ষর *request;
  ধ্রুবক অক্ষর *session_id;
  ধ্রুবক অক্ষর *accept;
  ধ্রুবক অক্ষর *range;
  ধ্রুবক অক্ষর *stream_uri;
  ধ্রুবক অক্ষর *hd_proxy_auth;
  ধ্রুবক অক্ষর *hd_auth;
  অক্ষর *referrer;
  অক্ষর *accept_encoding;
  অক্ষর *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

স্থির CURLcode rtsp_setup_request(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো rtsp_blocks *b,
                                   ধ্রুবক unsigned অক্ষর rtspreq)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  b->session_id = CURL_EASY_STR(তথ্য, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  যদি(CURL_EASY_STR(তথ্য, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(তথ্য, STRING_RTSP_STREAM_URI);
  অন্যথা
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(তথ্য, STRCONST("Transport"));
  যদি(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    যদি(CURL_EASY_STR(তথ্য, STRING_RTSP_TRANSPORT)) {
      ফলাফল = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(তথ্য, STRING_RTSP_TRANSPORT),
        &b->transport);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      b->transport_alloc = TRUE;
    }
    অন্যথা {
      failf(তথ্য,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  যদি(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(তথ্য, STRCONST("Accept")) ?
      ফাঁকা : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    যদি(!Curl_checkheaders(তথ্য, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(তথ্য, STRING_ENCODING)) {
      ফলাফল = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(তথ্য, STRING_ENCODING),
                                 &b->accept_encoding);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  /* setup the authentication headers */
  ফলাফল = Curl_http_output_auth(তথ্য, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, ফাঁকা, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

#ifndef CURL_DISABLE_PROXY
  b->hd_proxy_auth = তথ্য->req.hd_proxy_auth;
#endif
  b->hd_auth = তথ্য->req.hd_auth;

  /* Referrer */
  যদি(Curl_bufref_ptr(&তথ্য->state.referer) &&
     !Curl_checkheaders(তথ্য, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&তথ্য->state.referer));
    যদি(!b->referrer)
      ফলাফল = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  যদি(!ফলাফল &&
     তথ্য->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(তথ্য, STRCONST("Range")) && তথ্য->state.range) {
    ফলাফল = rtsp_header_alloc("Range",
                               তথ্য->state.range,
                               &তথ্য->state.rangeline);
    যদি(!ফলাফল)
      b->range = তথ্য->state.rangeline;
  }
  ফেরত ফলাফল;
}

#define HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

স্থির CURLcode rtsp_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক unsigned অক্ষর rtspreq = তথ্য->set.rtspreq;
  ধ্রুবক অক্ষর *str;
  কাঠামো RTSP *rtsp = Curl_meta_get(তথ্য, CURL_META_RTSP_EASY);
  কাঠামো dynbuf req_বাফার;
  কাঠামো rtsp_blocks block;
  memset(&block, 0, আকার(block));

  *done = TRUE;
  যদি(!rtsp)
    ফেরত CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_বাফার, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = তথ্য->state.rtsp_পরবর্তী_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  ফলাফল = pick_method(তথ্য, rtspreq, &block.request);
  যদি(ফলাফল)
    যাও out;

  যদি(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
    যাও out;
  }

  ফলাফল = rtsp_setup_request(তথ্য, &block,  rtspreq);
  যদি(ফলাফল)
    যাও out;

  যদি(block.session_id && তথ্য->state.rtsp_session_origin &&
     !তথ্য->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(তথ্য->state.origin, তথ্য->state.rtsp_session_origin))
    block.session_id = ফাঁকা;

  /*
   * Sanity check the custom headers
   */
  যদি(Curl_checkheaders(তথ্য, STRCONST("CSeq"))) {
    failf(তথ্য, "CSeq cannot be set as a custom header.");
    ফলাফল = CURLE_RTSP_CSEQ_ERROR;
    যাও out;
  }
  যদি(Curl_checkheaders(তথ্য, STRCONST("Session"))) {
    failf(তথ্য, "Session ID cannot be set as a custom header.");
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }

  ফলাফল =
    curlx_dyn_addf(&req_বাফার,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  যদি(ফলাফল)
    যাও out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  যদি(block.session_id) {
    ফলাফল = curlx_dyn_addf(&req_বাফার, "Session: %s\r\n", block.session_id);
    যদি(ফলাফল)
      যাও out;
  }

  /*
   * Shared HTTP-like options
   */
  ফলাফল = curlx_dyn_addf(&req_বাফার,
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

  str = CURL_EASY_STR(তথ্য, STRING_USERAGENT);
  যদি(!ফলাফল && str && *str &&
     !Curl_checkheaders(তথ্য, STRCONST("User-Agent")))
    ফলাফল = curlx_dyn_addf(&req_বাফার,
                            "User-Agent: %s\r\n", str);

  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addf(&req_বাফার,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  যদি(ফলাফল)
    যাও out;

  যদি((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    ফলাফল = Curl_add_timecondition(তথ্য, &req_বাফার);
    যদি(ফলাফল)
      যাও out;
  }

  ফলাফল = Curl_add_custom_headers(তথ্য, FALSE, HTTPVERSION, &req_বাফার);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = rtsp_setup_body(তথ্য, rtspreq, &req_বাফার);
  যদি(ফলাফল)
    যাও out;

  /* Finish the request buffer */
  ফলাফল = curlx_dyn_addn(&req_বাফার, STRCONST("\r\n"));
  যদি(ফলাফল)
    যাও out;

  Curl_xfer_setup_sendrecv(তথ্য, FIRSTSOCKET, -1);

  /* issue the request */
  ফলাফল = Curl_req_send(তথ্য, &req_বাফার, HTTPVERSION);
  যদি(ফলাফল) {
    failf(তথ্য, "Failed sending RTSP request");
    যাও out;
  }

  /* Increment the CSeq on success */
  তথ্য->state.rtsp_পরবর্তী_client_CSeq++;

  যদি(তথ্য->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_Set_Upload_গণক(তথ্য, তথ্য->req.writebytecount);
    ফলাফল = Curl_pgrsUpdate(তথ্য);
  }
out:
  যদি(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_বাফার);
  ফেরত ফলাফল;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
স্থির CURLcode rtp_write_body_junk(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো rtsp_conn *rtspc,
                                    ধ্রুবক অক্ষর *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (তথ্য->req.headerline && !rtspc->in_header) &&
            (তথ্য->req.আকার_2 >= 0) &&
            (তথ্য->req.bytecount < তথ্য->req.আকার_2);
  body_remain = in_body ? (তথ্য->req.আকার_2 - তথ্য->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  যদি(body_remain) {
    যদি((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    ফেরত Curl_client_write(তথ্য, CLIENTWRITE_BODY, buf, blen);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode rtp_client_write(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *ptr,
                                 size_t len)
{
  কাঠামো Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  শূন্য *user_ptr;

  যদি(len == 0) {
    failf(তথ্য, "Cannot write a 0 size RTP packet.");
    ফেরত CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  যদি(তথ্য->set.fwrite_rtp) {
    writeit = তথ্য->set.fwrite_rtp;
    user_ptr = তথ্য->set.rtp_out;
  }
  অন্যথা {
    writeit = তথ্য->set.fwrite_func;
    user_ptr = তথ্য->set.out;
  }

  CURL_CBAPI_START(&guard, তথ্য, easy_fwrite_rtp);
  wrote = writeit((অক্ষর *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  যদি(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(তথ্য, "Cannot pause RTP");
    ফেরত CURLE_WRITE_ERROR;
  }

  যদি(wrote != len) {
    failf(তথ্য, "Failed writing RTP data");
    ফেরত CURLE_WRITE_ERROR;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode rtsp_filter_rtp(কাঠামো Curl_easy *তথ্য,
                                কাঠামো rtsp_conn *rtspc,
                                ধ্রুবক অক্ষর *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  যখন(blen) {
    bool in_body = (তথ্য->req.headerline && !rtspc->in_header) &&
                   (তথ্য->req.আকার_2 >= 0) &&
                   (তথ্য->req.bytecount < তথ্য->req.আকার_2);
    নির্বাচন(rtspc->state) {

    ক্ষেত্র RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      যখন(blen && buf[0] != '$') {
        যদি(!in_body && buf[0] == 'R' &&
           তথ্য->set.rtspreq != RTSPREQ_RECEIVE) {
          যদি(!strncmp(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            যদি(*pconsumed) {
              DEBUGF(infof(তথ্য, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            যাও out;
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
          ফলাফল = rtp_write_body_junk(তথ্য, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          যদি(ফলাফল)
            যাও out;
        }
        যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          ফলাফল = CURLE_OUT_OF_MEMORY;
          যাও out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      বিরতি;
    }

    ক্ষেত্র RTP_PARSE_CHANNEL: {
      পূর্ণসংখ্যা idx = ((unsigned অক্ষর)buf[0]) / 8;
      পূর্ণসংখ্যা off = ((unsigned অক্ষর)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      যদি(!(তথ্য->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(তথ্য, "RTSP: invalid RTP channel %d, skipping", idx));
        যদি(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          ফলাফল = rtp_write_body_junk(তথ্য, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          যদি(ফলাফল)
            যাও out;
        }
        অন্যথা {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        বিরতি;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (unsigned অক্ষর)buf[0];
      যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      বিরতি;
    }

    ক্ষেত্র RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ধ্রুবক অক্ষর *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      যদি(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      যদি(rtp_len == 2)
        বিরতি;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_দৈর্ঘ্য(rtp_buf) + 4;
      যদি(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(তথ্য, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ফলাফল = rtp_client_write(তথ্য, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        যদি(ফলাফল)
          যাও out;
        বিরতি;
      }
      rtspc->state = RTP_PARSE_তথ্য;
      বিরতি;
    }

    ক্ষেত্র RTP_PARSE_তথ্য: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      যদি(needed <= blen) {
        যদি(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          ফলাফল = CURLE_OUT_OF_MEMORY;
          যাও out;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(তথ্য, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ফলাফল = rtp_client_write(তথ্য, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        যদি(ফলাফল)
          যাও out;
      }
      অন্যথা {
        যদি(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          ফলাফল = CURLE_OUT_OF_MEMORY;
          যাও out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      বিরতি;
    }

    পূর্বনির্ধারিত:
      DEBUGASSERT(0);
      ফেরত CURLE_RECV_ERROR;
    }
  }
out:
  যদি(!ফলাফল && skip_len)
    ফলাফল = rtp_write_body_junk(তথ্য, rtspc, buf - skip_len, skip_len);
  ফেরত ফলাফল;
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
স্থির CURLcode rtsp_rtp_write_resp(কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক অক্ষর *buf,
                                    size_t blen,
                                    bool is_eos)
{
  কাঠামো rtsp_conn *rtspc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_RTSP_CONN);
  CURLcode ফলাফল = CURLE_OK;
  size_t consumed = 0;

  যদি(!rtspc)
    ফেরত CURLE_FAILED_INIT;

  যদি(!তথ্য->req.header)
    rtspc->in_header = FALSE;
  যদি(!blen) {
    যাও out;
  }

  DEBUGF(infof(তথ্য, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  যদি(!rtspc->in_header) {
    ফলাফল = rtsp_filter_rtp(তথ্য, rtspc, buf, blen, &consumed);
    যদি(ফলাফল)
      যাও out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    যদি(blen && !তথ্য->req.header)
      DEBUGF(infof(তথ্য, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  যদি(তথ্য->req.header && blen) {
    rtspc->in_header = TRUE;
    ফলাফল = Curl_http_write_resp_hds(তথ্য, buf, blen, &consumed);
    যদি(ফলাফল)
      যাও out;

    buf += consumed;
    blen -= consumed;

    যদি(!তথ্য->req.header)
      rtspc->in_header = FALSE;

    যদি(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      যদি(তথ্য->req.আকার_2 <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        তথ্য->req.আকার_2 = 0;
        তথ্য->req.download_done = TRUE;
      }
      ফলাফল = rtsp_filter_rtp(তথ্য, rtspc, buf, blen, &consumed);
      যদি(ফলাফল)
        যাও out;
      buf += consumed;
      blen -= consumed;
    }
  }

  যদি(rtspc->state != RTP_PARSE_SKIP)
    তথ্য->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(তথ্য, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, তথ্য->req.done, (পূর্ণসংখ্যা)rtspc->state,
               তথ্য->req.আকার_2));
  যদি(!ফলাফল && (is_eos || blen)) {
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  যদি((তথ্য->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    তথ্য->req.download_done = TRUE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode rtsp_rtp_write_resp_hd(কাঠামো Curl_easy *তথ্য,
                                       ধ্রুবক অক্ষর *buf,
                                       size_t blen,
                                       bool is_eos)
{
  ফেরত rtsp_rtp_write_resp(তথ্য, buf, blen, is_eos);
}

স্থির CURLcode rtsp_parse_transport(কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক অক্ষর *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ধ্রুবক অক্ষর *start, *end;
  start = transport;
  যখন(start && *start) {
    curlx_str_passblanks(&start);
    end = strchr(start, ';');
    যদি(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ধ্রুবক অক্ষর *p = start + 12;
      যদি(!curlx_str_সংখ্যা(&p, &chan1, 255)) {
        unsigned অক্ষর *rtp_channel_mask = তথ্য->state.rtp_channel_mask;
        chan2 = chan1;
        যদি(!curlx_str_single(&p, '-') && curlx_str_সংখ্যা(&p, &chan2, 255)) {
          infof(তথ্য, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        জন্য(chan = chan1; chan <= chan2; chan++) {
          পূর্ণসংখ্যা idx = (পূর্ণসংখ্যা)chan / 8;
          পূর্ণসংখ্যা off = (পূর্ণসংখ্যা)chan % 8;
          rtp_channel_mask[idx] |= (unsigned অক্ষর)(1 << off);
        }
      }
      অন্যথা {
        infof(তথ্য, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      বিরতি;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *header)
{
  যদি(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    কাঠামো RTSP *rtsp = Curl_meta_get(তথ্য, CURL_META_RTSP_EASY);
    ধ্রুবক অক্ষর *p = &header[5];
    যদি(!rtsp)
      ফেরত CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    যদি(curlx_str_সংখ্যা(&p, &CSeq, UINT_MAX)) {
      failf(তথ্য, "Unable to read the CSeq header: [%s]", header);
      ফেরত CURLE_RTSP_CSEQ_ERROR;
    }
    তথ্য->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  অন্যথা যদি(checkprefix("Session:", header)) {
    ধ্রুবক অক্ষর *start, *end, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    যদি(!*start) {
      failf(তথ্য, "Got a blank Session ID");
      ফেরত CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    যখন((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(তথ্য, STRING_RTSP_SESSION_ID);
    যদি(str) {

      /* If the Session ID is set, then compare */
      যদি(strlen(str) != idlen ||
         strncmp(start, str, idlen)) {
        failf(তথ্য, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        ফেরত CURLE_RTSP_SESSION_ERROR;
      }
    }
    অন্যথা {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      শূন্য *mem = curlx_memdup0(start, idlen);
      যদি(!mem ||
         CURL_EASY_STR_SETN(তথ্য, STRING_RTSP_SESSION_ID, mem))
        ফেরত CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&তথ্য->state.rtsp_session_origin, তথ্য->state.origin);
    }
  }
  অন্যথা যদি(checkprefix("Transport:", header)) {
    CURLcode ফলাফল;
    ফলাফল = rtsp_parse_transport(তথ্য, header + 10);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_rtsp = {
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
