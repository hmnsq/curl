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

#که_نه_تعریف_شوی CURL_DISABLE_TFTP

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#د_که_پای

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
#تعریف_کړه TFTP_BLKSIZE_DEFAULT 512
#تعریف_کړه TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#تعریف_کړه TFTP_OPTION_TSIZE    "tsize"
#تعریف_کړه TFTP_OPTION_INTERVAL "timeout"

تعريفنوع تعداد {
  TFTP_ډېرپېښ_NETASCII = 0,
  TFTP_ډېرپېښ_OCTET
} tftp_ډېرپېښ_t;

تعريفنوع تعداد {
  TFTP_STATE_پیل = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

تعريفنوع تعداد {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_معلومات = 3,
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
  بینښه حرف *معلومات;
};

/* meta key for storing protocol meta at connection */
#تعریف_کړه CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

بنية tftp_conn {
  بنية Curl_sockaddr_storage local_addr;
  بنية Curl_sockaddr_storage remote_addr;
  بنية tftp_packet rpacket;
  بنية tftp_packet spacket;
  tftp_state_t    state;
  tftp_ډېرپېښ_t     ډېرپېښ;
  tftp_error_t    error;
  tftp_event_t    event;
  بنية Curl_easy *معلومات;
  curl_socket_t   sockfd;
  صحيح             retries;
  صحيح             retry_وخت;
  صحيح             retry_max;
  time_t          rx_وخت;
  curl_socklen_t  remote_addrlen;
  صحيح             rbytes;
  size_t          sbytes;
  بینښه صحيح    blksize;
  بینښه صحيح    requested_blksize;
  بینښه لنډ  بلاک;
  بټ(remote_pinned);
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
ساكن CURLcode tftp_سېټ_timeouts(بنية tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->معلومات);

  إذا(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->معلومات, "Connection time-out");
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
  state->retry_وخت = (صحيح)(timeout / state->retry_max);
  إذا(state->retry_وخت < 1)
    state->retry_وخت = 1;

  infof(state->معلومات,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_وخت, state->retry_max);

  /* init RX time */
  state->rx_وخت = وخت(عدم);

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, بینښه لنډ عدد)
{
  packet->معلومات[0] = (بینښه حرف)(عدد >> 8);
  packet->معلومات[1] = (بینښه حرف)(عدد & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, بینښه لنډ عدد)
{
  packet->معلومات[2] = (بینښه حرف)(عدد >> 8);
  packet->معلومات[3] = (بینښه حرف)(عدد & 0xff);
}

ساكن بینښه لنډ getrpacketevent(ثابت بنية tftp_packet *packet)
{
  أرجع (بینښه لنډ)((packet->معلومات[0] << 8) | packet->معلومات[1]);
}

ساكن بینښه لنډ getrpacketblock(ثابت بنية tftp_packet *packet)
{
  أرجع (بینښه لنډ)((packet->معلومات[2] << 8) | packet->معلومات[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *پای_ټکی = memchr(string, '\0', maxlen);
  أرجع پای_ټکی ? (size_t)(پای_ټکی - string) : maxlen;
}

ساكن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **ارزښت)
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
  *ارزښت = &buf[مزي_اوږدوالی(*option) + 1];

  أرجع &buf[loc];
}

ساكن CURLcode tftp_parse_option_ack(بنية tftp_conn *state,
                                      ثابت حرف *ptr, صحيح len)
{
  ثابت حرف *tmp = ptr;
  بنية Curl_easy *معلومات = state->معلومات;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  طالما(tmp < ptr + len) {
    ثابت حرف *option, *ارزښت;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &ارزښت);
    إذا(!tmp) {
      failf(معلومات, "Malformed ACK packet, rejecting");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    olen = مزي_اوږدوالی(option);

    infof(معلومات, "got option=(%s) value=(%s)", option, ارزښت);

    إذا((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      إذا(curlx_str_شمېره(&ارزښت, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(معلومات, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      إذا(!blksize) {
        failf(معلومات, "invalid blocksize value in OACK packet");
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize < TFTP_BLKSIZE_MIN) {
        failf(معلومات, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(معلومات, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        أرجع CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (بینښه صحيح)blksize;
      infof(معلومات, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وإلا إذا((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      إذا(!معلومات->state.upload &&
         !curlx_str_شمېره(&ارزښت, &tsize, CURL_OFF_T_MAX)) {
        إذا(!tsize) {
          failf(معلومات, "invalid tsize -:%s:- value in OACK packet", ارزښت);
          أرجع CURLE_TFTP_ILLEGAL;
        }
        infof(معلومات, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_سېټ_Download_اندازه(معلومات, tsize);
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode tftp_option_جمع_کړه(بنية tftp_conn *state, size_t *csize,
                                size_t شاخص, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.معلومات[شاخص];
  size_t oplen = مزي_اوږدوالی(option);
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
#تعریف_کړه بل_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ساكن CURLcode tftp_tx(بنية tftp_conn *state, tftp_event_t event)
{
  بنية Curl_easy *معلومات = state->معلومات;
  ssize_t sbytes;
  CURLcode پایله = CURLE_OK;
  بنية SingleRequest *k = &معلومات->req;
  size_t cb; /* Bytes currently read */
  حرف بفر[STRERROR_LEN];
  حرف *bufptr;
  bool eos;

  إذا(!معلومات->state.upload)
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
        infof(معلومات, "Received ACK for block %d, expecting %d",
              rblock, state->بلاک);
        state->retries++;
        /* Bail out if over the maximum */
        إذا(state->retries > state->retry_max) {
          failf(معلومات, "tftp_tx: giving up waiting for block %d ack",
                state->بلاک);
          پایله = CURLE_SEND_ERROR;
        }
        وإلا {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (بنية sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          إذا(sbytes < 0) {
            failf(معلومات, "%s",
                  curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
            پایله = CURLE_SEND_ERROR;
          }
        }

        أرجع پایله;
      }
      /* This is the expected packet. Reset the counters and send the next
بلاک*/
      state->rx_وخت = وخت(عدم);
      state->بلاک++;
    }
    وإلا
      state->بلاک = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_معلومات);
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
    bufptr = (حرف *)state->spacket.معلومات + 4;
    افعل {
      پایله = Curl_client_read(معلومات, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      إذا(پایله)
        أرجع پایله;
      state->sbytes += cb;
      bufptr += cb;
    } طالما(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    إذا(sbytes < 0) {
      failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(معلومات, state->sbytes);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(معلومات, "Timeout waiting for block %d ACK. "
          "Retries = %d", بل_BLOCKNUM(state->بلاک), state->retries);
    /* Decide if we have had enough */
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      إذا(sbytes < 0) {
        failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        أرجع CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_سېټ_Upload_Counter(معلومات, k->writebytecount);
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.معلومات, 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(معلومات, "tftp_tx: internal error, event: %d", (صحيح)event);
    اكسر;
  }

  أرجع پایله;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode پایله;

  infof(state->معلومات, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  پایله = tftp_سېټ_timeouts(state);
  إذا(پایله)
    أرجع پایله;
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
  بنية Curl_easy *معلومات = state->معلومات;
  حرف بفر[STRERROR_LEN];

  بدل(event) {

  حالة TFTP_EVENT_معلومات:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    إذا(بل_BLOCKNUM(state->بلاک) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وإلا إذا(state->بلاک == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(معلومات, "Received last DATA packet block %d again.", rblock);
    }
    وإلا {
      /* totally unexpected, log it */
      infof(معلومات,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, بل_BLOCKNUM(state->بلاک));
      اكسر;
    }

    /* ACK this block. */
    state->بلاک = (بینښه لنډ)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    إذا(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      state->state = TFTP_STATE_RX;
    }
    state->rx_وخت = وخت(عدم);
    اكسر;

  حالة TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->بلاک = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    إذا(sbytes < 0) {
      failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_وخت = وخت(عدم);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(معلومات,
          "Timeout waiting for block %d ACK. Retries = %d",
          بل_BLOCKNUM(state->بلاک), state->retries);
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                      4, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      إذا(sbytes < 0) {
        failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        أرجع CURLE_SEND_ERROR;
      }
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(معلومات, "%s", "tftp_rx: internal error");
    أرجع CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  أرجع CURLE_OK;
}

ساكن CURLcode tftp_connect_for_rx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode پایله;

  infof(state->معلومات, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  پایله = tftp_سېټ_timeouts(state);
  إذا(پایله)
    أرجع پایله;
  أرجع tftp_rx(state, event);
}

ساكن CURLcode tftp_send_لومړی(بنية tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *ډېرپېښ = "octet";
  حرف *filename;
  بنية Curl_easy *معلومات = state->معلومات;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode پایله = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  إذا(معلومات->state.prefer_ascii)
    ډېرپېښ = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      أرجع پایله;
    }

    إذا(معلومات->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      إذا(معلومات->state.infilesize != -1)
        Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);
    }
    وإلا {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    إذا(!state->معلومات->state.پورته.لار[1]) {
      failf(معلومات, "Missing filename");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    پایله = Curl_urldecode(&state->معلومات->state.پورته.لار[1], 0,
                            &filename, عدم, REJECT_ZERO);
    إذا(پایله)
      أرجع پایله;

    إذا(مزي_اوږدوالی(filename) + مزي_اوږدوالی(ډېرپېښ) + 4 > state->blksize) {
      failf(معلومات, "TFTP filename too long");
      curlx_free(filename);
      أرجع CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.معلومات + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', ډېرپېښ, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    إذا(!معلومات->سېټ.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     معلومات->state.upload && (معلومات->state.infilesize != -1) ?
                     معلومات->state.infilesize : 0);

      پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      إذا(پایله == CURLE_OK)
        پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      إذا(پایله == CURLE_OK)
        پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      إذا(پایله == CURLE_OK)
        پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_وخت);
      إذا(پایله == CURLE_OK)
        پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      إذا(پایله == CURLE_OK)
        پایله = tftp_option_جمع_کړه(state, &sbytes, sbytes, buf);

      إذا(پایله != CURLE_OK) {
        failf(معلومات, "TFTP buffer too small for options");
        أرجع CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#که_تعریف_شوی __AMIGA__
#تعریف_کړه CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وإلا
#تعریف_کړه CURL_SENDTO_ARG5(x) (x)
#د_که_پای
    remote_addr = Curl_conn_get_remote_addr(معلومات, FIRSTSOCKET);
    إذا(!remote_addr)
      أرجع CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (فراغ *)state->spacket.معلومات,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    إذا(senddata != (ssize_t)sbytes) {
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    اكسر;

  حالة TFTP_EVENT_OACK:
    إذا(معلومات->state.upload) {
      پایله = tftp_connect_for_tx(state, event);
    }
    وإلا {
      پایله = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    إذا(!معلومات->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    پایله = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_معلومات: /* Connected for receive */
    إذا(معلومات->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    پایله = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->معلومات, "tftp_send_first: internal error");
    أرجع CURLE_TFTP_ILLEGAL;
  }

  أرجع پایله;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساكن CURLcode tftp_translate_کوډ(tftp_error_t error)
{
  CURLcode پایله = CURLE_OK;

  إذا(error != TFTP_ERR_NONE) {
    بدل(error) {
    حالة TFTP_ERR_NOTFOUND:
      پایله = CURLE_TFTP_NOTFOUND;
      اكسر;
    حالة TFTP_ERR_PERM:
      پایله = CURLE_TFTP_PERM;
      اكسر;
    حالة TFTP_ERR_DISKFULL:
      پایله = CURLE_REMOTE_DISK_ډک;
      اكسر;
    حالة TFTP_ERR_UNDEF:
    حالة TFTP_ERR_ILLEGAL:
      پایله = CURLE_TFTP_ILLEGAL;
      اكسر;
    حالة TFTP_ERR_UNKNOWNID:
      پایله = CURLE_TFTP_UNKNOWNID;
      اكسر;
    حالة TFTP_ERR_EXISTS:
      پایله = CURLE_REMOTE_FILE_EXISTS;
      اكسر;
    حالة TFTP_ERR_NOSUCHUSER:
      پایله = CURLE_TFTP_NOSUCHUSER;
      اكسر;
    حالة TFTP_ERR_TIMEOUT:
      پایله = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    حالة TFTP_ERR_NORESPONSE:
      پایله = CURLE_COULDNT_CONNECT;
      اكسر;
    افتراضي:
      پایله = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }
  وإلا
    پایله = CURLE_OK;

  أرجع پایله;
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
  CURLcode پایله = CURLE_OK;
  بنية Curl_easy *معلومات = state->معلومات;

  بدل(state->state) {
  حالة TFTP_STATE_پیل:
    DEBUGF(infof(معلومات, "TFTP_STATE_START"));
    پایله = tftp_send_لومړی(state, event);
    اكسر;
  حالة TFTP_STATE_RX:
    DEBUGF(infof(معلومات, "TFTP_STATE_RX"));
    پایله = tftp_rx(state, event);
    اكسر;
  حالة TFTP_STATE_TX:
    DEBUGF(infof(معلومات, "TFTP_STATE_TX"));
    پایله = tftp_tx(state, event);
    اكسر;
  حالة TFTP_STATE_FIN:
    infof(معلومات, "%s", "TFTP finished");
    اكسر;
  افتراضي:
    DEBUGF(infof(معلومات, "STATE: %d", (صحيح)state->state));
    failf(معلومات, "%s", "Internal state machine error");
    پایله = CURLE_TFTP_ILLEGAL;
    اكسر;
  }

  أرجع پایله;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية tftp_conn *state = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  curlx_safefree(state->rpacket.معلومات);
  curlx_safefree(state->spacket.معلومات);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ساكن CURLcode tftp_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية tftp_conn *state;
  صحيح blksize;
  صحيح need_blksize;
  بنية connectdata *conn = معلومات->conn;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode پایله;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  إذا(!state ||
     Curl_conn_meta_سېټ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    أرجع CURLE_بهر_د_حافظه;

  /* alloc pkt buffers based on specified blksize */
  إذا(معلومات->سېټ.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)معلومات->سېټ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  إذا(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  إذا(!state->rpacket.معلومات) {
    state->rpacket.معلومات = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->rpacket.معلومات)
      أرجع CURLE_بهر_د_حافظه;
  }

  إذا(!state->spacket.معلومات) {
    state->spacket.معلومات = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->spacket.معلومات)
      أرجع CURLE_بهر_د_حافظه;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->معلومات = معلومات;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_پیل;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(معلومات, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  إذا(!remote_addr)
    أرجع CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  پایله = tftp_سېټ_timeouts(state);
  إذا(پایله)
    أرجع پایله;

  إذا(!conn->بټونه.bound) {
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
      failf(معلومات, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      أرجع CURLE_COULDNT_CONNECT;
    }
    conn->بټونه.bound = TRUE;
  }

  *بشپړ = TRUE;

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساكن CURLcode tftp_بشپړ(بنية Curl_easy *معلومات, CURLcode حالت,
                          bool premature)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (فراغ)حالت;
  (فراغ)premature;

  إذا(Curl_pgrs_بشپړ(معلومات, Curl_pgrs_now(معلومات)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  إذا(state)
    پایله = tftp_translate_کوډ(state->error);

  أرجع پایله;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  أرجع Curl_pollset_جمع_کړه_دننه(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ساكن CURLcode tftp_receive_packet(بنية Curl_easy *معلومات,
                                    بنية tftp_conn *state)
{
  CURLcode پایله = CURLE_OK;
  بنية Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = حجم(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحيح)recvfrom(state->sockfd,
                                (فراغ *)state->rpacket.معلومات,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (بنية sockaddr *)&remote_addr,
                                &fromlen);
  إذا((state->rbytes >= 0) && fromlen) {
    إذا(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      إذا((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(معلومات, "Data received from another address");
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      حافظه_نقل(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  إذا(state->rbytes < 4) {
    failf(معلومات, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  وإلا {
    /* The event is given by the TFTP packet time */
    بینښه لنډ event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_معلومات:
      إذا(معلومات->state.upload)
        أرجع CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      إذا(state->rbytes > 4 &&
         (بل_BLOCKNUM(state->بلاک) == getrpacketblock(&state->rpacket))) {
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.معلومات + 4,
                                   state->rbytes - 4);
        إذا(پایله) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          أرجع پایله;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      بینښه لنډ error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.معلومات + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      إذا(tftp_strnlen(str, strn) < strn)
        infof(معلومات, "TFTP error: %s", str);
      اكسر;
    }
    حالة TFTP_EVENT_ACK:
      اكسر;
    حالة TFTP_EVENT_OACK:
      پایله = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.معلومات + 2,
                                     state->rbytes-2);
      إذا(پایله)
        أرجع پایله;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(معلومات, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    پایله = Curl_pgrsUpdate(معلومات);
    إذا(پایله) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      أرجع پایله;
    }
  }
  أرجع پایله;
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

  timeout_ms = Curl_timeleft_ms(state->معلومات);
  إذا(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    أرجع timeout_ms;
  }
  current = وخت(عدم);
  إذا(current > state->rx_وخت + state->retry_وخت) {
    إذا(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_وخت = وخت(عدم); /* update even though we received nothing */
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
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *معلومات, bool *بشپړ)
{
  tftp_event_t event;
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *بشپړ = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  إذا(timeout_ms < 0) {
    failf(معلومات, "TFTP response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  إذا(event != TFTP_EVENT_NONE) {
    پایله = tftp_state_machine(state, event);
    إذا(پایله)
      أرجع پایله;
    *بشپړ = (state->state == TFTP_STATE_FIN);
    إذا(*بشپړ)
      /* Tell curl we are done */
      Curl_xfer_تیاری_nop(معلومات);
  }
  وإلا {
    /* no timeouts to handle, check our socket */
    صحيح rc = SOCKET_READABLE(state->sockfd, 0);

    إذا(rc == -1) {
      /* bail out */
      صحيح sockerr = SOCKERRNO;
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "%s", curlx_strerror(sockerr, بفر, حجم(بفر)));
      state->event = TFTP_EVENT_ERROR;
    }
    وإلا إذا(rc) {
      پایله = tftp_receive_packet(معلومات, state);
      إذا(پایله)
        أرجع پایله;
      پایله = tftp_state_machine(state, state->event);
      إذا(پایله)
        أرجع پایله;
      *بشپړ = (state->state == TFTP_STATE_FIN);
      إذا(*بشپړ)
        /* Tell curl we are done */
        Curl_xfer_تیاری_nop(معلومات);
    }
    /* if rc == 0, then select() timed out */
  }

  أرجع پایله;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساكن CURLcode tftp_doing(بنية Curl_easy *معلومات, bool *dophase_بشپړ)
{
  CURLcode پایله;
  پایله = tftp_multi_statemach(معلومات, dophase_بشپړ);

  إذا(*dophase_بشپړ) {
    DEBUGF(infof(معلومات, "DO phase is complete"));
  }
  وإلا إذا(!پایله) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    پایله = Curl_pgrsCheck(معلومات);
  }
  أرجع پایله;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساكن CURLcode tftp_perform(بنية Curl_easy *معلومات, bool *dophase_بشپړ)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_بشپړ = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  پایله = tftp_state_machine(state, TFTP_EVENT_INIT);

  إذا((state->state == TFTP_STATE_FIN) || پایله)
    أرجع پایله;

  پایله = tftp_multi_statemach(معلومات, dophase_بشپړ);

  إذا(*dophase_بشپړ)
    DEBUGF(infof(معلومات, "DO phase is complete"));

  أرجع پایله;
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

ساكن CURLcode tftp_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode پایله;

  *بشپړ = FALSE;

  إذا(!state) {
    پایله = tftp_connect(معلومات, بشپړ);
    إذا(پایله)
      أرجع پایله;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    إذا(!state)
      أرجع CURLE_TFTP_ILLEGAL;
  }

  پایله = tftp_perform(معلومات, بشپړ);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  إذا(!پایله)
    /* If we have encountered an internal tftp error, translate it. */
    پایله = tftp_translate_کوډ(state->error);

  أرجع پایله;
}

ساكن CURLcode tftp_تیاری_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  حرف *لار = معلومات->state.پورته.لار;
  size_t len = مزي_اوږدوالی(لار);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  إذا((len >= 14) && !memcmp(&لار[len - 14], ";mode=netascii", 14)) {
    معلومات->state.prefer_ascii = TRUE;
    لار[len - 14] = 0; /* cut it there */
  }
  وإلا إذا((len >= 11) && !memcmp(&لار[len - 11], ";mode=octet", 11)) {
    معلومات->state.prefer_ascii = FALSE;
    لار[len - 11] = 0; /* cut it there */
  }

  أرجع CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_tftp = {
  tftp_تیاری_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_بشپړ,                            /* done */
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

#د_که_پای
