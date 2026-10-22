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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "pingpong.h"

#अगर_घोषित USE_PINGPONG

#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "multiif.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "select.h"
#समावेश "progress.h"

timediff_t Curl_pp_state_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = तथ्य->set.server_response_timeout ?
    तथ्य->set.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(तथ्य);
  अगर(xfer_remain_ms)
    वापस CURLMIN(remain_ms, xfer_remain_ms);
  वापस remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(काष्ठा Curl_easy *तथ्य,
                           काष्ठा pingpong *pp, bool block,
                           bool disconnecting)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णांक rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(तथ्य, pp);
  CURLcode परिणाम = CURLE_OK;

  अगर(timeout_ms <= 0) {
    failf(तथ्य, "server response timeout");
    वापस CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  अगर(block) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    अगर(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  अन्यथा {
    interval_ms = 0; /* immediate */
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(तथ्य, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  अगर(Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET))
    rc = 1;
  अन्यथा अगर(pp->overflow)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  अन्यथा अगर(!pp->sendleft && Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  अन्यथा {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  अगर(block) {
    /* if we did not wait, we do not have to spend time on this now */
    परिणाम = Curl_pgrsCheck(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(rc == -1) {
    failf(तथ्य, "select/poll error");
    परिणाम = CURLE_OUT_OF_MEMORY;
  }
  अन्यथा अगर(rc)
    परिणाम = pp->statemachine(तथ्य, तथ्य->conn);
  अन्यथा अगर(disconnecting)
    वापस CURLE_OPERATION_TIMEDOUT;

  वापस परिणाम;
}

/* initialize stuff to prepare for reading a fresh new response */
व्योम Curl_pp_init(काष्ठा pingpong *pp, स्थिर काष्ठा curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(काष्ठा Curl_easy *तथ्य,
                        काष्ठा pingpong *pp,
                        स्थिर अक्षर *prefix,
                        स्थिर अक्षर *fmt,
                        बहु_सूची args)
{
  माप_प्रकार bytes_written = 0;
  माप_प्रकार write_len;
  अक्षर *s;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  अगर(!conn)
    /* cannot send without a connection! */
    वापस CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  अगर(prefix) {
    परिणाम = curlx_dyn_add(&pp->sendbuf, prefix);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  अगर(!परिणाम)
    परिणाम = curlx_dyn_vaddf(&pp->sendbuf, fmt, args);
  अगर(परिणाम)
    वापस परिणाम;

  /* append CRLF */
  परिणाम = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  अगर(परिणाम)
    वापस परिणाम;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  परिणाम = Curl_conn_send(तथ्य, FIRSTSOCKET, s, write_len, FALSE,
                          &bytes_written);
  अगर(परिणाम == CURLE_AGAIN) {
    bytes_written = 0;
  }
  अन्यथा अगर(परिणाम)
    वापस परिणाम;

  Curl_debug(तथ्य, CURLINFO_HEADER_OUT, s, bytes_written);

  अगर(bytes_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - bytes_written;
  }
  अन्यथा {
    pp->sendthis = शून्य;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(तथ्य);
  }

  वापस CURLE_OK;
}

CURLcode Curl_pp_sendf(काष्ठा Curl_easy *तथ्य, काष्ठा pingpong *pp,
                       स्थिर अक्षर *fmt, ...)
{
  CURLcode परिणाम;
  बहु_सूची ap;
  बहु_शुरू(ap, fmt);

  परिणाम = Curl_pp_vsendf(तथ्य, pp, शून्य, fmt, ap);

  बहु_पूर्ण(ap);

  वापस परिणाम;
}

अटल CURLcode pingpong_read(काष्ठा Curl_easy *तथ्य,
                              int8_t sockindex,
                              अक्षर *बफर,
                              माप_प्रकार buflen,
                              माप_प्रकार *nread)
{
  वापस Curl_conn_recv(तथ्य, sockindex, बफर, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(काष्ठा Curl_easy *तथ्य,
                          int8_t sockindex,
                          काष्ठा pingpong *pp,
                          पूर्णांक *code, /* return the server code if done */
                          माप_प्रकार *आकार) /* size of the response */
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार gotbytes;
  अक्षर बफर[900];
  पूर्णांक maxloops = 10;

  *code = 0; /* 0 for errors or not done */
  *आकार = 0;

  करो {
    gotbytes = 0;
    अगर(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      माप_प्रकार full = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_पुच्छ(&pp->recvbuf, full - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    अगर(!pp->overflow) {
      अगर(!maxloops--) /* don't get stuck */
        वापस CURLE_OK;

      परिणाम = pingpong_read(तथ्य, sockindex, बफर, माप(बफर),
                             &gotbytes);
      अगर(परिणाम == CURLE_AGAIN)
        वापस CURLE_OK;

      अगर(परिणाम)
        वापस परिणाम;

      अगर(!gotbytes) {
        failf(तथ्य, "response reading failed (errno: %d)", SOCKERRNO);
        वापस CURLE_RECV_ERROR;
      }

      परिणाम = curlx_dyn_addn(&pp->recvbuf, बफर, gotbytes);
      अगर(परिणाम)
        वापस परिणाम;

      तथ्य->req.headerbytecount += (अचिन्हित पूर्णांक)gotbytes;

      pp->nread_resp += gotbytes;
    }

    करो {
      स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&pp->recvbuf);
      स्थिर अक्षर *nl = स_प्रथम(पंक्ति, '\n', curlx_dyn_len(&pp->recvbuf));
      अगर(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        माप_प्रकार लंबाई = nl - पंक्ति + 1;

        अगर(स_प्रथम(पंक्ति, 0, लंबाई)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(तथ्य, "NUL byte in server response line");
          वापस CURLE_WEIRD_SERVER_REPLY;
        }

        /* output debug output if that is requested */
        Curl_debug(तथ्य, CURLINFO_HEADER_IN, पंक्ति, लंबाई);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_INFO, पंक्ति, लंबाई);
        अगर(परिणाम)
          वापस परिणाम;

        अगर(pp->endofresp(तथ्य, conn, पंक्ति, लंबाई, code)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = लंबाई;
          अगर(curlx_dyn_len(&pp->recvbuf) > लंबाई)
            pp->overflow = curlx_dyn_len(&pp->recvbuf) - लंबाई;
          अन्यथा
            pp->overflow = 0;
          *आकार = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          अवरोध;
        }
        अगर(curlx_dyn_len(&pp->recvbuf) > लंबाई)
          /* keep the remaining piece */
          curlx_dyn_पुच्छ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - लंबाई);
        अन्यथा
          curlx_dyn_reset(&pp->recvbuf);
      }
      अन्यथा {
        /* without a newline, there is no overflow */
        pp->overflow = 0;
        अवरोध;
      }

    } जबतक(1); /* while there is buffer left to scan */

  } जबतक(gotbytes == माप(बफर));

  pp->pending_resp = FALSE;

  वापस परिणाम;
}

CURLcode Curl_pp_pollset(काष्ठा Curl_easy *तथ्य,
                         काष्ठा pingpong *pp,
                         काष्ठा easy_pollset *ps)
{
  पूर्णांक flags = pp->sendleft ? CURL_POLL_OUT : CURL_POLL_IN;
  वापस Curl_pollset_change(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(काष्ठा Curl_easy *तथ्य,
                         काष्ठा pingpong *pp)
{
  (व्योम)तथ्य;
  वापस pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(काष्ठा Curl_easy *तथ्य,
                           काष्ठा pingpong *pp)
{
  /* we have a piece of a command still left to send */
  माप_प्रकार written;
  CURLcode परिणाम;

  अगर(!Curl_pp_needs_flush(तथ्य, pp))
    वापस CURLE_OK;

  परिणाम = Curl_conn_send(तथ्य, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  अगर(परिणाम == CURLE_AGAIN) {
    परिणाम = CURLE_OK;
    written = 0;
  }
  अगर(परिणाम)
    वापस परिणाम;

  अगर(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  अन्यथा {
    pp->sendthis = शून्य;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(तथ्य);
  }
  वापस CURLE_OK;
}

CURLcode Curl_pp_disconnect(काष्ठा pingpong *pp)
{
  अगर(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    स_रखो(pp, 0, माप(*pp));
  }
  वापस CURLE_OK;
}

bool Curl_pp_moredata(काष्ठा pingpong *pp)
{
  वापस !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#पूर्ण_अगर
