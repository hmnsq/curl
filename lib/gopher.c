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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "escape.h"

#ifdef USE_SSL
સ્થિર CURLcode gopher_connect(સંરચના Curl_easy *data, bool *done)
{
  (શૂન્ય)data;
  (શૂન્ય)done;
  પાછા CURLE_OK;
}

સ્થિર CURLcode gopher_connecting(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  જો(result)
    connclose(conn);
  *done = TRUE;
  પાછા result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
સ્થિર CURLcode send_buf(સંરચના Curl_easy *data,
                         અચલ અક્ષર *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  પૂર્ણાંક what;

  જ્યારે(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    જો(!result) { /* Which may not have written it all! */
      જો(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        જો(result)
          તોડો;
      }

      જો(nwritten > buf_len) {
        DEBUGASSERT(0);
        તોડો;
      }
      buf_len -= nwritten;
      buf += nwritten;
      જો(!buf_len)
        તોડો; /* but it did write it all */
    }
    નહીંતર
      તોડો;

    timeout_ms = Curl_timeleft_ms(data);
    જો(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    }
    જો(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    જો(what < 0) {
      result = CURLE_SEND_ERROR;
      તોડો;
    }
    નહીંતર જો(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    }
  }

  પાછા result;
}

સ્થિર CURLcode gopher_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  અક્ષર *gopherpath;
  અચલ અક્ષર *path = data->state.up.path;
  અચલ અક્ષર *query = data->state.up.query;
  અચલ અક્ષર *buf = ખાલી;
  અક્ષર *buf_alloc = ખાલી;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  જો(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  નહીંતર
    gopherpath = curlx_strdup(path);

  જો(!gopherpath)
    પાછા CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  જો(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  નહીંતર {
    અચલ અક્ષર *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    જો(result)
      પાછા result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    જો(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      પાછા CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  જો(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  જો(result) {
    failf(data, "Failed sending Gopher request");
    પાછા result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  જો(result)
    પાછા result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  પાછા CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

અચલ સંરચના Curl_protocol Curl_protocol_gopher = {
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
અચલ સંરચના Curl_protocol Curl_protocol_gophers = {
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
