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

#جيڪڏهن_وصف_ٿيل USE_PINGPONG

#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "multiif.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "progress.h"

timediff_t Curl_pp_state_timeleft_ms(بنية Curl_easy *معلومات,
                                     بنية pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = معلومات->سيٽ.server_response_timeout ?
    معلومات->سيٽ.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(معلومات);
  إذا(xfer_remain_ms)
    أرجع CURLMIN(remain_ms, xfer_remain_ms);
  أرجع remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(بنية Curl_easy *معلومات,
                           بنية pingpong *pp, bool بلاڪ,
                           bool disconnecting)
{
  بنية connectdata *conn = معلومات->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(معلومات, pp);
  CURLcode نتيجو = CURLE_OK;

  إذا(timeout_ms <= 0) {
    failf(معلومات, "server response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  إذا(بلاڪ) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    إذا(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  وإلا {
    interval_ms = 0; /*فوري*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(معلومات, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  إذا(Curl_conn_معلومات_pending(معلومات, FIRSTSOCKET))
    rc = 1;
  وإلا إذا(pp->حد_کان_مٿي)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  وإلا إذا(!pp->sendleft && Curl_conn_معلومات_pending(معلومات, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  وإلا {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  إذا(بلاڪ) {
    /* if we did not wait, we do not have to spend time on this now */
    نتيجو = Curl_pgrsCheck(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(rc == -1) {
    failf(معلومات, "select/poll error");
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
  وإلا إذا(rc)
    نتيجو = pp->statemachine(معلومات, معلومات->conn);
  وإلا إذا(disconnecting)
    أرجع CURLE_OPERATION_TIMEDOUT;

  أرجع نتيجو;
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

CURLcode Curl_pp_vsendf(بنية Curl_easy *معلومات,
                        بنية pingpong *pp,
                        ثابت حرف *اڳياڙي,
                        ثابت حرف *fmt,
                        va_list دليلون)
{
  size_t بائيٽون_written = 0;
  size_t write_len;
  حرف *s;
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  إذا(!conn)
    /* cannot send without a connection! */
    أرجع CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  إذا(اڳياڙي) {
    نتيجو = curlx_dyn_جوڙيو(&pp->sendbuf, اڳياڙي);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  إذا(!نتيجو)
    نتيجو = curlx_dyn_vaddf(&pp->sendbuf, fmt, دليلون);
  إذا(نتيجو)
    أرجع نتيجو;

  /* append CRLF */
  نتيجو = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  إذا(نتيجو)
    أرجع نتيجو;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  نتيجو = Curl_conn_send(معلومات, FIRSTSOCKET, s, write_len, FALSE,
                          &بائيٽون_written);
  إذا(نتيجو == CURLE_AGAIN) {
    بائيٽون_written = 0;
  }
  وإلا إذا(نتيجو)
    أرجع نتيجو;

  Curl_debug(معلومات, CURLINFO_سرو_ٻاهريون, s, بائيٽون_written);

  إذا(بائيٽون_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - بائيٽون_written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(معلومات);
  }

  أرجع CURLE_OK;
}

CURLcode Curl_pp_sendf(بنية Curl_easy *معلومات, بنية pingpong *pp,
                       ثابت حرف *fmt, ...)
{
  CURLcode نتيجو;
  va_list ap;
  va_start(ap, fmt);

  نتيجو = Curl_pp_vsendf(معلومات, pp, عدم, fmt, ap);

  va_end(ap);

  أرجع نتيجو;
}

ساكن CURLcode pingpong_read(بنية Curl_easy *معلومات,
                              int8_t sockindex,
                              حرف *بفر,
                              size_t buflen,
                              size_t *nread)
{
  أرجع Curl_conn_recv(معلومات, sockindex, بفر, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(بنية Curl_easy *معلومات,
                          int8_t sockindex,
                          بنية pingpong *pp,
                          صحيح *ڪوڊ, /* return the server code if done */
                          size_t *سائيز) /* size of the response */
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو = CURLE_OK;
  size_t gotbytes;
  حرف بفر[900];
  صحيح maxloops = 10;

  *ڪوڊ = 0; /* 0 for errors or not done */
  *سائيز = 0;

  افعل {
    gotbytes = 0;
    إذا(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t ڀريل = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_پڇ(&pp->recvbuf, ڀريل - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    إذا(!pp->حد_کان_مٿي) {
      إذا(!maxloops--) /* don't get stuck */
        أرجع CURLE_OK;

      نتيجو = pingpong_read(معلومات, sockindex, بفر, حجم(بفر),
                             &gotbytes);
      إذا(نتيجو == CURLE_AGAIN)
        أرجع CURLE_OK;

      إذا(نتيجو)
        أرجع نتيجو;

      إذا(!gotbytes) {
        failf(معلومات, "response reading failed (errno: %d)", SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }

      نتيجو = curlx_dyn_addn(&pp->recvbuf, بفر, gotbytes);
      إذا(نتيجو)
        أرجع نتيجو;

      معلومات->req.headerbytecount += (بنانشان صحيح)gotbytes;

      pp->nread_resp += gotbytes;
    }

    افعل {
      ثابت حرف *سٽ = curlx_dyn_ptr(&pp->recvbuf);
      ثابت حرف *nl = memchr(سٽ, '\n', curlx_dyn_len(&pp->recvbuf));
      إذا(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t ڊيگهائي = nl - سٽ + 1;

        إذا(memchr(سٽ, 0, ڊيگهائي)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(معلومات, "NUL byte in server response line");
          أرجع CURLE_WEIRD_SERVER_جواب;
        }

        /* output debug output if that is requested */
        Curl_debug(معلومات, CURLINFO_سرو_اندريون, سٽ, ڊيگهائي);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        نتيجو = Curl_client_write(معلومات, CLIENTWRITE_INFO, سٽ, ڊيگهائي);
        إذا(نتيجو)
          أرجع نتيجو;

        إذا(pp->endofresp(معلومات, conn, سٽ, ڊيگهائي, ڪوڊ)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = ڊيگهائي;
          إذا(curlx_dyn_len(&pp->recvbuf) > ڊيگهائي)
            pp->حد_کان_مٿي = curlx_dyn_len(&pp->recvbuf) - ڊيگهائي;
          وإلا
            pp->حد_کان_مٿي = 0;
          *سائيز = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          اكسر;
        }
        إذا(curlx_dyn_len(&pp->recvbuf) > ڊيگهائي)
          /* keep the remaining piece */
          curlx_dyn_پڇ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - ڊيگهائي);
        وإلا
          curlx_dyn_reset(&pp->recvbuf);
      }
      وإلا {
        /* without a newline, there is no overflow */
        pp->حد_کان_مٿي = 0;
        اكسر;
      }

    } طالما(1); /* while there is buffer left to scan */

  } طالما(gotbytes == حجم(بفر));

  pp->pending_resp = FALSE;

  أرجع نتيجو;
}

CURLcode Curl_pp_pollset(بنية Curl_easy *معلومات,
                         بنية pingpong *pp,
                         بنية easy_pollset *ps)
{
  صحيح flags = pp->sendleft ? CURL_POLL_ٻاهريون : CURL_POLL_اندريون;
  أرجع Curl_pollset_change(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(بنية Curl_easy *معلومات,
                         بنية pingpong *pp)
{
  (فراغ)معلومات;
  أرجع pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(بنية Curl_easy *معلومات,
                           بنية pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode نتيجو;

  إذا(!Curl_pp_needs_flush(معلومات, pp))
    أرجع CURLE_OK;

  نتيجو = Curl_conn_send(معلومات, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  إذا(نتيجو == CURLE_AGAIN) {
    نتيجو = CURLE_OK;
    written = 0;
  }
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  وإلا {
    pp->sendthis = عدم;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(معلومات);
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

#جيڪڏهن_پورو
