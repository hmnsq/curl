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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_GOPHER

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "url.h"
#شامل "escape.h"

#اگر_تعریف_شدہ USE_SSL
ساکن CURLcode gopher_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  (خالی)ڈیٹا;
  (خالی)مکمل;
  واپس CURLE_OK;
}

ساکن CURLcode gopher_connecting(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ;

  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, TRUE, مکمل);
  اگر(نتیجہ)
    connclose(conn);
  *مکمل = TRUE;
  واپس نتیجہ;
}
#اگر_ختم

/* Sends buf to the server and, optionally, writes it to the client too. */
ساکن CURLcode send_buf(ساخت Curl_easy *ڈیٹا,
                         ثابت حرف *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  صحیح what;

  جب(buf_len) {
    نتیجہ = Curl_xfer_send(ڈیٹا, buf, buf_len, FALSE, &nwritten);
    اگر(!نتیجہ) { /* Which may not have written it all! */
      اگر(client_write) {
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, buf, nwritten);
        اگر(نتیجہ)
          توڑو;
      }

      اگر(nwritten > buf_len) {
        DEBUGASSERT(0);
        توڑو;
      }
      buf_len -= nwritten;
      buf += nwritten;
      اگر(!buf_len)
        توڑو; /* but it did write it all */
    }
    ورنہ
      توڑو;

    timeout_ms = Curl_timeleft_ms(ڈیٹا);
    اگر(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
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
      نتیجہ = CURLE_SEND_ERROR;
      توڑو;
    }
    ورنہ اگر(!what) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    }
  }

  واپس نتیجہ;
}

ساکن CURLcode gopher_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *gopherpath;
  ثابت حرف *راستہ = ڈیٹا->state.اوپر.راستہ;
  ثابت حرف *query = ڈیٹا->state.اوپر.query;
  ثابت حرف *buf = تہی;
  حرف *buf_alloc = تہی;
  size_t buf_len;

  *مکمل = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(راستہ);

  اگر(query)
    gopherpath = curl_maprintf("%s?%s", راستہ, query);
  ورنہ
    gopherpath = curlx_strdup(راستہ);

  اگر(!gopherpath)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  اگر(لڑی_لمبائی(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  ورنہ {
    ثابت حرف *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    نتیجہ = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    اگر(نتیجہ)
      واپس نتیجہ;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    اگر(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(ڈیٹا, "Bad gopher selector, CR or LF not allowed");
      واپس CURLE_URL_MALFORMAT;
    }
  }

  نتیجہ = send_buf(ڈیٹا, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  اگر(!نتیجہ)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    نتیجہ = send_buf(ڈیٹا, "\r\n", 2, FALSE);
  اگر(نتیجہ) {
    failf(ڈیٹا, "Failed sending Gopher request");
    واپس نتیجہ;
  }
  نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, "\r\n", 2);
  اگر(نتیجہ)
    واپس نتیجہ;

  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
  واپس CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ثابت ساخت Curl_protocol Curl_protocol_gopher = {
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

#اگر_تعریف_شدہ USE_SSL
ثابت ساخت Curl_protocol Curl_protocol_gophers = {
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
#اگر_ختم

#اگر_ختم /* CURL_DISABLE_GOPHER */
