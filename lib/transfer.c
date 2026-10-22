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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#অন্তর্ভাব <signal.h>

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#অন্তর্ভাব <sys/select.h>
#নতুবা_যদি defined(HAVE_UNISTD_H)
#অন্তর্ভাব <unistd.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#অন্তর্ভাব "urldata.h"

#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cw-out.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "getinfo.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "headers.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "rtsp.h"

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
অক্ষর *Curl_checkheaders(ধ্রুবক কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *thisheader,
                        ধ্রুবক size_t thislen)
{
  কাঠামো curl_slist *শীর্ষ;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  জন্য(শীর্ষ = তথ্য->set.headers; শীর্ষ; শীর্ষ = শীর্ষ->পরবর্তী) {
    যদি(curl_strnequal(শীর্ষ->তথ্য, thisheader, thislen) &&
       Curl_headersep(শীর্ষ->তথ্য[thislen]))
      ফেরত শীর্ষ->তথ্য;
  }

  ফেরত ফাঁকা;
}
#endif

স্থির পূর্ণসংখ্যা তথ্য_pending(কাঠামো Curl_easy *তথ্য, bool rcvd_eagain)
{
  কাঠামো connectdata *conn = তথ্য->conn;

  যদি(conn->scheme->protocol & PROTO_FAMILY_FTP)
    ফেরত Curl_conn_তথ্য_pending(তথ্য, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  ফেরত (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(কাঠামো Curl_easy *তথ্য, time_t timeofdoc)
{
  যদি((timeofdoc == 0) || (তথ্য->set.timevalue == 0))
    ফেরত TRUE;

  নির্বাচন(তথ্য->set.timecondition) {
  ক্ষেত্র CURL_TIMECOND_IFMODSINCE:
  পূর্বনির্ধারিত:
    যদি(timeofdoc <= তথ্য->set.timevalue) {
      infof(তথ্য, "The requested document is not new enough");
      তথ্য->info.timecond = TRUE;
      ফেরত FALSE;
    }
    বিরতি;
  ক্ষেত্র CURL_TIMECOND_IFUNMODSINCE:
    যদি(timeofdoc >= তথ্য->set.timevalue) {
      infof(তথ্য, "The requested document is not old enough");
      তথ্য->info.timecond = TRUE;
      ফেরত FALSE;
    }
    বিরতি;
  }

  ফেরত TRUE;
}

স্থির CURLcode xfer_recv_shutdown(কাঠামো Curl_easy *তথ্য, bool *done)
{
  যদি(!তথ্য || !তথ্য->conn)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_cshutdn_try_once_idx(তথ্য, তথ্য->conn->recv_idx, done);
}

স্থির bool xfer_recv_shutdown_started(কাঠামো Curl_easy *তথ্য)
{
  যদি(!তথ্য || !তথ্য->conn)
    ফেরত FALSE;
  ফেরত CURL_CONN_IN_SHUTDOWN(তথ্য->conn, তথ্য->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(কাঠামো Curl_easy *তথ্য, bool *done)
{
  যদি(!তথ্য || !তথ্য->conn)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_cshutdn_try_once_idx(তথ্য, তথ্য->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
স্থির CURLcode xfer_recv_resp(কাঠামো Curl_easy *তথ্য,
                               অক্ষর *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode ফলাফল;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  যদি(!eos_reliable && !তথ্য->req.header && তথ্য->req.আকার_2 != -1) {
    blen = curlx_sotouz_range(তথ্য->req.আকার_2 - তথ্য->req.bytecount, 0, blen);
  }
  অন্যথা যদি(xfer_recv_shutdown_started(তথ্য)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  যদি(blen) {
    ফলাফল = Curl_xfer_recv(তথ্য, buf, blen, pnread);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(*pnread == 0) {
    যদি(তথ্য->req.shutdown) {
      bool done;
      ফলাফল = xfer_recv_shutdown(তথ্য, &done);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      যদি(!done) {
        ফেরত CURLE_AGAIN;
      }
    }
    DEBUGF(infof(তথ্য, "sendrecv_dl: we are done"));
  }
  ফেরত CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
স্থির CURLcode sendrecv_dl(কাঠামো Curl_easy *তথ্য,
                            কাঠামো SingleRequest *k)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *buf, *xfer_buf;
  size_t blen, xfer_blen;
  পূর্ণসংখ্যা maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  ফলাফল = Curl_multi_xfer_buf_borrow(তথ্য, &xfer_buf, &xfer_blen);
  যদি(ফলাফল)
    যাও out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  করো {
    size_t bytestoread;

    যদি(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    যদি(bytestoread && Curl_rlimit_active(&তথ্য->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&তথ্য->progress.dl.rlimit, ফাঁকা);
#যদি 0
      DEBUGF(infof(তথ্য, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      যদি(dl_avail <= 0) {
        rate_limited = TRUE;
        বিরতি;
      }
      যদি(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    ফলাফল = xfer_recv_resp(তথ্য, buf, bytestoread, is_multiplex, &blen);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN)
        যাও out; /* real error */
      rcvd_eagain = TRUE;
      ফলাফল = CURLE_OK;
      যদি(তথ্য->req.download_done && তথ্য->req.no_body &&
         !তথ্য->req.resp_trailer) {
        DEBUGF(infof(তথ্য, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      অন্যথা
        বিরতি; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    যদি(!blen) {
      ফলাফল = Curl_req_stop_send_recv(তথ্য);
      যদি(ফলাফল)
        যাও out;
      যদি(k->eos_written) /* already did write this to client, leave */
        বিরতি;
    }

    ফলাফল = Curl_xfer_write_resp(তথ্য, buf, blen, is_eos);
    যদি(ফলাফল || তথ্য->req.done)
      যাও out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    যদি((!is_multiplex && তথ্য->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(তথ্য);
    }
    /* if we stopped receiving, leave the loop */
    যদি(!CURL_REQ_WANT_RECV(তথ্য))
      বিরতি;

  } যখন(maxloops--);

  যদি(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(তথ্য) &&
     (!rcvd_eagain || তথ্য_pending(তথ্য, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(তথ্য);
    CURL_TRC_M(তথ্য, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  যদি(!CURL_REQ_WANT_RECV(তথ্য) && CURL_REQ_WANT_SEND(তথ্য) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(তথ্য, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(তথ্য);
  }

out:
  Curl_multi_xfer_buf_release(তথ্য, xfer_buf);
  যদি(ফলাফল)
    DEBUGF(infof(তথ্য, "sendrecv_dl() -> %d", (পূর্ণসংখ্যা)ফলাফল));
  ফেরত ফলাফল;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
স্থির CURLcode sendrecv_ul(কাঠামো Curl_easy *তথ্য)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(তথ্য));

  যদি(!Curl_req_done_sending(তথ্য))
    ফেরত Curl_req_send_more(তথ্য);
  ফেরত CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  ধ্রুবক কাঠামো curltime *pnow = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(Curl_xfer_is_blocked(তথ্য)) {
    ফলাফল = CURLE_OK;
    যাও out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  যদি(CURL_REQ_WANT_RECV(তথ্য)) {
    ফলাফল = sendrecv_dl(তথ্য, k);
    যদি(ফলাফল || তথ্য->req.done)
      যাও out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  যদি(Curl_req_want_send(তথ্য)) {
    ফলাফল = sendrecv_ul(তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  pnow = Curl_pgrs_now(তথ্য);
  যদি(CURL_REQ_WANT_IO(তথ্য)) {
    যদি(Curl_timeleft_now_ms(তথ্য, pnow) < 0) {
      যদি(k->আকার_2 != -1) {
        failf(তথ্য, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE),
              k->bytecount, k->আকার_2);
      }
      অন্যথা {
        failf(তথ্য, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE),
              k->bytecount);
      }
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      যাও out;
    }
  }
  অন্যথা {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    যদি(!তথ্য->req.no_body && (k->আকার_2 != -1) &&
       (k->bytecount != k->আকার_2) && !k->newurl) {
      failf(তথ্য, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->আকার_2 - k->bytecount);
      ফলাফল = CURLE_PARTIAL_FILE;
      যাও out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  যদি(!CURL_REQ_WANT_IO(তথ্য))
    তথ্য->req.done = TRUE;

  ফলাফল = Curl_pgrsCheckX(তথ্য, pnow);

out:
  যদি(ফলাফল)
    DEBUGF(infof(তথ্য, "Curl_sendrecv() -> %d", (পূর্ণসংখ্যা)ফলাফল));
  ফেরত ফলাফল;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
শূন্য Curl_init_CONNECT(কাঠামো Curl_easy *তথ্য)
{
  তথ্য->state.fread_func = তথ্য->set.fread_func_set;
  তথ্য->state.in = তথ্য->set.in_set;
  তথ্য->state.upload = (তথ্য->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  তথ্য->state.retrycount = 0;

  যদি(!CURL_EASY_STR(তথ্য, STRING_SET_URL) && !তথ্য->set.uh) {
    /* we cannot do anything without URL */
    failf(তথ্য, "No URL set");
    ফেরত CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  যদি(তথ্য->set.uh) {
    অক্ষর *url = ফাঁকা;
    CURLUcode uc;
    uc = curl_url_get(তথ্য->set.uh, CURLUPART_URL, &url, 0);
    যদি(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&তথ্য->state.url, ফাঁকা, 0, ফাঁকা);
      failf(তথ্য, "No URL set");
      ফেরত CURLE_URL_MALFORMAT;
    }
    ফলাফল = CURL_EASY_STR_SETN(তথ্য, STRING_SET_URL, url);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  Curl_bufref_set(&তথ্য->state.url, CURL_EASY_STR(তথ্য, STRING_SET_URL),
                  0, ফাঁকা);

  যদি(তথ্য->set.postfields && তথ্য->set.set_resume_from) {
    /* we cannot */
    failf(তথ্য, "cannot mix POSTFIELDS with RESUME_FROM");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  তথ্য->state.prefer_ascii = তথ্য->set.prefer_ascii;
#ifdef CURL_LIST_ONLY_PROTOCOL
  তথ্য->state.list_only = তথ্য->set.list_only;
#endif
  তথ্য->state.httpreq = তথ্য->set.method;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&তথ্য->state.initial_origin);
  Curl_peer_unlink(&তথ্য->state.origin);
  তথ্য->state.requests = 0;
  তথ্য->state.followlocation = 0; /* reset the location-follow counter */
  তথ্য->state.this_is_a_follow = FALSE; /* reset this */
  তথ্য->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  তথ্য->state.errorbuf = FALSE; /* no error has occurred */
#ifndef CURL_DISABLE_HTTP
  Curl_http_neg_init(তথ্য, &তথ্য->state.http_neg);
#endif
  তথ্য->state.authproblem = FALSE;
  তথ্য->state.authhost.want = তথ্য->set.httpauth;
  তথ্য->state.authproxy.want = তথ্য->set.proxyauth;
  curlx_safefree(তথ্য->info.wouldredirect);
  curlx_safefree(তথ্য->info.effective_url);
  Curl_তথ্য_priority_clear_state(তথ্য);
  যদি(তথ্য->set.http_auto_referer)
    Curl_bufref_free(&তথ্য->state.referer);
  যদি(CURL_EASY_STR(তথ্য, STRING_SET_REFERER))
    Curl_bufref_set(&তথ্য->state.referer,
                    CURL_EASY_STR(তথ্য, STRING_SET_REFERER), 0, ফাঁকা);
  অন্যথা
    Curl_bufref_free(&তথ্য->state.referer);

  যদি(তথ্য->state.httpreq == HTTPREQ_PUT)
    তথ্য->state.infilesize = তথ্য->set.filesize;
  অন্যথা যদি((তথ্য->state.httpreq != HTTPREQ_GET) &&
          (তথ্য->state.httpreq != HTTPREQ_শীর্ষ)) {
    তথ্য->state.infilesize = তথ্য->set.postfieldsize;
    যদি(তথ্য->set.postfields && (তথ্য->state.infilesize == -1))
      তথ্য->state.infilesize = (curl_off_t)strlen(তথ্য->set.postfields);
  }
  অন্যথা
    তথ্য->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  ফলাফল = Curl_cookie_loadfiles(তথ্য,
                                 তথ্য->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  যদি(!ফলাফল)
    Curl_cookie_run(তথ্য); /* activate */

  /* If there is a list of host pairs to deal with */
  যদি(!ফলাফল && তথ্য->state.resolve)
    ফলাফল = Curl_loadhostpairs(তথ্য);

  যদি(!ফলাফল)
    /* If there is a list of hsts files to read */
    ফলাফল = Curl_hsts_loadfiles(তথ্য);

  যদি(!ফলাফল) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    তথ্য->state.allow_port = TRUE;

#যদি defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    যদি(!তথ্য->set.no_signal)
      তথ্য->state.পূর্ব_signal = সংকেত(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(তথ্য); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(তথ্য);
    Curl_pgrsStart(তথ্য, ফাঁকা);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    তথ্য->state.authhost.picked &= তথ্য->state.authhost.want;
    তথ্য->state.authproxy.picked &= তথ্য->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    তথ্য->state.wildcardmatch = তথ্য->set.wildcard_enabled;
#ifdef DEBUGBUILD
  যদি(getenv("CURL_DBG_FTP_WILDCARD"))
    তথ্য->state.wildcardmatch = TRUE;
#endif
    যদি(তথ্য->state.wildcardmatch) {
      কাঠামো Wildcard_তথ্য *wc;
      যদি(!তথ্য->wildcard) {
        তথ্য->wildcard = curlx_calloc(1, আকার(কাঠামো Wildcard_তথ্য));
        যদি(!তথ্য->wildcard)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      wc = তথ্য->wildcard;
      যদি(wc->state < CURLWC_INIT) {
        যদি(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->pattern);
        curlx_safefree(wc->path);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#endif
    ফলাফল = Curl_hsts_loadcb(তথ্য, তথ্য->hsts);
  }

  তথ্য->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  তথ্য->req.rewind_read = FALSE;
  Curl_client_reset(তথ্য);
  Curl_headers_cleanup(তথ্য);
  ফেরত ফলাফল;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(কাঠামো Curl_easy *তথ্য, অক্ষর **url)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  bool retry = FALSE;
  *url = ফাঁকা;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  যদি(তথ্য->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    ফেরত CURLE_OK;

  যদি(conn->bits.reuse &&
     (তথ্য->req.bytecount + তথ্য->req.headerbytecount == 0) &&
     ((!তথ্য->req.no_body && !তথ্য->req.done) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#ifndef CURL_DISABLE_RTSP
     && (তথ্য->set.rtspreq != RTSPREQ_RECEIVE)
#endif
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  অন্যথা যদি(তথ্য->state.refused_stream &&
          (তথ্য->req.bytecount + তথ্য->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(তথ্য, "REFUSED_STREAM, retrying a fresh connect");
    তথ্য->state.refused_stream = FALSE; /* clear again */
    retry = TRUE;
  }
  যদি(retry) {
#define CONN_MAX_RETRIES 5
    যদি(তথ্য->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(তথ্য, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      তথ্য->state.retrycount = 0;
      ফেরত CURLE_SEND_ERROR;
    }
    infof(তথ্য, "Connection died, retrying a fresh connect (retry count: %d)",
          তথ্য->state.retrycount);
    *url = Curl_bufref_dup(&তথ্য->state.url);
    যদি(!*url)
      ফেরত CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(তথ্য, TRUE);
  }
  ফেরত CURLE_OK;
}

স্থির শূন্য xfer_setup(
  কাঠামো Curl_easy *তথ্য,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_আকার      /* how much to receive, -1 if unknown */
  )
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  কাঠামো connectdata *conn = তথ্য->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(তথ্য));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_আকার == -1));
  k->আকার_2 = recv_আকার;
  k->header = !!conn->scheme->run->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  যদি(!k->header && (recv_আকার > 0))
    Curl_pgrs_Set_Download_আকার(তথ্য, recv_আকার);

  /* we want header and/or body, if neither then do not do this! */
  যদি(conn->scheme->run->write_resp_hd || !তথ্য->req.no_body) {
    যদি(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(তথ্য);
    যদি(conn->send_idx != -1)
      CURL_REQ_SET_SEND(তথ্য);
  }
  CURL_TRC_M(তথ্য, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

শূন্য Curl_xfer_setup_nop(কাঠামো Curl_easy *তথ্য)
{
  xfer_setup(তথ্য, -1, -1, -1);
}

শূন্য Curl_xfer_setup_sendrecv(কাঠামো Curl_easy *তথ্য,
                              int8_t sockindex,
                              curl_off_t recv_আকার)
{
  xfer_setup(তথ্য, sockindex, sockindex, recv_আকার);
}

শূন্য Curl_xfer_setup_send(কাঠামো Curl_easy *তথ্য,
                          int8_t sockindex)
{
  xfer_setup(তথ্য, sockindex, -1, -1);
}

শূন্য Curl_xfer_setup_recv(কাঠামো Curl_easy *তথ্য,
                          int8_t sockindex,
                          curl_off_t recv_আকার)
{
  xfer_setup(তথ্য, -1, sockindex, recv_আকার);
}

শূন্য Curl_xfer_set_shutdown(কাঠামো Curl_easy *তথ্য,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (তথ্য->conn->send_idx < 0) || (তথ্য->conn->recv_idx < 0));
  তথ্য->req.shutdown = shutdown;
  তথ্য->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(তথ্য->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ফলাফল = তথ্য->conn->scheme->run->write_resp(তথ্য, buf, blen, is_eos);
  }
  অন্যথা {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    যদি(blen || is_eos) {
      পূর্ণসংখ্যা cwtype = CLIENTWRITE_BODY;
      যদি(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      ফলাফল = Curl_client_write(তথ্য, cwtype, buf, blen);
    }
  }

  যদি(!ফলাফল && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    তথ্য->req.eos_written = TRUE;
    তথ্য->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(তথ্য, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

bool Curl_xfer_write_is_paused(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_cwriter_is_paused(তথ্য);
}

CURLcode Curl_xfer_write_resp_hd(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *hd0, size_t hdlen, bool is_eos)
{
  যদি(তথ্য->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ফেরত তথ্য->conn->scheme->run->write_resp_hd(তথ্য, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  ফেরত Curl_xfer_write_resp(তথ্য, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(কাঠামো Curl_easy *তথ্য, bool premature)
{
  (শূন্য)premature;
  ফেরত Curl_cw_out_done(তথ্য);
}

bool Curl_xfer_needs_flush(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_conn_needs_flush(তথ্য, তথ্য->conn->send_idx);
}

CURLcode Curl_xfer_flush(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_conn_flush(তথ্য, তথ্য->conn->send_idx);
}

CURLcode Curl_xfer_send(কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক শূন্য *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);

  ফলাফল = Curl_conn_send(তথ্য, তথ্য->conn->send_idx,
                          buf, blen, eos, pnwritten);
  যদি(ফলাফল == CURLE_AGAIN) {
    ফলাফল = CURLE_OK;
    *pnwritten = 0;
  }
  অন্যথা যদি(!ফলাফল && *pnwritten)
    তথ্য->info.request_আকার += *pnwritten;

  DEBUGF(infof(তথ্য, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (পূর্ণসংখ্যা)ফলাফল, *pnwritten));
  ফেরত ফলাফল;
}

CURLcode Curl_xfer_recv(কাঠামো Curl_easy *তথ্য,
                        অক্ষর *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  DEBUGASSERT(তথ্য->set.বাফার_আকার > 0);

  যদি(curlx_uitouz(তথ্য->set.বাফার_আকার) < blen)
    blen = curlx_uitouz(তথ্য->set.বাফার_আকার);
  ফেরত Curl_conn_recv(তথ্য, তথ্য->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(কাঠামো Curl_easy *তথ্য)
{
  Curl_conn_ev_তথ্য_done_send(তথ্য);
  ফেরত CURLE_OK;
}

bool Curl_xfer_is_blocked(কাঠামো Curl_easy *তথ্য)
{
  bool want_send = CURL_REQ_WANT_SEND(তথ্য);
  bool want_recv = CURL_REQ_WANT_RECV(তথ্য);
  যদি(!want_send)
    ফেরত want_recv && Curl_xfer_recv_is_paused(তথ্য);
  অন্যথা যদি(!want_recv)
    ফেরত want_send && Curl_xfer_send_is_paused(তথ্য);
  অন্যথা
    ফেরত Curl_xfer_recv_is_paused(তথ্য) && Curl_xfer_send_is_paused(তথ্য);
}

bool Curl_xfer_send_is_paused(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_rlimit_is_blocked(&তথ্য->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_rlimit_is_blocked(&তথ্য->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(কাঠামো Curl_easy *তথ্য, bool enable)
{
  CURLcode ফলাফল = CURLE_OK;
  Curl_rlimit_block(&তথ্য->progress.ul.rlimit, enable, Curl_pgrs_now(তথ্য));
  যদি(!enable && Curl_creader_is_paused(তথ্য))
    ফলাফল = Curl_creader_unpause(তথ্য);
  Curl_pgrsSendPause(তথ্য, enable);
  ফেরত ফলাফল;
}

CURLcode Curl_xfer_pause_recv(কাঠামো Curl_easy *তথ্য, bool enable)
{
  CURLcode ফলাফল = CURLE_OK;
  Curl_rlimit_block(&তথ্য->progress.dl.rlimit, enable, Curl_pgrs_now(তথ্য));
  যদি(!enable && Curl_cwriter_is_paused(তথ্য))
    ফলাফল = Curl_cwriter_unpause(তথ্য);
  Curl_conn_ev_তথ্য_pause(তথ্য, enable);
  Curl_pgrsRecvPause(তথ্য, enable);
  ফেরত ফলাফল;
}

bool Curl_xfer_is_secure(কাঠামো Curl_easy *তথ্য)
{
#ifndef CURL_DISABLE_PROXY
  যদি(তথ্য->conn && তথ্য->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(তথ্য->state.origin->scheme->flags & PROTOPT_SSL));
    ফেরত FALSE;
  }
#endif
  ফেরত (তথ্য->state.origin->scheme->flags & PROTOPT_SSL);
}
