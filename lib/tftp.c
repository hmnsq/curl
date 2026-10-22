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
#شامل "urldata.h"
#شامل "tftp.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_TFTP

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

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "connect.h"
#شامل "sockaddr.h" /* required for Curl_sockaddr_storage */
#شامل "url.h"
#شامل "strcase.h"
#شامل "select.h"
#شامل "escape.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"
#شامل "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#تعریف_کرو TFTP_BLKSIZE_DEFAULT 512
#تعریف_کرو TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#تعریف_کرو TFTP_OPTION_TSIZE    "tsize"
#تعریف_کرو TFTP_OPTION_INTERVAL "timeout"

قسمتعریف شمار {
  TFTP_منوال_NETASCII = 0,
  TFTP_منوال_OCTET
} tftp_منوال_t;

قسمتعریف شمار {
  TFTP_STATE_آغاز = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

قسمتعریف شمار {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_ڈیٹا = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

قسمتعریف شمار {
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

ساخت tftp_packet {
  غیرعلامتی حرف *ڈیٹا;
};

/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ساخت tftp_conn {
  ساخت Curl_sockaddr_storage local_addr;
  ساخت Curl_sockaddr_storage remote_addr;
  ساخت tftp_packet rpacket;
  ساخت tftp_packet spacket;
  tftp_state_t    state;
  tftp_منوال_t     منوال;
  tftp_error_t    error;
  tftp_event_t    event;
  ساخت Curl_easy *ڈیٹا;
  curl_socket_t   sockfd;
  صحیح             retries;
  صحیح             retry_وقت;
  صحیح             retry_max;
  time_t          rx_وقت;
  curl_socklen_t  remote_addrlen;
  صحیح             rbytes;
  size_t          sbytes;
  غیرعلامتی صحیح    blksize;
  غیرعلامتی صحیح    requested_blksize;
  غیرعلامتی مختصر  بلاک;
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
ساکن CURLcode tftp_سیٹ_timeouts(ساخت tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->ڈیٹا);

  اگر(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->ڈیٹا, "Connection time-out");
    واپس CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  اگر((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ورنہ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (صحیح)timeout / 5;

  /* Bound the total number */
  اگر(state->retry_max < 3)
    state->retry_max = 3;

  اگر(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_وقت = (صحیح)(timeout / state->retry_max);
  اگر(state->retry_وقت < 1)
    state->retry_وقت = 1;

  infof(state->ڈیٹا,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحیح)state->state, timeout_ms, state->retry_وقت, state->retry_max);

  /* init RX time */
  state->rx_وقت = وقت(تہی);

  واپس CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساکن خالی setpacketevent(ساخت tftp_packet *packet, غیرعلامتی مختصر عدد)
{
  packet->ڈیٹا[0] = (غیرعلامتی حرف)(عدد >> 8);
  packet->ڈیٹا[1] = (غیرعلامتی حرف)(عدد & 0xff);
}

ساکن خالی setpacketblock(ساخت tftp_packet *packet, غیرعلامتی مختصر عدد)
{
  packet->ڈیٹا[2] = (غیرعلامتی حرف)(عدد >> 8);
  packet->ڈیٹا[3] = (غیرعلامتی حرف)(عدد & 0xff);
}

ساکن غیرعلامتی مختصر getrpacketevent(ثابت ساخت tftp_packet *packet)
{
  واپس (غیرعلامتی مختصر)((packet->ڈیٹا[0] << 8) | packet->ڈیٹا[1]);
}

ساکن غیرعلامتی مختصر getrpacketblock(ثابت ساخت tftp_packet *packet)
{
  واپس (غیرعلامتی مختصر)((packet->ڈیٹا[2] << 8) | packet->ڈیٹا[3]);
}

ساکن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *انتہا = memchr(string, '\0', maxlen);
  واپس انتہا ? (size_t)(انتہا - string) : maxlen;
}

ساکن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **قدر)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  اگر(loc >= len)
    واپس تہی;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  اگر(loc > len)
    واپس تہی;
  *قدر = &buf[لڑی_لمبائی(*option) + 1];

  واپس &buf[loc];
}

ساکن CURLcode tftp_parse_option_ack(ساخت tftp_conn *state,
                                      ثابت حرف *ptr, صحیح len)
{
  ثابت حرف *tmp = ptr;
  ساخت Curl_easy *ڈیٹا = state->ڈیٹا;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  جب(tmp < ptr + len) {
    ثابت حرف *option, *قدر;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &قدر);
    اگر(!tmp) {
      failf(ڈیٹا, "Malformed ACK packet, rejecting");
      واپس CURLE_TFTP_ILLEGAL;
    }
    olen = لڑی_لمبائی(option);

    infof(ڈیٹا, "got option=(%s) value=(%s)", option, قدر);

    اگر((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      اگر(curlx_str_نمبر(&قدر, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(ڈیٹا, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        واپس CURLE_TFTP_ILLEGAL;
      }
      اگر(!blksize) {
        failf(ڈیٹا, "invalid blocksize value in OACK packet");
        واپس CURLE_TFTP_ILLEGAL;
      }
      ورنہ اگر(blksize < TFTP_BLKSIZE_MIN) {
        failf(ڈیٹا, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        واپس CURLE_TFTP_ILLEGAL;
      }
      ورنہ اگر(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(ڈیٹا, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        واپس CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (غیرعلامتی صحیح)blksize;
      infof(ڈیٹا, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ورنہ اگر((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      اگر(!ڈیٹا->state.upload &&
         !curlx_str_نمبر(&قدر, &tsize, CURL_OFF_T_MAX)) {
        اگر(!tsize) {
          failf(ڈیٹا, "invalid tsize -:%s:- value in OACK packet", قدر);
          واپس CURLE_TFTP_ILLEGAL;
        }
        infof(ڈیٹا, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, tsize);
      }
    }
  }

  واپس CURLE_OK;
}

ساکن CURLcode tftp_option_جوڑو(ساخت tftp_conn *state, size_t *csize,
                                size_t اشاریہ, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.ڈیٹا[اشاریہ];
  size_t oplen = لڑی_لمبائی(option);
  size_t blen;
  اگر((state->blksize <= اشاریہ) ||
     (oplen + 1) > (size_t)(state->blksize - اشاریہ))
    واپس CURLE_TFTP_ILLEGAL;
  blen = state->blksize - اشاریہ;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  واپس CURLE_OK;
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
ساکن CURLcode tftp_tx(ساخت tftp_conn *state, tftp_event_t event)
{
  ساخت Curl_easy *ڈیٹا = state->ڈیٹا;
  ssize_t sbytes;
  CURLcode نتیجہ = CURLE_OK;
  ساخت SingleRequest *k = &ڈیٹا->req;
  size_t cb; /* Bytes currently read */
  حرف بفر[STRERROR_LEN];
  حرف *bufptr;
  bool eos;

  اگر(!ڈیٹا->state.upload)
    واپس CURLE_TFTP_ILLEGAL;

  انتخاب(event) {

  حالت TFTP_EVENT_ACK:
  حالت TFTP_EVENT_OACK:
    اگر(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحیح rblock = getrpacketblock(&state->rpacket);

      اگر(rblock != state->بلاک &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->بلاک == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(ڈیٹا, "Received ACK for block %d, expecting %d",
              rblock, state->بلاک);
        state->retries++;
        /* Bail out if over the maximum */
        اگر(state->retries > state->retry_max) {
          failf(ڈیٹا, "tftp_tx: giving up waiting for block %d ack",
                state->بلاک);
          نتیجہ = CURLE_SEND_ERROR;
        }
        ورنہ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ساخت sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          اگر(sbytes < 0) {
            failf(ڈیٹا, "%s",
                  curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
            نتیجہ = CURLE_SEND_ERROR;
          }
        }

        واپس نتیجہ;
      }
      /* This is the expected packet. Reset the counters and send the next
بلاک*/
      state->rx_وقت = وقت(تہی);
      state->بلاک++;
    }
    ورنہ
      state->بلاک = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ڈیٹا);
    setpacketblock(&state->spacket, state->بلاک);
    اگر(state->بلاک > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      واپس CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (حرف *)state->spacket.ڈیٹا + 4;
    کرو {
      نتیجہ = Curl_client_read(ڈیٹا, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      اگر(نتیجہ)
        واپس نتیجہ;
      state->sbytes += cb;
      bufptr += cb;
    } جب(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ساخت sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    اگر(sbytes < 0) {
      failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      واپس CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(ڈیٹا, state->sbytes);
    توڑو;

  حالت TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(ڈیٹا, "Timeout waiting for block %d ACK. "
          "Retries = %d", اگلا_BLOCKNUM(state->بلاک), state->retries);
    /* Decide if we have had enough */
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ورنہ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ساخت sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      اگر(sbytes < 0) {
        failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        واپس CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_سیٹ_Upload_شمار(ڈیٹا, k->writebytecount);
    }
    توڑو;

  حالت TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (خالی)sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا, 4, SEND_4TH_ARG,
                 (ساخت sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    توڑو;

  طےشدہ:
    failf(ڈیٹا, "tftp_tx: internal error, event: %d", (صحیح)event);
    توڑو;
  }

  واپس نتیجہ;
}

ساکن CURLcode tftp_connect_for_tx(ساخت tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجہ;

  infof(state->ڈیٹا, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  نتیجہ = tftp_سیٹ_timeouts(state);
  اگر(نتیجہ)
    واپس نتیجہ;
  واپس tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ساکن CURLcode tftp_rx(ساخت tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  صحیح rblock;
  ساخت Curl_easy *ڈیٹا = state->ڈیٹا;
  حرف بفر[STRERROR_LEN];

  انتخاب(event) {

  حالت TFTP_EVENT_ڈیٹا:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    اگر(اگلا_BLOCKNUM(state->بلاک) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ورنہ اگر(state->بلاک == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(ڈیٹا, "Received last DATA packet block %d again.", rblock);
    }
    ورنہ {
      /* totally unexpected, log it */
      infof(ڈیٹا,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, اگلا_BLOCKNUM(state->بلاک));
      توڑو;
    }

    /* ACK this block. */
    state->بلاک = (غیرعلامتی مختصر)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                    4, SEND_4TH_ARG,
                    (ساخت sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    اگر(sbytes < 0) {
      failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      واپس CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    اگر(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ورنہ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_وقت = وقت(تہی);
    توڑو;

  حالت TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->بلاک = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->بلاک);
    sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                    4, SEND_4TH_ARG,
                    (ساخت sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    اگر(sbytes < 0) {
      failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      واپس CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_وقت = وقت(تہی);
    توڑو;

  حالت TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(ڈیٹا,
          "Timeout waiting for block %d ACK. Retries = %d",
          اگلا_BLOCKNUM(state->بلاک), state->retries);
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ورنہ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                      4, SEND_4TH_ARG,
                      (ساخت sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      اگر(sbytes < 0) {
        failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
        واپس CURLE_SEND_ERROR;
      }
    }
    توڑو;

  حالت TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->بلاک);
    (خالی)sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                 4, SEND_4TH_ARG,
                 (ساخت sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    توڑو;

  طےشدہ:
    failf(ڈیٹا, "%s", "tftp_rx: internal error");
    واپس CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  واپس CURLE_OK;
}

ساکن CURLcode tftp_connect_for_rx(ساخت tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode نتیجہ;

  infof(state->ڈیٹا, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  نتیجہ = tftp_سیٹ_timeouts(state);
  اگر(نتیجہ)
    واپس نتیجہ;
  واپس tftp_rx(state, event);
}

ساکن CURLcode tftp_send_پہلا(ساخت tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *منوال = "octet";
  حرف *filename;
  ساخت Curl_easy *ڈیٹا = state->ڈیٹا;
  ثابت ساخت Curl_sockaddr_ex *remote_addr = تہی;
  CURLcode نتیجہ = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  اگر(ڈیٹا->state.prefer_ascii)
    منوال = "netascii";

  انتخاب(event) {

  حالت TFTP_EVENT_INIT:    /* Send the first packet out */
  حالت TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      واپس نتیجہ;
    }

    اگر(ڈیٹا->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      اگر(ڈیٹا->state.infilesize != -1)
        Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);
    }
    ورنہ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    اگر(!state->ڈیٹا->state.اوپر.راستہ[1]) {
      failf(ڈیٹا, "Missing filename");
      واپس CURLE_TFTP_ILLEGAL;
    }
    نتیجہ = Curl_urldecode(&state->ڈیٹا->state.اوپر.راستہ[1], 0,
                            &filename, تہی, REJECT_ZERO);
    اگر(نتیجہ)
      واپس نتیجہ;

    اگر(لڑی_لمبائی(filename) + لڑی_لمبائی(منوال) + 4 > state->blksize) {
      failf(ڈیٹا, "TFTP filename too long");
      curlx_free(filename);
      واپس CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.ڈیٹا + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', منوال, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    اگر(!ڈیٹا->سیٹ.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     ڈیٹا->state.upload && (ڈیٹا->state.infilesize != -1) ?
                     ڈیٹا->state.infilesize : 0);

      نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      اگر(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      اگر(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      اگر(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_وقت);
      اگر(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      اگر(نتیجہ == CURLE_OK)
        نتیجہ = tftp_option_جوڑو(state, &sbytes, sbytes, buf);

      اگر(نتیجہ != CURLE_OK) {
        failf(ڈیٹا, "TFTP buffer too small for options");
        واپس CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#اگر_تعریف_شدہ __AMIGA__
#تعریف_کرو CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ورنہ
#تعریف_کرو CURL_SENDTO_ARG5(x) (x)
#اگر_ختم
    remote_addr = Curl_conn_get_remote_addr(ڈیٹا, FIRSTSOCKET);
    اگر(!remote_addr)
      واپس CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (خالی *)state->spacket.ڈیٹا,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    اگر(senddata != (ssize_t)sbytes) {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "%s", curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      واپس CURLE_SEND_ERROR;
    }
    توڑو;

  حالت TFTP_EVENT_OACK:
    اگر(ڈیٹا->state.upload) {
      نتیجہ = tftp_connect_for_tx(state, event);
    }
    ورنہ {
      نتیجہ = tftp_connect_for_rx(state, event);
    }
    توڑو;

  حالت TFTP_EVENT_ACK: /* Connected for transmit */
    اگر(!ڈیٹا->state.upload)
      واپس CURLE_TFTP_ILLEGAL;
    نتیجہ = tftp_connect_for_tx(state, event);
    توڑو;

  حالت TFTP_EVENT_ڈیٹا: /* Connected for receive */
    اگر(ڈیٹا->state.upload)
      واپس CURLE_TFTP_ILLEGAL;
    نتیجہ = tftp_connect_for_rx(state, event);
    توڑو;

  حالت TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    توڑو;

  طےشدہ:
    failf(state->ڈیٹا, "tftp_send_first: internal error");
    واپس CURLE_TFTP_ILLEGAL;
  }

  واپس نتیجہ;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساکن CURLcode tftp_translate_رمز(tftp_error_t error)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(error != TFTP_ERR_NONE) {
    انتخاب(error) {
    حالت TFTP_ERR_NOTFOUND:
      نتیجہ = CURLE_TFTP_NOTFOUND;
      توڑو;
    حالت TFTP_ERR_PERM:
      نتیجہ = CURLE_TFTP_PERM;
      توڑو;
    حالت TFTP_ERR_DISKFULL:
      نتیجہ = CURLE_REMOTE_DISK_بھرا;
      توڑو;
    حالت TFTP_ERR_UNDEF:
    حالت TFTP_ERR_ILLEGAL:
      نتیجہ = CURLE_TFTP_ILLEGAL;
      توڑو;
    حالت TFTP_ERR_UNKNOWNID:
      نتیجہ = CURLE_TFTP_UNKNOWNID;
      توڑو;
    حالت TFTP_ERR_EXISTS:
      نتیجہ = CURLE_REMOTE_FILE_EXISTS;
      توڑو;
    حالت TFTP_ERR_NOSUCHUSER:
      نتیجہ = CURLE_TFTP_NOSUCHUSER;
      توڑو;
    حالت TFTP_ERR_TIMEOUT:
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    حالت TFTP_ERR_NORESPONSE:
      نتیجہ = CURLE_COULDNT_CONNECT;
      توڑو;
    طےشدہ:
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      توڑو;
    }
  }
  ورنہ
    نتیجہ = CURLE_OK;

  واپس نتیجہ;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ساکن CURLcode tftp_state_machine(ساخت tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت Curl_easy *ڈیٹا = state->ڈیٹا;

  انتخاب(state->state) {
  حالت TFTP_STATE_آغاز:
    DEBUGF(infof(ڈیٹا, "TFTP_STATE_START"));
    نتیجہ = tftp_send_پہلا(state, event);
    توڑو;
  حالت TFTP_STATE_RX:
    DEBUGF(infof(ڈیٹا, "TFTP_STATE_RX"));
    نتیجہ = tftp_rx(state, event);
    توڑو;
  حالت TFTP_STATE_TX:
    DEBUGF(infof(ڈیٹا, "TFTP_STATE_TX"));
    نتیجہ = tftp_tx(state, event);
    توڑو;
  حالت TFTP_STATE_FIN:
    infof(ڈیٹا, "%s", "TFTP finished");
    توڑو;
  طےشدہ:
    DEBUGF(infof(ڈیٹا, "STATE: %d", (صحیح)state->state));
    failf(ڈیٹا, "%s", "Internal state machine error");
    نتیجہ = CURLE_TFTP_ILLEGAL;
    توڑو;
  }

  واپس نتیجہ;
}

ساکن خالی tftp_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت tftp_conn *state = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_safefree(state->rpacket.ڈیٹا);
  curlx_safefree(state->spacket.ڈیٹا);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ساکن CURLcode tftp_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت tftp_conn *state;
  صحیح blksize;
  صحیح need_blksize;
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت ساخت Curl_sockaddr_ex *remote_addr = تہی;
  CURLcode نتیجہ;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  اگر(!state ||
     Curl_conn_meta_سیٹ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  /* alloc pkt buffers based on specified blksize */
  اگر(ڈیٹا->سیٹ.tftp_blksize)
    /* range checked when set */
    blksize = (صحیح)ڈیٹا->سیٹ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  اگر(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  اگر(!state->rpacket.ڈیٹا) {
    state->rpacket.ڈیٹا = curlx_calloc(1, need_blksize + 2 + 2);

    اگر(!state->rpacket.ڈیٹا)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(!state->spacket.ڈیٹا) {
    state->spacket.ڈیٹا = curlx_calloc(1, need_blksize + 2 + 2);

    اگر(!state->spacket.ڈیٹا)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->ڈیٹا = ڈیٹا;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_آغاز;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(ڈیٹا, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  اگر(!remote_addr)
    واپس CURLE_FAILED_INIT;

  ((ساخت sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  نتیجہ = tftp_سیٹ_timeouts(state);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!conn->بٹ.bound) {
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
    صحیح rc = bind(state->sockfd, (ساخت sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    اگر(rc) {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
      واپس CURLE_COULDNT_CONNECT;
    }
    conn->بٹ.bound = TRUE;
  }

  *مکمل = TRUE;

  واپس CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساکن CURLcode tftp_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                          bool premature)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (خالی)حالت_2;
  (خالی)premature;

  اگر(Curl_pgrs_مکمل(ڈیٹا, Curl_pgrs_now(ڈیٹا)))
    واپس CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  اگر(state)
    نتیجہ = tftp_translate_رمز(state->error);

  واپس نتیجہ;
}

ساکن CURLcode tftp_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  واپس Curl_pollset_جوڑو_اندر(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ساکن CURLcode tftp_receive_packet(ساخت Curl_easy *ڈیٹا,
                                    ساخت tftp_conn *state)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = حجم(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحیح)recvfrom(state->sockfd,
                                (خالی *)state->rpacket.ڈیٹا,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ساخت sockaddr *)&remote_addr,
                                &fromlen);
  اگر((state->rbytes >= 0) && fromlen) {
    اگر(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      اگر((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(ڈیٹا, "Data received from another address");
        واپس CURLE_RECV_ERROR;
      }
    }
    ورنہ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      حافظہ_نقل(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  اگر(state->rbytes < 4) {
    failf(ڈیٹا, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ورنہ {
    /* The event is given by the TFTP packet time */
    غیرعلامتی مختصر event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    انتخاب(state->event) {
    حالت TFTP_EVENT_ڈیٹا:
      اگر(ڈیٹا->state.upload)
        واپس CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      اگر(state->rbytes > 4 &&
         (اگلا_BLOCKNUM(state->بلاک) == getrpacketblock(&state->rpacket))) {
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.ڈیٹا + 4,
                                   state->rbytes - 4);
        اگر(نتیجہ) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          واپس نتیجہ;
        }
      }
      توڑو;
    حالت TFTP_EVENT_ERROR: {
      غیرعلامتی مختصر error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.ڈیٹا + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      اگر(tftp_strnlen(str, strn) < strn)
        infof(ڈیٹا, "TFTP error: %s", str);
      توڑو;
    }
    حالت TFTP_EVENT_ACK:
      توڑو;
    حالت TFTP_EVENT_OACK:
      نتیجہ = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.ڈیٹا + 2,
                                     state->rbytes-2);
      اگر(نتیجہ)
        واپس نتیجہ;
      توڑو;
    حالت TFTP_EVENT_RRQ:
    حالت TFTP_EVENT_WRQ:
    طےشدہ:
      failf(ڈیٹا, "%s", "Internal error: Unexpected packet");
      توڑو;
    }

    /* Update the progress meter */
    نتیجہ = Curl_pgrsUpdate(ڈیٹا);
    اگر(نتیجہ) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      واپس نتیجہ;
    }
  }
  واپس نتیجہ;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ساکن timediff_t tftp_state_timeout(ساخت tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  اگر(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->ڈیٹا);
  اگر(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    واپس timeout_ms;
  }
  current = وقت(تہی);
  اگر(current > state->rx_وقت + state->retry_وقت) {
    اگر(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_وقت = وقت(تہی); /* update even though we received nothing */
  }

  واپس timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ساکن CURLcode tftp_multi_statemach(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  tftp_event_t event;
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *مکمل = FALSE;
  اگر(!state)
    واپس CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  اگر(timeout_ms < 0) {
    failf(ڈیٹا, "TFTP response timeout");
    واپس CURLE_OPERATION_TIMEDOUT;
  }
  اگر(event != TFTP_EVENT_NONE) {
    نتیجہ = tftp_state_machine(state, event);
    اگر(نتیجہ)
      واپس نتیجہ;
    *مکمل = (state->state == TFTP_STATE_FIN);
    اگر(*مکمل)
      /* Tell curl we are done */
      Curl_xfer_تیاری_nop(ڈیٹا);
  }
  ورنہ {
    /* no timeouts to handle, check our socket */
    صحیح rc = SOCKET_READABLE(state->sockfd, 0);

    اگر(rc == -1) {
      /* bail out */
      صحیح sockerr = SOCKERRNO;
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "%s", curlx_strerror(sockerr, بفر, حجم(بفر)));
      state->event = TFTP_EVENT_ERROR;
    }
    ورنہ اگر(rc) {
      نتیجہ = tftp_receive_packet(ڈیٹا, state);
      اگر(نتیجہ)
        واپس نتیجہ;
      نتیجہ = tftp_state_machine(state, state->event);
      اگر(نتیجہ)
        واپس نتیجہ;
      *مکمل = (state->state == TFTP_STATE_FIN);
      اگر(*مکمل)
        /* Tell curl we are done */
        Curl_xfer_تیاری_nop(ڈیٹا);
    }
    /* if rc == 0, then select() timed out */
  }

  واپس نتیجہ;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساکن CURLcode tftp_doing(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  CURLcode نتیجہ;
  نتیجہ = tftp_multi_statemach(ڈیٹا, dophase_مکمل);

  اگر(*dophase_مکمل) {
    DEBUGF(infof(ڈیٹا, "DO phase is complete"));
  }
  ورنہ اگر(!نتیجہ) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    نتیجہ = Curl_pgrsCheck(ڈیٹا);
  }
  واپس نتیجہ;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساکن CURLcode tftp_perform(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_مکمل = FALSE;
  اگر(!state)
    واپس CURLE_FAILED_INIT;

  نتیجہ = tftp_state_machine(state, TFTP_EVENT_INIT);

  اگر((state->state == TFTP_STATE_FIN) || نتیجہ)
    واپس نتیجہ;

  نتیجہ = tftp_multi_statemach(ڈیٹا, dophase_مکمل);

  اگر(*dophase_مکمل)
    DEBUGF(infof(ڈیٹا, "DO phase is complete"));

  واپس نتیجہ;
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

ساکن CURLcode tftp_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode نتیجہ;

  *مکمل = FALSE;

  اگر(!state) {
    نتیجہ = tftp_connect(ڈیٹا, مکمل);
    اگر(نتیجہ)
      واپس نتیجہ;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    اگر(!state)
      واپس CURLE_TFTP_ILLEGAL;
  }

  نتیجہ = tftp_perform(ڈیٹا, مکمل);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  اگر(!نتیجہ)
    /* If we have encountered an internal tftp error, translate it. */
    نتیجہ = tftp_translate_رمز(state->error);

  واپس نتیجہ;
}

ساکن CURLcode tftp_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  حرف *راستہ = ڈیٹا->state.اوپر.راستہ;
  size_t len = لڑی_لمبائی(راستہ);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  اگر((len >= 14) && !memcmp(&راستہ[len - 14], ";mode=netascii", 14)) {
    ڈیٹا->state.prefer_ascii = TRUE;
    راستہ[len - 14] = 0; /* cut it there */
  }
  ورنہ اگر((len >= 11) && !memcmp(&راستہ[len - 11], ";mode=octet", 11)) {
    ڈیٹا->state.prefer_ascii = FALSE;
    راستہ[len - 11] = 0; /* cut it there */
  }

  واپس CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_tftp = {
  tftp_تیاری_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_مکمل,                            /* done */
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

#اگر_ختم
