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

#ifdef USE_PINGPONG

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  ಯದಿ(xfer_remain_ms)
    ಪ್ರತ್ಯಾಗಮ CURLMIN(remain_ms, xfer_remain_ms);
  ಪ್ರತ್ಯಾಗಮ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ pingpong *pp, bool block,
                           bool disconnecting)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಙ್ಕ rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  ಯದಿ(timeout_ms <= 0) {
    failf(data, "server response timeout");
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ಯದಿ(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ಯದಿ(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ಅನ್ಯಥಾ {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ಯದಿ(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  ಅನ್ಯಥಾ ಯದಿ(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ಅನ್ಯಥಾ ಯದಿ(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ಅನ್ಯಥಾ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ಯದಿ(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc)
    result = pp->statemachine(data, data->conn);
  ಅನ್ಯಥಾ ಯದಿ(disconnecting)
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;

  ಪ್ರತ್ಯಾಗಮ result;
}

/* initialize stuff to prepare for reading a fresh new response */
ಶೂನ್ಯ Curl_pp_init(ಸಂರಚನಾ pingpong *pp, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ pingpong *pp,
                        ಧ್ರುವ ವರ್ಣ *prefix,
                        ಧ್ರುವ ವರ್ಣ *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  ವರ್ಣ *s;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ಯದಿ(!conn)
    /* cannot send without a connection! */
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ಯದಿ(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ಯದಿ(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  ಯದಿ(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  ಅನ್ಯಥಾ ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  ಯದಿ(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  ಅನ್ಯಥಾ {
    pp->sendthis = ರಿಕ್ತ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_pp_sendf(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ pingpong *pp,
                       ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, ರಿಕ್ತ, fmt, ap);

  va_end(ap);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode pingpong_read(ಸಂರಚನಾ Curl_easy *data,
                              int8_t sockindex,
                              ವರ್ಣ *buffer,
                              size_t buflen,
                              size_t *nread)
{
  ಪ್ರತ್ಯಾಗಮ Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ಸಂರಚನಾ Curl_easy *data,
                          int8_t sockindex,
                          ಸಂರಚನಾ pingpong *pp,
                          ಪೂರ್ಣಾಙ್ಕ *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  ವರ್ಣ buffer[900];
  ಪೂರ್ಣಾಙ್ಕ maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  ಕುರು {
    gotbytes = 0;
    ಯದಿ(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ಯದಿ(!pp->overflow) {
      ಯದಿ(!maxloops--) /* don't get stuck */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, ಪರಿಮಾಣ(buffer),
                             &gotbytes);
      ಯದಿ(result == CURLE_AGAIN)
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;

      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      ಯದಿ(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      data->req.headerbytecount += (unsigned ಪೂರ್ಣಾಙ್ಕ)gotbytes;

      pp->nread_resp += gotbytes;
    }

    ಕುರು {
      ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&pp->recvbuf);
      ಧ್ರುವ ವರ್ಣ *nl = memchr(ಸಾಲು, '\n', curlx_dyn_len(&pp->recvbuf));
      ಯದಿ(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - ಸಾಲು + 1;

        ಯದಿ(memchr(ಸಾಲು, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, ಸಾಲು, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, ಸಾಲು, length);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        ಯದಿ(pp->endofresp(data, conn, ಸಾಲು, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          ಯದಿ(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          ಅನ್ಯಥಾ
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ಭಙ್ಗ;
        }
        ಯದಿ(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        ಅನ್ಯಥಾ
          curlx_dyn_reset(&pp->recvbuf);
      }
      ಅನ್ಯಥಾ {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        ಭಙ್ಗ;
      }

    } ಯಾವತ್(1); /* while there is buffer left to scan */

  } ಯಾವತ್(gotbytes == ಪರಿಮಾಣ(buffer));

  pp->pending_resp = FALSE;

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_pp_pollset(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ pingpong *pp,
                         ಸಂರಚನಾ easy_pollset *ps)
{
  ಪೂರ್ಣಾಙ್ಕ flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ pingpong *pp)
{
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  ಯದಿ(!Curl_pp_needs_flush(data, pp))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ಯದಿ(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ಅನ್ಯಥಾ {
    pp->sendthis = ರಿಕ್ತ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_pp_disconnect(ಸಂರಚನಾ pingpong *pp)
{
  ಯದಿ(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, ಪರಿಮಾಣ(*pp));
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

bool Curl_pp_moredata(ಸಂರಚನಾ pingpong *pp)
{
  ಪ್ರತ್ಯಾಗಮ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
