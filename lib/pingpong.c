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
#include "curl_setup.h"

#include "urldata.h"
#include "pingpong.h"

#ifdef USE_PINGPONG

#include "cfilters.h"
#include "connect.h"
#include "multiif.h"
#include "sendf.h"
#include "curl_trc.h"
#include "select.h"
#include "progress.h"

timediff_t Curl_pp_state_timeleft_ms(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  ꦪꦼꦤ꧀(xfer_remain_ms)
    ꦮꦭꦶ CURLMIN(remain_ms, xfer_remain_ms);
  ꦮꦭꦶ remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(sa_nracanaa Curl_easy *data,
                           sa_nracanaa pingpong *pp, bool block,
                           bool disconnecting)
{
  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  puur_naa_nka rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  ꦪꦼꦤ꧀(timeout_ms <= 0) {
    failf(data, "server response timeout");
    ꦮꦭꦶ CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ꦪꦼꦤ꧀(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ꦪꦼꦤ꧀(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ꦭꦶꦪ {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ꦪꦼꦤ꧀(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  ꦭꦶꦪ ꦪꦼꦤ꧀(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ꦭꦶꦪ ꦪꦼꦤ꧀(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ꦭꦶꦪ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ꦪꦼꦤ꧀(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ result;
  }

  ꦪꦼꦤ꧀(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(rc)
    result = pp->statemachine(data, data->conn);
  ꦭꦶꦪ ꦪꦼꦤ꧀(disconnecting)
    ꦮꦭꦶ CURLE_OPERATION_TIMEDOUT;

  ꦮꦭꦶ result;
}

/* initialize stuff to prepare for reading a fresh new response */
void Curl_pp_init(sa_nracanaa pingpong *pp, const sa_nracanaa curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(sa_nracanaa Curl_easy *data,
                        sa_nracanaa pingpong *pp,
                        const var_na *prefix,
                        const var_na *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  var_na *s;
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ꦪꦼꦤ꧀(!conn)
    /* cannot send without a connection! */
    ꦮꦭꦶ CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ꦪꦼꦤ꧀(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    ꦪꦼꦤ꧀(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ꦪꦼꦤ꧀(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  ꦪꦼꦤ꧀(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  ꦪꦼꦤ꧀(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  ꦭꦶꦪ {
    pp->sendthis = NULL;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  ꦮꦭꦶ CURLE_OK;
}

CURLcode Curl_pp_sendf(sa_nracanaa Curl_easy *data, sa_nracanaa pingpong *pp,
                       const var_na *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, NULL, fmt, ap);

  va_end(ap);

  ꦮꦭꦶ result;
}

static CURLcode pingpong_read(sa_nracanaa Curl_easy *data,
                              int8_t sockindex,
                              var_na *buffer,
                              size_t buflen,
                              size_t *nread)
{
  ꦮꦭꦶ Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(sa_nracanaa Curl_easy *data,
                          int8_t sockindex,
                          sa_nracanaa pingpong *pp,
                          puur_naa_nka *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  sa_nracanaa connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  var_na buffer[900];
  puur_naa_nka maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  do {
    gotbytes = 0;
    ꦪꦼꦤ꧀(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ꦪꦼꦤ꧀(!pp->overflow) {
      ꦪꦼꦤ꧀(!maxloops--) /* don't get stuck */
        ꦮꦭꦶ CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, parimaa_na(buffer),
                             &gotbytes);
      ꦪꦼꦤ꧀(result == CURLE_AGAIN)
        ꦮꦭꦶ CURLE_OK;

      ꦪꦼꦤ꧀(result)
        ꦮꦭꦶ result;

      ꦪꦼꦤ꧀(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        ꦮꦭꦶ CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      ꦪꦼꦤ꧀(result)
        ꦮꦭꦶ result;

      data->req.headerbytecount += (unsigned puur_naa_nka)gotbytes;

      pp->nread_resp += gotbytes;
    }

    do {
      const var_na *line = curlx_dyn_ptr(&pp->recvbuf);
      const var_na *nl = memchr(line, '\n', curlx_dyn_len(&pp->recvbuf));
      ꦪꦼꦤ꧀(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - line + 1;

        ꦪꦼꦤ꧀(memchr(line, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          ꦮꦭꦶ CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, line, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, line, length);
        ꦪꦼꦤ꧀(result)
          ꦮꦭꦶ result;

        ꦪꦼꦤ꧀(pp->endofresp(data, conn, line, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          ꦪꦼꦤ꧀(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          ꦭꦶꦪ
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          bha_nga;
        }
        ꦪꦼꦤ꧀(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        ꦭꦶꦪ
          curlx_dyn_reset(&pp->recvbuf);
      }
      ꦭꦶꦪ {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        bha_nga;
      }

    } ꦱꦼꦢꦼꦭ(1); /* while there is buffer left to scan */

  } ꦱꦼꦢꦼꦭ(gotbytes == parimaa_na(buffer));

  pp->pending_resp = FALSE;

  ꦮꦭꦶ result;
}

CURLcode Curl_pp_pollset(sa_nracanaa Curl_easy *data,
                         sa_nracanaa pingpong *pp,
                         sa_nracanaa easy_pollset *ps)
{
  puur_naa_nka flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  ꦮꦭꦶ Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(sa_nracanaa Curl_easy *data,
                         sa_nracanaa pingpong *pp)
{
  (void)data;
  ꦮꦭꦶ pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(sa_nracanaa Curl_easy *data,
                           sa_nracanaa pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  ꦪꦼꦤ꧀(!Curl_pp_needs_flush(data, pp))
    ꦮꦭꦶ CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ꦪꦼꦤ꧀(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  ꦪꦼꦤ꧀(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ꦭꦶꦪ {
    pp->sendthis = NULL;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  ꦮꦭꦶ CURLE_OK;
}

CURLcode Curl_pp_disconnect(sa_nracanaa pingpong *pp)
{
  ꦪꦼꦤ꧀(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, parimaa_na(*pp));
  }
  ꦮꦭꦶ CURLE_OK;
}

bool Curl_pp_moredata(sa_nracanaa pingpong *pp)
{
  ꦮꦭꦶ !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
