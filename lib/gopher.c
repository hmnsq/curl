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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"

#ifdef USE_SSL
ସ୍ଥିର CURLcode gopher_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)done;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode gopher_connecting(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  ଯଦି(result)
    connclose(conn);
  *done = TRUE;
  ଫେରନ୍ତୁ result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
ସ୍ଥିର CURLcode send_buf(ସଂରଚନା Curl_easy *data,
                         ଅଚଳ ଅକ୍ଷର *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା what;

  ଯେତେବେଳେ(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    ଯଦି(!result) { /* Which may not have written it all! */
      ଯଦି(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        ଯଦି(result)
          ଭାଙ୍ଗ;
      }

      ଯଦି(nwritten > buf_len) {
        DEBUGASSERT(0);
        ଭାଙ୍ଗ;
      }
      buf_len -= nwritten;
      buf += nwritten;
      ଯଦି(!buf_len)
        ଭାଙ୍ଗ; /* but it did write it all */
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;

    timeout_ms = Curl_timeleft_ms(data);
    ଯଦି(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    }
    ଯଦି(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ଯଦି(what < 0) {
      result = CURLE_SEND_ERROR;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode gopher_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *gopherpath;
  ଅଚଳ ଅକ୍ଷର *path = data->state.up.path;
  ଅଚଳ ଅକ୍ଷର *query = data->state.up.query;
  ଅଚଳ ଅକ୍ଷର *buf = ଖାଲି;
  ଅକ୍ଷର *buf_alloc = ଖାଲି;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  ଯଦି(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  ଅନ୍ୟଥା
    gopherpath = curlx_strdup(path);

  ଯଦି(!gopherpath)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  ଯଦି(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    ଯଦି(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  ଯଦି(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  ଯଦି(result) {
    failf(data, "Failed sending Gopher request");
    ଫେରନ୍ତୁ result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_gopher = {
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

#ifdef USE_SSL
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_gophers = {
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
#endif

#endif /* CURL_DISABLE_GOPHER */
