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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <signal.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ಅಥವಾಆದರೆ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SOCKET
#ದೋಷ "We cannot compile without socket() support"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cw-out.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "getinfo.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
ಅಕ್ಷರ *Curl_checkheaders(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *thisheader,
                        ಸ್ಥಿರಾಂಕ size_t thislen)
{
  ರಚನೆ curl_slist *ತಲೆ;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  ಗಾಗಿ(ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು; ತಲೆ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
    ಆದರೆ(curl_strnequal(ತಲೆ->ದತ್ತಾಂಶ, thisheader, thislen) &&
       Curl_headersep(ತಲೆ->ದತ್ತಾಂಶ[thislen]))
      ಹಿಂತಿರುಗಿ ತಲೆ->ದತ್ತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool rcvd_eagain)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  ಆದರೆ(conn->scheme->protocol & PROTO_FAMILY_FTP)
    ಹಿಂತಿರುಗಿ Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  ಹಿಂತಿರುಗಿ (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, time_t timeofdoc)
{
  ಆದರೆ((timeofdoc == 0) || (ದತ್ತಾಂಶ->ಗಣ.timevalue == 0))
    ಹಿಂತಿರುಗಿ TRUE;

  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition) {
  ಸ್ಥಿತಿ CURL_TIMECOND_IFMODSINCE:
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(timeofdoc <= ದತ್ತಾಂಶ->ಗಣ.timevalue) {
      infof(ದತ್ತಾಂಶ, "The requested document is not new enough");
      ದತ್ತಾಂಶ->info.timecond = TRUE;
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_TIMECOND_IFUNMODSINCE:
    ಆದರೆ(timeofdoc >= ದತ್ತಾಂಶ->ಗಣ.timevalue) {
      infof(ದತ್ತಾಂಶ, "The requested document is not old enough");
      ದತ್ತಾಂಶ->info.timecond = TRUE;
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode xfer_recv_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_cshutdn_try_once_idx(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->recv_idx, ಮುಗಿದ);
}

ಸ್ಥಿರ bool xfer_recv_shutdown_started(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ CURL_CONN_ಒಳ_SHUTDOWN(ದತ್ತಾಂಶ->conn, ದತ್ತಾಂಶ->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_cshutdn_try_once_idx(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->send_idx, ಮುಗಿದ);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
ಸ್ಥಿರ CURLcode xfer_recv_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಅಕ್ಷರ *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  ಆದರೆ(!eos_reliable && !ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ && ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 != -1) {
    blen = curlx_sotouz_range(ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->req.bytecount, 0, blen);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(xfer_recv_shutdown_started(ದತ್ತಾಂಶ)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  ಆದರೆ(blen) {
    ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, buf, blen, pnread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(*pnread == 0) {
    ಆದರೆ(ದತ್ತಾಂಶ->req.shutdown) {
      bool ಮುಗಿದ;
      ಫಲಿತಾಂಶ = xfer_recv_shutdown(ದತ್ತಾಂಶ, &ಮುಗಿದ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಆದರೆ(!ಮುಗಿದ) {
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      }
    }
    DEBUGF(infof(ದತ್ತಾಂಶ, "sendrecv_dl: we are done"));
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
ಸ್ಥಿರ CURLcode sendrecv_dl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ SingleRequest *k)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *buf, *xfer_buf;
  size_t blen, xfer_blen;
  ಪೂರ್ಣಾಂಕ maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, ದರ_limited = FALSE;

  ಫಲಿತಾಂಶ = Curl_multi_xfer_buf_borrow(ದತ್ತಾಂಶ, &xfer_buf, &xfer_blen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  ಮಾಡು {
    size_t bytestoread;

    ಆದರೆ(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    ಆದರೆ(bytestoread && Curl_rlimit_ಸಕ್ರಿಯ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, ಖಾಲಿ);
#ಆದರೆ 0
      DEBUGF(infof(ದತ್ತಾಂಶ, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#ಆದರೆ_ಅಂತ್ಯ
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      ಆದರೆ(dl_avail <= 0) {
        ದರ_limited = TRUE;
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    ಫಲಿತಾಂಶ = xfer_recv_resp(ದತ್ತಾಂಶ, buf, bytestoread, is_multiplex, &blen);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
        ಹೋಗು ಹೊರಹೋಗು; /* real error */
      rcvd_eagain = TRUE;
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಆದರೆ(ದತ್ತಾಂಶ->req.download_ಮುಗಿದ && ದತ್ತಾಂಶ->req.no_body &&
         !ದತ್ತಾಂಶ->req.resp_trailer) {
        DEBUGF(infof(ದತ್ತಾಂಶ, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ನಿಲ್ಲಿಸು; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    ಆದರೆ(!blen) {
      ಫಲಿತಾಂಶ = Curl_req_stop_send_recv(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಆದರೆ(k->eos_written) /* already did write this to client, leave */
        ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = Curl_xfer_write_resp(ದತ್ತಾಂಶ, buf, blen, is_eos);
    ಆದರೆ(ಫಲಿತಾಂಶ || ದತ್ತಾಂಶ->req.ಮುಗಿದ)
      ಹೋಗು ಹೊರಹೋಗು;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    ಆದರೆ((!is_multiplex && ದತ್ತಾಂಶ->req.download_ಮುಗಿದ) || is_eos) {
      CURL_REQ_CLEAR_RECV(ದತ್ತಾಂಶ);
    }
    /* if we stopped receiving, leave the loop */
    ಆದರೆ(!CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ))
      ನಿಲ್ಲಿಸು;

  } ಆಗ(maxloops--);

  ಆದರೆ(!is_eos && !ದರ_limited && CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ) &&
     (!rcvd_eagain || ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    CURL_TRC_M(ದತ್ತಾಂಶ, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  ಆದರೆ(!CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ) && CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ) &&
     (conn->ಬಿಟ್ಟುಗಳು.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(ದತ್ತಾಂಶ, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(ದತ್ತಾಂಶ);
  }

ಹೊರಹೋಗು:
  Curl_multi_xfer_buf_release(ದತ್ತಾಂಶ, xfer_buf);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "sendrecv_dl() -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
ಸ್ಥಿರ CURLcode sendrecv_ul(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_ಮುಗಿದ_sending(ದತ್ತಾಂಶ));

  ಆದರೆ(!Curl_req_ಮುಗಿದ_sending(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ Curl_req_send_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(Curl_xfer_is_blocked(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = CURLE_OK;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  ಆದರೆ(CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = sendrecv_dl(ದತ್ತಾಂಶ, k);
    ಆದರೆ(ಫಲಿತಾಂಶ || ದತ್ತಾಂಶ->req.ಮುಗಿದ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  ಆದರೆ(Curl_req_want_send(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = sendrecv_ul(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  ಆದರೆ(CURL_REQ_WANT_IO(ದತ್ತಾಂಶ)) {
    ಆದರೆ(Curl_timeleft_now_ms(ದತ್ತಾಂಶ, pnow) < 0) {
      ಆದರೆ(k->ಗಾತ್ರ_2 != -1) {
        failf(ದತ್ತಾಂಶ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE),
              k->bytecount, k->ಗಾತ್ರ_2);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE),
              k->bytecount);
      }
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    ಆದರೆ(!ದತ್ತಾಂಶ->req.no_body && (k->ಗಾತ್ರ_2 != -1) &&
       (k->bytecount != k->ಗಾತ್ರ_2) && !k->newurl) {
      failf(ದತ್ತಾಂಶ, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->ಗಾತ್ರ_2 - k->bytecount);
      ಫಲಿತಾಂಶ = CURLE_PARTIAL_FILE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  ಆದರೆ(!CURL_REQ_WANT_IO(ದತ್ತಾಂಶ))
    ದತ್ತಾಂಶ->req.ಮುಗಿದ = TRUE;

  ಫಲಿತಾಂಶ = Curl_pgrsCheckX(ದತ್ತಾಂಶ, pnow);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_sendrecv() -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
ಶೂನ್ಯ Curl_init_CONNECT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ದತ್ತಾಂಶ->state.fread_func = ದತ್ತಾಂಶ->ಗಣ.fread_func_ಗಣ;
  ದತ್ತಾಂಶ->state.ಒಳ = ದತ್ತಾಂಶ->ಗಣ.ಒಳ_ಗಣ;
  ದತ್ತಾಂಶ->state.upload = (ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  ದತ್ತಾಂಶ->state.retrycount = 0;

  ಆದರೆ(!CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_URL) && !ದತ್ತಾಂಶ->ಗಣ.uh) {
    /* we cannot do anything without URL */
    failf(ದತ್ತಾಂಶ, "No URL set");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.uh) {
    ಅಕ್ಷರ *url = ಖಾಲಿ;
    CURLUcode uc;
    uc = curl_url_get(ದತ್ತಾಂಶ->ಗಣ.uh, CURLUPART_URL, &url, 0);
    ಆದರೆ(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url, ಖಾಲಿ, 0, ಖಾಲಿ);
      failf(ದತ್ತಾಂಶ, "No URL set");
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }
    ಫಲಿತಾಂಶ = CURL_EASY_STR_SETN(ದತ್ತಾಂಶ, STRING_ಗಣ_URL, url);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_URL),
                  0, ಖಾಲಿ);

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postfields && ದತ್ತಾಂಶ->ಗಣ.ಗಣ_resume_from) {
    /* we cannot */
    failf(ದತ್ತಾಂಶ, "cannot mix POSTFIELDS with RESUME_FROM");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ದತ್ತಾಂಶ->state.prefer_ascii = ದತ್ತಾಂಶ->ಗಣ.prefer_ascii;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIST_ONLY_PROTOCOL
  ದತ್ತಾಂಶ->state.list_only = ದತ್ತಾಂಶ->ಗಣ.list_only;
#ಆದರೆ_ಅಂತ್ಯ
  ದತ್ತಾಂಶ->state.httpreq = ದತ್ತಾಂಶ->ಗಣ.ವಿಧಾನ;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&ದತ್ತಾಂಶ->state.initial_origin);
  Curl_peer_unlink(&ದತ್ತಾಂಶ->state.origin);
  ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು = 0;
  ದತ್ತಾಂಶ->state.followlocation = 0; /* reset the location-follow counter */
  ದತ್ತಾಂಶ->state.this_is_a_follow = FALSE; /* reset this */
  ದತ್ತಾಂಶ->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  ದತ್ತಾಂಶ->state.errorbuf = FALSE; /* no error has occurred */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  Curl_http_neg_init(ದತ್ತಾಂಶ, &ದತ್ತಾಂಶ->state.http_neg);
#ಆದರೆ_ಅಂತ್ಯ
  ದತ್ತಾಂಶ->state.authproblem = FALSE;
  ದತ್ತಾಂಶ->state.authhost.want = ದತ್ತಾಂಶ->ಗಣ.httpauth;
  ದತ್ತಾಂಶ->state.authproxy.want = ದತ್ತಾಂಶ->ಗಣ.proxyauth;
  curlx_safefree(ದತ್ತಾಂಶ->info.wouldredirect);
  curlx_safefree(ದತ್ತಾಂಶ->info.effective_url);
  Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_clear_state(ದತ್ತಾಂಶ);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.http_auto_referer)
    Curl_bufref_free(&ದತ್ತಾಂಶ->state.referer);
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_REFERER))
    Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.referer,
                    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_REFERER), 0, ಖಾಲಿ);
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_bufref_free(&ದತ್ತಾಂಶ->state.referer);

  ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_PUT)
    ದತ್ತಾಂಶ->state.infilesize = ದತ್ತಾಂಶ->ಗಣ.filesize;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_GET) &&
          (ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_ತಲೆ)) {
    ದತ್ತಾಂಶ->state.infilesize = ದತ್ತಾಂಶ->ಗಣ.postfieldsize;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postfields && (ದತ್ತಾಂಶ->state.infilesize == -1))
      ದತ್ತಾಂಶ->state.infilesize = (curl_off_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ->ಗಣ.postfields);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  ಫಲಿತಾಂಶ = Curl_cookie_loadfiles(ದತ್ತಾಂಶ,
                                 ದತ್ತಾಂಶ->ಗಣ.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    Curl_cookie_ಸತತ(ದತ್ತಾಂಶ); /* activate */

  /* If there is a list of host pairs to deal with */
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->state.resolve)
    ಫಲಿತಾಂಶ = Curl_loadhostpairs(ದತ್ತಾಂಶ);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    /* If there is a list of hsts files to read */
    ಫಲಿತಾಂಶ = Curl_hsts_loadfiles(ದತ್ತಾಂಶ);

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    ದತ್ತಾಂಶ->state.allow_ಪೋರ್ಟ್ = TRUE;

#ಆದರೆ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.no_signal)
      ದತ್ತಾಂಶ->state.prev_signal = ಸಂಕೇತ(SIGPIPE, SIG_IGN);
#ಆದರೆ_ಅಂತ್ಯ

    Curl_initinfo(ದತ್ತಾಂಶ); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(ದತ್ತಾಂಶ);
    Curl_pgrs_ಆರಂಭ(ದತ್ತಾಂಶ, ಖಾಲಿ);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    ದತ್ತಾಂಶ->state.authhost.picked &= ದತ್ತಾಂಶ->state.authhost.want;
    ದತ್ತಾಂಶ->state.authproxy.picked &= ದತ್ತಾಂಶ->state.authproxy.want;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
    ದತ್ತಾಂಶ->state.wildcardmatch = ದತ್ತಾಂಶ->ಗಣ.wildcard_enabled;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(getenv("CURL_DBG_FTP_WILDCARD"))
    ದತ್ತಾಂಶ->state.wildcardmatch = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
      ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc;
      ಆದರೆ(!ದತ್ತಾಂಶ->wildcard) {
        ದತ್ತಾಂಶ->wildcard = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Wildcard_ದತ್ತಾಂಶ));
        ಆದರೆ(!ದತ್ತಾಂಶ->wildcard)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      wc = ದತ್ತಾಂಶ->wildcard;
      ಆದರೆ(wc->state < CURLWC_INIT) {
        ಆದರೆ(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->ನಮೂನೆ);
        curlx_safefree(wc->ಮಾರ್ಗ);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = Curl_hsts_loadcb(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->hsts);
  }

  ದತ್ತಾಂಶ->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  ದತ್ತಾಂಶ->req.rewind_read = FALSE;
  Curl_client_reset(ದತ್ತಾಂಶ);
  Curl_ಶೀರ್ಷಿಕೆಗಳು_cleanup(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ **url)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  bool retry = FALSE;
  *url = ಖಾಲಿ;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.reuse &&
     (ದತ್ತಾಂಶ->req.bytecount + ದತ್ತಾಂಶ->req.headerbytecount == 0) &&
     ((!ದತ್ತಾಂಶ->req.no_body && !ದತ್ತಾಂಶ->req.ಮುಗಿದ) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
     && (ದತ್ತಾಂಶ->ಗಣ.rtspreq != RTSPREQ_RECEIVE)
#ಆದರೆ_ಅಂತ್ಯ
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.refused_ಹರಿವು &&
          (ದತ್ತಾಂಶ->req.bytecount + ದತ್ತಾಂಶ->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(ದತ್ತಾಂಶ, "REFUSED_STREAM, retrying a fresh connect");
    ದತ್ತಾಂಶ->state.refused_ಹರಿವು = FALSE; /* clear again */
    retry = TRUE;
  }
  ಆದರೆ(retry) {
#ವ್ಯಾಖ್ಯಾನಿಸು CONN_MAX_RETRIES 5
    ಆದರೆ(ದತ್ತಾಂಶ->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(ದತ್ತಾಂಶ, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      ದತ್ತಾಂಶ->state.retrycount = 0;
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    infof(ದತ್ತಾಂಶ, "Connection died, retrying a fresh connect (retry count: %d)",
          ದತ್ತಾಂಶ->state.retrycount);
    *url = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
    ಆದರೆ(!*url)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    connclose(conn); /* close this connection */
    conn->ಬಿಟ್ಟುಗಳು.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_ಗಣ_rewind(ದತ್ತಾಂಶ, TRUE);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ xfer_ಸಿದ್ಧತೆ(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_ಗಾತ್ರ      /* how much to receive, -1 if unknown */
  )
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(ದತ್ತಾಂಶ));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_ಗಾತ್ರ == -1));
  k->ಗಾತ್ರ_2 = recv_ಗಾತ್ರ;
  k->ಶೀರ್ಷಿಕೆ = !!conn->scheme->ಸತತ->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  ಆದರೆ(!k->ಶೀರ್ಷಿಕೆ && (recv_ಗಾತ್ರ > 0))
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, recv_ಗಾತ್ರ);

  /* we want header and/or body, if neither then do not do this! */
  ಆದರೆ(conn->scheme->ಸತತ->write_resp_hd || !ದತ್ತಾಂಶ->req.no_body) {
    ಆದರೆ(conn->recv_idx != -1)
      CURL_REQ_ಗಣ_RECV(ದತ್ತಾಂಶ);
    ಆದರೆ(conn->send_idx != -1)
      CURL_REQ_ಗಣ_SEND(ದತ್ತಾಂಶ);
  }
  CURL_TRC_M(ದತ್ತಾಂಶ, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

ಶೂನ್ಯ Curl_xfer_ಸಿದ್ಧತೆ_nop(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  xfer_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, -1, -1, -1);
}

ಶೂನ್ಯ Curl_xfer_ಸಿದ್ಧತೆ_sendrecv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              int8_t sockindex,
                              curl_off_t recv_ಗಾತ್ರ)
{
  xfer_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, sockindex, sockindex, recv_ಗಾತ್ರ);
}

ಶೂನ್ಯ Curl_xfer_ಸಿದ್ಧತೆ_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          int8_t sockindex)
{
  xfer_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, sockindex, -1, -1);
}

ಶೂನ್ಯ Curl_xfer_ಸಿದ್ಧತೆ_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          int8_t sockindex,
                          curl_off_t recv_ಗಾತ್ರ)
{
  xfer_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, -1, sockindex, recv_ಗಾತ್ರ);
}

ಶೂನ್ಯ Curl_xfer_ಗಣ_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (ದತ್ತಾಂಶ->conn->send_idx < 0) || (ದತ್ತಾಂಶ->conn->recv_idx < 0));
  ದತ್ತಾಂಶ->req.shutdown = shutdown;
  ದತ್ತಾಂಶ->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->ಸತತ->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ಫಲಿತಾಂಶ = ದತ್ತಾಂಶ->conn->scheme->ಸತತ->write_resp(ದತ್ತಾಂಶ, buf, blen, is_eos);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    ಆದರೆ(blen || is_eos) {
      ಪೂರ್ಣಾಂಕ cwtype = CLIENTWRITE_BODY;
      ಆದರೆ(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, cwtype, buf, blen);
    }
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    ದತ್ತಾಂಶ->req.eos_written = TRUE;
    ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
  }
  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_xfer_write_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_cwriter_is_paused(ದತ್ತಾಂಶ);
}

CURLcode Curl_xfer_write_resp_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd0, size_t hdlen, bool is_eos)
{
  ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->ಸತತ->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->conn->scheme->ಸತತ->write_resp_hd(ದತ್ತಾಂಶ, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  ಹಿಂತಿರುಗಿ Curl_xfer_write_resp(ದತ್ತಾಂಶ, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool premature)
{
  (ಶೂನ್ಯ)premature;
  ಹಿಂತಿರುಗಿ Curl_cw_ಹೊರಹೋಗು_ಮುಗಿದ(ದತ್ತಾಂಶ);
}

bool Curl_xfer_needs_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_conn_needs_flush(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->send_idx);
}

CURLcode Curl_xfer_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_conn_flush(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->send_idx);
}

CURLcode Curl_xfer_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

  ಫಲಿತಾಂಶ = Curl_conn_send(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->send_idx,
                          buf, blen, eos, pnwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    ಫಲಿತಾಂಶ = CURLE_OK;
    *pnwritten = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಫಲಿತಾಂಶ && *pnwritten)
    ದತ್ತಾಂಶ->info.request_ಗಾತ್ರ += *pnwritten;

  DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_xfer_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಅಕ್ಷರ *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  DEBUGASSERT(ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ > 0);

  ಆದರೆ(curlx_uitouz(ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ) < blen)
    blen = curlx_uitouz(ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ);
  ಹಿಂತಿರುಗಿ Curl_conn_recv(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ_send(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_xfer_is_blocked(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  bool want_send = CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ);
  bool want_recv = CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ);
  ಆದರೆ(!want_send)
    ಹಿಂತಿರುಗಿ want_recv && Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!want_recv)
    ಹಿಂತಿರುಗಿ want_send && Curl_xfer_send_is_paused(ದತ್ತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ) && Curl_xfer_send_is_paused(ದತ್ತಾಂಶ);
}

bool Curl_xfer_send_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_rlimit_is_blocked(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_rlimit_is_blocked(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  Curl_rlimit_ಖಂಡ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, enable, Curl_pgrs_now(ದತ್ತಾಂಶ));
  ಆದರೆ(!enable && Curl_creader_is_paused(ದತ್ತಾಂಶ))
    ಫಲಿತಾಂಶ = Curl_creader_unpause(ದತ್ತಾಂಶ);
  Curl_pgrsSendPause(ದತ್ತಾಂಶ, enable);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_xfer_pause_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  Curl_rlimit_ಖಂಡ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, enable, Curl_pgrs_now(ದತ್ತಾಂಶ));
  ಆದರೆ(!enable && Curl_cwriter_is_paused(ದತ್ತಾಂಶ))
    ಫಲಿತಾಂಶ = Curl_cwriter_unpause(ದತ್ತಾಂಶ);
  Curl_conn_ev_ದತ್ತಾಂಶ_pause(ದತ್ತಾಂಶ, enable);
  Curl_pgrsRecvPause(ದತ್ತಾಂಶ, enable);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_xfer_is_secure(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(ದತ್ತಾಂಶ->conn && ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_SSL));
    ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_SSL);
}
