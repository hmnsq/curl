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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#అన్తర్భావ <signal.h>

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_SELECT_H
#అన్తర్భావ <sys/select.h>
#లేదావేళ defined(HAVE_UNISTD_H)
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_SOCKET
#దోషం "We cannot compile without socket() support"
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cw-out.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "getinfo.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "headers.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "rtsp.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
అక్షరం *Curl_checkheaders(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *thisheader,
                        స్థిరరాశి size_t thislen)
{
  నిర్మాణం curl_slist *తల;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  కొరకు(తల = దత్తాంశం->సమితి.శీర్షికలు; తల; తల = తల->తదుపరి) {
    ఒకవేళ(curl_strnequal(తల->దత్తాంశం, thisheader, thislen) &&
       Curl_headersep(తల->దత్తాంశం[thislen]))
      తిరిగి తల->దత్తాంశం;
  }

  తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం దత్తాంశం_pending(నిర్మాణం Curl_easy *దత్తాంశం, bool rcvd_eagain)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  ఒకవేళ(conn->scheme->protocol & PROTO_FAMILY_FTP)
    తిరిగి Curl_conn_దత్తాంశం_pending(దత్తాంశం, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  తిరిగి (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(నిర్మాణం Curl_easy *దత్తాంశం, time_t timeofdoc)
{
  ఒకవేళ((timeofdoc == 0) || (దత్తాంశం->సమితి.timevalue == 0))
    తిరిగి TRUE;

  ఎంపిక(దత్తాంశం->సమితి.timecondition) {
  స్థితి CURL_TIMECOND_IFMODSINCE:
  అప్రమేయ:
    ఒకవేళ(timeofdoc <= దత్తాంశం->సమితి.timevalue) {
      infof(దత్తాంశం, "The requested document is not new enough");
      దత్తాంశం->info.timecond = TRUE;
      తిరిగి FALSE;
    }
    ఆపు;
  స్థితి CURL_TIMECOND_IFUNMODSINCE:
    ఒకవేళ(timeofdoc >= దత్తాంశం->సమితి.timevalue) {
      infof(దత్తాంశం, "The requested document is not old enough");
      దత్తాంశం->info.timecond = TRUE;
      తిరిగి FALSE;
    }
    ఆపు;
  }

  తిరిగి TRUE;
}

స్థిర CURLcode xfer_recv_shutdown(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->conn)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_cshutdn_try_once_idx(దత్తాంశం, దత్తాంశం->conn->recv_idx, పూర్తి);
}

స్థిర bool xfer_recv_shutdown_started(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->conn)
    తిరిగి FALSE;
  తిరిగి CURL_CONN_లోపల_SHUTDOWN(దత్తాంశం->conn, దత్తాంశం->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->conn)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_cshutdn_try_once_idx(దత్తాంశం, దత్తాంశం->conn->send_idx, పూర్తి);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
స్థిర CURLcode xfer_recv_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                               అక్షరం *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode ఫలితం;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  ఒకవేళ(!eos_reliable && !దత్తాంశం->req.శీర్షిక && దత్తాంశం->req.కొలత != -1) {
    blen = curlx_sotouz_range(దత్తాంశం->req.కొలత - దత్తాంశం->req.bytecount, 0, blen);
  }
  లేకపోతే ఒకవేళ(xfer_recv_shutdown_started(దత్తాంశం)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  ఒకవేళ(blen) {
    ఫలితం = Curl_xfer_recv(దత్తాంశం, buf, blen, pnread);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(*pnread == 0) {
    ఒకవేళ(దత్తాంశం->req.shutdown) {
      bool పూర్తి;
      ఫలితం = xfer_recv_shutdown(దత్తాంశం, &పూర్తి);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఒకవేళ(!పూర్తి) {
        తిరిగి CURLE_AGAIN;
      }
    }
    DEBUGF(infof(దత్తాంశం, "sendrecv_dl: we are done"));
  }
  తిరిగి CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
స్థిర CURLcode sendrecv_dl(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం SingleRequest *k)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *buf, *xfer_buf;
  size_t blen, xfer_blen;
  పూర్ణాంకం maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, రేటు_limited = FALSE;

  ఫలితం = Curl_multi_xfer_buf_borrow(దత్తాంశం, &xfer_buf, &xfer_blen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  చేయి {
    size_t bytestoread;

    ఒకవేళ(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    ఒకవేళ(bytestoread && Curl_rlimit_చురుకైన(&దత్తాంశం->పురోగతి.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&దత్తాంశం->పురోగతి.dl.rlimit, ఖాళీ);
#ఒకవేళ 0
      DEBUGF(infof(దత్తాంశం, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#అయితే_ముగింపు
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      ఒకవేళ(dl_avail <= 0) {
        రేటు_limited = TRUE;
        ఆపు;
      }
      ఒకవేళ(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    ఫలితం = xfer_recv_resp(దత్తాంశం, buf, bytestoread, is_multiplex, &blen);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN)
        వెళ్ళు బహిర్గతం; /* real error */
      rcvd_eagain = TRUE;
      ఫలితం = CURLE_OK;
      ఒకవేళ(దత్తాంశం->req.download_పూర్తి && దత్తాంశం->req.no_body &&
         !దత్తాంశం->req.resp_trailer) {
        DEBUGF(infof(దత్తాంశం, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      లేకపోతే
        ఆపు; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    ఒకవేళ(!blen) {
      ఫలితం = Curl_req_stop_send_recv(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఒకవేళ(k->eos_written) /* already did write this to client, leave */
        ఆపు;
    }

    ఫలితం = Curl_xfer_write_resp(దత్తాంశం, buf, blen, is_eos);
    ఒకవేళ(ఫలితం || దత్తాంశం->req.పూర్తి)
      వెళ్ళు బహిర్గతం;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    ఒకవేళ((!is_multiplex && దత్తాంశం->req.download_పూర్తి) || is_eos) {
      CURL_REQ_CLEAR_RECV(దత్తాంశం);
    }
    /* if we stopped receiving, leave the loop */
    ఒకవేళ(!CURL_REQ_WANT_RECV(దత్తాంశం))
      ఆపు;

  } అయితే(maxloops--);

  ఒకవేళ(!is_eos && !రేటు_limited && CURL_REQ_WANT_RECV(దత్తాంశం) &&
     (!rcvd_eagain || దత్తాంశం_pending(దత్తాంశం, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(దత్తాంశం);
    CURL_TRC_M(దత్తాంశం, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  ఒకవేళ(!CURL_REQ_WANT_RECV(దత్తాంశం) && CURL_REQ_WANT_SEND(దత్తాంశం) &&
     (conn->బిట్లు.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(దత్తాంశం, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(దత్తాంశం);
  }

బహిర్గతం:
  Curl_multi_xfer_buf_release(దత్తాంశం, xfer_buf);
  ఒకవేళ(ఫలితం)
    DEBUGF(infof(దత్తాంశం, "sendrecv_dl() -> %d", (పూర్ణాంకం)ఫలితం));
  తిరిగి ఫలితం;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
స్థిర CURLcode sendrecv_ul(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_పూర్తి_sending(దత్తాంశం));

  ఒకవేళ(!Curl_req_పూర్తి_sending(దత్తాంశం))
    తిరిగి Curl_req_send_మరిన్ని(దత్తాంశం);
  తిరిగి CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  స్థిరరాశి నిర్మాణం curltime *pnow = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(Curl_xfer_is_blocked(దత్తాంశం)) {
    ఫలితం = CURLE_OK;
    వెళ్ళు బహిర్గతం;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  ఒకవేళ(CURL_REQ_WANT_RECV(దత్తాంశం)) {
    ఫలితం = sendrecv_dl(దత్తాంశం, k);
    ఒకవేళ(ఫలితం || దత్తాంశం->req.పూర్తి)
      వెళ్ళు బహిర్గతం;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  ఒకవేళ(Curl_req_want_send(దత్తాంశం)) {
    ఫలితం = sendrecv_ul(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  pnow = Curl_pgrs_now(దత్తాంశం);
  ఒకవేళ(CURL_REQ_WANT_IO(దత్తాంశం)) {
    ఒకవేళ(Curl_timeleft_now_ms(దత్తాంశం, pnow) < 0) {
      ఒకవేళ(k->కొలత != -1) {
        failf(దత్తాంశం, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE),
              k->bytecount, k->కొలత);
      }
      లేకపోతే {
        failf(దత్తాంశం, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE),
              k->bytecount);
      }
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    ఒకవేళ(!దత్తాంశం->req.no_body && (k->కొలత != -1) &&
       (k->bytecount != k->కొలత) && !k->newurl) {
      failf(దత్తాంశం, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->కొలత - k->bytecount);
      ఫలితం = CURLE_PARTIAL_FILE;
      వెళ్ళు బహిర్గతం;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  ఒకవేళ(!CURL_REQ_WANT_IO(దత్తాంశం))
    దత్తాంశం->req.పూర్తి = TRUE;

  ఫలితం = Curl_pgrsCheckX(దత్తాంశం, pnow);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    DEBUGF(infof(దత్తాంశం, "Curl_sendrecv() -> %d", (పూర్ణాంకం)ఫలితం));
  తిరిగి ఫలితం;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
శూన్య Curl_init_CONNECT(నిర్మాణం Curl_easy *దత్తాంశం)
{
  దత్తాంశం->state.fread_func = దత్తాంశం->సమితి.fread_func_సమితి;
  దత్తాంశం->state.లోపల = దత్తాంశం->సమితి.లోపల_సమితి;
  దత్తాంశం->state.upload = (దత్తాంశం->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  దత్తాంశం->state.retrycount = 0;

  ఒకవేళ(!CURL_EASY_STR(దత్తాంశం, STRING_సమితి_URL) && !దత్తాంశం->సమితి.uh) {
    /* we cannot do anything without URL */
    failf(దత్తాంశం, "No URL set");
    తిరిగి CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  ఒకవేళ(దత్తాంశం->సమితి.uh) {
    అక్షరం *url = ఖాళీ;
    CURLUcode uc;
    uc = curl_url_get(దత్తాంశం->సమితి.uh, CURLUPART_URL, &url, 0);
    ఒకవేళ(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_సమితి(&దత్తాంశం->state.url, ఖాళీ, 0, ఖాళీ);
      failf(దత్తాంశం, "No URL set");
      తిరిగి CURLE_URL_MALFORMAT;
    }
    ఫలితం = CURL_EASY_STR_SETN(దత్తాంశం, STRING_సమితి_URL, url);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  Curl_bufref_సమితి(&దత్తాంశం->state.url, CURL_EASY_STR(దత్తాంశం, STRING_సమితి_URL),
                  0, ఖాళీ);

  ఒకవేళ(దత్తాంశం->సమితి.postfields && దత్తాంశం->సమితి.సమితి_resume_from) {
    /* we cannot */
    failf(దత్తాంశం, "cannot mix POSTFIELDS with RESUME_FROM");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  దత్తాంశం->state.prefer_ascii = దత్తాంశం->సమితి.prefer_ascii;
#నిర్వచితమైతే CURL_LIST_ONLY_PROTOCOL
  దత్తాంశం->state.list_only = దత్తాంశం->సమితి.list_only;
#అయితే_ముగింపు
  దత్తాంశం->state.httpreq = దత్తాంశం->సమితి.పద్ధతి;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&దత్తాంశం->state.initial_origin);
  Curl_peer_unlink(&దత్తాంశం->state.origin);
  దత్తాంశం->state.అభ్యర్థనలు = 0;
  దత్తాంశం->state.followlocation = 0; /* reset the location-follow counter */
  దత్తాంశం->state.this_is_a_follow = FALSE; /* reset this */
  దత్తాంశం->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  దత్తాంశం->state.errorbuf = FALSE; /* no error has occurred */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  Curl_http_neg_init(దత్తాంశం, &దత్తాంశం->state.http_neg);
#అయితే_ముగింపు
  దత్తాంశం->state.authproblem = FALSE;
  దత్తాంశం->state.authhost.want = దత్తాంశం->సమితి.httpauth;
  దత్తాంశం->state.authproxy.want = దత్తాంశం->సమితి.proxyauth;
  curlx_safefree(దత్తాంశం->info.wouldredirect);
  curlx_safefree(దత్తాంశం->info.effective_url);
  Curl_దత్తాంశం_ప్రాధాన్యత_clear_state(దత్తాంశం);
  ఒకవేళ(దత్తాంశం->సమితి.http_auto_referer)
    Curl_bufref_free(&దత్తాంశం->state.referer);
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_సమితి_REFERER))
    Curl_bufref_సమితి(&దత్తాంశం->state.referer,
                    CURL_EASY_STR(దత్తాంశం, STRING_సమితి_REFERER), 0, ఖాళీ);
  లేకపోతే
    Curl_bufref_free(&దత్తాంశం->state.referer);

  ఒకవేళ(దత్తాంశం->state.httpreq == HTTPREQ_PUT)
    దత్తాంశం->state.infilesize = దత్తాంశం->సమితి.filesize;
  లేకపోతే ఒకవేళ((దత్తాంశం->state.httpreq != HTTPREQ_GET) &&
          (దత్తాంశం->state.httpreq != HTTPREQ_తల)) {
    దత్తాంశం->state.infilesize = దత్తాంశం->సమితి.postfieldsize;
    ఒకవేళ(దత్తాంశం->సమితి.postfields && (దత్తాంశం->state.infilesize == -1))
      దత్తాంశం->state.infilesize = (curl_off_t)అక్షరశ్రేణి_పొడవు(దత్తాంశం->సమితి.postfields);
  }
  లేకపోతే
    దత్తాంశం->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  ఫలితం = Curl_cookie_loadfiles(దత్తాంశం,
                                 దత్తాంశం->సమితి.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  ఒకవేళ(!ఫలితం)
    Curl_cookie_నిరంతరం(దత్తాంశం); /* activate */

  /* If there is a list of host pairs to deal with */
  ఒకవేళ(!ఫలితం && దత్తాంశం->state.resolve)
    ఫలితం = Curl_loadhostpairs(దత్తాంశం);

  ఒకవేళ(!ఫలితం)
    /* If there is a list of hsts files to read */
    ఫలితం = Curl_hsts_loadfiles(దత్తాంశం);

  ఒకవేళ(!ఫలితం) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    దత్తాంశం->state.allow_పోర్టు = TRUE;

#ఒకవేళ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    ఒకవేళ(!దత్తాంశం->సమితి.no_signal)
      దత్తాంశం->state.prev_signal = సంకేతం(SIGPIPE, SIG_IGN);
#అయితే_ముగింపు

    Curl_initinfo(దత్తాంశం); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(దత్తాంశం);
    Curl_pgrs_ప్రారంభం(దత్తాంశం, ఖాళీ);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    దత్తాంశం->state.authhost.picked &= దత్తాంశం->state.authhost.want;
    దత్తాంశం->state.authproxy.picked &= దత్తాంశం->state.authproxy.want;

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
    దత్తాంశం->state.wildcardmatch = దత్తాంశం->సమితి.wildcard_enabled;
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(getenv("CURL_DBG_FTP_WILDCARD"))
    దత్తాంశం->state.wildcardmatch = TRUE;
#అయితే_ముగింపు
    ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
      నిర్మాణం Wildcard_దత్తాంశం *wc;
      ఒకవేళ(!దత్తాంశం->wildcard) {
        దత్తాంశం->wildcard = curlx_calloc(1, పరిమాణం(నిర్మాణం Wildcard_దత్తాంశం));
        ఒకవేళ(!దత్తాంశం->wildcard)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      wc = దత్తాంశం->wildcard;
      ఒకవేళ(wc->state < CURLWC_INIT) {
        ఒకవేళ(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->నమూనా);
        curlx_safefree(wc->మార్గం);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#అయితే_ముగింపు
    ఫలితం = Curl_hsts_loadcb(దత్తాంశం, దత్తాంశం->hsts);
  }

  దత్తాంశం->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  దత్తాంశం->req.rewind_read = FALSE;
  Curl_client_reset(దత్తాంశం);
  Curl_శీర్షికలు_cleanup(దత్తాంశం);
  తిరిగి ఫలితం;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం **url)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  bool retry = FALSE;
  *url = ఖాళీ;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  ఒకవేళ(దత్తాంశం->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    తిరిగి CURLE_OK;

  ఒకవేళ(conn->బిట్లు.reuse &&
     (దత్తాంశం->req.bytecount + దత్తాంశం->req.headerbytecount == 0) &&
     ((!దత్తాంశం->req.no_body && !దత్తాంశం->req.పూర్తి) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
     && (దత్తాంశం->సమితి.rtspreq != RTSPREQ_RECEIVE)
#అయితే_ముగింపు
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  లేకపోతే ఒకవేళ(దత్తాంశం->state.refused_ప్రవాహం &&
          (దత్తాంశం->req.bytecount + దత్తాంశం->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(దత్తాంశం, "REFUSED_STREAM, retrying a fresh connect");
    దత్తాంశం->state.refused_ప్రవాహం = FALSE; /* clear again */
    retry = TRUE;
  }
  ఒకవేళ(retry) {
#నిర్వచించు CONN_MAX_RETRIES 5
    ఒకవేళ(దత్తాంశం->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(దత్తాంశం, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      దత్తాంశం->state.retrycount = 0;
      తిరిగి CURLE_SEND_ERROR;
    }
    infof(దత్తాంశం, "Connection died, retrying a fresh connect (retry count: %d)",
          దత్తాంశం->state.retrycount);
    *url = Curl_bufref_dup(&దత్తాంశం->state.url);
    ఒకవేళ(!*url)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    connclose(conn); /* close this connection */
    conn->బిట్లు.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_సమితి_rewind(దత్తాంశం, TRUE);
  }
  తిరిగి CURLE_OK;
}

స్థిర శూన్య xfer_అమరిక(
  నిర్మాణం Curl_easy *దత్తాంశం,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_కొలత      /* how much to receive, -1 if unknown */
  )
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(దత్తాంశం));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_కొలత == -1));
  k->కొలత = recv_కొలత;
  k->శీర్షిక = !!conn->scheme->నిరంతరం->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  ఒకవేళ(!k->శీర్షిక && (recv_కొలత > 0))
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, recv_కొలత);

  /* we want header and/or body, if neither then do not do this! */
  ఒకవేళ(conn->scheme->నిరంతరం->write_resp_hd || !దత్తాంశం->req.no_body) {
    ఒకవేళ(conn->recv_idx != -1)
      CURL_REQ_సమితి_RECV(దత్తాంశం);
    ఒకవేళ(conn->send_idx != -1)
      CURL_REQ_సమితి_SEND(దత్తాంశం);
  }
  CURL_TRC_M(దత్తాంశం, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

శూన్య Curl_xfer_అమరిక_nop(నిర్మాణం Curl_easy *దత్తాంశం)
{
  xfer_అమరిక(దత్తాంశం, -1, -1, -1);
}

శూన్య Curl_xfer_అమరిక_sendrecv(నిర్మాణం Curl_easy *దత్తాంశం,
                              int8_t sockindex,
                              curl_off_t recv_కొలత)
{
  xfer_అమరిక(దత్తాంశం, sockindex, sockindex, recv_కొలత);
}

శూన్య Curl_xfer_అమరిక_send(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex)
{
  xfer_అమరిక(దత్తాంశం, sockindex, -1, -1);
}

శూన్య Curl_xfer_అమరిక_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex,
                          curl_off_t recv_కొలత)
{
  xfer_అమరిక(దత్తాంశం, -1, sockindex, recv_కొలత);
}

శూన్య Curl_xfer_సమితి_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (దత్తాంశం->conn->send_idx < 0) || (దత్తాంశం->conn->recv_idx < 0));
  దత్తాంశం->req.shutdown = shutdown;
  దత్తాంశం->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(దత్తాంశం->conn->scheme->నిరంతరం->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ఫలితం = దత్తాంశం->conn->scheme->నిరంతరం->write_resp(దత్తాంశం, buf, blen, is_eos);
  }
  లేకపోతే {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    ఒకవేళ(blen || is_eos) {
      పూర్ణాంకం cwtype = CLIENTWRITE_BODY;
      ఒకవేళ(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      ఫలితం = Curl_client_write(దత్తాంశం, cwtype, buf, blen);
    }
  }

  ఒకవేళ(!ఫలితం && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    దత్తాంశం->req.eos_written = TRUE;
    దత్తాంశం->req.download_పూర్తి = TRUE;
  }
  CURL_TRC_WRITE(దత్తాంశం, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

bool Curl_xfer_write_is_paused(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_cwriter_is_paused(దత్తాంశం);
}

CURLcode Curl_xfer_write_resp_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *hd0, size_t hdlen, bool is_eos)
{
  ఒకవేళ(దత్తాంశం->conn->scheme->నిరంతరం->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    తిరిగి దత్తాంశం->conn->scheme->నిరంతరం->write_resp_hd(దత్తాంశం, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  తిరిగి Curl_xfer_write_resp(దత్తాంశం, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, bool premature)
{
  (శూన్య)premature;
  తిరిగి Curl_cw_బహిర్గతం_పూర్తి(దత్తాంశం);
}

bool Curl_xfer_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_conn_needs_flush(దత్తాంశం, దత్తాంశం->conn->send_idx);
}

CURLcode Curl_xfer_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_conn_flush(దత్తాంశం, దత్తాంశం->conn->send_idx);
}

CURLcode Curl_xfer_send(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి శూన్య *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);

  ఫలితం = Curl_conn_send(దత్తాంశం, దత్తాంశం->conn->send_idx,
                          buf, blen, eos, pnwritten);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    ఫలితం = CURLE_OK;
    *pnwritten = 0;
  }
  లేకపోతే ఒకవేళ(!ఫలితం && *pnwritten)
    దత్తాంశం->info.request_కొలత += *pnwritten;

  DEBUGF(infof(దత్తాంశం, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (పూర్ణాంకం)ఫలితం, *pnwritten));
  తిరిగి ఫలితం;
}

CURLcode Curl_xfer_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                        అక్షరం *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  DEBUGASSERT(దత్తాంశం->సమితి.బఫరు_కొలత > 0);

  ఒకవేళ(curlx_uitouz(దత్తాంశం->సమితి.బఫరు_కొలత) < blen)
    blen = curlx_uitouz(దత్తాంశం->సమితి.బఫరు_కొలత);
  తిరిగి Curl_conn_recv(దత్తాంశం, దత్తాంశం->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_conn_ev_దత్తాంశం_పూర్తి_send(దత్తాంశం);
  తిరిగి CURLE_OK;
}

bool Curl_xfer_is_blocked(నిర్మాణం Curl_easy *దత్తాంశం)
{
  bool want_send = CURL_REQ_WANT_SEND(దత్తాంశం);
  bool want_recv = CURL_REQ_WANT_RECV(దత్తాంశం);
  ఒకవేళ(!want_send)
    తిరిగి want_recv && Curl_xfer_recv_is_paused(దత్తాంశం);
  లేకపోతే ఒకవేళ(!want_recv)
    తిరిగి want_send && Curl_xfer_send_is_paused(దత్తాంశం);
  లేకపోతే
    తిరిగి Curl_xfer_recv_is_paused(దత్తాంశం) && Curl_xfer_send_is_paused(దత్తాంశం);
}

bool Curl_xfer_send_is_paused(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_rlimit_is_blocked(&దత్తాంశం->పురోగతి.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_rlimit_is_blocked(&దత్తాంశం->పురోగతి.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(నిర్మాణం Curl_easy *దత్తాంశం, bool enable)
{
  CURLcode ఫలితం = CURLE_OK;
  Curl_rlimit_ఖండం(&దత్తాంశం->పురోగతి.ul.rlimit, enable, Curl_pgrs_now(దత్తాంశం));
  ఒకవేళ(!enable && Curl_creader_is_paused(దత్తాంశం))
    ఫలితం = Curl_creader_unpause(దత్తాంశం);
  Curl_pgrsSendPause(దత్తాంశం, enable);
  తిరిగి ఫలితం;
}

CURLcode Curl_xfer_pause_recv(నిర్మాణం Curl_easy *దత్తాంశం, bool enable)
{
  CURLcode ఫలితం = CURLE_OK;
  Curl_rlimit_ఖండం(&దత్తాంశం->పురోగతి.dl.rlimit, enable, Curl_pgrs_now(దత్తాంశం));
  ఒకవేళ(!enable && Curl_cwriter_is_paused(దత్తాంశం))
    ఫలితం = Curl_cwriter_unpause(దత్తాంశం);
  Curl_conn_ev_దత్తాంశం_pause(దత్తాంశం, enable);
  Curl_pgrsRecvPause(దత్తాంశం, enable);
  తిరిగి ఫలితం;
}

bool Curl_xfer_is_secure(నిర్మాణం Curl_easy *దత్తాంశం)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(దత్తాంశం->conn && దత్తాంశం->conn->బిట్లు.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(దత్తాంశం->state.origin->scheme->flags & PROTOPT_SSL));
    తిరిగి FALSE;
  }
#అయితే_ముగింపు
  తిరిగి (దత్తాంశం->state.origin->scheme->flags & PROTOPT_SSL);
}
