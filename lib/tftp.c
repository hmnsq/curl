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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TFTP

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

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
#وصف_ڪريو TFTP_BLKSIZE_DEFAULT 512
#وصف_ڪريو TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#وصف_ڪريو TFTP_OPTION_TSIZE    "tsize"
#وصف_ڪريو TFTP_OPTION_INTERVAL "timeout"

تعريفنوع تعداد {
  TFTP_گهڻائي_NETASCII = 0,
  TFTP_گهڻائي_OCTET
} tftp_گهڻائي_t;

تعريفنوع تعداد {
  TFTP_STATE_شروعات = 0,
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
  بنانشان حرف *معلومات;
};

/* meta key for storing protocol meta at connection */
#وصف_ڪريو CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

بنية tftp_conn {
  بنية Curl_sockaddr_storage local_addr;
  بنية Curl_sockaddr_storage remote_addr;
  بنية tftp_packet rpacket;
  بنية tftp_packet spacket;
  tftp_state_t    state;
  tftp_گهڻائي_t     گهڻائي;
  tftp_error_t    error;
  tftp_event_t    event;
  بنية Curl_easy *معلومات;
  curl_socket_t   sockfd;
  صحيح             retries;
  صحيح             retry_وقت;
  صحيح             retry_max;
  time_t          rx_وقت;
  curl_socklen_t  remote_addrlen;
  صحيح             rbytes;
  size_t          sbytes;
  بنانشان صحيح    blksize;
  بنانشان صحيح    requested_blksize;
  بنانشان ننڍو  بلاڪ;
  ٻٽو_انگ(remote_pinned);
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
ساكن CURLcode tftp_سيٽ_timeouts(بنية tftp_conn *state)
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
  state->retry_وقت = (صحيح)(timeout / state->retry_max);
  إذا(state->retry_وقت < 1)
    state->retry_وقت = 1;

  infof(state->معلومات,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_وقت, state->retry_max);

  /* init RX time */
  state->rx_وقت = وقت(عدم);

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, بنانشان ننڍو انگ)
{
  packet->معلومات[0] = (بنانشان حرف)(انگ >> 8);
  packet->معلومات[1] = (بنانشان حرف)(انگ & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, بنانشان ننڍو انگ)
{
  packet->معلومات[2] = (بنانشان حرف)(انگ >> 8);
  packet->معلومات[3] = (بنانشان حرف)(انگ & 0xff);
}

ساكن بنانشان ننڍو getrpacketevent(ثابت بنية tftp_packet *packet)
{
  أرجع (بنانشان ننڍو)((packet->معلومات[0] << 8) | packet->معلومات[1]);
}

ساكن بنانشان ننڍو getrpacketblock(ثابت بنية tftp_packet *packet)
{
  أرجع (بنانشان ننڍو)((packet->معلومات[2] << 8) | packet->معلومات[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *پڇاڙي = memchr(string, '\0', maxlen);
  أرجع پڇاڙي ? (size_t)(پڇاڙي - string) : maxlen;
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
  *قدر = &buf[لڙي_ڊيگهه(*option) + 1];

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
    ثابت حرف *option, *قدر;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &قدر);
    إذا(!tmp) {
      failf(معلومات, "Malformed ACK packet, rejecting");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    olen = لڙي_ڊيگهه(option);

    infof(معلومات, "got option=(%s) value=(%s)", option, قدر);

    إذا((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      إذا(curlx_str_عدد(&قدر, &blksize, TFTP_BLKSIZE_MAX)) {
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

      state->blksize = (بنانشان صحيح)blksize;
      infof(معلومات, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وإلا إذا((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      إذا(!معلومات->state.upload &&
         !curlx_str_عدد(&قدر, &tsize, CURL_OFF_T_MAX)) {
        إذا(!tsize) {
          failf(معلومات, "invalid tsize -:%s:- value in OACK packet", قدر);
          أرجع CURLE_TFTP_ILLEGAL;
        }
        infof(معلومات, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_سيٽ_Download_سائيز(معلومات, tsize);
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode tftp_option_جوڙيو(بنية tftp_conn *state, size_t *csize,
                                size_t اشاريو, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.معلومات[اشاريو];
  size_t oplen = لڙي_ڊيگهه(option);
  size_t blen;
  إذا((state->blksize <= اشاريو) ||
     (oplen + 1) > (size_t)(state->blksize - اشاريو))
    أرجع CURLE_TFTP_ILLEGAL;
  blen = state->blksize - اشاريو;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  أرجع CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#وصف_ڪريو ايندڙ_BLOCKNUM(x) (((x) + 1) & 0xffff)

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
  CURLcode نتيجو = CURLE_OK;
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

      إذا(rblock != state->بلاڪ &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->بلاڪ == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(معلومات, "Received ACK for block %d, expecting %d",
              rblock, state->بلاڪ);
        state->retries++;
        /* Bail out if over the maximum */
        إذا(state->retries > state->retry_max) {
          failf(معلومات, "tftp_tx: giving up waiting for block %d ack",
                state->بلاڪ);
          نتيجو = CURLE_SEND_ERROR;
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
            نتيجو = CURLE_SEND_ERROR;
          }
        }

        أرجع نتيجو;
      }
      /* This is the expected packet. Reset the counters and send the next
بلاڪ*/
      state->rx_وقت = وقت(عدم);
      state->بلاڪ++;
    }
    وإلا
      state->بلاڪ = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_معلومات);
    setpacketblock(&state->spacket, state->بلاڪ);
    إذا(state->بلاڪ > 1 && state->sbytes < state->blksize) {
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
      نتيجو = Curl_client_read(معلومات, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      إذا(نتيجو)
        أرجع نتيجو;
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
          "Retries = %d", ايندڙ_BLOCKNUM(state->بلاڪ), state->retries);
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
      Curl_pgrs_سيٽ_Upload_Counter(معلومات, k->writebytecount);
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاڪ);
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

  أرجع نتيجو;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتيجو;

  infof(state->معلومات, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  نتيجو = tftp_سيٽ_timeouts(state);
  إذا(نتيجو)
    أرجع نتيجو;
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
    إذا(ايندڙ_BLOCKNUM(state->بلاڪ) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وإلا إذا(state->بلاڪ == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(معلومات, "Received last DATA packet block %d again.", rblock);
    }
    وإلا {
      /* totally unexpected, log it */
      infof(معلومات,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, ايندڙ_BLOCKNUM(state->بلاڪ));
      اكسر;
    }

    /* ACK this block. */
    state->بلاڪ = (بنانشان ننڍو)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاڪ);
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
    state->rx_وقت = وقت(عدم);
    اكسر;

  حالة TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->بلاڪ = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاڪ);
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
    state->rx_وقت = وقت(عدم);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(معلومات,
          "Timeout waiting for block %d ACK. Retries = %d",
          ايندڙ_BLOCKNUM(state->بلاڪ), state->retries);
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
    setpacketblock(&state->spacket, state->بلاڪ);
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
  CURLcode نتيجو;

  infof(state->معلومات, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  نتيجو = tftp_سيٽ_timeouts(state);
  إذا(نتيجو)
    أرجع نتيجو;
  أرجع tftp_rx(state, event);
}

ساكن CURLcode tftp_send_پهريون(بنية tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *گهڻائي = "octet";
  حرف *filename;
  بنية Curl_easy *معلومات = state->معلومات;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتيجو = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  إذا(معلومات->state.prefer_ascii)
    گهڻائي = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      أرجع نتيجو;
    }

    إذا(معلومات->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      إذا(معلومات->state.infilesize != -1)
        Curl_pgrs_سيٽ_Upload_سائيز(معلومات, معلومات->state.infilesize);
    }
    وإلا {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    إذا(!state->معلومات->state.مٿي.رستو[1]) {
      failf(معلومات, "Missing filename");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    نتيجو = Curl_urldecode(&state->معلومات->state.مٿي.رستو[1], 0,
                            &filename, عدم, REJECT_ZERO);
    إذا(نتيجو)
      أرجع نتيجو;

    إذا(لڙي_ڊيگهه(filename) + لڙي_ڊيگهه(گهڻائي) + 4 > state->blksize) {
      failf(معلومات, "TFTP filename too long");
      curlx_free(filename);
      أرجع CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.معلومات + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', گهڻائي, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    إذا(!معلومات->سيٽ.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     معلومات->state.upload && (معلومات->state.infilesize != -1) ?
                     معلومات->state.infilesize : 0);

      نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      إذا(نتيجو == CURLE_OK)
        نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      إذا(نتيجو == CURLE_OK)
        نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      إذا(نتيجو == CURLE_OK)
        نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_وقت);
      إذا(نتيجو == CURLE_OK)
        نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      إذا(نتيجو == CURLE_OK)
        نتيجو = tftp_option_جوڙيو(state, &sbytes, sbytes, buf);

      إذا(نتيجو != CURLE_OK) {
        failf(معلومات, "TFTP buffer too small for options");
        أرجع CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#جيڪڏهن_وصف_ٿيل __AMIGA__
#وصف_ڪريو CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وإلا
#وصف_ڪريو CURL_SENDTO_ARG5(x) (x)
#جيڪڏهن_پورو
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
      نتيجو = tftp_connect_for_tx(state, event);
    }
    وإلا {
      نتيجو = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    إذا(!معلومات->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتيجو = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_معلومات: /* Connected for receive */
    إذا(معلومات->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    نتيجو = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->معلومات, "tftp_send_first: internal error");
    أرجع CURLE_TFTP_ILLEGAL;
  }

  أرجع نتيجو;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساكن CURLcode tftp_translate_ڪوڊ(tftp_error_t error)
{
  CURLcode نتيجو = CURLE_OK;

  إذا(error != TFTP_ERR_NONE) {
    بدل(error) {
    حالة TFTP_ERR_NOTFOUND:
      نتيجو = CURLE_TFTP_NOTFOUND;
      اكسر;
    حالة TFTP_ERR_PERM:
      نتيجو = CURLE_TFTP_PERM;
      اكسر;
    حالة TFTP_ERR_DISKFULL:
      نتيجو = CURLE_REMOTE_DISK_ڀريل;
      اكسر;
    حالة TFTP_ERR_UNDEF:
    حالة TFTP_ERR_ILLEGAL:
      نتيجو = CURLE_TFTP_ILLEGAL;
      اكسر;
    حالة TFTP_ERR_UNKNOWNID:
      نتيجو = CURLE_TFTP_UNKNOWNID;
      اكسر;
    حالة TFTP_ERR_EXISTS:
      نتيجو = CURLE_REMOTE_FILE_EXISTS;
      اكسر;
    حالة TFTP_ERR_NOSUCHUSER:
      نتيجو = CURLE_TFTP_NOSUCHUSER;
      اكسر;
    حالة TFTP_ERR_TIMEOUT:
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    حالة TFTP_ERR_NORESPONSE:
      نتيجو = CURLE_COULDNT_CONNECT;
      اكسر;
    افتراضي:
      نتيجو = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }
  وإلا
    نتيجو = CURLE_OK;

  أرجع نتيجو;
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
  CURLcode نتيجو = CURLE_OK;
  بنية Curl_easy *معلومات = state->معلومات;

  بدل(state->state) {
  حالة TFTP_STATE_شروعات:
    DEBUGF(infof(معلومات, "TFTP_STATE_START"));
    نتيجو = tftp_send_پهريون(state, event);
    اكسر;
  حالة TFTP_STATE_RX:
    DEBUGF(infof(معلومات, "TFTP_STATE_RX"));
    نتيجو = tftp_rx(state, event);
    اكسر;
  حالة TFTP_STATE_TX:
    DEBUGF(infof(معلومات, "TFTP_STATE_TX"));
    نتيجو = tftp_tx(state, event);
    اكسر;
  حالة TFTP_STATE_FIN:
    infof(معلومات, "%s", "TFTP finished");
    اكسر;
  افتراضي:
    DEBUGF(infof(معلومات, "STATE: %d", (صحيح)state->state));
    failf(معلومات, "%s", "Internal state machine error");
    نتيجو = CURLE_TFTP_ILLEGAL;
    اكسر;
  }

  أرجع نتيجو;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية tftp_conn *state = داخلا_2;
  (فراغ)ڪنجي;
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
ساكن CURLcode tftp_connect(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية tftp_conn *state;
  صحيح blksize;
  صحيح need_blksize;
  بنية connectdata *conn = معلومات->conn;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode نتيجو;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  إذا(!state ||
     Curl_conn_meta_سيٽ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  /* alloc pkt buffers based on specified blksize */
  إذا(معلومات->سيٽ.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)معلومات->سيٽ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  إذا(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  إذا(!state->rpacket.معلومات) {
    state->rpacket.معلومات = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->rpacket.معلومات)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  إذا(!state->spacket.معلومات) {
    state->spacket.معلومات = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->spacket.معلومات)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->معلومات = معلومات;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_شروعات;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(معلومات, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  إذا(!remote_addr)
    أرجع CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  نتيجو = tftp_سيٽ_timeouts(state);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!conn->بٽ.bound) {
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
    conn->بٽ.bound = TRUE;
  }

  *پورو = TRUE;

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساكن CURLcode tftp_پورو(بنية Curl_easy *معلومات, CURLcode حالت,
                          bool premature)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (فراغ)حالت;
  (فراغ)premature;

  إذا(Curl_pgrs_پورو(معلومات, Curl_pgrs_now(معلومات)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  إذا(state)
    نتيجو = tftp_translate_ڪوڊ(state->error);

  أرجع نتيجو;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  أرجع Curl_pollset_جوڙيو_اندريون(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
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
  CURLcode نتيجو = CURLE_OK;
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
      يادگيري_نقل(&state->remote_addr, &remote_addr, fromlen);
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
    بنانشان ننڍو event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_معلومات:
      إذا(معلومات->state.upload)
        أرجع CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      إذا(state->rbytes > 4 &&
         (ايندڙ_BLOCKNUM(state->بلاڪ) == getrpacketblock(&state->rpacket))) {
        نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.معلومات + 4,
                                   state->rbytes - 4);
        إذا(نتيجو) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          أرجع نتيجو;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      بنانشان ننڍو error = getrpacketblock(&state->rpacket);
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
      نتيجو = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.معلومات + 2,
                                     state->rbytes-2);
      إذا(نتيجو)
        أرجع نتيجو;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(معلومات, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    نتيجو = Curl_pgrsUpdate(معلومات);
    إذا(نتيجو) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      أرجع نتيجو;
    }
  }
  أرجع نتيجو;
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
  current = وقت(عدم);
  إذا(current > state->rx_وقت + state->retry_وقت) {
    إذا(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_وقت = وقت(عدم); /* update even though we received nothing */
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
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *معلومات, bool *پورو)
{
  tftp_event_t event;
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *پورو = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  إذا(timeout_ms < 0) {
    failf(معلومات, "TFTP response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  إذا(event != TFTP_EVENT_NONE) {
    نتيجو = tftp_state_machine(state, event);
    إذا(نتيجو)
      أرجع نتيجو;
    *پورو = (state->state == TFTP_STATE_FIN);
    إذا(*پورو)
      /* Tell curl we are done */
      Curl_xfer_تياري_nop(معلومات);
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
      نتيجو = tftp_receive_packet(معلومات, state);
      إذا(نتيجو)
        أرجع نتيجو;
      نتيجو = tftp_state_machine(state, state->event);
      إذا(نتيجو)
        أرجع نتيجو;
      *پورو = (state->state == TFTP_STATE_FIN);
      إذا(*پورو)
        /* Tell curl we are done */
        Curl_xfer_تياري_nop(معلومات);
    }
    /* if rc == 0, then select() timed out */
  }

  أرجع نتيجو;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساكن CURLcode tftp_doing(بنية Curl_easy *معلومات, bool *dophase_پورو)
{
  CURLcode نتيجو;
  نتيجو = tftp_multi_statemach(معلومات, dophase_پورو);

  إذا(*dophase_پورو) {
    DEBUGF(infof(معلومات, "DO phase is complete"));
  }
  وإلا إذا(!نتيجو) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    نتيجو = Curl_pgrsCheck(معلومات);
  }
  أرجع نتيجو;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساكن CURLcode tftp_perform(بنية Curl_easy *معلومات, bool *dophase_پورو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_پورو = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  نتيجو = tftp_state_machine(state, TFTP_EVENT_INIT);

  إذا((state->state == TFTP_STATE_FIN) || نتيجو)
    أرجع نتيجو;

  نتيجو = tftp_multi_statemach(معلومات, dophase_پورو);

  إذا(*dophase_پورو)
    DEBUGF(infof(معلومات, "DO phase is complete"));

  أرجع نتيجو;
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

ساكن CURLcode tftp_do(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية connectdata *conn = معلومات->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode نتيجو;

  *پورو = FALSE;

  إذا(!state) {
    نتيجو = tftp_connect(معلومات, پورو);
    إذا(نتيجو)
      أرجع نتيجو;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    إذا(!state)
      أرجع CURLE_TFTP_ILLEGAL;
  }

  نتيجو = tftp_perform(معلومات, پورو);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  إذا(!نتيجو)
    /* If we have encountered an internal tftp error, translate it. */
    نتيجو = tftp_translate_ڪوڊ(state->error);

  أرجع نتيجو;
}

ساكن CURLcode tftp_تياري_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  حرف *رستو = معلومات->state.مٿي.رستو;
  size_t len = لڙي_ڊيگهه(رستو);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  إذا((len >= 14) && !memcmp(&رستو[len - 14], ";mode=netascii", 14)) {
    معلومات->state.prefer_ascii = TRUE;
    رستو[len - 14] = 0; /* cut it there */
  }
  وإلا إذا((len >= 11) && !memcmp(&رستو[len - 11], ";mode=octet", 11)) {
    معلومات->state.prefer_ascii = FALSE;
    رستو[len - 11] = 0; /* cut it there */
  }

  أرجع CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_tftp = {
  tftp_تياري_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_پورو,                            /* done */
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

#جيڪڏهن_پورو
