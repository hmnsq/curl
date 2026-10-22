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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_SELECT_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/select.h>
#ਜਾਂਜੇ defined(HAVE_UNISTD_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKET
#ਗਲਤੀ "We cannot compile without socket() support"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-out.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "getinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "headers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rtsp.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
ਅੱਖਰ *Curl_checkheaders(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *thisheader,
                        ਅਚਲ size_t thislen)
{
  ਬਣਤਰ curl_slist *ਸਿਰ;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  ਲਈ(ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ; ਸਿਰ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
    ਜੇ(curl_strnequal(ਸਿਰ->ਅੰਕੜੇ, thisheader, thislen) &&
       Curl_headersep(ਸਿਰ->ਅੰਕੜੇ[thislen]))
      ਵਾਪਸ ਸਿਰ->ਅੰਕੜੇ;
  }

  ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool rcvd_eagain)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  ਜੇ(conn->scheme->protocol & PROTO_FAMILY_FTP)
    ਵਾਪਸ Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  ਵਾਪਸ (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, time_t timeofdoc)
{
  ਜੇ((timeofdoc == 0) || (ਅੰਕੜੇ->ਸੈੱਟ.timevalue == 0))
    ਵਾਪਸ TRUE;

  ਚੋਣ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition) {
  ਹਾਲਤ CURL_TIMECOND_IFMODSINCE:
  ਮੂਲ:
    ਜੇ(timeofdoc <= ਅੰਕੜੇ->ਸੈੱਟ.timevalue) {
      infof(ਅੰਕੜੇ, "The requested document is not new enough");
      ਅੰਕੜੇ->info.timecond = TRUE;
      ਵਾਪਸ FALSE;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURL_TIMECOND_IFUNMODSINCE:
    ਜੇ(timeofdoc >= ਅੰਕੜੇ->ਸੈੱਟ.timevalue) {
      infof(ਅੰਕੜੇ, "The requested document is not old enough");
      ਅੰਕੜੇ->info.timecond = TRUE;
      ਵਾਪਸ FALSE;
    }
    ਰੋਕੋ;
  }

  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode xfer_recv_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->conn)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_cshutdn_try_once_idx(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->recv_idx, ਪੂਰਾ);
}

ਸਥਿਰ bool xfer_recv_shutdown_started(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->conn)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ CURL_CONN_ਵਿੱਚ_SHUTDOWN(ਅੰਕੜੇ->conn, ਅੰਕੜੇ->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->conn)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_cshutdn_try_once_idx(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->send_idx, ਪੂਰਾ);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ਸਥਿਰ CURLcode xfer_recv_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅੱਖਰ *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  ਜੇ(!eos_reliable && !ਅੰਕੜੇ->req.ਸਿਰਲੇਖ && ਅੰਕੜੇ->req.ਮਾਪ != -1) {
    blen = curlx_sotouz_range(ਅੰਕੜੇ->req.ਮਾਪ - ਅੰਕੜੇ->req.bytecount, 0, blen);
  }
  ਨਹੀਂਤਾਂ ਜੇ(xfer_recv_shutdown_started(ਅੰਕੜੇ)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  ਜੇ(blen) {
    ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, buf, blen, pnread);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(*pnread == 0) {
    ਜੇ(ਅੰਕੜੇ->req.shutdown) {
      bool ਪੂਰਾ;
      ਨਤੀਜਾ = xfer_recv_shutdown(ਅੰਕੜੇ, &ਪੂਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਜੇ(!ਪੂਰਾ) {
        ਵਾਪਸ CURLE_AGAIN;
      }
    }
    DEBUGF(infof(ਅੰਕੜੇ, "sendrecv_dl: we are done"));
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ਸਥਿਰ CURLcode sendrecv_dl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ SingleRequest *k)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *buf, *xfer_buf;
  size_t blen, xfer_blen;
  ਪੂਰਨ maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, ਦਰ_limited = FALSE;

  ਨਤੀਜਾ = Curl_multi_xfer_buf_borrow(ਅੰਕੜੇ, &xfer_buf, &xfer_blen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  ਕਰੋ {
    size_t bytestoread;

    ਜੇ(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    ਜੇ(bytestoread && Curl_rlimit_ਸਰਗਰਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, ਨਿਰਾ);
#ਜੇ 0
      DEBUGF(infof(ਅੰਕੜੇ, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#ਜੇ_ਅੰਤ
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      ਜੇ(dl_avail <= 0) {
        ਦਰ_limited = TRUE;
        ਰੋਕੋ;
      }
      ਜੇ(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    ਨਤੀਜਾ = xfer_recv_resp(ਅੰਕੜੇ, buf, bytestoread, is_multiplex, &blen);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
        ਜਾਓ ਬਾਹਰਲਾ; /* real error */
      rcvd_eagain = TRUE;
      ਨਤੀਜਾ = CURLE_OK;
      ਜੇ(ਅੰਕੜੇ->req.download_ਪੂਰਾ && ਅੰਕੜੇ->req.no_body &&
         !ਅੰਕੜੇ->req.resp_trailer) {
        DEBUGF(infof(ਅੰਕੜੇ, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      ਨਹੀਂਤਾਂ
        ਰੋਕੋ; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    ਜੇ(!blen) {
      ਨਤੀਜਾ = Curl_req_stop_send_recv(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਜੇ(k->eos_written) /* already did write this to client, leave */
        ਰੋਕੋ;
    }

    ਨਤੀਜਾ = Curl_xfer_write_resp(ਅੰਕੜੇ, buf, blen, is_eos);
    ਜੇ(ਨਤੀਜਾ || ਅੰਕੜੇ->req.ਪੂਰਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    ਜੇ((!is_multiplex && ਅੰਕੜੇ->req.download_ਪੂਰਾ) || is_eos) {
      CURL_REQ_CLEAR_RECV(ਅੰਕੜੇ);
    }
    /* if we stopped receiving, leave the loop */
    ਜੇ(!CURL_REQ_WANT_RECV(ਅੰਕੜੇ))
      ਰੋਕੋ;

  } ਜਦੋਂ(maxloops--);

  ਜੇ(!is_eos && !ਦਰ_limited && CURL_REQ_WANT_RECV(ਅੰਕੜੇ) &&
     (!rcvd_eagain || ਅੰਕੜੇ_pending(ਅੰਕੜੇ, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    CURL_TRC_M(ਅੰਕੜੇ, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  ਜੇ(!CURL_REQ_WANT_RECV(ਅੰਕੜੇ) && CURL_REQ_WANT_SEND(ਅੰਕੜੇ) &&
     (conn->ਬਿੱਟ.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(ਅੰਕੜੇ, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(ਅੰਕੜੇ);
  }

ਬਾਹਰਲਾ:
  Curl_multi_xfer_buf_release(ਅੰਕੜੇ, xfer_buf);
  ਜੇ(ਨਤੀਜਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "sendrecv_dl() -> %d", (ਪੂਰਨ)ਨਤੀਜਾ));
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ਸਥਿਰ CURLcode sendrecv_ul(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_ਪੂਰਾ_sending(ਅੰਕੜੇ));

  ਜੇ(!Curl_req_ਪੂਰਾ_sending(ਅੰਕੜੇ))
    ਵਾਪਸ Curl_req_send_ਹੋਰ(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਅਚਲ ਬਣਤਰ curltime *pnow = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(Curl_xfer_is_blocked(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = CURLE_OK;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  ਜੇ(CURL_REQ_WANT_RECV(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = sendrecv_dl(ਅੰਕੜੇ, k);
    ਜੇ(ਨਤੀਜਾ || ਅੰਕੜੇ->req.ਪੂਰਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  ਜੇ(Curl_req_want_send(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = sendrecv_ul(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  ਜੇ(CURL_REQ_WANT_IO(ਅੰਕੜੇ)) {
    ਜੇ(Curl_timeleft_now_ms(ਅੰਕੜੇ, pnow) < 0) {
      ਜੇ(k->ਮਾਪ != -1) {
        failf(ਅੰਕੜੇ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE),
              k->bytecount, k->ਮਾਪ);
      }
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE),
              k->bytecount);
      }
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    ਜੇ(!ਅੰਕੜੇ->req.no_body && (k->ਮਾਪ != -1) &&
       (k->bytecount != k->ਮਾਪ) && !k->newurl) {
      failf(ਅੰਕੜੇ, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->ਮਾਪ - k->bytecount);
      ਨਤੀਜਾ = CURLE_PARTIAL_FILE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  ਜੇ(!CURL_REQ_WANT_IO(ਅੰਕੜੇ))
    ਅੰਕੜੇ->req.ਪੂਰਾ = TRUE;

  ਨਤੀਜਾ = Curl_pgrsCheckX(ਅੰਕੜੇ, pnow);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "Curl_sendrecv() -> %d", (ਪੂਰਨ)ਨਤੀਜਾ));
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
ਖਾਲੀ Curl_init_CONNECT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅੰਕੜੇ->state.fread_func = ਅੰਕੜੇ->ਸੈੱਟ.fread_func_ਸੈੱਟ;
  ਅੰਕੜੇ->state.ਵਿੱਚ = ਅੰਕੜੇ->ਸੈੱਟ.ਵਿੱਚ_ਸੈੱਟ;
  ਅੰਕੜੇ->state.upload = (ਅੰਕੜੇ->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  ਅੰਕੜੇ->state.retrycount = 0;

  ਜੇ(!CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL) && !ਅੰਕੜੇ->ਸੈੱਟ.uh) {
    /* we cannot do anything without URL */
    failf(ਅੰਕੜੇ, "No URL set");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.uh) {
    ਅੱਖਰ *url = ਨਿਰਾ;
    CURLUcode uc;
    uc = curl_url_get(ਅੰਕੜੇ->ਸੈੱਟ.uh, CURLUPART_URL, &url, 0);
    ਜੇ(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url, ਨਿਰਾ, 0, ਨਿਰਾ);
      failf(ਅੰਕੜੇ, "No URL set");
      ਵਾਪਸ CURLE_URL_MALFORMAT;
    }
    ਨਤੀਜਾ = CURL_EASY_STR_SETN(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL, url);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url, CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL),
                  0, ਨਿਰਾ);

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postfields && ਅੰਕੜੇ->ਸੈੱਟ.ਸੈੱਟ_resume_from) {
    /* we cannot */
    failf(ਅੰਕੜੇ, "cannot mix POSTFIELDS with RESUME_FROM");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਅੰਕੜੇ->state.prefer_ascii = ਅੰਕੜੇ->ਸੈੱਟ.prefer_ascii;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIST_ONLY_PROTOCOL
  ਅੰਕੜੇ->state.list_only = ਅੰਕੜੇ->ਸੈੱਟ.list_only;
#ਜੇ_ਅੰਤ
  ਅੰਕੜੇ->state.httpreq = ਅੰਕੜੇ->ਸੈੱਟ.ਵਿਧੀ;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&ਅੰਕੜੇ->state.initial_origin);
  Curl_peer_unlink(&ਅੰਕੜੇ->state.origin);
  ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ = 0;
  ਅੰਕੜੇ->state.followlocation = 0; /* reset the location-follow counter */
  ਅੰਕੜੇ->state.this_is_a_follow = FALSE; /* reset this */
  ਅੰਕੜੇ->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  ਅੰਕੜੇ->state.errorbuf = FALSE; /* no error has occurred */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  Curl_http_neg_init(ਅੰਕੜੇ, &ਅੰਕੜੇ->state.http_neg);
#ਜੇ_ਅੰਤ
  ਅੰਕੜੇ->state.authproblem = FALSE;
  ਅੰਕੜੇ->state.authhost.want = ਅੰਕੜੇ->ਸੈੱਟ.httpauth;
  ਅੰਕੜੇ->state.authproxy.want = ਅੰਕੜੇ->ਸੈੱਟ.proxyauth;
  curlx_safefree(ਅੰਕੜੇ->info.wouldredirect);
  curlx_safefree(ਅੰਕੜੇ->info.effective_url);
  Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_clear_state(ਅੰਕੜੇ);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.http_auto_referer)
    Curl_bufref_free(&ਅੰਕੜੇ->state.referer);
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_REFERER))
    Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.referer,
                    CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_REFERER), 0, ਨਿਰਾ);
  ਨਹੀਂਤਾਂ
    Curl_bufref_free(&ਅੰਕੜੇ->state.referer);

  ਜੇ(ਅੰਕੜੇ->state.httpreq == HTTPREQ_PUT)
    ਅੰਕੜੇ->state.infilesize = ਅੰਕੜੇ->ਸੈੱਟ.filesize;
  ਨਹੀਂਤਾਂ ਜੇ((ਅੰਕੜੇ->state.httpreq != HTTPREQ_GET) &&
          (ਅੰਕੜੇ->state.httpreq != HTTPREQ_ਸਿਰ)) {
    ਅੰਕੜੇ->state.infilesize = ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postfields && (ਅੰਕੜੇ->state.infilesize == -1))
      ਅੰਕੜੇ->state.infilesize = (curl_off_t)ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ->ਸੈੱਟ.postfields);
  }
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  ਨਤੀਜਾ = Curl_cookie_loadfiles(ਅੰਕੜੇ,
                                 ਅੰਕੜੇ->ਸੈੱਟ.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  ਜੇ(!ਨਤੀਜਾ)
    Curl_cookie_ਲੜੀਵਾਰ(ਅੰਕੜੇ); /* activate */

  /* If there is a list of host pairs to deal with */
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->state.resolve)
    ਨਤੀਜਾ = Curl_loadhostpairs(ਅੰਕੜੇ);

  ਜੇ(!ਨਤੀਜਾ)
    /* If there is a list of hsts files to read */
    ਨਤੀਜਾ = Curl_hsts_loadfiles(ਅੰਕੜੇ);

  ਜੇ(!ਨਤੀਜਾ) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    ਅੰਕੜੇ->state.allow_ਪੋਰਟ = TRUE;

#ਜੇ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.no_signal)
      ਅੰਕੜੇ->state.prev_signal = ਸੰਕੇਤ(SIGPIPE, SIG_IGN);
#ਜੇ_ਅੰਤ

    Curl_initinfo(ਅੰਕੜੇ); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(ਅੰਕੜੇ);
    Curl_pgrs_ਸ਼ੁਰੂ(ਅੰਕੜੇ, ਨਿਰਾ);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    ਅੰਕੜੇ->state.authhost.picked &= ਅੰਕੜੇ->state.authhost.want;
    ਅੰਕੜੇ->state.authproxy.picked &= ਅੰਕੜੇ->state.authproxy.want;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
    ਅੰਕੜੇ->state.wildcardmatch = ਅੰਕੜੇ->ਸੈੱਟ.wildcard_enabled;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(getenv("CURL_DBG_FTP_WILDCARD"))
    ਅੰਕੜੇ->state.wildcardmatch = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
      ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc;
      ਜੇ(!ਅੰਕੜੇ->wildcard) {
        ਅੰਕੜੇ->wildcard = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Wildcard_ਅੰਕੜੇ));
        ਜੇ(!ਅੰਕੜੇ->wildcard)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      wc = ਅੰਕੜੇ->wildcard;
      ਜੇ(wc->state < CURLWC_INIT) {
        ਜੇ(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->ਨਮੂਨਾ);
        curlx_safefree(wc->ਰਸਤਾ);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = Curl_hsts_loadcb(ਅੰਕੜੇ, ਅੰਕੜੇ->hsts);
  }

  ਅੰਕੜੇ->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  ਅੰਕੜੇ->req.rewind_read = FALSE;
  Curl_client_reset(ਅੰਕੜੇ);
  Curl_ਸਿਰਲੇਖਾਂ_cleanup(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ **url)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  bool retry = FALSE;
  *url = ਨਿਰਾ;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  ਜੇ(ਅੰਕੜੇ->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    ਵਾਪਸ CURLE_OK;

  ਜੇ(conn->ਬਿੱਟ.reuse &&
     (ਅੰਕੜੇ->req.bytecount + ਅੰਕੜੇ->req.headerbytecount == 0) &&
     ((!ਅੰਕੜੇ->req.no_body && !ਅੰਕੜੇ->req.ਪੂਰਾ) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
     && (ਅੰਕੜੇ->ਸੈੱਟ.rtspreq != RTSPREQ_RECEIVE)
#ਜੇ_ਅੰਤ
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.refused_ਧਾਰਾ &&
          (ਅੰਕੜੇ->req.bytecount + ਅੰਕੜੇ->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(ਅੰਕੜੇ, "REFUSED_STREAM, retrying a fresh connect");
    ਅੰਕੜੇ->state.refused_ਧਾਰਾ = FALSE; /* clear again */
    retry = TRUE;
  }
  ਜੇ(retry) {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONN_MAX_RETRIES 5
    ਜੇ(ਅੰਕੜੇ->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(ਅੰਕੜੇ, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      ਅੰਕੜੇ->state.retrycount = 0;
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    infof(ਅੰਕੜੇ, "Connection died, retrying a fresh connect (retry count: %d)",
          ਅੰਕੜੇ->state.retrycount);
    *url = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
    ਜੇ(!*url)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    connclose(conn); /* close this connection */
    conn->ਬਿੱਟ.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_ਸੈੱਟ_rewind(ਅੰਕੜੇ, TRUE);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ xfer_ਤਿਆਰੀ(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_ਮਾਪ      /* how much to receive, -1 if unknown */
  )
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(ਅੰਕੜੇ));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_ਮਾਪ == -1));
  k->ਮਾਪ = recv_ਮਾਪ;
  k->ਸਿਰਲੇਖ = !!conn->scheme->ਲੜੀਵਾਰ->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  ਜੇ(!k->ਸਿਰਲੇਖ && (recv_ਮਾਪ > 0))
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, recv_ਮਾਪ);

  /* we want header and/or body, if neither then do not do this! */
  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->write_resp_hd || !ਅੰਕੜੇ->req.no_body) {
    ਜੇ(conn->recv_idx != -1)
      CURL_REQ_ਸੈੱਟ_RECV(ਅੰਕੜੇ);
    ਜੇ(conn->send_idx != -1)
      CURL_REQ_ਸੈੱਟ_SEND(ਅੰਕੜੇ);
  }
  CURL_TRC_M(ਅੰਕੜੇ, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_nop(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  xfer_ਤਿਆਰੀ(ਅੰਕੜੇ, -1, -1, -1);
}

ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_sendrecv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              int8_t sockindex,
                              curl_off_t recv_ਮਾਪ)
{
  xfer_ਤਿਆਰੀ(ਅੰਕੜੇ, sockindex, sockindex, recv_ਮਾਪ);
}

ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex)
{
  xfer_ਤਿਆਰੀ(ਅੰਕੜੇ, sockindex, -1, -1);
}

ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex,
                          curl_off_t recv_ਮਾਪ)
{
  xfer_ਤਿਆਰੀ(ਅੰਕੜੇ, -1, sockindex, recv_ਮਾਪ);
}

ਖਾਲੀ Curl_xfer_ਸੈੱਟ_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (ਅੰਕੜੇ->conn->send_idx < 0) || (ਅੰਕੜੇ->conn->recv_idx < 0));
  ਅੰਕੜੇ->req.shutdown = shutdown;
  ਅੰਕੜੇ->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ਨਤੀਜਾ = ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->write_resp(ਅੰਕੜੇ, buf, blen, is_eos);
  }
  ਨਹੀਂਤਾਂ {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    ਜੇ(blen || is_eos) {
      ਪੂਰਨ cwtype = CLIENTWRITE_BODY;
      ਜੇ(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, cwtype, buf, blen);
    }
  }

  ਜੇ(!ਨਤੀਜਾ && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    ਅੰਕੜੇ->req.eos_written = TRUE;
    ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
  }
  CURL_TRC_WRITE(ਅੰਕੜੇ, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_xfer_write_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_cwriter_is_paused(ਅੰਕੜੇ);
}

CURLcode Curl_xfer_write_resp_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *hd0, size_t hdlen, bool is_eos)
{
  ਜੇ(ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ਵਾਪਸ ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->write_resp_hd(ਅੰਕੜੇ, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  ਵਾਪਸ Curl_xfer_write_resp(ਅੰਕੜੇ, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool premature)
{
  (ਖਾਲੀ)premature;
  ਵਾਪਸ Curl_cw_ਬਾਹਰਲਾ_ਪੂਰਾ(ਅੰਕੜੇ);
}

bool Curl_xfer_needs_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_conn_needs_flush(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->send_idx);
}

CURLcode Curl_xfer_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_conn_flush(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->send_idx);
}

CURLcode Curl_xfer_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਖਾਲੀ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);

  ਨਤੀਜਾ = Curl_conn_send(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->send_idx,
                          buf, blen, eos, pnwritten);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    ਨਤੀਜਾ = CURLE_OK;
    *pnwritten = 0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਨਤੀਜਾ && *pnwritten)
    ਅੰਕੜੇ->info.request_ਮਾਪ += *pnwritten;

  DEBUGF(infof(ਅੰਕੜੇ, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten));
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_xfer_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅੱਖਰ *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  DEBUGASSERT(ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ > 0);

  ਜੇ(curlx_uitouz(ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ) < blen)
    blen = curlx_uitouz(ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ);
  ਵਾਪਸ Curl_conn_recv(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_conn_ev_ਅੰਕੜੇ_ਪੂਰਾ_send(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

bool Curl_xfer_is_blocked(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  bool want_send = CURL_REQ_WANT_SEND(ਅੰਕੜੇ);
  bool want_recv = CURL_REQ_WANT_RECV(ਅੰਕੜੇ);
  ਜੇ(!want_send)
    ਵਾਪਸ want_recv && Curl_xfer_recv_is_paused(ਅੰਕੜੇ);
  ਨਹੀਂਤਾਂ ਜੇ(!want_recv)
    ਵਾਪਸ want_send && Curl_xfer_send_is_paused(ਅੰਕੜੇ);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ Curl_xfer_recv_is_paused(ਅੰਕੜੇ) && Curl_xfer_send_is_paused(ਅੰਕੜੇ);
}

bool Curl_xfer_send_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_rlimit_is_blocked(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_rlimit_is_blocked(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  Curl_rlimit_ਖੰਡ(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, enable, Curl_pgrs_now(ਅੰਕੜੇ));
  ਜੇ(!enable && Curl_creader_is_paused(ਅੰਕੜੇ))
    ਨਤੀਜਾ = Curl_creader_unpause(ਅੰਕੜੇ);
  Curl_pgrsSendPause(ਅੰਕੜੇ, enable);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_xfer_pause_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  Curl_rlimit_ਖੰਡ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, enable, Curl_pgrs_now(ਅੰਕੜੇ));
  ਜੇ(!enable && Curl_cwriter_is_paused(ਅੰਕੜੇ))
    ਨਤੀਜਾ = Curl_cwriter_unpause(ਅੰਕੜੇ);
  Curl_conn_ev_ਅੰਕੜੇ_pause(ਅੰਕੜੇ, enable);
  Curl_pgrsRecvPause(ਅੰਕੜੇ, enable);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_xfer_is_secure(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(ਅੰਕੜੇ->conn && ਅੰਕੜੇ->conn->ਬਿੱਟ.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_SSL));
    ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_SSL);
}
