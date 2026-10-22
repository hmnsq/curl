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
#ضمن "urldata.h"
#ضمن "tftp.h"

#جے_بنا_تعریف CURL_DISABLE_TFTP

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "connect.h"
#ضمن "sockaddr.h" /* required for Curl_sockaddr_storage */
#ضمن "url.h"
#ضمن "strcase.h"
#ضمن "select.h"
#ضمن "escape.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#تعریف_کرو TFTP_BLKSIZE_DEFAULT 512
#تعریف_کرو TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#تعریف_کرو TFTP_OPTION_TSIZE    "tsize"
#تعریف_کرو TFTP_OPTION_INTERVAL "timeout"

تعريفنوع تعداد {
  TFTP_منوال_NETASCII = 0,
  TFTP_منوال_OCTET
} tftp_منوال_t;

تعريفنوع تعداد {
  TFTP_STATE_شروع = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

تعريفنوع تعداد {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_انکڑے = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

تعريفنوع تعداد {
  TFTP_ERR_UNDEF = 0,
  TFTP_ERR_NOTFOUND,
  TFTP_ERR_PERM,
  TFTP_ERR_DISKFULL,
  TFTP_ERR_ILLEGAL,
  TFTP_ERR_UNKNOWNID,
  TFTP_ERR_EXISTS,
  TFTP_ERR_NOSUCHUSER,  /* This will never be triggered by this code */

  /* The remaining error codes are internal to curl */
  TFTP_ERR_NONE = -100,
  TFTP_ERR_TIMEOUT,
  TFTP_ERR_NORESPONSE
} tftp_error_t;

بنية tftp_packet {
  بنانشانی حرف *انکڑے;
};

/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

بنية tftp_conn {
  بنية Curl_sockaddr_storage local_addr;
  بنية Curl_sockaddr_storage remote_addr;
  بنية tftp_packet rpacket;
  بنية tftp_packet spacket;
  tftp_state_t    state;
  tftp_منوال_t     منوال;
  tftp_error_t    error;
  tftp_event_t    event;
  بنية Curl_easy *انکڑے;
  curl_socket_t   sockfd;
  صحيح             retries;
  صحيح             retry_ویلا;
  صحيح             retry_max;
  time_t          rx_ویلا;
  curl_socklen_t  remote_addrlen;
  صحيح             rbytes;
  size_t          sbytes;
  بنانشانی صحيح    blksize;
  بنانشانی صحيح    requested_blksize;
  بنانشانی چھوٹا  بلاک;
  ثنائی_ہندسہ(remote_pinned);
};

/**********************************************************
 *
 * tftp_set_timeouts -
 *
 * Set timeouts based on state machine state.
 * Use user provided connect timeouts until DATA or ACK
 * packet is received, then use user-provided transfer timeouts
 *
 *
 **********************************************************/
ساكن CURLcode tftp_سیٹ_timeouts(بنية tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->انکڑے);

  إذا(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->انکڑے, "Connection time-out");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  إذا((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  وإلا
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (صحيح)timeout / 5;

  /* Bound the total number */
  إذا(state->retry_max < 3)
    state->retry_max = 3;

  إذا(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_ویلا = (صحيح)(timeout / state->retry_max);
  إذا(state->retry_ویلا < 1)
    state->retry_ویلا = 1;

  infof(state->انکڑے,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_ویلا, state->retry_max);

  /* init RX time */
  state->rx_ویلا = ویلا(عدم);

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, بنانشانی چھوٹا عدد)
{
  packet->انکڑے[0] = (بنانشانی حرف)(عدد >> 8);
  packet->انکڑے[1] = (بنانشانی حرف)(عدد & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, بنانشانی چھوٹا عدد)
{
  packet->انکڑے[2] = (بنانشانی حرف)(عدد >> 8);
  packet->انکڑے[3] = (بنانشانی حرف)(عدد & 0xff);
}

ساكن بنانشانی چھوٹا getrpacketevent(ثابت بنية tftp_packet *packet)
{
  أرجع (بنانشانی چھوٹا)((packet->انکڑے[0] << 8) | packet->انکڑے[1]);
}

ساكن بنانشانی چھوٹا getrpacketblock(ثابت بنية tftp_packet *packet)
{
  أرجع (بنانشانی چھوٹا)((packet->انکڑے[2] << 8) | packet->انکڑے[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *سرا = memchr(string, '\0', maxlen);
  أرجع سرا ? (size_t)(سرا - string) : maxlen;
}

ساكن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **قدر)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  إذا(loc >= len)
    أرجع عدم;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  إذا(loc > len)
    أرجع عدم;
  *قدر = &buf[لڑی_لمبائی(*option) + 1];

  أرجع &buf[loc];
}

ساكن CURLcode tftp_parse_option_ack(بنية tftp_conn *state,
                                      ثابت حرف *ptr, صحيح len)
{
  ثابت حرف *tmp = ptr;
  بنية Curl_easy *انکڑے = state->انکڑے;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  طالما(tmp < ptr + len) {
    ثابت حرف *option, *قدر;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &قدر);
    إذا(!tmp) {
      failf(انکڑے, "Malformed ACK packet, rejecting");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    olen = لڑی_لمبائی(option);

    infof(انکڑے, "got option=(%s) value=(%s)", option, قدر);

    إذا((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      إذا(curlx_str_نمبر(&قدر, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(انکڑے, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      إذا(!blksize) {
        failf(انکڑے, "invalid blocksize value in OACK packet");
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize < TFTP_BLKSIZE_MIN) {
        failf(انکڑے, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(انکڑے, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        أرجع CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (بنانشانی صحيح)blksize;
      infof(انکڑے, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وإلا إذا((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      إذا(!انکڑے->state.upload &&
         !curlx_str_نمبر(&قدر, &tsize, CURL_OFF_T_MAX)) {
        إذا(!tsize) {
          failf(انکڑے, "invalid tsize -:%s:- value in OACK packet", قدر);
          أرجع CURLE_TFTP_ILLEGAL;
        }
        infof(انکڑے, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_سیٹ_Download_ناپ(انکڑے, tsize);
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode tftp_option_جوڑو(بنية tftp_conn *state, size_t *csize,
                                size_t اشاریہ, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.انکڑے[اشاریہ];
  size_t oplen = لڑی_لمبائی(option);
  size_t blen;
  إذا((state->blksize <= اشاریہ) ||
     (oplen + 1) > (size_t)(state->blksize - اشاریہ))
    أرجع CURLE_TFTP_ILLEGAL;
  blen = state->blksize - اشاریہ;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  أرجع CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#تعریف_کرو اگلا_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ساكن CURLcode tftp_tx(بنية tftp_conn *state, tftp_event_t event)
{
  بنية Curl_easy *انکڑے = state->انکڑے;
  ssize_t sbytes;
  CURLcode نتیجہ = CURLE_OK;
  بنية SingleRequest *k = &انکڑے->req;
  size_t cb; /* Bytes currently read */
  حرف بفر[STRERROR_LEN];
  حرف *bufptr;
  bool eos;

  إذا(!انکڑے->state.upload)
    أرجع CURLE_TFTP_ILLEGAL;

  بدل(event) {

  حالة TFTP_EVENT_ACK:
  حالة TFTP_EVENT_OACK:
    إذا(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحيح rblock = getrpacketblock(&state->rpacket);

      إذا(rblock != state->بلاک &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->بلاک == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(انکڑے, "Received ACK for block %d, expecting %d",
              rblock, state->بلاک);
        state->retries++;
        /* Bail out if over the maximum */
        إذا(state->retries > state->retry_max) {
          failf(انکڑے, "tftp_tx: giving up waiting for block %d ack",
                state->بلاک);
          نتیجہ = CURLE_SEND_ERROR;
        }
        وإلا {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (بنية sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          إذا(sbytes < 0) {
            failf(انکڑے, "%s",
                  curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
            نتیجہ = CURLE_SEND_ERROR;
          }
        }

        أرجع نتیجہ;
      }
      /* This is the expected packet. Reset the counters and send the next
بلاک*/
      state->rx_ویلا = ویلا(عدم);
      state->بلاک++;
    }
    وإلا
      state->بلاک = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_انکڑے);
    setpacketblock(&state->spacket, state->بلاک);
    إذا(state->بلاک > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      أرجع CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (حرف *)state->spacket.انکڑے + 4;
    افعل {
      نتیجہ = Curl_client_read(انکڑے, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      إذا(نتیجہ)
        أرجع نتیجہ;
      state->sbytes += cb;
      bufptr += cb;
    } طالما(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    إذا(sbytes < 0) {
      failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(انکڑے, state->sbytes);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(انکڑے, "Timeout waiting for block %d ACK. "
          "Retries = %d", اگلا_BLOCKNUM(state->بلاک), state->retries);
    /* Decide if we have had enough */
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      إذا(sbytes < 0) {
        failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        أرجع CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_سیٹ_Upload_Counter(انکڑے, k->writebytecount);
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.انکڑے, 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(انکڑے, "tftp_tx: internal error, event: %d", (صحيح)event);
    اكسر;
  }

  أرجع نتیجہ;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجہ;

  infof(state->انکڑے, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  نتیجہ = tftp_سیٹ_timeouts(state);
  إذا(نتیجہ)
    أرجع نتیجہ;
  أرجع tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ساكن CURLcode tftp_rx(بنية tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  صحيح rblock;
  بنية Curl_easy *انکڑے = state->انکڑے;
  حرف بفر[STRERROR_LEN];

  بدل(event) {

  حالة TFTP_EVENT_انکڑے:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    إذا(اگلا_BLOCKNUM(state->بلاک) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وإلا إذا(state->بلاک == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(انکڑے, "Received last DATA packet block %d again.", rblock);
    }
    وإلا {
      /* totally unexpected, log it */
      infof(انکڑے,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, اگلا_BLOCKNUM(state->بلاک));
      اكسر;
    }

    /* ACK this block. */
    state->بلاک = (بنانشانی چھوٹا)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    إذا(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      state->state = TFTP_STATE_RX;
    }
    state->rx_ویلا = ویلا(عدم);
    اكسر;

  حالة TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->بلاک = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_ویلا = ویلا(عدم);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(انکڑے,
          "Timeout waiting for block %d ACK. Retries = %d",
          اگلا_BLOCKNUM(state->بلاک), state->retries);
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                      4, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      إذا(sbytes < 0) {
        failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        أرجع CURLE_SEND_ERROR;
      }
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(انکڑے, "%s", "tftp_rx: internal error");
    أرجع CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  أرجع CURLE_OK;
}

ساكن CURLcode tftp_connect_for_rx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجہ;

  infof(state->انکڑے, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  نتیجہ = tftp_سیٹ_timeouts(state);
  إذا(نتیجہ)
    أرجع نتیجہ;
  أرجع tftp_rx(state, event);
}

ساكن CURLcode tftp_send_پہلا(بنية tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *منوال = "octet";
  حرف *filename;
  بنية Curl_easy *انکڑے = state->انکڑے;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتیجہ = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  إذا(انکڑے->state.prefer_ascii)
    منوال = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      أرجع نتیجہ;
    }

    إذا(انکڑے->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      إذا(انکڑے->state.infilesize != -1)
        Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);
    }
    وإلا {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    إذا(!state->انکڑے->state.اتے.رستہ[1]) {
      failf(انکڑے, "Missing filename");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    نتیجہ = Curl_urldecode(&state->انکڑے->state.اتے.رستہ[1], 0,
                            &filename, عدم, REJECT_ZERO);
    إذا(نتیجہ)
      أرجع نتیجہ;

    إذا(لڑی_لمبائی(filename) + لڑی_لمبائی(منوال) + 4 > state->blksize) {
      failf(انکڑے, "TFTP filename too long");
      curlx_free(filename);
      أرجع CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.انکڑے + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', منوال, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    إذا(!انکڑے->سیٹ.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     انکڑے->state.upload && (انکڑے->state.infilesize != -1) ?
                     انکڑے->state.infilesize : 0);

      نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      إذا(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      إذا(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      إذا(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_ویلا);
      إذا(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      إذا(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      إذا(نتیجہ != CURLE_OK) {
        failf(انکڑے, "TFTP buffer too small for options");
        أرجع CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#جے_تعریف_شدہ __AMIGA__
#تعریف_کرو CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وإلا
#تعریف_کرو CURL_SENDTO_ARG5(x) (x)
#جے_انت
    remote_addr = Curl_conn_get_remote_addr(انکڑے, FIRSTSOCKET);
    إذا(!remote_addr)
      أرجع CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (فراغ *)state->spacket.انکڑے,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    إذا(senddata != (ssize_t)sbytes) {
      حرف بفر[STRERROR_LEN];
      failf(انکڑے, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    اكسر;

  حالة TFTP_EVENT_OACK:
    إذا(انکڑے->state.upload) {
      نتیجہ = tftp_connect_for_tx(state, event);
    }
    وإلا {
      نتیجہ = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    إذا(!انکڑے->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتیجہ = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_انکڑے: /* Connected for receive */
    إذا(انکڑے->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتیجہ = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->انکڑے, "tftp_send_first: internal error");
    أرجع CURLE_TFTP_ILLEGAL;
  }

  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساكن CURLcode tftp_translate_کوڈ(tftp_error_t error)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(error != TFTP_ERR_NONE) {
    بدل(error) {
    حالة TFTP_ERR_NOTFOUND:
      نتیجہ = CURLE_TFTP_NOTFOUND;
      اكسر;
    حالة TFTP_ERR_PERM:
      نتیجہ = CURLE_TFTP_PERM;
      اكسر;
    حالة TFTP_ERR_DISKFULL:
      نتیجہ = CURLE_REMOTE_DISK_بھریا;
      اكسر;
    حالة TFTP_ERR_UNDEF:
    حالة TFTP_ERR_ILLEGAL:
      نتیجہ = CURLE_TFTP_ILLEGAL;
      اكسر;
    حالة TFTP_ERR_UNKNOWNID:
      نتیجہ = CURLE_TFTP_UNKNOWNID;
      اكسر;
    حالة TFTP_ERR_EXISTS:
      نتیجہ = CURLE_REMOTE_FILE_EXISTS;
      اكسر;
    حالة TFTP_ERR_NOSUCHUSER:
      نتیجہ = CURLE_TFTP_NOSUCHUSER;
      اكسر;
    حالة TFTP_ERR_TIMEOUT:
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    حالة TFTP_ERR_NORESPONSE:
      نتیجہ = CURLE_COULDNT_CONNECT;
      اكسر;
    افتراضي:
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }
  وإلا
    نتیجہ = CURLE_OK;

  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ساكن CURLcode tftp_state_machine(بنية tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية Curl_easy *انکڑے = state->انکڑے;

  بدل(state->state) {
  حالة TFTP_STATE_شروع:
    DEBUGF(infof(انکڑے, "TFTP_STATE_START"));
    نتیجہ = tftp_send_پہلا(state, event);
    اكسر;
  حالة TFTP_STATE_RX:
    DEBUGF(infof(انکڑے, "TFTP_STATE_RX"));
    نتیجہ = tftp_rx(state, event);
    اكسر;
  حالة TFTP_STATE_TX:
    DEBUGF(infof(انکڑے, "TFTP_STATE_TX"));
    نتیجہ = tftp_tx(state, event);
    اكسر;
  حالة TFTP_STATE_FIN:
    infof(انکڑے, "%s", "TFTP finished");
    اكسر;
  افتراضي:
    DEBUGF(infof(انکڑے, "STATE: %d", (صحيح)state->state));
    failf(انکڑے, "%s", "Internal state machine error");
    نتیجہ = CURLE_TFTP_ILLEGAL;
    اكسر;
  }

  أرجع نتیجہ;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية tftp_conn *state = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(state->rpacket.انکڑے);
  curlx_safefree(state->spacket.انکڑے);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ساكن CURLcode tftp_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية tftp_conn *state;
  صحيح blksize;
  صحيح need_blksize;
  بنية connectdata *conn = انکڑے->conn;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتیجہ;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  إذا(!state ||
     Curl_conn_meta_سیٹ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* alloc pkt buffers based on specified blksize */
  إذا(انکڑے->سیٹ.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)انکڑے->سیٹ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  إذا(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  إذا(!state->rpacket.انکڑے) {
    state->rpacket.انکڑے = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->rpacket.انکڑے)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  إذا(!state->spacket.انکڑے) {
    state->spacket.انکڑے = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->spacket.انکڑے)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->انکڑے = انکڑے;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_شروع;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(انکڑے, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  إذا(!remote_addr)
    أرجع CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  نتیجہ = tftp_سیٹ_timeouts(state);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(!conn->بٹ.bound) {
    /* If not already bound, bind to any interface, random UDP port. If it is
     * reused or a custom local port was desired, this has already been done!
     *
     * We once used the size of the local_addr struct as the third argument
     * for bind() to better work with IPv6 or whatever size the struct could
     * have, but we learned that at least Tru64, AIX and IRIX *requires* the
     * size of that argument to match the exact size of a 'sockaddr_in' struct
     * when running IPv4-only.
     *
     * Therefore we use the size from the address we connected to, which we
     * assume uses the same IP version and thus hopefully this works for both
     * IPv4 and IPv6...
     */
    صحيح rc = bind(state->sockfd, (بنية sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    إذا(rc) {
      حرف بفر[STRERROR_LEN];
      failf(انکڑے, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_COULDNT_CONNECT;
    }
    conn->بٹ.bound = TRUE;
  }

  *پورا = TRUE;

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساكن CURLcode tftp_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                          bool premature)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (فراغ)حالت;
  (فراغ)premature;

  إذا(Curl_pgrs_پورا(انکڑے, Curl_pgrs_now(انکڑے)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  إذا(state)
    نتیجہ = tftp_translate_کوڈ(state->error);

  أرجع نتیجہ;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  أرجع Curl_pollset_جوڑو_وچ(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ساكن CURLcode tftp_receive_packet(بنية Curl_easy *انکڑے,
                                    بنية tftp_conn *state)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = حجم(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحيح)recvfrom(state->sockfd,
                                (فراغ *)state->rpacket.انکڑے,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (بنية sockaddr *)&remote_addr,
                                &fromlen);
  إذا((state->rbytes >= 0) && fromlen) {
    إذا(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      إذا((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(انکڑے, "Data received from another address");
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      حافظہ_نقل(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  إذا(state->rbytes < 4) {
    failf(انکڑے, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  وإلا {
    /* The event is given by the TFTP packet time */
    بنانشانی چھوٹا event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_انکڑے:
      إذا(انکڑے->state.upload)
        أرجع CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      إذا(state->rbytes > 4 &&
         (اگلا_BLOCKNUM(state->بلاک) == getrpacketblock(&state->rpacket))) {
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.انکڑے + 4,
                                   state->rbytes - 4);
        إذا(نتیجہ) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          أرجع نتیجہ;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      بنانشانی چھوٹا error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.انکڑے + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      إذا(tftp_strnlen(str, strn) < strn)
        infof(انکڑے, "TFTP error: %s", str);
      اكسر;
    }
    حالة TFTP_EVENT_ACK:
      اكسر;
    حالة TFTP_EVENT_OACK:
      نتیجہ = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.انکڑے + 2,
                                     state->rbytes-2);
      إذا(نتیجہ)
        أرجع نتیجہ;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(انکڑے, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    نتیجہ = Curl_pgrsUpdate(انکڑے);
    إذا(نتیجہ) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      أرجع نتیجہ;
    }
  }
  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ساكن timediff_t tftp_state_timeout(بنية tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  إذا(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->انکڑے);
  إذا(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    أرجع timeout_ms;
  }
  current = ویلا(عدم);
  إذا(current > state->rx_ویلا + state->retry_ویلا) {
    إذا(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_ویلا = ویلا(عدم); /* update even though we received nothing */
  }

  أرجع timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *انکڑے, bool *پورا)
{
  tftp_event_t event;
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *پورا = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  إذا(timeout_ms < 0) {
    failf(انکڑے, "TFTP response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  إذا(event != TFTP_EVENT_NONE) {
    نتیجہ = tftp_state_machine(state, event);
    إذا(نتیجہ)
      أرجع نتیجہ;
    *پورا = (state->state == TFTP_STATE_FIN);
    إذا(*پورا)
      /* Tell curl we are done */
      Curl_xfer_تیاری_nop(انکڑے);
  }
  وإلا {
    /* no timeouts to handle, check our socket */
    صحيح rc = SOCKET_READABLE(state->sockfd, 0);

    إذا(rc == -1) {
      /* bail out */
      صحيح sockerr = SOCKERRNO;
      حرف بفر[STRERROR_LEN];
      failf(انکڑے, "%s", curlx_strerror(sockerr, بفر, حجم(بفر)));
      state->event = TFTP_EVENT_ERROR;
    }
    وإلا إذا(rc) {
      نتیجہ = tftp_receive_packet(انکڑے, state);
      إذا(نتیجہ)
        أرجع نتیجہ;
      نتیجہ = tftp_state_machine(state, state->event);
      إذا(نتیجہ)
        أرجع نتیجہ;
      *پورا = (state->state == TFTP_STATE_FIN);
      إذا(*پورا)
        /* Tell curl we are done */
        Curl_xfer_تیاری_nop(انکڑے);
    }
    /* if rc == 0, then select() timed out */
  }

  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساكن CURLcode tftp_doing(بنية Curl_easy *انکڑے, bool *dophase_پورا)
{
  CURLcode نتیجہ;
  نتیجہ = tftp_multi_statemach(انکڑے, dophase_پورا);

  إذا(*dophase_پورا) {
    DEBUGF(infof(انکڑے, "DO phase is complete"));
  }
  وإلا إذا(!نتیجہ) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    نتیجہ = Curl_pgrsCheck(انکڑے);
  }
  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساكن CURLcode tftp_perform(بنية Curl_easy *انکڑے, bool *dophase_پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_پورا = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = tftp_state_machine(state, TFTP_EVENT_INIT);

  إذا((state->state == TFTP_STATE_FIN) || نتیجہ)
    أرجع نتیجہ;

  نتیجہ = tftp_multi_statemach(انکڑے, dophase_پورا);

  إذا(*dophase_پورا)
    DEBUGF(infof(انکڑے, "DO phase is complete"));

  أرجع نتیجہ;
}

/**********************************************************
 *
 * tftp_do
 *
 * The do callback
 *
 * This callback initiates the TFTP transfer
 *
 **********************************************************/

ساكن CURLcode tftp_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode نتیجہ;

  *پورا = FALSE;

  إذا(!state) {
    نتیجہ = tftp_connect(انکڑے, پورا);
    إذا(نتیجہ)
      أرجع نتیجہ;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    إذا(!state)
      أرجع CURLE_TFTP_ILLEGAL;
  }

  نتیجہ = tftp_perform(انکڑے, پورا);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  إذا(!نتیجہ)
    /* If we have encountered an internal tftp error, translate it. */
    نتیجہ = tftp_translate_کوڈ(state->error);

  أرجع نتیجہ;
}

ساكن CURLcode tftp_تیاری_connection(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn)
{
  حرف *رستہ = انکڑے->state.اتے.رستہ;
  size_t len = لڑی_لمبائی(رستہ);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  إذا((len >= 14) && !memcmp(&رستہ[len - 14], ";mode=netascii", 14)) {
    انکڑے->state.prefer_ascii = TRUE;
    رستہ[len - 14] = 0; /* cut it there */
  }
  وإلا إذا((len >= 11) && !memcmp(&رستہ[len - 11], ";mode=octet", 11)) {
    انکڑے->state.prefer_ascii = FALSE;
    رستہ[len - 11] = 0; /* cut it there */
  }

  أرجع CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_tftp = {
  tftp_تیاری_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_پورا,                            /* done */
  ZERO_NULL,                            /* do_more */
  tftp_connect,                         /* connect_it */
  tftp_multi_statemach,                 /* connecting */
  tftp_doing,                           /* doing */
  tftp_pollset,                         /* proto_pollset */
  tftp_pollset,                         /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#جے_انت
