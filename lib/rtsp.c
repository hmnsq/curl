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

#यदि_अपरिभाषितम् CURL_DISABLE_RTSP

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
#परिभाषय CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

प्रकारनिर्देश गणना {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_दत्त
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
संरचना rtsp_conn {
  संरचना dynbuf buf;
  पूर्णाङ्क rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  BIT(अन्तर्_शीर्षकम्);
};

/* RTSP transfer data */
संरचना RTSP {
  uint32_t CSeq_प्रेषितम्; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#परिभाषय RTP_PKT_दीर्घता(p) ((((अचिह्नित पूर्णाङ्क)((अचिह्नित वर्ण)((p)[2]))) << 8) | \
                            ((अचिह्नित पूर्णाङ्क)((अचिह्नित वर्ण)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
स्थिर CURLcode rtsp_do_pollset(संरचना Curl_easy *दत्त,
                                संरचना easy_pollset *ps)
{
  /* write mode */
  प्रत्यागम Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
}

#परिभाषय MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

स्थिर शून्य rtsp_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना RTSP *rtsp = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(rtsp);
}

स्थिर शून्य rtsp_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना rtsp_conn *rtspc = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

स्थिर CURLcode rtsp_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना rtsp_conn *rtspc;
  संरचना RTSP *rtsp;

  rtspc = curlx_calloc(1, परिमाण(*rtspc));
  यदि(!rtspc)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  यदि(Curl_conn_meta_समुच्चयः(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  rtsp = curlx_calloc(1, परिमाण(संरचना RTSP));
  यदि(!rtsp ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
स्थिर bool rtsp_conn_is_dead(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  bool निवेशः_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  प्रत्यागम !Curl_conn_is_alive(दत्त, conn, &निवेशः_pending);
}

स्थिर CURLcode rtsp_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_RTSP_CONN);

  यदि(!rtspc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  यदि(दत्त->state.rtsp_अग्र_client_CSeq == 0)
    दत्त->state.rtsp_अग्र_client_CSeq = 1;
  यदि(दत्त->state.rtsp_अग्र_server_CSeq == 0)
    दत्त->state.rtsp_अग्र_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode rtsp_समाप्तम्(संरचना Curl_easy *दत्त,
                          CURLcode स्थितिः, bool premature)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_RTSP_CONN);
  संरचना RTSP *rtsp = Curl_meta_get(दत्त, CURL_META_RTSP_EASY);
  CURLcode परिणाम;

  यदि(!rtspc || !rtsp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  यदि(दत्त->समुच्चयः.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  परिणाम = Curl_http_समाप्तम्(दत्त, स्थितिः, premature);

  यदि(!स्थितिः && !परिणाम) {
    /* Check the sequence numbers */
    uint32_t CSeq_प्रेषितम् = rtsp->CSeq_प्रेषितम्;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    यदि((दत्त->समुच्चयः.rtspreq != RTSPREQ_RECEIVE) && (CSeq_प्रेषितम् != CSeq_recv)) {
      failf(दत्त,
            "The CSeq of this request %u did not match the response %u",
            CSeq_प्रेषितम्, CSeq_recv);
      प्रत्यागम CURLE_RTSP_CSEQ_ERROR;
    }
    यदि(दत्त->समुच्चयः.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(दत्त, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    यदि(दत्त->समुच्चयः.rtspreq == RTSPREQ_RECEIVE &&
       दत्त->req.eos_written) {
      failf(दत्त, "Server prematurely closed the RTSP connection.");
      प्रत्यागम CURLE_RECV_ERROR;
    }
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode rtsp_सज्जता_body(संरचना Curl_easy *दत्त,
                                अचिह्नित वर्ण rtspreq,
                                संरचना dynbuf *reqp)
{
  CURLcode परिणाम;
  यदि(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_समुच्चयः_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    यदि(दत्त->state.upload) {
      req_clen = दत्त->state.infilesize;
      दत्त->state.httpreq = HTTPREQ_PUT;
      परिणाम = Curl_creader_समुच्चयः_fread(दत्त, req_clen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा {
      यदि(दत्त->समुच्चयः.postfields) {
        size_t plen = (दत्त->समुच्चयः.postfieldsize >= 0) ?
          (size_t)दत्त->समुच्चयः.postfieldsize : वाक्यदैर्घ्यम्(दत्त->समुच्चयः.postfields);
        req_clen = (curl_off_t)plen;
        परिणाम = Curl_creader_समुच्चयः_buf(दत्त, दत्त->समुच्चयः.postfields, plen);
      }
      अन्यथा यदि(दत्त->state.infilesize >= 0) {
        req_clen = दत्त->state.infilesize;
        परिणाम = Curl_creader_समुच्चयः_fread(दत्त, req_clen);
      }
      अन्यथा {
        req_clen = 0;
        परिणाम = Curl_creader_समुच्चयः_null(दत्त);
      }
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    यदि(req_clen > 0) {
      यदि(दत्त->समुच्चयः.crlf) {
        failf(दत्त, "Cannot do CRLF conversions on upload");
        प्रत्यागम CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      यदि(!Curl_checkheaders(दत्त, STRCONST("Content-Length"))) {
        परिणाम = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }

      यदि(rtspreq == RTSPREQ_समुच्चयः_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        यदि(!Curl_checkheaders(दत्त, STRCONST("Content-Type"))) {
          परिणाम = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          यदि(परिणाम)
            प्रत्यागम परिणाम;
        }
      }

      यदि(rtspreq == RTSPREQ_ANNOUNCE) {
        यदि(!Curl_checkheaders(दत्त, STRCONST("Content-Type"))) {
          परिणाम = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          यदि(परिणाम)
            प्रत्यागम परिणाम;
        }
      }
    }
    अन्यथा यदि(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      दत्त->state.httpreq = HTTPREQ_शीर्ष;
      दत्त->req.no_body = TRUE;
    }
  }
  अन्यथा
    परिणाम = Curl_creader_समुच्चयः_null(दत्त);
  प्रत्यागम परिणाम;
}

संरचना rtspselect {
  ध्रुव वर्ण *विधानम्;
  bool no_body;
};

स्थिर CURLcode pick_विधानम्(संरचना Curl_easy *दत्त,
                            ध्रुव अचिह्नित वर्ण rtspreq,
                            ध्रुव वर्ण **p)
{
  स्थिर ध्रुव संरचना rtspselect req[] = {
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_चरमः));
  यदि((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_चरमः))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  *p = req[rtspreq - 1].विधानम्;
  दत्त->req.no_body = req[rtspreq - 1].no_body;
  प्रत्यागम CURLE_OK;
}

/* Allocate and store a header string. */
स्थिर CURLcode rtsp_शीर्षकम्_alloc(ध्रुव वर्ण *शीर्षकम्_name,
                                  ध्रुव वर्ण *मानम्,
                                  वर्ण **target)
{
  यदि(!मानम्)
    प्रत्यागम CURLE_OK;
  curlx_free(*target);
  *target = curl_maprintf("%s: %s\r\n", शीर्षकम्_name, मानम्);
  यदि(!*target)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  प्रत्यागम CURLE_OK;
}

संरचना rtsp_खण्डानि {
  ध्रुव वर्ण *request;
  ध्रुव वर्ण *session_id;
  ध्रुव वर्ण *accept;
  ध्रुव वर्ण *range;
  ध्रुव वर्ण *प्रवाहः_uri;
  ध्रुव वर्ण *hd_proxy_auth;
  ध्रुव वर्ण *hd_auth;
  वर्ण *referrer;
  वर्ण *accept_encoding;
  वर्ण *transport;
  BIT(transport_alloc); /* if 'transport' is allocated */
};

स्थिर CURLcode rtsp_सज्जता_request(संरचना Curl_easy *दत्त,
                                   संरचना rtsp_खण्डानि *b,
                                   ध्रुव अचिह्नित वर्ण rtspreq)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  b->session_id = CURL_EASY_STR(दत्त, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  यदि(CURL_EASY_STR(दत्त, STRING_RTSP_प्रवाहः_URI))
    b->प्रवाहः_uri = CURL_EASY_STR(दत्त, STRING_RTSP_प्रवाहः_URI);
  अन्यथा
    b->प्रवाहः_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(दत्त, STRCONST("Transport"));
  यदि(rtspreq == RTSPREQ_सज्जता && !b->transport) {
    /* New Transport: setting? */
    यदि(CURL_EASY_STR(दत्त, STRING_RTSP_TRANSPORT)) {
      परिणाम = rtsp_शीर्षकम्_alloc(
        "Transport", CURL_EASY_STR(दत्त, STRING_RTSP_TRANSPORT),
        &b->transport);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      b->transport_alloc = TRUE;
    }
    अन्यथा {
      failf(दत्त,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  यदि(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(दत्त, STRCONST("Accept")) ?
      रिक्त : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    यदि(!Curl_checkheaders(दत्त, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(दत्त, STRING_ENCODING)) {
      परिणाम = rtsp_शीर्षकम्_alloc("Accept-Encoding",
                                 CURL_EASY_STR(दत्त, STRING_ENCODING),
                                 &b->accept_encoding);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  /* setup the authentication headers */
  परिणाम = Curl_http_output_auth(दत्त, conn, b->request, HTTPREQ_GET,
                                 b->प्रवाहः_uri, रिक्त, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  b->hd_proxy_auth = दत्त->req.hd_proxy_auth;
#यदि_अन्तः
  b->hd_auth = दत्त->req.hd_auth;

  /* Referrer */
  यदि(Curl_bufref_ptr(&दत्त->state.referer) &&
     !Curl_checkheaders(दत्त, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&दत्त->state.referer));
    यदि(!b->referrer)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  यदि(!परिणाम &&
     दत्त->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_लेखः)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(दत्त, STRCONST("Range")) && दत्त->state.range) {
    परिणाम = rtsp_शीर्षकम्_alloc("Range",
                               दत्त->state.range,
                               &दत्त->state.rangeline);
    यदि(!परिणाम)
      b->range = दत्त->state.rangeline;
  }
  प्रत्यागम परिणाम;
}

#परिभाषय HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

स्थिर CURLcode rtsp_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित वर्ण rtspreq = दत्त->समुच्चयः.rtspreq;
  ध्रुव वर्ण *str;
  संरचना RTSP *rtsp = Curl_meta_get(दत्त, CURL_META_RTSP_EASY);
  संरचना dynbuf req_बफर;
  संरचना rtsp_खण्डानि खण्डम्;
  स्मृतिं_पूरय(&खण्डम्, 0, परिमाण(खण्डम्));

  *समाप्तम् = TRUE;
  यदि(!rtsp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_बफर, DYN_RTSP_REQ_शीर्षकम्);

  rtsp->CSeq_प्रेषितम् = दत्त->state.rtsp_अग्र_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  परिणाम = pick_विधानम्(दत्त, rtspreq, &खण्डम्.request);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
    गच्छ बहिर्गमनम्;
  }

  परिणाम = rtsp_सज्जता_request(दत्त, &खण्डम्,  rtspreq);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(खण्डम्.session_id && दत्त->state.rtsp_session_origin &&
     !दत्त->समुच्चयः.allow_auth_गन्तव्यम्_other_hosts &&
     !Curl_peer_equal(दत्त->state.origin, दत्त->state.rtsp_session_origin))
    खण्डम्.session_id = रिक्त;

  /*
   * Sanity check the custom headers
   */
  यदि(Curl_checkheaders(दत्त, STRCONST("CSeq"))) {
    failf(दत्त, "CSeq cannot be set as a custom header.");
    परिणाम = CURLE_RTSP_CSEQ_ERROR;
    गच्छ बहिर्गमनम्;
  }
  यदि(Curl_checkheaders(दत्त, STRCONST("Session"))) {
    failf(दत्त, "Session ID cannot be set as a custom header.");
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम =
    curlx_dyn_addf(&req_बफर,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   खण्डम्.request, खण्डम्.प्रवाहः_uri, rtsp->CSeq_प्रेषितम्);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  यदि(खण्डम्.session_id) {
    परिणाम = curlx_dyn_addf(&req_बफर, "Session: %s\r\n", खण्डम्.session_id);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
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
                          खण्डम्.transport ? खण्डम्.transport : "",
                          खण्डम्.accept ? खण्डम्.accept : "",
                          खण्डम्.accept_encoding ? खण्डम्.accept_encoding : "",
                          खण्डम्.range ? खण्डम्.range : "",
                          खण्डम्.referrer ? खण्डम्.referrer : "");

  str = CURL_EASY_STR(दत्त, STRING_USERAGENT);
  यदि(!परिणाम && str && *str &&
     !Curl_checkheaders(दत्त, STRCONST("User-Agent")))
    परिणाम = curlx_dyn_addf(&req_बफर,
                            "User-Agent: %s\r\n", str);

  यदि(!परिणाम)
    परिणाम = curlx_dyn_addf(&req_बफर,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            खण्डम्.hd_proxy_auth ? खण्डम्.hd_proxy_auth : "",
                            खण्डम्.hd_auth ? खण्डम्.hd_auth : "");
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि((rtspreq == RTSPREQ_सज्जता) || (rtspreq == RTSPREQ_DESCRIBE)) {
    परिणाम = Curl_सङ्कलय_timecondition(दत्त, &req_बफर);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_सङ्कलय_custom_शीर्षकाणि(दत्त, FALSE, HTTPVERSION, &req_बफर);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = rtsp_सज्जता_body(दत्त, rtspreq, &req_बफर);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Finish the request buffer */
  परिणाम = curlx_dyn_addn(&req_बफर, STRCONST("\r\n"));
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  Curl_xfer_सज्जता_sendrecv(दत्त, FIRSTSOCKET, -1);

  /* issue the request */
  परिणाम = Curl_req_send(दत्त, &req_बफर, HTTPVERSION);
  यदि(परिणाम) {
    failf(दत्त, "Failed sending RTSP request");
    गच्छ बहिर्गमनम्;
  }

  /* Increment the CSeq on success */
  दत्त->state.rtsp_अग्र_client_CSeq++;

  यदि(दत्त->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_समुच्चयः_Upload_गणक(दत्त, दत्त->req.writebytecount);
    परिणाम = Curl_pgrsUpdate(दत्त);
  }
बहिर्गमनम्:
  यदि(खण्डम्.transport_alloc)
    curlx_free(खण्डम्.transport);
  curlx_free(खण्डम्.accept_encoding);
  curlx_free(खण्डम्.referrer);
  curlx_dyn_free(&req_बफर);
  प्रत्यागम परिणाम;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
स्थिर CURLcode rtp_write_body_junk(संरचना Curl_easy *दत्त,
                                    संरचना rtsp_conn *rtspc,
                                    ध्रुव वर्ण *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool अन्तर्_body;

  अन्तर्_body = (दत्त->req.headerline && !rtspc->अन्तर्_शीर्षकम्) &&
            (दत्त->req.आकार >= 0) &&
            (दत्त->req.bytecount < दत्त->req.आकार);
  body_remain = अन्तर्_body ? (दत्त->req.आकार - दत्त->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  यदि(body_remain) {
    यदि((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    प्रत्यागम Curl_client_write(दत्त, CLIENTWRITE_BODY, buf, blen);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode rtp_client_write(संरचना Curl_easy *दत्त, ध्रुव वर्ण *ptr,
                                 size_t len)
{
  संरचना Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  शून्य *user_ptr;

  यदि(len == 0) {
    failf(दत्त, "Cannot write a 0 size RTP packet.");
    प्रत्यागम CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  यदि(दत्त->समुच्चयः.fwrite_rtp) {
    writeit = दत्त->समुच्चयः.fwrite_rtp;
    user_ptr = दत्त->समुच्चयः.rtp_बहिर्गमनम्;
  }
  अन्यथा {
    writeit = दत्त->समुच्चयः.fwrite_func;
    user_ptr = दत्त->समुच्चयः.बहिर्गमनम्;
  }

  CURL_CBAPI_START(&guard, दत्त, easy_fwrite_rtp);
  wrote = writeit((वर्ण *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_अन्तः(&guard);

  यदि(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(दत्त, "Cannot pause RTP");
    प्रत्यागम CURLE_WRITE_ERROR;
  }

  यदि(wrote != len) {
    failf(दत्त, "Failed writing RTP data");
    प्रत्यागम CURLE_WRITE_ERROR;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode rtsp_filter_rtp(संरचना Curl_easy *दत्त,
                                संरचना rtsp_conn *rtspc,
                                ध्रुव वर्ण *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  यावत्(blen) {
    bool अन्तर्_body = (दत्त->req.headerline && !rtspc->अन्तर्_शीर्षकम्) &&
                   (दत्त->req.आकार >= 0) &&
                   (दत्त->req.bytecount < दत्त->req.आकार);
    विकल्प(rtspc->state) {

    पक्ष RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      यावत्(blen && buf[0] != '$') {
        यदि(!अन्तर्_body && buf[0] == 'R' &&
           दत्त->समुच्चयः.rtspreq != RTSPREQ_RECEIVE) {
          यदि(!वाक्ये_तुलय(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            यदि(*pconsumed) {
              DEBUGF(infof(दत्त, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->अन्तर्_शीर्षकम् = TRUE;
            गच्छ बहिर्गमनम्;
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
          परिणाम = rtp_write_body_junk(दत्त, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          यदि(परिणाम)
            गच्छ बहिर्गमनम्;
        }
        यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          गच्छ बहिर्गमनम्;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      भङ्ग;
    }

    पक्ष RTP_PARSE_CHANNEL: {
      पूर्णाङ्क idx = ((अचिह्नित वर्ण)buf[0]) / 8;
      पूर्णाङ्क off = ((अचिह्नित वर्ण)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      यदि(!(दत्त->state.rtp_channel_आवरणम्[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(दत्त, "RTSP: invalid RTP channel %d, skipping", idx));
        यदि(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          परिणाम = rtp_write_body_junk(दत्त, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          यदि(परिणाम)
            गच्छ बहिर्गमनम्;
        }
        अन्यथा {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        भङ्ग;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (अचिह्नित वर्ण)buf[0];
      यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      भङ्ग;
    }

    पक्ष RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      ध्रुव वर्ण *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      यदि(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      यदि(rtp_len == 2)
        भङ्ग;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_दीर्घता(rtp_buf) + 4;
      यदि(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(दत्त, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        परिणाम = rtp_client_write(दत्त, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
        भङ्ग;
      }
      rtspc->state = RTP_PARSE_दत्त;
      भङ्ग;
    }

    पक्ष RTP_PARSE_दत्त: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      यदि(needed <= blen) {
        यदि(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          गच्छ बहिर्गमनम्;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(दत्त, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        परिणाम = rtp_client_write(दत्त, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
      अन्यथा {
        यदि(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          गच्छ बहिर्गमनम्;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      भङ्ग;
    }

    स्वभाव:
      DEBUGASSERT(0);
      प्रत्यागम CURLE_RECV_ERROR;
    }
  }
बहिर्गमनम्:
  यदि(!परिणाम && skip_len)
    परिणाम = rtp_write_body_junk(दत्त, rtspc, buf - skip_len, skip_len);
  प्रत्यागम परिणाम;
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
स्थिर CURLcode rtsp_rtp_write_resp(संरचना Curl_easy *दत्त,
                                    ध्रुव वर्ण *buf,
                                    size_t blen,
                                    bool is_eos)
{
  संरचना rtsp_conn *rtspc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_RTSP_CONN);
  CURLcode परिणाम = CURLE_OK;
  size_t consumed = 0;

  यदि(!rtspc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!दत्त->req.शीर्षकम्)
    rtspc->अन्तर्_शीर्षकम् = FALSE;
  यदि(!blen) {
    गच्छ बहिर्गमनम्;
  }

  DEBUGF(infof(दत्त, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->अन्तर्_शीर्षकम्, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  यदि(!rtspc->अन्तर्_शीर्षकम्) {
    परिणाम = rtsp_filter_rtp(दत्त, rtspc, buf, blen, &consumed);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    यदि(blen && !दत्त->req.शीर्षकम्)
      DEBUGF(infof(दत्त, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  यदि(दत्त->req.शीर्षकम् && blen) {
    rtspc->अन्तर्_शीर्षकम् = TRUE;
    परिणाम = Curl_http_write_resp_hds(दत्त, buf, blen, &consumed);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    buf += consumed;
    blen -= consumed;

    यदि(!दत्त->req.शीर्षकम्)
      rtspc->अन्तर्_शीर्षकम् = FALSE;

    यदि(!rtspc->अन्तर्_शीर्षकम्) {
      /* If header parsing is done, extract interleaved RTP messages */
      यदि(दत्त->req.आकार <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        दत्त->req.आकार = 0;
        दत्त->req.download_समाप्तम् = TRUE;
      }
      परिणाम = rtsp_filter_rtp(दत्त, rtspc, buf, blen, &consumed);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      buf += consumed;
      blen -= consumed;
    }
  }

  यदि(rtspc->state != RTP_PARSE_SKIP)
    दत्त->req.समाप्तम् = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(दत्त, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->अन्तर्_शीर्षकम्, दत्त->req.समाप्तम्, (पूर्णाङ्क)rtspc->state,
               दत्त->req.आकार));
  यदि(!परिणाम && (is_eos || blen)) {
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

बहिर्गमनम्:
  यदि((दत्त->समुच्चयः.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    दत्त->req.download_समाप्तम् = TRUE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode rtsp_rtp_write_resp_hd(संरचना Curl_easy *दत्त,
                                       ध्रुव वर्ण *buf,
                                       size_t blen,
                                       bool is_eos)
{
  प्रत्यागम rtsp_rtp_write_resp(दत्त, buf, blen, is_eos);
}

स्थिर CURLcode rtsp_parse_transport(संरचना Curl_easy *दत्त,
                                     ध्रुव वर्ण *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  ध्रुव वर्ण *start, *अन्तः;
  start = transport;
  यावत्(start && *start) {
    curlx_str_passblanks(&start);
    अन्तः = strchr(start, ';');
    यदि(checkprefix("interleaved=", start)) {
      curl_off_t chan1, chan2, chan;
      ध्रुव वर्ण *p = start + 12;
      यदि(!curlx_str_सङ्ख्या(&p, &chan1, 255)) {
        अचिह्नित वर्ण *rtp_channel_आवरणम् = दत्त->state.rtp_channel_आवरणम्;
        chan2 = chan1;
        यदि(!curlx_str_single(&p, '-') && curlx_str_सङ्ख्या(&p, &chan2, 255)) {
          infof(दत्त, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        प्रति(chan = chan1; chan <= chan2; chan++) {
          पूर्णाङ्क idx = (पूर्णाङ्क)chan / 8;
          पूर्णाङ्क off = (पूर्णाङ्क)chan % 8;
          rtp_channel_आवरणम्[idx] |= (अचिह्नित वर्ण)(1 << off);
        }
      }
      अन्यथा {
        infof(दत्त, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      भङ्ग;
    }
    /* skip to next parameter */
    start = (!अन्तः) ? अन्तः : (अन्तः + 1);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(संरचना Curl_easy *दत्त, ध्रुव वर्ण *शीर्षकम्)
{
  यदि(checkprefix("CSeq:", शीर्षकम्)) {
    curl_off_t CSeq = 0;
    संरचना RTSP *rtsp = Curl_meta_get(दत्त, CURL_META_RTSP_EASY);
    ध्रुव वर्ण *p = &शीर्षकम्[5];
    यदि(!rtsp)
      प्रत्यागम CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    यदि(curlx_str_सङ्ख्या(&p, &CSeq, UINT_MAX)) {
      failf(दत्त, "Unable to read the CSeq header: [%s]", शीर्षकम्);
      प्रत्यागम CURLE_RTSP_CSEQ_ERROR;
    }
    दत्त->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  अन्यथा यदि(checkprefix("Session:", शीर्षकम्)) {
    ध्रुव वर्ण *start, *अन्तः, *str;
    size_t idlen;

    /* Find the first non-space letter */
    start = शीर्षकम् + 8;
    curlx_str_passblanks(&start);

    यदि(!*start) {
      failf(दत्त, "Got a blank Session ID");
      प्रत्यागम CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    अन्तः = start;
    यावत्((*अन्तः > ' ') && (*अन्तः != ';'))
      अन्तः++;
    idlen = अन्तः - start;

    str = CURL_EASY_STR(दत्त, STRING_RTSP_SESSION_ID);
    यदि(str) {

      /* If the Session ID is set, then compare */
      यदि(वाक्यदैर्घ्यम्(str) != idlen ||
         वाक्ये_तुलय(start, str, idlen)) {
        failf(दत्त, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              start, str);
        प्रत्यागम CURLE_RTSP_SESSION_ERROR;
      }
    }
    अन्यथा {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      शून्य *mem = curlx_memdup0(start, idlen);
      यदि(!mem ||
         CURL_EASY_STR_SETN(दत्त, STRING_RTSP_SESSION_ID, mem))
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      Curl_peer_link(&दत्त->state.rtsp_session_origin, दत्त->state.origin);
    }
  }
  अन्यथा यदि(checkprefix("Transport:", शीर्षकम्)) {
    CURLcode परिणाम;
    परिणाम = rtsp_parse_transport(दत्त, शीर्षकम् + 10);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * RTSP handler interface.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_rtsp = {
  rtsp_सज्जता_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_समाप्तम्,                            /* done */
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

#यदि_अन्तः /* CURL_DISABLE_RTSP */
