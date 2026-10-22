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
 ***************************************************************************/
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#समावेश <signal.h>

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_SELECT_H
#समावेश <sys/select.h>
#या_अगर defined(HAVE_UNISTD_H)
#समावेश <unistd.h>
#पूर्ण_अगर

#अगर_अघोषित HAVE_SOCKET
#त्रुटि "We cannot compile without socket() support"
#पूर्ण_अगर

#समावेश "urldata.h"

#समावेश "cfilters.h"
#समावेश "cw-out.h"
#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "http.h"
#समावेश "url.h"
#समावेश "getinfo.h"
#समावेश "multiif.h"
#समावेश "connect.h"
#समावेश "hsts.h"
#समावेश "setopt.h"
#समावेश "headers.h"
#समावेश "bufref.h"
#समावेश "rtsp.h"

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
अक्षर *Curl_checkheaders(स्थिर काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *thisheader,
                        स्थिर माप_प्रकार thislen)
{
  काष्ठा curl_slist *शीर्ष;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  क्रम(शीर्ष = तथ्य->set.headers; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    अगर(curl_strnequal(शीर्ष->तथ्य, thisheader, thislen) &&
       Curl_headersep(शीर्ष->तथ्य[thislen]))
      वापस शीर्ष->तथ्य;
  }

  वापस शून्य;
}
#पूर्ण_अगर

अटल पूर्णांक तथ्य_pending(काष्ठा Curl_easy *तथ्य, bool rcvd_eagain)
{
  काष्ठा connectdata *conn = तथ्य->conn;

  अगर(conn->scheme->protocol & PROTO_FAMILY_FTP)
    वापस Curl_conn_तथ्य_pending(तथ्य, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  वापस (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(काष्ठा Curl_easy *तथ्य, समय_प्रकार timeofdoc)
{
  अगर((timeofdoc == 0) || (तथ्य->set.timevalue == 0))
    वापस TRUE;

  चयन(तथ्य->set.timecondition) {
  हाल CURL_TIMECOND_IFMODSINCE:
  शेष:
    अगर(timeofdoc <= तथ्य->set.timevalue) {
      infof(तथ्य, "The requested document is not new enough");
      तथ्य->info.timecond = TRUE;
      वापस FALSE;
    }
    अवरोध;
  हाल CURL_TIMECOND_IFUNMODSINCE:
    अगर(timeofdoc >= तथ्य->set.timevalue) {
      infof(तथ्य, "The requested document is not old enough");
      तथ्य->info.timecond = TRUE;
      वापस FALSE;
    }
    अवरोध;
  }

  वापस TRUE;
}

अटल CURLcode xfer_recv_shutdown(काष्ठा Curl_easy *तथ्य, bool *done)
{
  अगर(!तथ्य || !तथ्य->conn)
    वापस CURLE_FAILED_INIT;
  वापस Curl_cshutdn_try_once_idx(तथ्य, तथ्य->conn->recv_idx, done);
}

अटल bool xfer_recv_shutdown_started(काष्ठा Curl_easy *तथ्य)
{
  अगर(!तथ्य || !तथ्य->conn)
    वापस FALSE;
  वापस CURL_CONN_IN_SHUTDOWN(तथ्य->conn, तथ्य->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(काष्ठा Curl_easy *तथ्य, bool *done)
{
  अगर(!तथ्य || !तथ्य->conn)
    वापस CURLE_FAILED_INIT;
  वापस Curl_cshutdn_try_once_idx(तथ्य, तथ्य->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
अटल CURLcode xfer_recv_resp(काष्ठा Curl_easy *तथ्य,
                               अक्षर *buf, माप_प्रकार blen,
                               bool eos_reliable,
                               माप_प्रकार *pnread)
{
  CURLcode परिणाम;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  अगर(!eos_reliable && !तथ्य->req.header && तथ्य->req.आकार != -1) {
    blen = curlx_sotouz_range(तथ्य->req.आकार - तथ्य->req.bytecount, 0, blen);
  }
  अन्यथा अगर(xfer_recv_shutdown_started(तथ्य)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  अगर(blen) {
    परिणाम = Curl_xfer_recv(तथ्य, buf, blen, pnread);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(*pnread == 0) {
    अगर(तथ्य->req.shutdown) {
      bool done;
      परिणाम = xfer_recv_shutdown(तथ्य, &done);
      अगर(परिणाम)
        वापस परिणाम;
      अगर(!done) {
        वापस CURLE_AGAIN;
      }
    }
    DEBUGF(infof(तथ्य, "sendrecv_dl: we are done"));
  }
  वापस CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
अटल CURLcode sendrecv_dl(काष्ठा Curl_easy *तथ्य,
                            काष्ठा SingleRequest *k)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;
  अक्षर *buf, *xfer_buf;
  माप_प्रकार blen, xfer_blen;
  पूर्णांक maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  परिणाम = Curl_multi_xfer_buf_borrow(तथ्य, &xfer_buf, &xfer_blen);
  अगर(परिणाम)
    जाओ out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  करो {
    माप_प्रकार bytestoread;

    अगर(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    अगर(bytestoread && Curl_rlimit_active(&तथ्य->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&तथ्य->progress.dl.rlimit, शून्य);
#अगर 0
      DEBUGF(infof(तथ्य, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#पूर्ण_अगर
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      अगर(dl_avail <= 0) {
        rate_limited = TRUE;
        अवरोध;
      }
      अगर(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (माप_प्रकार)dl_avail;
    }

    rcvd_eagain = FALSE;
    परिणाम = xfer_recv_resp(तथ्य, buf, bytestoread, is_multiplex, &blen);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN)
        जाओ out; /* real error */
      rcvd_eagain = TRUE;
      परिणाम = CURLE_OK;
      अगर(तथ्य->req.download_done && तथ्य->req.no_body &&
         !तथ्य->req.resp_trailer) {
        DEBUGF(infof(तथ्य, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      अन्यथा
        अवरोध; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    अगर(!blen) {
      परिणाम = Curl_req_stop_send_recv(तथ्य);
      अगर(परिणाम)
        जाओ out;
      अगर(k->eos_written) /* already did write this to client, leave */
        अवरोध;
    }

    परिणाम = Curl_xfer_write_resp(तथ्य, buf, blen, is_eos);
    अगर(परिणाम || तथ्य->req.done)
      जाओ out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    अगर((!is_multiplex && तथ्य->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(तथ्य);
    }
    /* if we stopped receiving, leave the loop */
    अगर(!CURL_REQ_WANT_RECV(तथ्य))
      अवरोध;

  } जबतक(maxloops--);

  अगर(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(तथ्य) &&
     (!rcvd_eagain || तथ्य_pending(तथ्य, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(तथ्य);
    CURL_TRC_M(तथ्य, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  अगर(!CURL_REQ_WANT_RECV(तथ्य) && CURL_REQ_WANT_SEND(तथ्य) &&
     (conn->bits.बंद || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(तथ्य, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(तथ्य);
  }

out:
  Curl_multi_xfer_buf_release(तथ्य, xfer_buf);
  अगर(परिणाम)
    DEBUGF(infof(तथ्य, "sendrecv_dl() -> %d", (पूर्णांक)परिणाम));
  वापस परिणाम;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
अटल CURLcode sendrecv_ul(काष्ठा Curl_easy *तथ्य)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(तथ्य));

  अगर(!Curl_req_done_sending(तथ्य))
    वापस Curl_req_send_more(तथ्य);
  वापस CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  स्थिर काष्ठा curltime *pnow = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(Curl_xfer_is_blocked(तथ्य)) {
    परिणाम = CURLE_OK;
    जाओ out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  अगर(CURL_REQ_WANT_RECV(तथ्य)) {
    परिणाम = sendrecv_dl(तथ्य, k);
    अगर(परिणाम || तथ्य->req.done)
      जाओ out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  अगर(Curl_req_want_send(तथ्य)) {
    परिणाम = sendrecv_ul(तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  pnow = Curl_pgrs_now(तथ्य);
  अगर(CURL_REQ_WANT_IO(तथ्य)) {
    अगर(Curl_timeleft_now_ms(तथ्य, pnow) < 0) {
      अगर(k->आकार != -1) {
        failf(तथ्य, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE),
              k->bytecount, k->आकार);
      }
      अन्यथा {
        failf(तथ्य, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE),
              k->bytecount);
      }
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      जाओ out;
    }
  }
  अन्यथा {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    अगर(!तथ्य->req.no_body && (k->आकार != -1) &&
       (k->bytecount != k->आकार) && !k->newurl) {
      failf(तथ्य, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->आकार - k->bytecount);
      परिणाम = CURLE_PARTIAL_FILE;
      जाओ out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  अगर(!CURL_REQ_WANT_IO(तथ्य))
    तथ्य->req.done = TRUE;

  परिणाम = Curl_pgrsCheckX(तथ्य, pnow);

out:
  अगर(परिणाम)
    DEBUGF(infof(तथ्य, "Curl_sendrecv() -> %d", (पूर्णांक)परिणाम));
  वापस परिणाम;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
व्योम Curl_init_CONNECT(काष्ठा Curl_easy *तथ्य)
{
  तथ्य->state.fread_func = तथ्य->set.fread_func_set;
  तथ्य->state.in = तथ्य->set.in_set;
  तथ्य->state.upload = (तथ्य->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  तथ्य->state.retrycount = 0;

  अगर(!CURL_EASY_STR(तथ्य, STRING_SET_URL) && !तथ्य->set.uh) {
    /* we cannot do anything without URL */
    failf(तथ्य, "No URL set");
    वापस CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  अगर(तथ्य->set.uh) {
    अक्षर *url = शून्य;
    CURLUcode uc;
    uc = curl_url_get(तथ्य->set.uh, CURLUPART_URL, &url, 0);
    अगर(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&तथ्य->state.url, शून्य, 0, शून्य);
      failf(तथ्य, "No URL set");
      वापस CURLE_URL_MALFORMAT;
    }
    परिणाम = CURL_EASY_STR_SETN(तथ्य, STRING_SET_URL, url);
    अगर(परिणाम)
      वापस परिणाम;
  }

  Curl_bufref_set(&तथ्य->state.url, CURL_EASY_STR(तथ्य, STRING_SET_URL),
                  0, शून्य);

  अगर(तथ्य->set.postfields && तथ्य->set.set_resume_from) {
    /* we cannot */
    failf(तथ्य, "cannot mix POSTFIELDS with RESUME_FROM");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  तथ्य->state.prefer_ascii = तथ्य->set.prefer_ascii;
#अगर_घोषित CURL_LIST_ONLY_PROTOCOL
  तथ्य->state.list_only = तथ्य->set.list_only;
#पूर्ण_अगर
  तथ्य->state.httpreq = तथ्य->set.method;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&तथ्य->state.initial_origin);
  Curl_peer_unlink(&तथ्य->state.origin);
  तथ्य->state.requests = 0;
  तथ्य->state.followlocation = 0; /* reset the location-follow counter */
  तथ्य->state.this_is_a_follow = FALSE; /* reset this */
  तथ्य->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  तथ्य->state.errorbuf = FALSE; /* no error has occurred */
#अगर_अघोषित CURL_DISABLE_HTTP
  Curl_http_neg_init(तथ्य, &तथ्य->state.http_neg);
#पूर्ण_अगर
  तथ्य->state.authproblem = FALSE;
  तथ्य->state.authhost.want = तथ्य->set.httpauth;
  तथ्य->state.authproxy.want = तथ्य->set.proxyauth;
  curlx_safefree(तथ्य->info.wouldredirect);
  curlx_safefree(तथ्य->info.effective_url);
  Curl_तथ्य_priority_clear_state(तथ्य);
  अगर(तथ्य->set.http_auto_referer)
    Curl_bufref_free(&तथ्य->state.referer);
  अगर(CURL_EASY_STR(तथ्य, STRING_SET_REFERER))
    Curl_bufref_set(&तथ्य->state.referer,
                    CURL_EASY_STR(तथ्य, STRING_SET_REFERER), 0, शून्य);
  अन्यथा
    Curl_bufref_free(&तथ्य->state.referer);

  अगर(तथ्य->state.httpreq == HTTPREQ_PUT)
    तथ्य->state.infilesize = तथ्य->set.filesize;
  अन्यथा अगर((तथ्य->state.httpreq != HTTPREQ_GET) &&
          (तथ्य->state.httpreq != HTTPREQ_शीर्ष)) {
    तथ्य->state.infilesize = तथ्य->set.postfieldsize;
    अगर(तथ्य->set.postfields && (तथ्य->state.infilesize == -1))
      तथ्य->state.infilesize = (curl_off_t)म_माप(तथ्य->set.postfields);
  }
  अन्यथा
    तथ्य->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  परिणाम = Curl_cookie_loadfiles(तथ्य,
                                 तथ्य->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  अगर(!परिणाम)
    Curl_cookie_run(तथ्य); /* activate */

  /* If there is a list of host pairs to deal with */
  अगर(!परिणाम && तथ्य->state.resolve)
    परिणाम = Curl_loadhostpairs(तथ्य);

  अगर(!परिणाम)
    /* If there is a list of hsts files to read */
    परिणाम = Curl_hsts_loadfiles(तथ्य);

  अगर(!परिणाम) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    तथ्य->state.allow_port = TRUE;

#अगर defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    अगर(!तथ्य->set.no_signal)
      तथ्य->state.पूर्व_signal = संकेत(SIGPIPE, संक_छोड़ो);
#पूर्ण_अगर

    Curl_initinfo(तथ्य); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(तथ्य);
    Curl_pgrsStart(तथ्य, शून्य);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    तथ्य->state.authhost.picked &= तथ्य->state.authhost.want;
    तथ्य->state.authproxy.picked &= तथ्य->state.authproxy.want;

#अगर_अघोषित CURL_DISABLE_FTP
    तथ्य->state.wildcardmatch = तथ्य->set.wildcard_enabled;
#अगर_घोषित DEBUGBUILD
  अगर(दो_पर्या("CURL_DBG_FTP_WILDCARD"))
    तथ्य->state.wildcardmatch = TRUE;
#पूर्ण_अगर
    अगर(तथ्य->state.wildcardmatch) {
      काष्ठा Wildcard_तथ्य *wc;
      अगर(!तथ्य->wildcard) {
        तथ्य->wildcard = curlx_calloc(1, माप(काष्ठा Wildcard_तथ्य));
        अगर(!तथ्य->wildcard)
          वापस CURLE_OUT_OF_MEMORY;
      }
      wc = तथ्य->wildcard;
      अगर(wc->state < CURLWC_INIT) {
        अगर(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->pattern);
        curlx_safefree(wc->path);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#पूर्ण_अगर
    परिणाम = Curl_hsts_loadcb(तथ्य, तथ्य->hsts);
  }

  तथ्य->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  तथ्य->req.rewind_read = FALSE;
  Curl_client_reset(तथ्य);
  Curl_headers_cleanup(तथ्य);
  वापस परिणाम;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(काष्ठा Curl_easy *तथ्य, अक्षर **url)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  bool retry = FALSE;
  *url = शून्य;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  अगर(तथ्य->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    वापस CURLE_OK;

  अगर(conn->bits.reuse &&
     (तथ्य->req.bytecount + तथ्य->req.headerbytecount == 0) &&
     ((!तथ्य->req.no_body && !तथ्य->req.done) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#अगर_अघोषित CURL_DISABLE_RTSP
     && (तथ्य->set.rtspreq != RTSPREQ_RECEIVE)
#पूर्ण_अगर
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  अन्यथा अगर(तथ्य->state.refused_stream &&
          (तथ्य->req.bytecount + तथ्य->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(तथ्य, "REFUSED_STREAM, retrying a fresh connect");
    तथ्य->state.refused_stream = FALSE; /* clear again */
    retry = TRUE;
  }
  अगर(retry) {
#घोषणा CONN_MAX_RETRIES 5
    अगर(तथ्य->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(तथ्य, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      तथ्य->state.retrycount = 0;
      वापस CURLE_SEND_ERROR;
    }
    infof(तथ्य, "Connection died, retrying a fresh connect (retry count: %d)",
          तथ्य->state.retrycount);
    *url = Curl_bufref_dup(&तथ्य->state.url);
    अगर(!*url)
      वापस CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(तथ्य, TRUE);
  }
  वापस CURLE_OK;
}

अटल व्योम xfer_setup(
  काष्ठा Curl_easy *तथ्य,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_आकार      /* how much to receive, -1 if unknown */
  )
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  काष्ठा connectdata *conn = तथ्य->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(तथ्य));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_आकार == -1));
  k->आकार = recv_आकार;
  k->header = !!conn->scheme->run->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  अगर(!k->header && (recv_आकार > 0))
    Curl_pgrs_Set_Download_आकार(तथ्य, recv_आकार);

  /* we want header and/or body, if neither then do not do this! */
  अगर(conn->scheme->run->write_resp_hd || !तथ्य->req.no_body) {
    अगर(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(तथ्य);
    अगर(conn->send_idx != -1)
      CURL_REQ_SET_SEND(तथ्य);
  }
  CURL_TRC_M(तथ्य, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

व्योम Curl_xfer_setup_nop(काष्ठा Curl_easy *तथ्य)
{
  xfer_setup(तथ्य, -1, -1, -1);
}

व्योम Curl_xfer_setup_sendrecv(काष्ठा Curl_easy *तथ्य,
                              int8_t sockindex,
                              curl_off_t recv_आकार)
{
  xfer_setup(तथ्य, sockindex, sockindex, recv_आकार);
}

व्योम Curl_xfer_setup_send(काष्ठा Curl_easy *तथ्य,
                          int8_t sockindex)
{
  xfer_setup(तथ्य, sockindex, -1, -1);
}

व्योम Curl_xfer_setup_recv(काष्ठा Curl_easy *तथ्य,
                          int8_t sockindex,
                          curl_off_t recv_आकार)
{
  xfer_setup(तथ्य, -1, sockindex, recv_आकार);
}

व्योम Curl_xfer_set_shutdown(काष्ठा Curl_easy *तथ्य,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (तथ्य->conn->send_idx < 0) || (तथ्य->conn->recv_idx < 0));
  तथ्य->req.shutdown = shutdown;
  तथ्य->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *buf, माप_प्रकार blen,
                              bool is_eos)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(तथ्य->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    परिणाम = तथ्य->conn->scheme->run->write_resp(तथ्य, buf, blen, is_eos);
  }
  अन्यथा {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    अगर(blen || is_eos) {
      पूर्णांक cwtype = CLIENTWRITE_BODY;
      अगर(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      परिणाम = Curl_client_write(तथ्य, cwtype, buf, blen);
    }
  }

  अगर(!परिणाम && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    तथ्य->req.eos_written = TRUE;
    तथ्य->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(तथ्य, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

bool Curl_xfer_write_is_paused(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_cwriter_is_paused(तथ्य);
}

CURLcode Curl_xfer_write_resp_hd(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *hd0, माप_प्रकार hdlen, bool is_eos)
{
  अगर(तथ्य->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    वापस तथ्य->conn->scheme->run->write_resp_hd(तथ्य, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  वापस Curl_xfer_write_resp(तथ्य, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(काष्ठा Curl_easy *तथ्य, bool premature)
{
  (व्योम)premature;
  वापस Curl_cw_out_done(तथ्य);
}

bool Curl_xfer_needs_flush(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_conn_needs_flush(तथ्य, तथ्य->conn->send_idx);
}

CURLcode Curl_xfer_flush(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_conn_flush(तथ्य, तथ्य->conn->send_idx);
}

CURLcode Curl_xfer_send(काष्ठा Curl_easy *तथ्य,
                        स्थिर व्योम *buf, माप_प्रकार blen, bool eos,
                        माप_प्रकार *pnwritten)
{
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);

  परिणाम = Curl_conn_send(तथ्य, तथ्य->conn->send_idx,
                          buf, blen, eos, pnwritten);
  अगर(परिणाम == CURLE_AGAIN) {
    परिणाम = CURLE_OK;
    *pnwritten = 0;
  }
  अन्यथा अगर(!परिणाम && *pnwritten)
    तथ्य->info.request_आकार += *pnwritten;

  DEBUGF(infof(तथ्य, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (पूर्णांक)परिणाम, *pnwritten));
  वापस परिणाम;
}

CURLcode Curl_xfer_recv(काष्ठा Curl_easy *तथ्य,
                        अक्षर *buf, माप_प्रकार blen,
                        माप_प्रकार *pnrcvd)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  DEBUGASSERT(तथ्य->set.बफर_आकार > 0);

  अगर(curlx_uitouz(तथ्य->set.बफर_आकार) < blen)
    blen = curlx_uitouz(तथ्य->set.बफर_आकार);
  वापस Curl_conn_recv(तथ्य, तथ्य->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(काष्ठा Curl_easy *तथ्य)
{
  Curl_conn_ev_तथ्य_done_send(तथ्य);
  वापस CURLE_OK;
}

bool Curl_xfer_is_blocked(काष्ठा Curl_easy *तथ्य)
{
  bool want_send = CURL_REQ_WANT_SEND(तथ्य);
  bool want_recv = CURL_REQ_WANT_RECV(तथ्य);
  अगर(!want_send)
    वापस want_recv && Curl_xfer_recv_is_paused(तथ्य);
  अन्यथा अगर(!want_recv)
    वापस want_send && Curl_xfer_send_is_paused(तथ्य);
  अन्यथा
    वापस Curl_xfer_recv_is_paused(तथ्य) && Curl_xfer_send_is_paused(तथ्य);
}

bool Curl_xfer_send_is_paused(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_rlimit_is_blocked(&तथ्य->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_rlimit_is_blocked(&तथ्य->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(काष्ठा Curl_easy *तथ्य, bool enable)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_rlimit_block(&तथ्य->progress.ul.rlimit, enable, Curl_pgrs_now(तथ्य));
  अगर(!enable && Curl_creader_is_paused(तथ्य))
    परिणाम = Curl_creader_unpause(तथ्य);
  Curl_pgrsSendPause(तथ्य, enable);
  वापस परिणाम;
}

CURLcode Curl_xfer_pause_recv(काष्ठा Curl_easy *तथ्य, bool enable)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_rlimit_block(&तथ्य->progress.dl.rlimit, enable, Curl_pgrs_now(तथ्य));
  अगर(!enable && Curl_cwriter_is_paused(तथ्य))
    परिणाम = Curl_cwriter_unpause(तथ्य);
  Curl_conn_ev_तथ्य_pause(तथ्य, enable);
  Curl_pgrsRecvPause(तथ्य, enable);
  वापस परिणाम;
}

bool Curl_xfer_is_secure(काष्ठा Curl_easy *तथ्य)
{
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(तथ्य->conn && तथ्य->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(तथ्य->state.origin->scheme->flags & PROTOPT_SSL));
    वापस FALSE;
  }
#पूर्ण_अगर
  वापस (तथ्य->state.origin->scheme->flags & PROTOPT_SSL);
}
