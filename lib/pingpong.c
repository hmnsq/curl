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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "pingpong.h"

#ifdef USE_PINGPONG

#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "progress.h"

timediff_t Curl_pp_state_timeleft_ms(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = তথ্য->set.server_response_timeout ?
    তথ্য->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(তথ্য);
  যদি(xfer_remain_ms)
    ফেরত CURLMIN(remain_ms, xfer_remain_ms);
  ফেরত remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(কাঠামো Curl_easy *তথ্য,
                           কাঠামো pingpong *pp, bool block,
                           bool disconnecting)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  পূর্ণসংখ্যা rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(তথ্য, pp);
  CURLcode ফলাফল = CURLE_OK;

  যদি(timeout_ms <= 0) {
    failf(তথ্য, "server response timeout");
    ফেরত CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  যদি(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    যদি(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  অন্যথা {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(তথ্য, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  যদি(Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET))
    rc = 1;
  অন্যথা যদি(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  অন্যথা যদি(!pp->sendleft && Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET))
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
    ফলাফল = Curl_pgrsCheck(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(rc == -1) {
    failf(তথ্য, "select/poll error");
    ফলাফল = CURLE_OUT_OF_MEMORY;
  }
  অন্যথা যদি(rc)
    ফলাফল = pp->statemachine(তথ্য, তথ্য->conn);
  অন্যথা যদি(disconnecting)
    ফেরত CURLE_OPERATION_TIMEDOUT;

  ফেরত ফলাফল;
}

/* initialize stuff to prepare for reading a fresh new response */
শূন্য Curl_pp_init(কাঠামো pingpong *pp, ধ্রুবক কাঠামো curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(কাঠামো Curl_easy *তথ্য,
                        কাঠামো pingpong *pp,
                        ধ্রুবক অক্ষর *prefix,
                        ধ্রুবক অক্ষর *fmt,
                        va_list args)
{
  size_t bytes_written = 0;
  size_t write_len;
  অক্ষর *s;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  যদি(!conn)
    /* cannot send without a connection! */
    ফেরত CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  যদি(prefix) {
    ফলাফল = curlx_dyn_add(&pp->sendbuf, prefix);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* append CRLF */
  ফলাফল = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  ফলাফল = Curl_conn_send(তথ্য, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  যদি(ফলাফল == CURLE_AGAIN) {
    bytes_written = 0;
  }
  অন্যথা যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_debug(তথ্য, CURLINFO_HEADER_OUT, s, bytes_written);

  যদি(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  অন্যথা {
    pp->sendthis = ফাঁকা;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(তথ্য);
  }

  ফেরত CURLE_OK;
}

CURLcode Curl_pp_sendf(কাঠামো Curl_easy *তথ্য, কাঠামো pingpong *pp,
                       ধ্রুবক অক্ষর *fmt, ...)
{
  CURLcode ফলাফল;
  va_list ap;
  va_start(ap, fmt);

  ফলাফল = Curl_pp_vsendf(তথ্য, pp, ফাঁকা, fmt, ap);

  va_end(ap);

  ফেরত ফলাফল;
}

স্থির CURLcode pingpong_read(কাঠামো Curl_easy *তথ্য,
                              int8_t sockindex,
                              অক্ষর *বাফার,
                              size_t buflen,
                              size_t *nread)
{
  ফেরত Curl_conn_recv(তথ্য, sockindex, বাফার, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(কাঠামো Curl_easy *তথ্য,
                          int8_t sockindex,
                          কাঠামো pingpong *pp,
                          পূর্ণসংখ্যা *code, /* return the server code if done */
                          size_t *আকার_2) /* size of the response */
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;
  size_t gotbytes;
  অক্ষর বাফার[900];
  পূর্ণসংখ্যা maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *আকার_2 = 0;

  করো {
    gotbytes = 0;
    যদি(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_লেজ(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    যদি(!pp->overflow) {
      যদি(!maxloops--) /* don't get stuck */
        ফেরত CURLE_OK;

      ফলাফল = pingpong_read(তথ্য, sockindex, বাফার, আকার(বাফার),
                             &gotbytes);
      যদি(ফলাফল == CURLE_AGAIN)
        ফেরত CURLE_OK;

      যদি(ফলাফল)
        ফেরত ফলাফল;

      যদি(!gotbytes) {
        failf(তথ্য, "response reading failed (errno: %d)", SOCKERRNO);
        ফেরত CURLE_RECV_ERROR;
      }

      ফলাফল = curlx_dyn_addn(&pp->recvbuf, বাফার, gotbytes);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      তথ্য->req.headerbytecount += (unsigned পূর্ণসংখ্যা)gotbytes;

      pp->nread_resp += gotbytes;
    }

    করো {
      ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&pp->recvbuf);
      ধ্রুবক অক্ষর *nl = memchr(সারি, '\n', curlx_dyn_len(&pp->recvbuf));
      যদি(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t দৈর্ঘ্য = nl - সারি + 1;

        যদি(memchr(সারি, 0, দৈর্ঘ্য)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(তথ্য, "NUL byte in server response line");
          ফেরত CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(তথ্য, CURLINFO_HEADER_IN, সারি, দৈর্ঘ্য);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_INFO, সারি, দৈর্ঘ্য);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        যদি(pp->endofresp(তথ্য, conn, সারি, দৈর্ঘ্য, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = দৈর্ঘ্য;
          যদি(curlx_dyn_len(&pp->recvbuf) > দৈর্ঘ্য)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - দৈর্ঘ্য;
          অন্যথা
            pp->overflow = 0;
          *আকার_2 = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          বিরতি;
        }
        যদি(curlx_dyn_len(&pp->recvbuf) > দৈর্ঘ্য)
          /* keep the remaining piece */
          curlx_dyn_লেজ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - দৈর্ঘ্য);
        অন্যথা
          curlx_dyn_reset(&pp->recvbuf);
      }
      অন্যথা {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        বিরতি;
      }

    } যখন(1); /* while there is buffer left to scan */

  } যখন(gotbytes == আকার(বাফার));

  pp->pending_resp = FALSE;

  ফেরত ফলাফল;
}

CURLcode Curl_pp_pollset(কাঠামো Curl_easy *তথ্য,
                         কাঠামো pingpong *pp,
                         কাঠামো easy_pollset *ps)
{
  পূর্ণসংখ্যা flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  ফেরত Curl_pollset_change(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(কাঠামো Curl_easy *তথ্য,
                         কাঠামো pingpong *pp)
{
  (শূন্য)তথ্য;
  ফেরত pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(কাঠামো Curl_easy *তথ্য,
                           কাঠামো pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode ফলাফল;

  যদি(!Curl_pp_needs_flush(তথ্য, pp))
    ফেরত CURLE_OK;

  ফলাফল = Curl_conn_send(তথ্য, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  যদি(ফলাফল == CURLE_AGAIN) {
    ফলাফল = CURLE_OK;
    written = 0;
  }
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  অন্যথা {
    pp->sendthis = ফাঁকা;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(তথ্য);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_pp_disconnect(কাঠামো pingpong *pp)
{
  যদি(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, আকার(*pp));
  }
  ফেরত CURLE_OK;
}

bool Curl_pp_moredata(কাঠামো pingpong *pp)
{
  ফেরত !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#endif
