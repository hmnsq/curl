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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ଅନ୍ତର୍ଭାଵ <signal.h>

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#ଅନ୍ତର୍ଭାଵ <sys/select.h>
#ନଚେତଯଦି defined(HAVE_UNISTD_H)
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cw-out.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "getinfo.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "setopt.h"
#ଅନ୍ତର୍ଭାଵ "headers.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "rtsp.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
ଅକ୍ଷର *Curl_checkheaders(ଅଚଳ ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *thisheader,
                        ଅଚଳ size_t thislen)
{
  ସଂରଚନା curl_slist *head;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  ପାଇଁ(head = data->set.headers; head; head = head->next) {
    ଯଦି(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      ଫେରନ୍ତୁ head->data;
  }

  ଫେରନ୍ତୁ ଖାଲି;
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_pending(ସଂରଚନା Curl_easy *data, bool rcvd_eagain)
{
  ସଂରଚନା connectdata *conn = data->conn;

  ଯଦି(conn->scheme->protocol & PROTO_FAMILY_FTP)
    ଫେରନ୍ତୁ Curl_conn_data_pending(data, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  ଫେରନ୍ତୁ (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_data_pending(data, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(ସଂରଚନା Curl_easy *data, time_t timeofdoc)
{
  ଯଦି((timeofdoc == 0) || (data->set.timevalue == 0))
    ଫେରନ୍ତୁ TRUE;

  ବାଛ(data->set.timecondition) {
  ସ୍ଥିତି CURL_TIMECOND_IFMODSINCE:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(timeofdoc <= data->set.timevalue) {
      infof(data, "The requested document is not new enough");
      data->info.timecond = TRUE;
      ଫେରନ୍ତୁ FALSE;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_TIMECOND_IFUNMODSINCE:
    ଯଦି(timeofdoc >= data->set.timevalue) {
      infof(data, "The requested document is not old enough");
      data->info.timecond = TRUE;
      ଫେରନ୍ତୁ FALSE;
    }
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode xfer_recv_shutdown(ସଂରଚନା Curl_easy *data, bool *done)
{
  ଯଦି(!data || !data->conn)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_cshutdn_try_once_idx(data, data->conn->recv_idx, done);
}

ସ୍ଥିର bool xfer_recv_shutdown_started(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(!data || !data->conn)
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ CURL_CONN_IN_SHUTDOWN(data->conn, data->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(ସଂରଚନା Curl_easy *data, bool *done)
{
  ଯଦି(!data || !data->conn)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_cshutdn_try_once_idx(data, data->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ସ୍ଥିର CURLcode xfer_recv_resp(ସଂରଚନା Curl_easy *data,
                               ଅକ୍ଷର *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode result;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  ଯଦି(!eos_reliable && !data->req.header && data->req.size != -1) {
    blen = curlx_sotouz_range(data->req.size - data->req.bytecount, 0, blen);
  }
  ଅନ୍ୟଥା ଯଦି(xfer_recv_shutdown_started(data)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  ଯଦି(blen) {
    result = Curl_xfer_recv(data, buf, blen, pnread);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(*pnread == 0) {
    ଯଦି(data->req.shutdown) {
      bool done;
      result = xfer_recv_shutdown(data, &done);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      ଯଦି(!done) {
        ଫେରନ୍ତୁ CURLE_AGAIN;
      }
    }
    DEBUGF(infof(data, "sendrecv_dl: we are done"));
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ସ୍ଥିର CURLcode sendrecv_dl(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା SingleRequest *k)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *buf, *xfer_buf;
  size_t blen, xfer_blen;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  ଯଦି(result)
    ଯାଅ out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  କର {
    size_t bytestoread;

    ଯଦି(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    ଯଦି(bytestoread && Curl_rlimit_active(&data->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ଖାଲି);
#ଯଦି 0
      DEBUGF(infof(data, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      ଯଦି(dl_avail <= 0) {
        rate_limited = TRUE;
        ଭାଙ୍ଗ;
      }
      ଯଦି(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    result = xfer_recv_resp(data, buf, bytestoread, is_multiplex, &blen);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN)
        ଯାଅ out; /* real error */
      rcvd_eagain = TRUE;
      result = CURLE_OK;
      ଯଦି(data->req.download_done && data->req.no_body &&
         !data->req.resp_trailer) {
        DEBUGF(infof(data, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      ଅନ୍ୟଥା
        ଭାଙ୍ଗ; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    ଯଦି(!blen) {
      result = Curl_req_stop_send_recv(data);
      ଯଦି(result)
        ଯାଅ out;
      ଯଦି(k->eos_written) /* already did write this to client, leave */
        ଭାଙ୍ଗ;
    }

    result = Curl_xfer_write_resp(data, buf, blen, is_eos);
    ଯଦି(result || data->req.done)
      ଯାଅ out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    ଯଦି((!is_multiplex && data->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(data);
    }
    /* if we stopped receiving, leave the loop */
    ଯଦି(!CURL_REQ_WANT_RECV(data))
      ଭାଙ୍ଗ;

  } ଯେତେବେଳେ(maxloops--);

  ଯଦି(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(data) &&
     (!rcvd_eagain || data_pending(data, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(data);
    CURL_TRC_M(data, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  ଯଦି(!CURL_REQ_WANT_RECV(data) && CURL_REQ_WANT_SEND(data) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(data, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(data);
  }

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  ଯଦି(result)
    DEBUGF(infof(data, "sendrecv_dl() -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result));
  ଫେରନ୍ତୁ result;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ସ୍ଥିର CURLcode sendrecv_ul(ସଂରଚନା Curl_easy *data)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(data));

  ଯଦି(!Curl_req_done_sending(data))
    ଫେରନ୍ତୁ Curl_req_send_more(data);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା SingleRequest *k = &data->req;
  ଅଚଳ ସଂରଚନା curltime *pnow = ଖାଲି;
  CURLcode result = CURLE_OK;

  ଯଦି(Curl_xfer_is_blocked(data)) {
    result = CURLE_OK;
    ଯାଅ out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  ଯଦି(CURL_REQ_WANT_RECV(data)) {
    result = sendrecv_dl(data, k);
    ଯଦି(result || data->req.done)
      ଯାଅ out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  ଯଦି(Curl_req_want_send(data)) {
    result = sendrecv_ul(data);
    ଯଦି(result)
      ଯାଅ out;
  }

  pnow = Curl_pgrs_now(data);
  ଯଦି(CURL_REQ_WANT_IO(data)) {
    ଯଦି(Curl_timeleft_now_ms(data, pnow) < 0) {
      ଯଦି(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE),
              k->bytecount, k->size);
      }
      ଅନ୍ୟଥା {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE),
              k->bytecount);
      }
      result = CURLE_OPERATION_TIMEDOUT;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    ଯଦି(!data->req.no_body && (k->size != -1) &&
       (k->bytecount != k->size) && !k->newurl) {
      failf(data, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      result = CURLE_PARTIAL_FILE;
      ଯାଅ out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  ଯଦି(!CURL_REQ_WANT_IO(data))
    data->req.done = TRUE;

  result = Curl_pgrsCheckX(data, pnow);

out:
  ଯଦି(result)
    DEBUGF(infof(data, "Curl_sendrecv() -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result));
  ଫେରନ୍ତୁ result;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
ଶୂନ୍ୟ Curl_init_CONNECT(ସଂରଚନା Curl_easy *data)
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
CURLcode Curl_pretransfer(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  data->state.retrycount = 0;

  ଯଦି(!CURL_EASY_STR(data, STRING_SET_URL) && !data->set.uh) {
    /* we cannot do anything without URL */
    failf(data, "No URL set");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  ଯଦି(data->set.uh) {
    ଅକ୍ଷର *url = ଖାଲି;
    CURLUcode uc;
    uc = curl_url_get(data->set.uh, CURLUPART_URL, &url, 0);
    ଯଦି(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&data->state.url, ଖାଲି, 0, ଖାଲି);
      failf(data, "No URL set");
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }
    result = CURL_EASY_STR_SETN(data, STRING_SET_URL, url);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  Curl_bufref_set(&data->state.url, CURL_EASY_STR(data, STRING_SET_URL),
                  0, ଖାଲି);

  ଯଦି(data->set.postfields && data->set.set_resume_from) {
    /* we cannot */
    failf(data, "cannot mix POSTFIELDS with RESUME_FROM");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
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
  ଯଦି(data->set.http_auto_referer)
    Curl_bufref_free(&data->state.referer);
  ଯଦି(CURL_EASY_STR(data, STRING_SET_REFERER))
    Curl_bufref_set(&data->state.referer,
                    CURL_EASY_STR(data, STRING_SET_REFERER), 0, ଖାଲି);
  ଅନ୍ୟଥା
    Curl_bufref_free(&data->state.referer);

  ଯଦି(data->state.httpreq == HTTPREQ_PUT)
    data->state.infilesize = data->set.filesize;
  ଅନ୍ୟଥା ଯଦି((data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    data->state.infilesize = data->set.postfieldsize;
    ଯଦି(data->set.postfields && (data->state.infilesize == -1))
      data->state.infilesize = (curl_off_t)strlen(data->set.postfields);
  }
  ଅନ୍ୟଥା
    data->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  result = Curl_cookie_loadfiles(data,
                                 data->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  ଯଦି(!result)
    Curl_cookie_run(data); /* activate */

  /* If there is a list of host pairs to deal with */
  ଯଦି(!result && data->state.resolve)
    result = Curl_loadhostpairs(data);

  ଯଦି(!result)
    /* If there is a list of hsts files to read */
    result = Curl_hsts_loadfiles(data);

  ଯଦି(!result) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    data->state.allow_port = TRUE;

#ଯଦି defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    ଯଦି(!data->set.no_signal)
      data->state.prev_signal = ସଙ୍କେତ(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(data); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(data);
    Curl_pgrsStart(data, ଖାଲି);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    data->state.authhost.picked &= data->state.authhost.want;
    data->state.authproxy.picked &= data->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    data->state.wildcardmatch = data->set.wildcard_enabled;
#ifdef DEBUGBUILD
  ଯଦି(getenv("CURL_DBG_FTP_WILDCARD"))
    data->state.wildcardmatch = TRUE;
#endif
    ଯଦି(data->state.wildcardmatch) {
      ସଂରଚନା WildcardData *wc;
      ଯଦି(!data->wildcard) {
        data->wildcard = curlx_calloc(1, ଆକାର(ସଂରଚନା WildcardData));
        ଯଦି(!data->wildcard)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      wc = data->wildcard;
      ଯଦି(wc->state < CURLWC_INIT) {
        ଯଦି(wc->ftpwc)
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
  ଫେରନ୍ତୁ result;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର **url)
{
  ସଂରଚନା connectdata *conn = data->conn;
  bool retry = FALSE;
  *url = ଖାଲି;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  ଯଦି(data->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(conn->bits.reuse &&
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
  ଅନ୍ୟଥା ଯଦି(data->state.refused_stream &&
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
  ଯଦି(retry) {
#define CONN_MAX_RETRIES 5
    ଯଦି(data->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(data, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      data->state.retrycount = 0;
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    infof(data, "Connection died, retrying a fresh connect (retry count: %d)",
          data->state.retrycount);
    *url = Curl_bufref_dup(&data->state.url);
    ଯଦି(!*url)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(data, TRUE);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ xfer_setup(
  ସଂରଚନା Curl_easy *data,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_size      /* how much to receive, -1 if unknown */
  )
{
  ସଂରଚନା SingleRequest *k = &data->req;
  ସଂରଚନା connectdata *conn = data->conn;

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
  ଯଦି(!k->header && (recv_size > 0))
    Curl_pgrsSetDownloadSize(data, recv_size);

  /* we want header and/or body, if neither then do not do this! */
  ଯଦି(conn->scheme->run->write_resp_hd || !data->req.no_body) {
    ଯଦି(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(data);
    ଯଦି(conn->send_idx != -1)
      CURL_REQ_SET_SEND(data);
  }
  CURL_TRC_M(data, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

ଶୂନ୍ୟ Curl_xfer_setup_nop(ସଂରଚନା Curl_easy *data)
{
  xfer_setup(data, -1, -1, -1);
}

ଶୂନ୍ୟ Curl_xfer_setup_sendrecv(ସଂରଚନା Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size)
{
  xfer_setup(data, sockindex, sockindex, recv_size);
}

ଶୂନ୍ୟ Curl_xfer_setup_send(ସଂରଚନା Curl_easy *data,
                          int8_t sockindex)
{
  xfer_setup(data, sockindex, -1, -1);
}

ଶୂନ୍ୟ Curl_xfer_setup_recv(ସଂରଚନା Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size)
{
  xfer_setup(data, -1, sockindex, recv_size);
}

ଶୂନ୍ୟ Curl_xfer_set_shutdown(ସଂରଚନା Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (data->conn->send_idx < 0) || (data->conn->recv_idx < 0));
  data->req.shutdown = shutdown;
  data->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result = CURLE_OK;

  ଯଦି(data->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    result = data->conn->scheme->run->write_resp(data, buf, blen, is_eos);
  }
  ଅନ୍ୟଥା {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    ଯଦି(blen || is_eos) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cwtype = CLIENTWRITE_BODY;
      ଯଦି(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      result = Curl_client_write(data, cwtype, buf, blen);
    }
  }

  ଯଦି(!result && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    data->req.eos_written = TRUE;
    data->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(data, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

bool Curl_xfer_write_is_paused(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_cwriter_is_paused(data);
}

CURLcode Curl_xfer_write_resp_hd(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *hd0, size_t hdlen, bool is_eos)
{
  ଯଦି(data->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ଫେରନ୍ତୁ data->conn->scheme->run->write_resp_hd(data, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  ଫେରନ୍ତୁ Curl_xfer_write_resp(data, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(ସଂରଚନା Curl_easy *data, bool premature)
{
  (ଶୂନ୍ୟ)premature;
  ଫେରନ୍ତୁ Curl_cw_out_done(data);
}

bool Curl_xfer_needs_flush(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_conn_needs_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_flush(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_conn_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_send(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode result;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  result = Curl_conn_send(data, data->conn->send_idx,
                          buf, blen, eos, pnwritten);
  ଯଦି(result == CURLE_AGAIN) {
    result = CURLE_OK;
    *pnwritten = 0;
  }
  ଅନ୍ୟଥା ଯଦି(!result && *pnwritten)
    data->info.request_size += *pnwritten;

  DEBUGF(infof(data, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten));
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_xfer_recv(ସଂରଚନା Curl_easy *data,
                        ଅକ୍ଷର *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(data->set.buffer_size > 0);

  ଯଦି(curlx_uitouz(data->set.buffer_size) < blen)
    blen = curlx_uitouz(data->set.buffer_size);
  ଫେରନ୍ତୁ Curl_conn_recv(data, data->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(ସଂରଚନା Curl_easy *data)
{
  Curl_conn_ev_data_done_send(data);
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_xfer_is_blocked(ସଂରଚନା Curl_easy *data)
{
  bool want_send = CURL_REQ_WANT_SEND(data);
  bool want_recv = CURL_REQ_WANT_RECV(data);
  ଯଦି(!want_send)
    ଫେରନ୍ତୁ want_recv && Curl_xfer_recv_is_paused(data);
  ଅନ୍ୟଥା ଯଦି(!want_recv)
    ଫେରନ୍ତୁ want_send && Curl_xfer_send_is_paused(data);
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ Curl_xfer_recv_is_paused(data) && Curl_xfer_send_is_paused(data);
}

bool Curl_xfer_send_is_paused(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_rlimit_is_blocked(&data->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_rlimit_is_blocked(&data->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(ସଂରଚନା Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.ul.rlimit, enable, Curl_pgrs_now(data));
  ଯଦି(!enable && Curl_creader_is_paused(data))
    result = Curl_creader_unpause(data);
  Curl_pgrsSendPause(data, enable);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_xfer_pause_recv(ସଂରଚନା Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.dl.rlimit, enable, Curl_pgrs_now(data));
  ଯଦି(!enable && Curl_cwriter_is_paused(data))
    result = Curl_cwriter_unpause(data);
  Curl_conn_ev_data_pause(data, enable);
  Curl_pgrsRecvPause(data, enable);
  ଫେରନ୍ତୁ result;
}

bool Curl_xfer_is_secure(ସଂରଚନା Curl_easy *data)
{
#ifndef CURL_DISABLE_PROXY
  ଯଦି(data->conn && data->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(data->state.origin->scheme->flags & PROTOPT_SSL));
    ଫେରନ୍ତୁ FALSE;
  }
#endif
  ଫେରନ୍ତୁ (data->state.origin->scheme->flags & PROTOPT_SSL);
}
