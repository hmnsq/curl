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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#अन्तर्भाव <signal.h>

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#अन्तर्भाव "urldata.h"

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cw-out.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "getinfo.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "setopt.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "rtsp.h"

#सचे !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
वर्ण *Curl_checkheaders(ध्रुव संरचना Curl_easy *data,
                        ध्रुव वर्ण *thisheader,
                        ध्रुव size_t thislen)
{
  संरचना curl_slist *head;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  अत्थाय(head = data->set.headers; head; head = head->next) {
    सचे(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      पटिनिवत्त head->data;
  }

  पटिनिवत्त रिक्त;
}
#endif

स्थिर पूर्णाङ्क data_pending(संरचना Curl_easy *data, bool rcvd_eagain)
{
  संरचना connectdata *conn = data->conn;

  सचे(conn->scheme->protocol & PROTO_FAMILY_FTP)
    पटिनिवत्त Curl_conn_data_pending(data, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  पटिनिवत्त (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_data_pending(data, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(संरचना Curl_easy *data, time_t timeofdoc)
{
  सचे((timeofdoc == 0) || (data->set.timevalue == 0))
    पटिनिवत्त TRUE;

  विकल्प(data->set.timecondition) {
  पक्ष CURL_TIMECOND_IFMODSINCE:
  स्वभाव:
    सचे(timeofdoc <= data->set.timevalue) {
      infof(data, "The requested document is not new enough");
      data->info.timecond = TRUE;
      पटिनिवत्त FALSE;
    }
    भङ्ग;
  पक्ष CURL_TIMECOND_IFUNMODSINCE:
    सचे(timeofdoc >= data->set.timevalue) {
      infof(data, "The requested document is not old enough");
      data->info.timecond = TRUE;
      पटिनिवत्त FALSE;
    }
    भङ्ग;
  }

  पटिनिवत्त TRUE;
}

स्थिर CURLcode xfer_recv_shutdown(संरचना Curl_easy *data, bool *done)
{
  सचे(!data || !data->conn)
    पटिनिवत्त CURLE_FAILED_INIT;
  पटिनिवत्त Curl_cshutdn_try_once_idx(data, data->conn->recv_idx, done);
}

स्थिर bool xfer_recv_shutdown_started(संरचना Curl_easy *data)
{
  सचे(!data || !data->conn)
    पटिनिवत्त FALSE;
  पटिनिवत्त CURL_CONN_IN_SHUTDOWN(data->conn, data->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(संरचना Curl_easy *data, bool *done)
{
  सचे(!data || !data->conn)
    पटिनिवत्त CURLE_FAILED_INIT;
  पटिनिवत्त Curl_cshutdn_try_once_idx(data, data->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
स्थिर CURLcode xfer_recv_resp(संरचना Curl_easy *data,
                               वर्ण *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode result;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  सचे(!eos_reliable && !data->req.header && data->req.size != -1) {
    blen = curlx_sotouz_range(data->req.size - data->req.bytecount, 0, blen);
  }
  अञ्ञथा सचे(xfer_recv_shutdown_started(data)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  सचे(blen) {
    result = Curl_xfer_recv(data, buf, blen, pnread);
    सचे(result)
      पटिनिवत्त result;
  }

  सचे(*pnread == 0) {
    सचे(data->req.shutdown) {
      bool done;
      result = xfer_recv_shutdown(data, &done);
      सचे(result)
        पटिनिवत्त result;
      सचे(!done) {
        पटिनिवत्त CURLE_AGAIN;
      }
    }
    DEBUGF(infof(data, "sendrecv_dl: we are done"));
  }
  पटिनिवत्त CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
स्थिर CURLcode sendrecv_dl(संरचना Curl_easy *data,
                            संरचना SingleRequest *k)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  वर्ण *buf, *xfer_buf;
  size_t blen, xfer_blen;
  पूर्णाङ्क maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  सचे(result)
    गच्छ out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  करोहि {
    size_t bytestoread;

    सचे(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    सचे(bytestoread && Curl_rlimit_active(&data->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&data->progress.dl.rlimit, रिक्त);
#सचे 0
      DEBUGF(infof(data, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      सचे(dl_avail <= 0) {
        rate_limited = TRUE;
        भङ्ग;
      }
      सचे(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    result = xfer_recv_resp(data, buf, bytestoread, is_multiplex, &blen);
    सचे(result) {
      सचे(result != CURLE_AGAIN)
        गच्छ out; /* real error */
      rcvd_eagain = TRUE;
      result = CURLE_OK;
      सचे(data->req.download_done && data->req.no_body &&
         !data->req.resp_trailer) {
        DEBUGF(infof(data, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      अञ्ञथा
        भङ्ग; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    सचे(!blen) {
      result = Curl_req_stop_send_recv(data);
      सचे(result)
        गच्छ out;
      सचे(k->eos_written) /* already did write this to client, leave */
        भङ्ग;
    }

    result = Curl_xfer_write_resp(data, buf, blen, is_eos);
    सचे(result || data->req.done)
      गच्छ out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    सचे((!is_multiplex && data->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(data);
    }
    /* if we stopped receiving, leave the loop */
    सचे(!CURL_REQ_WANT_RECV(data))
      भङ्ग;

  } याव(maxloops--);

  सचे(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(data) &&
     (!rcvd_eagain || data_pending(data, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(data);
    CURL_TRC_M(data, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  सचे(!CURL_REQ_WANT_RECV(data) && CURL_REQ_WANT_SEND(data) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(data, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(data);
  }

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  सचे(result)
    DEBUGF(infof(data, "sendrecv_dl() -> %d", (पूर्णाङ्क)result));
  पटिनिवत्त result;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
स्थिर CURLcode sendrecv_ul(संरचना Curl_easy *data)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(data));

  सचे(!Curl_req_done_sending(data))
    पटिनिवत्त Curl_req_send_more(data);
  पटिनिवत्त CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(संरचना Curl_easy *data)
{
  संरचना SingleRequest *k = &data->req;
  ध्रुव संरचना curltime *pnow = रिक्त;
  CURLcode result = CURLE_OK;

  सचे(Curl_xfer_is_blocked(data)) {
    result = CURLE_OK;
    गच्छ out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  सचे(CURL_REQ_WANT_RECV(data)) {
    result = sendrecv_dl(data, k);
    सचे(result || data->req.done)
      गच्छ out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  सचे(Curl_req_want_send(data)) {
    result = sendrecv_ul(data);
    सचे(result)
      गच्छ out;
  }

  pnow = Curl_pgrs_now(data);
  सचे(CURL_REQ_WANT_IO(data)) {
    सचे(Curl_timeleft_now_ms(data, pnow) < 0) {
      सचे(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE),
              k->bytecount, k->size);
      }
      अञ्ञथा {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE),
              k->bytecount);
      }
      result = CURLE_OPERATION_TIMEDOUT;
      गच्छ out;
    }
  }
  अञ्ञथा {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    सचे(!data->req.no_body && (k->size != -1) &&
       (k->bytecount != k->size) && !k->newurl) {
      failf(data, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      result = CURLE_PARTIAL_FILE;
      गच्छ out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  सचे(!CURL_REQ_WANT_IO(data))
    data->req.done = TRUE;

  result = Curl_pgrsCheckX(data, pnow);

out:
  सचे(result)
    DEBUGF(infof(data, "Curl_sendrecv() -> %d", (पूर्णाङ्क)result));
  पटिनिवत्त result;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
शून्य Curl_init_CONNECT(संरचना Curl_easy *data)
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
CURLcode Curl_pretransfer(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  data->state.retrycount = 0;

  सचे(!CURL_EASY_STR(data, STRING_SET_URL) && !data->set.uh) {
    /* we cannot do anything without URL */
    failf(data, "No URL set");
    पटिनिवत्त CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  सचे(data->set.uh) {
    वर्ण *url = रिक्त;
    CURLUcode uc;
    uc = curl_url_get(data->set.uh, CURLUPART_URL, &url, 0);
    सचे(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&data->state.url, रिक्त, 0, रिक्त);
      failf(data, "No URL set");
      पटिनिवत्त CURLE_URL_MALFORMAT;
    }
    result = CURL_EASY_STR_SETN(data, STRING_SET_URL, url);
    सचे(result)
      पटिनिवत्त result;
  }

  Curl_bufref_set(&data->state.url, CURL_EASY_STR(data, STRING_SET_URL),
                  0, रिक्त);

  सचे(data->set.postfields && data->set.set_resume_from) {
    /* we cannot */
    failf(data, "cannot mix POSTFIELDS with RESUME_FROM");
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
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
  सचे(data->set.http_auto_referer)
    Curl_bufref_free(&data->state.referer);
  सचे(CURL_EASY_STR(data, STRING_SET_REFERER))
    Curl_bufref_set(&data->state.referer,
                    CURL_EASY_STR(data, STRING_SET_REFERER), 0, रिक्त);
  अञ्ञथा
    Curl_bufref_free(&data->state.referer);

  सचे(data->state.httpreq == HTTPREQ_PUT)
    data->state.infilesize = data->set.filesize;
  अञ्ञथा सचे((data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    data->state.infilesize = data->set.postfieldsize;
    सचे(data->set.postfields && (data->state.infilesize == -1))
      data->state.infilesize = (curl_off_t)strlen(data->set.postfields);
  }
  अञ्ञथा
    data->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  result = Curl_cookie_loadfiles(data,
                                 data->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  सचे(!result)
    Curl_cookie_run(data); /* activate */

  /* If there is a list of host pairs to deal with */
  सचे(!result && data->state.resolve)
    result = Curl_loadhostpairs(data);

  सचे(!result)
    /* If there is a list of hsts files to read */
    result = Curl_hsts_loadfiles(data);

  सचे(!result) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    data->state.allow_port = TRUE;

#सचे defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    सचे(!data->set.no_signal)
      data->state.prev_signal = सङ्केत(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(data); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(data);
    Curl_pgrsStart(data, रिक्त);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    data->state.authhost.picked &= data->state.authhost.want;
    data->state.authproxy.picked &= data->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    data->state.wildcardmatch = data->set.wildcard_enabled;
#ifdef DEBUGBUILD
  सचे(getenv("CURL_DBG_FTP_WILDCARD"))
    data->state.wildcardmatch = TRUE;
#endif
    सचे(data->state.wildcardmatch) {
      संरचना WildcardData *wc;
      सचे(!data->wildcard) {
        data->wildcard = curlx_calloc(1, परिमाण(संरचना WildcardData));
        सचे(!data->wildcard)
          पटिनिवत्त CURLE_OUT_OF_MEMORY;
      }
      wc = data->wildcard;
      सचे(wc->state < CURLWC_INIT) {
        सचे(wc->ftpwc)
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
  पटिनिवत्त result;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(संरचना Curl_easy *data, वर्ण **url)
{
  संरचना connectdata *conn = data->conn;
  bool retry = FALSE;
  *url = रिक्त;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  सचे(data->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    पटिनिवत्त CURLE_OK;

  सचे(conn->bits.reuse &&
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
  अञ्ञथा सचे(data->state.refused_stream &&
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
  सचे(retry) {
#define CONN_MAX_RETRIES 5
    सचे(data->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(data, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      data->state.retrycount = 0;
      पटिनिवत्त CURLE_SEND_ERROR;
    }
    infof(data, "Connection died, retrying a fresh connect (retry count: %d)",
          data->state.retrycount);
    *url = Curl_bufref_dup(&data->state.url);
    सचे(!*url)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(data, TRUE);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य xfer_setup(
  संरचना Curl_easy *data,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_size      /* how much to receive, -1 if unknown */
  )
{
  संरचना SingleRequest *k = &data->req;
  संरचना connectdata *conn = data->conn;

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
  सचे(!k->header && (recv_size > 0))
    Curl_pgrsSetDownloadSize(data, recv_size);

  /* we want header and/or body, if neither then do not do this! */
  सचे(conn->scheme->run->write_resp_hd || !data->req.no_body) {
    सचे(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(data);
    सचे(conn->send_idx != -1)
      CURL_REQ_SET_SEND(data);
  }
  CURL_TRC_M(data, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

शून्य Curl_xfer_setup_nop(संरचना Curl_easy *data)
{
  xfer_setup(data, -1, -1, -1);
}

शून्य Curl_xfer_setup_sendrecv(संरचना Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size)
{
  xfer_setup(data, sockindex, sockindex, recv_size);
}

शून्य Curl_xfer_setup_send(संरचना Curl_easy *data,
                          int8_t sockindex)
{
  xfer_setup(data, sockindex, -1, -1);
}

शून्य Curl_xfer_setup_recv(संरचना Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size)
{
  xfer_setup(data, -1, sockindex, recv_size);
}

शून्य Curl_xfer_set_shutdown(संरचना Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (data->conn->send_idx < 0) || (data->conn->recv_idx < 0));
  data->req.shutdown = shutdown;
  data->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(संरचना Curl_easy *data,
                              ध्रुव वर्ण *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result = CURLE_OK;

  सचे(data->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    result = data->conn->scheme->run->write_resp(data, buf, blen, is_eos);
  }
  अञ्ञथा {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    सचे(blen || is_eos) {
      पूर्णाङ्क cwtype = CLIENTWRITE_BODY;
      सचे(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      result = Curl_client_write(data, cwtype, buf, blen);
    }
  }

  सचे(!result && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    data->req.eos_written = TRUE;
    data->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(data, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (पूर्णाङ्क)result);
  पटिनिवत्त result;
}

bool Curl_xfer_write_is_paused(संरचना Curl_easy *data)
{
  पटिनिवत्त Curl_cwriter_is_paused(data);
}

CURLcode Curl_xfer_write_resp_hd(संरचना Curl_easy *data,
                                 ध्रुव वर्ण *hd0, size_t hdlen, bool is_eos)
{
  सचे(data->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    पटिनिवत्त data->conn->scheme->run->write_resp_hd(data, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  पटिनिवत्त Curl_xfer_write_resp(data, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(संरचना Curl_easy *data, bool premature)
{
  (शून्य)premature;
  पटिनिवत्त Curl_cw_out_done(data);
}

bool Curl_xfer_needs_flush(संरचना Curl_easy *data)
{
  पटिनिवत्त Curl_conn_needs_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_flush(संरचना Curl_easy *data)
{
  पटिनिवत्त Curl_conn_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_send(संरचना Curl_easy *data,
                        ध्रुव शून्य *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode result;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  result = Curl_conn_send(data, data->conn->send_idx,
                          buf, blen, eos, pnwritten);
  सचे(result == CURLE_AGAIN) {
    result = CURLE_OK;
    *pnwritten = 0;
  }
  अञ्ञथा सचे(!result && *pnwritten)
    data->info.request_size += *pnwritten;

  DEBUGF(infof(data, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (पूर्णाङ्क)result, *pnwritten));
  पटिनिवत्त result;
}

CURLcode Curl_xfer_recv(संरचना Curl_easy *data,
                        वर्ण *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(data->set.buffer_size > 0);

  सचे(curlx_uitouz(data->set.buffer_size) < blen)
    blen = curlx_uitouz(data->set.buffer_size);
  पटिनिवत्त Curl_conn_recv(data, data->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(संरचना Curl_easy *data)
{
  Curl_conn_ev_data_done_send(data);
  पटिनिवत्त CURLE_OK;
}

bool Curl_xfer_is_blocked(संरचना Curl_easy *data)
{
  bool want_send = CURL_REQ_WANT_SEND(data);
  bool want_recv = CURL_REQ_WANT_RECV(data);
  सचे(!want_send)
    पटिनिवत्त want_recv && Curl_xfer_recv_is_paused(data);
  अञ्ञथा सचे(!want_recv)
    पटिनिवत्त want_send && Curl_xfer_send_is_paused(data);
  अञ्ञथा
    पटिनिवत्त Curl_xfer_recv_is_paused(data) && Curl_xfer_send_is_paused(data);
}

bool Curl_xfer_send_is_paused(संरचना Curl_easy *data)
{
  पटिनिवत्त Curl_rlimit_is_blocked(&data->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(संरचना Curl_easy *data)
{
  पटिनिवत्त Curl_rlimit_is_blocked(&data->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(संरचना Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.ul.rlimit, enable, Curl_pgrs_now(data));
  सचे(!enable && Curl_creader_is_paused(data))
    result = Curl_creader_unpause(data);
  Curl_pgrsSendPause(data, enable);
  पटिनिवत्त result;
}

CURLcode Curl_xfer_pause_recv(संरचना Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.dl.rlimit, enable, Curl_pgrs_now(data));
  सचे(!enable && Curl_cwriter_is_paused(data))
    result = Curl_cwriter_unpause(data);
  Curl_conn_ev_data_pause(data, enable);
  Curl_pgrsRecvPause(data, enable);
  पटिनिवत्त result;
}

bool Curl_xfer_is_secure(संरचना Curl_easy *data)
{
#ifndef CURL_DISABLE_PROXY
  सचे(data->conn && data->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(data->state.origin->scheme->flags & PROTOPT_SSL));
    पटिनिवत्त FALSE;
  }
#endif
  पटिनिवत्त (data->state.origin->scheme->flags & PROTOPT_SSL);
}
