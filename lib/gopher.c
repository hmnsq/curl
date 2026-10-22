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
#अन्तर्भाव "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "escape.h"

#ifdef USE_SSL
स्थिर CURLcode gopher_connect(संरचना Curl_easy *data, bool *done)
{
  (शून्य)data;
  (शून्य)done;
  परत CURLE_OK;
}

स्थिर CURLcode gopher_connecting(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  जर(result)
    connclose(conn);
  *done = TRUE;
  परत result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
स्थिर CURLcode send_buf(संरचना Curl_easy *data,
                         स्थिरांक अक्षर *buf,
                         माप_प्रकार buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  माप_प्रकार nwritten;
  timediff_t timeout_ms;
  पूर्णांक what;

  पर्यंत(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    जर(!result) { /* Which may not have written it all! */
      जर(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        जर(result)
          थांबा;
      }

      जर(nwritten > buf_len) {
        DEBUGASSERT(0);
        थांबा;
      }
      buf_len -= nwritten;
      buf += nwritten;
      जर(!buf_len)
        थांबा; /* but it did write it all */
    }
    अन्यथा
      थांबा;

    timeout_ms = Curl_timeleft_ms(data);
    जर(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      थांबा;
    }
    जर(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    जर(what < 0) {
      result = CURLE_SEND_ERROR;
      थांबा;
    }
    अन्यथा जर(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      थांबा;
    }
  }

  परत result;
}

स्थिर CURLcode gopher_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  अक्षर *gopherpath;
  स्थिरांक अक्षर *path = data->state.up.path;
  स्थिरांक अक्षर *query = data->state.up.query;
  स्थिरांक अक्षर *buf = रिक्त;
  अक्षर *buf_alloc = रिक्त;
  माप_प्रकार buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  जर(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  अन्यथा
    gopherpath = curlx_strdup(path);

  जर(!gopherpath)
    परत CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  जर(म_माप(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  अन्यथा {
    स्थिरांक अक्षर *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    जर(result)
      परत result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    जर(स_प्रथम(buf, '\r', buf_len) || स_प्रथम(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      परत CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  जर(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  जर(result) {
    failf(data, "Failed sending Gopher request");
    परत result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  जर(result)
    परत result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  परत CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

स्थिरांक संरचना Curl_protocol Curl_protocol_gopher = {
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
स्थिरांक संरचना Curl_protocol Curl_protocol_gophers = {
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
