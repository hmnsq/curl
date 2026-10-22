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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "url.h"
#شامل "escape.h"

#ifdef USE_SSL
ایستا CURLcode gopher_connect(ساختار Curl_easy *data, bool *done)
{
  (تهی)data;
  (تهی)done;
  بازگشت CURLE_OK;
}

ایستا CURLcode gopher_connecting(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  اگر(result)
    connclose(conn);
  *done = TRUE;
  بازگشت result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
ایستا CURLcode send_buf(ساختار Curl_easy *data,
                         ثابت نویسه *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  صحیح what;

  وقتی(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    اگر(!result) { /* Which may not have written it all! */
      اگر(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        اگر(result)
          بشکن;
      }

      اگر(nwritten > buf_len) {
        DEBUGASSERT(0);
        بشکن;
      }
      buf_len -= nwritten;
      buf += nwritten;
      اگر(!buf_len)
        بشکن; /* but it did write it all */
    }
    وگرنه
      بشکن;

    timeout_ms = Curl_timeleft_ms(data);
    اگر(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    }
    اگر(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    اگر(what < 0) {
      result = CURLE_SEND_ERROR;
      بشکن;
    }
    وگرنه اگر(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    }
  }

  بازگشت result;
}

ایستا CURLcode gopher_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  نویسه *gopherpath;
  ثابت نویسه *path = data->state.up.path;
  ثابت نویسه *query = data->state.up.query;
  ثابت نویسه *buf = پوچ;
  نویسه *buf_alloc = پوچ;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  اگر(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  وگرنه
    gopherpath = curlx_strdup(path);

  اگر(!gopherpath)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  اگر(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  وگرنه {
    ثابت نویسه *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    اگر(result)
      بازگشت result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    اگر(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      بازگشت CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  اگر(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  اگر(result) {
    failf(data, "Failed sending Gopher request");
    بازگشت result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  اگر(result)
    بازگشت result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  بازگشت CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ثابت ساختار Curl_protocol Curl_protocol_gopher = {
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
ثابت ساختار Curl_protocol Curl_protocol_gophers = {
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
