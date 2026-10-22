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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "gopher.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_GOPHER

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಸ್ಥಿರ CURLcode gopher_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ಮುಗಿದ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode gopher_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, TRUE, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    connclose(conn);
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/* Sends buf to the server and, optionally, writes it to the client too. */
ಸ್ಥಿರ CURLcode send_buf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                         size_t buf_len,
                         bool client_write)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  size_t nwritten;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಂಕ what;

  ಆಗ(buf_len) {
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, buf, buf_len, FALSE, &nwritten);
    ಆದರೆ(!ಫಲಿತಾಂಶ) { /* Which may not have written it all! */
      ಆದರೆ(client_write) {
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, buf, nwritten);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ(nwritten > buf_len) {
        DEBUGASSERT(0);
        ನಿಲ್ಲಿಸು;
      }
      buf_len -= nwritten;
      buf += nwritten;
      ಆದರೆ(!buf_len)
        ನಿಲ್ಲಿಸು; /* but it did write it all */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;

    timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
    ಆದರೆ(timeout_ms < 0) {
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ಆದರೆ(what < 0) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!what) {
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode gopher_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *gopherpath;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query = ದತ್ತಾಂಶ->state.ಮೇಲೆ.query;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf = ಖಾಲಿ;
  ಅಕ್ಷರ *buf_alloc = ಖಾಲಿ;
  size_t buf_len;

  *ಮುಗಿದ = TRUE; /* unconditionally */

  /* path is guaranteed non-NULL */
  DEBUGASSERT(ಮಾರ್ಗ);

  ಆದರೆ(query)
    gopherpath = curl_maprintf("%s?%s", ಮಾರ್ಗ, query);
  ಇಲ್ಲದಿದ್ದರೆ
    gopherpath = curlx_strdup(ಮಾರ್ಗ);

  ಆದರೆ(!gopherpath)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Create selector. Degenerate cases: / and /1 => convert to "" */
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(gopherpath) <= 2) {
    buf = "";
    buf_len = 0;
    curlx_free(gopherpath);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newp;

    /* Otherwise, drop / and the first character (i.e., item type) ... */
    newp = gopherpath;
    newp += 2;

    /* ... and finally unescape */
    ಫಲಿತಾಂಶ = Curl_urldecode(newp, 0, &buf_alloc, &buf_len, REJECT_ZERO);
    curlx_free(gopherpath);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    buf = buf_alloc;

    /* A decoded CR or LF would terminate the single-line gopher request and
       let a crafted URL smuggle additional bytes onto the wire. REJECT_ZERO
       only blocks NUL; reject CR and LF here too. A TAB is left alone as it
       is the legitimate gopher type-7 selector/search separator. */
    ಆದರೆ(memchr(buf, '\r', buf_len) || memchr(buf, '\n', buf_len)) {
      curlx_free(buf_alloc);
      failf(ದತ್ತಾಂಶ, "Bad gopher selector, CR or LF not allowed");
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }
  }

  ಫಲಿತಾಂಶ = send_buf(ದತ್ತಾಂಶ, buf, buf_len, TRUE);
  curlx_free(buf_alloc);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    /* Send CRLF to the server now, but defer writing it to the client to
       preserve the historical behavior of this file. */
    ಫಲಿತಾಂಶ = send_buf(ದತ್ತಾಂಶ, "\r\n", 2, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "Failed sending Gopher request");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, "\r\n", 2);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Gopher protocol handler.
 * This is also a nice simple template to build off for simple
 * connect-command-download protocols.
 */

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_gopher = {
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_gophers = {
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
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_GOPHER */
