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
#include "curl_setup.h"
#include "urldata.h"
#include "gopher.h"

#ifndef CURL_DISABLE_GOPHER

#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "cfilters.h"
#include "connect.h"
#include "select.h"
#include "url.h"
#include "escape.h"

#ifdef USE_SSL
static CURLcode gopher_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  (void)data;
  (void)done;
  ᯀᯮᯞ CURLE_OK;
}

static CURLcode gopher_connecting(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  CURLcode result;

  result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
  ᯀᯮᯝ(result)
    connclose(conn);
  *done = TRUE;
  ᯀᯮᯞ result;
}
#endif

/* Sends buf to the server and, optionally, writes it to the client too. */
static CURLcode send_buf(sa_nracanaa Curl_easy *data,
                         const var_na *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  puur_naa_nka what;

  ᯀᯬᯝ(buf_len) {
    result = Curl_xfer_send(data, buf, buf_len, FALSE, &nwritten);
    ᯀᯮᯝ(!result) { /* Which may not have written it all! */
      ᯀᯮᯝ(client_write) {
        result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, nwritten);
        ᯀᯮᯝ(result)
          bha_nga;
      }

      ᯀᯮᯝ(nwritten > buf_len) {
        DEBUGASSERT(0);
        bha_nga;
      }
      buf_len -= nwritten;
      buf += nwritten;
      ᯀᯮᯝ(!buf_len)
        bha_nga; /* but it did write it all */
    }
    ᯀᯮᯝᯂᯪ
      bha_nga;

    timeout_ms = Curl_timeleft_ms(data);
    ᯀᯮᯝ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    }
    ᯀᯮᯝ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ᯀᯮᯝ(what < 0) {
      result = CURLE_SEND_ERROR;
      bha_nga;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    }
  }

  ᯀᯮᯞ result;
}

static CURLcode gopher_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  var_na *gopherpath;
  const var_na *path = data->state.up.path;
  const var_na *query = data->state.up.query;
  const var_na *buf = NULL;
  var_na *buf_alloc = NULL;
  size_t buf_len;

  *done = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(path);

  ᯀᯮᯝ(query)
    gopherpath = curl_maprintf("%s?%s", path, query);
  ᯀᯮᯝᯂᯪ
    gopherpath = curlx_strdup(path);

  ᯀᯮᯝ(!gopherpath)
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  ᯀᯮᯝ(strlen(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  ᯀᯮᯝᯂᯪ {
    const var_na *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    result = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    ᯀᯮᯝ(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(data, "Bad gopher selector, CR or LF not allowed");
      ᯀᯮᯞ CURLE_URL_MALFORMAT;
    }
  }

  result = send_buf(data, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  ᯀᯮᯝ(!result)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    result = send_buf(data, "\r\n", 2, FALSE);
  ᯀᯮᯝ(result) {
    failf(data, "Failed sending Gopher request");
    ᯀᯮᯞ result;
  }
  result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  ᯀᯮᯞ CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

const sa_nracanaa Curl_protocol Curl_protocol_gopher = {
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
const sa_nracanaa Curl_protocol Curl_protocol_gophers = {
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
