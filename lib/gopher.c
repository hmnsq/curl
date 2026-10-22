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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "escape.h"

#ifdef USE_SSL
സ്ഥിതം CURLcode gopher_connect(ഘടന Curl_easy *data, bool *done)
{
  (ശൂന്യം)data;
  (ശൂന്യം)done;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode gopher_connecting(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  എങ്കിൽ(result)
    connclose(conn);
  *done = TRUE;
  തിരികെ result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
സ്ഥിതം CURLcode send_buf(ഘടന Curl_easy *data,
                         സ്ഥിരം അക്ഷരം *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  പൂർണ്ണസംഖ്യ what;

  ആകുമ്പോൾ(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    എങ്കിൽ(!result) { /* Which may not have written it all! */
      എങ്കിൽ(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        എങ്കിൽ(result)
          നിർത്തുക;
      }

      എങ്കിൽ(nwritten > buf_len) {
        DEBUGASSERT(0);
        നിർത്തുക;
      }
      buf_len -= nwritten;
      buf += nwritten;
      എങ്കിൽ(!buf_len)
        നിർത്തുക; /* but it did write it all */
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;

    timeout_ms = Curl_timeleft_ms(data);
    എങ്കിൽ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    }
    എങ്കിൽ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    എങ്കിൽ(what < 0) {
      result = CURLE_SEND_ERROR;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    }
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode gopher_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *gopherpath;
  സ്ഥിരം അക്ഷരം *path = data->state.up.path;
  സ്ഥിരം അക്ഷരം *query = data->state.up.query;
  സ്ഥിരം അക്ഷരം *buf = ശൂന്യ;
  അക്ഷരം *buf_alloc = ശൂന്യ;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  എങ്കിൽ(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  അല്ലെങ്കിൽ
    gopherpath = curlx_strdup(path);

  എങ്കിൽ(!gopherpath)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  എങ്കിൽ(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    എങ്കിൽ(result)
      തിരികെ result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    എങ്കിൽ(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      തിരികെ CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  എങ്കിൽ(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  എങ്കിൽ(result) {
    failf(data, "Failed sending Gopher request");
    തിരികെ result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  എങ്കിൽ(result)
    തിരികെ result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  തിരികെ CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

സ്ഥിരം ഘടന Curl_protocol Curl_protocol_gopher = {
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
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_gophers = {
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
