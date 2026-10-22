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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "cf-socket.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "sockaddr.h" /* required for Curl_sockaddr_storage */
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "strcase.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "curlx/strerr.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

প্রকারনির্দেশ গণনা {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

প্রকারনির্দেশ গণনা {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

প্রকারনির্দেশ গণনা {
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

প্রকারনির্দেশ গণনা {
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

সংরচনা tftp_packet {
  unsigned ৱর্ণ *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

সংরচনা tftp_conn {
  সংরচনা Curl_sockaddr_storage local_addr;
  সংরচনা Curl_sockaddr_storage remote_addr;
  সংরচনা tftp_packet rpacket;
  সংরচনা tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  সংরচনা Curl_easy *data;
  curl_socket_t   sockfd;
  পূর্ণাঙ্ক             retries;
  পূর্ণাঙ্ক             retry_time;
  পূর্ণাঙ্ক             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  পূর্ণাঙ্ক             rbytes;
  size_t          sbytes;
  unsigned পূর্ণাঙ্ক    blksize;
  unsigned পূর্ণাঙ্ক    requested_blksize;
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
স্থির CURLcode tftp_set_timeouts(সংরচনা tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  যদি(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  যদি((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  অন্যথা
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (পূর্ণাঙ্ক)timeout / 5;

  /* Bound the total number */
  যদি(state->retry_max < 3)
    state->retry_max = 3;

  যদি(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (পূর্ণাঙ্ক)(timeout / state->retry_max);
  যদি(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (পূর্ণাঙ্ক)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(রিক্ত);

  ঘূৰাই CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

স্থির শূন্য setpacketevent(সংরচনা tftp_packet *packet, unsigned short অঙ্ক)
{
  packet->data[0] = (unsigned ৱর্ণ)(অঙ্ক >> 8);
  packet->data[1] = (unsigned ৱর্ণ)(অঙ্ক & 0xff);
}

স্থির শূন্য setpacketblock(সংরচনা tftp_packet *packet, unsigned short অঙ্ক)
{
  packet->data[2] = (unsigned ৱর্ণ)(অঙ্ক >> 8);
  packet->data[3] = (unsigned ৱর্ণ)(অঙ্ক & 0xff);
}

স্থির unsigned short getrpacketevent(ধ্রুৱ সংরচনা tftp_packet *packet)
{
  ঘূৰাই (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

স্থির unsigned short getrpacketblock(ধ্রুৱ সংরচনা tftp_packet *packet)
{
  ঘূৰাই (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

স্থির size_t tftp_strnlen(ধ্রুৱ ৱর্ণ *string, size_t maxlen)
{
  ধ্রুৱ ৱর্ণ *end = memchr(string, '\0', maxlen);
  ঘূৰাই end ? (size_t)(end - string) : maxlen;
}

স্থির ধ্রুৱ ৱর্ণ *tftp_option_get(ধ্রুৱ ৱর্ণ *buf, size_t len,
                                   ধ্রুৱ ৱর্ণ **option, ধ্রুৱ ৱর্ণ **মান)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  যদি(loc >= len)
    ঘূৰাই রিক্ত;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  যদি(loc > len)
    ঘূৰাই রিক্ত;
  *মান = &buf[strlen(*option) + 1];

  ঘূৰাই &buf[loc];
}

স্থির CURLcode tftp_parse_option_ack(সংরচনা tftp_conn *state,
                                      ধ্রুৱ ৱর্ণ *ptr, পূর্ণাঙ্ক len)
{
  ধ্রুৱ ৱর্ণ *tmp = ptr;
  সংরচনা Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  যেতিয়ালৈ(tmp < ptr + len) {
    ধ্রুৱ ৱর্ণ *option, *মান;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &মান);
    যদি(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      ঘূৰাই CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, মান);

    যদি((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      যদি(curlx_str_সংখ্যা(&মান, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      }
      যদি(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      }
      অন্যথা যদি(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      }
      অন্যথা যদি(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned পূর্ণাঙ্ক)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    অন্যথা যদি((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      যদি(!data->state.upload &&
         !curlx_str_সংখ্যা(&মান, &tsize, CURL_OFF_T_MAX)) {
        যদি(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", মান);
          ঘূৰাই CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode tftp_option_add(সংরচনা tftp_conn *state, size_t *csize,
                                size_t index, ধ্রুৱ ৱর্ণ *option)
{
  ৱর্ণ *buf = (ৱর্ণ *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  যদি((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    ঘূৰাই CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ঘূৰাই CURLE_OK;
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
স্থির CURLcode tftp_tx(সংরচনা tftp_conn *state, tftp_event_t event)
{
  সংরচনা Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  সংরচনা SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  ৱর্ণ buffer[STRERROR_LEN];
  ৱর্ণ *bufptr;
  bool eos;

  যদি(!data->state.upload)
    ঘূৰাই CURLE_TFTP_ILLEGAL;

  ৱিকল্প(event) {

  পক্ষ TFTP_EVENT_ACK:
  পক্ষ TFTP_EVENT_OACK:
    যদি(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      পূর্ণাঙ্ক rblock = getrpacketblock(&state->rpacket);

      যদি(rblock != state->block &&
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
        যদি(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        অন্যথা {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (সংরচনা sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          যদি(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        ঘূৰাই result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(রিক্ত);
      state->block++;
    }
    অন্যথা
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    যদি(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ঘূৰাই CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (ৱর্ণ *)state->spacket.data + 4;
    কৰা {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      যদি(result)
        ঘূৰাই result;
      state->sbytes += cb;
      bufptr += cb;
    } যেতিয়ালৈ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (সংরচনা sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    যদি(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
      ঘূৰাই CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    ভঙ্গ;

  পক্ষ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (সংরচনা sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      যদি(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
        ঘূৰাই CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    ভঙ্গ;

  পক্ষ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (শূন্য)sendto(state->sockfd, (শূন্য *)state->spacket.data, 4, SEND_4TH_ARG,
                 (সংরচনা sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ভঙ্গ;

  স্ৱভাৱ:
    failf(data, "tftp_tx: internal error, event: %d", (পূর্ণাঙ্ক)event);
    ভঙ্গ;
  }

  ঘূৰাই result;
}

স্থির CURLcode tftp_connect_for_tx(সংরচনা tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  যদি(result)
    ঘূৰাই result;
  ঘূৰাই tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
স্থির CURLcode tftp_rx(সংরচনা tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  পূর্ণাঙ্ক rblock;
  সংরচনা Curl_easy *data = state->data;
  ৱর্ণ buffer[STRERROR_LEN];

  ৱিকল্প(event) {

  পক্ষ TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    যদি(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    অন্যথা যদি(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    অন্যথা {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      ভঙ্গ;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (সংরচনা sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    যদি(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
      ঘূৰাই CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    যদি(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(রিক্ত);
    ভঙ্গ;

  পক্ষ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (সংরচনা sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    যদি(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
      ঘূৰাই CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(রিক্ত);
    ভঙ্গ;

  পক্ষ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (সংরচনা sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      যদি(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
        ঘূৰাই CURLE_SEND_ERROR;
      }
    }
    ভঙ্গ;

  পক্ষ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (শূন্য)sendto(state->sockfd, (শূন্য *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (সংরচনা sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ভঙ্গ;

  স্ৱভাৱ:
    failf(data, "%s", "tftp_rx: internal error");
    ঘূৰাই CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode tftp_connect_for_rx(সংরচনা tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  যদি(result)
    ঘূৰাই result;
  ঘূৰাই tftp_rx(state, event);
}

স্থির CURLcode tftp_send_first(সংরচনা tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ধ্রুৱ ৱর্ণ *mode = "octet";
  ৱর্ণ *filename;
  সংরচনা Curl_easy *data = state->data;
  ধ্রুৱ সংরচনা Curl_sockaddr_ex *remote_addr = রিক্ত;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  যদি(data->state.prefer_ascii)
    mode = "netascii";

  ৱিকল্প(event) {

  পক্ষ TFTP_EVENT_INIT:    /* Send the first packet out */
  পক্ষ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ঘূৰাই result;
    }

    যদি(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      যদি(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    অন্যথা {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    যদি(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      ঘূৰাই CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, রিক্ত, REJECT_ZERO);
    যদি(result)
      ঘূৰাই result;

    যদি(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      ঘূৰাই CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((ৱর্ণ *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    যদি(!data->set.tftp_no_options) {
      ৱর্ণ buf[64];
      /* add tsize option */
      curl_msnprintf(buf, পরিমাণ(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      যদি(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, পরিমাণ(buf), "%u", state->requested_blksize);
      যদি(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      যদি(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, পরিমাণ(buf), "%d", state->retry_time);
      যদি(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      যদি(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      যদি(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#অন্যথা
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    যদি(!remote_addr)
      ঘূৰাই CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (শূন্য *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    যদি(senddata != (ssize_t)sbytes) {
      ৱর্ণ buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
      ঘূৰাই CURLE_SEND_ERROR;
    }
    ভঙ্গ;

  পক্ষ TFTP_EVENT_OACK:
    যদি(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    অন্যথা {
      result = tftp_connect_for_rx(state, event);
    }
    ভঙ্গ;

  পক্ষ TFTP_EVENT_ACK: /* Connected for transmit */
    যদি(!data->state.upload)
      ঘূৰাই CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    ভঙ্গ;

  পক্ষ TFTP_EVENT_DATA: /* Connected for receive */
    যদি(data->state.upload)
      ঘূৰাই CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    ভঙ্গ;

  পক্ষ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ভঙ্গ;

  স্ৱভাৱ:
    failf(state->data, "tftp_send_first: internal error");
    ঘূৰাই CURLE_TFTP_ILLEGAL;
  }

  ঘূৰাই result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
স্থির CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  যদি(error != TFTP_ERR_NONE) {
    ৱিকল্প(error) {
    পক্ষ TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      ভঙ্গ;
    পক্ষ TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      ভঙ্গ;
    পক্ষ TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      ভঙ্গ;
    পক্ষ TFTP_ERR_UNDEF:
    পক্ষ TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      ভঙ্গ;
    পক্ষ TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      ভঙ্গ;
    পক্ষ TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      ভঙ্গ;
    পক্ষ TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      ভঙ্গ;
    পক্ষ TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    পক্ষ TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      ভঙ্গ;
    স্ৱভাৱ:
      result = CURLE_ABORTED_BY_CALLBACK;
      ভঙ্গ;
    }
  }
  অন্যথা
    result = CURLE_OK;

  ঘূৰাই result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
স্থির CURLcode tftp_state_machine(সংরচনা tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  সংরচনা Curl_easy *data = state->data;

  ৱিকল্প(state->state) {
  পক্ষ TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    ভঙ্গ;
  পক্ষ TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    ভঙ্গ;
  পক্ষ TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    ভঙ্গ;
  পক্ষ TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    ভঙ্গ;
  স্ৱভাৱ:
    DEBUGF(infof(data, "STATE: %d", (পূর্ণাঙ্ক)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    ভঙ্গ;
  }

  ঘূৰাই result;
}

স্থির শূন্য tftp_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা tftp_conn *state = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
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
স্থির CURLcode tftp_connect(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা tftp_conn *state;
  পূর্ণাঙ্ক blksize;
  পূর্ণাঙ্ক need_blksize;
  সংরচনা connectdata *conn = data->conn;
  ধ্রুৱ সংরচনা Curl_sockaddr_ex *remote_addr = রিক্ত;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, পরিমাণ(*state));
  যদি(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  যদি(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (পূর্ণাঙ্ক)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  যদি(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  যদি(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    যদি(!state->rpacket.data)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  যদি(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    যদি(!state->spacket.data)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
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
  যদি(!remote_addr)
    ঘূৰাই CURLE_FAILED_INIT;

  ((সংরচনা sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  যদি(result)
    ঘূৰাই result;

  যদি(!conn->bits.bound) {
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
    পূর্ণাঙ্ক rc = bind(state->sockfd, (সংরচনা sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    যদি(rc) {
      ৱর্ণ buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
      ঘূৰাই CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ঘূৰাই CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
স্থির CURLcode tftp_done(সংরচনা Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (শূন্য)status;
  (শূন্য)premature;

  যদি(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ঘূৰাই CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  যদি(state)
    result = tftp_translate_code(state->error);

  ঘূৰাই result;
}

স্থির CURLcode tftp_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  ঘূৰাই Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
স্থির CURLcode tftp_receive_packet(সংরচনা Curl_easy *data,
                                    সংরচনা tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  সংরচনা Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = পরিমাণ(remote_addr);

  /* Receive the packet */
  state->rbytes = (পূর্ণাঙ্ক)recvfrom(state->sockfd,
                                (শূন্য *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (সংরচনা sockaddr *)&remote_addr,
                                &fromlen);
  যদি((state->rbytes >= 0) && fromlen) {
    যদি(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      যদি((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        ঘূৰাই CURLE_RECV_ERROR;
      }
    }
    অন্যথা {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  যদি(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  অন্যথা {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ৱিকল্প(state->event) {
    পক্ষ TFTP_EVENT_DATA:
      যদি(data->state.upload)
        ঘূৰাই CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      যদি(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ধ্রুৱ ৱর্ণ *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        যদি(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ঘূৰাই result;
        }
      }
      ভঙ্গ;
    পক্ষ TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ধ্রুৱ ৱর্ণ *str = (ধ্রুৱ ৱর্ণ *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      যদি(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      ভঙ্গ;
    }
    পক্ষ TFTP_EVENT_ACK:
      ভঙ্গ;
    পক্ষ TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ধ্রুৱ ৱর্ণ *)state->rpacket.data + 2,
                                     state->rbytes-2);
      যদি(result)
        ঘূৰাই result;
      ভঙ্গ;
    পক্ষ TFTP_EVENT_RRQ:
    পক্ষ TFTP_EVENT_WRQ:
    স্ৱভাৱ:
      failf(data, "%s", "Internal error: Unexpected packet");
      ভঙ্গ;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    যদি(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ঘূৰাই result;
    }
  }
  ঘূৰাই result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
স্থির timediff_t tftp_state_timeout(সংরচনা tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  যদি(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  যদি(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ঘূৰাই timeout_ms;
  }
  current = time(রিক্ত);
  যদি(current > state->rx_time + state->retry_time) {
    যদি(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(রিক্ত); /* update even though we received nothing */
  }

  ঘূৰাই timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
স্থির CURLcode tftp_multi_statemach(সংরচনা Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  যদি(!state)
    ঘূৰাই CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  যদি(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;
  }
  যদি(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    যদি(result)
      ঘূৰাই result;
    *done = (state->state == TFTP_STATE_FIN);
    যদি(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  অন্যথা {
    /* no timeouts to handle, check our socket */
    পূর্ণাঙ্ক rc = SOCKET_READABLE(state->sockfd, 0);

    যদি(rc == -1) {
      /* bail out */
      পূর্ণাঙ্ক sockerr = SOCKERRNO;
      ৱর্ণ buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, পরিমাণ(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    অন্যথা যদি(rc) {
      result = tftp_receive_packet(data, state);
      যদি(result)
        ঘূৰাই result;
      result = tftp_state_machine(state, state->event);
      যদি(result)
        ঘূৰাই result;
      *done = (state->state == TFTP_STATE_FIN);
      যদি(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  ঘূৰাই result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
স্থির CURLcode tftp_doing(সংরচনা Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  যদি(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  অন্যথা যদি(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  ঘূৰাই result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
স্থির CURLcode tftp_perform(সংরচনা Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  যদি(!state)
    ঘূৰাই CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  যদি((state->state == TFTP_STATE_FIN) || result)
    ঘূৰাই result;

  result = tftp_multi_statemach(data, dophase_done);

  যদি(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ঘূৰাই result;
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

স্থির CURLcode tftp_do(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  যদি(!state) {
    result = tftp_connect(data, done);
    যদি(result)
      ঘূৰাই result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    যদি(!state)
      ঘূৰাই CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  যদি(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  ঘূৰাই result;
}

স্থির CURLcode tftp_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  ৱর্ণ *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  যদি((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  অন্যথা যদি((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ঘূৰাই CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_tftp = {
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
