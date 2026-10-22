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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "pingpong.h"

#ifdef USE_PINGPONG

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"

timediff_t Curl_pp_state_timeleft_ms(संरचना Curl_easy *data,
                                     संरचना pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  सचे(xfer_remain_ms)
    पटिनिवत्त CURLMIN(remain_ms, xfer_remain_ms);
  पटिनिवत्त remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(संरचना Curl_easy *data,
                           संरचना pingpong *pp, bool block,
                           bool disconnecting)
{
  संरचना connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  सचे(timeout_ms <= 0) {
    failf(data, "server response timeout");
    पटिनिवत्त CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  सचे(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    सचे(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  अञ्ञथा {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  सचे(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  अञ्ञथा सचे(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  अञ्ञथा सचे(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  अञ्ञथा {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  सचे(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    सचे(result)
      पटिनिवत्त result;
  }

  सचे(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  अञ्ञथा सचे(rc)
    result = pp->statemachine(data, data->conn);
  अञ्ञथा सचे(disconnecting)
    पटिनिवत्त CURLE_OPERATION_TIMEDOUT;

  पटिनिवत्त result;
}

/* initialize stuff to prepare for reading a fresh new response */
शून्य Curl_pp_init(संरचना pingpong *pp, ध्रुव संरचना curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(संरचना Curl_easy *data,
                        संरचना pingpong *pp,
                        ध्रुव वर्ण *prefix,
                        ध्रुव वर्ण *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  वर्ण *s;
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  सचे(!conn)
    /* cannot send without a connection! */
    पटिनिवत्त CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  सचे(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    सचे(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  सचे(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  सचे(result)
    पटिनिवत्त result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  सचे(result)
    पटिनिवत्त result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  सचे(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  अञ्ञथा सचे(result)
    पटिनिवत्त result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  सचे(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  अञ्ञथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_pp_sendf(संरचना Curl_easy *data, संरचना pingpong *pp,
                       ध्रुव वर्ण *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, रिक्त, fmt, ap);

  va_end(ap);

  पटिनिवत्त result;
}

स्थिर CURLcode pingpong_read(संरचना Curl_easy *data,
                              int8_t sockindex,
                              वर्ण *buffer,
                              size_t buflen,
                              size_t *nread)
{
  पटिनिवत्त Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(संरचना Curl_easy *data,
                          int8_t sockindex,
                          संरचना pingpong *pp,
                          पूर्णाङ्क *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  वर्ण buffer[900];
  पूर्णाङ्क maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  करोहि {
    gotbytes = 0;
    सचे(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    सचे(!pp->overflow) {
      सचे(!maxloops--) /* don't get stuck */
        पटिनिवत्त CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, परिमाण(buffer),
                             &gotbytes);
      सचे(result == CURLE_AGAIN)
        पटिनिवत्त CURLE_OK;

      सचे(result)
        पटिनिवत्त result;

      सचे(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        पटिनिवत्त CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      सचे(result)
        पटिनिवत्त result;

      data->req.headerbytecount += (unsigned पूर्णाङ्क)gotbytes;

      pp->nread_resp += gotbytes;
    }

    करोहि {
      ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&pp->recvbuf);
      ध्रुव वर्ण *nl = memchr(पङ्क्तिः, '\n', curlx_dyn_len(&pp->recvbuf));
      सचे(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - पङ्क्तिः + 1;

        सचे(memchr(पङ्क्तिः, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, पङ्क्तिः, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, पङ्क्तिः, length);
        सचे(result)
          पटिनिवत्त result;

        सचे(pp->endofresp(data, conn, पङ्क्तिः, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          सचे(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          अञ्ञथा
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          भङ्ग;
        }
        सचे(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        अञ्ञथा
          curlx_dyn_reset(&pp->recvbuf);
      }
      अञ्ञथा {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        भङ्ग;
      }

    } याव(1); /* while there is buffer left to scan */

  } याव(gotbytes == परिमाण(buffer));

  pp->pending_resp = FALSE;

  पटिनिवत्त result;
}

CURLcode Curl_pp_pollset(संरचना Curl_easy *data,
                         संरचना pingpong *pp,
                         संरचना easy_pollset *ps)
{
  पूर्णाङ्क flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  पटिनिवत्त Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(संरचना Curl_easy *data,
                         संरचना pingpong *pp)
{
  (शून्य)data;
  पटिनिवत्त pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(संरचना Curl_easy *data,
                           संरचना pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  सचे(!Curl_pp_needs_flush(data, pp))
    पटिनिवत्त CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  सचे(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  सचे(result)
    पटिनिवत्त result;

  सचे(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  अञ्ञथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_pp_disconnect(संरचना pingpong *pp)
{
  सचे(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, परिमाण(*pp));
  }
  पटिनिवत्त CURLE_OK;
}

bool Curl_pp_moredata(संरचना pingpong *pp)
{
  पटिनिवत्त !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
