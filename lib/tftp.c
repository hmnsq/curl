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

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

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
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

تعریفنوع شمارش {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

تعریفنوع شمارش {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

تعریفنوع شمارش {
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

تعریفنوع شمارش {
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

ساختار tftp_packet {
  unsigned نویسه *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ساختار tftp_conn {
  ساختار Curl_sockaddr_storage local_addr;
  ساختار Curl_sockaddr_storage remote_addr;
  ساختار tftp_packet rpacket;
  ساختار tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  ساختار Curl_easy *data;
  curl_socket_t   sockfd;
  صحیح             retries;
  صحیح             retry_time;
  صحیح             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  صحیح             rbytes;
  size_t          sbytes;
  unsigned صحیح    blksize;
  unsigned صحیح    requested_blksize;
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
ایستا CURLcode tftp_set_timeouts(ساختار tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  اگر(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  اگر((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  وگرنه
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (صحیح)timeout / 5;

  /* Bound the total number */
  اگر(state->retry_max < 3)
    state->retry_max = 3;

  اگر(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (صحیح)(timeout / state->retry_max);
  اگر(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (صحیح)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(پوچ);

  بازگشت CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ایستا تهی setpacketevent(ساختار tftp_packet *packet, unsigned short رقم)
{
  packet->data[0] = (unsigned نویسه)(رقم >> 8);
  packet->data[1] = (unsigned نویسه)(رقم & 0xff);
}

ایستا تهی setpacketblock(ساختار tftp_packet *packet, unsigned short رقم)
{
  packet->data[2] = (unsigned نویسه)(رقم >> 8);
  packet->data[3] = (unsigned نویسه)(رقم & 0xff);
}

ایستا unsigned short getrpacketevent(ثابت ساختار tftp_packet *packet)
{
  بازگشت (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

ایستا unsigned short getrpacketblock(ثابت ساختار tftp_packet *packet)
{
  بازگشت (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

ایستا size_t tftp_strnlen(ثابت نویسه *string, size_t maxlen)
{
  ثابت نویسه *end = memchr(string, '\0', maxlen);
  بازگشت end ? (size_t)(end - string) : maxlen;
}

ایستا ثابت نویسه *tftp_option_get(ثابت نویسه *buf, size_t len,
                                   ثابت نویسه **option, ثابت نویسه **مقدار)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  اگر(loc >= len)
    بازگشت پوچ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  اگر(loc > len)
    بازگشت پوچ;
  *مقدار = &buf[strlen(*option) + 1];

  بازگشت &buf[loc];
}

ایستا CURLcode tftp_parse_option_ack(ساختار tftp_conn *state,
                                      ثابت نویسه *ptr, صحیح len)
{
  ثابت نویسه *tmp = ptr;
  ساختار Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  وقتی(tmp < ptr + len) {
    ثابت نویسه *option, *مقدار;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &مقدار);
    اگر(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      بازگشت CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, مقدار);

    اگر((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      اگر(curlx_str_عدد(&مقدار, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        بازگشت CURLE_TFTP_ILLEGAL;
      }
      اگر(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        بازگشت CURLE_TFTP_ILLEGAL;
      }
      وگرنه اگر(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        بازگشت CURLE_TFTP_ILLEGAL;
      }
      وگرنه اگر(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        بازگشت CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned صحیح)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    وگرنه اگر((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      اگر(!data->state.upload &&
         !curlx_str_عدد(&مقدار, &tsize, CURL_OFF_T_MAX)) {
        اگر(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", مقدار);
          بازگشت CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode tftp_option_add(ساختار tftp_conn *state, size_t *csize,
                                size_t index, ثابت نویسه *option)
{
  نویسه *buf = (نویسه *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  اگر((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    بازگشت CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  بازگشت CURLE_OK;
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
ایستا CURLcode tftp_tx(ساختار tftp_conn *state, tftp_event_t event)
{
  ساختار Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  ساختار SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  نویسه buffer[STRERROR_LEN];
  نویسه *bufptr;
  bool eos;

  اگر(!data->state.upload)
    بازگشت CURLE_TFTP_ILLEGAL;

  انتخاب(event) {

  حالت TFTP_EVENT_ACK:
  حالت TFTP_EVENT_OACK:
    اگر(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      صحیح rblock = getrpacketblock(&state->rpacket);

      اگر(rblock != state->block &&
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
        اگر(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        وگرنه {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ساختار sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          اگر(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        بازگشت result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(پوچ);
      state->block++;
    }
    وگرنه
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    اگر(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      بازگشت CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (نویسه *)state->spacket.data + 4;
    انجامبده {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      اگر(result)
        بازگشت result;
      state->sbytes += cb;
      bufptr += cb;
    } وقتی(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ساختار sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    اگر(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
      بازگشت CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    بشکن;

  حالت TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وگرنه {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ساختار sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      اگر(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
        بازگشت CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    بشکن;

  حالت TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (تهی)sendto(state->sockfd, (تهی *)state->spacket.data, 4, SEND_4TH_ARG,
                 (ساختار sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    بشکن;

  پیشفرض:
    failf(data, "tftp_tx: internal error, event: %d", (صحیح)event);
    بشکن;
  }

  بازگشت result;
}

ایستا CURLcode tftp_connect_for_tx(ساختار tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  اگر(result)
    بازگشت result;
  بازگشت tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ایستا CURLcode tftp_rx(ساختار tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  صحیح rblock;
  ساختار Curl_easy *data = state->data;
  نویسه buffer[STRERROR_LEN];

  انتخاب(event) {

  حالت TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    اگر(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    وگرنه اگر(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    وگرنه {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      بشکن;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ساختار sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    اگر(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
      بازگشت CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    اگر(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    وگرنه {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(پوچ);
    بشکن;

  حالت TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ساختار sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    اگر(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
      بازگشت CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(پوچ);
    بشکن;

  حالت TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    وگرنه {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (تهی *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (ساختار sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      اگر(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
        بازگشت CURLE_SEND_ERROR;
      }
    }
    بشکن;

  حالت TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (تهی)sendto(state->sockfd, (تهی *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (ساختار sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    بشکن;

  پیشفرض:
    failf(data, "%s", "tftp_rx: internal error");
    بازگشت CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode tftp_connect_for_rx(ساختار tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  اگر(result)
    بازگشت result;
  بازگشت tftp_rx(state, event);
}

ایستا CURLcode tftp_send_first(ساختار tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ثابت نویسه *mode = "octet";
  نویسه *filename;
  ساختار Curl_easy *data = state->data;
  ثابت ساختار Curl_sockaddr_ex *remote_addr = پوچ;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  اگر(data->state.prefer_ascii)
    mode = "netascii";

  انتخاب(event) {

  حالت TFTP_EVENT_INIT:    /* Send the first packet out */
  حالت TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    اگر(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      بازگشت result;
    }

    اگر(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      اگر(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    وگرنه {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    اگر(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      بازگشت CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, پوچ, REJECT_ZERO);
    اگر(result)
      بازگشت result;

    اگر(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      بازگشت CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((نویسه *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    اگر(!data->set.tftp_no_options) {
      نویسه buf[64];
      /* add tsize option */
      curl_msnprintf(buf, اندازه(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      اگر(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, اندازه(buf), "%u", state->requested_blksize);
      اگر(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      اگر(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, اندازه(buf), "%d", state->retry_time);
      اگر(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      اگر(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      اگر(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        بازگشت CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#وگرنه
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    اگر(!remote_addr)
      بازگشت CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (تهی *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    اگر(senddata != (ssize_t)sbytes) {
      نویسه buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
      بازگشت CURLE_SEND_ERROR;
    }
    بشکن;

  حالت TFTP_EVENT_OACK:
    اگر(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    وگرنه {
      result = tftp_connect_for_rx(state, event);
    }
    بشکن;

  حالت TFTP_EVENT_ACK: /* Connected for transmit */
    اگر(!data->state.upload)
      بازگشت CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    بشکن;

  حالت TFTP_EVENT_DATA: /* Connected for receive */
    اگر(data->state.upload)
      بازگشت CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    بشکن;

  حالت TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    بشکن;

  پیشفرض:
    failf(state->data, "tftp_send_first: internal error");
    بازگشت CURLE_TFTP_ILLEGAL;
  }

  بازگشت result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ایستا CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  اگر(error != TFTP_ERR_NONE) {
    انتخاب(error) {
    حالت TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      بشکن;
    حالت TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      بشکن;
    حالت TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      بشکن;
    حالت TFTP_ERR_UNDEF:
    حالت TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      بشکن;
    حالت TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      بشکن;
    حالت TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      بشکن;
    حالت TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      بشکن;
    حالت TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    حالت TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      بشکن;
    پیشفرض:
      result = CURLE_ABORTED_BY_CALLBACK;
      بشکن;
    }
  }
  وگرنه
    result = CURLE_OK;

  بازگشت result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ایستا CURLcode tftp_state_machine(ساختار tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  ساختار Curl_easy *data = state->data;

  انتخاب(state->state) {
  حالت TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    بشکن;
  حالت TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    بشکن;
  حالت TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    بشکن;
  حالت TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    بشکن;
  پیشفرض:
    DEBUGF(infof(data, "STATE: %d", (صحیح)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    بشکن;
  }

  بازگشت result;
}

ایستا تهی tftp_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار tftp_conn *state = entry;
  (تهی)کلید;
  (تهی)klen;
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
ایستا CURLcode tftp_connect(ساختار Curl_easy *data, bool *done)
{
  ساختار tftp_conn *state;
  صحیح blksize;
  صحیح need_blksize;
  ساختار connectdata *conn = data->conn;
  ثابت ساختار Curl_sockaddr_ex *remote_addr = پوچ;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, اندازه(*state));
  اگر(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  اگر(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (صحیح)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  اگر(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  اگر(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    اگر(!state->rpacket.data)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    اگر(!state->spacket.data)
      بازگشت CURLE_OUT_OF_MEMORY;
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
  اگر(!remote_addr)
    بازگشت CURLE_FAILED_INIT;

  ((ساختار sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  اگر(result)
    بازگشت result;

  اگر(!conn->bits.bound) {
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
    صحیح rc = bind(state->sockfd, (ساختار sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    اگر(rc) {
      نویسه buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
      بازگشت CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  بازگشت CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ایستا CURLcode tftp_done(ساختار Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (تهی)status;
  (تهی)premature;

  اگر(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    بازگشت CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  اگر(state)
    result = tftp_translate_code(state->error);

  بازگشت result;
}

ایستا CURLcode tftp_pollset(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps)
{
  بازگشت Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ایستا CURLcode tftp_receive_packet(ساختار Curl_easy *data,
                                    ساختار tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  ساختار Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = اندازه(remote_addr);

  /* Receive the packet */
  state->rbytes = (صحیح)recvfrom(state->sockfd,
                                (تهی *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ساختار sockaddr *)&remote_addr,
                                &fromlen);
  اگر((state->rbytes >= 0) && fromlen) {
    اگر(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      اگر((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        بازگشت CURLE_RECV_ERROR;
      }
    }
    وگرنه {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  اگر(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  وگرنه {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    انتخاب(state->event) {
    حالت TFTP_EVENT_DATA:
      اگر(data->state.upload)
        بازگشت CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      اگر(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ثابت نویسه *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        اگر(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          بازگشت result;
        }
      }
      بشکن;
    حالت TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ثابت نویسه *str = (ثابت نویسه *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      اگر(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      بشکن;
    }
    حالت TFTP_EVENT_ACK:
      بشکن;
    حالت TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ثابت نویسه *)state->rpacket.data + 2,
                                     state->rbytes-2);
      اگر(result)
        بازگشت result;
      بشکن;
    حالت TFTP_EVENT_RRQ:
    حالت TFTP_EVENT_WRQ:
    پیشفرض:
      failf(data, "%s", "Internal error: Unexpected packet");
      بشکن;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    اگر(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      بازگشت result;
    }
  }
  بازگشت result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ایستا timediff_t tftp_state_timeout(ساختار tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  اگر(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  اگر(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    بازگشت timeout_ms;
  }
  current = time(پوچ);
  اگر(current > state->rx_time + state->retry_time) {
    اگر(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(پوچ); /* update even though we received nothing */
  }

  بازگشت timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ایستا CURLcode tftp_multi_statemach(ساختار Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  اگر(!state)
    بازگشت CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  اگر(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }
  اگر(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    اگر(result)
      بازگشت result;
    *done = (state->state == TFTP_STATE_FIN);
    اگر(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  وگرنه {
    /* no timeouts to handle, check our socket */
    صحیح rc = SOCKET_READABLE(state->sockfd, 0);

    اگر(rc == -1) {
      /* bail out */
      صحیح sockerr = SOCKERRNO;
      نویسه buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, اندازه(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    وگرنه اگر(rc) {
      result = tftp_receive_packet(data, state);
      اگر(result)
        بازگشت result;
      result = tftp_state_machine(state, state->event);
      اگر(result)
        بازگشت result;
      *done = (state->state == TFTP_STATE_FIN);
      اگر(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  بازگشت result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ایستا CURLcode tftp_doing(ساختار Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  اگر(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  وگرنه اگر(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  بازگشت result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ایستا CURLcode tftp_perform(ساختار Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  اگر(!state)
    بازگشت CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  اگر((state->state == TFTP_STATE_FIN) || result)
    بازگشت result;

  result = tftp_multi_statemach(data, dophase_done);

  اگر(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  بازگشت result;
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

ایستا CURLcode tftp_do(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  ساختار tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  اگر(!state) {
    result = tftp_connect(data, done);
    اگر(result)
      بازگشت result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    اگر(!state)
      بازگشت CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  اگر(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  بازگشت result;
}

ایستا CURLcode tftp_setup_connection(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  نویسه *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  اگر((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  وگرنه اگر((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  بازگشت CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ثابت ساختار Curl_protocol Curl_protocol_tftp = {
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
