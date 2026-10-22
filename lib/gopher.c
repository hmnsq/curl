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
#అన్తర్భావ "gopher.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_GOPHER

#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "escape.h"

#నిర్వచితమైతే USE_SSL
స్థిర CURLcode gopher_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)పూర్తి;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode gopher_connecting(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం;

  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, TRUE, పూర్తి);
  ఒకవేళ(ఫలితం)
    connclose(conn);
  *పూర్తి = TRUE;
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/* Sends buf to the server and, optionally, writes it to the client too. */
స్థిర CURLcode send_buf(నిర్మాణం Curl_easy *దత్తాంశం,
                         స్థిరరాశి అక్షరం *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  పూర్ణాంకం what;

  అయితే(buf_len) {
    ఫలితం = Curl_xfer_send(దత్తాంశం, buf, buf_len, FALSE, &nwritten);
    ఒకవేళ(!ఫలితం) { /* Which may not have written it all! */
      ఒకవేళ(client_write) {
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, buf, nwritten);
        ఒకవేళ(ఫలితం)
          ఆపు;
      }

      ఒకవేళ(nwritten > buf_len) {
        DEBUGASSERT(0);
        ఆపు;
      }
      buf_len -= nwritten;
      buf += nwritten;
      ఒకవేళ(!buf_len)
        ఆపు; /* but it did write it all */
    }
    లేకపోతే
      ఆపు;

    timeout_ms = Curl_timeleft_ms(దత్తాంశం);
    ఒకవేళ(timeout_ms < 0) {
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    }
    ఒకవేళ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ఒకవేళ(what < 0) {
      ఫలితం = CURLE_SEND_ERROR;
      ఆపు;
    }
    లేకపోతే ఒకవేళ(!what) {
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    }
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode gopher_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *gopherpath;
  స్థిరరాశి అక్షరం *మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  స్థిరరాశి అక్షరం *query = దత్తాంశం->state.పైకి.query;
  స్థిరరాశి అక్షరం *buf = ఖాళీ;
  అక్షరం *buf_alloc = ఖాళీ;
  size_t buf_len;

  *పూర్తి = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(మార్గం);

  ఒకవేళ(query)
    gopherpath = curl_maprintf("%s?%s", మార్గం, query);
  లేకపోతే
    gopherpath = curlx_strdup(మార్గం);

  ఒకవేళ(!gopherpath)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  ఒకవేళ(అక్షరశ్రేణి_పొడవు(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  లేకపోతే {
    స్థిరరాశి అక్షరం *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    ఫలితం = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    ఒకవేళ(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(దత్తాంశం, "Bad gopher selector, CR or LF not allowed");
      తిరిగి CURLE_URL_MALFORMAT;
    }
  }

  ఫలితం = send_buf(దత్తాంశం, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  ఒకవేళ(!ఫలితం)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    ఫలితం = send_buf(దత్తాంశం, "\r\n", 2, FALSE);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "Failed sending Gopher request");
    తిరిగి ఫలితం;
  }
  ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, "\r\n", 2);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
  తిరిగి CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_gopher = {
  ZERO_NULL,                            /* setup_connection */
  gopher_do,                            /* do_it */
  ZERO_NULL,                            /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#నిర్వచితమైతే USE_SSL
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_gophers = {
  ZERO_NULL,                            /* setup_connection */
  gopher_do,                            /* do_it */
  ZERO_NULL,                            /* done */
  ZERO_NULL,                            /* do_more */
  gopher_connect,                       /* connect_it */
  gopher_connecting,                    /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};
#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_DISABLE_GOPHER */
