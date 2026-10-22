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
  जर(xfer_remain_ms)
    परत CURLMIN(remain_ms, xfer_remain_ms);
  परत remain_ms;
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
  पूर्णांक rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  जर(timeout_ms <= 0) {
    failf(data, "server response timeout");
    परत CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  जर(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    जर(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  अन्यथा {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  जर(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  अन्यथा जर(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  अन्यथा जर(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  अन्यथा {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  जर(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    जर(result)
      परत result;
  }

  जर(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  अन्यथा जर(rc)
    result = pp->statemachine(data, data->conn);
  अन्यथा जर(disconnecting)
    परत CURLE_OPERATION_TIMEDOUT;

  परत result;
}

/* initialize stuff to prepare for reading a fresh new response */
शून्य Curl_pp_init(संरचना pingpong *pp, स्थिरांक संरचना curltime *pnow)
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
                        स्थिरांक अक्षर *prefix,
                        स्थिरांक अक्षर *fmt,
                        बहु_सूची args)
{
  माप_प्रकार bytes_written = 0;
  माप_प्रकार write_len;
  अक्षर *s;
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  जर(!conn)
    /* cannot send without a connection! */
    परत CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  जर(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    जर(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  जर(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  जर(result)
    परत result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  जर(result)
    परत result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  जर(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  अन्यथा जर(result)
    परत result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  जर(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  अन्यथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  परत CURLE_OK;
}

CURLcode Curl_pp_sendf(संरचना Curl_easy *data, संरचना pingpong *pp,
                       स्थिरांक अक्षर *fmt, ...)
{
  CURLcode result;
  बहु_सूची ap;
  बहु_शुरू(ap, fmt);

  result = Curl_pp_vsendf(data, pp, रिक्त, fmt, ap);

  बहु_पूर्ण(ap);

  परत result;
}

स्थिर CURLcode pingpong_read(संरचना Curl_easy *data,
                              int8_t sockindex,
                              अक्षर *buffer,
                              माप_प्रकार buflen,
                              माप_प्रकार *nread)
{
  परत Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(संरचना Curl_easy *data,
                          int8_t sockindex,
                          संरचना pingpong *pp,
                          पूर्णांक *code, /* return the server code if done */
                          माप_प्रकार *size) /* size of the response */
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  माप_प्रकार gotbytes;
  अक्षर buffer[900];
  पूर्णांक maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  करा {
    gotbytes = 0;
    जर(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      माप_प्रकार full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    जर(!pp->overflow) {
      जर(!maxloops--) /* don't get stuck */
        परत CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, आकार(buffer),
                             &gotbytes);
      जर(result == CURLE_AGAIN)
        परत CURLE_OK;

      जर(result)
        परत result;

      जर(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        परत CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      जर(result)
        परत result;

      data->req.headerbytecount += (अचिन्हित पूर्णांक)gotbytes;

      pp->nread_resp += gotbytes;
    }

    करा {
      स्थिरांक अक्षर *ओळ = curlx_dyn_ptr(&pp->recvbuf);
      स्थिरांक अक्षर *nl = स_प्रथम(ओळ, '\n', curlx_dyn_len(&pp->recvbuf));
      जर(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        माप_प्रकार length = nl - ओळ + 1;

        जर(स_प्रथम(ओळ, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          परत CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, ओळ, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, ओळ, length);
        जर(result)
          परत result;

        जर(pp->endofresp(data, conn, ओळ, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          जर(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          अन्यथा
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          थांबा;
        }
        जर(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        अन्यथा
          curlx_dyn_reset(&pp->recvbuf);
      }
      अन्यथा {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        थांबा;
      }

    } पर्यंत(1); /* while there is buffer left to scan */

  } पर्यंत(gotbytes == आकार(buffer));

  pp->pending_resp = FALSE;

  परत result;
}

CURLcode Curl_pp_pollset(संरचना Curl_easy *data,
                         संरचना pingpong *pp,
                         संरचना easy_pollset *ps)
{
  पूर्णांक flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  परत Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(संरचना Curl_easy *data,
                         संरचना pingpong *pp)
{
  (शून्य)data;
  परत pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(संरचना Curl_easy *data,
                           संरचना pingpong *pp)
{
  /* we have a piece of a command still left to send */
  माप_प्रकार written;
  CURLcode result;

  जर(!Curl_pp_needs_flush(data, pp))
    परत CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  जर(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  जर(result)
    परत result;

  जर(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  अन्यथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  परत CURLE_OK;
}

CURLcode Curl_pp_disconnect(संरचना pingpong *pp)
{
  जर(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    स_रखो(pp, 0, आकार(*pp));
  }
  परत CURLE_OK;
}

bool Curl_pp_moredata(संरचना pingpong *pp)
{
  परत !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
