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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "pingpong.h"

#यदि_परिभाषितम् USE_PINGPONG

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"

timediff_t Curl_pp_state_timeleft_ms(संरचना Curl_easy *दत्त,
                                     संरचना pingpong *pp)
{
  timediff_t xfer_remain_ms;
  timediff_t remain_ms = दत्त->समुच्चयः.server_response_timeout ?
    दत्त->समुच्चयः.server_response_timeout : PINGPONG_TIMEOUT_MS;

  /* If the overall transfer has less time remaining than pingpong
   * has otherwise for the state, return that. */
  remain_ms -= curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &pp->response);
  /* transfer remaining time is 0, when it has no timeout. */
  xfer_remain_ms = Curl_timeleft_ms(दत्त);
  यदि(xfer_remain_ms)
    प्रत्यागम CURLMIN(remain_ms, xfer_remain_ms);
  प्रत्यागम remain_ms;
}

/*
 * Curl_pp_statemach()
 */
CURLcode Curl_pp_statemach(संरचना Curl_easy *दत्त,
                           संरचना pingpong *pp, bool खण्डम्,
                           bool disconnecting)
{
  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क rc;
  timediff_t interval_ms;
  timediff_t timeout_ms = Curl_pp_state_timeleft_ms(दत्त, pp);
  CURLcode परिणाम = CURLE_OK;

  यदि(timeout_ms <= 0) {
    failf(दत्त, "server response timeout");
    प्रत्यागम CURLE_OPERATION_TIMEDOUT; /* already too little time */
  }

  यदि(खण्डम्) {
    interval_ms = 1000;  /* use 1 second timeout intervals */
    यदि(timeout_ms < interval_ms)
      interval_ms = timeout_ms;
  }
  अन्यथा {
    interval_ms = 0; /*तात्कालिकम्*/
    /* Wake the multi socket API even when the server sends no response. */
    Curl_expire(दत्त, timeout_ms, EXPIRE_SERVER_RESPONSE);
  }

  यदि(Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET))
    rc = 1;
  अन्यथा यदि(pp->अतिप्रवाहः)
    /* We are receiving and there is data in the cache so read it */
    rc = 1;
  अन्यथा यदि(!pp->sendleft && Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET))
    /* We are receiving and there is data ready in the SSL library */
    rc = 1;
  अन्यथा {
    rc = Curl_socket_check(pp->sendleft ? CURL_SOCKET_BAD : sock, /* reading */
                           CURL_SOCKET_BAD,
                           pp->sendleft ? sock : CURL_SOCKET_BAD, /* writing */
                           interval_ms);
  }

  यदि(खण्डम्) {
    /* if we did not wait, we do not have to spend time on this now */
    परिणाम = Curl_pgrsCheck(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(rc == -1) {
    failf(दत्त, "select/poll error");
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा यदि(rc)
    परिणाम = pp->statemachine(दत्त, दत्त->conn);
  अन्यथा यदि(disconnecting)
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;

  प्रत्यागम परिणाम;
}

/* initialize stuff to prepare for reading a fresh new response */
शून्य Curl_pp_init(संरचना pingpong *pp, ध्रुव संरचना curltime *pnow)
{
  DEBUGASSERT(!pp->initialized);
  pp->nread_resp = 0;
  pp->response = *pnow; /* start response time-out */
  pp->pending_resp = TRUE;
  curlx_dyn_init(&pp->sendbuf, DYN_PINGPPONG_CMD);
  curlx_dyn_init(&pp->recvbuf, DYN_PINGPPONG_CMD);
  pp->initialized = TRUE;
}

CURLcode Curl_pp_vsendf(संरचना Curl_easy *दत्त,
                        संरचना pingpong *pp,
                        ध्रुव वर्ण *उपसर्गः,
                        ध्रुव वर्ण *fmt,
                        va_list तर्काः)
{
  size_t अष्टकानि_written = 0;
  size_t write_len;
  वर्ण *s;
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  DEBUGASSERT(pp->sendleft == 0);
  DEBUGASSERT(pp->sendsize == 0);
  DEBUGASSERT(!pp->sendthis);

  यदि(!conn)
    /* cannot send without a connection! */
    प्रत्यागम CURLE_SEND_ERROR;

  curlx_dyn_reset(&pp->sendbuf);
  यदि(उपसर्गः) {
    परिणाम = curlx_dyn_सङ्कलय(&pp->sendbuf, उपसर्गः);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&pp->sendbuf, " ", 1);
  }
  यदि(!परिणाम)
    परिणाम = curlx_dyn_vaddf(&pp->sendbuf, fmt, तर्काः);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* append CRLF */
  परिणाम = curlx_dyn_addn(&pp->sendbuf, "\r\n", 2);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  pp->pending_resp = TRUE;
  write_len = curlx_dyn_len(&pp->sendbuf);
  s = curlx_dyn_ptr(&pp->sendbuf);

  परिणाम = Curl_conn_send(दत्त, FIRSTSOCKET, s, write_len, FALSE,
                          &अष्टकानि_written);
  यदि(परिणाम == CURLE_AGAIN) {
    अष्टकानि_written = 0;
  }
  अन्यथा यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, s, अष्टकानि_written);

  यदि(अष्टकानि_written < write_len) {
    /* the whole chunk was not sent, keep it around and adjust sizes */
    pp->sendthis = s;
    pp->sendsize = write_len;
    pp->sendleft = write_len - अष्टकानि_written;
  }
  अन्यथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(दत्त);
  }

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pp_sendf(संरचना Curl_easy *दत्त, संरचना pingpong *pp,
                       ध्रुव वर्ण *fmt, ...)
{
  CURLcode परिणाम;
  va_list ap;
  va_start(ap, fmt);

  परिणाम = Curl_pp_vsendf(दत्त, pp, रिक्त, fmt, ap);

  va_end(ap);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode pingpong_read(संरचना Curl_easy *दत्त,
                              int8_t sockindex,
                              वर्ण *बफर,
                              size_t buflen,
                              size_t *nread)
{
  प्रत्यागम Curl_conn_recv(दत्त, sockindex, बफर, buflen, nread);
}

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(संरचना Curl_easy *दत्त,
                          int8_t sockindex,
                          संरचना pingpong *pp,
                          पूर्णाङ्क *संहिता, /* return the server code if done */
                          size_t *आकार) /* size of the response */
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;
  size_t gotbytes;
  वर्ण बफर[900];
  पूर्णाङ्क maxloops = 10;

  *संहिता = 0; /* 0 for errors or not done */
  *आकार = 0;

  कुरु {
    gotbytes = 0;
    यदि(pp->nfinal) {
      /* a previous call left this many bytes in the beginning of the buffer as
         that was the final line; now ditch that */
      size_t पूर्णम् = curlx_dyn_len(&pp->recvbuf);

      /* trim off the "final" leading part */
      curlx_dyn_पुच्छ(&pp->recvbuf, पूर्णम् - pp->nfinal);

      pp->nfinal = 0; /* now gone */
    }
    यदि(!pp->अतिप्रवाहः) {
      यदि(!maxloops--) /* don't get stuck */
        प्रत्यागम CURLE_OK;

      परिणाम = pingpong_read(दत्त, sockindex, बफर, परिमाण(बफर),
                             &gotbytes);
      यदि(परिणाम == CURLE_AGAIN)
        प्रत्यागम CURLE_OK;

      यदि(परिणाम)
        प्रत्यागम परिणाम;

      यदि(!gotbytes) {
        failf(दत्त, "response reading failed (errno: %d)", SOCKERRNO);
        प्रत्यागम CURLE_RECV_ERROR;
      }

      परिणाम = curlx_dyn_addn(&pp->recvbuf, बफर, gotbytes);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      दत्त->req.headerbytecount += (अचिह्नित पूर्णाङ्क)gotbytes;

      pp->nread_resp += gotbytes;
    }

    कुरु {
      ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&pp->recvbuf);
      ध्रुव वर्ण *nl = memchr(पङ्क्तिः, '\n', curlx_dyn_len(&pp->recvbuf));
      यदि(nl) {
        /* a newline is CRLF in pp-talk, so the CR is ignored as
           the line is not really terminated until the LF comes */
        size_t दीर्घता = nl - पङ्क्तिः + 1;

        यदि(memchr(पङ्क्तिः, 0, दीर्घता)) {
          /* The response line is passed on as a "header" below, so reject an
             embedded NUL byte the same way verify_header() does for HTTP. */
          failf(दत्त, "NUL byte in server response line");
          प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
        }

        /* output debug output if that is requested */
        Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, पङ्क्तिः, दीर्घता);

        /*
         * Pass all response-lines to the callback function registered for
         * "headers". The response lines can be seen as a kind of headers.
         */
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_INFO, पङ्क्तिः, दीर्घता);
        यदि(परिणाम)
          प्रत्यागम परिणाम;

        यदि(pp->endofresp(दत्त, conn, पङ्क्तिः, दीर्घता, संहिता)) {
          /* When at "end of response", keep the endofresp line first in the
             buffer since it will be accessed outside (by pingpong
             parsers). Store the overflow counter to inform about additional
             data in this buffer after the endofresp line. */
          pp->nfinal = दीर्घता;
          यदि(curlx_dyn_len(&pp->recvbuf) > दीर्घता)
            pp->अतिप्रवाहः = curlx_dyn_len(&pp->recvbuf) - दीर्घता;
          अन्यथा
            pp->अतिप्रवाहः = 0;
          *आकार = pp->nread_resp; /* size of the response */
          pp->nread_resp = 0; /* restart */
          gotbytes = 0; /* force break out of outer loop */
          भङ्ग;
        }
        यदि(curlx_dyn_len(&pp->recvbuf) > दीर्घता)
          /* keep the remaining piece */
          curlx_dyn_पुच्छ((&pp->recvbuf), curlx_dyn_len(&pp->recvbuf) - दीर्घता);
        अन्यथा
          curlx_dyn_reset(&pp->recvbuf);
      }
      अन्यथा {
        /* without a newline, there is no overflow */
        pp->अतिप्रवाहः = 0;
        भङ्ग;
      }

    } यावत्(1); /* while there is buffer left to scan */

  } यावत्(gotbytes == परिमाण(बफर));

  pp->pending_resp = FALSE;

  प्रत्यागम परिणाम;
}

CURLcode Curl_pp_pollset(संरचना Curl_easy *दत्त,
                         संरचना pingpong *pp,
                         संरचना easy_pollset *ps)
{
  पूर्णाङ्क flags = pp->sendleft ? CURL_POLL_बहिर्गमनम् : CURL_POLL_अन्तर्;
  प्रत्यागम Curl_pollset_change(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET],
                             flags, 0);
}

bool Curl_pp_needs_flush(संरचना Curl_easy *दत्त,
                         संरचना pingpong *pp)
{
  (शून्य)दत्त;
  प्रत्यागम pp->sendleft > 0;
}

CURLcode Curl_pp_flushsend(संरचना Curl_easy *दत्त,
                           संरचना pingpong *pp)
{
  /* we have a piece of a command still left to send */
  size_t written;
  CURLcode परिणाम;

  यदि(!Curl_pp_needs_flush(दत्त, pp))
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_conn_send(दत्त, FIRSTSOCKET,
                          pp->sendthis + pp->sendsize - pp->sendleft,
                          pp->sendleft, FALSE, &written);
  यदि(परिणाम == CURLE_AGAIN) {
    परिणाम = CURLE_OK;
    written = 0;
  }
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(written < pp->sendleft) {
    /* only a fraction was sent */
    pp->sendleft -= written;
  }
  अन्यथा {
    pp->sendthis = रिक्त;
    pp->sendleft = pp->sendsize = 0;
    pp->response = *Curl_pgrs_now(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pp_disconnect(संरचना pingpong *pp)
{
  यदि(pp->initialized) {
    curlx_dyn_free(&pp->sendbuf);
    curlx_dyn_free(&pp->recvbuf);
    स्मृतिं_पूरय(pp, 0, परिमाण(*pp));
  }
  प्रत्यागम CURLE_OK;
}

bool Curl_pp_moredata(संरचना pingpong *pp)
{
  प्रत्यागम !pp->sendleft && curlx_dyn_len(&pp->recvbuf) > pp->nfinal;
}

#यदि_अन्तः
