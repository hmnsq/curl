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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "pingpong.h"

#ifdef USE_PINGPONG

#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "multiif.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "progress.h"

timediff_t Curl_pp_state_timeleft_ms(بنية Curl_easy *data,
                                     بنية pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  إذا(xfer_remain_ms)
    أرجع CURLMIN(remain_ms, xfer_remain_ms);
  أرجع remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(بنية Curl_easy *data,
                           بنية pingpong *pp, bool block,
                           bool disconnecting)
{
  بنية connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  إذا(timeout_ms <= 0) {
    failf(data, "server response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  إذا(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    إذا(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  وإلا {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  إذا(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  وإلا إذا(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  وإلا إذا(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  وإلا {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  إذا(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    إذا(result)
      أرجع result;
  }

  إذا(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  وإلا إذا(rc)
    result = pp->statemachine(data, data->conn);
  وإلا إذا(disconnecting)
    أرجع CURLE_OPERATION_TIMEDOUT;

  أرجع result;
}

/* initialize stuff to prepare for reading a fresh new response */
فراغ Curl_pp_init(بنية pingpong *pp, ثابت بنية curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(بنية Curl_easy *data,
                        بنية pingpong *pp,
                        ثابت حرف *prefix,
                        ثابت حرف *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  حرف *s;
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  إذا(!conn)
    /* cannot send without a connection! */
    أرجع CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  إذا(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    إذا(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  إذا(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  إذا(result)
    أرجع result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  إذا(result)
    أرجع result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  إذا(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  وإلا إذا(result)
    أرجع result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  إذا(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  أرجع CURLE_OK;
}

CURLcode Curl_pp_sendf(بنية Curl_easy *data, بنية pingpong *pp,
                       ثابت حرف *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, عدم, fmt, ap);

  va_end(ap);

  أرجع result;
}

ساكن CURLcode pingpong_read(بنية Curl_easy *data,
                              int8_t sockindex,
                              حرف *buffer,
                              size_t buflen,
                              size_t *nread)
{
  أرجع Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(بنية Curl_easy *data,
                          int8_t sockindex,
                          بنية pingpong *pp,
                          صحيح *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  حرف buffer[900];
  صحيح maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  افعل {
    gotbytes = 0;
    إذا(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    إذا(!pp->overflow) {
      إذا(!maxloops--) /* don't get stuck */
        أرجع CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, حجم(buffer),
                             &gotbytes);
      إذا(result == CURLE_AGAIN)
        أرجع CURLE_OK;

      إذا(result)
        أرجع result;

      إذا(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      إذا(result)
        أرجع result;

      data->req.headerbytecount += (unsigned صحيح)gotbytes;

      pp->nread_resp += gotbytes;
    }

    افعل {
      ثابت حرف *هێڵ = curlx_dyn_ptr(&pp->recvbuf);
      ثابت حرف *nl = memchr(هێڵ, '\n', curlx_dyn_len(&pp->recvbuf));
      إذا(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - هێڵ + 1;

        إذا(memchr(هێڵ, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          أرجع CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, هێڵ, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, هێڵ, length);
        إذا(result)
          أرجع result;

        إذا(pp->endofresp(data, conn, هێڵ, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          إذا(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          وإلا
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          اكسر;
        }
        إذا(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        وإلا
          curlx_dyn_reset(&pp->recvbuf);
      }
      وإلا {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        اكسر;
      }

    } طالما(1); /* while there is buffer left to scan */

  } طالما(gotbytes == حجم(buffer));

  pp->pending_resp = FALSE;

  أرجع result;
}

CURLcode Curl_pp_pollset(بنية Curl_easy *data,
                         بنية pingpong *pp,
                         بنية easy_pollset *ps)
{
  صحيح flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  أرجع Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(بنية Curl_easy *data,
                         بنية pingpong *pp)
{
  (فراغ)data;
  أرجع pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(بنية Curl_easy *data,
                           بنية pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  إذا(!Curl_pp_needs_flush(data, pp))
    أرجع CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  إذا(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  إذا(result)
    أرجع result;

  إذا(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_pp_disconnect(بنية pingpong *pp)
{
  إذا(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, حجم(*pp));
  }
  أرجع CURLE_OK;
}

bool Curl_pp_moredata(بنية pingpong *pp)
{
  أرجع !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
