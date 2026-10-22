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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#ضمن <signal.h>

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#پایان_اگر

#اگر_تعریف_نشده HAVE_SOCKET
#خطا "We cannot compile without socket() support"
#پایان_اگر

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
حرف *Curl_checkheaders(ثابت بنية Curl_easy *داده,
                        ثابت حرف *thisheader,
                        ثابت size_t thislen)
{
  بنية curl_slist *سر;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  لكل(سر = داده->مجموعه.سرآیندها; سر; سر = سر->بعدی) {
    إذا(curl_strnequal(سر->داده, thisheader, thislen) &&
       Curl_headersep(سر->داده[thislen]))
      أرجع سر->داده;
  }

  أرجع عدم;
}
#پایان_اگر

ساكن صحيح داده_pending(بنية Curl_easy *داده, bool rcvd_eagain)
{
  بنية connectdata *conn = داده->conn;

  إذا(conn->scheme->protocol & PROTO_FAMILY_FTP)
    أرجع Curl_conn_داده_pending(داده, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  أرجع (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_داده_pending(داده, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(بنية Curl_easy *داده, time_t timeofdoc)
{
  إذا((timeofdoc == 0) || (داده->مجموعه.timevalue == 0))
    أرجع TRUE;

  بدل(داده->مجموعه.timecondition) {
  حالة CURL_TIMECOND_IFMODSINCE:
  افتراضي:
    إذا(timeofdoc <= داده->مجموعه.timevalue) {
      infof(داده, "The requested document is not new enough");
      داده->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  حالة CURL_TIMECOND_IFUNMODSINCE:
    إذا(timeofdoc >= داده->مجموعه.timevalue) {
      infof(داده, "The requested document is not old enough");
      داده->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  }

  أرجع TRUE;
}

ساكن CURLcode xfer_recv_shutdown(بنية Curl_easy *داده, bool *انجامشده)
{
  إذا(!داده || !داده->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(داده, داده->conn->recv_idx, انجامشده);
}

ساكن bool xfer_recv_shutdown_started(بنية Curl_easy *داده)
{
  إذا(!داده || !داده->conn)
    أرجع FALSE;
  أرجع CURL_CONN_درون_SHUTDOWN(داده->conn, داده->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(بنية Curl_easy *داده, bool *انجامشده)
{
  إذا(!داده || !داده->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(داده, داده->conn->send_idx, انجامشده);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ساكن CURLcode xfer_recv_resp(بنية Curl_easy *داده,
                               حرف *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode نتیجه;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  إذا(!eos_reliable && !داده->req.سرآیند && داده->req.اندازه != -1) {
    blen = curlx_sotouz_range(داده->req.اندازه - داده->req.bytecount, 0, blen);
  }
  وإلا إذا(xfer_recv_shutdown_started(داده)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  إذا(blen) {
    نتیجه = Curl_xfer_recv(داده, buf, blen, pnread);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(*pnread == 0) {
    إذا(داده->req.shutdown) {
      bool انجامشده;
      نتیجه = xfer_recv_shutdown(داده, &انجامشده);
      إذا(نتیجه)
        أرجع نتیجه;
      إذا(!انجامشده) {
        أرجع CURLE_AGAIN;
      }
    }
    DEBUGF(infof(داده, "sendrecv_dl: we are done"));
  }
  أرجع CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ساكن CURLcode sendrecv_dl(بنية Curl_easy *داده,
                            بنية SingleRequest *k)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;
  حرف *buf, *xfer_buf;
  size_t blen, xfer_blen;
  صحيح maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, نرخ_limited = FALSE;

  نتیجه = Curl_multi_xfer_buf_borrow(داده, &xfer_buf, &xfer_blen);
  إذا(نتیجه)
    اذهب بیرون;

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

    إذا(bytestoread && Curl_rlimit_فعال(&داده->پیشرفت.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&داده->پیشرفت.dl.rlimit, عدم);
#إذا 0
      DEBUGF(infof(داده, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#پایان_اگر
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      إذا(dl_avail <= 0) {
        نرخ_limited = TRUE;
        اكسر;
      }
      إذا(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    نتیجه = xfer_recv_resp(داده, buf, bytestoread, is_multiplex, &blen);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN)
        اذهب بیرون; /* real error */
      rcvd_eagain = TRUE;
      نتیجه = CURLE_OK;
      إذا(داده->req.download_انجامشده && داده->req.no_body &&
         !داده->req.resp_trailer) {
        DEBUGF(infof(داده, "EAGAIN, download done, no trailer announced, "
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
      نتیجه = Curl_req_stop_send_recv(داده);
      إذا(نتیجه)
        اذهب بیرون;
      إذا(k->eos_written) /* already did write this to client, leave */
        اكسر;
    }

    نتیجه = Curl_xfer_write_resp(داده, buf, blen, is_eos);
    إذا(نتیجه || داده->req.انجامشده)
      اذهب بیرون;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    إذا((!is_multiplex && داده->req.download_انجامشده) || is_eos) {
      CURL_REQ_CLEAR_RECV(داده);
    }
    /* if we stopped receiving, leave the loop */
    إذا(!CURL_REQ_WANT_RECV(داده))
      اكسر;

  } طالما(maxloops--);

  إذا(!is_eos && !نرخ_limited && CURL_REQ_WANT_RECV(داده) &&
     (!rcvd_eagain || داده_pending(داده, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(داده);
    CURL_TRC_M(داده, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  إذا(!CURL_REQ_WANT_RECV(داده) && CURL_REQ_WANT_SEND(داده) &&
     (conn->بیتها.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(داده, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(داده);
  }

بیرون:
  Curl_multi_xfer_buf_release(داده, xfer_buf);
  إذا(نتیجه)
    DEBUGF(infof(داده, "sendrecv_dl() -> %d", (صحيح)نتیجه));
  أرجع نتیجه;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ساكن CURLcode sendrecv_ul(بنية Curl_easy *داده)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_انجامشده_sending(داده));

  إذا(!Curl_req_انجامشده_sending(داده))
    أرجع Curl_req_send_بیشتر(داده);
  أرجع CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(بنية Curl_easy *داده)
{
  بنية SingleRequest *k = &داده->req;
  ثابت بنية curltime *pnow = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(Curl_xfer_is_blocked(داده)) {
    نتیجه = CURLE_OK;
    اذهب بیرون;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  إذا(CURL_REQ_WANT_RECV(داده)) {
    نتیجه = sendrecv_dl(داده, k);
    إذا(نتیجه || داده->req.انجامشده)
      اذهب بیرون;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  إذا(Curl_req_want_send(داده)) {
    نتیجه = sendrecv_ul(داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  pnow = Curl_pgrs_now(داده);
  إذا(CURL_REQ_WANT_IO(داده)) {
    إذا(Curl_timeleft_now_ms(داده, pnow) < 0) {
      إذا(k->اندازه != -1) {
        failf(داده, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE),
              k->bytecount, k->اندازه);
      }
      وإلا {
        failf(داده, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE),
              k->bytecount);
      }
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اذهب بیرون;
    }
  }
  وإلا {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    إذا(!داده->req.no_body && (k->اندازه != -1) &&
       (k->bytecount != k->اندازه) && !k->newurl) {
      failf(داده, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->اندازه - k->bytecount);
      نتیجه = CURLE_PARTIAL_FILE;
      اذهب بیرون;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  إذا(!CURL_REQ_WANT_IO(داده))
    داده->req.انجامشده = TRUE;

  نتیجه = Curl_pgrsCheckX(داده, pnow);

بیرون:
  إذا(نتیجه)
    DEBUGF(infof(داده, "Curl_sendrecv() -> %d", (صحيح)نتیجه));
  أرجع نتیجه;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
فراغ Curl_init_CONNECT(بنية Curl_easy *داده)
{
  داده->state.fread_func = داده->مجموعه.fread_func_مجموعه;
  داده->state.درون = داده->مجموعه.درون_مجموعه;
  داده->state.upload = (داده->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  داده->state.retrycount = 0;

  إذا(!CURL_EASY_STR(داده, STRING_مجموعه_URL) && !داده->مجموعه.uh) {
    /* we cannot do anything without URL */
    failf(داده, "No URL set");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  إذا(داده->مجموعه.uh) {
    حرف *url = عدم;
    CURLUcode uc;
    uc = curl_url_get(داده->مجموعه.uh, CURLUPART_URL, &url, 0);
    إذا(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_مجموعه(&داده->state.url, عدم, 0, عدم);
      failf(داده, "No URL set");
      أرجع CURLE_URL_MALFORMAT;
    }
    نتیجه = CURL_EASY_STR_SETN(داده, STRING_مجموعه_URL, url);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  Curl_bufref_مجموعه(&داده->state.url, CURL_EASY_STR(داده, STRING_مجموعه_URL),
                  0, عدم);

  إذا(داده->مجموعه.postfields && داده->مجموعه.مجموعه_resume_from) {
    /* we cannot */
    failf(داده, "cannot mix POSTFIELDS with RESUME_FROM");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  داده->state.prefer_ascii = داده->مجموعه.prefer_ascii;
#اگر_تعریف_شده CURL_LIST_ONLY_PROTOCOL
  داده->state.list_only = داده->مجموعه.list_only;
#پایان_اگر
  داده->state.httpreq = داده->مجموعه.روش;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&داده->state.initial_origin);
  Curl_peer_unlink(&داده->state.origin);
  داده->state.درخواستها = 0;
  داده->state.followlocation = 0; /* reset the location-follow counter */
  داده->state.this_is_a_follow = FALSE; /* reset this */
  داده->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  داده->state.errorbuf = FALSE; /* no error has occurred */
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  Curl_http_neg_init(داده, &داده->state.http_neg);
#پایان_اگر
  داده->state.authproblem = FALSE;
  داده->state.authhost.want = داده->مجموعه.httpauth;
  داده->state.authproxy.want = داده->مجموعه.proxyauth;
  curlx_safefree(داده->info.wouldredirect);
  curlx_safefree(داده->info.effective_url);
  Curl_داده_اولویت_clear_state(داده);
  إذا(داده->مجموعه.http_auto_referer)
    Curl_bufref_free(&داده->state.referer);
  إذا(CURL_EASY_STR(داده, STRING_مجموعه_REFERER))
    Curl_bufref_مجموعه(&داده->state.referer,
                    CURL_EASY_STR(داده, STRING_مجموعه_REFERER), 0, عدم);
  وإلا
    Curl_bufref_free(&داده->state.referer);

  إذا(داده->state.httpreq == HTTPREQ_PUT)
    داده->state.infilesize = داده->مجموعه.filesize;
  وإلا إذا((داده->state.httpreq != HTTPREQ_GET) &&
          (داده->state.httpreq != HTTPREQ_سر)) {
    داده->state.infilesize = داده->مجموعه.postfieldsize;
    إذا(داده->مجموعه.postfields && (داده->state.infilesize == -1))
      داده->state.infilesize = (curl_off_t)طول_رشته(داده->مجموعه.postfields);
  }
  وإلا
    داده->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  نتیجه = Curl_cookie_loadfiles(داده,
                                 داده->مجموعه.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  إذا(!نتیجه)
    Curl_cookie_پیاپی(داده); /* activate */

  /* If there is a list of host pairs to deal with */
  إذا(!نتیجه && داده->state.resolve)
    نتیجه = Curl_loadhostpairs(داده);

  إذا(!نتیجه)
    /* If there is a list of hsts files to read */
    نتیجه = Curl_hsts_loadfiles(داده);

  إذا(!نتیجه) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    داده->state.allow_پورت = TRUE;

#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    إذا(!داده->مجموعه.no_signal)
      داده->state.prev_signal = إشارة(SIGPIPE, SIG_IGN);
#پایان_اگر

    Curl_initinfo(داده); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(داده);
    Curl_pgrs_شروع(داده, عدم);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    داده->state.authhost.picked &= داده->state.authhost.want;
    داده->state.authproxy.picked &= داده->state.authproxy.want;

#اگر_تعریف_نشده CURL_DISABLE_FTP
    داده->state.wildcardmatch = داده->مجموعه.wildcard_enabled;
#اگر_تعریف_شده DEBUGBUILD
  إذا(getenv("CURL_DBG_FTP_WILDCARD"))
    داده->state.wildcardmatch = TRUE;
#پایان_اگر
    إذا(داده->state.wildcardmatch) {
      بنية Wildcard_داده *wc;
      إذا(!داده->wildcard) {
        داده->wildcard = curlx_calloc(1, حجم(بنية Wildcard_داده));
        إذا(!داده->wildcard)
          أرجع CURLE_بیرون_از_حافظه;
      }
      wc = داده->wildcard;
      إذا(wc->state < CURLWC_INIT) {
        إذا(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->الگو);
        curlx_safefree(wc->مسیر);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#پایان_اگر
    نتیجه = Curl_hsts_loadcb(داده, داده->hsts);
  }

  داده->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  داده->req.rewind_read = FALSE;
  Curl_client_reset(داده);
  Curl_سرآیندها_cleanup(داده);
  أرجع نتیجه;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(بنية Curl_easy *داده, حرف **url)
{
  بنية connectdata *conn = داده->conn;
  bool retry = FALSE;
  *url = عدم;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  إذا(داده->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    أرجع CURLE_OK;

  إذا(conn->بیتها.reuse &&
     (داده->req.bytecount + داده->req.headerbytecount == 0) &&
     ((!داده->req.no_body && !داده->req.انجامشده) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#اگر_تعریف_نشده CURL_DISABLE_RTSP
     && (داده->مجموعه.rtspreq != RTSPREQ_RECEIVE)
#پایان_اگر
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  وإلا إذا(داده->state.refused_جریان &&
          (داده->req.bytecount + داده->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(داده, "REFUSED_STREAM, retrying a fresh connect");
    داده->state.refused_جریان = FALSE; /* clear again */
    retry = TRUE;
  }
  إذا(retry) {
#تعریف_کن CONN_MAX_RETRIES 5
    إذا(داده->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(داده, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      داده->state.retrycount = 0;
      أرجع CURLE_SEND_ERROR;
    }
    infof(داده, "Connection died, retrying a fresh connect (retry count: %d)",
          داده->state.retrycount);
    *url = Curl_bufref_dup(&داده->state.url);
    إذا(!*url)
      أرجع CURLE_بیرون_از_حافظه;

    connclose(conn); /* close this connection */
    conn->بیتها.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_مجموعه_rewind(داده, TRUE);
  }
  أرجع CURLE_OK;
}

ساكن فراغ xfer_راهاندازی(
  بنية Curl_easy *داده,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_اندازه      /* how much to receive, -1 if unknown */
  )
{
  بنية SingleRequest *k = &داده->req;
  بنية connectdata *conn = داده->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(داده));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_اندازه == -1));
  k->اندازه = recv_اندازه;
  k->سرآیند = !!conn->scheme->پیاپی->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  إذا(!k->سرآیند && (recv_اندازه > 0))
    Curl_pgrs_مجموعه_Download_اندازه(داده, recv_اندازه);

  /* we want header and/or body, if neither then do not do this! */
  إذا(conn->scheme->پیاپی->write_resp_hd || !داده->req.no_body) {
    إذا(conn->recv_idx != -1)
      CURL_REQ_مجموعه_RECV(داده);
    إذا(conn->send_idx != -1)
      CURL_REQ_مجموعه_SEND(داده);
  }
  CURL_TRC_M(داده, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

فراغ Curl_xfer_راهاندازی_nop(بنية Curl_easy *داده)
{
  xfer_راهاندازی(داده, -1, -1, -1);
}

فراغ Curl_xfer_راهاندازی_sendrecv(بنية Curl_easy *داده,
                              int8_t sockindex,
                              curl_off_t recv_اندازه)
{
  xfer_راهاندازی(داده, sockindex, sockindex, recv_اندازه);
}

فراغ Curl_xfer_راهاندازی_send(بنية Curl_easy *داده,
                          int8_t sockindex)
{
  xfer_راهاندازی(داده, sockindex, -1, -1);
}

فراغ Curl_xfer_راهاندازی_recv(بنية Curl_easy *داده,
                          int8_t sockindex,
                          curl_off_t recv_اندازه)
{
  xfer_راهاندازی(داده, -1, sockindex, recv_اندازه);
}

فراغ Curl_xfer_مجموعه_shutdown(بنية Curl_easy *داده,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (داده->conn->send_idx < 0) || (داده->conn->recv_idx < 0));
  داده->req.shutdown = shutdown;
  داده->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(بنية Curl_easy *داده,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(داده->conn->scheme->پیاپی->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    نتیجه = داده->conn->scheme->پیاپی->write_resp(داده, buf, blen, is_eos);
  }
  وإلا {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    إذا(blen || is_eos) {
      صحيح cwtype = CLIENTWRITE_BODY;
      إذا(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      نتیجه = Curl_client_write(داده, cwtype, buf, blen);
    }
  }

  إذا(!نتیجه && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    داده->req.eos_written = TRUE;
    داده->req.download_انجامشده = TRUE;
  }
  CURL_TRC_WRITE(داده, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (صحيح)نتیجه);
  أرجع نتیجه;
}

bool Curl_xfer_write_is_paused(بنية Curl_easy *داده)
{
  أرجع Curl_cwriter_is_paused(داده);
}

CURLcode Curl_xfer_write_resp_hd(بنية Curl_easy *داده,
                                 ثابت حرف *hd0, size_t hdlen, bool is_eos)
{
  إذا(داده->conn->scheme->پیاپی->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    أرجع داده->conn->scheme->پیاپی->write_resp_hd(داده, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  أرجع Curl_xfer_write_resp(داده, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_انجامشده(بنية Curl_easy *داده, bool premature)
{
  (فراغ)premature;
  أرجع Curl_cw_بیرون_انجامشده(داده);
}

bool Curl_xfer_needs_flush(بنية Curl_easy *داده)
{
  أرجع Curl_conn_needs_flush(داده, داده->conn->send_idx);
}

CURLcode Curl_xfer_flush(بنية Curl_easy *داده)
{
  أرجع Curl_conn_flush(داده, داده->conn->send_idx);
}

CURLcode Curl_xfer_send(بنية Curl_easy *داده,
                        ثابت فراغ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);

  نتیجه = Curl_conn_send(داده, داده->conn->send_idx,
                          buf, blen, eos, pnwritten);
  إذا(نتیجه == CURLE_AGAIN) {
    نتیجه = CURLE_OK;
    *pnwritten = 0;
  }
  وإلا إذا(!نتیجه && *pnwritten)
    داده->info.request_اندازه += *pnwritten;

  DEBUGF(infof(داده, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (صحيح)نتیجه, *pnwritten));
  أرجع نتیجه;
}

CURLcode Curl_xfer_recv(بنية Curl_easy *داده,
                        حرف *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  DEBUGASSERT(داده->مجموعه.میانگیر_اندازه > 0);

  إذا(curlx_uitouz(داده->مجموعه.میانگیر_اندازه) < blen)
    blen = curlx_uitouz(داده->مجموعه.میانگیر_اندازه);
  أرجع Curl_conn_recv(داده, داده->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(بنية Curl_easy *داده)
{
  Curl_conn_ev_داده_انجامشده_send(داده);
  أرجع CURLE_OK;
}

bool Curl_xfer_is_blocked(بنية Curl_easy *داده)
{
  bool want_send = CURL_REQ_WANT_SEND(داده);
  bool want_recv = CURL_REQ_WANT_RECV(داده);
  إذا(!want_send)
    أرجع want_recv && Curl_xfer_recv_is_paused(داده);
  وإلا إذا(!want_recv)
    أرجع want_send && Curl_xfer_send_is_paused(داده);
  وإلا
    أرجع Curl_xfer_recv_is_paused(داده) && Curl_xfer_send_is_paused(داده);
}

bool Curl_xfer_send_is_paused(بنية Curl_easy *داده)
{
  أرجع Curl_rlimit_is_blocked(&داده->پیشرفت.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(بنية Curl_easy *داده)
{
  أرجع Curl_rlimit_is_blocked(&داده->پیشرفت.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(بنية Curl_easy *داده, bool enable)
{
  CURLcode نتیجه = CURLE_OK;
  Curl_rlimit_بلوک(&داده->پیشرفت.ul.rlimit, enable, Curl_pgrs_now(داده));
  إذا(!enable && Curl_creader_is_paused(داده))
    نتیجه = Curl_creader_unpause(داده);
  Curl_pgrsSendPause(داده, enable);
  أرجع نتیجه;
}

CURLcode Curl_xfer_pause_recv(بنية Curl_easy *داده, bool enable)
{
  CURLcode نتیجه = CURLE_OK;
  Curl_rlimit_بلوک(&داده->پیشرفت.dl.rlimit, enable, Curl_pgrs_now(داده));
  إذا(!enable && Curl_cwriter_is_paused(داده))
    نتیجه = Curl_cwriter_unpause(داده);
  Curl_conn_ev_داده_pause(داده, enable);
  Curl_pgrsRecvPause(داده, enable);
  أرجع نتیجه;
}

bool Curl_xfer_is_secure(بنية Curl_easy *داده)
{
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(داده->conn && داده->conn->بیتها.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(داده->state.origin->scheme->flags & PROTOPT_SSL));
    أرجع FALSE;
  }
#پایان_اگر
  أرجع (داده->state.origin->scheme->flags & PROTOPT_SSL);
}
