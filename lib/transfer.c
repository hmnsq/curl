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
#ضمن "curl_setup.h"

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت
#ضمن <signal.h>

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#جے_انت

#جے_بنا_تعریف HAVE_SOCKET
#غلطی "We cannot compile without socket() support"
#جے_انت

#ضمن "urldata.h"

#ضمن "cfilters.h"
#ضمن "cw-out.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "http.h"
#ضمن "url.h"
#ضمن "getinfo.h"
#ضمن "multiif.h"
#ضمن "connect.h"
#ضمن "hsts.h"
#ضمن "setopt.h"
#ضمن "headers.h"
#ضمن "bufref.h"
#ضمن "rtsp.h"

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
حرف *Curl_checkheaders(ثابت بنية Curl_easy *انکڑے,
                        ثابت حرف *thisheader,
                        ثابت size_t thislen)
{
  بنية curl_slist *سر;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  لكل(سر = انکڑے->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    إذا(curl_strnequal(سر->انکڑے, thisheader, thislen) &&
       Curl_headersep(سر->انکڑے[thislen]))
      أرجع سر->انکڑے;
  }

  أرجع عدم;
}
#جے_انت

ساكن صحيح انکڑے_pending(بنية Curl_easy *انکڑے, bool rcvd_eagain)
{
  بنية connectdata *conn = انکڑے->conn;

  إذا(conn->scheme->protocol & PROTO_FAMILY_FTP)
    أرجع Curl_conn_انکڑے_pending(انکڑے, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  أرجع (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(بنية Curl_easy *انکڑے, time_t timeofdoc)
{
  إذا((timeofdoc == 0) || (انکڑے->سیٹ.timevalue == 0))
    أرجع TRUE;

  بدل(انکڑے->سیٹ.timecondition) {
  حالة CURL_TIMECOND_IFMODSINCE:
  افتراضي:
    إذا(timeofdoc <= انکڑے->سیٹ.timevalue) {
      infof(انکڑے, "The requested document is not new enough");
      انکڑے->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  حالة CURL_TIMECOND_IFUNMODSINCE:
    إذا(timeofdoc >= انکڑے->سیٹ.timevalue) {
      infof(انکڑے, "The requested document is not old enough");
      انکڑے->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  }

  أرجع TRUE;
}

ساكن CURLcode xfer_recv_shutdown(بنية Curl_easy *انکڑے, bool *پورا)
{
  إذا(!انکڑے || !انکڑے->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(انکڑے, انکڑے->conn->recv_idx, پورا);
}

ساكن bool xfer_recv_shutdown_started(بنية Curl_easy *انکڑے)
{
  إذا(!انکڑے || !انکڑے->conn)
    أرجع FALSE;
  أرجع CURL_CONN_وچ_SHUTDOWN(انکڑے->conn, انکڑے->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(بنية Curl_easy *انکڑے, bool *پورا)
{
  إذا(!انکڑے || !انکڑے->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(انکڑے, انکڑے->conn->send_idx, پورا);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ساكن CURLcode xfer_recv_resp(بنية Curl_easy *انکڑے,
                               حرف *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode نتیجہ;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  إذا(!eos_reliable && !انکڑے->req.سرخی && انکڑے->req.ناپ != -1) {
    blen = curlx_sotouz_range(انکڑے->req.ناپ - انکڑے->req.bytecount, 0, blen);
  }
  وإلا إذا(xfer_recv_shutdown_started(انکڑے)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  إذا(blen) {
    نتیجہ = Curl_xfer_recv(انکڑے, buf, blen, pnread);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(*pnread == 0) {
    إذا(انکڑے->req.shutdown) {
      bool پورا;
      نتیجہ = xfer_recv_shutdown(انکڑے, &پورا);
      إذا(نتیجہ)
        أرجع نتیجہ;
      إذا(!پورا) {
        أرجع CURLE_AGAIN;
      }
    }
    DEBUGF(infof(انکڑے, "sendrecv_dl: we are done"));
  }
  أرجع CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ساكن CURLcode sendrecv_dl(بنية Curl_easy *انکڑے,
                            بنية SingleRequest *k)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;
  حرف *buf, *xfer_buf;
  size_t blen, xfer_blen;
  صحيح maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, شرح_limited = FALSE;

  نتیجہ = Curl_multi_xfer_buf_borrow(انکڑے, &xfer_buf, &xfer_blen);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  افعل {
    size_t bytestoread;

    إذا(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    إذا(bytestoread && Curl_rlimit_سرگرم(&انکڑے->ترقی.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&انکڑے->ترقی.dl.rlimit, عدم);
#إذا 0
      DEBUGF(infof(انکڑے, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#جے_انت
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      إذا(dl_avail <= 0) {
        شرح_limited = TRUE;
        اكسر;
      }
      إذا(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    نتیجہ = xfer_recv_resp(انکڑے, buf, bytestoread, is_multiplex, &blen);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN)
        اذهب باہرلا; /* real error */
      rcvd_eagain = TRUE;
      نتیجہ = CURLE_OK;
      إذا(انکڑے->req.download_پورا && انکڑے->req.no_body &&
         !انکڑے->req.resp_trailer) {
        DEBUGF(infof(انکڑے, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      وإلا
        اكسر; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    إذا(!blen) {
      نتیجہ = Curl_req_stop_send_recv(انکڑے);
      إذا(نتیجہ)
        اذهب باہرلا;
      إذا(k->eos_written) /* already did write this to client, leave */
        اكسر;
    }

    نتیجہ = Curl_xfer_write_resp(انکڑے, buf, blen, is_eos);
    إذا(نتیجہ || انکڑے->req.پورا)
      اذهب باہرلا;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    إذا((!is_multiplex && انکڑے->req.download_پورا) || is_eos) {
      CURL_REQ_CLEAR_RECV(انکڑے);
    }
    /* if we stopped receiving, leave the loop */
    إذا(!CURL_REQ_WANT_RECV(انکڑے))
      اكسر;

  } طالما(maxloops--);

  إذا(!is_eos && !شرح_limited && CURL_REQ_WANT_RECV(انکڑے) &&
     (!rcvd_eagain || انکڑے_pending(انکڑے, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(انکڑے);
    CURL_TRC_M(انکڑے, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  إذا(!CURL_REQ_WANT_RECV(انکڑے) && CURL_REQ_WANT_SEND(انکڑے) &&
     (conn->بٹ.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(انکڑے, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(انکڑے);
  }

باہرلا:
  Curl_multi_xfer_buf_release(انکڑے, xfer_buf);
  إذا(نتیجہ)
    DEBUGF(infof(انکڑے, "sendrecv_dl() -> %d", (صحيح)نتیجہ));
  أرجع نتیجہ;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ساكن CURLcode sendrecv_ul(بنية Curl_easy *انکڑے)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_پورا_sending(انکڑے));

  إذا(!Curl_req_پورا_sending(انکڑے))
    أرجع Curl_req_send_ہور(انکڑے);
  أرجع CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(بنية Curl_easy *انکڑے)
{
  بنية SingleRequest *k = &انکڑے->req;
  ثابت بنية curltime *pnow = عدم;
  CURLcode نتیجہ = CURLE_OK;

  إذا(Curl_xfer_is_blocked(انکڑے)) {
    نتیجہ = CURLE_OK;
    اذهب باہرلا;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  إذا(CURL_REQ_WANT_RECV(انکڑے)) {
    نتیجہ = sendrecv_dl(انکڑے, k);
    إذا(نتیجہ || انکڑے->req.پورا)
      اذهب باہرلا;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  إذا(Curl_req_want_send(انکڑے)) {
    نتیجہ = sendrecv_ul(انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  pnow = Curl_pgrs_now(انکڑے);
  إذا(CURL_REQ_WANT_IO(انکڑے)) {
    إذا(Curl_timeleft_now_ms(انکڑے, pnow) < 0) {
      إذا(k->ناپ != -1) {
        failf(انکڑے, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE),
              k->bytecount, k->ناپ);
      }
      وإلا {
        failf(انکڑے, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE),
              k->bytecount);
      }
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اذهب باہرلا;
    }
  }
  وإلا {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    إذا(!انکڑے->req.no_body && (k->ناپ != -1) &&
       (k->bytecount != k->ناپ) && !k->newurl) {
      failf(انکڑے, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->ناپ - k->bytecount);
      نتیجہ = CURLE_PARTIAL_FILE;
      اذهب باہرلا;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  إذا(!CURL_REQ_WANT_IO(انکڑے))
    انکڑے->req.پورا = TRUE;

  نتیجہ = Curl_pgrsCheckX(انکڑے, pnow);

باہرلا:
  إذا(نتیجہ)
    DEBUGF(infof(انکڑے, "Curl_sendrecv() -> %d", (صحيح)نتیجہ));
  أرجع نتیجہ;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
فراغ Curl_init_CONNECT(بنية Curl_easy *انکڑے)
{
  انکڑے->state.fread_func = انکڑے->سیٹ.fread_func_سیٹ;
  انکڑے->state.وچ = انکڑے->سیٹ.وچ_سیٹ;
  انکڑے->state.upload = (انکڑے->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  انکڑے->state.retrycount = 0;

  إذا(!CURL_EASY_STR(انکڑے, STRING_سیٹ_URL) && !انکڑے->سیٹ.uh) {
    /* we cannot do anything without URL */
    failf(انکڑے, "No URL set");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  إذا(انکڑے->سیٹ.uh) {
    حرف *url = عدم;
    CURLUcode uc;
    uc = curl_url_get(انکڑے->سیٹ.uh, CURLUPART_URL, &url, 0);
    إذا(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_سیٹ(&انکڑے->state.url, عدم, 0, عدم);
      failf(انکڑے, "No URL set");
      أرجع CURLE_URL_MALFORMAT;
    }
    نتیجہ = CURL_EASY_STR_SETN(انکڑے, STRING_سیٹ_URL, url);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  Curl_bufref_سیٹ(&انکڑے->state.url, CURL_EASY_STR(انکڑے, STRING_سیٹ_URL),
                  0, عدم);

  إذا(انکڑے->سیٹ.postfields && انکڑے->سیٹ.سیٹ_resume_from) {
    /* we cannot */
    failf(انکڑے, "cannot mix POSTFIELDS with RESUME_FROM");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  انکڑے->state.prefer_ascii = انکڑے->سیٹ.prefer_ascii;
#جے_تعریف_شدہ CURL_LIST_ONLY_PROTOCOL
  انکڑے->state.list_only = انکڑے->سیٹ.list_only;
#جے_انت
  انکڑے->state.httpreq = انکڑے->سیٹ.طریقہ;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&انکڑے->state.initial_origin);
  Curl_peer_unlink(&انکڑے->state.origin);
  انکڑے->state.بینتیاں = 0;
  انکڑے->state.followlocation = 0; /* reset the location-follow counter */
  انکڑے->state.this_is_a_follow = FALSE; /* reset this */
  انکڑے->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  انکڑے->state.errorbuf = FALSE; /* no error has occurred */
#جے_بنا_تعریف CURL_DISABLE_HTTP
  Curl_http_neg_init(انکڑے, &انکڑے->state.http_neg);
#جے_انت
  انکڑے->state.authproblem = FALSE;
  انکڑے->state.authhost.want = انکڑے->سیٹ.httpauth;
  انکڑے->state.authproxy.want = انکڑے->سیٹ.proxyauth;
  curlx_safefree(انکڑے->info.wouldredirect);
  curlx_safefree(انکڑے->info.effective_url);
  Curl_انکڑے_ترجیح_clear_state(انکڑے);
  إذا(انکڑے->سیٹ.http_auto_referer)
    Curl_bufref_free(&انکڑے->state.referer);
  إذا(CURL_EASY_STR(انکڑے, STRING_سیٹ_REFERER))
    Curl_bufref_سیٹ(&انکڑے->state.referer,
                    CURL_EASY_STR(انکڑے, STRING_سیٹ_REFERER), 0, عدم);
  وإلا
    Curl_bufref_free(&انکڑے->state.referer);

  إذا(انکڑے->state.httpreq == HTTPREQ_PUT)
    انکڑے->state.infilesize = انکڑے->سیٹ.filesize;
  وإلا إذا((انکڑے->state.httpreq != HTTPREQ_GET) &&
          (انکڑے->state.httpreq != HTTPREQ_سر)) {
    انکڑے->state.infilesize = انکڑے->سیٹ.postfieldsize;
    إذا(انکڑے->سیٹ.postfields && (انکڑے->state.infilesize == -1))
      انکڑے->state.infilesize = (curl_off_t)لڑی_لمبائی(انکڑے->سیٹ.postfields);
  }
  وإلا
    انکڑے->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  نتیجہ = Curl_cookie_loadfiles(انکڑے,
                                 انکڑے->سیٹ.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  إذا(!نتیجہ)
    Curl_cookie_لگاتار(انکڑے); /* activate */

  /* If there is a list of host pairs to deal with */
  إذا(!نتیجہ && انکڑے->state.resolve)
    نتیجہ = Curl_loadhostpairs(انکڑے);

  إذا(!نتیجہ)
    /* If there is a list of hsts files to read */
    نتیجہ = Curl_hsts_loadfiles(انکڑے);

  إذا(!نتیجہ) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    انکڑے->state.allow_پورٹ = TRUE;

#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    إذا(!انکڑے->سیٹ.no_signal)
      انکڑے->state.prev_signal = إشارة(SIGPIPE, SIG_IGN);
#جے_انت

    Curl_initinfo(انکڑے); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(انکڑے);
    Curl_pgrs_شروع(انکڑے, عدم);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    انکڑے->state.authhost.picked &= انکڑے->state.authhost.want;
    انکڑے->state.authproxy.picked &= انکڑے->state.authproxy.want;

#جے_بنا_تعریف CURL_DISABLE_FTP
    انکڑے->state.wildcardmatch = انکڑے->سیٹ.wildcard_enabled;
#جے_تعریف_شدہ DEBUGBUILD
  إذا(getenv("CURL_DBG_FTP_WILDCARD"))
    انکڑے->state.wildcardmatch = TRUE;
#جے_انت
    إذا(انکڑے->state.wildcardmatch) {
      بنية Wildcard_انکڑے *wc;
      إذا(!انکڑے->wildcard) {
        انکڑے->wildcard = curlx_calloc(1, حجم(بنية Wildcard_انکڑے));
        إذا(!انکڑے->wildcard)
          أرجع CURLE_باہرلا_دا_حافظہ;
      }
      wc = انکڑے->wildcard;
      إذا(wc->state < CURLWC_INIT) {
        إذا(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->نمونہ);
        curlx_safefree(wc->رستہ);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#جے_انت
    نتیجہ = Curl_hsts_loadcb(انکڑے, انکڑے->hsts);
  }

  انکڑے->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  انکڑے->req.rewind_read = FALSE;
  Curl_client_reset(انکڑے);
  Curl_سرخیاں_cleanup(انکڑے);
  أرجع نتیجہ;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(بنية Curl_easy *انکڑے, حرف **url)
{
  بنية connectdata *conn = انکڑے->conn;
  bool retry = FALSE;
  *url = عدم;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  إذا(انکڑے->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    أرجع CURLE_OK;

  إذا(conn->بٹ.reuse &&
     (انکڑے->req.bytecount + انکڑے->req.headerbytecount == 0) &&
     ((!انکڑے->req.no_body && !انکڑے->req.پورا) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#جے_بنا_تعریف CURL_DISABLE_RTSP
     && (انکڑے->سیٹ.rtspreq != RTSPREQ_RECEIVE)
#جے_انت
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  وإلا إذا(انکڑے->state.refused_دھارا &&
          (انکڑے->req.bytecount + انکڑے->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(انکڑے, "REFUSED_STREAM, retrying a fresh connect");
    انکڑے->state.refused_دھارا = FALSE; /* clear again */
    retry = TRUE;
  }
  إذا(retry) {
#تعریف_کرو CONN_MAX_RETRIES 5
    إذا(انکڑے->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(انکڑے, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      انکڑے->state.retrycount = 0;
      أرجع CURLE_SEND_ERROR;
    }
    infof(انکڑے, "Connection died, retrying a fresh connect (retry count: %d)",
          انکڑے->state.retrycount);
    *url = Curl_bufref_dup(&انکڑے->state.url);
    إذا(!*url)
      أرجع CURLE_باہرلا_دا_حافظہ;

    connclose(conn); /* close this connection */
    conn->بٹ.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_سیٹ_rewind(انکڑے, TRUE);
  }
  أرجع CURLE_OK;
}

ساكن فراغ xfer_تیاری(
  بنية Curl_easy *انکڑے,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_ناپ      /* how much to receive, -1 if unknown */
  )
{
  بنية SingleRequest *k = &انکڑے->req;
  بنية connectdata *conn = انکڑے->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(انکڑے));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_ناپ == -1));
  k->ناپ = recv_ناپ;
  k->سرخی = !!conn->scheme->لگاتار->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  إذا(!k->سرخی && (recv_ناپ > 0))
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, recv_ناپ);

  /* we want header and/or body, if neither then do not do this! */
  إذا(conn->scheme->لگاتار->write_resp_hd || !انکڑے->req.no_body) {
    إذا(conn->recv_idx != -1)
      CURL_REQ_سیٹ_RECV(انکڑے);
    إذا(conn->send_idx != -1)
      CURL_REQ_سیٹ_SEND(انکڑے);
  }
  CURL_TRC_M(انکڑے, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

فراغ Curl_xfer_تیاری_nop(بنية Curl_easy *انکڑے)
{
  xfer_تیاری(انکڑے, -1, -1, -1);
}

فراغ Curl_xfer_تیاری_sendrecv(بنية Curl_easy *انکڑے,
                              int8_t sockindex,
                              curl_off_t recv_ناپ)
{
  xfer_تیاری(انکڑے, sockindex, sockindex, recv_ناپ);
}

فراغ Curl_xfer_تیاری_send(بنية Curl_easy *انکڑے,
                          int8_t sockindex)
{
  xfer_تیاری(انکڑے, sockindex, -1, -1);
}

فراغ Curl_xfer_تیاری_recv(بنية Curl_easy *انکڑے,
                          int8_t sockindex,
                          curl_off_t recv_ناپ)
{
  xfer_تیاری(انکڑے, -1, sockindex, recv_ناپ);
}

فراغ Curl_xfer_سیٹ_shutdown(بنية Curl_easy *انکڑے,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (انکڑے->conn->send_idx < 0) || (انکڑے->conn->recv_idx < 0));
  انکڑے->req.shutdown = shutdown;
  انکڑے->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(بنية Curl_easy *انکڑے,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(انکڑے->conn->scheme->لگاتار->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    نتیجہ = انکڑے->conn->scheme->لگاتار->write_resp(انکڑے, buf, blen, is_eos);
  }
  وإلا {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    إذا(blen || is_eos) {
      صحيح cwtype = CLIENTWRITE_BODY;
      إذا(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      نتیجہ = Curl_client_write(انکڑے, cwtype, buf, blen);
    }
  }

  إذا(!نتیجہ && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    انکڑے->req.eos_written = TRUE;
    انکڑے->req.download_پورا = TRUE;
  }
  CURL_TRC_WRITE(انکڑے, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

bool Curl_xfer_write_is_paused(بنية Curl_easy *انکڑے)
{
  أرجع Curl_cwriter_is_paused(انکڑے);
}

CURLcode Curl_xfer_write_resp_hd(بنية Curl_easy *انکڑے,
                                 ثابت حرف *hd0, size_t hdlen, bool is_eos)
{
  إذا(انکڑے->conn->scheme->لگاتار->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    أرجع انکڑے->conn->scheme->لگاتار->write_resp_hd(انکڑے, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  أرجع Curl_xfer_write_resp(انکڑے, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_پورا(بنية Curl_easy *انکڑے, bool premature)
{
  (فراغ)premature;
  أرجع Curl_cw_باہرلا_پورا(انکڑے);
}

bool Curl_xfer_needs_flush(بنية Curl_easy *انکڑے)
{
  أرجع Curl_conn_needs_flush(انکڑے, انکڑے->conn->send_idx);
}

CURLcode Curl_xfer_flush(بنية Curl_easy *انکڑے)
{
  أرجع Curl_conn_flush(انکڑے, انکڑے->conn->send_idx);
}

CURLcode Curl_xfer_send(بنية Curl_easy *انکڑے,
                        ثابت فراغ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->conn);

  نتیجہ = Curl_conn_send(انکڑے, انکڑے->conn->send_idx,
                          buf, blen, eos, pnwritten);
  إذا(نتیجہ == CURLE_AGAIN) {
    نتیجہ = CURLE_OK;
    *pnwritten = 0;
  }
  وإلا إذا(!نتیجہ && *pnwritten)
    انکڑے->info.request_ناپ += *pnwritten;

  DEBUGF(infof(انکڑے, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (صحيح)نتیجہ, *pnwritten));
  أرجع نتیجہ;
}

CURLcode Curl_xfer_recv(بنية Curl_easy *انکڑے,
                        حرف *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->conn);
  DEBUGASSERT(انکڑے->سیٹ.بفر_ناپ > 0);

  إذا(curlx_uitouz(انکڑے->سیٹ.بفر_ناپ) < blen)
    blen = curlx_uitouz(انکڑے->سیٹ.بفر_ناپ);
  أرجع Curl_conn_recv(انکڑے, انکڑے->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(بنية Curl_easy *انکڑے)
{
  Curl_conn_ev_انکڑے_پورا_send(انکڑے);
  أرجع CURLE_OK;
}

bool Curl_xfer_is_blocked(بنية Curl_easy *انکڑے)
{
  bool want_send = CURL_REQ_WANT_SEND(انکڑے);
  bool want_recv = CURL_REQ_WANT_RECV(انکڑے);
  إذا(!want_send)
    أرجع want_recv && Curl_xfer_recv_is_paused(انکڑے);
  وإلا إذا(!want_recv)
    أرجع want_send && Curl_xfer_send_is_paused(انکڑے);
  وإلا
    أرجع Curl_xfer_recv_is_paused(انکڑے) && Curl_xfer_send_is_paused(انکڑے);
}

bool Curl_xfer_send_is_paused(بنية Curl_easy *انکڑے)
{
  أرجع Curl_rlimit_is_blocked(&انکڑے->ترقی.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(بنية Curl_easy *انکڑے)
{
  أرجع Curl_rlimit_is_blocked(&انکڑے->ترقی.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(بنية Curl_easy *انکڑے, bool enable)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_rlimit_بلاک(&انکڑے->ترقی.ul.rlimit, enable, Curl_pgrs_now(انکڑے));
  إذا(!enable && Curl_creader_is_paused(انکڑے))
    نتیجہ = Curl_creader_unpause(انکڑے);
  Curl_pgrsSendPause(انکڑے, enable);
  أرجع نتیجہ;
}

CURLcode Curl_xfer_pause_recv(بنية Curl_easy *انکڑے, bool enable)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_rlimit_بلاک(&انکڑے->ترقی.dl.rlimit, enable, Curl_pgrs_now(انکڑے));
  إذا(!enable && Curl_cwriter_is_paused(انکڑے))
    نتیجہ = Curl_cwriter_unpause(انکڑے);
  Curl_conn_ev_انکڑے_pause(انکڑے, enable);
  Curl_pgrsRecvPause(انکڑے, enable);
  أرجع نتیجہ;
}

bool Curl_xfer_is_secure(بنية Curl_easy *انکڑے)
{
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(انکڑے->conn && انکڑے->conn->بٹ.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(انکڑے->state.origin->scheme->flags & PROTOPT_SSL));
    أرجع FALSE;
  }
#جے_انت
  أرجع (انکڑے->state.origin->scheme->flags & PROTOPT_SSL);
}
