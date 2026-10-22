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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#شامل <signal.h>

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنہ_اگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_SOCKET
#غلطی "We cannot compile without socket() support"
#اگر_ختم

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
حرف *Curl_checkheaders(ثابت ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *thisheader,
                        ثابت size_t thislen)
{
  ساخت curl_slist *سر;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  برائے(سر = ڈیٹا->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    اگر(curl_strnequal(سر->ڈیٹا, thisheader, thislen) &&
       Curl_headersep(سر->ڈیٹا[thislen]))
      واپس سر->ڈیٹا;
  }

  واپس تہی;
}
#اگر_ختم

ساکن صحیح ڈیٹا_pending(ساخت Curl_easy *ڈیٹا, bool rcvd_eagain)
{
  ساخت connectdata *conn = ڈیٹا->conn;

  اگر(conn->scheme->protocol & PROTO_FAMILY_FTP)
    واپس Curl_conn_ڈیٹا_pending(ڈیٹا, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  واپس (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(ساخت Curl_easy *ڈیٹا, time_t timeofdoc)
{
  اگر((timeofdoc == 0) || (ڈیٹا->سیٹ.timevalue == 0))
    واپس TRUE;

  انتخاب(ڈیٹا->سیٹ.timecondition) {
  حالت CURL_TIMECOND_IFMODSINCE:
  طےشدہ:
    اگر(timeofdoc <= ڈیٹا->سیٹ.timevalue) {
      infof(ڈیٹا, "The requested document is not new enough");
      ڈیٹا->info.timecond = TRUE;
      واپس FALSE;
    }
    توڑو;
  حالت CURL_TIMECOND_IFUNMODSINCE:
    اگر(timeofdoc >= ڈیٹا->سیٹ.timevalue) {
      infof(ڈیٹا, "The requested document is not old enough");
      ڈیٹا->info.timecond = TRUE;
      واپس FALSE;
    }
    توڑو;
  }

  واپس TRUE;
}

ساکن CURLcode xfer_recv_shutdown(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  اگر(!ڈیٹا || !ڈیٹا->conn)
    واپس CURLE_FAILED_INIT;
  واپس Curl_cshutdn_try_once_idx(ڈیٹا, ڈیٹا->conn->recv_idx, مکمل);
}

ساکن bool xfer_recv_shutdown_started(ساخت Curl_easy *ڈیٹا)
{
  اگر(!ڈیٹا || !ڈیٹا->conn)
    واپس FALSE;
  واپس CURL_CONN_اندر_SHUTDOWN(ڈیٹا->conn, ڈیٹا->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  اگر(!ڈیٹا || !ڈیٹا->conn)
    واپس CURLE_FAILED_INIT;
  واپس Curl_cshutdn_try_once_idx(ڈیٹا, ڈیٹا->conn->send_idx, مکمل);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ساکن CURLcode xfer_recv_resp(ساخت Curl_easy *ڈیٹا,
                               حرف *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode نتیجہ;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  اگر(!eos_reliable && !ڈیٹا->req.سرخی && ڈیٹا->req.سائز != -1) {
    blen = curlx_sotouz_range(ڈیٹا->req.سائز - ڈیٹا->req.bytecount, 0, blen);
  }
  ورنہ اگر(xfer_recv_shutdown_started(ڈیٹا)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  اگر(blen) {
    نتیجہ = Curl_xfer_recv(ڈیٹا, buf, blen, pnread);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(*pnread == 0) {
    اگر(ڈیٹا->req.shutdown) {
      bool مکمل;
      نتیجہ = xfer_recv_shutdown(ڈیٹا, &مکمل);
      اگر(نتیجہ)
        واپس نتیجہ;
      اگر(!مکمل) {
        واپس CURLE_AGAIN;
      }
    }
    DEBUGF(infof(ڈیٹا, "sendrecv_dl: we are done"));
  }
  واپس CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ساکن CURLcode sendrecv_dl(ساخت Curl_easy *ڈیٹا,
                            ساخت SingleRequest *k)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;
  حرف *buf, *xfer_buf;
  size_t blen, xfer_blen;
  صحیح maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, شرح_limited = FALSE;

  نتیجہ = Curl_multi_xfer_buf_borrow(ڈیٹا, &xfer_buf, &xfer_blen);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  کرو {
    size_t bytestoread;

    اگر(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    اگر(bytestoread && Curl_rlimit_فعال(&ڈیٹا->پیش_رفت.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&ڈیٹا->پیش_رفت.dl.rlimit, تہی);
#اگر 0
      DEBUGF(infof(ڈیٹا, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#اگر_ختم
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      اگر(dl_avail <= 0) {
        شرح_limited = TRUE;
        توڑو;
      }
      اگر(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    نتیجہ = xfer_recv_resp(ڈیٹا, buf, bytestoread, is_multiplex, &blen);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN)
        جاؤ برآمد; /* real error */
      rcvd_eagain = TRUE;
      نتیجہ = CURLE_OK;
      اگر(ڈیٹا->req.download_مکمل && ڈیٹا->req.no_body &&
         !ڈیٹا->req.resp_trailer) {
        DEBUGF(infof(ڈیٹا, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      ورنہ
        توڑو; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    اگر(!blen) {
      نتیجہ = Curl_req_stop_send_recv(ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
      اگر(k->eos_written) /* already did write this to client, leave */
        توڑو;
    }

    نتیجہ = Curl_xfer_write_resp(ڈیٹا, buf, blen, is_eos);
    اگر(نتیجہ || ڈیٹا->req.مکمل)
      جاؤ برآمد;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    اگر((!is_multiplex && ڈیٹا->req.download_مکمل) || is_eos) {
      CURL_REQ_CLEAR_RECV(ڈیٹا);
    }
    /* if we stopped receiving, leave the loop */
    اگر(!CURL_REQ_WANT_RECV(ڈیٹا))
      توڑو;

  } جب(maxloops--);

  اگر(!is_eos && !شرح_limited && CURL_REQ_WANT_RECV(ڈیٹا) &&
     (!rcvd_eagain || ڈیٹا_pending(ڈیٹا, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(ڈیٹا);
    CURL_TRC_M(ڈیٹا, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  اگر(!CURL_REQ_WANT_RECV(ڈیٹا) && CURL_REQ_WANT_SEND(ڈیٹا) &&
     (conn->بٹ.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(ڈیٹا, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(ڈیٹا);
  }

برآمد:
  Curl_multi_xfer_buf_release(ڈیٹا, xfer_buf);
  اگر(نتیجہ)
    DEBUGF(infof(ڈیٹا, "sendrecv_dl() -> %d", (صحیح)نتیجہ));
  واپس نتیجہ;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ساکن CURLcode sendrecv_ul(ساخت Curl_easy *ڈیٹا)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_مکمل_sending(ڈیٹا));

  اگر(!Curl_req_مکمل_sending(ڈیٹا))
    واپس Curl_req_send_مزید(ڈیٹا);
  واپس CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(ساخت Curl_easy *ڈیٹا)
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  ثابت ساخت curltime *pnow = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(Curl_xfer_is_blocked(ڈیٹا)) {
    نتیجہ = CURLE_OK;
    جاؤ برآمد;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  اگر(CURL_REQ_WANT_RECV(ڈیٹا)) {
    نتیجہ = sendrecv_dl(ڈیٹا, k);
    اگر(نتیجہ || ڈیٹا->req.مکمل)
      جاؤ برآمد;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  اگر(Curl_req_want_send(ڈیٹا)) {
    نتیجہ = sendrecv_ul(ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  pnow = Curl_pgrs_now(ڈیٹا);
  اگر(CURL_REQ_WANT_IO(ڈیٹا)) {
    اگر(Curl_timeleft_now_ms(ڈیٹا, pnow) < 0) {
      اگر(k->سائز != -1) {
        failf(ڈیٹا, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE),
              k->bytecount, k->سائز);
      }
      ورنہ {
        failf(ڈیٹا, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE),
              k->bytecount);
      }
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      جاؤ برآمد;
    }
  }
  ورنہ {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    اگر(!ڈیٹا->req.no_body && (k->سائز != -1) &&
       (k->bytecount != k->سائز) && !k->newurl) {
      failf(ڈیٹا, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->سائز - k->bytecount);
      نتیجہ = CURLE_PARTIAL_FILE;
      جاؤ برآمد;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  اگر(!CURL_REQ_WANT_IO(ڈیٹا))
    ڈیٹا->req.مکمل = TRUE;

  نتیجہ = Curl_pgrsCheckX(ڈیٹا, pnow);

برآمد:
  اگر(نتیجہ)
    DEBUGF(infof(ڈیٹا, "Curl_sendrecv() -> %d", (صحیح)نتیجہ));
  واپس نتیجہ;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
خالی Curl_init_CONNECT(ساخت Curl_easy *ڈیٹا)
{
  ڈیٹا->state.fread_func = ڈیٹا->سیٹ.fread_func_سیٹ;
  ڈیٹا->state.اندر = ڈیٹا->سیٹ.اندر_سیٹ;
  ڈیٹا->state.upload = (ڈیٹا->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  ڈیٹا->state.retrycount = 0;

  اگر(!CURL_EASY_STR(ڈیٹا, STRING_سیٹ_URL) && !ڈیٹا->سیٹ.uh) {
    /* we cannot do anything without URL */
    failf(ڈیٹا, "No URL set");
    واپس CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  اگر(ڈیٹا->سیٹ.uh) {
    حرف *url = تہی;
    CURLUcode uc;
    uc = curl_url_get(ڈیٹا->سیٹ.uh, CURLUPART_URL, &url, 0);
    اگر(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_سیٹ(&ڈیٹا->state.url, تہی, 0, تہی);
      failf(ڈیٹا, "No URL set");
      واپس CURLE_URL_MALFORMAT;
    }
    نتیجہ = CURL_EASY_STR_SETN(ڈیٹا, STRING_سیٹ_URL, url);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  Curl_bufref_سیٹ(&ڈیٹا->state.url, CURL_EASY_STR(ڈیٹا, STRING_سیٹ_URL),
                  0, تہی);

  اگر(ڈیٹا->سیٹ.postfields && ڈیٹا->سیٹ.سیٹ_resume_from) {
    /* we cannot */
    failf(ڈیٹا, "cannot mix POSTFIELDS with RESUME_FROM");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  ڈیٹا->state.prefer_ascii = ڈیٹا->سیٹ.prefer_ascii;
#اگر_تعریف_شدہ CURL_LIST_ONLY_PROTOCOL
  ڈیٹا->state.list_only = ڈیٹا->سیٹ.list_only;
#اگر_ختم
  ڈیٹا->state.httpreq = ڈیٹا->سیٹ.طریقہ;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&ڈیٹا->state.initial_origin);
  Curl_peer_unlink(&ڈیٹا->state.origin);
  ڈیٹا->state.درخواستیں = 0;
  ڈیٹا->state.followlocation = 0; /* reset the location-follow counter */
  ڈیٹا->state.this_is_a_follow = FALSE; /* reset this */
  ڈیٹا->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  ڈیٹا->state.errorbuf = FALSE; /* no error has occurred */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  Curl_http_neg_init(ڈیٹا, &ڈیٹا->state.http_neg);
#اگر_ختم
  ڈیٹا->state.authproblem = FALSE;
  ڈیٹا->state.authhost.want = ڈیٹا->سیٹ.httpauth;
  ڈیٹا->state.authproxy.want = ڈیٹا->سیٹ.proxyauth;
  curlx_safefree(ڈیٹا->info.wouldredirect);
  curlx_safefree(ڈیٹا->info.effective_url);
  Curl_ڈیٹا_ترجیح_clear_state(ڈیٹا);
  اگر(ڈیٹا->سیٹ.http_auto_referer)
    Curl_bufref_free(&ڈیٹا->state.referer);
  اگر(CURL_EASY_STR(ڈیٹا, STRING_سیٹ_REFERER))
    Curl_bufref_سیٹ(&ڈیٹا->state.referer,
                    CURL_EASY_STR(ڈیٹا, STRING_سیٹ_REFERER), 0, تہی);
  ورنہ
    Curl_bufref_free(&ڈیٹا->state.referer);

  اگر(ڈیٹا->state.httpreq == HTTPREQ_PUT)
    ڈیٹا->state.infilesize = ڈیٹا->سیٹ.filesize;
  ورنہ اگر((ڈیٹا->state.httpreq != HTTPREQ_GET) &&
          (ڈیٹا->state.httpreq != HTTPREQ_سر)) {
    ڈیٹا->state.infilesize = ڈیٹا->سیٹ.postfieldsize;
    اگر(ڈیٹا->سیٹ.postfields && (ڈیٹا->state.infilesize == -1))
      ڈیٹا->state.infilesize = (curl_off_t)لڑی_لمبائی(ڈیٹا->سیٹ.postfields);
  }
  ورنہ
    ڈیٹا->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  نتیجہ = Curl_cookie_loadfiles(ڈیٹا,
                                 ڈیٹا->سیٹ.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  اگر(!نتیجہ)
    Curl_cookie_تسلسل(ڈیٹا); /* activate */

  /* If there is a list of host pairs to deal with */
  اگر(!نتیجہ && ڈیٹا->state.resolve)
    نتیجہ = Curl_loadhostpairs(ڈیٹا);

  اگر(!نتیجہ)
    /* If there is a list of hsts files to read */
    نتیجہ = Curl_hsts_loadfiles(ڈیٹا);

  اگر(!نتیجہ) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    ڈیٹا->state.allow_پورٹ = TRUE;

#اگر defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    اگر(!ڈیٹا->سیٹ.no_signal)
      ڈیٹا->state.پچھلا_signal = اشارہ(SIGPIPE, SIG_IGN);
#اگر_ختم

    Curl_initinfo(ڈیٹا); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(ڈیٹا);
    Curl_pgrs_آغاز(ڈیٹا, تہی);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    ڈیٹا->state.authhost.picked &= ڈیٹا->state.authhost.want;
    ڈیٹا->state.authproxy.picked &= ڈیٹا->state.authproxy.want;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
    ڈیٹا->state.wildcardmatch = ڈیٹا->سیٹ.wildcard_enabled;
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(getenv("CURL_DBG_FTP_WILDCARD"))
    ڈیٹا->state.wildcardmatch = TRUE;
#اگر_ختم
    اگر(ڈیٹا->state.wildcardmatch) {
      ساخت Wildcard_ڈیٹا *wc;
      اگر(!ڈیٹا->wildcard) {
        ڈیٹا->wildcard = curlx_calloc(1, حجم(ساخت Wildcard_ڈیٹا));
        اگر(!ڈیٹا->wildcard)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      wc = ڈیٹا->wildcard;
      اگر(wc->state < CURLWC_INIT) {
        اگر(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->نمونہ);
        curlx_safefree(wc->راستہ);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#اگر_ختم
    نتیجہ = Curl_hsts_loadcb(ڈیٹا, ڈیٹا->hsts);
  }

  ڈیٹا->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  ڈیٹا->req.rewind_read = FALSE;
  Curl_client_reset(ڈیٹا);
  Curl_سرخیاں_cleanup(ڈیٹا);
  واپس نتیجہ;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(ساخت Curl_easy *ڈیٹا, حرف **url)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  bool retry = FALSE;
  *url = تہی;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  اگر(ڈیٹا->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    واپس CURLE_OK;

  اگر(conn->بٹ.reuse &&
     (ڈیٹا->req.bytecount + ڈیٹا->req.headerbytecount == 0) &&
     ((!ڈیٹا->req.no_body && !ڈیٹا->req.مکمل) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
     && (ڈیٹا->سیٹ.rtspreq != RTSPREQ_RECEIVE)
#اگر_ختم
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  ورنہ اگر(ڈیٹا->state.refused_دھارا &&
          (ڈیٹا->req.bytecount + ڈیٹا->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(ڈیٹا, "REFUSED_STREAM, retrying a fresh connect");
    ڈیٹا->state.refused_دھارا = FALSE; /* clear again */
    retry = TRUE;
  }
  اگر(retry) {
#تعریف_کرو CONN_MAX_RETRIES 5
    اگر(ڈیٹا->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(ڈیٹا, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      ڈیٹا->state.retrycount = 0;
      واپس CURLE_SEND_ERROR;
    }
    infof(ڈیٹا, "Connection died, retrying a fresh connect (retry count: %d)",
          ڈیٹا->state.retrycount);
    *url = Curl_bufref_dup(&ڈیٹا->state.url);
    اگر(!*url)
      واپس CURLE_برآمد_کا_حافظہ;

    connclose(conn); /* close this connection */
    conn->بٹ.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_سیٹ_rewind(ڈیٹا, TRUE);
  }
  واپس CURLE_OK;
}

ساکن خالی xfer_تیاری(
  ساخت Curl_easy *ڈیٹا,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_سائز      /* how much to receive, -1 if unknown */
  )
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  ساخت connectdata *conn = ڈیٹا->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(ڈیٹا));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_سائز == -1));
  k->سائز = recv_سائز;
  k->سرخی = !!conn->scheme->تسلسل->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  اگر(!k->سرخی && (recv_سائز > 0))
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, recv_سائز);

  /* we want header and/or body, if neither then do not do this! */
  اگر(conn->scheme->تسلسل->write_resp_hd || !ڈیٹا->req.no_body) {
    اگر(conn->recv_idx != -1)
      CURL_REQ_سیٹ_RECV(ڈیٹا);
    اگر(conn->send_idx != -1)
      CURL_REQ_سیٹ_SEND(ڈیٹا);
  }
  CURL_TRC_M(ڈیٹا, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

خالی Curl_xfer_تیاری_nop(ساخت Curl_easy *ڈیٹا)
{
  xfer_تیاری(ڈیٹا, -1, -1, -1);
}

خالی Curl_xfer_تیاری_sendrecv(ساخت Curl_easy *ڈیٹا,
                              int8_t sockindex,
                              curl_off_t recv_سائز)
{
  xfer_تیاری(ڈیٹا, sockindex, sockindex, recv_سائز);
}

خالی Curl_xfer_تیاری_send(ساخت Curl_easy *ڈیٹا,
                          int8_t sockindex)
{
  xfer_تیاری(ڈیٹا, sockindex, -1, -1);
}

خالی Curl_xfer_تیاری_recv(ساخت Curl_easy *ڈیٹا,
                          int8_t sockindex,
                          curl_off_t recv_سائز)
{
  xfer_تیاری(ڈیٹا, -1, sockindex, recv_سائز);
}

خالی Curl_xfer_سیٹ_shutdown(ساخت Curl_easy *ڈیٹا,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (ڈیٹا->conn->send_idx < 0) || (ڈیٹا->conn->recv_idx < 0));
  ڈیٹا->req.shutdown = shutdown;
  ڈیٹا->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ڈیٹا->conn->scheme->تسلسل->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    نتیجہ = ڈیٹا->conn->scheme->تسلسل->write_resp(ڈیٹا, buf, blen, is_eos);
  }
  ورنہ {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    اگر(blen || is_eos) {
      صحیح cwtype = CLIENTWRITE_BODY;
      اگر(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      نتیجہ = Curl_client_write(ڈیٹا, cwtype, buf, blen);
    }
  }

  اگر(!نتیجہ && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    ڈیٹا->req.eos_written = TRUE;
    ڈیٹا->req.download_مکمل = TRUE;
  }
  CURL_TRC_WRITE(ڈیٹا, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (صحیح)نتیجہ);
  واپس نتیجہ;
}

bool Curl_xfer_write_is_paused(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_cwriter_is_paused(ڈیٹا);
}

CURLcode Curl_xfer_write_resp_hd(ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *hd0, size_t hdlen, bool is_eos)
{
  اگر(ڈیٹا->conn->scheme->تسلسل->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    واپس ڈیٹا->conn->scheme->تسلسل->write_resp_hd(ڈیٹا, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  واپس Curl_xfer_write_resp(ڈیٹا, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_مکمل(ساخت Curl_easy *ڈیٹا, bool premature)
{
  (خالی)premature;
  واپس Curl_cw_برآمد_مکمل(ڈیٹا);
}

bool Curl_xfer_needs_flush(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_conn_needs_flush(ڈیٹا, ڈیٹا->conn->send_idx);
}

CURLcode Curl_xfer_flush(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_conn_flush(ڈیٹا, ڈیٹا->conn->send_idx);
}

CURLcode Curl_xfer_send(ساخت Curl_easy *ڈیٹا,
                        ثابت خالی *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);

  نتیجہ = Curl_conn_send(ڈیٹا, ڈیٹا->conn->send_idx,
                          buf, blen, eos, pnwritten);
  اگر(نتیجہ == CURLE_AGAIN) {
    نتیجہ = CURLE_OK;
    *pnwritten = 0;
  }
  ورنہ اگر(!نتیجہ && *pnwritten)
    ڈیٹا->info.request_سائز += *pnwritten;

  DEBUGF(infof(ڈیٹا, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (صحیح)نتیجہ, *pnwritten));
  واپس نتیجہ;
}

CURLcode Curl_xfer_recv(ساخت Curl_easy *ڈیٹا,
                        حرف *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  DEBUGASSERT(ڈیٹا->سیٹ.بفر_سائز > 0);

  اگر(curlx_uitouz(ڈیٹا->سیٹ.بفر_سائز) < blen)
    blen = curlx_uitouz(ڈیٹا->سیٹ.بفر_سائز);
  واپس Curl_conn_recv(ڈیٹا, ڈیٹا->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(ساخت Curl_easy *ڈیٹا)
{
  Curl_conn_ev_ڈیٹا_مکمل_send(ڈیٹا);
  واپس CURLE_OK;
}

bool Curl_xfer_is_blocked(ساخت Curl_easy *ڈیٹا)
{
  bool want_send = CURL_REQ_WANT_SEND(ڈیٹا);
  bool want_recv = CURL_REQ_WANT_RECV(ڈیٹا);
  اگر(!want_send)
    واپس want_recv && Curl_xfer_recv_is_paused(ڈیٹا);
  ورنہ اگر(!want_recv)
    واپس want_send && Curl_xfer_send_is_paused(ڈیٹا);
  ورنہ
    واپس Curl_xfer_recv_is_paused(ڈیٹا) && Curl_xfer_send_is_paused(ڈیٹا);
}

bool Curl_xfer_send_is_paused(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_rlimit_is_blocked(&ڈیٹا->پیش_رفت.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_rlimit_is_blocked(&ڈیٹا->پیش_رفت.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(ساخت Curl_easy *ڈیٹا, bool enable)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_rlimit_بلاک(&ڈیٹا->پیش_رفت.ul.rlimit, enable, Curl_pgrs_now(ڈیٹا));
  اگر(!enable && Curl_creader_is_paused(ڈیٹا))
    نتیجہ = Curl_creader_unpause(ڈیٹا);
  Curl_pgrsSendPause(ڈیٹا, enable);
  واپس نتیجہ;
}

CURLcode Curl_xfer_pause_recv(ساخت Curl_easy *ڈیٹا, bool enable)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_rlimit_بلاک(&ڈیٹا->پیش_رفت.dl.rlimit, enable, Curl_pgrs_now(ڈیٹا));
  اگر(!enable && Curl_cwriter_is_paused(ڈیٹا))
    نتیجہ = Curl_cwriter_unpause(ڈیٹا);
  Curl_conn_ev_ڈیٹا_pause(ڈیٹا, enable);
  Curl_pgrsRecvPause(ڈیٹا, enable);
  واپس نتیجہ;
}

bool Curl_xfer_is_secure(ساخت Curl_easy *ڈیٹا)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(ڈیٹا->conn && ڈیٹا->conn->بٹ.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(ڈیٹا->state.origin->scheme->flags & PROTOPT_SSL));
    واپس FALSE;
  }
#اگر_ختم
  واپس (ڈیٹا->state.origin->scheme->flags & PROTOPT_SSL);
}
