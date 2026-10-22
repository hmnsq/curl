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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gopher.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_GOPHER

#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਸਥਿਰ CURLcode gopher_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ਪੂਰਾ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode gopher_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, TRUE, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    connclose(conn);
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/* Sends buf to the server and, optionally, writes it to the client too. */
ਸਥਿਰ CURLcode send_buf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅਚਲ ਅੱਖਰ *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  ਪੂਰਨ what;

  ਜਦੋਂ(buf_len) {
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, buf, buf_len, FALSE, &nwritten);
    ਜੇ(!ਨਤੀਜਾ) { /* Which may not have written it all! */
      ਜੇ(client_write) {
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, buf, nwritten);
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;
      }

      ਜੇ(nwritten > buf_len) {
        DEBUGASSERT(0);
        ਰੋਕੋ;
      }
      buf_len -= nwritten;
      buf += nwritten;
      ਜੇ(!buf_len)
        ਰੋਕੋ; /* but it did write it all */
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;

    timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
    ਜੇ(timeout_ms < 0) {
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    }
    ਜੇ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ਜੇ(what < 0) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!what) {
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode gopher_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *gopherpath;
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  ਅਚਲ ਅੱਖਰ *query = ਅੰਕੜੇ->state.ਉੱਪਰ.query;
  ਅਚਲ ਅੱਖਰ *buf = ਨਿਰਾ;
  ਅੱਖਰ *buf_alloc = ਨਿਰਾ;
  size_t buf_len;

  *ਪੂਰਾ = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(ਰਸਤਾ);

  ਜੇ(query)
    gopherpath = curl_maprintf("%s?%s", ਰਸਤਾ, query);
  ਨਹੀਂਤਾਂ
    gopherpath = curlx_strdup(ਰਸਤਾ);

  ਜੇ(!gopherpath)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  ਜੇ(ਲੜੀ_ਲੰਬਾਈ(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    ਨਤੀਜਾ = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    ਜੇ(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(ਅੰਕੜੇ, "Bad gopher selector, CR or LF not allowed");
      ਵਾਪਸ CURLE_URL_MALFORMAT;
    }
  }

  ਨਤੀਜਾ = send_buf(ਅੰਕੜੇ, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  ਜੇ(!ਨਤੀਜਾ)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    ਨਤੀਜਾ = send_buf(ਅੰਕੜੇ, "\r\n", 2, FALSE);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "Failed sending Gopher request");
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, "\r\n", 2);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_gopher = {
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_gophers = {
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
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CURL_DISABLE_GOPHER */
