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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "gopher.h"

#که_نه_تعریف_شوی CURL_DISABLE_GOPHER

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "escape.h"

#که_تعریف_شوی USE_SSL
ساكن CURLcode gopher_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
  (فراغ)معلومات;
  (فراغ)بشپړ;
  أرجع CURLE_OK;
}

ساكن CURLcode gopher_connecting(بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله;

  پایله = Curl_conn_connect(معلومات, FIRSTSOCKET, TRUE, بشپړ);
  إذا(پایله)
    connclose(conn);
  *بشپړ = TRUE;
  أرجع پایله;
}
#د_که_پای

/* Sends buf to the server and, optionally, writes it to the client too. */
ساكن CURLcode send_buf(بنية Curl_easy *معلومات,
                         ثابت حرف *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  صحيح what;

  طالما(buf_len) {
    پایله = Curl_xfer_send(معلومات, buf, buf_len, FALSE, &nwritten);
    إذا(!پایله) { /* Which may not have written it all! */
      إذا(client_write) {
        پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, buf, nwritten);
        إذا(پایله)
          اكسر;
      }

      إذا(nwritten > buf_len) {
        DEBUGASSERT(0);
        اكسر;
      }
      buf_len -= nwritten;
      buf += nwritten;
      إذا(!buf_len)
        اكسر; /* but it did write it all */
    }
    وإلا
      اكسر;

    timeout_ms = Curl_timeleft_ms(معلومات);
    إذا(timeout_ms < 0) {
      پایله = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
    إذا(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    إذا(what < 0) {
      پایله = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      پایله = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  أرجع پایله;
}

ساكن CURLcode gopher_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  حرف *gopherpath;
  ثابت حرف *لار = معلومات->state.پورته.لار;
  ثابت حرف *query = معلومات->state.پورته.query;
  ثابت حرف *buf = عدم;
  حرف *buf_alloc = عدم;
  size_t buf_len;

  *بشپړ = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(لار);

  إذا(query)
    gopherpath = curl_maprintf("%s?%s", لار, query);
  وإلا
    gopherpath = curlx_strdup(لار);

  إذا(!gopherpath)
    أرجع CURLE_بهر_د_حافظه;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  إذا(مزي_اوږدوالی(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  وإلا {
    ثابت حرف *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    پایله = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    إذا(پایله)
      أرجع پایله;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    إذا(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(معلومات, "Bad gopher selector, CR or LF not allowed");
      أرجع CURLE_URL_MALFORMAT;
    }
  }

  پایله = send_buf(معلومات, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  إذا(!پایله)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    پایله = send_buf(معلومات, "\r\n", 2, FALSE);
  إذا(پایله) {
    failf(معلومات, "Failed sending Gopher request");
    أرجع پایله;
  }
  پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, "\r\n", 2);
  إذا(پایله)
    أرجع پایله;

  Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, -1);
  أرجع CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ثابت بنية Curl_protocol Curl_protocol_gopher = {
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

#که_تعریف_شوی USE_SSL
ثابت بنية Curl_protocol Curl_protocol_gophers = {
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
#د_که_پای

#د_که_پای /* CURL_DISABLE_GOPHER */
