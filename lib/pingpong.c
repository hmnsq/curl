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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pingpong.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PINGPONG

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout ?
    ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
  ਜੇ(xfer_remain_ms)
    ਵਾਪਸ CURLMIN(remain_ms, xfer_remain_ms);
  ਵਾਪਸ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ pingpong *pp, bool ਖੰਡ,
                           bool disconnecting)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ਪੂਰਨ rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(ਅੰਕੜੇ, pp);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(timeout_ms <= 0) {
    failf(ਅੰਕੜੇ, "server response timeout");
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ਜੇ(ਖੰਡ) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ਜੇ(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ਨਹੀਂਤਾਂ {
    interval_ms = 0; /*ਤੁਰੰਤ*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(ਅੰਕੜੇ, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ਜੇ(Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET))
    rc = 1;
  ਨਹੀਂਤਾਂ ਜੇ(pp->ਉਛਾਲ)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!pp->sendleft && Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ਨਹੀਂਤਾਂ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ਜੇ(ਖੰਡ) {
    /* if we did not wait, we do not have to spend time on this now */
    ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(rc == -1) {
    failf(ਅੰਕੜੇ, "select/poll error");
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc)
    ਨਤੀਜਾ = pp->statemachine(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
  ਨਹੀਂਤਾਂ ਜੇ(disconnecting)
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* initialize stuff to prepare for reading a fresh new response */
ਖਾਲੀ Curl_pp_init(ਬਣਤਰ pingpong *pp, ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ pingpong *pp,
                        ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ,
                        ਅਚਲ ਅੱਖਰ *fmt,
                        va_list ਦਲੀਲਾਂ)
{
  size_t ਬਾਈਟਾਂ_written = 0;
  size_t write_len;
  ਅੱਖਰ *s;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ਜੇ(!conn)
    /* cannot send without a connection! */
    ਵਾਪਸ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ਜੇ(ਅਗੇਤਰ) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&pp->sendbuf, ਅਗੇਤਰ);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_vaddf(&pp->sendbuf, fmt, ਦਲੀਲਾਂ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* append CRLF */
  ਨਤੀਜਾ = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  ਨਤੀਜਾ = Curl_conn_send(ਅੰਕੜੇ, FIRSTSOCKET, s, write_len, FALSE,
                          &ਬਾਈਟਾਂ_written);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    ਬਾਈਟਾਂ_written = 0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, s, ਬਾਈਟਾਂ_written);

  ਜੇ(ਬਾਈਟਾਂ_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - ਬਾਈਟਾਂ_written;
  }
  ਨਹੀਂਤਾਂ {
    pp->sendthis = ਨਿਰਾ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ਅੰਕੜੇ);
  }

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_pp_sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ pingpong *pp,
                       ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  CURLcode ਨਤੀਜਾ;
  va_list ap;
  va_start(ap, fmt);

  ਨਤੀਜਾ = Curl_pp_vsendf(ਅੰਕੜੇ, pp, ਨਿਰਾ, fmt, ap);

  va_end(ap);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode pingpong_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              int8_t sockindex,
                              ਅੱਖਰ *ਬਫਰ,
                              size_t buflen,
                              size_t *nread)
{
  ਵਾਪਸ Curl_conn_recv(ਅੰਕੜੇ, sockindex, ਬਫਰ, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex,
                          ਬਣਤਰ pingpong *pp,
                          ਪੂਰਨ *ਕੋਡ, /* return the server code if done */
                          size_t *ਮਾਪ) /* size of the response */
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t gotbytes;
  ਅੱਖਰ ਬਫਰ[900];
  ਪੂਰਨ maxloops = 10;

  *ਕੋਡ = 0; /* 0 for errors or not done */
  *ਮਾਪ = 0;

  ਕਰੋ {
    gotbytes = 0;
    ਜੇ(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t ਭਰਿਆ = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_ਪੂਛ(&pp->recvbuf, ਭਰਿਆ - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ਜੇ(!pp->ਉਛਾਲ) {
      ਜੇ(!maxloops--) /* don't get stuck */
        ਵਾਪਸ CURLE_OK;

      ਨਤੀਜਾ = pingpong_read(ਅੰਕੜੇ, sockindex, ਬਫਰ, ਆਕਾਰ(ਬਫਰ),
                             &gotbytes);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
        ਵਾਪਸ CURLE_OK;

      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਜੇ(!gotbytes) {
        failf(ਅੰਕੜੇ, "response reading failed (errno: %d)", SOCKERRNO);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }

      ਨਤੀਜਾ = curlx_dyn_addn(&pp->recvbuf, ਬਫਰ, gotbytes);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਅੰਕੜੇ->req.headerbytecount += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)gotbytes;

      pp->nread_resp += gotbytes;
    }

    ਕਰੋ {
      ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&pp->recvbuf);
      ਅਚਲ ਅੱਖਰ *nl = memchr(ਸਤਰ, '\n', curlx_dyn_len(&pp->recvbuf));
      ਜੇ(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t ਲੰਮਾਈ = nl - ਸਤਰ + 1;

        ਜੇ(memchr(ਸਤਰ, 0, ਲੰਮਾਈ)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(ਅੰਕੜੇ, "NUL byte in server response line");
          ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
        }

        /* output debug output if that is requested */
        Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, ਸਤਰ, ਲੰਮਾਈ);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_INFO, ਸਤਰ, ਲੰਮਾਈ);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        ਜੇ(pp->endofresp(ਅੰਕੜੇ, conn, ਸਤਰ, ਲੰਮਾਈ, ਕੋਡ)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = ਲੰਮਾਈ;
          ਜੇ(curlx_dyn_len(&pp->recvbuf) > ਲੰਮਾਈ)
            pp->ਉਛਾਲ = curlx_dyn_len(&pp->recvbuf) - ਲੰਮਾਈ;
          ਨਹੀਂਤਾਂ
            pp->ਉਛਾਲ = 0;
          *ਮਾਪ = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ਰੋਕੋ;
        }
        ਜੇ(curlx_dyn_len(&pp->recvbuf) > ਲੰਮਾਈ)
          /* keep the remaining piece */
          curlx_dyn_ਪੂਛ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - ਲੰਮਾਈ);
        ਨਹੀਂਤਾਂ
          curlx_dyn_reset(&pp->recvbuf);
      }
      ਨਹੀਂਤਾਂ {
        /* without a newline, there is no overflow */
        pp->ਉਛਾਲ = 0;
        ਰੋਕੋ;
      }

    } ਜਦੋਂ(1); /* while there is buffer left to scan */

  } ਜਦੋਂ(gotbytes == ਆਕਾਰ(ਬਫਰ));

  pp->pending_resp = FALSE;

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_pp_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ pingpong *pp,
                         ਬਣਤਰ easy_pollset *ps)
{
  ਪੂਰਨ flags = pp->sendleft ? CURL_POLL_ਬਾਹਰਲਾ : CURL_POLL_ਵਿੱਚ;
  ਵਾਪਸ Curl_pollset_change(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ pingpong *pp)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!Curl_pp_needs_flush(ਅੰਕੜੇ, pp))
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_conn_send(ਅੰਕੜੇ, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    ਨਤੀਜਾ = CURLE_OK;
    written = 0;
  }
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ਨਹੀਂਤਾਂ {
    pp->sendthis = ਨਿਰਾ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_pp_disconnect(ਬਣਤਰ pingpong *pp)
{
  ਜੇ(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, ਆਕਾਰ(*pp));
  }
  ਵਾਪਸ CURLE_OK;
}

bool Curl_pp_moredata(ਬਣਤਰ pingpong *pp)
{
  ਵਾਪਸ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#ਜੇ_ਅੰਤ
