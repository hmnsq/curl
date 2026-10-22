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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "escape.h"

#ifdef USE_SSL
স্থির CURLcode gopher_connect(সংরচনা Curl_easy *data, bool *done)
{
  (শূন্য)data;
  (শূন্য)done;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode gopher_connecting(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  যদি(result)
    connclose(conn);
  *done = TRUE;
  ঘূৰাই result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
স্থির CURLcode send_buf(সংরচনা Curl_easy *data,
                         ধ্রুৱ ৱর্ণ *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  পূর্ণাঙ্ক what;

  যেতিয়ালৈ(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    যদি(!result) { /* Which may not have written it all! */
      যদি(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        যদি(result)
          ভঙ্গ;
      }

      যদি(nwritten > buf_len) {
        DEBUGASSERT(0);
        ভঙ্গ;
      }
      buf_len -= nwritten;
      buf += nwritten;
      যদি(!buf_len)
        ভঙ্গ; /* but it did write it all */
    }
    অন্যথা
      ভঙ্গ;

    timeout_ms = Curl_timeleft_ms(data);
    যদি(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
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
      result = CURLE_SEND_ERROR;
      ভঙ্গ;
    }
    অন্যথা যদি(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    }
  }

  ঘূৰাই result;
}

স্থির CURLcode gopher_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *gopherpath;
  ধ্রুৱ ৱর্ণ *path = data->state.up.path;
  ধ্রুৱ ৱর্ণ *query = data->state.up.query;
  ধ্রুৱ ৱর্ণ *buf = রিক্ত;
  ৱর্ণ *buf_alloc = রিক্ত;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  যদি(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  অন্যথা
    gopherpath = curlx_strdup(path);

  যদি(!gopherpath)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  যদি(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  অন্যথা {
    ধ্রুৱ ৱর্ণ *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    যদি(result)
      ঘূৰাই result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    যদি(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      ঘূৰাই CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  যদি(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  যদি(result) {
    failf(data, "Failed sending Gopher request");
    ঘূৰাই result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  যদি(result)
    ঘূৰাই result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  ঘূৰাই CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_gopher = {
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
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_gophers = {
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
