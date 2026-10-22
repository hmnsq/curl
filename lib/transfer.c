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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای
#ضمن <signal.h>

#که_تعریف_شوی HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#د_که_پای

#که_نه_تعریف_شوی HAVE_SOCKET
#تېروتنه "We cannot compile without socket() support"
#د_که_پای

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
حرف *Curl_checkheaders(ثابت بنية Curl_easy *معلومات,
                        ثابت حرف *thisheader,
                        ثابت size_t thislen)
{
  بنية curl_slist *سر;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  لكل(سر = معلومات->سېټ.سرلیکونه; سر; سر = سر->بل) {
    إذا(curl_strnequal(سر->معلومات, thisheader, thislen) &&
       Curl_headersep(سر->معلومات[thislen]))
      أرجع سر->معلومات;
  }

  أرجع عدم;
}
#د_که_پای

ساكن صحيح معلومات_pending(بنية Curl_easy *معلومات, bool rcvd_eagain)
{
  بنية connectdata *conn = معلومات->conn;

  إذا(conn->scheme->protocol & PROTO_FAMILY_FTP)
    أرجع Curl_conn_معلومات_pending(معلومات, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  أرجع (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_معلومات_pending(معلومات, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(بنية Curl_easy *معلومات, time_t timeofdoc)
{
  إذا((timeofdoc == 0) || (معلومات->سېټ.timevalue == 0))
    أرجع TRUE;

  بدل(معلومات->سېټ.timecondition) {
  حالة CURL_TIMECOND_IFMODSINCE:
  افتراضي:
    إذا(timeofdoc <= معلومات->سېټ.timevalue) {
      infof(معلومات, "The requested document is not new enough");
      معلومات->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  حالة CURL_TIMECOND_IFUNMODSINCE:
    إذا(timeofdoc >= معلومات->سېټ.timevalue) {
      infof(معلومات, "The requested document is not old enough");
      معلومات->info.timecond = TRUE;
      أرجع FALSE;
    }
    اكسر;
  }

  أرجع TRUE;
}

ساكن CURLcode xfer_recv_shutdown(بنية Curl_easy *معلومات, bool *بشپړ)
{
  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(معلومات, معلومات->conn->recv_idx, بشپړ);
}

ساكن bool xfer_recv_shutdown_started(بنية Curl_easy *معلومات)
{
  إذا(!معلومات || !معلومات->conn)
    أرجع FALSE;
  أرجع CURL_CONN_دننه_SHUTDOWN(معلومات->conn, معلومات->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(بنية Curl_easy *معلومات, bool *بشپړ)
{
  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_cshutdn_try_once_idx(معلومات, معلومات->conn->send_idx, بشپړ);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ساكن CURLcode xfer_recv_resp(بنية Curl_easy *معلومات,
                               حرف *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode پایله;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  إذا(!eos_reliable && !معلومات->req.سرلیک && معلومات->req.اندازه != -1) {
    blen = curlx_sotouz_range(معلومات->req.اندازه - معلومات->req.bytecount, 0, blen);
  }
  وإلا إذا(xfer_recv_shutdown_started(معلومات)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  إذا(blen) {
    پایله = Curl_xfer_recv(معلومات, buf, blen, pnread);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(*pnread == 0) {
    إذا(معلومات->req.shutdown) {
      bool بشپړ;
      پایله = xfer_recv_shutdown(معلومات, &بشپړ);
      إذا(پایله)
        أرجع پایله;
      إذا(!بشپړ) {
        أرجع CURLE_AGAIN;
      }
    }
    DEBUGF(infof(معلومات, "sendrecv_dl: we are done"));
  }
  أرجع CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ساكن CURLcode sendrecv_dl(بنية Curl_easy *معلومات,
                            بنية SingleRequest *k)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;
  حرف *buf, *xfer_buf;
  size_t blen, xfer_blen;
  صحيح maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, شرح_limited = FALSE;

  پایله = Curl_multi_xfer_buf_borrow(معلومات, &xfer_buf, &xfer_blen);
  إذا(پایله)
    اذهب بهر;

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

    إذا(bytestoread && Curl_rlimit_فعال(&معلومات->پرمختګ.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&معلومات->پرمختګ.dl.rlimit, عدم);
#إذا 0
      DEBUGF(infof(معلومات, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#د_که_پای
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
    پایله = xfer_recv_resp(معلومات, buf, bytestoread, is_multiplex, &blen);
    إذا(پایله) {
      إذا(پایله != CURLE_AGAIN)
        اذهب بهر; /* real error */
      rcvd_eagain = TRUE;
      پایله = CURLE_OK;
      إذا(معلومات->req.download_بشپړ && معلومات->req.no_body &&
         !معلومات->req.resp_trailer) {
        DEBUGF(infof(معلومات, "EAGAIN, download done, no trailer announced, "
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
      پایله = Curl_req_stop_send_recv(معلومات);
      إذا(پایله)
        اذهب بهر;
      إذا(k->eos_written) /* already did write this to client, leave */
        اكسر;
    }

    پایله = Curl_xfer_write_resp(معلومات, buf, blen, is_eos);
    إذا(پایله || معلومات->req.بشپړ)
      اذهب بهر;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    إذا((!is_multiplex && معلومات->req.download_بشپړ) || is_eos) {
      CURL_REQ_CLEAR_RECV(معلومات);
    }
    /* if we stopped receiving, leave the loop */
    إذا(!CURL_REQ_WANT_RECV(معلومات))
      اكسر;

  } طالما(maxloops--);

  إذا(!is_eos && !شرح_limited && CURL_REQ_WANT_RECV(معلومات) &&
     (!rcvd_eagain || معلومات_pending(معلومات, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(معلومات);
    CURL_TRC_M(معلومات, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  إذا(!CURL_REQ_WANT_RECV(معلومات) && CURL_REQ_WANT_SEND(معلومات) &&
     (conn->بټونه.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(معلومات, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(معلومات);
  }

بهر:
  Curl_multi_xfer_buf_release(معلومات, xfer_buf);
  إذا(پایله)
    DEBUGF(infof(معلومات, "sendrecv_dl() -> %d", (صحيح)پایله));
  أرجع پایله;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ساكن CURLcode sendrecv_ul(بنية Curl_easy *معلومات)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_بشپړ_sending(معلومات));

  إذا(!Curl_req_بشپړ_sending(معلومات))
    أرجع Curl_req_send_نور(معلومات);
  أرجع CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(بنية Curl_easy *معلومات)
{
  بنية SingleRequest *k = &معلومات->req;
  ثابت بنية curltime *pnow = عدم;
  CURLcode پایله = CURLE_OK;

  إذا(Curl_xfer_is_blocked(معلومات)) {
    پایله = CURLE_OK;
    اذهب بهر;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  إذا(CURL_REQ_WANT_RECV(معلومات)) {
    پایله = sendrecv_dl(معلومات, k);
    إذا(پایله || معلومات->req.بشپړ)
      اذهب بهر;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  إذا(Curl_req_want_send(معلومات)) {
    پایله = sendrecv_ul(معلومات);
    إذا(پایله)
      اذهب بهر;
  }

  pnow = Curl_pgrs_now(معلومات);
  إذا(CURL_REQ_WANT_IO(معلومات)) {
    إذا(Curl_timeleft_now_ms(معلومات, pnow) < 0) {
      إذا(k->اندازه != -1) {
        failf(معلومات, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE),
              k->bytecount, k->اندازه);
      }
      وإلا {
        failf(معلومات, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE),
              k->bytecount);
      }
      پایله = CURLE_OPERATION_TIMEDOUT;
      اذهب بهر;
    }
  }
  وإلا {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    إذا(!معلومات->req.no_body && (k->اندازه != -1) &&
       (k->bytecount != k->اندازه) && !k->newurl) {
      failf(معلومات, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->اندازه - k->bytecount);
      پایله = CURLE_PARTIAL_FILE;
      اذهب بهر;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  إذا(!CURL_REQ_WANT_IO(معلومات))
    معلومات->req.بشپړ = TRUE;

  پایله = Curl_pgrsCheckX(معلومات, pnow);

بهر:
  إذا(پایله)
    DEBUGF(infof(معلومات, "Curl_sendrecv() -> %d", (صحيح)پایله));
  أرجع پایله;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
فراغ Curl_init_CONNECT(بنية Curl_easy *معلومات)
{
  معلومات->state.fread_func = معلومات->سېټ.fread_func_سېټ;
  معلومات->state.دننه = معلومات->سېټ.دننه_سېټ;
  معلومات->state.upload = (معلومات->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(بنية Curl_easy *معلومات)
{
  CURLcode پایله = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  معلومات->state.retrycount = 0;

  إذا(!CURL_EASY_STR(معلومات, STRING_سېټ_URL) && !معلومات->سېټ.uh) {
    /* we cannot do anything without URL */
    failf(معلومات, "No URL set");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  إذا(معلومات->سېټ.uh) {
    حرف *url = عدم;
    CURLUcode uc;
    uc = curl_url_get(معلومات->سېټ.uh, CURLUPART_URL, &url, 0);
    إذا(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_سېټ(&معلومات->state.url, عدم, 0, عدم);
      failf(معلومات, "No URL set");
      أرجع CURLE_URL_MALFORMAT;
    }
    پایله = CURL_EASY_STR_SETN(معلومات, STRING_سېټ_URL, url);
    إذا(پایله)
      أرجع پایله;
  }

  Curl_bufref_سېټ(&معلومات->state.url, CURL_EASY_STR(معلومات, STRING_سېټ_URL),
                  0, عدم);

  إذا(معلومات->سېټ.postfields && معلومات->سېټ.سېټ_resume_from) {
    /* we cannot */
    failf(معلومات, "cannot mix POSTFIELDS with RESUME_FROM");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  معلومات->state.prefer_ascii = معلومات->سېټ.prefer_ascii;
#که_تعریف_شوی CURL_LIST_ONLY_PROTOCOL
  معلومات->state.list_only = معلومات->سېټ.list_only;
#د_که_پای
  معلومات->state.httpreq = معلومات->سېټ.لاره;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&معلومات->state.initial_origin);
  Curl_peer_unlink(&معلومات->state.origin);
  معلومات->state.غوښتنې = 0;
  معلومات->state.followlocation = 0; /* reset the location-follow counter */
  معلومات->state.this_is_a_follow = FALSE; /* reset this */
  معلومات->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  معلومات->state.errorbuf = FALSE; /* no error has occurred */
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  Curl_http_neg_init(معلومات, &معلومات->state.http_neg);
#د_که_پای
  معلومات->state.authproblem = FALSE;
  معلومات->state.authhost.want = معلومات->سېټ.httpauth;
  معلومات->state.authproxy.want = معلومات->سېټ.proxyauth;
  curlx_safefree(معلومات->info.wouldredirect);
  curlx_safefree(معلومات->info.effective_url);
  Curl_معلومات_لومړیتوب_clear_state(معلومات);
  إذا(معلومات->سېټ.http_auto_referer)
    Curl_bufref_free(&معلومات->state.referer);
  إذا(CURL_EASY_STR(معلومات, STRING_سېټ_REFERER))
    Curl_bufref_سېټ(&معلومات->state.referer,
                    CURL_EASY_STR(معلومات, STRING_سېټ_REFERER), 0, عدم);
  وإلا
    Curl_bufref_free(&معلومات->state.referer);

  إذا(معلومات->state.httpreq == HTTPREQ_PUT)
    معلومات->state.infilesize = معلومات->سېټ.filesize;
  وإلا إذا((معلومات->state.httpreq != HTTPREQ_GET) &&
          (معلومات->state.httpreq != HTTPREQ_سر)) {
    معلومات->state.infilesize = معلومات->سېټ.postfieldsize;
    إذا(معلومات->سېټ.postfields && (معلومات->state.infilesize == -1))
      معلومات->state.infilesize = (curl_off_t)مزي_اوږدوالی(معلومات->سېټ.postfields);
  }
  وإلا
    معلومات->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  پایله = Curl_cookie_loadfiles(معلومات,
                                 معلومات->سېټ.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  إذا(!پایله)
    Curl_cookie_پرلپسې(معلومات); /* activate */

  /* If there is a list of host pairs to deal with */
  إذا(!پایله && معلومات->state.resolve)
    پایله = Curl_loadhostpairs(معلومات);

  إذا(!پایله)
    /* If there is a list of hsts files to read */
    پایله = Curl_hsts_loadfiles(معلومات);

  إذا(!پایله) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    معلومات->state.allow_پورټ = TRUE;

#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    إذا(!معلومات->سېټ.no_signal)
      معلومات->state.prev_signal = إشارة(SIGPIPE, SIG_IGN);
#د_که_پای

    Curl_initinfo(معلومات); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(معلومات);
    Curl_pgrs_پیل(معلومات, عدم);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    معلومات->state.authhost.picked &= معلومات->state.authhost.want;
    معلومات->state.authproxy.picked &= معلومات->state.authproxy.want;

#که_نه_تعریف_شوی CURL_DISABLE_FTP
    معلومات->state.wildcardmatch = معلومات->سېټ.wildcard_enabled;
#که_تعریف_شوی DEBUGBUILD
  إذا(getenv("CURL_DBG_FTP_WILDCARD"))
    معلومات->state.wildcardmatch = TRUE;
#د_که_پای
    إذا(معلومات->state.wildcardmatch) {
      بنية Wildcard_معلومات *wc;
      إذا(!معلومات->wildcard) {
        معلومات->wildcard = curlx_calloc(1, حجم(بنية Wildcard_معلومات));
        إذا(!معلومات->wildcard)
          أرجع CURLE_بهر_د_حافظه;
      }
      wc = معلومات->wildcard;
      إذا(wc->state < CURLWC_INIT) {
        إذا(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->نمونه);
        curlx_safefree(wc->لار);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#د_که_پای
    پایله = Curl_hsts_loadcb(معلومات, معلومات->hsts);
  }

  معلومات->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  معلومات->req.rewind_read = FALSE;
  Curl_client_reset(معلومات);
  Curl_سرلیکونه_cleanup(معلومات);
  أرجع پایله;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(بنية Curl_easy *معلومات, حرف **url)
{
  بنية connectdata *conn = معلومات->conn;
  bool retry = FALSE;
  *url = عدم;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  إذا(معلومات->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    أرجع CURLE_OK;

  إذا(conn->بټونه.reuse &&
     (معلومات->req.bytecount + معلومات->req.headerbytecount == 0) &&
     ((!معلومات->req.no_body && !معلومات->req.بشپړ) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
     && (معلومات->سېټ.rtspreq != RTSPREQ_RECEIVE)
#د_که_پای
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  وإلا إذا(معلومات->state.refused_بهیر &&
          (معلومات->req.bytecount + معلومات->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(معلومات, "REFUSED_STREAM, retrying a fresh connect");
    معلومات->state.refused_بهیر = FALSE; /* clear again */
    retry = TRUE;
  }
  إذا(retry) {
#تعریف_کړه CONN_MAX_RETRIES 5
    إذا(معلومات->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(معلومات, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      معلومات->state.retrycount = 0;
      أرجع CURLE_SEND_ERROR;
    }
    infof(معلومات, "Connection died, retrying a fresh connect (retry count: %d)",
          معلومات->state.retrycount);
    *url = Curl_bufref_dup(&معلومات->state.url);
    إذا(!*url)
      أرجع CURLE_بهر_د_حافظه;

    connclose(conn); /* close this connection */
    conn->بټونه.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_سېټ_rewind(معلومات, TRUE);
  }
  أرجع CURLE_OK;
}

ساكن فراغ xfer_تیاری(
  بنية Curl_easy *معلومات,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_اندازه      /* how much to receive, -1 if unknown */
  )
{
  بنية SingleRequest *k = &معلومات->req;
  بنية connectdata *conn = معلومات->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(معلومات));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_اندازه == -1));
  k->اندازه = recv_اندازه;
  k->سرلیک = !!conn->scheme->پرلپسې->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  إذا(!k->سرلیک && (recv_اندازه > 0))
    Curl_pgrs_سېټ_Download_اندازه(معلومات, recv_اندازه);

  /* we want header and/or body, if neither then do not do this! */
  إذا(conn->scheme->پرلپسې->write_resp_hd || !معلومات->req.no_body) {
    إذا(conn->recv_idx != -1)
      CURL_REQ_سېټ_RECV(معلومات);
    إذا(conn->send_idx != -1)
      CURL_REQ_سېټ_SEND(معلومات);
  }
  CURL_TRC_M(معلومات, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

فراغ Curl_xfer_تیاری_nop(بنية Curl_easy *معلومات)
{
  xfer_تیاری(معلومات, -1, -1, -1);
}

فراغ Curl_xfer_تیاری_sendrecv(بنية Curl_easy *معلومات,
                              int8_t sockindex,
                              curl_off_t recv_اندازه)
{
  xfer_تیاری(معلومات, sockindex, sockindex, recv_اندازه);
}

فراغ Curl_xfer_تیاری_send(بنية Curl_easy *معلومات,
                          int8_t sockindex)
{
  xfer_تیاری(معلومات, sockindex, -1, -1);
}

فراغ Curl_xfer_تیاری_recv(بنية Curl_easy *معلومات,
                          int8_t sockindex,
                          curl_off_t recv_اندازه)
{
  xfer_تیاری(معلومات, -1, sockindex, recv_اندازه);
}

فراغ Curl_xfer_سېټ_shutdown(بنية Curl_easy *معلومات,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (معلومات->conn->send_idx < 0) || (معلومات->conn->recv_idx < 0));
  معلومات->req.shutdown = shutdown;
  معلومات->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(بنية Curl_easy *معلومات,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode پایله = CURLE_OK;

  إذا(معلومات->conn->scheme->پرلپسې->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    پایله = معلومات->conn->scheme->پرلپسې->write_resp(معلومات, buf, blen, is_eos);
  }
  وإلا {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    إذا(blen || is_eos) {
      صحيح cwtype = CLIENTWRITE_BODY;
      إذا(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      پایله = Curl_client_write(معلومات, cwtype, buf, blen);
    }
  }

  إذا(!پایله && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    معلومات->req.eos_written = TRUE;
    معلومات->req.download_بشپړ = TRUE;
  }
  CURL_TRC_WRITE(معلومات, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (صحيح)پایله);
  أرجع پایله;
}

bool Curl_xfer_write_is_paused(بنية Curl_easy *معلومات)
{
  أرجع Curl_cwriter_is_paused(معلومات);
}

CURLcode Curl_xfer_write_resp_hd(بنية Curl_easy *معلومات,
                                 ثابت حرف *hd0, size_t hdlen, bool is_eos)
{
  إذا(معلومات->conn->scheme->پرلپسې->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    أرجع معلومات->conn->scheme->پرلپسې->write_resp_hd(معلومات, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  أرجع Curl_xfer_write_resp(معلومات, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_بشپړ(بنية Curl_easy *معلومات, bool premature)
{
  (فراغ)premature;
  أرجع Curl_cw_بهر_بشپړ(معلومات);
}

bool Curl_xfer_needs_flush(بنية Curl_easy *معلومات)
{
  أرجع Curl_conn_needs_flush(معلومات, معلومات->conn->send_idx);
}

CURLcode Curl_xfer_flush(بنية Curl_easy *معلومات)
{
  أرجع Curl_conn_flush(معلومات, معلومات->conn->send_idx);
}

CURLcode Curl_xfer_send(بنية Curl_easy *معلومات,
                        ثابت فراغ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode پایله;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);

  پایله = Curl_conn_send(معلومات, معلومات->conn->send_idx,
                          buf, blen, eos, pnwritten);
  إذا(پایله == CURLE_AGAIN) {
    پایله = CURLE_OK;
    *pnwritten = 0;
  }
  وإلا إذا(!پایله && *pnwritten)
    معلومات->info.request_اندازه += *pnwritten;

  DEBUGF(infof(معلومات, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (صحيح)پایله, *pnwritten));
  أرجع پایله;
}

CURLcode Curl_xfer_recv(بنية Curl_easy *معلومات,
                        حرف *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  DEBUGASSERT(معلومات->سېټ.بفر_اندازه > 0);

  إذا(curlx_uitouz(معلومات->سېټ.بفر_اندازه) < blen)
    blen = curlx_uitouz(معلومات->سېټ.بفر_اندازه);
  أرجع Curl_conn_recv(معلومات, معلومات->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(بنية Curl_easy *معلومات)
{
  Curl_conn_ev_معلومات_بشپړ_send(معلومات);
  أرجع CURLE_OK;
}

bool Curl_xfer_is_blocked(بنية Curl_easy *معلومات)
{
  bool want_send = CURL_REQ_WANT_SEND(معلومات);
  bool want_recv = CURL_REQ_WANT_RECV(معلومات);
  إذا(!want_send)
    أرجع want_recv && Curl_xfer_recv_is_paused(معلومات);
  وإلا إذا(!want_recv)
    أرجع want_send && Curl_xfer_send_is_paused(معلومات);
  وإلا
    أرجع Curl_xfer_recv_is_paused(معلومات) && Curl_xfer_send_is_paused(معلومات);
}

bool Curl_xfer_send_is_paused(بنية Curl_easy *معلومات)
{
  أرجع Curl_rlimit_is_blocked(&معلومات->پرمختګ.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(بنية Curl_easy *معلومات)
{
  أرجع Curl_rlimit_is_blocked(&معلومات->پرمختګ.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(بنية Curl_easy *معلومات, bool enable)
{
  CURLcode پایله = CURLE_OK;
  Curl_rlimit_بلاک(&معلومات->پرمختګ.ul.rlimit, enable, Curl_pgrs_now(معلومات));
  إذا(!enable && Curl_creader_is_paused(معلومات))
    پایله = Curl_creader_unpause(معلومات);
  Curl_pgrsSendPause(معلومات, enable);
  أرجع پایله;
}

CURLcode Curl_xfer_pause_recv(بنية Curl_easy *معلومات, bool enable)
{
  CURLcode پایله = CURLE_OK;
  Curl_rlimit_بلاک(&معلومات->پرمختګ.dl.rlimit, enable, Curl_pgrs_now(معلومات));
  إذا(!enable && Curl_cwriter_is_paused(معلومات))
    پایله = Curl_cwriter_unpause(معلومات);
  Curl_conn_ev_معلومات_pause(معلومات, enable);
  Curl_pgrsRecvPause(معلومات, enable);
  أرجع پایله;
}

bool Curl_xfer_is_secure(بنية Curl_easy *معلومات)
{
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  إذا(معلومات->conn && معلومات->conn->بټونه.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(معلومات->state.origin->scheme->flags & PROTOPT_SSL));
    أرجع FALSE;
  }
#د_که_پای
  أرجع (معلومات->state.origin->scheme->flags & PROTOPT_SSL);
}
