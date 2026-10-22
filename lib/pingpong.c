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

timediff_t Curl_pp_state_timeleft_ms(struct Curl_easy *data,
                                     struct pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = data->set.server_response_timeout ?
    data->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(data);
  만약(xfer_remain_ms)
    반환 CURLMIN(remain_ms, xfer_remain_ms);
  반환 remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(struct Curl_easy *data,
                           struct pingpong *pp, bool block,
                           bool disconnecting)
{
  struct connectdata *conn = data->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  정수 rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(data, pp);
  CURLcode result = CURLE_OK;

  만약(timeout_ms <= 0) {
    failf(data, "server response timeout");
    반환 CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  만약(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    만약(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  아니면 {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(data, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  만약(Curl_conn_data_pending(data, FIRSTSOCKET))
    rc = 1;
  아니면 만약(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  아니면 만약(!pp->sendleft && Curl_conn_data_pending(data, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  아니면 {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  만약(block) {
    /* if we did not wait, we do not have to spend time on this now */
    result = Curl_pgrsCheck(data);
    만약(result)
      반환 result;
  }

  만약(rc == -1) {
    failf(data, "select/poll error");
    result = CURLE_OUT_OF_MEMORY;
  }
  아니면 만약(rc)
    result = pp->statemachine(data, data->conn);
  아니면 만약(disconnecting)
    반환 CURLE_OPERATION_TIMEDOUT;

  반환 result;
}

/* initialize stuff to prepare for reading a fresh new response */
void Curl_pp_init(struct pingpong *pp, const struct curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(struct Curl_easy *data,
                        struct pingpong *pp,
                        const char *prefix,
                        const char *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  char *s;
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  만약(!conn)
    /* cannot send without a connection! */
    반환 CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  만약(prefix) {
    result = curlx_dyn_add(&pp->sendbuf, prefix);
    만약(!result)
      result = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  만약(!result)
    result = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  만약(result)
    반환 result;

  /* append CRLF */
  result = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  만약(result)
    반환 result;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  result = Curl_conn_send(data, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  만약(result == CURLE_AGAIN) {
    bytes_written = 0;
  }
  아니면 만약(result)
    반환 result;

  Curl_debug(data, CURLINFO_HEADER_OUT, s, bytes_written);

  만약(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  아니면 {
    pp->sendthis = NULL;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }

  반환 CURLE_OK;
}

CURLcode Curl_pp_sendf(struct Curl_easy *data, struct pingpong *pp,
                       const char *fmt, ...)
{
  CURLcode result;
  va_list ap;
  va_start(ap, fmt);

  result = Curl_pp_vsendf(data, pp, NULL, fmt, ap);

  va_end(ap);

  반환 result;
}

static CURLcode pingpong_read(struct Curl_easy *data,
                              int8_t sockindex,
                              char *buffer,
                              size_t buflen,
                              size_t *nread)
{
  반환 Curl_conn_recv(data, sockindex, buffer, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(struct Curl_easy *data,
                          int8_t sockindex,
                          struct pingpong *pp,
                          정수 *code, /* return the server code if done */
                          size_t *size) /* size of the response */
{
  struct connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  size_t gotbytes;
  char buffer[900];
  정수 maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *size = 0;

  do {
    gotbytes = 0;
    만약(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_tail(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    만약(!pp->overflow) {
      만약(!maxloops--) /* don't get stuck */
        반환 CURLE_OK;

      result = pingpong_read(data, sockindex, buffer, sizeof(buffer),
                             &gotbytes);
      만약(result == CURLE_AGAIN)
        반환 CURLE_OK;

      만약(result)
        반환 result;

      만약(!gotbytes) {
        failf(data, "response reading failed (errno: %d)", SOCKERRNO);
        반환 CURLE_RECV_ERROR;
      }

      result = curlx_dyn_addn(&pp->recvbuf, buffer, gotbytes);
      만약(result)
        반환 result;

      data->req.headerbytecount += (unsigned 정수)gotbytes;

      pp->nread_resp += gotbytes;
    }

    do {
      const char *line = curlx_dyn_ptr(&pp->recvbuf);
      const char *nl = memchr(line, '\n', curlx_dyn_len(&pp->recvbuf));
      만약(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t length = nl - line + 1;

        만약(memchr(line, 0, length)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(data, "NUL byte in server response line");
          반환 CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(data, CURLINFO_HEADER_IN, line, length);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        result = Curl_client_write(data, CLIENTWRITE_INFO, line, length);
        만약(result)
          반환 result;

        만약(pp->endofresp(data, conn, line, length, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = length;
          만약(curlx_dyn_len(&pp->recvbuf) > length)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - length;
          아니면
            pp->overflow = 0;
          *size = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          break;
        }
        만약(curlx_dyn_len(&pp->recvbuf) > length)
          /* keep the remaining piece */
          curlx_dyn_tail((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - length);
        아니면
          curlx_dyn_reset(&pp->recvbuf);
      }
      아니면 {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        break;
      }

    } 동안(1); /* while there is buffer left to scan */

  } 동안(gotbytes == sizeof(buffer));

  pp->pending_resp = FALSE;

  반환 result;
}

CURLcode Curl_pp_pollset(struct Curl_easy *data,
                         struct pingpong *pp,
                         struct easy_pollset *ps)
{
  정수 flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  반환 Curl_pollset_change(data, ps, data->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(struct Curl_easy *data,
                         struct pingpong *pp)
{
  (void)data;
  반환 pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(struct Curl_easy *data,
                           struct pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode result;

  만약(!Curl_pp_needs_flush(data, pp))
    반환 CURLE_OK;

  result = Curl_conn_send(data, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  만약(result == CURLE_AGAIN) {
    result = CURLE_OK;
    written = 0;
  }
  만약(result)
    반환 result;

  만약(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  아니면 {
    pp->sendthis = NULL;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(data);
  }
  반환 CURLE_OK;
}

CURLcode Curl_pp_disconnect(struct pingpong *pp)
{
  만약(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, sizeof(*pp));
  }
  반환 CURLE_OK;
}

bool Curl_pp_moredata(struct pingpong *pp)
{
  반환 !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
