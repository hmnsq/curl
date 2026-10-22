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
#নোচেদ্যদি defined(HAVE_UNISTD_H)
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
বর্ণ *Curl_checkheaders(ধ্রুব সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *thisheader,
                        ধ্রুব size_t thislen)
{
  সংরচনা curl_slist *head;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  প্রতি(head = data->set.headers; head; head = head->next) {
    যদি(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      প্রত্যাগম head->data;
  }

  প্রত্যাগম রিক্ত;
}
#endif

স্থির পূর্ণাঙ্ক data_pending(সংরচনা Curl_easy *data, bool rcvd_eagain)
{
  সংরচনা connectdata *conn = data->conn;

  যদি(conn->scheme->protocol & PROTO_FAMILY_FTP)
    প্রত্যাগম Curl_conn_data_pending(data, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  প্রত্যাগম (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_data_pending(data, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(সংরচনা Curl_easy *data, time_t timeofdoc)
{
  যদি((timeofdoc == 0) || (data->set.timevalue == 0))
    প্রত্যাগম TRUE;

  বিকল্প(data->set.timecondition) {
  পক্ষ CURL_TIMECOND_IFMODSINCE:
  স্বভাব:
    যদি(timeofdoc <= data->set.timevalue) {
      infof(data, "The requested document is not new enough");
      data->info.timecond = TRUE;
      প্রত্যাগম FALSE;
    }
    ভঙ্গ;
  পক্ষ CURL_TIMECOND_IFUNMODSINCE:
    যদি(timeofdoc >= data->set.timevalue) {
      infof(data, "The requested document is not old enough");
      data->info.timecond = TRUE;
      প্রত্যাগম FALSE;
    }
    ভঙ্গ;
  }

  প্রত্যাগম TRUE;
}

স্থির CURLcode xfer_recv_shutdown(সংরচনা Curl_easy *data, bool *done)
{
  যদি(!data || !data->conn)
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_cshutdn_try_once_idx(data, data->conn->recv_idx, done);
}

স্থির bool xfer_recv_shutdown_started(সংরচনা Curl_easy *data)
{
  যদি(!data || !data->conn)
    প্রত্যাগম FALSE;
  প্রত্যাগম CURL_CONN_IN_SHUTDOWN(data->conn, data->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(সংরচনা Curl_easy *data, bool *done)
{
  যদি(!data || !data->conn)
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_cshutdn_try_once_idx(data, data->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
স্থির CURLcode xfer_recv_resp(সংরচনা Curl_easy *data,
                               বর্ণ *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode result;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  যদি(!eos_reliable && !data->req.header && data->req.size != -1) {
    blen = curlx_sotouz_range(data->req.size - data->req.bytecount, 0, blen);
  }
  অন্যথা যদি(xfer_recv_shutdown_started(data)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  যদি(blen) {
    result = Curl_xfer_recv(data, buf, blen, pnread);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(*pnread == 0) {
    যদি(data->req.shutdown) {
      bool done;
      result = xfer_recv_shutdown(data, &done);
      যদি(result)
        প্রত্যাগম result;
      যদি(!done) {
        প্রত্যাগম CURLE_AGAIN;
      }
    }
    DEBUGF(infof(data, "sendrecv_dl: we are done"));
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
স্থির CURLcode sendrecv_dl(সংরচনা Curl_easy *data,
                            সংরচনা SingleRequest *k)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  বর্ণ *buf, *xfer_buf;
  size_t blen, xfer_blen;
  পূর্ণাঙ্ক maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  যদি(result)
    গচ্ছ out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  কুরু {
    size_t bytestoread;

    যদি(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    যদি(bytestoread && Curl_rlimit_active(&data->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&data->progress.dl.rlimit, রিক্ত);
#যদি 0
      DEBUGF(infof(data, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      যদি(dl_avail <= 0) {
        rate_limited = TRUE;
        ভঙ্গ;
      }
      যদি(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    result = xfer_recv_resp(data, buf, bytestoread, is_multiplex, &blen);
    যদি(result) {
      যদি(result != CURLE_AGAIN)
        গচ্ছ out; /* real error */
      rcvd_eagain = TRUE;
      result = CURLE_OK;
      যদি(data->req.download_done && data->req.no_body &&
         !data->req.resp_trailer) {
        DEBUGF(infof(data, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      অন্যথা
        ভঙ্গ; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    যদি(!blen) {
      result = Curl_req_stop_send_recv(data);
      যদি(result)
        গচ্ছ out;
      যদি(k->eos_written) /* already did write this to client, leave */
        ভঙ্গ;
    }

    result = Curl_xfer_write_resp(data, buf, blen, is_eos);
    যদি(result || data->req.done)
      গচ্ছ out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    যদি((!is_multiplex && data->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(data);
    }
    /* if we stopped receiving, leave the loop */
    যদি(!CURL_REQ_WANT_RECV(data))
      ভঙ্গ;

  } যাবত্(maxloops--);

  যদি(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(data) &&
     (!rcvd_eagain || data_pending(data, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(data);
    CURL_TRC_M(data, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  যদি(!CURL_REQ_WANT_RECV(data) && CURL_REQ_WANT_SEND(data) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(data, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(data);
  }

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  যদি(result)
    DEBUGF(infof(data, "sendrecv_dl() -> %d", (পূর্ণাঙ্ক)result));
  প্রত্যাগম result;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
স্থির CURLcode sendrecv_ul(সংরচনা Curl_easy *data)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(data));

  যদি(!Curl_req_done_sending(data))
    প্রত্যাগম Curl_req_send_more(data);
  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(সংরচনা Curl_easy *data)
{
  সংরচনা SingleRequest *k = &data->req;
  ধ্রুব সংরচনা curltime *pnow = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(Curl_xfer_is_blocked(data)) {
    result = CURLE_OK;
    গচ্ছ out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  যদি(CURL_REQ_WANT_RECV(data)) {
    result = sendrecv_dl(data, k);
    যদি(result || data->req.done)
      গচ্ছ out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  যদি(Curl_req_want_send(data)) {
    result = sendrecv_ul(data);
    যদি(result)
      গচ্ছ out;
  }

  pnow = Curl_pgrs_now(data);
  যদি(CURL_REQ_WANT_IO(data)) {
    যদি(Curl_timeleft_now_ms(data, pnow) < 0) {
      যদি(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE),
              k->bytecount, k->size);
      }
      অন্যথা {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE),
              k->bytecount);
      }
      result = CURLE_OPERATION_TIMEDOUT;
      গচ্ছ out;
    }
  }
  অন্যথা {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    যদি(!data->req.no_body && (k->size != -1) &&
       (k->bytecount != k->size) && !k->newurl) {
      failf(data, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      result = CURLE_PARTIAL_FILE;
      গচ্ছ out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  যদি(!CURL_REQ_WANT_IO(data))
    data->req.done = TRUE;

  result = Curl_pgrsCheckX(data, pnow);

out:
  যদি(result)
    DEBUGF(infof(data, "Curl_sendrecv() -> %d", (পূর্ণাঙ্ক)result));
  প্রত্যাগম result;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
শূন্য Curl_init_CONNECT(সংরচনা Curl_easy *data)
{
  data->state.fread_func = data->set.fread_func_set;
  data->state.in = data->set.in_set;
  data->state.upload = (data->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  data->state.retrycount = 0;

  যদি(!CURL_EASY_STR(data, STRING_SET_URL) && !data->set.uh) {
    /* we cannot do anything without URL */
    failf(data, "No URL set");
    প্রত্যাগম CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  যদি(data->set.uh) {
    বর্ণ *url = রিক্ত;
    CURLUcode uc;
    uc = curl_url_get(data->set.uh, CURLUPART_URL, &url, 0);
    যদি(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&data->state.url, রিক্ত, 0, রিক্ত);
      failf(data, "No URL set");
      প্রত্যাগম CURLE_URL_MALFORMAT;
    }
    result = CURL_EASY_STR_SETN(data, STRING_SET_URL, url);
    যদি(result)
      প্রত্যাগম result;
  }

  Curl_bufref_set(&data->state.url, CURL_EASY_STR(data, STRING_SET_URL),
                  0, রিক্ত);

  যদি(data->set.postfields && data->set.set_resume_from) {
    /* we cannot */
    failf(data, "cannot mix POSTFIELDS with RESUME_FROM");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  data->state.prefer_ascii = data->set.prefer_ascii;
#ifdef CURL_LIST_ONLY_PROTOCOL
  data->state.list_only = data->set.list_only;
#endif
  data->state.httpreq = data->set.method;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&data->state.initial_origin);
  Curl_peer_unlink(&data->state.origin);
  data->state.requests = 0;
  data->state.followlocation = 0; /* reset the location-follow counter */
  data->state.this_is_a_follow = FALSE; /* reset this */
  data->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  data->state.errorbuf = FALSE; /* no error has occurred */
#ifndef CURL_DISABLE_HTTP
  Curl_http_neg_init(data, &data->state.http_neg);
#endif
  data->state.authproblem = FALSE;
  data->state.authhost.want = data->set.httpauth;
  data->state.authproxy.want = data->set.proxyauth;
  curlx_safefree(data->info.wouldredirect);
  curlx_safefree(data->info.effective_url);
  Curl_data_priority_clear_state(data);
  যদি(data->set.http_auto_referer)
    Curl_bufref_free(&data->state.referer);
  যদি(CURL_EASY_STR(data, STRING_SET_REFERER))
    Curl_bufref_set(&data->state.referer,
                    CURL_EASY_STR(data, STRING_SET_REFERER), 0, রিক্ত);
  অন্যথা
    Curl_bufref_free(&data->state.referer);

  যদি(data->state.httpreq == HTTPREQ_PUT)
    data->state.infilesize = data->set.filesize;
  অন্যথা যদি((data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    data->state.infilesize = data->set.postfieldsize;
    যদি(data->set.postfields && (data->state.infilesize == -1))
      data->state.infilesize = (curl_off_t)strlen(data->set.postfields);
  }
  অন্যথা
    data->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  result = Curl_cookie_loadfiles(data,
                                 data->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  যদি(!result)
    Curl_cookie_run(data); /* activate */

  /* If there is a list of host pairs to deal with */
  যদি(!result && data->state.resolve)
    result = Curl_loadhostpairs(data);

  যদি(!result)
    /* If there is a list of hsts files to read */
    result = Curl_hsts_loadfiles(data);

  যদি(!result) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    data->state.allow_port = TRUE;

#যদি defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    যদি(!data->set.no_signal)
      data->state.prev_signal = সঙ্কেত(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(data); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(data);
    Curl_pgrsStart(data, রিক্ত);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    data->state.authhost.picked &= data->state.authhost.want;
    data->state.authproxy.picked &= data->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    data->state.wildcardmatch = data->set.wildcard_enabled;
#ifdef DEBUGBUILD
  যদি(getenv("CURL_DBG_FTP_WILDCARD"))
    data->state.wildcardmatch = TRUE;
#endif
    যদি(data->state.wildcardmatch) {
      সংরচনা WildcardData *wc;
      যদি(!data->wildcard) {
        data->wildcard = curlx_calloc(1, পরিমাণ(সংরচনা WildcardData));
        যদি(!data->wildcard)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
      wc = data->wildcard;
      যদি(wc->state < CURLWC_INIT) {
        যদি(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->pattern);
        curlx_safefree(wc->path);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#endif
    result = Curl_hsts_loadcb(data, data->hsts);
  }

  data->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  data->req.rewind_read = FALSE;
  Curl_client_reset(data);
  Curl_headers_cleanup(data);
  প্রত্যাগম result;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(সংরচনা Curl_easy *data, বর্ণ **url)
{
  সংরচনা connectdata *conn = data->conn;
  bool retry = FALSE;
  *url = রিক্ত;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  যদি(data->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    প্রত্যাগম CURLE_OK;

  যদি(conn->bits.reuse &&
     (data->req.bytecount + data->req.headerbytecount == 0) &&
     ((!data->req.no_body && !data->req.done) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#ifndef CURL_DISABLE_RTSP
     && (data->set.rtspreq != RTSPREQ_RECEIVE)
#endif
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  অন্যথা যদি(data->state.refused_stream &&
          (data->req.bytecount + data->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(data, "REFUSED_STREAM, retrying a fresh connect");
    data->state.refused_stream = FALSE; /* clear again */
    retry = TRUE;
  }
  যদি(retry) {
#define CONN_MAX_RETRIES 5
    যদি(data->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(data, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      data->state.retrycount = 0;
      প্রত্যাগম CURLE_SEND_ERROR;
    }
    infof(data, "Connection died, retrying a fresh connect (retry count: %d)",
          data->state.retrycount);
    *url = Curl_bufref_dup(&data->state.url);
    যদি(!*url)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(data, TRUE);
  }
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য xfer_setup(
  সংরচনা Curl_easy *data,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_size      /* how much to receive, -1 if unknown */
  )
{
  সংরচনা SingleRequest *k = &data->req;
  সংরচনা connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(data));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_size == -1));
  k->size = recv_size;
  k->header = !!conn->scheme->run->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  যদি(!k->header && (recv_size > 0))
    Curl_pgrsSetDownloadSize(data, recv_size);

  /* we want header and/or body, if neither then do not do this! */
  যদি(conn->scheme->run->write_resp_hd || !data->req.no_body) {
    যদি(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(data);
    যদি(conn->send_idx != -1)
      CURL_REQ_SET_SEND(data);
  }
  CURL_TRC_M(data, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

শূন্য Curl_xfer_setup_nop(সংরচনা Curl_easy *data)
{
  xfer_setup(data, -1, -1, -1);
}

শূন্য Curl_xfer_setup_sendrecv(সংরচনা Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size)
{
  xfer_setup(data, sockindex, sockindex, recv_size);
}

শূন্য Curl_xfer_setup_send(সংরচনা Curl_easy *data,
                          int8_t sockindex)
{
  xfer_setup(data, sockindex, -1, -1);
}

শূন্য Curl_xfer_setup_recv(সংরচনা Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size)
{
  xfer_setup(data, -1, sockindex, recv_size);
}

শূন্য Curl_xfer_set_shutdown(সংরচনা Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (data->conn->send_idx < 0) || (data->conn->recv_idx < 0));
  data->req.shutdown = shutdown;
  data->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result = CURLE_OK;

  যদি(data->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    result = data->conn->scheme->run->write_resp(data, buf, blen, is_eos);
  }
  অন্যথা {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    যদি(blen || is_eos) {
      পূর্ণাঙ্ক cwtype = CLIENTWRITE_BODY;
      যদি(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      result = Curl_client_write(data, cwtype, buf, blen);
    }
  }

  যদি(!result && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    data->req.eos_written = TRUE;
    data->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(data, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (পূর্ণাঙ্ক)result);
  প্রত্যাগম result;
}

bool Curl_xfer_write_is_paused(সংরচনা Curl_easy *data)
{
  প্রত্যাগম Curl_cwriter_is_paused(data);
}

CURLcode Curl_xfer_write_resp_hd(সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *hd0, size_t hdlen, bool is_eos)
{
  যদি(data->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    প্রত্যাগম data->conn->scheme->run->write_resp_hd(data, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  প্রত্যাগম Curl_xfer_write_resp(data, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(সংরচনা Curl_easy *data, bool premature)
{
  (শূন্য)premature;
  প্রত্যাগম Curl_cw_out_done(data);
}

bool Curl_xfer_needs_flush(সংরচনা Curl_easy *data)
{
  প্রত্যাগম Curl_conn_needs_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_flush(সংরচনা Curl_easy *data)
{
  প্রত্যাগম Curl_conn_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_send(সংরচনা Curl_easy *data,
                        ধ্রুব শূন্য *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode result;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  result = Curl_conn_send(data, data->conn->send_idx,
                          buf, blen, eos, pnwritten);
  যদি(result == CURLE_AGAIN) {
    result = CURLE_OK;
    *pnwritten = 0;
  }
  অন্যথা যদি(!result && *pnwritten)
    data->info.request_size += *pnwritten;

  DEBUGF(infof(data, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (পূর্ণাঙ্ক)result, *pnwritten));
  প্রত্যাগম result;
}

CURLcode Curl_xfer_recv(সংরচনা Curl_easy *data,
                        বর্ণ *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(data->set.buffer_size > 0);

  যদি(curlx_uitouz(data->set.buffer_size) < blen)
    blen = curlx_uitouz(data->set.buffer_size);
  প্রত্যাগম Curl_conn_recv(data, data->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(সংরচনা Curl_easy *data)
{
  Curl_conn_ev_data_done_send(data);
  প্রত্যাগম CURLE_OK;
}

bool Curl_xfer_is_blocked(সংরচনা Curl_easy *data)
{
  bool want_send = CURL_REQ_WANT_SEND(data);
  bool want_recv = CURL_REQ_WANT_RECV(data);
  যদি(!want_send)
    প্রত্যাগম want_recv && Curl_xfer_recv_is_paused(data);
  অন্যথা যদি(!want_recv)
    প্রত্যাগম want_send && Curl_xfer_send_is_paused(data);
  অন্যথা
    প্রত্যাগম Curl_xfer_recv_is_paused(data) && Curl_xfer_send_is_paused(data);
}

bool Curl_xfer_send_is_paused(সংরচনা Curl_easy *data)
{
  প্রত্যাগম Curl_rlimit_is_blocked(&data->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(সংরচনা Curl_easy *data)
{
  প্রত্যাগম Curl_rlimit_is_blocked(&data->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(সংরচনা Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.ul.rlimit, enable, Curl_pgrs_now(data));
  যদি(!enable && Curl_creader_is_paused(data))
    result = Curl_creader_unpause(data);
  Curl_pgrsSendPause(data, enable);
  প্রত্যাগম result;
}

CURLcode Curl_xfer_pause_recv(সংরচনা Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.dl.rlimit, enable, Curl_pgrs_now(data));
  যদি(!enable && Curl_cwriter_is_paused(data))
    result = Curl_cwriter_unpause(data);
  Curl_conn_ev_data_pause(data, enable);
  Curl_pgrsRecvPause(data, enable);
  প্রত্যাগম result;
}

bool Curl_xfer_is_secure(সংরচনা Curl_easy *data)
{
#ifndef CURL_DISABLE_PROXY
  যদি(data->conn && data->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(data->state.origin->scheme->flags & PROTOPT_SSL));
    প্রত্যাগম FALSE;
  }
#endif
  প্রত্যাগম (data->state.origin->scheme->flags & PROTOPT_SSL);
}
