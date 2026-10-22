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
#অন্তর্ভাব "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "escape.h"

#ifdef USE_SSL
স্থির CURLcode gopher_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  (শূন্য)তথ্য;
  (শূন্য)done;
  ফেরত CURLE_OK;
}

স্থির CURLcode gopher_connecting(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল;

  ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, TRUE, done);
  যদি(ফলাফল)
    connclose(conn);
  *done = TRUE;
  ফেরত ফলাফল;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
স্থির CURLcode send_buf(কাঠামো Curl_easy *তথ্য,
                         ধ্রুবক অক্ষর *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  পূর্ণসংখ্যা what;

  যখন(buf_len) {
    ফলাফল = Curl_xfer_send(তথ্য, buf, buf_len, FALSE, &nwritten);
    যদি(!ফলাফল) { /* Which may not have written it all! */
      যদি(client_write) {
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, buf, nwritten);
        যদি(ফলাফল)
          বিরতি;
      }

      যদি(nwritten > buf_len) {
        DEBUGASSERT(0);
        বিরতি;
      }
      buf_len -= nwritten;
      buf += nwritten;
      যদি(!buf_len)
        বিরতি; /* but it did write it all */
    }
    অন্যথা
      বিরতি;

    timeout_ms = Curl_timeleft_ms(তথ্য);
    যদি(timeout_ms < 0) {
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
    }
    যদি(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    যদি(what < 0) {
      ফলাফল = CURLE_SEND_ERROR;
      বিরতি;
    }
    অন্যথা যদি(!what) {
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
    }
  }

  ফেরত ফলাফল;
}

স্থির CURLcode gopher_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *gopherpath;
  ধ্রুবক অক্ষর *path = তথ্য->state.up.path;
  ধ্রুবক অক্ষর *query = তথ্য->state.up.query;
  ধ্রুবক অক্ষর *buf = ফাঁকা;
  অক্ষর *buf_alloc = ফাঁকা;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  যদি(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  অন্যথা
    gopherpath = curlx_strdup(path);

  যদি(!gopherpath)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  যদি(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  অন্যথা {
    ধ্রুবক অক্ষর *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    ফলাফল = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    যদি(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(তথ্য, "Bad gopher selector, CR or LF not allowed");
      ফেরত CURLE_URL_MALFORMAT;
    }
  }

  ফলাফল = send_buf(তথ্য, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  যদি(!ফলাফল)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    ফলাফল = send_buf(তথ্য, "\r\n", 2, FALSE);
  যদি(ফলাফল) {
    failf(তথ্য, "Failed sending Gopher request");
    ফেরত ফলাফল;
  }
  ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, "\r\n", 2);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
  ফেরত CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ধ্রুবক কাঠামো Curl_protocol Curl_protocol_gopher = {
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
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_gophers = {
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
