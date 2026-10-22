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

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

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
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

تعريفنوع تعداد {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

تعريفنوع تعداد {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

تعريفنوع تعداد {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_DATA = 3,
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
  unsigned حرف *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

بنية tftp_conn {
  بنية Curl_sockaddr_storage local_addr;
  بنية Curl_sockaddr_storage remote_addr;
  بنية tftp_packet rpacket;
  بنية tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  بنية Curl_easy *data;
  curl_socket_t   sockfd;
  صحيح             retries;
  صحيح             retry_time;
  صحيح             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  صحيح             rbytes;
  size_t          sbytes;
  unsigned صحيح    blksize;
  unsigned صحيح    requested_blksize;
  unsigned short  block;
  BIT(remote_pinned);
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
ساكن CURLcode tftp_set_timeouts(بنية tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  ܐܢ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    ܦܢܐ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ܐܢ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ܐܚܪܢܐ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (صحيح)timeout / 5;

  /* Bound the total number */
  ܐܢ(state->retry_max < 3)
    state->retry_max = 3;

  ܐܢ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (صحيح)(timeout / state->retry_max);
  ܐܢ(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(عدم);

  ܦܢܐ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, unsigned short num)
{
  packet->data[0] = (unsigned حرف)(num >> 8);
  packet->data[1] = (unsigned حرف)(num & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, unsigned short num)
{
  packet->data[2] = (unsigned حرف)(num >> 8);
  packet->data[3] = (unsigned حرف)(num & 0xff);
}

ساكن unsigned short getrpacketevent(ثابت بنية tftp_packet *packet)
{
  ܦܢܐ (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

ساكن unsigned short getrpacketblock(ثابت بنية tftp_packet *packet)
{
  ܦܢܐ (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *end = memchr(string, '\0', maxlen);
  ܦܢܐ end ? (size_t)(end - string) : maxlen;
}

ساكن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **value)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ܐܢ(loc >= len)
    ܦܢܐ عدم;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ܐܢ(loc > len)
    ܦܢܐ عدم;
  *value = &buf[strlen(*option) + 1];

  ܦܢܐ &buf[loc];
}

ساكن CURLcode tftp_parse_option_ack(بنية tftp_conn *state,
                                      ثابت حرف *ptr, صحيح len)
{
  ثابت حرف *tmp = ptr;
  بنية Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ܟܡܐ(tmp < ptr + len) {
    ثابت حرف *option, *value;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &value);
    ܐܢ(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      ܦܢܐ CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, value);

    ܐܢ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ܐܢ(curlx_str_number(&value, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      }
      ܐܢ(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      }
      ܐܚܪܢܐ ܐܢ(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      }
      ܐܚܪܢܐ ܐܢ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned صحيح)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ܐܚܪܢܐ ܐܢ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ܐܢ(!data->state.upload &&
         !curlx_str_number(&value, &tsize, CURL_OFF_T_MAX)) {
        ܐܢ(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", value);
          ܦܢܐ CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode tftp_option_add(بنية tftp_conn *state, size_t *csize,
                                size_t index, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  ܐܢ((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    ܦܢܐ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ܦܢܐ CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#define NEXT_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ساكن CURLcode tftp_tx(بنية tftp_conn *state, tftp_event_t event)
{
  بنية Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  بنية SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  حرف buffer[STRERROR_LEN];
  حرف *bufptr;
  bool eos;

  ܐܢ(!data->state.upload)
    ܦܢܐ CURLE_TFTP_ILLEGAL;

  بدل(event) {

  حالة TFTP_EVENT_ACK:
  حالة TFTP_EVENT_OACK:
    ܐܢ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحيح rblock = getrpacketblock(&state->rpacket);

      ܐܢ(rblock != state->block &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->block == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(data, "Received ACK for block %d, expecting %d",
              rblock, state->block);
        state->retries++;
        /* Bail out if over the maximum */
        ܐܢ(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        ܐܚܪܢܐ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (بنية sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ܐܢ(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        ܦܢܐ result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(عدم);
      state->block++;
    }
    ܐܚܪܢܐ
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    ܐܢ(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ܦܢܐ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (حرف *)state->spacket.data + 4;
    ܥܒܕ {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ܐܢ(result)
        ܦܢܐ result;
      state->sbytes += cb;
      bufptr += cb;
    } ܟܡܐ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ܐܢ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      ܦܢܐ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    ܐܢ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ܐܚܪܢܐ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ܐܢ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
        ܦܢܐ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.data, 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(data, "tftp_tx: internal error, event: %d", (صحيح)event);
    اكسر;
  }

  ܦܢܐ result;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  ܐܢ(result)
    ܦܢܐ result;
  ܦܢܐ tftp_tx(state, event);
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
  بنية Curl_easy *data = state->data;
  حرف buffer[STRERROR_LEN];

  بدل(event) {

  حالة TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ܐܢ(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ܐܚܪܢܐ ܐܢ(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    ܐܚܪܢܐ {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      اكسر;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ܐܢ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      ܦܢܐ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ܐܢ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ܐܚܪܢܐ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(عدم);
    اكسر;

  حالة TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ܐܢ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      ܦܢܐ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(عدم);
    اكسر;

  حالة TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    ܐܢ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ܐܚܪܢܐ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ܐܢ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
        ܦܢܐ CURLE_SEND_ERROR;
      }
    }
    اكسر;

  حالة TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (فراغ)sendto(state->sockfd, (فراغ *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (بنية sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(data, "%s", "tftp_rx: internal error");
    ܦܢܐ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode tftp_connect_for_rx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  ܐܢ(result)
    ܦܢܐ result;
  ܦܢܐ tftp_rx(state, event);
}

ساكن CURLcode tftp_send_first(بنية tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت حرف *mode = "octet";
  حرف *filename;
  بنية Curl_easy *data = state->data;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ܐܢ(data->state.prefer_ascii)
    mode = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ܐܢ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ܦܢܐ result;
    }

    ܐܢ(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ܐܢ(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    ܐܚܪܢܐ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ܐܢ(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      ܦܢܐ CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, عدم, REJECT_ZERO);
    ܐܢ(result)
      ܦܢܐ result;

    ܐܢ(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      ܦܢܐ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ܐܢ(!data->set.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ܐܢ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      ܐܢ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ܐܢ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_time);
      ܐܢ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ܐܢ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      ܐܢ(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ܐܚܪܢܐ
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    ܐܢ(!remote_addr)
      ܦܢܐ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ܐܢ(senddata != (ssize_t)sbytes) {
      حرف buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      ܦܢܐ CURLE_SEND_ERROR;
    }
    اكسر;

  حالة TFTP_EVENT_OACK:
    ܐܢ(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    ܐܚܪܢܐ {
      result = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    ܐܢ(!data->state.upload)
      ܦܢܐ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_DATA: /* Connected for receive */
    ܐܢ(data->state.upload)
      ܦܢܐ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->data, "tftp_send_first: internal error");
    ܦܢܐ CURLE_TFTP_ILLEGAL;
  }

  ܦܢܐ result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ساكن CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  ܐܢ(error != TFTP_ERR_NONE) {
    بدل(error) {
    حالة TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      اكسر;
    حالة TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      اكسر;
    حالة TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      اكسر;
    حالة TFTP_ERR_UNDEF:
    حالة TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      اكسر;
    حالة TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      اكسر;
    حالة TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      اكسر;
    حالة TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      اكسر;
    حالة TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    حالة TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      اكسر;
    افتراضي:
      result = CURLE_ABORTED_BY_CALLBACK;
      اكسر;
    }
  }
  ܐܚܪܢܐ
    result = CURLE_OK;

  ܦܢܐ result;
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
  CURLcode result = CURLE_OK;
  بنية Curl_easy *data = state->data;

  بدل(state->state) {
  حالة TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    اكسر;
  حالة TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    اكسر;
  حالة TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    اكسر;
  حالة TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    اكسر;
  افتراضي:
    DEBUGF(infof(data, "STATE: %d", (صحيح)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    اكسر;
  }

  ܦܢܐ result;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية tftp_conn *state = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_safefree(state->rpacket.data);
  curlx_safefree(state->spacket.data);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ساكن CURLcode tftp_connect(بنية Curl_easy *data, bool *done)
{
  بنية tftp_conn *state;
  صحيح blksize;
  صحيح need_blksize;
  بنية connectdata *conn = data->conn;
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, حجم(*state));
  ܐܢ(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  ܐܢ(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ܐܢ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ܐܢ(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ܐܢ(!state->rpacket.data)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  ܐܢ(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ܐܢ(!state->spacket.data)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->data = data;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_START;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  ܐܢ(!remote_addr)
    ܦܢܐ CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  ܐܢ(result)
    ܦܢܐ result;

  ܐܢ(!conn->bits.bound) {
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
    ܐܢ(rc) {
      حرف buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      ܦܢܐ CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ܦܢܐ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ساكن CURLcode tftp_done(بنية Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (فراغ)status;
  (فراغ)premature;

  ܐܢ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ܦܢܐ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ܐܢ(state)
    result = tftp_translate_code(state->error);

  ܦܢܐ result;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  ܦܢܐ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ساكن CURLcode tftp_receive_packet(بنية Curl_easy *data,
                                    بنية tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  بنية Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = حجم(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحيح)recvfrom(state->sockfd,
                                (فراغ *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (بنية sockaddr *)&remote_addr,
                                &fromlen);
  ܐܢ((state->rbytes >= 0) && fromlen) {
    ܐܢ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ܐܢ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        ܦܢܐ CURLE_RECV_ERROR;
      }
    }
    ܐܚܪܢܐ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ܐܢ(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ܐܚܪܢܐ {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_DATA:
      ܐܢ(data->state.upload)
        ܦܢܐ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ܐܢ(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        ܐܢ(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ܦܢܐ result;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ܐܢ(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      اكسر;
    }
    حالة TFTP_EVENT_ACK:
      اكسر;
    حالة TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.data + 2,
                                     state->rbytes-2);
      ܐܢ(result)
        ܦܢܐ result;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(data, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    ܐܢ(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ܦܢܐ result;
    }
  }
  ܦܢܐ result;
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

  ܐܢ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  ܐܢ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ܦܢܐ timeout_ms;
  }
  current = time(عدم);
  ܐܢ(current > state->rx_time + state->retry_time) {
    ܐܢ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(عدم); /* update even though we received nothing */
  }

  ܦܢܐ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  ܐܢ(!state)
    ܦܢܐ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ܐܢ(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    ܦܢܐ CURLE_OPERATION_TIMEDOUT;
  }
  ܐܢ(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    ܐܢ(result)
      ܦܢܐ result;
    *done = (state->state == TFTP_STATE_FIN);
    ܐܢ(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  ܐܚܪܢܐ {
    /* no timeouts to handle, check our socket */
    صحيح rc = SOCKET_READABLE(state->sockfd, 0);

    ܐܢ(rc == -1) {
      /* bail out */
      صحيح sockerr = SOCKERRNO;
      حرف buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, حجم(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    ܐܚܪܢܐ ܐܢ(rc) {
      result = tftp_receive_packet(data, state);
      ܐܢ(result)
        ܦܢܐ result;
      result = tftp_state_machine(state, state->event);
      ܐܢ(result)
        ܦܢܐ result;
      *done = (state->state == TFTP_STATE_FIN);
      ܐܢ(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  ܦܢܐ result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ساكن CURLcode tftp_doing(بنية Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  ܐܢ(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  ܐܚܪܢܐ ܐܢ(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  ܦܢܐ result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ساكن CURLcode tftp_perform(بنية Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  ܐܢ(!state)
    ܦܢܐ CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  ܐܢ((state->state == TFTP_STATE_FIN) || result)
    ܦܢܐ result;

  result = tftp_multi_statemach(data, dophase_done);

  ܐܢ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ܦܢܐ result;
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

ساكن CURLcode tftp_do(بنية Curl_easy *data, bool *done)
{
  بنية connectdata *conn = data->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  ܐܢ(!state) {
    result = tftp_connect(data, done);
    ܐܢ(result)
      ܦܢܐ result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ܐܢ(!state)
      ܦܢܐ CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ܐܢ(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  ܦܢܐ result;
}

ساكن CURLcode tftp_setup_connection(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  حرف *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ܐܢ((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  ܐܚܪܢܐ ܐܢ((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ܦܢܐ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_tftp = {
  tftp_setup_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_done,                            /* done */
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

#endif
