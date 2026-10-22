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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "rtsp.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP

#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "peer.h"

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_RTSP_EASY   "meta:proto:rtsp:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_RTSP_CONN   "meta:proto:rtsp:conn"

రకనిర్వచనం గణన {
  RTP_PARSE_SKIP,
  RTP_PARSE_CHANNEL,
  RTP_PARSE_LEN,
  RTP_PARSE_దత్తాంశం
} rtp_parse_st;

/* RTSP Connection data
 * Currently, only used for tracking incomplete RTP data reads */
నిర్మాణం rtsp_conn {
  నిర్మాణం dynbuf buf;
  పూర్ణాంకం rtp_channel;
  size_t rtp_len;
  rtp_parse_st state;
  బిట్టు(లోపల_శీర్షిక);
};

/* RTSP transfer data */
నిర్మాణం RTSP {
  uint32_t CSeq_పంపిన; /* CSeq of this request */
  uint32_t CSeq_recv; /* CSeq received */
};

#నిర్వచించు RTP_PKT_నిడివి(p) ((((గుర్తులేని పూర్ణాంకం)((గుర్తులేని అక్షరం)((p)[2]))) << 8) | \
                            ((గుర్తులేని పూర్ణాంకం)((గుర్తులేని అక్షరం)((p)[3]))))

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
స్థిర CURLcode rtsp_do_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం easy_pollset *ps)
{
  /* write mode */
  తిరిగి Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
}

#నిర్వచించు MAX_RTP_BUFFERSIZE 1000000 /* arbitrary */

స్థిర శూన్య rtsp_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం RTSP *rtsp = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_free(rtsp);
}

స్థిర శూన్య rtsp_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం rtsp_conn *rtspc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_dyn_free(&rtspc->buf);
  curlx_free(rtspc);
}

స్థిర CURLcode rtsp_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం rtsp_conn *rtspc;
  నిర్మాణం RTSP *rtsp;

  rtspc = curlx_calloc(1, పరిమాణం(*rtspc));
  ఒకవేళ(!rtspc)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  curlx_dyn_init(&rtspc->buf, MAX_RTP_BUFFERSIZE);
  ఒకవేళ(Curl_conn_meta_సమితి(conn, CURL_META_RTSP_CONN, rtspc, rtsp_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  rtsp = curlx_calloc(1, పరిమాణం(నిర్మాణం RTSP));
  ఒకవేళ(!rtsp ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_RTSP_EASY, rtsp, rtsp_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

/*
 * Function to check on various aspects of a connection.
 */
స్థిర bool rtsp_conn_is_dead(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  bool ఆదానం_pending;
  /* Contrary to default handling, this protocol allows pending
   * input on an unused connection. */
  తిరిగి !Curl_conn_is_alive(దత్తాంశం, conn, &ఆదానం_pending);
}

స్థిర CURLcode rtsp_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం rtsp_conn *rtspc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_RTSP_CONN);

  ఒకవేళ(!rtspc)
    తిరిగి CURLE_FAILED_INIT;

  /* Initialize the CSeq if not already done */
  ఒకవేళ(దత్తాంశం->state.rtsp_తదుపరి_client_CSeq == 0)
    దత్తాంశం->state.rtsp_తదుపరి_client_CSeq = 1;
  ఒకవేళ(దత్తాంశం->state.rtsp_తదుపరి_server_CSeq == 0)
    దత్తాంశం->state.rtsp_తదుపరి_server_CSeq = 1;

  rtspc->rtp_channel = -1;
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode rtsp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                          CURLcode స్థితి_2, bool premature)
{
  నిర్మాణం rtsp_conn *rtspc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_RTSP_CONN);
  నిర్మాణం RTSP *rtsp = Curl_meta_get(దత్తాంశం, CURL_META_RTSP_EASY);
  CURLcode ఫలితం;

  ఒకవేళ(!rtspc || !rtsp)
    తిరిగి CURLE_FAILED_INIT;

  /* Bypass HTTP empty-reply checks on receive */
  ఒకవేళ(దత్తాంశం->సమితి.rtspreq == RTSPREQ_RECEIVE)
    premature = TRUE;

  ఫలితం = Curl_http_పూర్తి(దత్తాంశం, స్థితి_2, premature);

  ఒకవేళ(!స్థితి_2 && !ఫలితం) {
    /* Check the sequence numbers */
    uint32_t CSeq_పంపిన = rtsp->CSeq_పంపిన;
    uint32_t CSeq_recv = rtsp->CSeq_recv;
    ఒకవేళ((దత్తాంశం->సమితి.rtspreq != RTSPREQ_RECEIVE) && (CSeq_పంపిన != CSeq_recv)) {
      failf(దత్తాంశం,
            "The CSeq of this request %u did not match the response %u",
            CSeq_పంపిన, CSeq_recv);
      తిరిగి CURLE_RTSP_CSEQ_ERROR;
    }
    ఒకవేళ(దత్తాంశం->సమితి.rtspreq == RTSPREQ_RECEIVE && (rtspc->rtp_channel == -1)) {
      infof(దత్తాంశం, "Got an RTP Receive with a CSeq of %u", CSeq_recv);
    }
    ఒకవేళ(దత్తాంశం->సమితి.rtspreq == RTSPREQ_RECEIVE &&
       దత్తాంశం->req.eos_written) {
      failf(దత్తాంశం, "Server prematurely closed the RTSP connection.");
      తిరిగి CURLE_RECV_ERROR;
    }
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode rtsp_అమరిక_body(నిర్మాణం Curl_easy *దత్తాంశం,
                                గుర్తులేని అక్షరం rtspreq,
                                నిర్మాణం dynbuf *reqp)
{
  CURLcode ఫలితం;
  ఒకవేళ(rtspreq == RTSPREQ_ANNOUNCE ||
     rtspreq == RTSPREQ_సమితి_PARAMETER ||
     rtspreq == RTSPREQ_GET_PARAMETER) {
    curl_off_t req_clen; /* request content length */

    ఒకవేళ(దత్తాంశం->state.upload) {
      req_clen = దత్తాంశం->state.infilesize;
      దత్తాంశం->state.httpreq = HTTPREQ_PUT;
      ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, req_clen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే {
      ఒకవేళ(దత్తాంశం->సమితి.postfields) {
        size_t plen = (దత్తాంశం->సమితి.postfieldsize >= 0) ?
          (size_t)దత్తాంశం->సమితి.postfieldsize : అక్షరశ్రేణి_పొడవు(దత్తాంశం->సమితి.postfields);
        req_clen = (curl_off_t)plen;
        ఫలితం = Curl_creader_సమితి_buf(దత్తాంశం, దత్తాంశం->సమితి.postfields, plen);
      }
      లేకపోతే ఒకవేళ(దత్తాంశం->state.infilesize >= 0) {
        req_clen = దత్తాంశం->state.infilesize;
        ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, req_clen);
      }
      లేకపోతే {
        req_clen = 0;
        ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
      }
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    ఒకవేళ(req_clen > 0) {
      ఒకవేళ(దత్తాంశం->సమితి.crlf) {
        failf(దత్తాంశం, "Cannot do CRLF conversions on upload");
        తిరిగి CURLE_UPLOAD_FAILED;
      }
      /* As stated in the http comments, it is probably not wise to
       * actually set a custom Content-Length in the headers */
      ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Content-Length"))) {
        ఫలితం = curlx_dyn_addf(reqp, "Content-Length: %" FMT_OFF_T "\r\n",
                                req_clen);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }

      ఒకవేళ(rtspreq == RTSPREQ_సమితి_PARAMETER ||
         rtspreq == RTSPREQ_GET_PARAMETER) {
        ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Content-Type"))) {
          ఫలితం = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "text/parameters\r\n"));
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
        }
      }

      ఒకవేళ(rtspreq == RTSPREQ_ANNOUNCE) {
        ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Content-Type"))) {
          ఫలితం = curlx_dyn_addn(reqp, STRCONST("Content-Type: "
                                                 "application/sdp\r\n"));
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
        }
      }
    }
    లేకపోతే ఒకవేళ(rtspreq == RTSPREQ_GET_PARAMETER) {
      /* Check for an empty GET_PARAMETER (heartbeat) request */
      దత్తాంశం->state.httpreq = HTTPREQ_తల;
      దత్తాంశం->req.no_body = TRUE;
    }
  }
  లేకపోతే
    ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
  తిరిగి ఫలితం;
}

నిర్మాణం rtspselect {
  స్థిరరాశి అక్షరం *పద్ధతి;
  bool no_body;
};

స్థిర CURLcode pick_పద్ధతి(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి గుర్తులేని అక్షరం rtspreq,
                            స్థిరరాశి అక్షరం **p)
{
  స్థిర స్థిరరాశి నిర్మాణం rtspselect req[] = {
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
  DEBUGASSERT((rtspreq > RTSPREQ_NONE) && (rtspreq < RTSPREQ_చివరి));
  ఒకవేళ((rtspreq <= RTSPREQ_NONE) || (rtspreq >= RTSPREQ_చివరి))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  *p = req[rtspreq - 1].పద్ధతి;
  దత్తాంశం->req.no_body = req[rtspreq - 1].no_body;
  తిరిగి CURLE_OK;
}

/* Allocate and store a header string. */
స్థిర CURLcode rtsp_శీర్షిక_alloc(స్థిరరాశి అక్షరం *శీర్షిక_name,
                                  స్థిరరాశి అక్షరం *విలువ,
                                  అక్షరం **లక్ష్యం)
{
  ఒకవేళ(!విలువ)
    తిరిగి CURLE_OK;
  curlx_free(*లక్ష్యం);
  *లక్ష్యం = curl_maprintf("%s: %s\r\n", శీర్షిక_name, విలువ);
  ఒకవేళ(!*లక్ష్యం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

నిర్మాణం rtsp_ఖండాలు {
  స్థిరరాశి అక్షరం *request;
  స్థిరరాశి అక్షరం *session_id;
  స్థిరరాశి అక్షరం *accept;
  స్థిరరాశి అక్షరం *range;
  స్థిరరాశి అక్షరం *ప్రవాహం_uri;
  స్థిరరాశి అక్షరం *hd_proxy_auth;
  స్థిరరాశి అక్షరం *hd_auth;
  అక్షరం *referrer;
  అక్షరం *accept_encoding;
  అక్షరం *transport;
  బిట్టు(transport_alloc); /* if 'transport' is allocated */
};

స్థిర CURLcode rtsp_అమరిక_request(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం rtsp_ఖండాలు *b,
                                   స్థిరరాశి గుర్తులేని అక్షరం rtspreq)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  b->session_id = CURL_EASY_STR(దత్తాంశం, STRING_RTSP_SESSION_ID);

  /* Stream URI. Default to server '*' if not specified */
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_RTSP_ప్రవాహం_URI))
    b->ప్రవాహం_uri = CURL_EASY_STR(దత్తాంశం, STRING_RTSP_ప్రవాహం_URI);
  లేకపోతే
    b->ప్రవాహం_uri = "*";

  /* Transport Header for SETUP requests */
  b->transport = Curl_checkheaders(దత్తాంశం, STRCONST("Transport"));
  ఒకవేళ(rtspreq == RTSPREQ_అమరిక && !b->transport) {
    /* New Transport: setting? */
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_RTSP_TRANSPORT)) {
      ఫలితం = rtsp_శీర్షిక_alloc(
        "Transport", CURL_EASY_STR(దత్తాంశం, STRING_RTSP_TRANSPORT),
        &b->transport);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      b->transport_alloc = TRUE;
    }
    లేకపోతే {
      failf(దత్తాంశం,
            "Refusing to issue an RTSP SETUP without a Transport: header.");
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
  }

  /* Accept Headers for DESCRIBE requests */
  ఒకవేళ(rtspreq == RTSPREQ_DESCRIBE) {
    /* Accept Header */
    b->accept = Curl_checkheaders(దత్తాంశం, STRCONST("Accept")) ?
      ఖాళీ : "Accept: application/sdp\r\n";

    /* Accept-Encoding header */
    ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Accept-Encoding")) &&
       CURL_EASY_STR(దత్తాంశం, STRING_ENCODING)) {
      ఫలితం = rtsp_శీర్షిక_alloc("Accept-Encoding",
                                 CURL_EASY_STR(దత్తాంశం, STRING_ENCODING),
                                 &b->accept_encoding);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  /* setup the authentication headers */
  ఫలితం = Curl_http_output_auth(దత్తాంశం, conn, b->request, HTTPREQ_GET,
                                 b->ప్రవాహం_uri, ఖాళీ, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  b->hd_proxy_auth = దత్తాంశం->req.hd_proxy_auth;
#అయితే_ముగింపు
  b->hd_auth = దత్తాంశం->req.hd_auth;

  /* Referrer */
  ఒకవేళ(Curl_bufref_ptr(&దత్తాంశం->state.referer) &&
     !Curl_checkheaders(దత్తాంశం, STRCONST("Referer"))) {
    b->referrer =
      curl_maprintf("Referer: %s\r\n", Curl_bufref_ptr(&దత్తాంశం->state.referer));
    ఒకవేళ(!b->referrer)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /*
   * Range Header
   * Only applies to PLAY, PAUSE, RECORD
   *
   * Go ahead and use the Range stuff supplied for HTTP
   */
  ఒకవేళ(!ఫలితం &&
     దత్తాంశం->state.use_range &&
     ((rtspreq == RTSPREQ_PLAY) ||
      (rtspreq == RTSPREQ_PAUSE) ||
      (rtspreq == RTSPREQ_రికార్డు)) &&
     /* Check to see if there is a range set in the custom headers */
     !Curl_checkheaders(దత్తాంశం, STRCONST("Range")) && దత్తాంశం->state.range) {
    ఫలితం = rtsp_శీర్షిక_alloc("Range",
                               దత్తాంశం->state.range,
                               &దత్తాంశం->state.rangeline);
    ఒకవేళ(!ఫలితం)
      b->range = దత్తాంశం->state.rangeline;
  }
  తిరిగి ఫలితం;
}

#నిర్వచించు HTTPVERSION 11 /* RTSP is close to HTTP/1.1, sort of... */

స్థిర CURLcode rtsp_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని అక్షరం rtspreq = దత్తాంశం->సమితి.rtspreq;
  స్థిరరాశి అక్షరం *str;
  నిర్మాణం RTSP *rtsp = Curl_meta_get(దత్తాంశం, CURL_META_RTSP_EASY);
  నిర్మాణం dynbuf req_బఫరు;
  నిర్మాణం rtsp_ఖండాలు ఖండం;
  memset(&ఖండం, 0, పరిమాణం(ఖండం));

  *పూర్తి = TRUE;
  ఒకవేళ(!rtsp)
    తిరిగి CURLE_FAILED_INIT;

  /* Initialize a dynamic send buffer */
  curlx_dyn_init(&req_బఫరు, DYN_RTSP_REQ_శీర్షిక);

  rtsp->CSeq_పంపిన = దత్తాంశం->state.rtsp_తదుపరి_client_CSeq;
  rtsp->CSeq_recv = 0;

  /* Setup the 'p_request' pointer to the proper method. */
  ఫలితం = pick_పద్ధతి(దత్తాంశం, rtspreq, &ఖండం.request);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(rtspreq == RTSPREQ_RECEIVE) {
    Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = rtsp_అమరిక_request(దత్తాంశం, &ఖండం,  rtspreq);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(ఖండం.session_id && దత్తాంశం->state.rtsp_session_origin &&
     !దత్తాంశం->సమితి.allow_auth_గమ్యం_other_hosts &&
     !Curl_peer_equal(దత్తాంశం->state.origin, దత్తాంశం->state.rtsp_session_origin))
    ఖండం.session_id = ఖాళీ;

  /*
   * Sanity check the custom headers
   */
  ఒకవేళ(Curl_checkheaders(దత్తాంశం, STRCONST("CSeq"))) {
    failf(దత్తాంశం, "CSeq cannot be set as a custom header.");
    ఫలితం = CURLE_RTSP_CSEQ_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(Curl_checkheaders(దత్తాంశం, STRCONST("Session"))) {
    failf(దత్తాంశం, "Session ID cannot be set as a custom header.");
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం =
    curlx_dyn_addf(&req_బఫరు,
                   "%s %s RTSP/1.0\r\n" /* Request Stream-URI RTSP/1.0 */
                   "CSeq: %u\r\n", /* CSeq */
                   ఖండం.request, ఖండం.ప్రవాహం_uri, rtsp->CSeq_పంపిన);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /*
   * Rather than do a normal alloc line, keep the session_id unformatted
   * to make comparison easier
   */
  ఒకవేళ(ఖండం.session_id) {
    ఫలితం = curlx_dyn_addf(&req_బఫరు, "Session: %s\r\n", ఖండం.session_id);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /*
   * Shared HTTP-like options
   */
  ఫలితం = curlx_dyn_addf(&req_బఫరు,
                          "%s" /* transport */
                          "%s" /* accept */
                          "%s" /* accept-encoding */
                          "%s" /* range */
                          "%s" /* referrer */
                          ,
                          ఖండం.transport ? ఖండం.transport : "",
                          ఖండం.accept ? ఖండం.accept : "",
                          ఖండం.accept_encoding ? ఖండం.accept_encoding : "",
                          ఖండం.range ? ఖండం.range : "",
                          ఖండం.referrer ? ఖండం.referrer : "");

  str = CURL_EASY_STR(దత్తాంశం, STRING_USERAGENT);
  ఒకవేళ(!ఫలితం && str && *str &&
     !Curl_checkheaders(దత్తాంశం, STRCONST("User-Agent")))
    ఫలితం = curlx_dyn_addf(&req_బఫరు,
                            "User-Agent: %s\r\n", str);

  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addf(&req_బఫరు,
                            "%s" /* hd_proxy_auth */
                            "%s", /* hd_auth */
                            ఖండం.hd_proxy_auth ? ఖండం.hd_proxy_auth : "",
                            ఖండం.hd_auth ? ఖండం.hd_auth : "");
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ((rtspreq == RTSPREQ_అమరిక) || (rtspreq == RTSPREQ_DESCRIBE)) {
    ఫలితం = Curl_కూడు_timecondition(దత్తాంశం, &req_బఫరు);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_కూడు_custom_శీర్షికలు(దత్తాంశం, FALSE, HTTPVERSION, &req_బఫరు);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = rtsp_అమరిక_body(దత్తాంశం, rtspreq, &req_బఫరు);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Finish the request buffer */
  ఫలితం = curlx_dyn_addn(&req_బఫరు, STRCONST("\r\n"));
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  Curl_xfer_అమరిక_sendrecv(దత్తాంశం, FIRSTSOCKET, -1);

  /* issue the request */
  ఫలితం = Curl_req_send(దత్తాంశం, &req_బఫరు, HTTPVERSION);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "Failed sending RTSP request");
    వెళ్ళు బహిర్గతం;
  }

  /* Increment the CSeq on success */
  దత్తాంశం->state.rtsp_తదుపరి_client_CSeq++;

  ఒకవేళ(దత్తాంశం->req.writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is
       noted properly */
    Curl_pgrs_సమితి_Upload_Counter(దత్తాంశం, దత్తాంశం->req.writebytecount);
    ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
  }
బహిర్గతం:
  ఒకవేళ(ఖండం.transport_alloc)
    curlx_free(ఖండం.transport);
  curlx_free(ఖండం.accept_encoding);
  curlx_free(ఖండం.referrer);
  curlx_dyn_free(&req_బఫరు);
  తిరిగి ఫలితం;
}

/**
 * write any BODY bytes missing to the client, ignore the rest.
 */
స్థిర CURLcode rtp_write_body_junk(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం rtsp_conn *rtspc,
                                    స్థిరరాశి అక్షరం *buf,
                                    size_t blen)
{
  curl_off_t body_remain;
  bool లోపల_body;

  లోపల_body = (దత్తాంశం->req.headerline && !rtspc->లోపల_శీర్షిక) &&
            (దత్తాంశం->req.కొలత >= 0) &&
            (దత్తాంశం->req.bytecount < దత్తాంశం->req.కొలత);
  body_remain = లోపల_body ? (దత్తాంశం->req.కొలత - దత్తాంశం->req.bytecount) : 0;
  DEBUGASSERT(body_remain >= 0);
  ఒకవేళ(body_remain) {
    ఒకవేళ((curl_off_t)blen > body_remain)
      blen = (size_t)body_remain;
    తిరిగి Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, buf, blen);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode rtp_client_write(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ptr,
                                 size_t len)
{
  నిర్మాణం Curl_mapi_guard guard;
  size_t wrote;
  curl_write_callback writeit;
  శూన్య *user_ptr;

  ఒకవేళ(len == 0) {
    failf(దత్తాంశం, "Cannot write a 0 size RTP packet.");
    తిరిగి CURLE_WRITE_ERROR;
  }

  /* If the user has configured CURLOPT_INTERLEAVEFUNCTION then use that
     function and any configured CURLOPT_INTERLEAVEDATA to write out the RTP
     data. Otherwise, use the CURLOPT_WRITEFUNCTION with the CURLOPT_WRITEDATA
     pointer to write out the RTP data. */
  ఒకవేళ(దత్తాంశం->సమితి.fwrite_rtp) {
    writeit = దత్తాంశం->సమితి.fwrite_rtp;
    user_ptr = దత్తాంశం->సమితి.rtp_బహిర్గతం;
  }
  లేకపోతే {
    writeit = దత్తాంశం->సమితి.fwrite_func;
    user_ptr = దత్తాంశం->సమితి.బహిర్గతం;
  }

  CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fwrite_rtp);
  wrote = writeit((అక్షరం *)CURL_UNCONST(ptr), 1, len, user_ptr);
  CURL_CBAPI_చివర(&guard);

  ఒకవేళ(wrote == CURL_WRITEFUNC_PAUSE) {
    failf(దత్తాంశం, "Cannot pause RTP");
    తిరిగి CURLE_WRITE_ERROR;
  }

  ఒకవేళ(wrote != len) {
    failf(దత్తాంశం, "Failed writing RTP data");
    తిరిగి CURLE_WRITE_ERROR;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode rtsp_filter_rtp(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం rtsp_conn *rtspc,
                                స్థిరరాశి అక్షరం *buf,
                                size_t blen,
                                size_t *pconsumed)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t skip_len = 0;

  *pconsumed = 0;
  అయితే(blen) {
    bool లోపల_body = (దత్తాంశం->req.headerline && !rtspc->లోపల_శీర్షిక) &&
                   (దత్తాంశం->req.కొలత >= 0) &&
                   (దత్తాంశం->req.bytecount < దత్తాంశం->req.కొలత);
    ఎంపిక(rtspc->state) {

    స్థితి RTP_PARSE_SKIP: {
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 0);
      అయితే(blen && buf[0] != '$') {
        ఒకవేళ(!లోపల_body && buf[0] == 'R' &&
           దత్తాంశం->సమితి.rtspreq != RTSPREQ_RECEIVE) {
          ఒకవేళ(!అక్షరశ్రేణి_పోలిక(buf, "RTSP/", (blen < 5) ? blen : 5)) {
            /* This could be the next response, no consume and return */
            ఒకవేళ(*pconsumed) {
              DEBUGF(infof(దత్తాంశం, "RTP rtsp_filter_rtp[SKIP] RTSP/ prefix, "
                           "skipping %zu bytes of junk", *pconsumed));
            }
            rtspc->state = RTP_PARSE_SKIP;
            rtspc->లోపల_శీర్షిక = TRUE;
            వెళ్ళు బహిర్గతం;
          }
        }
        /* junk/BODY, consume without buffering */
        *pconsumed += 1;
        ++buf;
        --blen;
        ++skip_len;
      }
      ఒకవేళ(blen && buf[0] == '$') {
        /* possible start of an RTP message, buffer */
        ఒకవేళ(skip_len) {
          /* end of junk/BODY bytes, flush */
          ఫలితం = rtp_write_body_junk(దత్తాంశం, rtspc, buf - skip_len, skip_len);
          skip_len = 0;
          ఒకవేళ(ఫలితం)
            వెళ్ళు బహిర్గతం;
        }
        ఒకవేళ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          వెళ్ళు బహిర్గతం;
        }
        *pconsumed += 1;
        ++buf;
        --blen;
        rtspc->state = RTP_PARSE_CHANNEL;
      }
      ఆపు;
    }

    స్థితి RTP_PARSE_CHANNEL: {
      పూర్ణాంకం idx = ((గుర్తులేని అక్షరం)buf[0]) / 8;
      పూర్ణాంకం off = ((గుర్తులేని అక్షరం)buf[0]) % 8;
      DEBUGASSERT(curlx_dyn_len(&rtspc->buf) == 1);
      ఒకవేళ(!(దత్తాంశం->state.rtp_channel_ముసుగు[idx] & (1 << off))) {
        /* invalid channel number, junk or BODY data */
        rtspc->state = RTP_PARSE_SKIP;
        DEBUGASSERT(skip_len == 0);
        /* we do not consume this byte, it is BODY data */
        DEBUGF(infof(దత్తాంశం, "RTSP: invalid RTP channel %d, skipping", idx));
        ఒకవేళ(*pconsumed == 0) {
          /* We did not consume the initial '$' in our buffer, but had
           * it from an earlier call. We cannot un-consume it and have
           * to write it directly as BODY data */
          ఫలితం = rtp_write_body_junk(దత్తాంశం, rtspc,
                                       curlx_dyn_ptr(&rtspc->buf), 1);
          ఒకవేళ(ఫలితం)
            వెళ్ళు బహిర్గతం;
        }
        లేకపోతే {
          /* count the '$' as skip and continue */
          skip_len = 1;
        }
        curlx_dyn_free(&rtspc->buf);
        ఆపు;
      }
      /* a valid channel, so we expect this to be a real RTP message */
      rtspc->rtp_channel = (గుర్తులేని అక్షరం)buf[0];
      ఒకవేళ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      rtspc->state = RTP_PARSE_LEN;
      ఆపు;
    }

    స్థితి RTP_PARSE_LEN: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      స్థిరరాశి అక్షరం *rtp_buf;
      DEBUGASSERT(rtp_len >= 2 && rtp_len < 4);
      ఒకవేళ(curlx_dyn_addn(&rtspc->buf, buf, 1)) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      *pconsumed += 1;
      ++buf;
      --blen;
      ఒకవేళ(rtp_len == 2)
        ఆపు;
      rtp_buf = curlx_dyn_ptr(&rtspc->buf);
      rtspc->rtp_len = RTP_PKT_నిడివి(rtp_buf) + 4;
      ఒకవేళ(rtspc->rtp_len == 4) {
        /* zero-length payload, the 4-byte header is the complete RTP
           message. Dispatch immediately without entering RTP_PARSE_DATA. */
        DEBUGF(infof(దత్తాంశం, "RTP write channel %d rtp_len %zu (no payload)",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ఫలితం = rtp_client_write(దత్తాంశం, rtp_buf, rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
        ఆపు;
      }
      rtspc->state = RTP_PARSE_దత్తాంశం;
      ఆపు;
    }

    స్థితి RTP_PARSE_దత్తాంశం: {
      size_t rtp_len = curlx_dyn_len(&rtspc->buf);
      size_t needed;
      DEBUGASSERT(rtp_len < rtspc->rtp_len);
      needed = rtspc->rtp_len - rtp_len;
      ఒకవేళ(needed <= blen) {
        ఒకవేళ(curlx_dyn_addn(&rtspc->buf, buf, needed)) {
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          వెళ్ళు బహిర్గతం;
        }
        *pconsumed += needed;
        buf += needed;
        blen -= needed;
        /* complete RTP message in buffer */
        DEBUGF(infof(దత్తాంశం, "RTP write channel %d rtp_len %zu",
                     rtspc->rtp_channel, rtspc->rtp_len));
        ఫలితం = rtp_client_write(దత్తాంశం, curlx_dyn_ptr(&rtspc->buf),
                                  rtspc->rtp_len);
        curlx_dyn_free(&rtspc->buf);
        rtspc->state = RTP_PARSE_SKIP;
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
      లేకపోతే {
        ఒకవేళ(curlx_dyn_addn(&rtspc->buf, buf, blen)) {
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          వెళ్ళు బహిర్గతం;
        }
        *pconsumed += blen;
        buf += blen;
        blen = 0;
      }
      ఆపు;
    }

    అప్రమేయ:
      DEBUGASSERT(0);
      తిరిగి CURLE_RECV_ERROR;
    }
  }
బహిర్గతం:
  ఒకవేళ(!ఫలితం && skip_len)
    ఫలితం = rtp_write_body_junk(దత్తాంశం, rtspc, buf - skip_len, skip_len);
  తిరిగి ఫలితం;
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
స్థిర CURLcode rtsp_rtp_write_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి అక్షరం *buf,
                                    size_t blen,
                                    bool is_eos)
{
  నిర్మాణం rtsp_conn *rtspc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_RTSP_CONN);
  CURLcode ఫలితం = CURLE_OK;
  size_t consumed = 0;

  ఒకవేళ(!rtspc)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!దత్తాంశం->req.శీర్షిక)
    rtspc->లోపల_శీర్షిక = FALSE;
  ఒకవేళ(!blen) {
    వెళ్ళు బహిర్గతం;
  }

  DEBUGF(infof(దత్తాంశం, "rtsp_rtp_write_resp(len=%zu, in_header=%d, eos=%d)",
               blen, rtspc->లోపల_శీర్షిక, is_eos));

  /* If header parsing is not ongoing, extract RTP messages */
  ఒకవేళ(!rtspc->లోపల_శీర్షిక) {
    ఫలితం = rtsp_filter_rtp(దత్తాంశం, rtspc, buf, blen, &consumed);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    buf += consumed;
    blen -= consumed;
    /* either we consumed all or are at the start of header parsing */
    ఒకవేళ(blen && !దత్తాంశం->req.శీర్షిక)
      DEBUGF(infof(దత్తాంశం, "RTSP: %zu bytes, possibly excess in response body",
                   blen));
  }

  /* we want to parse headers, do so */
  ఒకవేళ(దత్తాంశం->req.శీర్షిక && blen) {
    rtspc->లోపల_శీర్షిక = TRUE;
    ఫలితం = Curl_http_write_resp_hds(దత్తాంశం, buf, blen, &consumed);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    buf += consumed;
    blen -= consumed;

    ఒకవేళ(!దత్తాంశం->req.శీర్షిక)
      rtspc->లోపల_శీర్షిక = FALSE;

    ఒకవేళ(!rtspc->లోపల_శీర్షిక) {
      /* If header parsing is done, extract interleaved RTP messages */
      ఒకవేళ(దత్తాంశం->req.కొలత <= -1) {
        /* Respect section 4.4 of rfc2326: If the Content-Length header is
           absent, a length 0 must be assumed. */
        దత్తాంశం->req.కొలత = 0;
        దత్తాంశం->req.download_పూర్తి = TRUE;
      }
      ఫలితం = rtsp_filter_rtp(దత్తాంశం, rtspc, buf, blen, &consumed);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      buf += consumed;
      blen -= consumed;
    }
  }

  ఒకవేళ(rtspc->state != RTP_PARSE_SKIP)
    దత్తాంశం->req.పూర్తి = FALSE;
  /* we SHOULD have consumed all bytes, unless the response is borked.
   * In which case we write out the left over bytes, letting the client
   * writer deal with it (it will report EXCESS and fail the transfer). */
  DEBUGF(infof(దత్తాంశం, "rtsp_rtp_write_resp(len=%zu, in_header=%d, done=%d, "
               "rtspc->state=%d, req.size=%" FMT_OFF_T ")",
               blen, rtspc->లోపల_శీర్షిక, దత్తాంశం->req.పూర్తి, (పూర్ణాంకం)rtspc->state,
               దత్తాంశం->req.కొలత));
  ఒకవేళ(!ఫలితం && (is_eos || blen)) {
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY |
                               (is_eos ? CLIENTWRITE_EOS : 0), buf, blen);
  }

బహిర్గతం:
  ఒకవేళ((దత్తాంశం->సమితి.rtspreq == RTSPREQ_RECEIVE) &&
     (rtspc->state == RTP_PARSE_SKIP)) {
    /* In special mode RECEIVE, we process one chunk of network
     * data, so we stop the transfer here, if we have no incomplete
     * RTP message pending. */
    దత్తాంశం->req.download_పూర్తి = TRUE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode rtsp_rtp_write_resp_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                       స్థిరరాశి అక్షరం *buf,
                                       size_t blen,
                                       bool is_eos)
{
  తిరిగి rtsp_rtp_write_resp(దత్తాంశం, buf, blen, is_eos);
}

స్థిర CURLcode rtsp_parse_transport(నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి అక్షరం *transport)
{
  /* If we receive multiple Transport response-headers, the interleaved
     channels of each response header is recorded and used together for
     subsequent data validity checks.*/
  /* e.g.: ' RTP/AVP/TCP;unicast;interleaved=5-6' */
  స్థిరరాశి అక్షరం *ప్రారంభం, *చివర;
  ప్రారంభం = transport;
  అయితే(ప్రారంభం && *ప్రారంభం) {
    curlx_str_passblanks(&ప్రారంభం);
    చివర = strchr(ప్రారంభం, ';');
    ఒకవేళ(checkprefix("interleaved=", ప్రారంభం)) {
      curl_off_t chan1, chan2, chan;
      స్థిరరాశి అక్షరం *p = ప్రారంభం + 12;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &chan1, 255)) {
        గుర్తులేని అక్షరం *rtp_channel_ముసుగు = దత్తాంశం->state.rtp_channel_ముసుగు;
        chan2 = chan1;
        ఒకవేళ(!curlx_str_single(&p, '-') && curlx_str_సంఖ్య(&p, &chan2, 255)) {
          infof(దత్తాంశం, "Unable to read the interleaved parameter from "
                "Transport header: [%s]", transport);
          chan2 = chan1;
        }
        కొరకు(chan = chan1; chan <= chan2; chan++) {
          పూర్ణాంకం idx = (పూర్ణాంకం)chan / 8;
          పూర్ణాంకం off = (పూర్ణాంకం)chan % 8;
          rtp_channel_ముసుగు[idx] |= (గుర్తులేని అక్షరం)(1 << off);
        }
      }
      లేకపోతే {
        infof(దత్తాంశం, "Unable to read the interleaved parameter from "
              "Transport header: [%s]", transport);
      }
      ఆపు;
    }
    /* skip to next parameter */
    ప్రారంభం = (!చివర) ? చివర : (చివర + 1);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_rtsp_parseheader(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *శీర్షిక)
{
  ఒకవేళ(checkprefix("CSeq:", శీర్షిక)) {
    curl_off_t CSeq = 0;
    నిర్మాణం RTSP *rtsp = Curl_meta_get(దత్తాంశం, CURL_META_RTSP_EASY);
    స్థిరరాశి అక్షరం *p = &శీర్షిక[5];
    ఒకవేళ(!rtsp)
      తిరిగి CURLE_FAILED_INIT;
    curlx_str_passblanks(&p);
    ఒకవేళ(curlx_str_సంఖ్య(&p, &CSeq, UINT_MAX)) {
      failf(దత్తాంశం, "Unable to read the CSeq header: [%s]", శీర్షిక);
      తిరిగి CURLE_RTSP_CSEQ_ERROR;
    }
    దత్తాంశం->state.rtsp_CSeq_recv = rtsp->CSeq_recv = (uint32_t)CSeq;
  }
  లేకపోతే ఒకవేళ(checkprefix("Session:", శీర్షిక)) {
    స్థిరరాశి అక్షరం *ప్రారంభం, *చివర, *str;
    size_t idlen;

    /* Find the first non-space letter */
    ప్రారంభం = శీర్షిక + 8;
    curlx_str_passblanks(&ప్రారంభం);

    ఒకవేళ(!*ప్రారంభం) {
      failf(దత్తాంశం, "Got a blank Session ID");
      తిరిగి CURLE_RTSP_SESSION_ERROR;
    }

    /* Find the end of Session ID
     *
     * Allow any non whitespace content, up to the field separator or end of
     * line. RFC 2326 is not 100% clear on the session ID and for example
     * gstreamer does URL-encoded session ID's not covered by the standard.
     */
    చివర = ప్రారంభం;
    అయితే((*చివర > ' ') && (*చివర != ';'))
      చివర++;
    idlen = చివర - ప్రారంభం;

    str = CURL_EASY_STR(దత్తాంశం, STRING_RTSP_SESSION_ID);
    ఒకవేళ(str) {

      /* If the Session ID is set, then compare */
      ఒకవేళ(అక్షరశ్రేణి_పొడవు(str) != idlen ||
         అక్షరశ్రేణి_పోలిక(ప్రారంభం, str, idlen)) {
        failf(దత్తాంశం, "Got RTSP Session ID Line [%s], but wanted ID [%s]",
              ప్రారంభం, str);
        తిరిగి CURLE_RTSP_SESSION_ERROR;
      }
    }
    లేకపోతే {
      /* If the Session ID is not set, and we find it in a response, then set
       * it.
       * Copy the id substring into a new buffer */
      శూన్య *mem = curlx_memdup0(ప్రారంభం, idlen);
      ఒకవేళ(!mem ||
         CURL_EASY_STR_SETN(దత్తాంశం, STRING_RTSP_SESSION_ID, mem))
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      Curl_peer_link(&దత్తాంశం->state.rtsp_session_origin, దత్తాంశం->state.origin);
    }
  }
  లేకపోతే ఒకవేళ(checkprefix("Transport:", శీర్షిక)) {
    CURLcode ఫలితం;
    ఫలితం = rtsp_parse_transport(దత్తాంశం, శీర్షిక + 10);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

/*
 * RTSP handler interface.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_rtsp = {
  rtsp_అమరిక_connection,                /* setup_connection */
  rtsp_do,                              /* do_it */
  rtsp_పూర్తి,                            /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_RTSP */
