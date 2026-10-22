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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "rtsp.h"

#अगर_अघोषित CURL_DISABLE_RTSP

#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "http.h"
#समावेश "url.h"
#समावेश "progress.h"
#समावेश "strcase.h"
#समावेश "select.h"
#समावेश "connect.h"
#समावेश "cfilters.h"
#समावेश "curlx/strdup.h"
#समावेश "bufref.h"
#समावेश "curlx/strparse.h"
#समावेश "peer.h"

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

प्रकार क्रमागत {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_तथ्य
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
काष्ठा rtsp_conn {
  काष्ठा dynbuf buf;
  पूर्णांक rtp_channel;
  माप_प्रकार rtp_len;
  rtp_parse_st state;
  BIT(in_header);
};

/* RTSP transfer data */
काष्ठा RTSP {
  uint32_t CSeq_sent; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#घोषणा RTP_PKT_लंबाई(p) ((((अचिन्हित पूर्णांक)((अचिन्हित अक्षर)((p)[2]))) << 8) | \
                            ((अचिन्हित पूर्णांक)((अचिन्हित अक्षर)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
अटल CURLcode rtsp_do_pollset(काष्ठा Curl_easy *तथ्य,
                                काष्ठा easy_pollset *ps)
{
  /* write mode */
  वापस Curl_pollset_add_out(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
}

#घोषणा MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

अटल व्योम rtsp_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा RTSP *rtsp = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_free(rtsp);
}

अटल व्योम rtsp_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा rtsp_conn *rtspc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

अटल CURLcode rtsp_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा rtsp_conn *rtspc;
  काष्ठा RTSP *rtsp;

  rtspc = curlx_calloc(1, माप(*rtspc));
  अगर(!rtspc)
    वापस CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  अगर(Curl_conn_meta_set(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  rtsp = curlx_calloc(1, माप(काष्ठा RTSP));
  अगर(!rtsp ||
     Curl_meta_set(तथ्य, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
अटल bool rtsp_conn_is_dead(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  bool निवेश_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  वापस !Curl_conn_is_alive(तथ्य, conn, &निवेश_pending);
}

अटल CURLcode rtsp_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा rtsp_conn *rtspc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_RTSP_CONN);

  अगर(!rtspc)
    वापस CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  अगर(तथ्य->state.rtsp_अग्र_client_CSeq == 0)
    तथ्य->state.rtsp_अग्र_client_CSeq = 1;
  अगर(तथ्य->state.rtsp_अग्र_server_CSeq == 0)
    तथ्य->state.rtsp_अग्र_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *done = TRUE;
  वापस CURLE_OK;
}

अटल CURLcode rtsp_done(काष्ठा Curl_easy *तथ्य,
                          CURLcode status, bool premature)
{
  काष्ठा rtsp_conn *rtspc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_RTSP_CONN);
  काष्ठा RTSP *rtsp = Curl_meta_get(तथ्य, CURL_META_RTSP_EASY);
  CURLcode परिणाम;

  अगर(!rtspc || !rtsp)
    वापस CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  अगर(तथ्य->set.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  परिणाम = Curl_http_done(तथ्य, status, premature);

  अगर(!status && !परिणाम) {
    /* Check the sequence numbers */
    uint32_t CSeq_sent = rtsp->CSeq_sent;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    अगर((तथ्य->set.rtspreq != RTSPREQ_RECEIVE) && (CSeq_sent != CSeq_recv)) {
      failf(तथ्य,
            "The CSeq of this request %u did not match the response %u",
            CSeq_sent, CSeq_recv);
      वापस CURLE_RTSP_CSEQ_ERROR;
    }
    अगर(तथ्य->set.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(तथ्य, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    अगर(तथ्य->set.rtspreq == RTSPREQ_RECEIVE &&
       तथ्य->req.eos_written) {
      failf(तथ्य, "Server prematurely closed the RTSP connection.");
      वापस CURLE_RECV_ERROR;
    }
  }

  वापस परिणाम;
}

अटल CURLcode rtsp_setup_body(काष्ठा Curl_easy *तथ्य,
                                अचिन्हित अक्षर rtspreq,
                                काष्ठा dynbuf *reqp)
{
  CURLcode परिणाम;
  अगर(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_SET_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    अगर(तथ्य->state.upload) {
      req_clen = तथ्य->state.infilesize;
      तथ्य->state.httpreq = HTTPREQ_PUT;
      परिणाम = Curl_creader_set_fread(तथ्य, req_clen);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा {
      अगर(तथ्य->set.postfields) {
        माप_प्रकार plen = (तथ्य->set.postfieldsize >= 0) ?
          (माप_प्रकार)तथ्य->set.postfieldsize : म_माप(तथ्य->set.postfields);
        req_clen = (curl_off_t)plen;
        परिणाम = Curl_creader_set_buf(तथ्य, तथ्य->set.postfields, plen);
      }
      अन्यथा अगर(तथ्य->state.infilesize >= 0) {
        req_clen = तथ्य->state.infilesize;
        परिणाम = Curl_creader_set_fread(तथ्य, req_clen);
      }
      अन्यथा {
        req_clen = 0;
        परिणाम = Curl_creader_set_null(तथ्य);
      }
      अगर(परिणाम)
        वापस परिणाम;
    }

    अगर(req_clen > 0) {
      अगर(तथ्य->set.crlf) {
        failf(तथ्य, "Cannot do CRLF conversions on upload");
        वापस CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      अगर(!Curl_checkheaders(तथ्य, STRCONST("Content-Length"))) {
        परिणाम = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        अगर(परिणाम)
          वापस परिणाम;
      }

      अगर(rtspreq == RTSPREQ_SET_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        अगर(!Curl_checkheaders(तथ्य, STRCONST("Content-Type"))) {
          परिणाम = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          अगर(परिणाम)
            वापस परिणाम;
        }
      }

      अगर(rtspreq == RTSPREQ_ANNOUNCE) {
        अगर(!Curl_checkheaders(तथ्य, STRCONST("Content-Type"))) {
          परिणाम = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          अगर(परिणाम)
            वापस परिणाम;
        }
      }
    }
    अन्यथा अगर(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      तथ्य->state.httpreq = HTTPREQ_शीर्ष;
      तथ्य->req.no_body = TRUE;
    }
  }
  अन्यथा
    परिणाम = Curl_creader_set_null(तथ्य);
  वापस परिणाम;
}

काष्ठा rtspselect {
  स्थिर अक्षर *method;
  bool no_body;
};

अटल CURLcode pick_method(काष्ठा Curl_easy *तथ्य,
                            स्थिर अचिन्हित अक्षर rtspreq,
                            स्थिर अक्षर **p)
{
  अटल स्थिर काष्ठा rtspselect req[] = {
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
  अगर((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_LAST))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  *p = req[rtspreq - 1].method;
  तथ्य->req.no_body = req[rtspreq - 1].no_body;
  वापस CURLE_OK;
}

/* Allocate and store a header string. */
अटल CURLcode rtsp_header_alloc(स्थिर अक्षर *header_name,
                                  स्थिर अक्षर *मूल्य,
                                  अक्षर **target)
{
  अगर(!मूल्य)
    वापस CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", header_name, मूल्य);
  अगर(!*target)
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
}

काष्ठा rtsp_blocks {
  स्थिर अक्षर *request;
  स्थिर अक्षर *session_id;
  स्थिर अक्षर *accept;
  स्थिर अक्षर *range;
  स्थिर अक्षर *stream_uri;
  स्थिर अक्षर *hd_proxy_auth;
  स्थिर अक्षर *hd_auth;
  अक्षर *referrer;
  अक्षर *accept_encoding;
  अक्षर *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

अटल CURLcode rtsp_setup_request(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा rtsp_blocks *b,
                                   स्थिर अचिन्हित अक्षर rtspreq)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  b->session_id = CURL_EASY_STR(तथ्य, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  अगर(CURL_EASY_STR(तथ्य, STRING_RTSP_STREAM_URI))
    b->stream_uri = CURL_EASY_STR(तथ्य, STRING_RTSP_STREAM_URI);
  अन्यथा
    b->stream_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(तथ्य, STRCONST("Transport"));
  अगर(rtspreq == RTSPREQ_SETUP && !b->transport) {
    /* New Transport: setting? */
    अगर(CURL_EASY_STR(तथ्य, STRING_RTSP_TRANSPORT)) {
      परिणाम = rtsp_header_alloc(
        "Transport", CURL_EASY_STR(तथ्य, STRING_RTSP_TRANSPORT),
        &b->transport);
      अगर(परिणाम)
        वापस परिणाम;
      b->transport_alloc = TRUE;
    }
    अन्यथा {
      failf(तथ्य,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  अगर(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(तथ्य, STRCONST("Accept")) ?
      शून्य : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    अगर(!Curl_checkheaders(तथ्य, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(तथ्य, STRING_ENCODING)) {
      परिणाम = rtsp_header_alloc("Accept-Encoding",
                                 CURL_EASY_STR(तथ्य, STRING_ENCODING),
                                 &b->accept_encoding);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  /* setup the authentication headers */
  परिणाम = Curl_http_output_auth(तथ्य, conn, b->request, HTTPREQ_GET,
                                 b->stream_uri, शून्य, FALSE);
  अगर(परिणाम)
    वापस परिणाम;

#अगर_अघोषित CURL_DISABLE_PROXY
  b->hd_proxy_auth = तथ्य->req.hd_proxy_auth;
#पूर्ण_अगर
  b->hd_auth = तथ्य->req.hd_auth;

  /* Referrer */
  अगर(Curl_bufref_ptr(&तथ्य->state.referer) &&
     !Curl_checkheaders(तथ्य, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&तथ्य->state.referer));
    अगर(!b->referrer)
      परिणाम = CURLE_OUT_OF_MEMORY;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  अगर(!परिणाम &&
     तथ्य->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_RECORD)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(तथ्य, STRCONST("Range")) && तथ्य->state.range) {
    परिणाम = rtsp_header_alloc("Range",
                               तथ्य->state.range,
                               &तथ्य->state.rangeline);
    अगर(!परिणाम)
      b->range = तथ्य->state.rangeline;
  }
  वापस परिणाम;
}

#घोषणा HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

अटल CURLcode rtsp_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अचिन्हित अक्षर rtspreq = तथ्य->set.rtspreq;
  स्थिर अक्षर *str;
  काष्ठा RTSP *rtsp = Curl_meta_get(तथ्य, CURL_META_RTSP_EASY);
  काष्ठा dynbuf req_बफर;
  काष्ठा rtsp_blocks block;
  स_रखो(&block, 0, माप(block));

  *done = TRUE;
  अगर(!rtsp)
    वापस CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_बफर, DYN_RTSP_REQ_HEADER);

  rtsp->CSeq_sent = तथ्य->state.rtsp_अग्र_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  परिणाम = pick_method(तथ्य, rtspreq, &block.request);
  अगर(परिणाम)
    जाओ out;

  अगर(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
    जाओ out;
  }

  परिणाम = rtsp_setup_request(तथ्य, &block,  rtspreq);
  अगर(परिणाम)
    जाओ out;

  अगर(block.session_id && तथ्य->state.rtsp_session_origin &&
     !तथ्य->set.allow_auth_to_other_hosts &&
     !Curl_peer_equal(तथ्य->state.origin, तथ्य->state.rtsp_session_origin))
    block.session_id = शून्य;

  /*
   * Sanity check the custom headers
   */
  अगर(Curl_checkheaders(तथ्य, STRCONST("CSeq"))) {
    failf(तथ्य, "CSeq cannot be set as a custom header.");
    परिणाम = CURLE_RTSP_CSEQ_ERROR;
    जाओ out;
  }
  अगर(Curl_checkheaders(तथ्य, STRCONST("Session"))) {
    failf(तथ्य, "Session ID cannot be set as a custom header.");
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }

  परिणाम =
    curlx_dyn_addf(&req_बफर,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   block.request, block.stream_uri, rtsp->CSeq_sent);
  अगर(परिणाम)
    जाओ out;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  अगर(block.session_id) {
    परिणाम = curlx_dyn_addf(&req_बफर, "Session: %s\r\n", block.session_id);
    अगर(परिणाम)
      जाओ out;
  }

  /*
   * Shared HTTP-like options
   */
  परिणाम = curlx_dyn_addf(&req_बफर,
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

  str = CURL_EASY_STR(तथ्य, STRING_USERAGENT);
  अगर(!परिणाम && str && *str &&
     !Curl_checkheaders(तथ्य, STRCONST("User-Agent")))
    परिणाम = curlx_dyn_addf(&req_बफर,
                            "User-Agent: %s\r\n", str);

  अगर(!परिणाम)
    परिणाम = curlx_dyn_addf(&req_बफर,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            block.hd_proxy_auth ? block.hd_proxy_auth : "",
                            block.hd_auth ? block.hd_auth : "");
  अगर(परिणाम)
    जाओ out;

  अगर((rtspreq == RTSPREQ_SETUP) || (rtspreq == RTSPREQ_DESCRIBE)) {
    परिणाम = Curl_add_timecondition(तथ्य, &req_बफर);
    अगर(परिणाम)
      जाओ out;
  }

  परिणाम = Curl_add_custom_headers(तथ्य, FALSE, HTTPVERSION, &req_बफर);
  अगर(परिणाम)
    जाओ out;

  परिणाम = rtsp_setup_body(तथ्य, rtspreq, &req_बफर);
  अगर(परिणाम)
    जाओ out;

  /* Finish the request buffer */
  परिणाम = curlx_dyn_addn(&req_बफर, STRCONST("\r\n"));
  अगर(परिणाम)
    जाओ out;

  Curl_xfer_setup_sendrecv(तथ्य, FIRSTSOCKET, -1);

  /* issue the request */
  परिणाम = Curl_req_send(तथ्य, &req_बफर, HTTPVERSION);
  अगर(परिणाम) {
    failf(तथ्य, "Failed sending RTSP request");
    जाओ out;
  }

  /* Increment the CSeq on success */
  तथ्य->state.rtsp_अग्र_client_CSeq++;

  अगर(तथ्य->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_Set_Upload_गणक(तथ्य, तथ्य->req.writebytecount);
    परिणाम = Curl_pgrsUpdate(तथ्य);
  }
out:
  अगर(block.transport_alloc)
    curlx_free(block.transport);
  curlx_free(block.accept_encoding);
  curlx_free(block.referrer);
  curlx_dyn_free(&req_बफर);
  वापस परिणाम;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
अटल CURLcode rtp_write_body_junk(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा rtsp_conn *rtspc,
                                    स्थिर अक्षर *buf,
                                    माप_प्रकार blen)
{
  curl_off_t body_remain;
  bool in_body;

  in_body = (तथ्य->req.headerline && !rtspc->in_header) &&
            (तथ्य->req.आकार >= 0) &&
            (तथ्य->req.bytecount < तथ्य->req.आकार);
  body_remain = in_body ? (तथ्य->req.आकार - तथ्य->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  अगर(body_remain) {
    अगर((curl_off_t)blen > body_remain)
      blen = (माप_प्रकार)body_remain;
    वापस Curl_client_write(तथ्य, CLIENTWRITE_BODY, buf, blen);
  }
  वापस CURLE_OK;
}

अटल CURLcode rtp_client_write(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *ptr,
                                 माप_प्रकार len)
{
  काष्ठा Curl_mapi_guard guard;
  माप_प्रकार wrote;
  curl_write_callback writeit;
  व्योम *user_ptr;

  अगर(len == 0) {
    failf(तथ्य, "Cannot write a 0 size RTP packet.");
    वापस CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  अगर(तथ्य->set.fwrite_rtp) {
    writeit = तथ्य->set.fwrite_rtp;
    user_ptr = तथ्य->set.rtp_out;
  }
  अन्यथा {
    writeit = तथ्य->set.fwrite_func;
    user_ptr = तथ्य->set.out;
  }

  CURL_CBAPI_START(&guard, तथ्य, easy_fwrite_rtp);
  wrote = writeit((अक्षर *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_END(&guard);

  अगर(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(तथ्य, "Cannot pause RTP");
    वापस CURLE_WRITE_ERROR;
  }

  अगर(wrote != len) {
    failf(तथ्य, "Failed writing RTP data");
    वापस CURLE_WRITE_ERROR;
  }

  वापस CURLE_OK;
}

अटल CURLcode rtsp_filter_rtp(काष्ठा Curl_easy *तथ्य,
                                काष्ठा rtsp_conn *rtspc,
                                स्थिर अक्षर *buf,
                                माप_प्रकार blen,
                                माप_प्रकार *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार skip_len = 0;

  *pconsumed = 0;
  जबतक(blen) {
    bool in_body = (तथ्य->req.headerline && !rtspc->in_header) &&
                   (तथ्य->req.आकार >= 0) &&
                   (तथ्य->req.bytecount < तथ्य->req.आकार);
    चयन(rtspc->state) {

    हाल RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      जबतक(blen && buf[0] != '$') {
        अगर(!in_body && buf[0] == 'R' &&
           तथ्य->set.rtspreq != RTSPREQ_RECEIVE) {
          अगर(!म_भेदन(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            अगर(*pconsumed) {
              DEBUGF(infof(तथ्य, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->in_header = TRUE;
            जाओ out;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      अगर(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        अगर(skip_len) {
          /* end of junk/BODY bytes, flush */
          परिणाम = rtp_write_body_junk(तथ्य, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          अगर(परिणाम)
            जाओ out;
        }
        अगर(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          परिणाम = CURLE_OUT_OF_MEMORY;
          जाओ out;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      अवरोध;
    }

    हाल RTP_PARSE_CHANNEL: {
      पूर्णांक idx = ((अचिन्हित अक्षर)buf[0]) / 8;
      पूर्णांक off = ((अचिन्हित अक्षर)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      अगर(!(तथ्य->state.rtp_channel_mask[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(तथ्य, "RTSP: invalid RTP channel %d, skipping", idx));
        अगर(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          परिणाम = rtp_write_body_junk(तथ्य, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          अगर(परिणाम)
            जाओ out;
        }
        अन्यथा {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        अवरोध;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (अचिन्हित अक्षर)buf[0];
      अगर(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      अवरोध;
    }

    हाल RTP_PARSE_LEN: {
      माप_प्रकार rtp_len = curlx_dyn_len(&rtspc->buf);
      स्थिर अक्षर *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      अगर(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      अगर(rtp_len == 2)
        अवरोध;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_लंबाई(rtp_buf) + 4;
      अगर(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(तथ्य, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        परिणाम = rtp_client_write(तथ्य, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        अगर(परिणाम)
          जाओ out;
        अवरोध;
      }
      rtspc->state = RTP_PARSE_तथ्य;
      अवरोध;
    }

    हाल RTP_PARSE_तथ्य: {
      माप_प्रकार rtp_len = curlx_dyn_len(&rtspc->buf);
      माप_प्रकार needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      अगर(needed <= blen) {
        अगर(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          परिणाम = CURLE_OUT_OF_MEMORY;
          जाओ out;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(तथ्य, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        परिणाम = rtp_client_write(तथ्य, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        अगर(परिणाम)
          जाओ out;
      }
      अन्यथा {
        अगर(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          परिणाम = CURLE_OUT_OF_MEMORY;
          जाओ out;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      अवरोध;
    }

    शेष:
      DEBUGASSERT(0);
      वापस CURLE_RECV_ERROR;
    }
  }
out:
  अगर(!परिणाम && skip_len)
    परिणाम = rtp_write_body_junk(तथ्य, rtspc, buf - skip_len, skip_len);
  वापस परिणाम;
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
अटल CURLcode rtsp_rtp_write_resp(काष्ठा Curl_easy *तथ्य,
                                    स्थिर अक्षर *buf,
                                    माप_प्रकार blen,
                                    bool is_eos)
{
  काष्ठा rtsp_conn *rtspc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_RTSP_CONN);
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार consumed = 0;

  अगर(!rtspc)
    वापस CURLE_FAILED_INIT;

  अगर(!तथ्य->req.header)
    rtspc->in_header = FALSE;
  अगर(!blen) {
    जाओ out;
  }

  DEBUGF(infof(तथ्य, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->in_header, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  अगर(!rtspc->in_header) {
    परिणाम = rtsp_filter_rtp(तथ्य, rtspc, buf, blen, &consumed);
    अगर(परिणाम)
      जाओ out;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    अगर(blen && !तथ्य->req.header)
      DEBUGF(infof(तथ्य, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  अगर(तथ्य->req.header && blen) {
    rtspc->in_header = TRUE;
    परिणाम = Curl_http_write_resp_hds(तथ्य, buf, blen, &consumed);
    अगर(परिणाम)
      जाओ out;

    buf += consumed;
    blen -= consumed;

    अगर(!तथ्य->req.header)
      rtspc->in_header = FALSE;

    अगर(!rtspc->in_header) {
      /* If header parsing is done, extract interleaved RTP messages */
      अगर(तथ्य->req.आकार <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        तथ्य->req.आकार = 0;
        तथ्य->req.download_done = TRUE;
      }
      परिणाम = rtsp_filter_rtp(तथ्य, rtspc, buf, blen, &consumed);
      अगर(परिणाम)
        जाओ out;
      buf += consumed;
      blen -= consumed;
    }
  }

  अगर(rtspc->state != RTP_PARSE_SKIP)
    तथ्य->req.done = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(तथ्य, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->in_header, तथ्य->req.done, (पूर्णांक)rtspc->state,
               तथ्य->req.आकार));
  अगर(!परिणाम && (is_eos || blen)) {
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

out:
  अगर((तथ्य->set.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    तथ्य->req.download_done = TRUE;
  }
  वापस परिणाम;
}

अटल CURLcode rtsp_rtp_write_resp_hd(काष्ठा Curl_easy *तथ्य,
                                       स्थिर अक्षर *buf,
                                       माप_प्रकार blen,
                                       bool is_eos)
{
  वापस rtsp_rtp_write_resp(तथ्य, buf, blen, is_eos);
}

अटल CURLcode rtsp_parse_transport(काष्ठा Curl_easy *तथ्य,
                                     स्थिर अक्षर *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  स्थिर अक्षर *start, *end;
  start = transport;
  जबतक(start && *start) {
    curlx_str_passblanks(&start);
    end = म_अक्षर(start, ';');
    अगर(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      स्थिर अक्षर *p = start + 12;
      अगर(!curlx_str_संख्या(&p, &chan1, 255)) {
        अचिन्हित अक्षर *rtp_channel_mask = तथ्य->state.rtp_channel_mask;
        chan2 = chan1;
        अगर(!curlx_str_single(&p, '-') && curlx_str_संख्या(&p, &chan2, 255)) {
          infof(तथ्य, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        क्रम(chan = chan1; chan <= chan2; chan++) {
          पूर्णांक idx = (पूर्णांक)chan / 8;
          पूर्णांक off = (पूर्णांक)chan % 8;
          rtp_channel_mask[idx] |= (अचिन्हित अक्षर)(1 << off);
        }
      }
      अन्यथा {
        infof(तथ्य, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      अवरोध;
    }
    /* skip to next parameter */
    start = (!end) ? end : (end + 1);
  }
  वापस CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *header)
{
  अगर(checkprefix("CSeq:", header)) {
    curl_off_t CSeq = 0;
    काष्ठा RTSP *rtsp = Curl_meta_get(तथ्य, CURL_META_RTSP_EASY);
    स्थिर अक्षर *p = &header[5];
    अगर(!rtsp)
      वापस CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    अगर(curlx_str_संख्या(&p, &CSeq, अच_पूर्णांक_उच्च)) {
      failf(तथ्य, "Unable to read the CSeq header: [%s]", header);
      वापस CURLE_RTSP_CSEQ_ERROR;
    }
    तथ्य->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  अन्यथा अगर(checkprefix("Session:", header)) {
    स्थिर अक्षर *start, *end, *str;
    माप_प्रकार idlen;

    /* Find the first non-space letter */
    start = header + 8;
    curlx_str_passblanks(&start);

    अगर(!*start) {
      failf(तथ्य, "Got a blank Session ID");
      वापस CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    end = start;
    जबतक((*end > ' ') && (*end != ';'))
      end++;
    idlen = end - start;

    str = CURL_EASY_STR(तथ्य, STRING_RTSP_SESSION_ID);
    अगर(str) {

      /* If the Session ID is set, then compare */
      अगर(म_माप(str) != idlen ||
         म_भेदन(start, str, idlen)) {
        failf(तथ्य, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        वापस CURLE_RTSP_SESSION_ERROR;
      }
    }
    अन्यथा {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      व्योम *mem = curlx_memdup0(start, idlen);
      अगर(!mem ||
         CURL_EASY_STR_SETN(तथ्य, STRING_RTSP_SESSION_ID, mem))
        वापस CURLE_OUT_OF_MEMORY;
      Curl_peer_link(&तथ्य->state.rtsp_session_origin, तथ्य->state.origin);
    }
  }
  अन्यथा अगर(checkprefix("Transport:", header)) {
    CURLcode परिणाम;
    परिणाम = rtsp_parse_transport(तथ्य, header + 10);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस CURLE_OK;
}

/*
 * RTSP handler interface.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_rtsp = {
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

#पूर्ण_अगर /* CURL_DISABLE_RTSP */
