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

#جے_بنا_تعریف CURL_DISABLE_GOPHER

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "escape.h"

#جے_تعریف_شدہ USE_SSL
ساكن CURLcode gopher_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  (فراغ)انکڑے;
  (فراغ)پورا;
  أرجع CURLE_OK;
}

ساكن CURLcode gopher_connecting(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ;

  نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, TRUE, پورا);
  إذا(نتیجہ)
    connclose(conn);
  *پورا = TRUE;
  أرجع نتیجہ;
}
#جے_انت

/* Sends buf to the server and, optionally, writes it to the client too. */
ساكن CURLcode send_buf(بنية Curl_easy *انکڑے,
                         ثابت حرف *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  صحيح what;

  طالما(buf_len) {
    نتیجہ = Curl_xfer_send(انکڑے, buf, buf_len, FALSE, &nwritten);
    إذا(!نتیجہ) { /* Which may not have written it all! */
      إذا(client_write) {
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, buf, nwritten);
        إذا(نتیجہ)
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

    timeout_ms = Curl_timeleft_ms(انکڑے);
    إذا(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
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
      نتیجہ = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  أرجع نتیجہ;
}

ساكن CURLcode gopher_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *gopherpath;
  ثابت حرف *رستہ = انکڑے->state.اتے.رستہ;
  ثابت حرف *query = انکڑے->state.اتے.query;
  ثابت حرف *buf = عدم;
  حرف *buf_alloc = عدم;
  size_t buf_len;

  *پورا = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(رستہ);

  إذا(query)
    gopherpath = curl_maprintf("%s?%s", رستہ, query);
  وإلا
    gopherpath = curlx_strdup(رستہ);

  إذا(!gopherpath)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  إذا(لڑی_لمبائی(gopherpath) <= 2) {
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
    نتیجہ = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    إذا(نتیجہ)
      أرجع نتیجہ;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    إذا(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(انکڑے, "Bad gopher selector, CR or LF not allowed");
      أرجع CURLE_URL_MALFORMAT;
    }
  }

  نتیجہ = send_buf(انکڑے, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  إذا(!نتیجہ)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    نتیجہ = send_buf(انکڑے, "\r\n", 2, FALSE);
  إذا(نتیجہ) {
    failf(انکڑے, "Failed sending Gopher request");
    أرجع نتیجہ;
  }
  نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, "\r\n", 2);
  إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
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

#جے_تعریف_شدہ USE_SSL
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
#جے_انت

#جے_انت /* CURL_DISABLE_GOPHER */
