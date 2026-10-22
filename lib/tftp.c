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

  إذا(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
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
  state->retry_time = (صحيح)(timeout / state->retry_max);
  إذا(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحيح)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(عدم);

  أرجع CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ساكن فراغ setpacketevent(بنية tftp_packet *packet, unsigned short رەقەم)
{
  packet->data[0] = (unsigned حرف)(رەقەم >> 8);
  packet->data[1] = (unsigned حرف)(رەقەم & 0xff);
}

ساكن فراغ setpacketblock(بنية tftp_packet *packet, unsigned short رەقەم)
{
  packet->data[2] = (unsigned حرف)(رەقەم >> 8);
  packet->data[3] = (unsigned حرف)(رەقەم & 0xff);
}

ساكن unsigned short getrpacketevent(ثابت بنية tftp_packet *packet)
{
  أرجع (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

ساكن unsigned short getrpacketblock(ثابت بنية tftp_packet *packet)
{
  أرجع (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

ساكن size_t tftp_strnlen(ثابت حرف *string, size_t maxlen)
{
  ثابت حرف *end = memchr(string, '\0', maxlen);
  أرجع end ? (size_t)(end - string) : maxlen;
}

ساكن ثابت حرف *tftp_option_get(ثابت حرف *buf, size_t len,
                                   ثابت حرف **option, ثابت حرف **قىممەت)
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
  *قىممەت = &buf[strlen(*option) + 1];

  أرجع &buf[loc];
}

ساكن CURLcode tftp_parse_option_ack(بنية tftp_conn *state,
                                      ثابت حرف *ptr, صحيح len)
{
  ثابت حرف *tmp = ptr;
  بنية Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  طالما(tmp < ptr + len) {
    ثابت حرف *option, *قىممەت;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &قىممەت);
    إذا(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, قىممەت);

    إذا((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      إذا(curlx_str_سان(&قىممەت, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      إذا(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        أرجع CURLE_TFTP_ILLEGAL;
      }
      وإلا إذا(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        أرجع CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned صحيح)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وإلا إذا((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      إذا(!data->state.upload &&
         !curlx_str_سان(&قىممەت, &tsize, CURL_OFF_T_MAX)) {
        إذا(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", قىممەت);
          أرجع CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode tftp_option_add(بنية tftp_conn *state, size_t *csize,
                                size_t index, ثابت حرف *option)
{
  حرف *buf = (حرف *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  إذا((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    أرجع CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  أرجع CURLE_OK;
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

  إذا(!data->state.upload)
    أرجع CURLE_TFTP_ILLEGAL;

  بدل(event) {

  حالة TFTP_EVENT_ACK:
  حالة TFTP_EVENT_OACK:
    إذا(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحيح rblock = getrpacketblock(&state->rpacket);

      إذا(rblock != state->block &&
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
        إذا(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        وإلا {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (بنية sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          إذا(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        أرجع result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(عدم);
      state->block++;
    }
    وإلا
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    إذا(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      أرجع CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (حرف *)state->spacket.data + 4;
    افعل {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      إذا(result)
        أرجع result;
      state->sbytes += cb;
      bufptr += cb;
    } طالما(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (بنية sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    إذا(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      أرجع CURLE_SEND_ERROR;
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
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      إذا(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
        أرجع CURLE_SEND_ERROR;
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

  أرجع result;
}

ساكن CURLcode tftp_connect_for_tx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  إذا(result)
    أرجع result;
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
  بنية Curl_easy *data = state->data;
  حرف buffer[STRERROR_LEN];

  بدل(event) {

  حالة TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    إذا(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وإلا إذا(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    وإلا {
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
    إذا(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      أرجع CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    إذا(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
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
    إذا(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      أرجع CURLE_SEND_ERROR;
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
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وإلا {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (بنية sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      إذا(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
        أرجع CURLE_SEND_ERROR;
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
    أرجع CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  أرجع CURLE_OK;
}

ساكن CURLcode tftp_connect_for_rx(بنية tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  إذا(result)
    أرجع result;
  أرجع tftp_rx(state, event);
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
  إذا(data->state.prefer_ascii)
    mode = "netascii";

  بدل(event) {

  حالة TFTP_EVENT_INIT:    /* Send the first packet out */
  حالة TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    إذا(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      أرجع result;
    }

    إذا(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      إذا(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    وإلا {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    إذا(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      أرجع CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, عدم, REJECT_ZERO);
    إذا(result)
      أرجع result;

    إذا(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      أرجع CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((حرف *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    إذا(!data->set.tftp_no_options) {
      حرف buf[64];
      /* add tsize option */
      curl_msnprintf(buf, حجم(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      إذا(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, حجم(buf), "%u", state->requested_blksize);
      إذا(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      إذا(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, حجم(buf), "%d", state->retry_time);
      إذا(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      إذا(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      إذا(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        أرجع CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وإلا
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    إذا(!remote_addr)
      أرجع CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (فراغ *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    إذا(senddata != (ssize_t)sbytes) {
      حرف buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      أرجع CURLE_SEND_ERROR;
    }
    اكسر;

  حالة TFTP_EVENT_OACK:
    إذا(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    وإلا {
      result = tftp_connect_for_rx(state, event);
    }
    اكسر;

  حالة TFTP_EVENT_ACK: /* Connected for transmit */
    إذا(!data->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    اكسر;

  حالة TFTP_EVENT_DATA: /* Connected for receive */
    إذا(data->state.upload)
      أرجع CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    اكسر;

  حالة TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    اكسر;

  افتراضي:
    failf(state->data, "tftp_send_first: internal error");
    أرجع CURLE_TFTP_ILLEGAL;
  }

  أرجع result;
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

  إذا(error != TFTP_ERR_NONE) {
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
  وإلا
    result = CURLE_OK;

  أرجع result;
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

  أرجع result;
}

ساكن فراغ tftp_conn_dtor(ثابت فراغ *ئاچقۇچ, size_t klen, فراغ *entry)
{
  بنية tftp_conn *state = entry;
  (فراغ)ئاچقۇچ;
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
  إذا(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  إذا(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (صحيح)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  إذا(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  إذا(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->rpacket.data)
      أرجع CURLE_OUT_OF_MEMORY;
  }

  إذا(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    إذا(!state->spacket.data)
      أرجع CURLE_OUT_OF_MEMORY;
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
  إذا(!remote_addr)
    أرجع CURLE_FAILED_INIT;

  ((بنية sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  إذا(result)
    أرجع result;

  إذا(!conn->bits.bound) {
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
      حرف buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
      أرجع CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  أرجع CURLE_OK;
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

  إذا(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  إذا(state)
    result = tftp_translate_code(state->error);

  أرجع result;
}

ساكن CURLcode tftp_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  أرجع Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
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
  إذا((state->rbytes >= 0) && fromlen) {
    إذا(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      إذا((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  إذا(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  وإلا {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    بدل(state->event) {
    حالة TFTP_EVENT_DATA:
      إذا(data->state.upload)
        أرجع CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      إذا(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ثابت حرف *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        إذا(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          أرجع result;
        }
      }
      اكسر;
    حالة TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ثابت حرف *str = (ثابت حرف *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      إذا(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      اكسر;
    }
    حالة TFTP_EVENT_ACK:
      اكسر;
    حالة TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ثابت حرف *)state->rpacket.data + 2,
                                     state->rbytes-2);
      إذا(result)
        أرجع result;
      اكسر;
    حالة TFTP_EVENT_RRQ:
    حالة TFTP_EVENT_WRQ:
    افتراضي:
      failf(data, "%s", "Internal error: Unexpected packet");
      اكسر;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    إذا(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      أرجع result;
    }
  }
  أرجع result;
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

  timeout_ms = Curl_timeleft_ms(state->data);
  إذا(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    أرجع timeout_ms;
  }
  current = time(عدم);
  إذا(current > state->rx_time + state->retry_time) {
    إذا(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(عدم); /* update even though we received nothing */
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
ساكن CURLcode tftp_multi_statemach(بنية Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  إذا(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  إذا(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    إذا(result)
      أرجع result;
    *done = (state->state == TFTP_STATE_FIN);
    إذا(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  وإلا {
    /* no timeouts to handle, check our socket */
    صحيح rc = SOCKET_READABLE(state->sockfd, 0);

    إذا(rc == -1) {
      /* bail out */
      صحيح sockerr = SOCKERRNO;
      حرف buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, حجم(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    وإلا إذا(rc) {
      result = tftp_receive_packet(data, state);
      إذا(result)
        أرجع result;
      result = tftp_state_machine(state, state->event);
      إذا(result)
        أرجع result;
      *done = (state->state == TFTP_STATE_FIN);
      إذا(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  أرجع result;
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

  إذا(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  وإلا إذا(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  أرجع result;
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
  إذا(!state)
    أرجع CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  إذا((state->state == TFTP_STATE_FIN) || result)
    أرجع result;

  result = tftp_multi_statemach(data, dophase_done);

  إذا(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  أرجع result;
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

  إذا(!state) {
    result = tftp_connect(data, done);
    إذا(result)
      أرجع result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    إذا(!state)
      أرجع CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  إذا(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  أرجع result;
}

ساكن CURLcode tftp_setup_connection(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  حرف *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  إذا((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  وإلا إذا((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  أرجع CURLE_OK;
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
