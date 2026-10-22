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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः
#अन्तर्भाव <signal.h>

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् HAVE_SOCKET
#दोषः "We cannot compile without socket() support"
#यदि_अन्तः

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

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
वर्ण *Curl_checkheaders(ध्रुव संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *thisheader,
                        ध्रुव size_t thislen)
{
  संरचना curl_slist *शीर्ष;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  प्रति(शीर्ष = दत्त->समुच्चयः.शीर्षकाणि; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    यदि(curl_strnequal(शीर्ष->दत्त, thisheader, thislen) &&
       Curl_headersep(शीर्ष->दत्त[thislen]))
      प्रत्यागम शीर्ष->दत्त;
  }

  प्रत्यागम रिक्त;
}
#यदि_अन्तः

स्थिर पूर्णाङ्क दत्त_pending(संरचना Curl_easy *दत्त, bool rcvd_eagain)
{
  संरचना connectdata *conn = दत्त->conn;

  यदि(conn->scheme->protocol & PROTO_FAMILY_FTP)
    प्रत्यागम Curl_conn_दत्त_pending(दत्त, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  प्रत्यागम (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(संरचना Curl_easy *दत्त, time_t timeofdoc)
{
  यदि((timeofdoc == 0) || (दत्त->समुच्चयः.timevalue == 0))
    प्रत्यागम TRUE;

  विकल्प(दत्त->समुच्चयः.timecondition) {
  पक्ष CURL_TIMECOND_IFMODSINCE:
  स्वभाव:
    यदि(timeofdoc <= दत्त->समुच्चयः.timevalue) {
      infof(दत्त, "The requested document is not new enough");
      दत्त->info.timecond = TRUE;
      प्रत्यागम FALSE;
    }
    भङ्ग;
  पक्ष CURL_TIMECOND_IFUNMODSINCE:
    यदि(timeofdoc >= दत्त->समुच्चयः.timevalue) {
      infof(दत्त, "The requested document is not old enough");
      दत्त->info.timecond = TRUE;
      प्रत्यागम FALSE;
    }
    भङ्ग;
  }

  प्रत्यागम TRUE;
}

स्थिर CURLcode xfer_recv_shutdown(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  यदि(!दत्त || !दत्त->conn)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_cshutdn_try_once_idx(दत्त, दत्त->conn->recv_idx, समाप्तम्);
}

स्थिर bool xfer_recv_shutdown_started(संरचना Curl_easy *दत्त)
{
  यदि(!दत्त || !दत्त->conn)
    प्रत्यागम FALSE;
  प्रत्यागम CURL_CONN_अन्तर्_SHUTDOWN(दत्त->conn, दत्त->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  यदि(!दत्त || !दत्त->conn)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_cshutdn_try_once_idx(दत्त, दत्त->conn->send_idx, समाप्तम्);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
स्थिर CURLcode xfer_recv_resp(संरचना Curl_easy *दत्त,
                               वर्ण *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode परिणाम;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  यदि(!eos_reliable && !दत्त->req.शीर्षकम् && दत्त->req.आकार != -1) {
    blen = curlx_sotouz_range(दत्त->req.आकार - दत्त->req.bytecount, 0, blen);
  }
  अन्यथा यदि(xfer_recv_shutdown_started(दत्त)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  यदि(blen) {
    परिणाम = Curl_xfer_recv(दत्त, buf, blen, pnread);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(*pnread == 0) {
    यदि(दत्त->req.shutdown) {
      bool समाप्तम्;
      परिणाम = xfer_recv_shutdown(दत्त, &समाप्तम्);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      यदि(!समाप्तम्) {
        प्रत्यागम CURLE_AGAIN;
      }
    }
    DEBUGF(infof(दत्त, "sendrecv_dl: we are done"));
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
स्थिर CURLcode sendrecv_dl(संरचना Curl_easy *दत्त,
                            संरचना SingleRequest *k)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;
  वर्ण *buf, *xfer_buf;
  size_t blen, xfer_blen;
  पूर्णाङ्क maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, दरः_limited = FALSE;

  परिणाम = Curl_multi_xfer_buf_borrow(दत्त, &xfer_buf, &xfer_blen);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  कुरु {
    size_t bytestoread;

    यदि(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    यदि(bytestoread && Curl_rlimit_सक्रिय(&दत्त->प्रगतिः.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&दत्त->प्रगतिः.dl.rlimit, रिक्त);
#यदि 0
      DEBUGF(infof(दत्त, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#यदि_अन्तः
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      यदि(dl_avail <= 0) {
        दरः_limited = TRUE;
        भङ्ग;
      }
      यदि(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    परिणाम = xfer_recv_resp(दत्त, buf, bytestoread, is_multiplex, &blen);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN)
        गच्छ बहिर्गमनम्; /* real error */
      rcvd_eagain = TRUE;
      परिणाम = CURLE_OK;
      यदि(दत्त->req.download_समाप्तम् && दत्त->req.no_body &&
         !दत्त->req.resp_trailer) {
        DEBUGF(infof(दत्त, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      अन्यथा
        भङ्ग; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    यदि(!blen) {
      परिणाम = Curl_req_stop_send_recv(दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      यदि(k->eos_written) /* already did write this to client, leave */
        भङ्ग;
    }

    परिणाम = Curl_xfer_write_resp(दत्त, buf, blen, is_eos);
    यदि(परिणाम || दत्त->req.समाप्तम्)
      गच्छ बहिर्गमनम्;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    यदि((!is_multiplex && दत्त->req.download_समाप्तम्) || is_eos) {
      CURL_REQ_CLEAR_RECV(दत्त);
    }
    /* if we stopped receiving, leave the loop */
    यदि(!CURL_REQ_WANT_RECV(दत्त))
      भङ्ग;

  } यावत्(maxloops--);

  यदि(!is_eos && !दरः_limited && CURL_REQ_WANT_RECV(दत्त) &&
     (!rcvd_eagain || दत्त_pending(दत्त, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(दत्त);
    CURL_TRC_M(दत्त, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  यदि(!CURL_REQ_WANT_RECV(दत्त) && CURL_REQ_WANT_SEND(दत्त) &&
     (conn->द्वयङ्काः.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(दत्त, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(दत्त);
  }

बहिर्गमनम्:
  Curl_multi_xfer_buf_release(दत्त, xfer_buf);
  यदि(परिणाम)
    DEBUGF(infof(दत्त, "sendrecv_dl() -> %d", (पूर्णाङ्क)परिणाम));
  प्रत्यागम परिणाम;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
स्थिर CURLcode sendrecv_ul(संरचना Curl_easy *दत्त)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_समाप्तम्_sending(दत्त));

  यदि(!Curl_req_समाप्तम्_sending(दत्त))
    प्रत्यागम Curl_req_send_अधिकम्(दत्त);
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(संरचना Curl_easy *दत्त)
{
  संरचना SingleRequest *k = &दत्त->req;
  ध्रुव संरचना curltime *pnow = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(Curl_xfer_is_blocked(दत्त)) {
    परिणाम = CURLE_OK;
    गच्छ बहिर्गमनम्;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  यदि(CURL_REQ_WANT_RECV(दत्त)) {
    परिणाम = sendrecv_dl(दत्त, k);
    यदि(परिणाम || दत्त->req.समाप्तम्)
      गच्छ बहिर्गमनम्;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  यदि(Curl_req_want_send(दत्त)) {
    परिणाम = sendrecv_ul(दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  pnow = Curl_pgrs_now(दत्त);
  यदि(CURL_REQ_WANT_IO(दत्त)) {
    यदि(Curl_timeleft_now_ms(दत्त, pnow) < 0) {
      यदि(k->आकार != -1) {
        failf(दत्त, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE),
              k->bytecount, k->आकार);
      }
      अन्यथा {
        failf(दत्त, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE),
              k->bytecount);
      }
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    यदि(!दत्त->req.no_body && (k->आकार != -1) &&
       (k->bytecount != k->आकार) && !k->newurl) {
      failf(दत्त, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->आकार - k->bytecount);
      परिणाम = CURLE_PARTIAL_FILE;
      गच्छ बहिर्गमनम्;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  यदि(!CURL_REQ_WANT_IO(दत्त))
    दत्त->req.समाप्तम् = TRUE;

  परिणाम = Curl_pgrsCheckX(दत्त, pnow);

बहिर्गमनम्:
  यदि(परिणाम)
    DEBUGF(infof(दत्त, "Curl_sendrecv() -> %d", (पूर्णाङ्क)परिणाम));
  प्रत्यागम परिणाम;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
शून्य Curl_init_CONNECT(संरचना Curl_easy *दत्त)
{
  दत्त->state.fread_func = दत्त->समुच्चयः.fread_func_समुच्चयः;
  दत्त->state.अन्तर् = दत्त->समुच्चयः.अन्तर्_समुच्चयः;
  दत्त->state.upload = (दत्त->state.httpreq == HTTPREQ_PUT);
}

/*
 * Curl_pretransfer() is called immediately before a transfer starts, and only
 * once for one transfer no matter if it has redirects or do multi-pass
 * authentication etc.
 */
CURLcode Curl_pretransfer(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  दत्त->state.retrycount = 0;

  यदि(!CURL_EASY_STR(दत्त, STRING_समुच्चयः_URL) && !दत्त->समुच्चयः.uh) {
    /* we cannot do anything without URL */
    failf(दत्त, "No URL set");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  यदि(दत्त->समुच्चयः.uh) {
    वर्ण *url = रिक्त;
    CURLUcode uc;
    uc = curl_url_get(दत्त->समुच्चयः.uh, CURLUPART_URL, &url, 0);
    यदि(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_समुच्चयः(&दत्त->state.url, रिक्त, 0, रिक्त);
      failf(दत्त, "No URL set");
      प्रत्यागम CURLE_URL_MALFORMAT;
    }
    परिणाम = CURL_EASY_STR_SETN(दत्त, STRING_समुच्चयः_URL, url);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  Curl_bufref_समुच्चयः(&दत्त->state.url, CURL_EASY_STR(दत्त, STRING_समुच्चयः_URL),
                  0, रिक्त);

  यदि(दत्त->समुच्चयः.postfields && दत्त->समुच्चयः.समुच्चयः_resume_from) {
    /* we cannot */
    failf(दत्त, "cannot mix POSTFIELDS with RESUME_FROM");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  दत्त->state.prefer_ascii = दत्त->समुच्चयः.prefer_ascii;
#यदि_परिभाषितम् CURL_LIST_ONLY_PROTOCOL
  दत्त->state.list_only = दत्त->समुच्चयः.list_only;
#यदि_अन्तः
  दत्त->state.httpreq = दत्त->समुच्चयः.विधानम्;

  /* initial transfer request coming up, forget the initial origin
   * from a previous perform() on this handle. */
  Curl_peer_unlink(&दत्त->state.initial_origin);
  Curl_peer_unlink(&दत्त->state.origin);
  दत्त->state.अनुरोधाः = 0;
  दत्त->state.followlocation = 0; /* reset the location-follow counter */
  दत्त->state.this_is_a_follow = FALSE; /* reset this */
  दत्त->state.http_ignorecustom = FALSE; /* use custom HTTP method */
  दत्त->state.errorbuf = FALSE; /* no error has occurred */
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  Curl_http_neg_init(दत्त, &दत्त->state.http_neg);
#यदि_अन्तः
  दत्त->state.authproblem = FALSE;
  दत्त->state.authhost.want = दत्त->समुच्चयः.httpauth;
  दत्त->state.authproxy.want = दत्त->समुच्चयः.proxyauth;
  curlx_safefree(दत्त->info.wouldredirect);
  curlx_safefree(दत्त->info.effective_url);
  Curl_दत्त_प्राथमिकता_clear_state(दत्त);
  यदि(दत्त->समुच्चयः.http_auto_referer)
    Curl_bufref_free(&दत्त->state.referer);
  यदि(CURL_EASY_STR(दत्त, STRING_समुच्चयः_REFERER))
    Curl_bufref_समुच्चयः(&दत्त->state.referer,
                    CURL_EASY_STR(दत्त, STRING_समुच्चयः_REFERER), 0, रिक्त);
  अन्यथा
    Curl_bufref_free(&दत्त->state.referer);

  यदि(दत्त->state.httpreq == HTTPREQ_PUT)
    दत्त->state.infilesize = दत्त->समुच्चयः.filesize;
  अन्यथा यदि((दत्त->state.httpreq != HTTPREQ_GET) &&
          (दत्त->state.httpreq != HTTPREQ_शीर्ष)) {
    दत्त->state.infilesize = दत्त->समुच्चयः.postfieldsize;
    यदि(दत्त->समुच्चयः.postfields && (दत्त->state.infilesize == -1))
      दत्त->state.infilesize = (curl_off_t)वाक्यदैर्घ्यम्(दत्त->समुच्चयः.postfields);
  }
  अन्यथा
    दत्त->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  परिणाम = Curl_cookie_loadfiles(दत्त,
                                 दत्त->समुच्चयः.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  यदि(!परिणाम)
    Curl_cookie_सततम्(दत्त); /* activate */

  /* If there is a list of host pairs to deal with */
  यदि(!परिणाम && दत्त->state.resolve)
    परिणाम = Curl_loadhostpairs(दत्त);

  यदि(!परिणाम)
    /* If there is a list of hsts files to read */
    परिणाम = Curl_hsts_loadfiles(दत्त);

  यदि(!परिणाम) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    दत्त->state.allow_पत्तनम् = TRUE;

#यदि defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    यदि(!दत्त->समुच्चयः.no_signal)
      दत्त->state.पूर्व_signal = सङ्केत(SIGPIPE, SIG_IGN);
#यदि_अन्तः

    Curl_initinfo(दत्त); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(दत्त);
    Curl_pgrsStart(दत्त, रिक्त);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    दत्त->state.authhost.picked &= दत्त->state.authhost.want;
    दत्त->state.authproxy.picked &= दत्त->state.authproxy.want;

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
    दत्त->state.wildcardmatch = दत्त->समुच्चयः.wildcard_enabled;
#यदि_परिभाषितम् DEBUGBUILD
  यदि(getenv("CURL_DBG_FTP_WILDCARD"))
    दत्त->state.wildcardmatch = TRUE;
#यदि_अन्तः
    यदि(दत्त->state.wildcardmatch) {
      संरचना Wildcard_दत्त *wc;
      यदि(!दत्त->wildcard) {
        दत्त->wildcard = curlx_calloc(1, परिमाण(संरचना Wildcard_दत्त));
        यदि(!दत्त->wildcard)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      wc = दत्त->wildcard;
      यदि(wc->state < CURLWC_INIT) {
        यदि(wc->ftpwc)
          wc->dtor(wc->ftpwc);
        curlx_safefree(wc->आदर्शः);
        curlx_safefree(wc->मार्गः);
        Curl_wildcard_init(wc); /* init wildcard structures */
      }
    }
#यदि_अन्तः
    परिणाम = Curl_hsts_loadcb(दत्त, दत्त->hsts);
  }

  दत्त->req.headerbytecount = 0;
  /* a previous transfer on this handle may have ended with a rewind request
   * still pending (a follow or auth retry that failed). This is a new
   * transfer, so drop those readers now instead of letting a later upload
   * read from them. */
  दत्त->req.rewind_read = FALSE;
  Curl_client_reset(दत्त);
  Curl_शीर्षकाणि_cleanup(दत्त);
  प्रत्यागम परिणाम;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(संरचना Curl_easy *दत्त, वर्ण **url)
{
  संरचना connectdata *conn = दत्त->conn;
  bool retry = FALSE;
  *url = रिक्त;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  यदि(दत्त->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    प्रत्यागम CURLE_OK;

  यदि(conn->द्वयङ्काः.reuse &&
     (दत्त->req.bytecount + दत्त->req.headerbytecount == 0) &&
     ((!दत्त->req.no_body && !दत्त->req.समाप्तम्) ||
      (conn->scheme->protocol & PROTO_FAMILY_HTTP))
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
     && (दत्त->समुच्चयः.rtspreq != RTSPREQ_RECEIVE)
#यदि_अन्तः
    )
    /* We got no data, we attempted to reuse a connection. For HTTP this
       can be a retry so we try again regardless if we expected a body.
       For other protocols we only try again only if we expected a body.

       This might happen if the connection was left alive when we were
       done using it before, but that was closed when we wanted to read from
       it again. Bad luck. Retry the same request on a fresh connect! */
    retry = TRUE;
  अन्यथा यदि(दत्त->state.refused_प्रवाहः &&
          (दत्त->req.bytecount + दत्त->req.headerbytecount == 0)) {
    /* This was sent on a refused stream, safe to rerun. A refused stream
       error can typically only happen on HTTP/2 level if the stream is safe
       to issue again, but the nghttp2 API can deliver the message to other
       streams as well, which is why this adds the check the data counters
       too. */
    infof(दत्त, "REFUSED_STREAM, retrying a fresh connect");
    दत्त->state.refused_प्रवाहः = FALSE; /* clear again */
    retry = TRUE;
  }
  यदि(retry) {
#परिभाषय CONN_MAX_RETRIES 5
    यदि(दत्त->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(दत्त, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      दत्त->state.retrycount = 0;
      प्रत्यागम CURLE_SEND_ERROR;
    }
    infof(दत्त, "Connection died, retrying a fresh connect (retry count: %d)",
          दत्त->state.retrycount);
    *url = Curl_bufref_dup(&दत्त->state.url);
    यदि(!*url)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    connclose(conn); /* close this connection */
    conn->द्वयङ्काः.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_समुच्चयः_rewind(दत्त, TRUE);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य xfer_सज्जता(
  संरचना Curl_easy *दत्त,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_आकार      /* how much to receive, -1 if unknown */
  )
{
  संरचना SingleRequest *k = &दत्त->req;
  संरचना connectdata *conn = दत्त->conn;

  DEBUGASSERT(conn);
  /* indexes are in range */
  DEBUGASSERT((send_idx <= 1) && (send_idx >= -1));
  DEBUGASSERT((recv_idx <= 1) && (recv_idx >= -1));
  /* if request wants to send, switching off the send direction is wrong */
  DEBUGASSERT((send_idx >= 0) || !Curl_req_want_send(दत्त));

  conn->send_idx = send_idx;
  conn->recv_idx = recv_idx;

  /* without receiving, there should be not recv_size */
  DEBUGASSERT((conn->recv_idx >= 0) || (recv_आकार == -1));
  k->आकार = recv_आकार;
  k->शीर्षकम् = !!conn->scheme->सततम्->write_resp_hd;
  /* by default, we do not shutdown at the end of the transfer */
  k->shutdown = FALSE;
  k->shutdown_err_ignore = FALSE;

  /* The code sequence below is placed in this function because all necessary
     input is not always known in do_complete() as this function may be called
     after that */
  यदि(!k->शीर्षकम् && (recv_आकार > 0))
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, recv_आकार);

  /* we want header and/or body, if neither then do not do this! */
  यदि(conn->scheme->सततम्->write_resp_hd || !दत्त->req.no_body) {
    यदि(conn->recv_idx != -1)
      CURL_REQ_समुच्चयः_RECV(दत्त);
    यदि(conn->send_idx != -1)
      CURL_REQ_समुच्चयः_SEND(दत्त);
  }
  CURL_TRC_M(दत्त, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

शून्य Curl_xfer_सज्जता_nop(संरचना Curl_easy *दत्त)
{
  xfer_सज्जता(दत्त, -1, -1, -1);
}

शून्य Curl_xfer_सज्जता_sendrecv(संरचना Curl_easy *दत्त,
                              int8_t sockindex,
                              curl_off_t recv_आकार)
{
  xfer_सज्जता(दत्त, sockindex, sockindex, recv_आकार);
}

शून्य Curl_xfer_सज्जता_send(संरचना Curl_easy *दत्त,
                          int8_t sockindex)
{
  xfer_सज्जता(दत्त, sockindex, -1, -1);
}

शून्य Curl_xfer_सज्जता_recv(संरचना Curl_easy *दत्त,
                          int8_t sockindex,
                          curl_off_t recv_आकार)
{
  xfer_सज्जता(दत्त, -1, sockindex, recv_आकार);
}

शून्य Curl_xfer_समुच्चयः_shutdown(संरचना Curl_easy *दत्त,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (दत्त->conn->send_idx < 0) || (दत्त->conn->recv_idx < 0));
  दत्त->req.shutdown = shutdown;
  दत्त->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *buf, size_t blen,
                              bool is_eos)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(दत्त->conn->scheme->सततम्->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    परिणाम = दत्त->conn->scheme->सततम्->write_resp(दत्त, buf, blen, is_eos);
  }
  अन्यथा {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    यदि(blen || is_eos) {
      पूर्णाङ्क cwtype = CLIENTWRITE_BODY;
      यदि(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      परिणाम = Curl_client_write(दत्त, cwtype, buf, blen);
    }
  }

  यदि(!परिणाम && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    दत्त->req.eos_written = TRUE;
    दत्त->req.download_समाप्तम् = TRUE;
  }
  CURL_TRC_WRITE(दत्त, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

bool Curl_xfer_write_is_paused(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_cwriter_is_paused(दत्त);
}

CURLcode Curl_xfer_write_resp_hd(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *hd0, size_t hdlen, bool is_eos)
{
  यदि(दत्त->conn->scheme->सततम्->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    प्रत्यागम दत्त->conn->scheme->सततम्->write_resp_hd(दत्त, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  प्रत्यागम Curl_xfer_write_resp(दत्त, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_समाप्तम्(संरचना Curl_easy *दत्त, bool premature)
{
  (शून्य)premature;
  प्रत्यागम Curl_cw_बहिर्गमनम्_समाप्तम्(दत्त);
}

bool Curl_xfer_needs_flush(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_conn_needs_flush(दत्त, दत्त->conn->send_idx);
}

CURLcode Curl_xfer_flush(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_conn_flush(दत्त, दत्त->conn->send_idx);
}

CURLcode Curl_xfer_send(संरचना Curl_easy *दत्त,
                        ध्रुव शून्य *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);

  परिणाम = Curl_conn_send(दत्त, दत्त->conn->send_idx,
                          buf, blen, eos, pnwritten);
  यदि(परिणाम == CURLE_AGAIN) {
    परिणाम = CURLE_OK;
    *pnwritten = 0;
  }
  अन्यथा यदि(!परिणाम && *pnwritten)
    दत्त->info.request_आकार += *pnwritten;

  DEBUGF(infof(दत्त, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (पूर्णाङ्क)परिणाम, *pnwritten));
  प्रत्यागम परिणाम;
}

CURLcode Curl_xfer_recv(संरचना Curl_easy *दत्त,
                        वर्ण *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  DEBUGASSERT(दत्त->समुच्चयः.बफर_आकार > 0);

  यदि(curlx_uitouz(दत्त->समुच्चयः.बफर_आकार) < blen)
    blen = curlx_uitouz(दत्त->समुच्चयः.बफर_आकार);
  प्रत्यागम Curl_conn_recv(दत्त, दत्त->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(संरचना Curl_easy *दत्त)
{
  Curl_conn_ev_दत्त_समाप्तम्_send(दत्त);
  प्रत्यागम CURLE_OK;
}

bool Curl_xfer_is_blocked(संरचना Curl_easy *दत्त)
{
  bool want_send = CURL_REQ_WANT_SEND(दत्त);
  bool want_recv = CURL_REQ_WANT_RECV(दत्त);
  यदि(!want_send)
    प्रत्यागम want_recv && Curl_xfer_recv_is_paused(दत्त);
  अन्यथा यदि(!want_recv)
    प्रत्यागम want_send && Curl_xfer_send_is_paused(दत्त);
  अन्यथा
    प्रत्यागम Curl_xfer_recv_is_paused(दत्त) && Curl_xfer_send_is_paused(दत्त);
}

bool Curl_xfer_send_is_paused(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_rlimit_is_blocked(&दत्त->प्रगतिः.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_rlimit_is_blocked(&दत्त->प्रगतिः.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(संरचना Curl_easy *दत्त, bool enable)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_rlimit_खण्डम्(&दत्त->प्रगतिः.ul.rlimit, enable, Curl_pgrs_now(दत्त));
  यदि(!enable && Curl_creader_is_paused(दत्त))
    परिणाम = Curl_creader_unpause(दत्त);
  Curl_pgrsSendPause(दत्त, enable);
  प्रत्यागम परिणाम;
}

CURLcode Curl_xfer_pause_recv(संरचना Curl_easy *दत्त, bool enable)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_rlimit_खण्डम्(&दत्त->प्रगतिः.dl.rlimit, enable, Curl_pgrs_now(दत्त));
  यदि(!enable && Curl_cwriter_is_paused(दत्त))
    परिणाम = Curl_cwriter_unpause(दत्त);
  Curl_conn_ev_दत्त_pause(दत्त, enable);
  Curl_pgrsRecvPause(दत्त, enable);
  प्रत्यागम परिणाम;
}

bool Curl_xfer_is_secure(संरचना Curl_easy *दत्त)
{
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(दत्त->conn && दत्त->conn->द्वयङ्काः.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(दत्त->state.origin->scheme->flags & PROTOPT_SSL));
    प्रत्यागम FALSE;
  }
#यदि_अन्तः
  प्रत्यागम (दत्त->state.origin->scheme->flags & PROTOPT_SSL);
}
