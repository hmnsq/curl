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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "escape.h"

#ifdef USE_SSL
நிலையான CURLcode gopher_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  (வெறுமை)data;
  (வெறுமை)done;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode gopher_connecting(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  எனில்(result)
    connclose(conn);
  *done = TRUE;
  திரும்பு result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
நிலையான CURLcode send_buf(கட்டமைப்பு Curl_easy *data,
                         மாறிலி எழுத்து *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  முழு what;

  போது(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    எனில்(!result) { /* Which may not have written it all! */
      எனில்(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        எனில்(result)
          நிறுத்து;
      }

      எனில்(nwritten > buf_len) {
        DEBUGASSERT(0);
        நிறுத்து;
      }
      buf_len -= nwritten;
      buf += nwritten;
      எனில்(!buf_len)
        நிறுத்து; /* but it did write it all */
    }
    இல்லையெனில்
      நிறுத்து;

    timeout_ms = Curl_timeleft_ms(data);
    எனில்(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    }
    எனில்(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    எனில்(what < 0) {
      result = CURLE_SEND_ERROR;
      நிறுத்து;
    }
    இல்லையெனில் எனில்(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    }
  }

  திரும்பு result;
}

நிலையான CURLcode gopher_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  எழுத்து *gopherpath;
  மாறிலி எழுத்து *path = data->state.up.path;
  மாறிலி எழுத்து *query = data->state.up.query;
  மாறிலி எழுத்து *buf = வெற்று;
  எழுத்து *buf_alloc = வெற்று;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  எனில்(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  இல்லையெனில்
    gopherpath = curlx_strdup(path);

  எனில்(!gopherpath)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  எனில்(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  இல்லையெனில் {
    மாறிலி எழுத்து *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    எனில்(result)
      திரும்பு result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    எனில்(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      திரும்பு CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  எனில்(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  எனில்(result) {
    failf(data, "Failed sending Gopher request");
    திரும்பு result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  எனில்(result)
    திரும்பு result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  திரும்பு CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_gopher = {
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
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_gophers = {
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
