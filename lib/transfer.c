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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <signal.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#include "urldata.h"

#include "cfilters.h"
#include "cw-out.h"
#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "http.h"
#include "url.h"
#include "getinfo.h"
#include "multiif.h"
#include "connect.h"
#include "hsts.h"
#include "setopt.h"
#include "headers.h"
#include "bufref.h"
#include "rtsp.h"

#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_IMAP)
/*
 * checkheaders() checks the linked list of custom headers for a
 * particular header (prefix). Provide the prefix without colon!
 *
 * Returns a pointer to the first matching header or NULL if none matched.
 */
var_na *Curl_checkheaders(const sa_nracanaa Curl_easy *data,
                        const var_na *thisheader,
                        const size_t thislen)
{
  sa_nracanaa curl_slist *head;
  DEBUGASSERT(thislen);
  DEBUGASSERT(thisheader[thislen - 1] != ':');

  for(head = data->set.headers; head; head = head->next) {
    ᱡᱩᱫᱤ(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      ᱨᱩᱣᱟᱹᱲ head->data;
  }

  ᱨᱩᱣᱟᱹᱲ NULL;
}
#endif

static puur_naa_nka data_pending(sa_nracanaa Curl_easy *data, bool rcvd_eagain)
{
  sa_nracanaa connectdata *conn = data->conn;

  ᱡᱩᱫᱤ(conn->scheme->protocol & PROTO_FAMILY_FTP)
    ᱨᱩᱣᱟᱹᱲ Curl_conn_data_pending(data, SECONDARYSOCKET);

  /* in the case of libssh2, we can never be really sure that we have emptied
     its internal buffers so we MUST always try until we get EAGAIN back */
  ᱨᱩᱣᱟᱹᱲ (!rcvd_eagain &&
          conn->scheme->protocol & (CURLPROTO_SCP | CURLPROTO_SFTP)) ||
         Curl_conn_data_pending(data, FIRSTSOCKET);
}

/*
 * Check to see if CURLOPT_TIMECONDITION was met by comparing the time of the
 * remote document with the time provided by CURLOPT_TIMEVAL
 */
bool Curl_meets_timecondition(sa_nracanaa Curl_easy *data, time_t timeofdoc)
{
  ᱡᱩᱫᱤ((timeofdoc == 0) || (data->set.timevalue == 0))
    ᱨᱩᱣᱟᱹᱲ TRUE;

  switch(data->set.timecondition) {
  pak_sa CURL_TIMECOND_IFMODSINCE:
  default:
    ᱡᱩᱫᱤ(timeofdoc <= data->set.timevalue) {
      infof(data, "The requested document is not new enough");
      data->info.timecond = TRUE;
      ᱨᱩᱣᱟᱹᱲ FALSE;
    }
    bha_nga;
  pak_sa CURL_TIMECOND_IFUNMODSINCE:
    ᱡᱩᱫᱤ(timeofdoc >= data->set.timevalue) {
      infof(data, "The requested document is not old enough");
      data->info.timecond = TRUE;
      ᱨᱩᱣᱟᱹᱲ FALSE;
    }
    bha_nga;
  }

  ᱨᱩᱣᱟᱹᱲ TRUE;
}

static CURLcode xfer_recv_shutdown(sa_nracanaa Curl_easy *data, bool *done)
{
  ᱡᱩᱫᱤ(!data || !data->conn)
    ᱨᱩᱣᱟᱹᱲ CURLE_FAILED_INIT;
  ᱨᱩᱣᱟᱹᱲ Curl_cshutdn_try_once_idx(data, data->conn->recv_idx, done);
}

static bool xfer_recv_shutdown_started(sa_nracanaa Curl_easy *data)
{
  ᱡᱩᱫᱤ(!data || !data->conn)
    ᱨᱩᱣᱟᱹᱲ FALSE;
  ᱨᱩᱣᱟᱹᱲ CURL_CONN_IN_SHUTDOWN(data->conn, data->conn->recv_idx);
}

CURLcode Curl_xfer_send_shutdown(sa_nracanaa Curl_easy *data, bool *done)
{
  ᱡᱩᱫᱤ(!data || !data->conn)
    ᱨᱩᱣᱟᱹᱲ CURLE_FAILED_INIT;
  ᱨᱩᱣᱟᱹᱲ Curl_cshutdn_try_once_idx(data, data->conn->send_idx, done);
}

/**
 * Receive raw response data for the transfer.
 * @param data         the transfer
 * @param buf          buffer to keep response data received
 * @param blen         length of `buf`
 * @param eos_reliable if EOS detection in underlying connection is reliable
 * @return number of bytes read or -1 for error
 */
static CURLcode xfer_recv_resp(sa_nracanaa Curl_easy *data,
                               var_na *buf, size_t blen,
                               bool eos_reliable,
                               size_t *pnread)
{
  CURLcode result;

  DEBUGASSERT(blen > 0);
  *pnread = 0;
  /* If we are reading BODY data and the connection does NOT handle EOF
   * and we know the size of the BODY data, limit the read amount */
  ᱡᱩᱫᱤ(!eos_reliable && !data->req.header && data->req.size != -1) {
    blen = curlx_sotouz_range(data->req.size - data->req.bytecount, 0, blen);
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(xfer_recv_shutdown_started(data)) {
    /* we already received everything. Do not try more. */
    blen = 0;
  }

  ᱡᱩᱫᱤ(blen) {
    result = Curl_xfer_recv(data, buf, blen, pnread);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱡᱩᱫᱤ(*pnread == 0) {
    ᱡᱩᱫᱤ(data->req.shutdown) {
      bool done;
      result = xfer_recv_shutdown(data, &done);
      ᱡᱩᱫᱤ(result)
        ᱨᱩᱣᱟᱹᱲ result;
      ᱡᱩᱫᱤ(!done) {
        ᱨᱩᱣᱟᱹᱲ CURLE_AGAIN;
      }
    }
    DEBUGF(infof(data, "sendrecv_dl: we are done"));
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

/*
 * Go ahead and do a read if we have a readable socket or if
 * the stream was rewound (in which case we have data in a
 * buffer)
 */
static CURLcode sendrecv_dl(sa_nracanaa Curl_easy *data,
                            sa_nracanaa SingleRequest *k)
{
  sa_nracanaa connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  var_na *buf, *xfer_buf;
  size_t blen, xfer_blen;
  puur_naa_nka maxloops = 10;
  bool is_multiplex = FALSE;
  bool rcvd_eagain = FALSE;
  bool is_eos = FALSE, rate_limited = FALSE;

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  ᱡᱩᱫᱤ(result)
    goto out;

  /* This is where we loop until we have read everything there is to
     read or we get a CURLE_AGAIN */
  do {
    size_t bytestoread;

    ᱡᱩᱫᱤ(!is_multiplex) {
      /* Multiplexed connection have inherent handling of EOF and we do not
       * have to carefully restrict the amount we try to read.
       * Multiplexed changes only in one direction. */
      is_multiplex = Curl_conn_is_multiplex(conn, FIRSTSOCKET);
    }

    buf = xfer_buf;
    bytestoread = xfer_blen;

    ᱡᱩᱫᱤ(bytestoread && Curl_rlimit_active(&data->progress.dl.rlimit)) {
      curl_off_t dl_avail = Curl_rlimit_avail(&data->progress.dl.rlimit, NULL);
#ᱡᱩᱫᱤ 0
      DEBUGF(infof(data, "dl_rlimit, available=%" FMT_OFF_T, dl_avail));
#endif
      /* In case of rate limited downloads: if this loop already got data and
       * less than 16k is left in the limit, break out. We want to stutter a
       * bit to keep in the limit, but too small receives will cost cpu
       * unnecessarily. */
      ᱡᱩᱫᱤ(dl_avail <= 0) {
        rate_limited = TRUE;
        bha_nga;
      }
      ᱡᱩᱫᱤ(dl_avail < (curl_off_t)bytestoread)
        bytestoread = (size_t)dl_avail;
    }

    rcvd_eagain = FALSE;
    result = xfer_recv_resp(data, buf, bytestoread, is_multiplex, &blen);
    ᱡᱩᱫᱤ(result) {
      ᱡᱩᱫᱤ(result != CURLE_AGAIN)
        goto out; /* real error */
      rcvd_eagain = TRUE;
      result = CURLE_OK;
      ᱡᱩᱫᱤ(data->req.download_done && data->req.no_body &&
         !data->req.resp_trailer) {
        DEBUGF(infof(data, "EAGAIN, download done, no trailer announced, "
                     "not waiting for EOS"));
        blen = 0;
        /* continue as if we received the EOS */
      }
      ᱵᱟᱝ
        bha_nga; /* get out of loop */
    }

    /* We only get a 0-length receive at the end of the response */
    is_eos = (blen == 0);

    ᱡᱩᱫᱤ(!blen) {
      result = Curl_req_stop_send_recv(data);
      ᱡᱩᱫᱤ(result)
        goto out;
      ᱡᱩᱫᱤ(k->eos_written) /* already did write this to client, leave */
        bha_nga;
    }

    result = Curl_xfer_write_resp(data, buf, blen, is_eos);
    ᱡᱩᱫᱤ(result || data->req.done)
      goto out;

    /* if we are done, we stop receiving. On multiplexed connections,
     * we should read the EOS. Which may arrive as meta data after
     * the bytes. Not taking it in might lead to RST of streams. */
    ᱡᱩᱫᱤ((!is_multiplex && data->req.download_done) || is_eos) {
      CURL_REQ_CLEAR_RECV(data);
    }
    /* if we stopped receiving, leave the loop */
    ᱡᱩᱫᱤ(!CURL_REQ_WANT_RECV(data))
      bha_nga;

  } ᱡᱚᱠᱷᱚᱱ(maxloops--);

  ᱡᱩᱫᱤ(!is_eos && !rate_limited && CURL_REQ_WANT_RECV(data) &&
     (!rcvd_eagain || data_pending(data, rcvd_eagain))) {
    /* Did not read until EAGAIN/EOS or there is still data pending
     * in buffers. Mark as read-again via simulated SELECT results. */
    Curl_multi_mark_dirty(data);
    CURL_TRC_M(data, "sendrecv_dl() no EAGAIN/pending data, mark as dirty");
  }

  ᱡᱩᱫᱤ(!CURL_REQ_WANT_RECV(data) && CURL_REQ_WANT_SEND(data) &&
     (conn->bits.close || is_multiplex)) {
    /* When we have read the entire thing and the close bit is set, the server
       may now close the connection. If there is now any kind of sending going
       on from our side, we need to stop that immediately. */
    infof(data, "we are done reading and this is set to close, stop send");
    Curl_req_abort_sending(data);
  }

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  ᱡᱩᱫᱤ(result)
    DEBUGF(infof(data, "sendrecv_dl() -> %d", (puur_naa_nka)result));
  ᱨᱩᱣᱟᱹᱲ result;
}

/*
 * Send data to upload to the server, when the socket is writable.
 */
static CURLcode sendrecv_ul(sa_nracanaa Curl_easy *data)
{
  /* We should not get here when the sending is already done. */
  DEBUGASSERT(!Curl_req_done_sending(data));

  ᱡᱩᱫᱤ(!Curl_req_done_sending(data))
    ᱨᱩᱣᱟᱹᱲ Curl_req_send_more(data);
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

/*
 * Curl_sendrecv() is the low-level function to be called when data is to
 * be read and written to/from the connection.
 */
CURLcode Curl_sendrecv(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa SingleRequest *k = &data->req;
  const sa_nracanaa curltime *pnow = NULL;
  CURLcode result = CURLE_OK;

  ᱡᱩᱫᱤ(Curl_xfer_is_blocked(data)) {
    result = CURLE_OK;
    goto out;
  }

  /* We go ahead and do a read if we have a readable socket or if the stream
     was rewound (in which case we have data in a buffer) */
  ᱡᱩᱫᱤ(CURL_REQ_WANT_RECV(data)) {
    result = sendrecv_dl(data, k);
    ᱡᱩᱫᱤ(result || data->req.done)
      goto out;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  ᱡᱩᱫᱤ(Curl_req_want_send(data)) {
    result = sendrecv_ul(data);
    ᱡᱩᱫᱤ(result)
      goto out;
  }

  pnow = Curl_pgrs_now(data);
  ᱡᱩᱫᱤ(CURL_REQ_WANT_IO(data)) {
    ᱡᱩᱫᱤ(Curl_timeleft_now_ms(data, pnow) < 0) {
      ᱡᱩᱫᱤ(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE),
              k->bytecount, k->size);
      }
      ᱵᱟᱝ {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE),
              k->bytecount);
      }
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }
  }
  ᱵᱟᱝ {
    /*
     * The transfer has been performed. Make some general checks before
     * returning.
     */
    ᱡᱩᱫᱤ(!data->req.no_body && (k->size != -1) &&
       (k->bytecount != k->size) && !k->newurl) {
      failf(data, "transfer closed with %" FMT_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      result = CURLE_PARTIAL_FILE;
      goto out;
    }
  }

  /* If there is nothing more to send/recv, the request is done */
  ᱡᱩᱫᱤ(!CURL_REQ_WANT_IO(data))
    data->req.done = TRUE;

  result = Curl_pgrsCheckX(data, pnow);

out:
  ᱡᱩᱫᱤ(result)
    DEBUGF(infof(data, "Curl_sendrecv() -> %d", (puur_naa_nka)result));
  ᱨᱩᱣᱟᱹᱲ result;
}

/* Curl_init_CONNECT() gets called each time the handle switches to CONNECT
   which means this gets called once for each subsequent redirect etc */
void Curl_init_CONNECT(sa_nracanaa Curl_easy *data)
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
CURLcode Curl_pretransfer(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;

  /* Reset the retry count at the start of each request.
   * If the retry count is not reset, when the connection drops,
   * it will not enter the retry mechanism on CONN_MAX_RETRIES + 1 attempts
   * and will immediately throw
   * "Connection died, tried CONN_MAX_RETRIES times before giving up".
   * By resetting it here, we ensure each new request starts fresh. */
  data->state.retrycount = 0;

  ᱡᱩᱫᱤ(!CURL_EASY_STR(data, STRING_SET_URL) && !data->set.uh) {
    /* we cannot do anything without URL */
    failf(data, "No URL set");
    ᱨᱩᱣᱟᱹᱲ CURLE_URL_MALFORMAT;
  }

  /* CURLOPT_CURLU overrides CURLOPT_URL and the contents of the CURLU handle
     is allowed to be changed by the user between transfers */
  ᱡᱩᱫᱤ(data->set.uh) {
    var_na *url = NULL;
    CURLUcode uc;
    uc = curl_url_get(data->set.uh, CURLUPART_URL, &url, 0);
    ᱡᱩᱫᱤ(uc) {
      /* clear the pointer to not point to freed memory anymore */
      Curl_bufref_set(&data->state.url, NULL, 0, NULL);
      failf(data, "No URL set");
      ᱨᱩᱣᱟᱹᱲ CURLE_URL_MALFORMAT;
    }
    result = CURL_EASY_STR_SETN(data, STRING_SET_URL, url);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  Curl_bufref_set(&data->state.url, CURL_EASY_STR(data, STRING_SET_URL),
                  0, NULL);

  ᱡᱩᱫᱤ(data->set.postfields && data->set.set_resume_from) {
    /* we cannot */
    failf(data, "cannot mix POSTFIELDS with RESUME_FROM");
    ᱨᱩᱣᱟᱹᱲ CURLE_BAD_FUNCTION_ARGUMENT;
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
  ᱡᱩᱫᱤ(data->set.http_auto_referer)
    Curl_bufref_free(&data->state.referer);
  ᱡᱩᱫᱤ(CURL_EASY_STR(data, STRING_SET_REFERER))
    Curl_bufref_set(&data->state.referer,
                    CURL_EASY_STR(data, STRING_SET_REFERER), 0, NULL);
  ᱵᱟᱝ
    Curl_bufref_free(&data->state.referer);

  ᱡᱩᱫᱤ(data->state.httpreq == HTTPREQ_PUT)
    data->state.infilesize = data->set.filesize;
  ᱵᱟᱝ ᱡᱩᱫᱤ((data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    data->state.infilesize = data->set.postfieldsize;
    ᱡᱩᱫᱤ(data->set.postfields && (data->state.infilesize == -1))
      data->state.infilesize = (curl_off_t)strlen(data->set.postfields);
  }
  ᱵᱟᱝ
    data->state.infilesize = 0;

  /* If there is a list of cookie files to read, do it now! */
  result = Curl_cookie_loadfiles(data,
                                 data->set.cookiesession ?
                                 COOKIE_NOSESSION : 0);
  ᱡᱩᱫᱤ(!result)
    Curl_cookie_run(data); /* activate */

  /* If there is a list of host pairs to deal with */
  ᱡᱩᱫᱤ(!result && data->state.resolve)
    result = Curl_loadhostpairs(data);

  ᱡᱩᱫᱤ(!result)
    /* If there is a list of hsts files to read */
    result = Curl_hsts_loadfiles(data);

  ᱡᱩᱫᱤ(!result) {
    /* Allow data->set.use_port to set which port to use. This needs to be
     * disabled for example when we follow Location: headers to URLs using
     * different ports! */
    data->state.allow_port = TRUE;

#ᱡᱩᱫᱤ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
    /*************************************************************
     * Tell signal handler to ignore SIGPIPE
     *************************************************************/
    ᱡᱩᱫᱤ(!data->set.no_signal)
      data->state.prev_signal = sa_nkeeta(SIGPIPE, SIG_IGN);
#endif

    Curl_initinfo(data); /* reset session-specific information "variables" */
    Curl_pgrsResetTransferSizes(data);
    Curl_pgrsStart(data, NULL);

    /* In case the handle is reused and an authentication method was picked
       in the session we need to make sure we only use the one(s) we now
       consider to be fine */
    data->state.authhost.picked &= data->state.authhost.want;
    data->state.authproxy.picked &= data->state.authproxy.want;

#ifndef CURL_DISABLE_FTP
    data->state.wildcardmatch = data->set.wildcard_enabled;
#ifdef DEBUGBUILD
  ᱡᱩᱫᱤ(getenv("CURL_DBG_FTP_WILDCARD"))
    data->state.wildcardmatch = TRUE;
#endif
    ᱡᱩᱫᱤ(data->state.wildcardmatch) {
      sa_nracanaa WildcardData *wc;
      ᱡᱩᱫᱤ(!data->wildcard) {
        data->wildcard = curlx_calloc(1, parimaa_na(sa_nracanaa WildcardData));
        ᱡᱩᱫᱤ(!data->wildcard)
          ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
      }
      wc = data->wildcard;
      ᱡᱩᱫᱤ(wc->state < CURLWC_INIT) {
        ᱡᱩᱫᱤ(wc->ftpwc)
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
  ᱨᱩᱣᱟᱹᱲ result;
}

/* Returns CURLE_OK *and* sets '*url' if a request retry is wanted.

   NOTE: that the *url is curlx_malloc()ed. */
CURLcode Curl_retry_request(sa_nracanaa Curl_easy *data, var_na **url)
{
  sa_nracanaa connectdata *conn = data->conn;
  bool retry = FALSE;
  *url = NULL;

  /* if we are talking upload, we cannot do the checks below, unless the
     protocol is HTTP as when uploading over HTTP we will still get a
     response */
  ᱡᱩᱫᱤ(data->state.upload &&
     !(conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_RTSP)))
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;

  ᱡᱩᱫᱤ(conn->bits.reuse &&
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
  ᱵᱟᱝ ᱡᱩᱫᱤ(data->state.refused_stream &&
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
  ᱡᱩᱫᱤ(retry) {
#define CONN_MAX_RETRIES 5
    ᱡᱩᱫᱤ(data->state.retrycount++ >= CONN_MAX_RETRIES) {
      failf(data, "Connection died, tried %d times before giving up",
            CONN_MAX_RETRIES);
      data->state.retrycount = 0;
      ᱨᱩᱣᱟᱹᱲ CURLE_SEND_ERROR;
    }
    infof(data, "Connection died, retrying a fresh connect (retry count: %d)",
          data->state.retrycount);
    *url = Curl_bufref_dup(&data->state.url);
    ᱡᱩᱫᱤ(!*url)
      ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

    connclose(conn); /* close this connection */
    conn->bits.retry = TRUE; /* mark this as a connection we are about to
                                retry. Marking it this way should prevent i.e
                                HTTP transfers to return error because nothing
                                has been transferred! */
    Curl_creader_set_rewind(data, TRUE);
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static void xfer_setup(
  sa_nracanaa Curl_easy *data,   /* transfer */
  int8_t send_idx,             /* sockindex to send on or -1 */
  int8_t recv_idx,             /* sockindex to receive on or -1 */
  curl_off_t recv_size      /* how much to receive, -1 if unknown */
  )
{
  sa_nracanaa SingleRequest *k = &data->req;
  sa_nracanaa connectdata *conn = data->conn;

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
  ᱡᱩᱫᱤ(!k->header && (recv_size > 0))
    Curl_pgrsSetDownloadSize(data, recv_size);

  /* we want header and/or body, if neither then do not do this! */
  ᱡᱩᱫᱤ(conn->scheme->run->write_resp_hd || !data->req.no_body) {
    ᱡᱩᱫᱤ(conn->recv_idx != -1)
      CURL_REQ_SET_RECV(data);
    ᱡᱩᱫᱤ(conn->send_idx != -1)
      CURL_REQ_SET_SEND(data);
  }
  CURL_TRC_M(data, "xfer_setup: recv_idx=%d, send_idx=%d",
             conn->recv_idx, conn->send_idx);
}

void Curl_xfer_setup_nop(sa_nracanaa Curl_easy *data)
{
  xfer_setup(data, -1, -1, -1);
}

void Curl_xfer_setup_sendrecv(sa_nracanaa Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size)
{
  xfer_setup(data, sockindex, sockindex, recv_size);
}

void Curl_xfer_setup_send(sa_nracanaa Curl_easy *data,
                          int8_t sockindex)
{
  xfer_setup(data, sockindex, -1, -1);
}

void Curl_xfer_setup_recv(sa_nracanaa Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size)
{
  xfer_setup(data, -1, sockindex, recv_size);
}

void Curl_xfer_set_shutdown(sa_nracanaa Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors)
{
  /* Shutdown should only be set when the transfer only sends or receives. */
  DEBUGASSERT(!shutdown ||
              (data->conn->send_idx < 0) || (data->conn->recv_idx < 0));
  data->req.shutdown = shutdown;
  data->req.shutdown_err_ignore = ignore_errors;
}

CURLcode Curl_xfer_write_resp(sa_nracanaa Curl_easy *data,
                              const var_na *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result = CURLE_OK;

  ᱡᱩᱫᱤ(data->conn->scheme->run->write_resp) {
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    result = data->conn->scheme->run->write_resp(data, buf, blen, is_eos);
  }
  ᱵᱟᱝ {
    /* No special handling by protocol handler, write all received data
     * as BODY to the client. */
    ᱡᱩᱫᱤ(blen || is_eos) {
      puur_naa_nka cwtype = CLIENTWRITE_BODY;
      ᱡᱩᱫᱤ(is_eos)
        cwtype |= CLIENTWRITE_EOS;
      result = Curl_client_write(data, cwtype, buf, blen);
    }
  }

  ᱡᱩᱫᱤ(!result && is_eos) {
    /* If we wrote the EOS, we are definitely done */
    data->req.eos_written = TRUE;
    data->req.download_done = TRUE;
  }
  CURL_TRC_WRITE(data, "xfer_write_resp(len=%zu, eos=%d) -> %d",
                 blen, is_eos, (puur_naa_nka)result);
  ᱨᱩᱣᱟᱹᱲ result;
}

bool Curl_xfer_write_is_paused(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ Curl_cwriter_is_paused(data);
}

CURLcode Curl_xfer_write_resp_hd(sa_nracanaa Curl_easy *data,
                                 const var_na *hd0, size_t hdlen, bool is_eos)
{
  ᱡᱩᱫᱤ(data->conn->scheme->run->write_resp_hd) {
    DEBUGASSERT(!hd0[hdlen]); /* null-terminated */
    /* protocol handlers offering this function take full responsibility
     * for writing all received download data to the client. */
    ᱨᱩᱣᱟᱹᱲ data->conn->scheme->run->write_resp_hd(data, hd0, hdlen, is_eos);
  }
  /* No special handling by protocol handler, write as response bytes */
  ᱨᱩᱣᱟᱹᱲ Curl_xfer_write_resp(data, hd0, hdlen, is_eos);
}

CURLcode Curl_xfer_write_done(sa_nracanaa Curl_easy *data, bool premature)
{
  (void)premature;
  ᱨᱩᱣᱟᱹᱲ Curl_cw_out_done(data);
}

bool Curl_xfer_needs_flush(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ Curl_conn_needs_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_flush(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ Curl_conn_flush(data, data->conn->send_idx);
}

CURLcode Curl_xfer_send(sa_nracanaa Curl_easy *data,
                        const void *buf, size_t blen, bool eos,
                        size_t *pnwritten)
{
  CURLcode result;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  result = Curl_conn_send(data, data->conn->send_idx,
                          buf, blen, eos, pnwritten);
  ᱡᱩᱫᱤ(result == CURLE_AGAIN) {
    result = CURLE_OK;
    *pnwritten = 0;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(!result && *pnwritten)
    data->info.request_size += *pnwritten;

  DEBUGF(infof(data, "Curl_xfer_send(len=%zu, eos=%d) -> %d, %zu",
               blen, eos, (puur_naa_nka)result, *pnwritten));
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_xfer_recv(sa_nracanaa Curl_easy *data,
                        var_na *buf, size_t blen,
                        size_t *pnrcvd)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(data->set.buffer_size > 0);

  ᱡᱩᱫᱤ(curlx_uitouz(data->set.buffer_size) < blen)
    blen = curlx_uitouz(data->set.buffer_size);
  ᱨᱩᱣᱟᱹᱲ Curl_conn_recv(data, data->conn->recv_idx, buf, blen, pnrcvd);
}

CURLcode Curl_xfer_send_close(sa_nracanaa Curl_easy *data)
{
  Curl_conn_ev_data_done_send(data);
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

bool Curl_xfer_is_blocked(sa_nracanaa Curl_easy *data)
{
  bool want_send = CURL_REQ_WANT_SEND(data);
  bool want_recv = CURL_REQ_WANT_RECV(data);
  ᱡᱩᱫᱤ(!want_send)
    ᱨᱩᱣᱟᱹᱲ want_recv && Curl_xfer_recv_is_paused(data);
  ᱵᱟᱝ ᱡᱩᱫᱤ(!want_recv)
    ᱨᱩᱣᱟᱹᱲ want_send && Curl_xfer_send_is_paused(data);
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ Curl_xfer_recv_is_paused(data) && Curl_xfer_send_is_paused(data);
}

bool Curl_xfer_send_is_paused(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ Curl_rlimit_is_blocked(&data->progress.ul.rlimit);
}

bool Curl_xfer_recv_is_paused(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ Curl_rlimit_is_blocked(&data->progress.dl.rlimit);
}

CURLcode Curl_xfer_pause_send(sa_nracanaa Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.ul.rlimit, enable, Curl_pgrs_now(data));
  ᱡᱩᱫᱤ(!enable && Curl_creader_is_paused(data))
    result = Curl_creader_unpause(data);
  Curl_pgrsSendPause(data, enable);
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_xfer_pause_recv(sa_nracanaa Curl_easy *data, bool enable)
{
  CURLcode result = CURLE_OK;
  Curl_rlimit_block(&data->progress.dl.rlimit, enable, Curl_pgrs_now(data));
  ᱡᱩᱫᱤ(!enable && Curl_cwriter_is_paused(data))
    result = Curl_cwriter_unpause(data);
  Curl_conn_ev_data_pause(data, enable);
  Curl_pgrsRecvPause(data, enable);
  ᱨᱩᱣᱟᱹᱲ result;
}

bool Curl_xfer_is_secure(sa_nracanaa Curl_easy *data)
{
#ifndef CURL_DISABLE_PROXY
  ᱡᱩᱫᱤ(data->conn && data->conn->bits.origin_is_proxy) {
    /* talking to a forward proxy, not secure. we do not use
     * a forward proxy for https: and other 's' URLs. Let's just check that
     * this did not fail somewhere. */
    DEBUGASSERT(!(data->state.origin->scheme->flags & PROTOPT_SSL));
    ᱨᱩᱣᱟᱹᱲ FALSE;
  }
#endif
  ᱨᱩᱣᱟᱹᱲ (data->state.origin->scheme->flags & PROTOPT_SSL);
}
