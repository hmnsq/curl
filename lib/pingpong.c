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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "pingpong.h"

#ifdef USE_PINGPONG

#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ഘടന Curl_easy *data,
                                     ഘടന pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  എങ്കിൽ(xfer_remain_ms)
    തിരികെ CURLMIN(remain_ms, xfer_remain_ms);
  തിരികെ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ഘടന Curl_easy *data,
                           ഘടന pingpong *pp, bool block,
                           bool disconnecting)
{
  ഘടന connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  പൂർണ്ണസംഖ്യ rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(timeout_ms <= 0) {
    failf(data, "server response timeout");
    തിരികെ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  എങ്കിൽ(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    എങ്കിൽ(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  അല്ലെങ്കിൽ {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  എങ്കിൽ(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  അല്ലെങ്കിൽ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  എങ്കിൽ(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc)
    result = pp->statemachine(data, data->conn);
  അല്ലെങ്കിൽ എങ്കിൽ(disconnecting)
    തിരികെ CURLE_OPERATION_TIMEDOUT;

  തിരികെ result;
}

/* initialize stuff to prepare for reading a fresh new response */
ശൂന്യം Curl_pp_init(ഘടന pingpong *pp, സ്ഥിരം ഘടന curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ഘടന Curl_easy *data,
                        ഘടന pingpong *pp,
                        സ്ഥിരം അക്ഷരം *prefix,
                        സ്ഥിരം അക്ഷരം *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  അക്ഷരം *s;
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  എങ്കിൽ(!conn)
    /* cannot send without a connection! */
    തിരികെ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  എങ്കിൽ(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  എങ്കിൽ(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  എങ്കിൽ(result)
    തിരികെ result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  എങ്കിൽ(result)
    തിരികെ result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  എങ്കിൽ(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result)
    തിരികെ result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  എങ്കിൽ(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  അല്ലെങ്കിൽ {
    pp->sendthis = ശൂന്യ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  തിരികെ CURLE_OK;
}

CURLcode Curl_pp_sendf(ഘടന Curl_easy *data, ഘടന pingpong *pp,
                       സ്ഥിരം അക്ഷരം *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, ശൂന്യ, fmt, ap);

  va_end(ap);

  തിരികെ result;
}

സ്ഥിതം CURLcode pingpong_read(ഘടന Curl_easy *data,
                              int8_t sockindex,
                              അക്ഷരം *buffer,
                              size_t buflen,
                              size_t *nread)
{
  തിരികെ Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ഘടന Curl_easy *data,
                          int8_t sockindex,
                          ഘടന pingpong *pp,
                          പൂർണ്ണസംഖ്യ *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  അക്ഷരം buffer[900];
  പൂർണ്ണസംഖ്യ maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  ചെയ്യുക {
    gotbytes = 0;
    എങ്കിൽ(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    എങ്കിൽ(!pp->overflow) {
      എങ്കിൽ(!maxloops--) /* don't get stuck */
        തിരികെ CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, വലുപ്പം(buffer),
                             &gotbytes);
      എങ്കിൽ(result == CURLE_AGAIN)
        തിരികെ CURLE_OK;

      എങ്കിൽ(result)
        തിരികെ result;

      എങ്കിൽ(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        തിരികെ CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      എങ്കിൽ(result)
        തിരികെ result;

      data->req.headerbytecount += (unsigned പൂർണ്ണസംഖ്യ)gotbytes;

      pp->nread_resp += gotbytes;
    }

    ചെയ്യുക {
      സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&pp->recvbuf);
      സ്ഥിരം അക്ഷരം *nl = memchr(വരി, '\n', curlx_dyn_len(&pp->recvbuf));
      എങ്കിൽ(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - വരി + 1;

        എങ്കിൽ(memchr(വരി, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          തിരികെ CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, വരി, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, വരി, length);
        എങ്കിൽ(result)
          തിരികെ result;

        എങ്കിൽ(pp->endofresp(data, conn, വരി, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          എങ്കിൽ(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          അല്ലെങ്കിൽ
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          നിർത്തുക;
        }
        എങ്കിൽ(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        അല്ലെങ്കിൽ
          curlx_dyn_reset(&pp->recvbuf);
      }
      അല്ലെങ്കിൽ {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        നിർത്തുക;
      }

    } ആകുമ്പോൾ(1); /* while there is buffer left to scan */

  } ആകുമ്പോൾ(gotbytes == വലുപ്പം(buffer));

  pp->pending_resp = FALSE;

  തിരികെ result;
}

CURLcode Curl_pp_pollset(ഘടന Curl_easy *data,
                         ഘടന pingpong *pp,
                         ഘടന easy_pollset *ps)
{
  പൂർണ്ണസംഖ്യ flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  തിരികെ Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ഘടന Curl_easy *data,
                         ഘടന pingpong *pp)
{
  (ശൂന്യം)data;
  തിരികെ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ഘടന Curl_easy *data,
                           ഘടന pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  എങ്കിൽ(!Curl_pp_needs_flush(data, pp))
    തിരികെ CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  എങ്കിൽ(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  അല്ലെങ്കിൽ {
    pp->sendthis = ശൂന്യ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_pp_disconnect(ഘടന pingpong *pp)
{
  എങ്കിൽ(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, വലുപ്പം(*pp));
  }
  തിരികെ CURLE_OK;
}

bool Curl_pp_moredata(ഘടന pingpong *pp)
{
  തിരികെ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
