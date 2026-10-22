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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "pingpong.h"

#ifdef USE_PINGPONG

#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  எனில்(xfer_remain_ms)
    திரும்பு CURLMIN(remain_ms, xfer_remain_ms);
  திரும்பு remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு pingpong *pp, bool block,
                           bool disconnecting)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  முழு rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  எனில்(timeout_ms <= 0) {
    failf(data, "server response timeout");
    திரும்பு CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  எனில்(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    எனில்(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  இல்லையெனில் {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  எனில்(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  இல்லையெனில் எனில்(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  இல்லையெனில் எனில்(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  இல்லையெனில் {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  எனில்(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் எனில்(rc)
    result = pp->statemachine(data, data->conn);
  இல்லையெனில் எனில்(disconnecting)
    திரும்பு CURLE_OPERATION_TIMEDOUT;

  திரும்பு result;
}

/* initialize stuff to prepare for reading a fresh new response */
வெறுமை Curl_pp_init(கட்டமைப்பு pingpong *pp, மாறிலி கட்டமைப்பு curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு pingpong *pp,
                        மாறிலி எழுத்து *prefix,
                        மாறிலி எழுத்து *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  எழுத்து *s;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  எனில்(!conn)
    /* cannot send without a connection! */
    திரும்பு CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  எனில்(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    எனில்(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  எனில்(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  எனில்(result)
    திரும்பு result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  எனில்(result)
    திரும்பு result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  எனில்(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  இல்லையெனில் எனில்(result)
    திரும்பு result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  எனில்(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  இல்லையெனில் {
    pp->sendthis = வெற்று;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  திரும்பு CURLE_OK;
}

CURLcode Curl_pp_sendf(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு pingpong *pp,
                       மாறிலி எழுத்து *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, வெற்று, fmt, ap);

  va_end(ap);

  திரும்பு result;
}

நிலையான CURLcode pingpong_read(கட்டமைப்பு Curl_easy *data,
                              int8_t sockindex,
                              எழுத்து *buffer,
                              size_t buflen,
                              size_t *nread)
{
  திரும்பு Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(கட்டமைப்பு Curl_easy *data,
                          int8_t sockindex,
                          கட்டமைப்பு pingpong *pp,
                          முழு *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  எழுத்து buffer[900];
  முழு maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  செய் {
    gotbytes = 0;
    எனில்(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    எனில்(!pp->overflow) {
      எனில்(!maxloops--) /* don't get stuck */
        திரும்பு CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, அளவு(buffer),
                             &gotbytes);
      எனில்(result == CURLE_AGAIN)
        திரும்பு CURLE_OK;

      எனில்(result)
        திரும்பு result;

      எனில்(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        திரும்பு CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      எனில்(result)
        திரும்பு result;

      data->req.headerbytecount += (unsigned முழு)gotbytes;

      pp->nread_resp += gotbytes;
    }

    செய் {
      மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&pp->recvbuf);
      மாறிலி எழுத்து *nl = memchr(வரி, '\n', curlx_dyn_len(&pp->recvbuf));
      எனில்(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - வரி + 1;

        எனில்(memchr(வரி, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          திரும்பு CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, வரி, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, வரி, length);
        எனில்(result)
          திரும்பு result;

        எனில்(pp->endofresp(data, conn, வரி, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          எனில்(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          இல்லையெனில்
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          நிறுத்து;
        }
        எனில்(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        இல்லையெனில்
          curlx_dyn_reset(&pp->recvbuf);
      }
      இல்லையெனில் {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        நிறுத்து;
      }

    } போது(1); /* while there is buffer left to scan */

  } போது(gotbytes == அளவு(buffer));

  pp->pending_resp = FALSE;

  திரும்பு result;
}

CURLcode Curl_pp_pollset(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு pingpong *pp,
                         கட்டமைப்பு easy_pollset *ps)
{
  முழு flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  திரும்பு Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு pingpong *pp)
{
  (வெறுமை)data;
  திரும்பு pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  எனில்(!Curl_pp_needs_flush(data, pp))
    திரும்பு CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  எனில்(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  எனில்(result)
    திரும்பு result;

  எனில்(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  இல்லையெனில் {
    pp->sendthis = வெற்று;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_pp_disconnect(கட்டமைப்பு pingpong *pp)
{
  எனில்(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, அளவு(*pp));
  }
  திரும்பு CURLE_OK;
}

bool Curl_pp_moredata(கட்டமைப்பு pingpong *pp)
{
  திரும்பு !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
