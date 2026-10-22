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

#اگر_تعریف_نشده CURL_DISABLE_GOPHER

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "escape.h"

#اگر_تعریف_شده USE_SSL
ساكن CURLcode gopher_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  (فراغ)داده;
  (فراغ)انجامشده;
  أرجع CURLE_OK;
}

ساكن CURLcode gopher_connecting(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه;

  نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, TRUE, انجامشده);
  إذا(نتیجه)
    connclose(conn);
  *انجامشده = TRUE;
  أرجع نتیجه;
}
#پایان_اگر

/* Sends buf to the server and, optionally, writes it to the client too. */
ساكن CURLcode send_buf(بنية Curl_easy *داده,
                         ثابت حرف *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  صحيح what;

  طالما(buf_len) {
    نتیجه = Curl_xfer_send(داده, buf, buf_len, FALSE, &nwritten);
    إذا(!نتیجه) { /* Which may not have written it all! */
      إذا(client_write) {
        نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, buf, nwritten);
        إذا(نتیجه)
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

    timeout_ms = Curl_timeleft_ms(داده);
    إذا(timeout_ms < 0) {
      نتیجه = CURLE_OPERATION_TIMEDOUT;
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
      نتیجه = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  أرجع نتیجه;
}

ساكن CURLcode gopher_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *gopherpath;
  ثابت حرف *مسیر = داده->state.به_بالا.مسیر;
  ثابت حرف *query = داده->state.به_بالا.query;
  ثابت حرف *buf = عدم;
  حرف *buf_alloc = عدم;
  size_t buf_len;

  *انجامشده = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(مسیر);

  إذا(query)
    gopherpath = curl_maprintf("%s?%s", مسیر, query);
  وإلا
    gopherpath = curlx_strdup(مسیر);

  إذا(!gopherpath)
    أرجع CURLE_بیرون_از_حافظه;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  إذا(طول_رشته(gopherpath) <= 2) {
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
    نتیجه = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    إذا(نتیجه)
      أرجع نتیجه;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    إذا(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(داده, "Bad gopher selector, CR or LF not allowed");
      أرجع CURLE_URL_MALFORMAT;
    }
  }

  نتیجه = send_buf(داده, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  إذا(!نتیجه)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    نتیجه = send_buf(داده, "\r\n", 2, FALSE);
  إذا(نتیجه) {
    failf(داده, "Failed sending Gopher request");
    أرجع نتیجه;
  }
  نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, "\r\n", 2);
  إذا(نتیجه)
    أرجع نتیجه;

  Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);
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

#اگر_تعریف_شده USE_SSL
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
#پایان_اگر

#پایان_اگر /* CURL_DISABLE_GOPHER */
