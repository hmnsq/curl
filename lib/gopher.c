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
#समावेश "gopher.h"

#अगर_अघोषित CURL_DISABLE_GOPHER

#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "escape.h"

#अगर_घोषित USE_SSL
अटल CURLcode gopher_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  (व्योम)तथ्य;
  (व्योम)done;
  वापस CURLE_OK;
}

अटल CURLcode gopher_connecting(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम;

  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, TRUE, done);
  अगर(परिणाम)
    connclose(conn);
  *done = TRUE;
  वापस परिणाम;
}
#पूर्ण_अगर

/* Sends buf to the server and, optionally, writes it to the client too. */
अटल CURLcode send_buf(काष्ठा Curl_easy *तथ्य,
                         स्थिर अक्षर *buf,
                         माप_प्रकार buf_len,
                         bool client_write)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  माप_प्रकार nwritten;
  timediff_t timeout_ms;
  पूर्णांक what;

  जबतक(buf_len) {
    परिणाम = Curl_xfer_send(तथ्य, buf, buf_len, FALSE, &nwritten);
    अगर(!परिणाम) { /* Which may not have written it all! */
      अगर(client_write) {
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, buf, nwritten);
        अगर(परिणाम)
          अवरोध;
      }

      अगर(nwritten > buf_len) {
        DEBUGASSERT(0);
        अवरोध;
      }
      buf_len -= nwritten;
      buf += nwritten;
      अगर(!buf_len)
        अवरोध; /* but it did write it all */
    }
    अन्यथा
      अवरोध;

    timeout_ms = Curl_timeleft_ms(तथ्य);
    अगर(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    }
    अगर(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    अगर(what < 0) {
      परिणाम = CURLE_SEND_ERROR;
      अवरोध;
    }
    अन्यथा अगर(!what) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    }
  }

  वापस परिणाम;
}

अटल CURLcode gopher_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *gopherpath;
  स्थिर अक्षर *path = तथ्य->state.up.path;
  स्थिर अक्षर *query = तथ्य->state.up.query;
  स्थिर अक्षर *buf = शून्य;
  अक्षर *buf_alloc = शून्य;
  माप_प्रकार buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  अगर(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  अन्यथा
    gopherpath = curlx_strdup(path);

  अगर(!gopherpath)
    वापस CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  अगर(म_माप(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  अन्यथा {
    स्थिर अक्षर *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    परिणाम = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    अगर(परिणाम)
      वापस परिणाम;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    अगर(स_प्रथम(buf, '\r', buf_len) || स_प्रथम(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(तथ्य, "Bad gopher selector, CR or LF not allowed");
      वापस CURLE_URL_MALFORMAT;
    }
  }

  परिणाम = send_buf(तथ्य, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  अगर(!परिणाम)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    परिणाम = send_buf(तथ्य, "\r\n", 2, FALSE);
  अगर(परिणाम) {
    failf(तथ्य, "Failed sending Gopher request");
    वापस परिणाम;
  }
  परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, "\r\n", 2);
  अगर(परिणाम)
    वापस परिणाम;

  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
  वापस CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

स्थिर काष्ठा Curl_protocol Curl_protocol_gopher = {
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

#अगर_घोषित USE_SSL
स्थिर काष्ठा Curl_protocol Curl_protocol_gophers = {
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
#पूर्ण_अगर

#पूर्ण_अगर /* CURL_DISABLE_GOPHER */
