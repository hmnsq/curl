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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "pingpong.h"

#ifdef USE_PINGPONG

#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(সংরচনা Curl_easy *data,
                                     সংরচনা pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  যদি(xfer_remain_ms)
    ঘূৰাই CURLMIN(remain_ms, xfer_remain_ms);
  ঘূৰাই remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(সংরচনা Curl_easy *data,
                           সংরচনা pingpong *pp, bool block,
                           bool disconnecting)
{
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  পূর্ণাঙ্ক rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  যদি(timeout_ms <= 0) {
    failf(data, "server response timeout");
    ঘূৰাই CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  যদি(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    যদি(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  অন্যথা {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  যদি(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  অন্যথা যদি(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  অন্যথা যদি(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  অন্যথা {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  যদি(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    যদি(result)
      ঘূৰাই result;
  }

  যদি(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  অন্যথা যদি(rc)
    result = pp->statemachine(data, data->conn);
  অন্যথা যদি(disconnecting)
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;

  ঘূৰাই result;
}

/* initialize stuff to prepare for reading a fresh new response */
শূন্য Curl_pp_init(সংরচনা pingpong *pp, ধ্রুৱ সংরচনা curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(সংরচনা Curl_easy *data,
                        সংরচনা pingpong *pp,
                        ধ্রুৱ ৱর্ণ *prefix,
                        ধ্রুৱ ৱর্ণ *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  ৱর্ণ *s;
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  যদি(!conn)
    /* cannot send without a connection! */
    ঘূৰাই CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  যদি(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    যদি(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  যদি(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  যদি(result)
    ঘূৰাই result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  যদি(result)
    ঘূৰাই result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  যদি(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  অন্যথা যদি(result)
    ঘূৰাই result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  যদি(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  অন্যথা {
    pp->sendthis = রিক্ত;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  ঘূৰাই CURLE_OK;
}

CURLcode Curl_pp_sendf(সংরচনা Curl_easy *data, সংরচনা pingpong *pp,
                       ধ্রুৱ ৱর্ণ *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, রিক্ত, fmt, ap);

  va_end(ap);

  ঘূৰাই result;
}

স্থির CURLcode pingpong_read(সংরচনা Curl_easy *data,
                              int8_t sockindex,
                              ৱর্ণ *buffer,
                              size_t buflen,
                              size_t *nread)
{
  ঘূৰাই Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(সংরচনা Curl_easy *data,
                          int8_t sockindex,
                          সংরচনা pingpong *pp,
                          পূর্ণাঙ্ক *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  ৱর্ণ buffer[900];
  পূর্ণাঙ্ক maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  কৰা {
    gotbytes = 0;
    যদি(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    যদি(!pp->overflow) {
      যদি(!maxloops--) /* don't get stuck */
        ঘূৰাই CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, পরিমাণ(buffer),
                             &gotbytes);
      যদি(result == CURLE_AGAIN)
        ঘূৰাই CURLE_OK;

      যদি(result)
        ঘূৰাই result;

      যদি(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        ঘূৰাই CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      যদি(result)
        ঘূৰাই result;

      data->req.headerbytecount += (unsigned পূর্ণাঙ্ক)gotbytes;

      pp->nread_resp += gotbytes;
    }

    কৰা {
      ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&pp->recvbuf);
      ধ্রুৱ ৱর্ণ *nl = memchr(সাৰি, '\n', curlx_dyn_len(&pp->recvbuf));
      যদি(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - সাৰি + 1;

        যদি(memchr(সাৰি, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, সাৰি, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, সাৰি, length);
        যদি(result)
          ঘূৰাই result;

        যদি(pp->endofresp(data, conn, সাৰি, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          যদি(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          অন্যথা
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ভঙ্গ;
        }
        যদি(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        অন্যথা
          curlx_dyn_reset(&pp->recvbuf);
      }
      অন্যথা {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        ভঙ্গ;
      }

    } যেতিয়ালৈ(1); /* while there is buffer left to scan */

  } যেতিয়ালৈ(gotbytes == পরিমাণ(buffer));

  pp->pending_resp = FALSE;

  ঘূৰাই result;
}

CURLcode Curl_pp_pollset(সংরচনা Curl_easy *data,
                         সংরচনা pingpong *pp,
                         সংরচনা easy_pollset *ps)
{
  পূর্ণাঙ্ক flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  ঘূৰাই Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(সংরচনা Curl_easy *data,
                         সংরচনা pingpong *pp)
{
  (শূন্য)data;
  ঘূৰাই pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(সংরচনা Curl_easy *data,
                           সংরচনা pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  যদি(!Curl_pp_needs_flush(data, pp))
    ঘূৰাই CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  যদি(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  যদি(result)
    ঘূৰাই result;

  যদি(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  অন্যথা {
    pp->sendthis = রিক্ত;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_pp_disconnect(সংরচনা pingpong *pp)
{
  যদি(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, পরিমাণ(*pp));
  }
  ঘূৰাই CURLE_OK;
}

bool Curl_pp_moredata(সংরচনা pingpong *pp)
{
  ঘূৰাই !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
