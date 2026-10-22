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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#شامل <signal.h>

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنهاگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#شامل "urldata.h"

#شامل "cfilters.h"
#شامل "cw-out.h"
#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "http.h"
#شامل "url.h"
#شامل "getinfo.h"
#شامل "multiif.h"
#شامل "connect.h"
#شامل "hsts.h"
#شامل "setopt.h"
#شامل "headers.h"
#شامل "bufref.h"
#شامل "rtsp.h"

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
نویسه *Curl_checkheaders(ثابت ساختار Curl_easy *data,
                        ثابت نویسه *thisheader,
                        ثابت size_t thislen)
{
  ساختار curl_slist *head;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  برای(head = data->set.headers; head; head = head->next) {
    اگر(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      بازگشت head->data;
  }

  بازگشت پوچ;
}
#endif

ایستا صحیح data_pending(ساختار Curl_easy *data, bool rcvd_eagain)
{
  ساختار connectdata *conn = data->conn;

  اگر(conn->scheme->protocol & PROTO_FAMILY_FTP)
    بازگشت Curl_conn_data_pending(data, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  بازگشت (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_data_pending(data, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(ساختار Curl_easy *data, time_t timeofdoc)
{
  اگر((timeofdoc == 0) || (data->set.timevalue == 0))
    بازگشت TRUE;

  انتخاب(data->set.timecondition) {
  حالت CURL_TIMECOND_IFMODSINCE:
  پیشفرض:
    اگر(timeofdoc <= data->set.timevalue) {
      infof(data, "The requested document is not new enough");
      data->info.timecond = TRUE;
      بازگشت FALSE;
    }
    بشکن;
  حالت CURL_TIMECOND_IFUNMODSINCE:
    اگر(timeofdoc >= data->set.timevalue) {
      infof(data, "The requested document is not old enough");
      data->info.timecond = TRUE;
      بازگشت FALSE;
    }
    بشکن;
  }

  بازگشت TRUE;
}

ایستا CURLcode xfer_recv_shutdown(ساختار Curl_easy *data, bool *done)
{
  اگر(!data || !data->conn)
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_cshutdn_try_once_idx(data, data->conn->recv_idx, done);
}

ایستا bool xfer_recv_shutdown_started(ساختار Curl_easy *data)
{
  اگر(!data || !data->conn)
    بازگشت FALSE;
  بازگشت CURL_CONN_IN_SHUTDOWN(data->conn, data->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(ساختار Curl_easy *data, bool *done)
{
  اگر(!data || !data->conn)
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_cshutdn_try_once_idx(data, data->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ایستا CURLcode xfer_recv_resp(ساختار Curl_easy *data,
                               نویسه *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode result;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  اگر(!eos_reliable && !data->req.header && data->req.size != -1) {
    blen = curlx_sotouz_range(data->req.size - data->req.bytecount, 0, blen);
  }
  وگرنه اگر(xfer_recv_shutdown_started(data)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  اگر(blen) {
    result = Curl_xfer_recv(data, buf, blen, pnread);
    اگر(result)
      بازگشت result;
  }

  اگر(*pnread == 0) {
    اگر(data->req.shutdown) {
      bool done;
      result = xfer_recv_shutdown(data, &done);
      اگر(result)
        بازگشت result;
      اگر(!done) {
        بازگشت CURLE_AGAIN;
      }
    }
    DEBUGF(infof(data, "sendrecv_dl: we are done"));
  }
  بازگشت CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ایستا CURLcode sendrecv_dl(ساختار Curl_easy *data,
                            ساختار SingleRequest *k)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  نویسه *buf, *xfer_buf;
  size_t blen, xfer_blen;
  صحیح maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  اگر(result)
    برو out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  انجامبده {
    size_t bytestoread;

    اگر(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    اگر(bytestoread && Curl_rlimit_active(&data->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&data->progress.dl.rlimit, پوچ);
#اگر 0
      DEBUGF(infof(data, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      اگر(dl_avail <= 0) {
        rate_limited = TRUE;
        بشکن;
      }
      اگر(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    result = xfer_recv_resp(data, buf, bytestoread, is_multiplex, &blen);
    اگر(result) {
      اگر(result != CURLE_AGAIN)
        برو out; /* real error */
      rcvd_eagain = TRUE;
      result = CURLE_OK;
      اگر(data->req.download_done && data->req.no_body &&
         !data->req.resp_trailer) {
        DEBUGF(infof(data, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      وگرنه
        بشکن; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    اگر(!blen) {
      result = Curl_req_stop_send_recv(data);
      اگر(result)
        برو out;
      اگر(k->eos_written) /* already did write this to client, leave */
        بشکن;
    }

    result = Curl_xfer_write_resp(data, buf, blen, is_eos);
    اگر(result || data->req.done)
      برو out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    اگر((!is_multiplex && data->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(data);
    }
    /* if we stopped receiving, leave the loop */
    اگر(!CURL_REQ_WANT_RECV(data))
      بشکن;

  } وقتی(maxloops--);

  اگر(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(data) &&
     (!rcvd_eagain || data_pending(data, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(data);
    CURL_TRC_M(data, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  اگر(!CURL_REQ_WANT_RECV(data) && CURL_REQ_WANT_SEND(data) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(data, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(data);
  }

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  اگر(result)
    DEBUGF(infof(data, "sendrecv_dl() -> %d", (صحیح)result));
  بازگشت result;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ایستا CURLcode sendrecv_ul(ساختار Curl_easy *data)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(data));

  اگر(!Curl_req_done_sending(data))
    بازگشت Curl_req_send_more(data);
  بازگشت CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(ساختار Curl_easy *data)
{
  ساختار SingleRequest *k = &data->req;
  ثابت ساختار curltime *pnow = پوچ;
  CURLcode result = CURLE_OK;

  اگر(Curl_xfer_is_blocked(data)) {
    result = CURLE_OK;
    برو out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  اگر(CURL_REQ_WANT_RECV(data)) {
    result = sendrecv_dl(data, k);
    اگر(result || data->req.done)
      برو out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  اگر(Curl_req_want_send(data)) {
    result = sendrecv_ul(data);
    اگر(result)
      برو out;
  }

  pnow = Curl_pgrs_now(data);
  اگر(CURL_REQ_WANT_IO(data)) {
    اگر(Curl_timeleft_now_ms(data, pnow) < 0) {
      اگر(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE),
              k->bytecount, k->size);
      }
      وگرنه {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE),
              k->bytecount);
      }
      result = CURLE_OPERATION_TIMEDOUT;
      برو out;
    }
  }
  وگرنه {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    اگر(!data->req.no_body && (k->size != -1) &&
       (k->bytecount != k->size) && !k->newurl) {
      failf(data, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      result = CURLE_PARTIAL_FILE;
      برو out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  اگر(!CURL_REQ_WANT_IO(data))
    data->req.done = TRUE;

  result = Curl_pgrsCheckX(data, pnow);

out:
  اگر(result)
    DEBUGF(infof(data, "Curl_sendrecv() -> %d", (صحیح)result));
  بازگشت result;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
تهی Curl_init_CONNECT(ساختار Curl_easy *data)
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
CURLcode Curl_pretransfer(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  data->state.retrycount = 0;

  اگر(!CURL_EASY_STR(data, STRING_SET_URL) && !data->set.uh) {
    /* we cannot do anything without URL */
    failf(data, "No URL set");
    بازگشت CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  اگر(data->set.uh) {
    نویسه *url = پوچ;
    CURLUcode uc;
    uc = curl_url_get(data->set.uh, CURLUPART_URL, &url, 0);
    اگر(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&data->state.url, پوچ, 0, پوچ);
      failf(data, "No URL set");
      بازگشت CURLE_URL_MALFORMAT;
    }
    result = CURL_EASY_STR_SETN(data, STRING_SET_URL, url);
    اگر(result)
      بازگشت result;
  }

  Curl_bufref_set(&data->state.url, CURL_EASY_STR(data, STRING_SET_URL),
                  0, پوچ);

  اگر(data->set.postfields && data->set.set_resume_from) {
    /* we cannot */
    failf(data, "cannot mix POSTFIELDS with RESUME_FROM");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
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
  اگر(data->set.http_auto_referer)
    Curl_bufref_free(&data->state.referer);
  اگر(CURL_EASY_STR(data, STRING_SET_REFERER))
    Curl_bufref_set(&data->state.referer,
                    CURL_EASY_STR(data, STRING_SET_REFERER), 0, پوچ);
  وگرنه
    Curl_bufref_free(&data->state.referer);

  اگر(data->state.httpreq == HTTPREQ_PUT)
    data->state.infilesize = data->set.filesize;
  وگرنه اگر((data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    data->state.infilesize = data->set.postfieldsize;
    اگر(data->set.postfields && (data->state.infilesize == -1))
      data->state.infilesize = (curl_off_t)strlen(data->set.postfields);
  }
  وگرنه
    data->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  result = Curl_cookie_loadfiles(data,
                                 data->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  اگر(!result)
    Curl_cookie_run(data); /* activate */

  /* If there is a list of host pairs to deal with */
  اگر(!result && data->state.resolve)
    result = Curl_loadhostpairs(data);

  اگر(!result)
    /* If there is a list of hsts files to read */
    result = Curl_hsts_loadfiles(data);

  اگر(!result) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    data->state.allow_port = TRUE;

#اگر defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    اگر(!data->set.no_signal)
      data->state.prev_signal = سیگنال(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(data); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(data);
    Curl_pgrsStart(data, پوچ);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    data->state.authhost.picked &= data->state.authhost.want;
    data->state.authproxy.picked &= data->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    data->state.wildcardmatch = data->set.wildcard_enabled;
#ifdef DEBUGBUILD
  اگر(getenv("CURL_DBG_FTP_WILDCARD"))
    data->state.wildcardmatch = TRUE;
#endif
    اگر(data->state.wildcardmatch) {
      ساختار WildcardData *wc;
      اگر(!data->wildcard) {
        data->wildcard = curlx_calloc(1, اندازه(ساختار WildcardData));
        اگر(!data->wildcard)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      wc = data->wildcard;
      اگر(wc->state < CURLWC_INIT) {
        اگر(wc->ftpwc)
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
  بازگشت result;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(ساختار Curl_easy *data, نویسه **url)
{
  ساختار connectdata *conn = data->conn;
  bool retry = FALSE;
  *url = پوچ;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  اگر(data->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    بازگشت CURLE_OK;

  اگر(conn->bits.reuse &&
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
  وگرنه اگر(data->state.refused_stream &&
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
  اگر(retry) {
#define CONN_MAX_RETRIES 5
    اگر(data->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(data, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      data->state.retrycount = 0;
      بازگشت CURLE_SEND_ERROR;
    }
    infof(data, "Connection died, retrying a fresh connect (retry count: %d)",
          data->state.retrycount);
    *url = Curl_bufref_dup(&data->state.url);
    اگر(!*url)
      بازگشت CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(data, TRUE);
  }
  بازگشت CURLE_OK;
}

ایستا تهی xfer_setup(
  ساختار Curl_easy *data,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_size      /* how much to receive, -1 if unknown */
  )
{
  ساختار SingleRequest *k = &data->req;
  ساختار connectdata *conn = data->conn;

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
  اگر(!k->header && (recv_size > 0))
    Curl_pgrsSetDownloadSize(data, recv_size);

  /* we want header and/or body, if neither then do not do this! */
  اگر(conn->scheme->run->write_resp_hd || !data->req.no_body) {
    اگر(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(data);
    اگر(conn->send_idx != -1)
      CURL_REQ_SET_SEND(data);
  }
  CURL_TRC_M(data, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

تهی Curl_xfer_setup_nop(ساختار Curl_easy *data)
{
  xfer_setup(data, -1, -1, -1);
}

تهی Curl_xfer_setup_sendrecv(ساختار Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size)
{
  xfer_setup(data, sockindex, sockindex, recv_size);
}

تهی Curl_xfer_setup_send(ساختار Curl_easy *data,
                          int8_t sockindex)
{
  xfer_setup(data, sockindex, -1, -1);
}

تهی Curl_xfer_setup_recv(ساختار Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size)
{
  xfer_setup(data, -1, sockindex, recv_size);
}

تهی Curl_xfer_set_shutdown(ساختار Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (data->conn->send_idx < 0) || (data->conn->recv_idx < 0));
  data->req.shutdown = shutdown;
  data->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(ساختار Curl_easy *data,
                              ثابت نویسه *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result = CURLE_OK;

  اگر(data->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    result = data->conn->scheme->run->write_resp(data, buf, blen, is_eos);
  }
  وگرنه {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    اگر(blen || is_eos) {
      صحیح cwtype = CLIENTWRITE_BODY;
      اگر(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      result = Curl_client_write(data, cwtype, buf, blen);
    }
  }

  اگر(!result && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    data->req.eos_written = TRUE;
    data->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(data, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (صحیح)result);
  بازگشت result;
}

bool Curl_xfer_write_is_paused(ساختار Curl_easy *data)
{
  بازگشت Curl_cwriter_is_paused(data);
}

CURLcode Curl_xfer_write_resp_hd(ساختار Curl_easy *data,
                                 ثابت نویسه *hd0, size_t hdlen, bool is_eos)
{
  اگر(data->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    بازگشت data->conn->scheme->run->write_resp_hd(data, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  بازگشت Curl_xfer_write_resp(data, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(ساختار Curl_easy *data, bool premature)
{
  (تهی)premature;
  بازگشت Curl_cw_out_done(data);
}

bool Curl_xfer_needs_flush(ساختار Curl_easy *data)
{
  بازگشت Curl_conn_needs_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_flush(ساختار Curl_easy *data)
{
  بازگشت Curl_conn_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_send(ساختار Curl_easy *data,
                        ثابت تهی *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode result;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  result = Curl_conn_send(data, data->conn->send_idx,
                          buf, blen, eos, pnwritten);
  اگر(result == CURLE_AGAIN) {
    result = CURLE_OK;
    *pnwritten = 0;
  }
  وگرنه اگر(!result && *pnwritten)
    data->info.request_size += *pnwritten;

  DEBUGF(infof(data, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (صحیح)result, *pnwritten));
  بازگشت result;
}

CURLcode Curl_xfer_recv(ساختار Curl_easy *data,
                        نویسه *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(data->set.buffer_size > 0);

  اگر(curlx_uitouz(data->set.buffer_size) < blen)
    blen = curlx_uitouz(data->set.buffer_size);
  بازگشت Curl_conn_recv(data, data->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(ساختار Curl_easy *data)
{
  Curl_conn_ev_data_done_send(data);
  بازگشت CURLE_OK;
}

bool Curl_xfer_is_blocked(ساختار Curl_easy *data)
{
  bool want_send = CURL_REQ_WANT_SEND(data);
  bool want_recv = CURL_REQ_WANT_RECV(data);
  اگر(!want_send)
    بازگشت want_recv && Curl_xfer_recv_is_paused(data);
  وگرنه اگر(!want_recv)
    بازگشت want_send && Curl_xfer_send_is_paused(data);
  وگرنه
    بازگشت Curl_xfer_recv_is_paused(data) && Curl_xfer_send_is_paused(data);
}

bool Curl_xfer_send_is_paused(ساختار Curl_easy *data)
{
  بازگشت Curl_rlimit_is_blocked(&data->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(ساختار Curl_easy *data)
{
  بازگشت Curl_rlimit_is_blocked(&data->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(ساختار Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.ul.rlimit, enable, Curl_pgrs_now(data));
  اگر(!enable && Curl_creader_is_paused(data))
    result = Curl_creader_unpause(data);
  Curl_pgrsSendPause(data, enable);
  بازگشت result;
}

CURLcode Curl_xfer_pause_recv(ساختار Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.dl.rlimit, enable, Curl_pgrs_now(data));
  اگر(!enable && Curl_cwriter_is_paused(data))
    result = Curl_cwriter_unpause(data);
  Curl_conn_ev_data_pause(data, enable);
  Curl_pgrsRecvPause(data, enable);
  بازگشت result;
}

bool Curl_xfer_is_secure(ساختار Curl_easy *data)
{
#ifndef CURL_DISABLE_PROXY
  اگر(data->conn && data->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(data->state.origin->scheme->flags & PROTOPT_SSL));
    بازگشت FALSE;
  }
#endif
  بازگشت (data->state.origin->scheme->flags & PROTOPT_SSL);
}
