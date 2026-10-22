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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "pingpong.h"

#ifdef USE_PINGPONG

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(સંરચના Curl_easy *data,
                                     સંરચના pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  જો(xfer_remain_ms)
    પાછા CURLMIN(remain_ms, xfer_remain_ms);
  પાછા remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(સંરચના Curl_easy *data,
                           સંરચના pingpong *pp, bool block,
                           bool disconnecting)
{
  સંરચના connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  પૂર્ણાંક rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  જો(timeout_ms <= 0) {
    failf(data, "server response timeout");
    પાછા CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  જો(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    જો(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  નહીંતર {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  જો(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  નહીંતર જો(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  નહીંતર જો(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  નહીંતર {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  જો(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    જો(result)
      પાછા result;
  }

  જો(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  નહીંતર જો(rc)
    result = pp->statemachine(data, data->conn);
  નહીંતર જો(disconnecting)
    પાછા CURLE_OPERATION_TIMEDOUT;

  પાછા result;
}

/* initialize stuff to prepare for reading a fresh new response */
શૂન્ય Curl_pp_init(સંરચના pingpong *pp, અચલ સંરચના curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(સંરચના Curl_easy *data,
                        સંરચના pingpong *pp,
                        અચલ અક્ષર *prefix,
                        અચલ અક્ષર *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  અક્ષર *s;
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  જો(!conn)
    /* cannot send without a connection! */
    પાછા CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  જો(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    જો(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  જો(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  જો(result)
    પાછા result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  જો(result)
    પાછા result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  જો(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  નહીંતર જો(result)
    પાછા result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  જો(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  નહીંતર {
    pp->sendthis = ખાલી;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  પાછા CURLE_OK;
}

CURLcode Curl_pp_sendf(સંરચના Curl_easy *data, સંરચના pingpong *pp,
                       અચલ અક્ષર *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, ખાલી, fmt, ap);

  va_end(ap);

  પાછા result;
}

સ્થિર CURLcode pingpong_read(સંરચના Curl_easy *data,
                              int8_t sockindex,
                              અક્ષર *buffer,
                              size_t buflen,
                              size_t *nread)
{
  પાછા Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(સંરચના Curl_easy *data,
                          int8_t sockindex,
                          સંરચના pingpong *pp,
                          પૂર્ણાંક *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  સંરચના connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  અક્ષર buffer[900];
  પૂર્ણાંક maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  કરો {
    gotbytes = 0;
    જો(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    જો(!pp->overflow) {
      જો(!maxloops--) /* don't get stuck */
        પાછા CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, કદ(buffer),
                             &gotbytes);
      જો(result == CURLE_AGAIN)
        પાછા CURLE_OK;

      જો(result)
        પાછા result;

      જો(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        પાછા CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      જો(result)
        પાછા result;

      data->req.headerbytecount += (unsigned પૂર્ણાંક)gotbytes;

      pp->nread_resp += gotbytes;
    }

    કરો {
      અચલ અક્ષર *લીટી = curlx_dyn_ptr(&pp->recvbuf);
      અચલ અક્ષર *nl = memchr(લીટી, '\n', curlx_dyn_len(&pp->recvbuf));
      જો(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - લીટી + 1;

        જો(memchr(લીટી, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          પાછા CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, લીટી, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, લીટી, length);
        જો(result)
          પાછા result;

        જો(pp->endofresp(data, conn, લીટી, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          જો(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          નહીંતર
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          તોડો;
        }
        જો(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        નહીંતર
          curlx_dyn_reset(&pp->recvbuf);
      }
      નહીંતર {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        તોડો;
      }

    } જ્યારે(1); /* while there is buffer left to scan */

  } જ્યારે(gotbytes == કદ(buffer));

  pp->pending_resp = FALSE;

  પાછા result;
}

CURLcode Curl_pp_pollset(સંરચના Curl_easy *data,
                         સંરચના pingpong *pp,
                         સંરચના easy_pollset *ps)
{
  પૂર્ણાંક flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  પાછા Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(સંરચના Curl_easy *data,
                         સંરચના pingpong *pp)
{
  (શૂન્ય)data;
  પાછા pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(સંરચના Curl_easy *data,
                           સંરચના pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  જો(!Curl_pp_needs_flush(data, pp))
    પાછા CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  જો(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  જો(result)
    પાછા result;

  જો(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  નહીંતર {
    pp->sendthis = ખાલી;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_pp_disconnect(સંરચના pingpong *pp)
{
  જો(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, કદ(*pp));
  }
  પાછા CURLE_OK;
}

bool Curl_pp_moredata(સંરચના pingpong *pp)
{
  પાછા !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
