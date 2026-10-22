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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "pingpong.h"

#నిర్వచితమైతే USE_PINGPONG

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "progress.h"

timediff_t Curl_pp_state_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = దత్తాంశం->సమితి.server_response_timeout ?
    దత్తాంశం->సమితి.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(దత్తాంశం);
  ఒకవేళ(xfer_remain_ms)
    తిరిగి CURLMIN(remain_ms, xfer_remain_ms);
  తిరిగి remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం pingpong *pp, bool ఖండం,
                           bool disconnecting)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  పూర్ణాంకం rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(దత్తాంశం, pp);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(timeout_ms <= 0) {
    failf(దత్తాంశం, "server response timeout");
    తిరిగి CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  ఒకవేళ(ఖండం) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    ఒకవేళ(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  లేకపోతే {
    interval_ms = 0; /*తక్షణ*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(దత్తాంశం, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  ఒకవేళ(Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET))
    rc = 1;
  లేకపోతే ఒకవేళ(pp->అధికప్రవాహం)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  లేకపోతే ఒకవేళ(!pp->sendleft && Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  లేకపోతే {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  ఒకవేళ(ఖండం) {
    /* if we did not wait, we do not have to spend time on this now */
    ఫలితం = Curl_pgrsCheck(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(rc == -1) {
    failf(దత్తాంశం, "select/poll error");
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే ఒకవేళ(rc)
    ఫలితం = pp->statemachine(దత్తాంశం, దత్తాంశం->conn);
  లేకపోతే ఒకవేళ(disconnecting)
    తిరిగి CURLE_OPERATION_TIMEDOUT;

  తిరిగి ఫలితం;
}

/* initialize stuff to prepare for reading a fresh new response */
శూన్య Curl_pp_init(నిర్మాణం pingpong *pp, స్థిరరాశి నిర్మాణం curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం pingpong *pp,
                        స్థిరరాశి అక్షరం *ఉపసర్గ,
                        స్థిరరాశి అక్షరం *fmt,
                        va_list వాదనలు)
{
  size_t బైట్లు_written = 0;
  size_t write_len;
  అక్షరం *s;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  ఒకవేళ(!conn)
    /* cannot send without a connection! */
    తిరిగి CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  ఒకవేళ(ఉపసర్గ) {
    ఫలితం = curlx_dyn_కూడు(&pp->sendbuf, ఉపసర్గ);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_vaddf(&pp->sendbuf, fmt, వాదనలు);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* append CRLF */
  ఫలితం = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  ఫలితం = Curl_conn_send(దత్తాంశం, FIRSTSOCKET, s, write_len, FALSE,
                          &బైట్లు_written);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    బైట్లు_written = 0;
  }
  లేకపోతే ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, s, బైట్లు_written);

  ఒకవేళ(బైట్లు_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - బైట్లు_written;
  }
  లేకపోతే {
    pp->sendthis = ఖాళీ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(దత్తాంశం);
  }

  తిరిగి CURLE_OK;
}

CURLcode Curl_pp_sendf(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం pingpong *pp,
                       స్థిరరాశి అక్షరం *fmt, ...)
{
  CURLcode ఫలితం;
  va_list ap;
  va_start(ap, fmt);

  ఫలితం = Curl_pp_vsendf(దత్తాంశం, pp, ఖాళీ, fmt, ap);

  va_end(ap);

  తిరిగి ఫలితం;
}

స్థిర CURLcode pingpong_read(నిర్మాణం Curl_easy *దత్తాంశం,
                              int8_t sockindex,
                              అక్షరం *బఫరు,
                              size_t buflen,
                              size_t *nread)
{
  తిరిగి Curl_conn_recv(దత్తాంశం, sockindex, బఫరు, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex,
                          నిర్మాణం pingpong *pp,
                          పూర్ణాంకం *సంకేతం_2, /* return the server code if done */
                          size_t *కొలత) /* size of the response */
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;
  size_t gotbytes;
  అక్షరం బఫరు[900];
  పూర్ణాంకం maxloops = 10;

  *సంకేతం_2 = 0; /* 0 for errors or not done */
  *కొలత = 0;

  చేయి {
    gotbytes = 0;
    ఒకవేళ(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t నిండిన = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_తోక(&pp->recvbuf, నిండిన - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    ఒకవేళ(!pp->అధికప్రవాహం) {
      ఒకవేళ(!maxloops--) /* don't get stuck */
        తిరిగి CURLE_OK;

      ఫలితం = pingpong_read(దత్తాంశం, sockindex, బఫరు, పరిమాణం(బఫరు),
                             &gotbytes);
      ఒకవేళ(ఫలితం == CURLE_AGAIN)
        తిరిగి CURLE_OK;

      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఒకవేళ(!gotbytes) {
        failf(దత్తాంశం, "response reading failed (errno: %d)", SOCKERRNO);
        తిరిగి CURLE_RECV_ERROR;
      }

      ఫలితం = curlx_dyn_addn(&pp->recvbuf, బఫరు, gotbytes);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      దత్తాంశం->req.headerbytecount += (గుర్తులేని పూర్ణాంకం)gotbytes;

      pp->nread_resp += gotbytes;
    }

    చేయి {
      స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&pp->recvbuf);
      స్థిరరాశి అక్షరం *nl = memchr(పంక్తి, '\n', curlx_dyn_len(&pp->recvbuf));
      ఒకవేళ(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t నిడివి = nl - పంక్తి + 1;

        ఒకవేళ(memchr(పంక్తి, 0, నిడివి)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(దత్తాంశం, "NUL byte in server response line");
          తిరిగి CURLE_WEIRD_SERVER_జవాబు;
        }

        /* output debug output if that is requested */
        Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, పంక్తి, నిడివి);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_INFO, పంక్తి, నిడివి);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        ఒకవేళ(pp->endofresp(దత్తాంశం, conn, పంక్తి, నిడివి, సంకేతం_2)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = నిడివి;
          ఒకవేళ(curlx_dyn_len(&pp->recvbuf) > నిడివి)
            pp->అధికప్రవాహం = curlx_dyn_len(&pp->recvbuf) - నిడివి;
          లేకపోతే
            pp->అధికప్రవాహం = 0;
          *కొలత = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          ఆపు;
        }
        ఒకవేళ(curlx_dyn_len(&pp->recvbuf) > నిడివి)
          /* keep the remaining piece */
          curlx_dyn_తోక((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - నిడివి);
        లేకపోతే
          curlx_dyn_reset(&pp->recvbuf);
      }
      లేకపోతే {
        /* without a newline, there is no overflow */
        pp->అధికప్రవాహం = 0;
        ఆపు;
      }

    } అయితే(1); /* while there is buffer left to scan */

  } అయితే(gotbytes == పరిమాణం(బఫరు));

  pp->pending_resp = FALSE;

  తిరిగి ఫలితం;
}

CURLcode Curl_pp_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం pingpong *pp,
                         నిర్మాణం easy_pollset *ps)
{
  పూర్ణాంకం flags = pp->sendleft ? CURL_POLL_బహిర్గతం : CURL_POLL_లోపల;
  తిరిగి Curl_pollset_change(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం pingpong *pp)
{
  (శూన్య)దత్తాంశం;
  తిరిగి pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode ఫలితం;

  ఒకవేళ(!Curl_pp_needs_flush(దత్తాంశం, pp))
    తిరిగి CURLE_OK;

  ఫలితం = Curl_conn_send(దత్తాంశం, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    ఫలితం = CURLE_OK;
    written = 0;
  }
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  లేకపోతే {
    pp->sendthis = ఖాళీ;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_pp_disconnect(నిర్మాణం pingpong *pp)
{
  ఒకవేళ(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    memset(pp, 0, పరిమాణం(*pp));
  }
  తిరిగి CURLE_OK;
}

bool Curl_pp_moredata(నిర్మాణం pingpong *pp)
{
  తిరిగి !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#అయితే_ముగింపు
