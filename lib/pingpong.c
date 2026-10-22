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

#جے_تعریف_شدہ USE_PINGPONG

#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "multiif.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "progress.h"

timediff_t Curl_pp_state_timeleft_ms(بنية Curl_easy *انکڑے,
                                     بنية pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = انکڑے->سیٹ.server_response_timeout ?
    انکڑے->سیٹ.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(انکڑے);
  إذا(xfer_remain_ms)
    أرجع CURLMIN(remain_ms, xfer_remain_ms);
  أرجع remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(بنية Curl_easy *انکڑے,
                           بنية pingpong *pp, bool بلاک,
                           bool disconnecting)
{
  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(انکڑے, pp);
  CURLcode نتیجہ = CURLE_OK;

  إذا(timeout_ms <= 0) {
    failf(انکڑے, "server response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  إذا(بلاک) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    إذا(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  وإلا {
    interval_ms = 0; /*فوری*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(انکڑے, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  إذا(Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET))
    rc = 1;
  وإلا إذا(pp->حد_توں_ودھ)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  وإلا إذا(!pp->sendleft && Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  وإلا {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  إذا(بلاک) {
    /* if we did not wait, we do not have to spend time on this now */
    نتیجہ = Curl_pgrsCheck(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(rc == -1) {
    failf(انکڑے, "select/poll error");
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
  وإلا إذا(rc)
    نتیجہ = pp->statemachine(انکڑے, انکڑے->conn);
  وإلا إذا(disconnecting)
    أرجع CURLE_OPERATION_TIMEDOUT;

  أرجع نتیجہ;
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

CURLcode Curl_pp_vsendf(بنية Curl_easy *انکڑے,
                        بنية pingpong *pp,
                        ثابت حرف *اگیتر,
                        ثابت حرف *fmt,
                        va_list دلیلاں)
{
  size_t بائٹاں_written = 0;
  size_t write_len;
  حرف *s;
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  إذا(!conn)
    /* cannot send without a connection! */
    أرجع CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  إذا(اگیتر) {
    نتیجہ = curlx_dyn_جوڑو(&pp->sendbuf, اگیتر);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_vaddf(&pp->sendbuf, fmt, دلیلاں);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* append CRLF */
  نتیجہ = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  إذا(نتیجہ)
    أرجع نتیجہ;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  نتیجہ = Curl_conn_send(انکڑے, FIRSTSOCKET, s, write_len, FALSE,
                          &بائٹاں_written);
  إذا(نتیجہ == CURLE_AGAIN) {
    بائٹاں_written = 0;
  }
  وإلا إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, s, بائٹاں_written);

  إذا(بائٹاں_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - بائٹاں_written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(انکڑے);
  }

  أرجع CURLE_OK;
}

CURLcode Curl_pp_sendf(بنية Curl_easy *انکڑے, بنية pingpong *pp,
                       ثابت حرف *fmt, ...)
{
  CURLcode نتیجہ;
  va_list ap;
  va_start(ap, fmt);

  نتیجہ = Curl_pp_vsendf(انکڑے, pp, عدم, fmt, ap);

  va_end(ap);

  أرجع نتیجہ;
}

ساكن CURLcode pingpong_read(بنية Curl_easy *انکڑے,
                              int8_t sockindex,
                              حرف *بفر,
                              size_t buflen,
                              size_t *nread)
{
  أرجع Curl_conn_recv(انکڑے, sockindex, بفر, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(بنية Curl_easy *انکڑے,
                          int8_t sockindex,
                          بنية pingpong *pp,
                          صحيح *کوڈ, /* return the server code if done */
                          size_t *ناپ) /* size of the response */
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;
  size_t gotbytes;
  حرف بفر[900];
  صحيح maxloops = 10;

  *کوڈ = 0; /* 0 for errors or not done */
  *ناپ = 0;

  افعل {
    gotbytes = 0;
    إذا(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t بھریا = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_پوچھ(&pp->recvbuf, بھریا - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    إذا(!pp->حد_توں_ودھ) {
      إذا(!maxloops--) /* don't get stuck */
        أرجع CURLE_OK;

      نتیجہ = pingpong_read(انکڑے, sockindex, بفر, حجم(بفر),
                             &gotbytes);
      إذا(نتیجہ == CURLE_AGAIN)
        أرجع CURLE_OK;

      إذا(نتیجہ)
        أرجع نتیجہ;

      إذا(!gotbytes) {
        failf(انکڑے, "response reading failed (errno: %d)", SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }

      نتیجہ = curlx_dyn_addn(&pp->recvbuf, بفر, gotbytes);
      إذا(نتیجہ)
        أرجع نتیجہ;

      انکڑے->req.headerbytecount += (بنانشانی صحيح)gotbytes;

      pp->nread_resp += gotbytes;
    }

    افعل {
      ثابت حرف *سطر = curlx_dyn_ptr(&pp->recvbuf);
      ثابت حرف *nl = memchr(سطر, '\n', curlx_dyn_len(&pp->recvbuf));
      إذا(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t لمائی = nl - سطر + 1;

        إذا(memchr(سطر, 0, لمائی)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(انکڑے, "NUL byte in server response line");
          أرجع CURLE_WEIRD_SERVER_جواب;
        }

        /* output debug output if that is requested */
        Curl_debug(انکڑے, CURLINFO_سرخی_وچ, سطر, لمائی);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_INFO, سطر, لمائی);
        إذا(نتیجہ)
          أرجع نتیجہ;

        إذا(pp->endofresp(انکڑے, conn, سطر, لمائی, کوڈ)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = لمائی;
          إذا(curlx_dyn_len(&pp->recvbuf) > لمائی)
            pp->حد_توں_ودھ = curlx_dyn_len(&pp->recvbuf) - لمائی;
          وإلا
            pp->حد_توں_ودھ = 0;
          *ناپ = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          اكسر;
        }
        إذا(curlx_dyn_len(&pp->recvbuf) > لمائی)
          /* keep the remaining piece */
          curlx_dyn_پوچھ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - لمائی);
        وإلا
          curlx_dyn_reset(&pp->recvbuf);
      }
      وإلا {
        /* without a newline, there is no overflow */
        pp->حد_توں_ودھ = 0;
        اكسر;
      }

    } طالما(1); /* while there is buffer left to scan */

  } طالما(gotbytes == حجم(بفر));

  pp->pending_resp = FALSE;

  أرجع نتیجہ;
}

CURLcode Curl_pp_pollset(بنية Curl_easy *انکڑے,
                         بنية pingpong *pp,
                         بنية easy_pollset *ps)
{
  صحيح flags = pp->sendleft ? CURL_POLL_باہرلا : CURL_POLL_وچ;
  أرجع Curl_pollset_change(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(بنية Curl_easy *انکڑے,
                         بنية pingpong *pp)
{
  (فراغ)انکڑے;
  أرجع pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(بنية Curl_easy *انکڑے,
                           بنية pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode نتیجہ;

  إذا(!Curl_pp_needs_flush(انکڑے, pp))
    أرجع CURLE_OK;

  نتیجہ = Curl_conn_send(انکڑے, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  إذا(نتیجہ == CURLE_AGAIN) {
    نتیجہ = CURLE_OK;
    written = 0;
  }
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(انکڑے);
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

#جے_انت
