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
 *   'pingpong' is for generic back-and-forth support functions used by FTP,
 *   IMAP, POP3, SMTP and whatever more that likes them.
 *
 ***************************************************************************/
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "pingpong.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PINGPONG

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = ದತ್ತಾಂಶ->ಗಣ.server_response_timeout ?
    ದತ್ತಾಂಶ->ಗಣ.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
  ಆದರೆ(xfer_remain_ms)
    ಹಿಂತಿರುಗಿ CURLMIN(remain_ms, xfer_remain_ms);
  ಹಿಂತಿರುಗಿ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ pingpong *pp, bool ಖಂಡ,
                           bool disconnecting)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಂಕ rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(ದತ್ತಾಂಶ, pp);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(timeout_ms <= 0) {
    failf(ದತ್ತಾಂಶ, "server response timeout");
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ಆದರೆ(ಖಂಡ) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ಆದರೆ(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    interval_ms = 0; /*ತಕ್ಷಣ*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(ದತ್ತಾಂಶ, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ಆದರೆ(Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET))
    rc = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!pp->sendleft && Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ಇಲ್ಲದಿದ್ದರೆ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ಆದರೆ(ಖಂಡ) {
    /* if we did not wait, we do not have to spend time on this now */
    ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(rc == -1) {
    failf(ದತ್ತಾಂಶ, "select/poll error");
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc)
    ಫಲಿತಾಂಶ = pp->statemachine(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(disconnecting)
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* initialize stuff to prepare for reading a fresh new response */
ಶೂನ್ಯ Curl_pp_init(ರಚನೆ pingpong *pp, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ pingpong *pp,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt,
                        va_list ವಾದಗಳು)
{
  size_t ಬೈಟುಗಳು_written = 0;
  size_t write_len;
  ಅಕ್ಷರ *s;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ಆದರೆ(!conn)
    /* cannot send without a connection! */
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ಆದರೆ(ಪೂರ್ವಪ್ರತ್ಯಯ) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&pp->sendbuf, ಪೂರ್ವಪ್ರತ್ಯಯ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_vaddf(&pp->sendbuf, fmt, ವಾದಗಳು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* append CRLF */
  ಫಲಿತಾಂಶ = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  ಫಲಿತಾಂಶ = Curl_conn_send(ದತ್ತಾಂಶ, FIRSTSOCKET, s, write_len, FALSE,
                          &ಬೈಟುಗಳು_written);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    ಬೈಟುಗಳು_written = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, s, ಬೈಟುಗಳು_written);

  ಆದರೆ(ಬೈಟುಗಳು_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - ಬೈಟುಗಳು_written;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    pp->sendthis = ಖಾಲಿ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_pp_sendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ pingpong *pp,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  CURLcode ಫಲಿತಾಂಶ;
  va_list ap;
  va_start(ap, fmt);

  ಫಲಿತಾಂಶ = Curl_pp_vsendf(ದತ್ತಾಂಶ, pp, ಖಾಲಿ, fmt, ap);

  va_end(ap);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode pingpong_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              int8_t sockindex,
                              ಅಕ್ಷರ *ಬಫರ್,
                              size_t buflen,
                              size_t *nread)
{
  ಹಿಂತಿರುಗಿ Curl_conn_recv(ದತ್ತಾಂಶ, sockindex, ಬಫರ್, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          int8_t sockindex,
                          ರಚನೆ pingpong *pp,
                          ಪೂರ್ಣಾಂಕ *ಸಂಕೇತ_2, /* return the server code if done */
                          size_t *ಗಾತ್ರ_2) /* size of the response */
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t gotbytes;
  ಅಕ್ಷರ ಬಫರ್[900];
  ಪೂರ್ಣಾಂಕ maxloops = 10;

  *ಸಂಕೇತ_2 = 0; /* 0 for errors or not done */
  *ಗಾತ್ರ_2 = 0;

  ಮಾಡು {
    gotbytes = 0;
    ಆದರೆ(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t ತುಂಬಿದ = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_ಬಾಲ(&pp->recvbuf, ತುಂಬಿದ - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ಆದರೆ(!pp->ಅತಿಪ್ರವಾಹ) {
      ಆದರೆ(!maxloops--) /* don't get stuck */
        ಹಿಂತಿರುಗಿ CURLE_OK;

      ಫಲಿತಾಂಶ = pingpong_read(ದತ್ತಾಂಶ, sockindex, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್),
                             &gotbytes);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
        ಹಿಂತಿರುಗಿ CURLE_OK;

      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಆದರೆ(!gotbytes) {
        failf(ದತ್ತಾಂಶ, "response reading failed (errno: %d)", SOCKERRNO);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }

      ಫಲಿತಾಂಶ = curlx_dyn_addn(&pp->recvbuf, ಬಫರ್, gotbytes);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ದತ್ತಾಂಶ->req.headerbytecount += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)gotbytes;

      pp->nread_resp += gotbytes;
    }

    ಮಾಡು {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&pp->recvbuf);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nl = memchr(ಸಾಲು, '\n', curlx_dyn_len(&pp->recvbuf));
      ಆದರೆ(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t ಉದ್ದಳತೆ = nl - ಸಾಲು + 1;

        ಆದರೆ(memchr(ಸಾಲು, 0, ಉದ್ದಳತೆ)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(ದತ್ತಾಂಶ, "NUL byte in server response line");
          ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
        }

        /* output debug output if that is requested */
        Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, ಸಾಲು, ಉದ್ದಳತೆ);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_INFO, ಸಾಲು, ಉದ್ದಳತೆ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        ಆದರೆ(pp->endofresp(ದತ್ತಾಂಶ, conn, ಸಾಲು, ಉದ್ದಳತೆ, ಸಂಕೇತ_2)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = ಉದ್ದಳತೆ;
          ಆದರೆ(curlx_dyn_len(&pp->recvbuf) > ಉದ್ದಳತೆ)
            pp->ಅತಿಪ್ರವಾಹ = curlx_dyn_len(&pp->recvbuf) - ಉದ್ದಳತೆ;
          ಇಲ್ಲದಿದ್ದರೆ
            pp->ಅತಿಪ್ರವಾಹ = 0;
          *ಗಾತ್ರ_2 = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(curlx_dyn_len(&pp->recvbuf) > ಉದ್ದಳತೆ)
          /* keep the remaining piece */
          curlx_dyn_ಬಾಲ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - ಉದ್ದಳತೆ);
        ಇಲ್ಲದಿದ್ದರೆ
          curlx_dyn_reset(&pp->recvbuf);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* without a newline, there is no overflow */
        pp->ಅತಿಪ್ರವಾಹ = 0;
        ನಿಲ್ಲಿಸು;
      }

    } ಆಗ(1); /* while there is buffer left to scan */

  } ಆಗ(gotbytes == ಗಾತ್ರ(ಬಫರ್));

  pp->pending_resp = FALSE;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_pp_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ pingpong *pp,
                         ರಚನೆ easy_pollset *ps)
{
  ಪೂರ್ಣಾಂಕ flags = pp->sendleft ? CURL_POLL_ಹೊರಹೋಗು : CURL_POLL_ಒಳ;
  ಹಿಂತಿರುಗಿ Curl_pollset_change(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ pingpong *pp)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!Curl_pp_needs_flush(ದತ್ತಾಂಶ, pp))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_conn_send(ದತ್ತಾಂಶ, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    ಫಲಿತಾಂಶ = CURLE_OK;
    written = 0;
  }
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    pp->sendthis = ಖಾಲಿ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_pp_disconnect(ರಚನೆ pingpong *pp)
{
  ಆದರೆ(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, ಗಾತ್ರ(*pp));
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_pp_moredata(ರಚನೆ pingpong *pp)
{
  ಹಿಂತಿರುಗಿ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#ಆದರೆ_ಅಂತ್ಯ
