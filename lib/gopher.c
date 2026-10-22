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

#यदि_अपरिभाषितम् CURL_DISABLE_GOPHER

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "escape.h"

#यदि_परिभाषितम् USE_SSL
स्थिर CURLcode gopher_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  (शून्य)दत्त;
  (शून्य)समाप्तम्;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode gopher_connecting(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम;

  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, TRUE, समाप्तम्);
  यदि(परिणाम)
    connclose(conn);
  *समाप्तम् = TRUE;
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/* Sends buf to the server and, optionally, writes it to the client too. */
स्थिर CURLcode send_buf(संरचना Curl_easy *दत्त,
                         ध्रुव वर्ण *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  पूर्णाङ्क what;

  यावत्(buf_len) {
    परिणाम = Curl_xfer_send(दत्त, buf, buf_len, FALSE, &nwritten);
    यदि(!परिणाम) { /* Which may not have written it all! */
      यदि(client_write) {
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, buf, nwritten);
        यदि(परिणाम)
          भङ्ग;
      }

      यदि(nwritten > buf_len) {
        DEBUGASSERT(0);
        भङ्ग;
      }
      buf_len -= nwritten;
      buf += nwritten;
      यदि(!buf_len)
        भङ्ग; /* but it did write it all */
    }
    अन्यथा
      भङ्ग;

    timeout_ms = Curl_timeleft_ms(दत्त);
    यदि(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
    यदि(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    यदि(what < 0) {
      परिणाम = CURLE_SEND_ERROR;
      भङ्ग;
    }
    अन्यथा यदि(!what) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode gopher_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *gopherpath;
  ध्रुव वर्ण *मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  ध्रुव वर्ण *query = दत्त->state.ऊर्ध्वम्.query;
  ध्रुव वर्ण *buf = रिक्त;
  वर्ण *buf_alloc = रिक्त;
  size_t buf_len;

  *समाप्तम् = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(मार्गः);

  यदि(query)
    gopherpath = curl_maprintf("%s?%s", मार्गः, query);
  अन्यथा
    gopherpath = curlx_strdup(मार्गः);

  यदि(!gopherpath)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  यदि(वाक्यदैर्घ्यम्(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  अन्यथा {
    ध्रुव वर्ण *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    परिणाम = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    यदि(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(दत्त, "Bad gopher selector, CR or LF not allowed");
      प्रत्यागम CURLE_URL_MALFORMAT;
    }
  }

  परिणाम = send_buf(दत्त, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  यदि(!परिणाम)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    परिणाम = send_buf(दत्त, "\r\n", 2, FALSE);
  यदि(परिणाम) {
    failf(दत्त, "Failed sending Gopher request");
    प्रत्यागम परिणाम;
  }
  परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, "\r\n", 2);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
  प्रत्यागम CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ध्रुव संरचना Curl_protocol Curl_protocol_gopher = {
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

#यदि_परिभाषितम् USE_SSL
ध्रुव संरचना Curl_protocol Curl_protocol_gophers = {
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
#यदि_अन्तः

#यदि_अन्तः /* CURL_DISABLE_GOPHER */
