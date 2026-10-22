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

#اگر_تعریف_شده USE_PINGPONG

#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "multiif.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "progress.h"

timediff_t Curl_pp_state_timeleft_ms(بنية Curl_easy *داده,
                                     بنية pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = داده->مجموعه.server_response_timeout ?
    داده->مجموعه.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(داده), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(داده);
  إذا(xfer_remain_ms)
    أرجع CURLMIN(remain_ms, xfer_remain_ms);
  أرجع remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(بنية Curl_easy *داده,
                           بنية pingpong *pp, bool بلوک,
                           bool disconnecting)
{
  بنية connectdata *conn = داده->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(داده, pp);
  CURLcode نتیجه = CURLE_OK;

  إذا(timeout_ms <= 0) {
    failf(داده, "server response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  إذا(بلوک) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    إذا(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  وإلا {
    interval_ms = 0; /*فوری*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(داده, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  إذا(Curl_conn_داده_pending(داده, FIRSTSOCKET))
    rc = 1;
  وإلا إذا(pp->سرریز)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  وإلا إذا(!pp->sendleft && Curl_conn_داده_pending(داده, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  وإلا {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  إذا(بلوک) {
    /* if we did not wait, we do not have to spend time on this now */
    نتیجه = Curl_pgrsCheck(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(rc == -1) {
    failf(داده, "select/poll error");
    نتیجه = CURLE_بیرون_از_حافظه;
  }
  وإلا إذا(rc)
    نتیجه = pp->statemachine(داده, داده->conn);
  وإلا إذا(disconnecting)
    أرجع CURLE_OPERATION_TIMEDOUT;

  أرجع نتیجه;
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

CURLcode Curl_pp_vsendf(بنية Curl_easy *داده,
                        بنية pingpong *pp,
                        ثابت حرف *پیشوند,
                        ثابت حرف *fmt,
                        va_list آرگومانها)
{
  size_t بایتها_written = 0;
  size_t write_len;
  حرف *s;
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  إذا(!conn)
    /* cannot send without a connection! */
    أرجع CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  إذا(پیشوند) {
    نتیجه = curlx_dyn_جمع_کن(&pp->sendbuf, پیشوند);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  إذا(!نتیجه)
    نتیجه = curlx_dyn_vaddf(&pp->sendbuf, fmt, آرگومانها);
  إذا(نتیجه)
    أرجع نتیجه;

  /* append CRLF */
  نتیجه = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  إذا(نتیجه)
    أرجع نتیجه;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  نتیجه = Curl_conn_send(داده, FIRSTSOCKET, s, write_len, FALSE,
                          &بایتها_written);
  إذا(نتیجه == CURLE_AGAIN) {
    بایتها_written = 0;
  }
  وإلا إذا(نتیجه)
    أرجع نتیجه;

  Curl_debug(داده, CURLINFO_سرآیند_بیرون, s, بایتها_written);

  إذا(بایتها_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - بایتها_written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(داده);
  }

  أرجع CURLE_OK;
}

CURLcode Curl_pp_sendf(بنية Curl_easy *داده, بنية pingpong *pp,
                       ثابت حرف *fmt, ...)
{
  CURLcode نتیجه;
  va_list ap;
  va_start(ap, fmt);

  نتیجه = Curl_pp_vsendf(داده, pp, عدم, fmt, ap);

  va_end(ap);

  أرجع نتیجه;
}

ساكن CURLcode pingpong_read(بنية Curl_easy *داده,
                              int8_t sockindex,
                              حرف *میانگیر,
                              size_t buflen,
                              size_t *nread)
{
  أرجع Curl_conn_recv(داده, sockindex, میانگیر, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(بنية Curl_easy *داده,
                          int8_t sockindex,
                          بنية pingpong *pp,
                          صحيح *کد, /* return the server code if done */
                          size_t *اندازه) /* size of the response */
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;
  size_t gotbytes;
  حرف میانگیر[900];
  صحيح maxloops = 10;

  *کد = 0; /* 0 for errors or not done */
  *اندازه = 0;

  افعل {
    gotbytes = 0;
    إذا(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t پر = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_دم(&pp->recvbuf, پر - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    إذا(!pp->سرریز) {
      إذا(!maxloops--) /* don't get stuck */
        أرجع CURLE_OK;

      نتیجه = pingpong_read(داده, sockindex, میانگیر, حجم(میانگیر),
                             &gotbytes);
      إذا(نتیجه == CURLE_AGAIN)
        أرجع CURLE_OK;

      إذا(نتیجه)
        أرجع نتیجه;

      إذا(!gotbytes) {
        failf(داده, "response reading failed (errno: %d)", SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }

      نتیجه = curlx_dyn_addn(&pp->recvbuf, میانگیر, gotbytes);
      إذا(نتیجه)
        أرجع نتیجه;

      داده->req.headerbytecount += (بیعلامت صحيح)gotbytes;

      pp->nread_resp += gotbytes;
    }

    افعل {
      ثابت حرف *سطر = curlx_dyn_ptr(&pp->recvbuf);
      ثابت حرف *nl = memchr(سطر, '\n', curlx_dyn_len(&pp->recvbuf));
      إذا(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t درازی = nl - سطر + 1;

        إذا(memchr(سطر, 0, درازی)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(داده, "NUL byte in server response line");
          أرجع CURLE_WEIRD_SERVER_پاسخ;
        }

        /* output debug output if that is requested */
        Curl_debug(داده, CURLINFO_سرآیند_درون, سطر, درازی);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        نتیجه = Curl_client_write(داده, CLIENTWRITE_INFO, سطر, درازی);
        إذا(نتیجه)
          أرجع نتیجه;

        إذا(pp->endofresp(داده, conn, سطر, درازی, کد)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = درازی;
          إذا(curlx_dyn_len(&pp->recvbuf) > درازی)
            pp->سرریز = curlx_dyn_len(&pp->recvbuf) - درازی;
          وإلا
            pp->سرریز = 0;
          *اندازه = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          اكسر;
        }
        إذا(curlx_dyn_len(&pp->recvbuf) > درازی)
          /* keep the remaining piece */
          curlx_dyn_دم((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - درازی);
        وإلا
          curlx_dyn_reset(&pp->recvbuf);
      }
      وإلا {
        /* without a newline, there is no overflow */
        pp->سرریز = 0;
        اكسر;
      }

    } طالما(1); /* while there is buffer left to scan */

  } طالما(gotbytes == حجم(میانگیر));

  pp->pending_resp = FALSE;

  أرجع نتیجه;
}

CURLcode Curl_pp_pollset(بنية Curl_easy *داده,
                         بنية pingpong *pp,
                         بنية easy_pollset *ps)
{
  صحيح flags = pp->sendleft ? CURL_POLL_بیرون : CURL_POLL_درون;
  أرجع Curl_pollset_change(داده, ps, داده->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(بنية Curl_easy *داده,
                         بنية pingpong *pp)
{
  (فراغ)داده;
  أرجع pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(بنية Curl_easy *داده,
                           بنية pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode نتیجه;

  إذا(!Curl_pp_needs_flush(داده, pp))
    أرجع CURLE_OK;

  نتیجه = Curl_conn_send(داده, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  إذا(نتیجه == CURLE_AGAIN) {
    نتیجه = CURLE_OK;
    written = 0;
  }
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(داده);
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

#پایان_اگر
