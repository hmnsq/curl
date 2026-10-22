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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "pingpong.h"

#ifdef USE_PINGPONG

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  ଯଦି(xfer_remain_ms)
    ଫେରନ୍ତୁ CURLMIN(remain_ms, xfer_remain_ms);
  ଫେରନ୍ତୁ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା pingpong *pp, bool block,
                           bool disconnecting)
{
  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  ଯଦି(timeout_ms <= 0) {
    failf(data, "server response timeout");
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ଯଦି(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ଯଦି(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ଅନ୍ୟଥା {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ଯଦି(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  ଅନ୍ୟଥା ଯଦି(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ଅନ୍ୟଥା ଯଦି(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ଅନ୍ୟଥା {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ଯଦି(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  ଅନ୍ୟଥା ଯଦି(rc)
    result = pp->statemachine(data, data->conn);
  ଅନ୍ୟଥା ଯଦି(disconnecting)
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;

  ଫେରନ୍ତୁ result;
}

/* initialize stuff to prepare for reading a fresh new response */
ଶୂନ୍ୟ Curl_pp_init(ସଂରଚନା pingpong *pp, ଅଚଳ ସଂରଚନା curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା pingpong *pp,
                        ଅଚଳ ଅକ୍ଷର *prefix,
                        ଅଚଳ ଅକ୍ଷର *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  ଅକ୍ଷର *s;
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ଯଦି(!conn)
    /* cannot send without a connection! */
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ଯଦି(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    ଯଦି(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ଯଦି(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  ଯଦି(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  ଅନ୍ୟଥା ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  ଯଦି(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  ଅନ୍ୟଥା {
    pp->sendthis = ଖାଲି;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_pp_sendf(ସଂରଚନା Curl_easy *data, ସଂରଚନା pingpong *pp,
                       ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, ଖାଲି, fmt, ap);

  va_end(ap);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode pingpong_read(ସଂରଚନା Curl_easy *data,
                              int8_t sockindex,
                              ଅକ୍ଷର *buffer,
                              size_t buflen,
                              size_t *nread)
{
  ଫେରନ୍ତୁ Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ସଂରଚନା Curl_easy *data,
                          int8_t sockindex,
                          ସଂରଚନା pingpong *pp,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  ଅକ୍ଷର buffer[900];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  କର {
    gotbytes = 0;
    ଯଦି(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ଯଦି(!pp->overflow) {
      ଯଦି(!maxloops--) /* don't get stuck */
        ଫେରନ୍ତୁ CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, ଆକାର(buffer),
                             &gotbytes);
      ଯଦି(result == CURLE_AGAIN)
        ଫେରନ୍ତୁ CURLE_OK;

      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      ଯଦି(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      data->req.headerbytecount += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gotbytes;

      pp->nread_resp += gotbytes;
    }

    କର {
      ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&pp->recvbuf);
      ଅଚଳ ଅକ୍ଷର *nl = memchr(ଧାଡ଼ି, '\n', curlx_dyn_len(&pp->recvbuf));
      ଯଦି(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - ଧାଡ଼ି + 1;

        ଯଦି(memchr(ଧାଡ଼ି, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, ଧାଡ଼ି, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, ଧାଡ଼ି, length);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        ଯଦି(pp->endofresp(data, conn, ଧାଡ଼ି, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          ଯଦି(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          ଅନ୍ୟଥା
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ଭାଙ୍ଗ;
        }
        ଯଦି(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        ଅନ୍ୟଥା
          curlx_dyn_reset(&pp->recvbuf);
      }
      ଅନ୍ୟଥା {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        ଭାଙ୍ଗ;
      }

    } ଯେତେବେଳେ(1); /* while there is buffer left to scan */

  } ଯେତେବେଳେ(gotbytes == ଆକାର(buffer));

  pp->pending_resp = FALSE;

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_pp_pollset(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା pingpong *pp,
                         ସଂରଚନା easy_pollset *ps)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  ଫେରନ୍ତୁ Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା pingpong *pp)
{
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  ଯଦି(!Curl_pp_needs_flush(data, pp))
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ଯଦି(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ଅନ୍ୟଥା {
    pp->sendthis = ଖାଲି;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_pp_disconnect(ସଂରଚନା pingpong *pp)
{
  ଯଦି(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, ଆକାର(*pp));
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_pp_moredata(ସଂରଚନା pingpong *pp)
{
  ଫେରନ୍ତୁ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
