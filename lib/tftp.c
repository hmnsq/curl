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

#اگر_تعریف_نشده CURL_DISABLE_TFTP

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

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
#تعریف_کن TFTP_BLKSIZE_DEFAULT 512
#تعریف_کن TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#تعریف_کن TFTP_OPTION_TSIZE    "tsize"
#تعریف_کن TFTP_OPTION_INTERVAL "timeout"

تعريفنوع تعداد {
  TFTP_نما_NETASCII = 0,
  TFTP_نما_OCTET
} tftp_نما_t;

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
  TFTP_EVENT_داده = 3,
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
  بیعلامت حرف *داده;
};

/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

بنية tftp_conn {
  بنية Curl_sockaddr_storage local_addr;
  بنية Curl_sockaddr_storage remote_addr;
  بنية tftp_packet rpacket;
  بنية tftp_packet spacket;
  tftp_state_t    state;
  tftp_نما_t     نما;
  tftp_error_t    error;
  tftp_event_t    event;
  بنية Curl_easy *داده;
  curl_socket_t   sockfd;
  صحيح             retries;
  صحيح             retry_زمان;
  صحيح             retry_max;
  time_t          rx_زمان;
  curl_socklen_t  remote_addrlen;
  صحيح             rbytes;
  size_t          sbytes;
  بیعلامت صحيح    blksize;
  بیعلامت صحيح    requested_blksize;
  بیعلامت کوتاه  بلوک;
  بیت(remote_pinned);
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
ساكن CURLcode tftp_مجموعه_timeouts(بنية tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->داده);

  إذا(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->داده, "Connection time-out");
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
  state->retry_زمان = (صحيح)(timeout / state->retry_max);
  إذا(state->retry_زمان < 1)
    state->retry_زمان = 1;

  infof(state->داده,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_زمان, state->retry_max);

  /* init RX time */
  state->rx_زمان = زمان(عدم);

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, بیعلامت کوتاه رقم)
{
  packet->داده[0] = (بیعلامت حرف)(رقم >> 8);
  packet->داده[1] = (بیعلامت حرف)(رقم & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, بیعلامت کوتاه رقم)
{
  packet->داده[2] = (بیعلامت حرف)(رقم >> 8);
  packet->داده[3] = (بیعلامت حرف)(رقم & 0xff);
}

ساكن بیعلامت کوتاه getrpacketevent(ثابت بنية tftp_packet *packet)
{
  أرجع (بیعلامت کوتاه)((packet->داده[0] << 8) | packet->داده[1]);
}

ساكن بیعلامت کوتاه getrpacketblock(ثابت بنية tftp_packet *packet)
{
  أرجع (بیعلامت کوتاه)((packet->داده[2] << 8) | packet->داده[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *انتها = memchr(string, '\0', maxlen);
  أرجع انتها ? (size_t)(انتها - string) : maxlen;
}

ساكن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **مقدار)
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
  *مقدار = &buf[طول_رشته(*option) + 1];

  أرجع &buf[loc];
}

ساكن CURLcode tftp_parse_option_ack(بنية tftp_conn *state,
                                      ثابت حرف *ptr, صحيح len)
{
  ثابت حرف *tmp = ptr;
  بنية Curl_easy *داده = state->داده;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  طالما(tmp < ptr + len) {
    ثابت حرف *option, *مقدار;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &مقدار);
    إذا(!tmp) {
      failf(داده, "Malformed ACK packet, rejecting");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    olen = طول_رشته(option);

    infof(داده, "got option=(%s) value=(%s)", option, مقدار);

    إذا((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      إذا(curlx_str_عدد(&مقدار, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(داده, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      إذا(!blksize) {
        failf(داده, "invalid blocksize value in OACK packet");
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize < TFTP_BLKSIZE_MIN) {
        failf(داده, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(داده, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        أرجع CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (بیعلامت صحيح)blksize;
      infof(داده, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وإلا إذا((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      إذا(!داده->state.upload &&
         !curlx_str_عدد(&مقدار, &tsize, CURL_OFF_T_MAX)) {
        إذا(!tsize) {
          failf(داده, "invalid tsize -:%s:- value in OACK packet", مقدار);
          أرجع CURLE_TFTP_ILLEGAL;
        }
        infof(داده, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_مجموعه_Download_اندازه(داده, tsize);
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode tftp_option_جمع_کن(بنية tftp_conn *state, size_t *csize,
                                size_t شاخص, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.داده[شاخص];
  size_t oplen = طول_رشته(option);
  size_t blen;
  إذا((state->blksize <= شاخص) ||
     (oplen + 1) > (size_t)(state->blksize - شاخص))
    أرجع CURLE_TFTP_ILLEGAL;
  blen = state->blksize - شاخص;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  أرجع CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#تعریف_کن بعدی_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ساكن CURLcode tftp_tx(بنية tftp_conn *state, tftp_event_t event)
{
  بنية Curl_easy *داده = state->داده;
  ssize_t sbytes;
  CURLcode نتیجه = CURLE_OK;
  بنية SingleRequest *k = &داده->req;
  size_t cb; /* Bytes currently read */
  حرف میانگیر[STRERROR_LEN];
  حرف *bufptr;
  bool eos;

  إذا(!داده->state.upload)
    أرجع CURLE_TFTP_ILLEGAL;

  بدل(event) {

  حالة TFTP_EVENT_ACK:
  حالة TFTP_EVENT_OACK:
    إذا(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحيح rblock = getrpacketblock(&state->rpacket);

      إذا(rblock != state->بلوک &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->بلوک == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(داده, "Received ACK for block %d, expecting %d",
              rblock, state->بلوک);
        state->retries++;
        /* Bail out if over the maximum */
        إذا(state->retries > state->retry_max) {
          failf(داده, "tftp_tx: giving up waiting for block %d ack",
                state->بلوک);
          نتیجه = CURLE_SEND_ERROR;
        }
        وإلا {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (بنية sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          إذا(sbytes < 0) {
            failf(داده, "%s",
                  curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
            نتیجه = CURLE_SEND_ERROR;
          }
        }

        أرجع نتیجه;
      }
      /* This is the expected packet. Reset the counters and send the next
بلوک*/
      state->rx_زمان = زمان(عدم);
      state->بلوک++;
    }
    وإلا
      state->بلوک = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_داده);
    setpacketblock(&state->spacket, state->بلوک);
    إذا(state->بلوک > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      أرجع CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (حرف *)state->spacket.داده + 4;
    افعل {
      نتیجه = Curl_client_read(داده, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      إذا(نتیجه)
        أرجع نتیجه;
      state->sbytes += cb;
      bufptr += cb;
    } طالما(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    إذا(sbytes < 0) {
      failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
      أرجع CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(داده, state->sbytes);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(داده, "Timeout waiting for block %d ACK. "
          "Retries = %d", بعدی_BLOCKNUM(state->بلوک), state->retries);
    /* Decide if we have had enough */
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      إذا(sbytes < 0) {
        failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
        أرجع CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_مجموعه_Upload_Counter(داده, k->writebytecount);
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلوک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.داده, 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(داده, "tftp_tx: internal error, event: %d", (صحيح)event);
    اكسر;
  }

  أرجع نتیجه;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجه;

  infof(state->داده, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  نتیجه = tftp_مجموعه_timeouts(state);
  إذا(نتیجه)
    أرجع نتیجه;
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
  بنية Curl_easy *داده = state->داده;
  حرف میانگیر[STRERROR_LEN];

  بدل(event) {

  حالة TFTP_EVENT_داده:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    إذا(بعدی_BLOCKNUM(state->بلوک) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وإلا إذا(state->بلوک == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(داده, "Received last DATA packet block %d again.", rblock);
    }
    وإلا {
      /* totally unexpected, log it */
      infof(داده,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, بعدی_BLOCKNUM(state->بلوک));
      اكسر;
    }

    /* ACK this block. */
    state->بلوک = (بیعلامت کوتاه)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلوک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    إذا(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      state->state = TFTP_STATE_RX;
    }
    state->rx_زمان = زمان(عدم);
    اكسر;

  حالة TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->بلوک = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلوک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_زمان = زمان(عدم);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(داده,
          "Timeout waiting for block %d ACK. Retries = %d",
          بعدی_BLOCKNUM(state->بلوک), state->retries);
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                      4, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      إذا(sbytes < 0) {
        failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
        أرجع CURLE_SEND_ERROR;
      }
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلوک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.داده,
                 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(داده, "%s", "tftp_rx: internal error");
    أرجع CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  أرجع CURLE_OK;
}

ساكن CURLcode tftp_connect_for_rx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجه;

  infof(state->داده, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  نتیجه = tftp_مجموعه_timeouts(state);
  إذا(نتیجه)
    أرجع نتیجه;
  أرجع tftp_rx(state, event);
}

ساكن CURLcode tftp_send_اولی(بنية tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *نما = "octet";
  حرف *filename;
  بنية Curl_easy *داده = state->داده;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتیجه = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  إذا(داده->state.prefer_ascii)
    نما = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      أرجع نتیجه;
    }

    إذا(داده->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      إذا(داده->state.infilesize != -1)
        Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);
    }
    وإلا {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    إذا(!state->داده->state.به_بالا.مسیر[1]) {
      failf(داده, "Missing filename");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    نتیجه = Curl_urldecode(&state->داده->state.به_بالا.مسیر[1], 0,
                            &filename, عدم, REJECT_ZERO);
    إذا(نتیجه)
      أرجع نتیجه;

    إذا(طول_رشته(filename) + طول_رشته(نما) + 4 > state->blksize) {
      failf(داده, "TFTP filename too long");
      curlx_free(filename);
      أرجع CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.داده + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', نما, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    إذا(!داده->مجموعه.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     داده->state.upload && (داده->state.infilesize != -1) ?
                     داده->state.infilesize : 0);

      نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      إذا(نتیجه == CURLE_OK)
        نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      إذا(نتیجه == CURLE_OK)
        نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      إذا(نتیجه == CURLE_OK)
        نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_زمان);
      إذا(نتیجه == CURLE_OK)
        نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      إذا(نتیجه == CURLE_OK)
        نتیجه = tftp_option_جمع_کن(state, &sbytes, sbytes, buf);

      إذا(نتیجه != CURLE_OK) {
        failf(داده, "TFTP buffer too small for options");
        أرجع CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#اگر_تعریف_شده __AMIGA__
#تعریف_کن CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وإلا
#تعریف_کن CURL_SENDTO_ARG5(x) (x)
#پایان_اگر
    remote_addr = Curl_conn_get_remote_addr(داده, FIRSTSOCKET);
    إذا(!remote_addr)
      أرجع CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (فراغ *)state->spacket.داده,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    إذا(senddata != (ssize_t)sbytes) {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "%s", curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
      أرجع CURLE_SEND_ERROR;
    }
    اكسر;

  حالة TFTP_EVENT_OACK:
    إذا(داده->state.upload) {
      نتیجه = tftp_connect_for_tx(state, event);
    }
    وإلا {
      نتیجه = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    إذا(!داده->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتیجه = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_داده: /* Connected for receive */
    إذا(داده->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتیجه = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->داده, "tftp_send_first: internal error");
    أرجع CURLE_TFTP_ILLEGAL;
  }

  أرجع نتیجه;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساكن CURLcode tftp_translate_کد(tftp_error_t error)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(error != TFTP_ERR_NONE) {
    بدل(error) {
    حالة TFTP_ERR_NOTFOUND:
      نتیجه = CURLE_TFTP_NOTFOUND;
      اكسر;
    حالة TFTP_ERR_PERM:
      نتیجه = CURLE_TFTP_PERM;
      اكسر;
    حالة TFTP_ERR_DISKFULL:
      نتیجه = CURLE_REMOTE_DISK_پر;
      اكسر;
    حالة TFTP_ERR_UNDEF:
    حالة TFTP_ERR_ILLEGAL:
      نتیجه = CURLE_TFTP_ILLEGAL;
      اكسر;
    حالة TFTP_ERR_UNKNOWNID:
      نتیجه = CURLE_TFTP_UNKNOWNID;
      اكسر;
    حالة TFTP_ERR_EXISTS:
      نتیجه = CURLE_REMOTE_FILE_EXISTS;
      اكسر;
    حالة TFTP_ERR_NOSUCHUSER:
      نتیجه = CURLE_TFTP_NOSUCHUSER;
      اكسر;
    حالة TFTP_ERR_TIMEOUT:
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    حالة TFTP_ERR_NORESPONSE:
      نتیجه = CURLE_COULDNT_CONNECT;
      اكسر;
    افتراضي:
      نتیجه = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }
  وإلا
    نتیجه = CURLE_OK;

  أرجع نتیجه;
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
  CURLcode نتیجه = CURLE_OK;
  بنية Curl_easy *داده = state->داده;

  بدل(state->state) {
  حالة TFTP_STATE_شروع:
    DEBUGF(infof(داده, "TFTP_STATE_START"));
    نتیجه = tftp_send_اولی(state, event);
    اكسر;
  حالة TFTP_STATE_RX:
    DEBUGF(infof(داده, "TFTP_STATE_RX"));
    نتیجه = tftp_rx(state, event);
    اكسر;
  حالة TFTP_STATE_TX:
    DEBUGF(infof(داده, "TFTP_STATE_TX"));
    نتیجه = tftp_tx(state, event);
    اكسر;
  حالة TFTP_STATE_FIN:
    infof(داده, "%s", "TFTP finished");
    اكسر;
  افتراضي:
    DEBUGF(infof(داده, "STATE: %d", (صحيح)state->state));
    failf(داده, "%s", "Internal state machine error");
    نتیجه = CURLE_TFTP_ILLEGAL;
    اكسر;
  }

  أرجع نتیجه;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية tftp_conn *state = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(state->rpacket.داده);
  curlx_safefree(state->spacket.داده);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ساكن CURLcode tftp_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية tftp_conn *state;
  صحيح blksize;
  صحيح need_blksize;
  بنية connectdata *conn = داده->conn;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتیجه;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  إذا(!state ||
     Curl_conn_meta_مجموعه(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  /* alloc pkt buffers based on specified blksize */
  إذا(داده->مجموعه.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)داده->مجموعه.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  إذا(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  إذا(!state->rpacket.داده) {
    state->rpacket.داده = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->rpacket.داده)
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(!state->spacket.داده) {
    state->spacket.داده = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->spacket.داده)
      أرجع CURLE_بیرون_از_حافظه;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->داده = داده;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_شروع;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(داده, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  إذا(!remote_addr)
    أرجع CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  نتیجه = tftp_مجموعه_timeouts(state);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!conn->بیتها.bound) {
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
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
      أرجع CURLE_COULDNT_CONNECT;
    }
    conn->بیتها.bound = TRUE;
  }

  *انجامشده = TRUE;

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساكن CURLcode tftp_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                          bool premature)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (فراغ)وضعیت;
  (فراغ)premature;

  إذا(Curl_pgrs_انجامشده(داده, Curl_pgrs_now(داده)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  إذا(state)
    نتیجه = tftp_translate_کد(state->error);

  أرجع نتیجه;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  أرجع Curl_pollset_جمع_کن_درون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ساكن CURLcode tftp_receive_packet(بنية Curl_easy *داده,
                                    بنية tftp_conn *state)
{
  CURLcode نتیجه = CURLE_OK;
  بنية Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = حجم(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحيح)recvfrom(state->sockfd,
                                (فراغ *)state->rpacket.داده,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (بنية sockaddr *)&remote_addr,
                                &fromlen);
  إذا((state->rbytes >= 0) && fromlen) {
    إذا(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      إذا((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(داده, "Data received from another address");
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      کپی_حافظه(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  إذا(state->rbytes < 4) {
    failf(داده, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  وإلا {
    /* The event is given by the TFTP packet time */
    بیعلامت کوتاه event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_داده:
      إذا(داده->state.upload)
        أرجع CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      إذا(state->rbytes > 4 &&
         (بعدی_BLOCKNUM(state->بلوک) == getrpacketblock(&state->rpacket))) {
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.داده + 4,
                                   state->rbytes - 4);
        إذا(نتیجه) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          أرجع نتیجه;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      بیعلامت کوتاه error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.داده + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      إذا(tftp_strnlen(str, strn) < strn)
        infof(داده, "TFTP error: %s", str);
      اكسر;
    }
    حالة TFTP_EVENT_ACK:
      اكسر;
    حالة TFTP_EVENT_OACK:
      نتیجه = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.داده + 2,
                                     state->rbytes-2);
      إذا(نتیجه)
        أرجع نتیجه;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(داده, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    نتیجه = Curl_pgrsUpdate(داده);
    إذا(نتیجه) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      أرجع نتیجه;
    }
  }
  أرجع نتیجه;
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

  timeout_ms = Curl_timeleft_ms(state->داده);
  إذا(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    أرجع timeout_ms;
  }
  current = زمان(عدم);
  إذا(current > state->rx_زمان + state->retry_زمان) {
    إذا(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_زمان = زمان(عدم); /* update even though we received nothing */
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
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *داده, bool *انجامشده)
{
  tftp_event_t event;
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *انجامشده = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  إذا(timeout_ms < 0) {
    failf(داده, "TFTP response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  إذا(event != TFTP_EVENT_NONE) {
    نتیجه = tftp_state_machine(state, event);
    إذا(نتیجه)
      أرجع نتیجه;
    *انجامشده = (state->state == TFTP_STATE_FIN);
    إذا(*انجامشده)
      /* Tell curl we are done */
      Curl_xfer_راهاندازی_nop(داده);
  }
  وإلا {
    /* no timeouts to handle, check our socket */
    صحيح rc = SOCKET_READABLE(state->sockfd, 0);

    إذا(rc == -1) {
      /* bail out */
      صحيح sockerr = SOCKERRNO;
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "%s", curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
      state->event = TFTP_EVENT_ERROR;
    }
    وإلا إذا(rc) {
      نتیجه = tftp_receive_packet(داده, state);
      إذا(نتیجه)
        أرجع نتیجه;
      نتیجه = tftp_state_machine(state, state->event);
      إذا(نتیجه)
        أرجع نتیجه;
      *انجامشده = (state->state == TFTP_STATE_FIN);
      إذا(*انجامشده)
        /* Tell curl we are done */
        Curl_xfer_راهاندازی_nop(داده);
    }
    /* if rc == 0, then select() timed out */
  }

  أرجع نتیجه;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساكن CURLcode tftp_doing(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  CURLcode نتیجه;
  نتیجه = tftp_multi_statemach(داده, dophase_انجامشده);

  إذا(*dophase_انجامشده) {
    DEBUGF(infof(داده, "DO phase is complete"));
  }
  وإلا إذا(!نتیجه) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    نتیجه = Curl_pgrsCheck(داده);
  }
  أرجع نتیجه;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساكن CURLcode tftp_perform(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_انجامشده = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  نتیجه = tftp_state_machine(state, TFTP_EVENT_INIT);

  إذا((state->state == TFTP_STATE_FIN) || نتیجه)
    أرجع نتیجه;

  نتیجه = tftp_multi_statemach(داده, dophase_انجامشده);

  إذا(*dophase_انجامشده)
    DEBUGF(infof(داده, "DO phase is complete"));

  أرجع نتیجه;
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

ساكن CURLcode tftp_do(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية connectdata *conn = داده->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode نتیجه;

  *انجامشده = FALSE;

  إذا(!state) {
    نتیجه = tftp_connect(داده, انجامشده);
    إذا(نتیجه)
      أرجع نتیجه;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    إذا(!state)
      أرجع CURLE_TFTP_ILLEGAL;
  }

  نتیجه = tftp_perform(داده, انجامشده);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  إذا(!نتیجه)
    /* If we have encountered an internal tftp error, translate it. */
    نتیجه = tftp_translate_کد(state->error);

  أرجع نتیجه;
}

ساكن CURLcode tftp_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  حرف *مسیر = داده->state.به_بالا.مسیر;
  size_t len = طول_رشته(مسیر);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  إذا((len >= 14) && !memcmp(&مسیر[len - 14], ";mode=netascii", 14)) {
    داده->state.prefer_ascii = TRUE;
    مسیر[len - 14] = 0; /* cut it there */
  }
  وإلا إذا((len >= 11) && !memcmp(&مسیر[len - 11], ";mode=octet", 11)) {
    داده->state.prefer_ascii = FALSE;
    مسیر[len - 11] = 0; /* cut it there */
  }

  أرجع CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_tftp = {
  tftp_راهاندازی_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_انجامشده,                            /* done */
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

#پایان_اگر
