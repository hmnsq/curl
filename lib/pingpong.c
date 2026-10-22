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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "pingpong.h"

#اگر_تعریف_شدہ USE_PINGPONG

#شامل "cfilters.h"
#شامل "connect.h"
#شامل "multiif.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "select.h"
#شامل "progress.h"

timediff_t Curl_pp_state_timeleft_ms(ساخت Curl_easy *ڈیٹا,
                                     ساخت pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = ڈیٹا->سیٹ.server_response_timeout ?
    ڈیٹا->سیٹ.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(ڈیٹا);
  اگر(xfer_remain_ms)
    واپس CURLMIN(remain_ms, xfer_remain_ms);
  واپس remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(ساخت Curl_easy *ڈیٹا,
                           ساخت pingpong *pp, bool بلاک,
                           bool disconnecting)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحیح rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(ڈیٹا, pp);
  CURLcode نتیجہ = CURLE_OK;

  اگر(timeout_ms <= 0) {
    failf(ڈیٹا, "server response timeout");
    واپس CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  اگر(بلاک) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    اگر(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  ورنہ {
    interval_ms = 0; /*فوری*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(ڈیٹا, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  اگر(Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET))
    rc = 1;
  ورنہ اگر(pp->حد_سے_تجاوز)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  ورنہ اگر(!pp->sendleft && Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  ورنہ {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  اگر(بلاک) {
    /* if we did not wait, we do not have to spend time on this now */
    نتیجہ = Curl_pgrsCheck(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(rc == -1) {
    failf(ڈیٹا, "select/poll error");
    نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
  ورنہ اگر(rc)
    نتیجہ = pp->statemachine(ڈیٹا, ڈیٹا->conn);
  ورنہ اگر(disconnecting)
    واپس CURLE_OPERATION_TIMEDOUT;

  واپس نتیجہ;
}

/* initialize stuff to prepare for reading a fresh new response */
خالی Curl_pp_init(ساخت pingpong *pp, ثابت ساخت curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(ساخت Curl_easy *ڈیٹا,
                        ساخت pingpong *pp,
                        ثابت حرف *سابقہ,
                        ثابت حرف *fmt,
                        va_list دلائل)
{
  size_t بائٹیں_written = 0;
  size_t write_len;
  حرف *s;
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  اگر(!conn)
    /* cannot send without a connection! */
    واپس CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  اگر(سابقہ) {
    نتیجہ = curlx_dyn_جوڑو(&pp->sendbuf, سابقہ);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_vaddf(&pp->sendbuf, fmt, دلائل);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* append CRLF */
  نتیجہ = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  اگر(نتیجہ)
    واپس نتیجہ;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  نتیجہ = Curl_conn_send(ڈیٹا, FIRSTSOCKET, s, write_len, FALSE,
                          &بائٹیں_written);
  اگر(نتیجہ == CURLE_AGAIN) {
    بائٹیں_written = 0;
  }
  ورنہ اگر(نتیجہ)
    واپس نتیجہ;

  Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, s, بائٹیں_written);

  اگر(بائٹیں_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - بائٹیں_written;
  }
  ورنہ {
    pp->sendthis = تہی;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ڈیٹا);
  }

  واپس CURLE_OK;
}

CURLcode Curl_pp_sendf(ساخت Curl_easy *ڈیٹا, ساخت pingpong *pp,
                       ثابت حرف *fmt, ...)
{
  CURLcode نتیجہ;
  va_list ap;
  va_start(ap, fmt);

  نتیجہ = Curl_pp_vsendf(ڈیٹا, pp, تہی, fmt, ap);

  va_end(ap);

  واپس نتیجہ;
}

ساکن CURLcode pingpong_read(ساخت Curl_easy *ڈیٹا,
                              int8_t sockindex,
                              حرف *بفر,
                              size_t buflen,
                              size_t *nread)
{
  واپس Curl_conn_recv(ڈیٹا, sockindex, بفر, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ساخت Curl_easy *ڈیٹا,
                          int8_t sockindex,
                          ساخت pingpong *pp,
                          صحیح *رمز, /* return the server code if done */
                          size_t *سائز) /* size of the response */
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;
  size_t gotbytes;
  حرف بفر[900];
  صحیح maxloops = 10;

  *رمز = 0; /* 0 for errors or not done */
  *سائز = 0;

  کرو {
    gotbytes = 0;
    اگر(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t بھرا = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_دم(&pp->recvbuf, بھرا - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    اگر(!pp->حد_سے_تجاوز) {
      اگر(!maxloops--) /* don't get stuck */
        واپس CURLE_OK;

      نتیجہ = pingpong_read(ڈیٹا, sockindex, بفر, حجم(بفر),
                             &gotbytes);
      اگر(نتیجہ == CURLE_AGAIN)
        واپس CURLE_OK;

      اگر(نتیجہ)
        واپس نتیجہ;

      اگر(!gotbytes) {
        failf(ڈیٹا, "response reading failed (errno: %d)", SOCKERRNO);
        واپس CURLE_RECV_ERROR;
      }

      نتیجہ = curlx_dyn_addn(&pp->recvbuf, بفر, gotbytes);
      اگر(نتیجہ)
        واپس نتیجہ;

      ڈیٹا->req.headerbytecount += (غیرعلامتی صحیح)gotbytes;

      pp->nread_resp += gotbytes;
    }

    کرو {
      ثابت حرف *سطر = curlx_dyn_ptr(&pp->recvbuf);
      ثابت حرف *nl = memchr(سطر, '\n', curlx_dyn_len(&pp->recvbuf));
      اگر(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t لمبائی = nl - سطر + 1;

        اگر(memchr(سطر, 0, لمبائی)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(ڈیٹا, "NUL byte in server response line");
          واپس CURLE_WEIRD_SERVER_جواب;
        }

        /* output debug output if that is requested */
        Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, سطر, لمبائی);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_INFO, سطر, لمبائی);
        اگر(نتیجہ)
          واپس نتیجہ;

        اگر(pp->endofresp(ڈیٹا, conn, سطر, لمبائی, رمز)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = لمبائی;
          اگر(curlx_dyn_len(&pp->recvbuf) > لمبائی)
            pp->حد_سے_تجاوز = curlx_dyn_len(&pp->recvbuf) - لمبائی;
          ورنہ
            pp->حد_سے_تجاوز = 0;
          *سائز = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          توڑو;
        }
        اگر(curlx_dyn_len(&pp->recvbuf) > لمبائی)
          /* keep the remaining piece */
          curlx_dyn_دم((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - لمبائی);
        ورنہ
          curlx_dyn_reset(&pp->recvbuf);
      }
      ورنہ {
        /* without a newline, there is no overflow */
        pp->حد_سے_تجاوز = 0;
        توڑو;
      }

    } جب(1); /* while there is buffer left to scan */

  } جب(gotbytes == حجم(بفر));

  pp->pending_resp = FALSE;

  واپس نتیجہ;
}

CURLcode Curl_pp_pollset(ساخت Curl_easy *ڈیٹا,
                         ساخت pingpong *pp,
                         ساخت easy_pollset *ps)
{
  صحیح flags = pp->sendleft ? CURL_POLL_برآمد : CURL_POLL_اندر;
  واپس Curl_pollset_change(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(ساخت Curl_easy *ڈیٹا,
                         ساخت pingpong *pp)
{
  (خالی)ڈیٹا;
  واپس pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(ساخت Curl_easy *ڈیٹا,
                           ساخت pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode نتیجہ;

  اگر(!Curl_pp_needs_flush(ڈیٹا, pp))
    واپس CURLE_OK;

  نتیجہ = Curl_conn_send(ڈیٹا, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  اگر(نتیجہ == CURLE_AGAIN) {
    نتیجہ = CURLE_OK;
    written = 0;
  }
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  ورنہ {
    pp->sendthis = تہی;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(ڈیٹا);
  }
  واپس CURLE_OK;
}

CURLcode Curl_pp_disconnect(ساخت pingpong *pp)
{
  اگر(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, حجم(*pp));
  }
  واپس CURLE_OK;
}

bool Curl_pp_moredata(ساخت pingpong *pp)
{
  واپس !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#اگر_ختم
