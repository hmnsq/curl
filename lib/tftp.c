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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಸಂರಚನಾ tftp_packet {
  unsigned ವರ್ಣ *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ಸಂರಚನಾ tftp_conn {
  ಸಂರಚನಾ Curl_sockaddr_storage local_addr;
  ಸಂರಚನಾ Curl_sockaddr_storage remote_addr;
  ಸಂರಚನಾ tftp_packet rpacket;
  ಸಂರಚನಾ tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  ಸಂರಚನಾ Curl_easy *data;
  curl_socket_t   sockfd;
  ಪೂರ್ಣಾಙ್ಕ             retries;
  ಪೂರ್ಣಾಙ್ಕ             retry_time;
  ಪೂರ್ಣಾಙ್ಕ             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  ಪೂರ್ಣಾಙ್ಕ             rbytes;
  size_t          sbytes;
  unsigned ಪೂರ್ಣಾಙ್ಕ    blksize;
  unsigned ಪೂರ್ಣಾಙ್ಕ    requested_blksize;
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
ಸ್ಥಿರ CURLcode tftp_set_timeouts(ಸಂರಚನಾ tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  ಯದಿ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ಯದಿ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ಅನ್ಯಥಾ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (ಪೂರ್ಣಾಙ್ಕ)timeout / 5;

  /* Bound the total number */
  ಯದಿ(state->retry_max < 3)
    state->retry_max = 3;

  ಯದಿ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (ಪೂರ್ಣಾಙ್ಕ)(timeout / state->retry_max);
  ಯದಿ(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (ಪೂರ್ಣಾಙ್ಕ)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ಸ್ಥಿರ ಶೂನ್ಯ setpacketevent(ಸಂರಚನಾ tftp_packet *packet, unsigned short ಅಂಕೆ)
{
  packet->data[0] = (unsigned ವರ್ಣ)(ಅಂಕೆ >> 8);
  packet->data[1] = (unsigned ವರ್ಣ)(ಅಂಕೆ & 0xff);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpacketblock(ಸಂರಚನಾ tftp_packet *packet, unsigned short ಅಂಕೆ)
{
  packet->data[2] = (unsigned ವರ್ಣ)(ಅಂಕೆ >> 8);
  packet->data[3] = (unsigned ವರ್ಣ)(ಅಂಕೆ & 0xff);
}

ಸ್ಥಿರ unsigned short getrpacketevent(ಧ್ರುವ ಸಂರಚನಾ tftp_packet *packet)
{
  ಪ್ರತ್ಯಾಗಮ (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

ಸ್ಥಿರ unsigned short getrpacketblock(ಧ್ರುವ ಸಂರಚನಾ tftp_packet *packet)
{
  ಪ್ರತ್ಯಾಗಮ (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

ಸ್ಥಿರ size_t tftp_strnlen(ಧ್ರುವ ವರ್ಣ *string, size_t maxlen)
{
  ಧ್ರುವ ವರ್ಣ *end = memchr(string, '\0', maxlen);
  ಪ್ರತ್ಯಾಗಮ end ? (size_t)(end - string) : maxlen;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *tftp_option_get(ಧ್ರುವ ವರ್ಣ *buf, size_t len,
                                   ಧ್ರುವ ವರ್ಣ **option, ಧ್ರುವ ವರ್ಣ **ಮೌಲ್ಯ)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ಯದಿ(loc >= len)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ಯದಿ(loc > len)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  *ಮೌಲ್ಯ = &buf[strlen(*option) + 1];

  ಪ್ರತ್ಯಾಗಮ &buf[loc];
}

ಸ್ಥಿರ CURLcode tftp_parse_option_ack(ಸಂರಚನಾ tftp_conn *state,
                                      ಧ್ರುವ ವರ್ಣ *ptr, ಪೂರ್ಣಾಙ್ಕ len)
{
  ಧ್ರುವ ವರ್ಣ *tmp = ptr;
  ಸಂರಚನಾ Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ಯಾವತ್(tmp < ptr + len) {
    ಧ್ರುವ ವರ್ಣ *option, *ಮೌಲ್ಯ;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &ಮೌಲ್ಯ);
    ಯದಿ(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, ಮೌಲ್ಯ);

    ಯದಿ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&ಮೌಲ್ಯ, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      }
      ಯದಿ(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      }
      ಅನ್ಯಥಾ ಯದಿ(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      }
      ಅನ್ಯಥಾ ಯದಿ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned ಪೂರ್ಣಾಙ್ಕ)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ಅನ್ಯಥಾ ಯದಿ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ಯದಿ(!data->state.upload &&
         !curlx_str_ಸಂಖ್ಯೆ(&ಮೌಲ್ಯ, &tsize, CURL_OFF_T_MAX)) {
        ಯದಿ(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", ಮೌಲ್ಯ);
          ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tftp_option_add(ಸಂರಚನಾ tftp_conn *state, size_t *csize,
                                size_t index, ಧ್ರುವ ವರ್ಣ *option)
{
  ವರ್ಣ *buf = (ವರ್ಣ *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  ಯದಿ((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode tftp_tx(ಸಂರಚನಾ tftp_conn *state, tftp_event_t event)
{
  ಸಂರಚನಾ Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  ವರ್ಣ buffer[STRERROR_LEN];
  ವರ್ಣ *bufptr;
  bool eos;

  ಯದಿ(!data->state.upload)
    ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;

  ವಿಕಲ್ಪ(event) {

  ಪಕ್ಷ TFTP_EVENT_ACK:
  ಪಕ್ಷ TFTP_EVENT_OACK:
    ಯದಿ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      ಪೂರ್ಣಾಙ್ಕ rblock = getrpacketblock(&state->rpacket);

      ಯದಿ(rblock != state->block &&
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
        ಯದಿ(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        ಅನ್ಯಥಾ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ಯದಿ(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        ಪ್ರತ್ಯಾಗಮ result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(ರಿಕ್ತ);
      state->block++;
    }
    ಅನ್ಯಥಾ
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    ಯದಿ(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (ವರ್ಣ *)state->spacket.data + 4;
    ಕುರು {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      state->sbytes += cb;
      bufptr += cb;
    } ಯಾವತ್(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ಯದಿ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    ಯದಿ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ಅನ್ಯಥಾ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ಯದಿ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ಶೂನ್ಯ)sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data, 4, SEND_4TH_ARG,
                 (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    failf(data, "tftp_tx: internal error, event: %d", (ಪೂರ್ಣಾಙ್ಕ)event);
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode tftp_connect_for_tx(ಸಂರಚನಾ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಪ್ರತ್ಯಾಗಮ tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_rx(ಸಂರಚನಾ tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  ಪೂರ್ಣಾಙ್ಕ rblock;
  ಸಂರಚನಾ Curl_easy *data = state->data;
  ವರ್ಣ buffer[STRERROR_LEN];

  ವಿಕಲ್ಪ(event) {

  ಪಕ್ಷ TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ಯದಿ(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    ಅನ್ಯಥಾ {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      ಭಙ್ಗ;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ಯದಿ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ಯದಿ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ಅನ್ಯಥಾ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(ರಿಕ್ತ);
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ಯದಿ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(ರಿಕ್ತ);
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    ಯದಿ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ಅನ್ಯಥಾ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ಯದಿ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ಶೂನ್ಯ)sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (ಸಂರಚನಾ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    failf(data, "%s", "tftp_rx: internal error");
    ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tftp_connect_for_rx(ಸಂರಚನಾ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಪ್ರತ್ಯಾಗಮ tftp_rx(state, event);
}

ಸ್ಥಿರ CURLcode tftp_send_first(ಸಂರಚನಾ tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ಧ್ರುವ ವರ್ಣ *mode = "octet";
  ವರ್ಣ *filename;
  ಸಂರಚನಾ Curl_easy *data = state->data;
  ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *remote_addr = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ಯದಿ(data->state.prefer_ascii)
    mode = "netascii";

  ವಿಕಲ್ಪ(event) {

  ಪಕ್ಷ TFTP_EVENT_INIT:    /* Send the first packet out */
  ಪಕ್ಷ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ಯದಿ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ಯದಿ(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    ಅನ್ಯಥಾ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ಯದಿ(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, ರಿಕ್ತ, REJECT_ZERO);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((ವರ್ಣ *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ಯದಿ(!data->set.tftp_no_options) {
      ವರ್ಣ buf[64];
      /* add tsize option */
      curl_msnprintf(buf, ಪರಿಮಾಣ(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ಯದಿ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, ಪರಿಮಾಣ(buf), "%u", state->requested_blksize);
      ಯದಿ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ಯದಿ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, ಪರಿಮಾಣ(buf), "%d", state->retry_time);
      ಯದಿ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ಯದಿ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      ಯದಿ(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ಅನ್ಯಥಾ
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    ಯದಿ(!remote_addr)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ಯದಿ(senddata != (ssize_t)sbytes) {
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_OACK:
    ಯದಿ(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    ಅನ್ಯಥಾ {
      result = tftp_connect_for_rx(state, event);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_ACK: /* Connected for transmit */
    ಯದಿ(!data->state.upload)
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_DATA: /* Connected for receive */
    ಯದಿ(data->state.upload)
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    ಭಙ್ಗ;

  ಪಕ್ಷ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    failf(state->data, "tftp_send_first: internal error");
    ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(error != TFTP_ERR_NONE) {
    ವಿಕಲ್ಪ(error) {
    ಪಕ್ಷ TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_UNDEF:
    ಪಕ್ಷ TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      result = CURLE_ABORTED_BY_CALLBACK;
      ಭಙ್ಗ;
    }
  }
  ಅನ್ಯಥಾ
    result = CURLE_OK;

  ಪ್ರತ್ಯಾಗಮ result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_state_machine(ಸಂರಚನಾ tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ Curl_easy *data = state->data;

  ವಿಕಲ್ಪ(state->state) {
  ಪಕ್ಷ TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    ಭಙ್ಗ;
  ಪಕ್ಷ TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    ಭಙ್ಗ;
  ಪಕ್ಷ TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    ಭಙ್ಗ;
  ಪಕ್ಷ TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    DEBUGF(infof(data, "STATE: %d", (ಪೂರ್ಣಾಙ್ಕ)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ tftp_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ tftp_conn *state = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
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
ಸ್ಥಿರ CURLcode tftp_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ tftp_conn *state;
  ಪೂರ್ಣಾಙ್ಕ blksize;
  ಪೂರ್ಣಾಙ್ಕ need_blksize;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *remote_addr = ರಿಕ್ತ;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, ಪರಿಮಾಣ(*state));
  ಯದಿ(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  ಯದಿ(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (ಪೂರ್ಣಾಙ್ಕ)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ಯದಿ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ಯದಿ(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ಯದಿ(!state->rpacket.data)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ಯದಿ(!state->spacket.data)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
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
  ಯದಿ(!remote_addr)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ((ಸಂರಚನಾ sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(!conn->bits.bound) {
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
    ಪೂರ್ಣಾಙ್ಕ rc = bind(state->sockfd, (ಸಂರಚನಾ sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    ಯದಿ(rc) {
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (ಶೂನ್ಯ)status;
  (ಶೂನ್ಯ)premature;

  ಯದಿ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ಯದಿ(state)
    result = tftp_translate_code(state->error);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode tftp_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_receive_packet(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = ಪರಿಮಾಣ(remote_addr);

  /* Receive the packet */
  state->rbytes = (ಪೂರ್ಣಾಙ್ಕ)recvfrom(state->sockfd,
                                (ಶೂನ್ಯ *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ಸಂರಚನಾ sockaddr *)&remote_addr,
                                &fromlen);
  ಯದಿ((state->rbytes >= 0) && fromlen) {
    ಯದಿ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ಯದಿ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
    }
    ಅನ್ಯಥಾ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ಯದಿ(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ಅನ್ಯಥಾ {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ವಿಕಲ್ಪ(state->event) {
    ಪಕ್ಷ TFTP_EVENT_DATA:
      ಯದಿ(data->state.upload)
        ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ಯದಿ(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ಧ್ರುವ ವರ್ಣ *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        ಯದಿ(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ಧ್ರುವ ವರ್ಣ *str = (ಧ್ರುವ ವರ್ಣ *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ಯದಿ(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ TFTP_EVENT_ACK:
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ಧ್ರುವ ವರ್ಣ *)state->rpacket.data + 2,
                                     state->rbytes-2);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      ಭಙ್ಗ;
    ಪಕ್ಷ TFTP_EVENT_RRQ:
    ಪಕ್ಷ TFTP_EVENT_WRQ:
    ಸ್ವಭಾವ:
      failf(data, "%s", "Internal error: Unexpected packet");
      ಭಙ್ಗ;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    ಯದಿ(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ಸ್ಥಿರ timediff_t tftp_state_timeout(ಸಂರಚನಾ tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  ಯದಿ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  ಯದಿ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ಪ್ರತ್ಯಾಗಮ timeout_ms;
  }
  current = time(ರಿಕ್ತ);
  ಯದಿ(current > state->rx_time + state->retry_time) {
    ಯದಿ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(ರಿಕ್ತ); /* update even though we received nothing */
  }

  ಪ್ರತ್ಯಾಗಮ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_multi_statemach(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  ಯದಿ(!state)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ಯದಿ(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }
  ಯದಿ(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    *done = (state->state == TFTP_STATE_FIN);
    ಯದಿ(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  ಅನ್ಯಥಾ {
    /* no timeouts to handle, check our socket */
    ಪೂರ್ಣಾಙ್ಕ rc = SOCKET_READABLE(state->sockfd, 0);

    ಯದಿ(rc == -1) {
      /* bail out */
      ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    ಅನ್ಯಥಾ ಯದಿ(rc) {
      result = tftp_receive_packet(data, state);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      result = tftp_state_machine(state, state->event);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      *done = (state->state == TFTP_STATE_FIN);
      ಯದಿ(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_doing(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  ಯದಿ(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  ಅನ್ಯಥಾ ಯದಿ(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_perform(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  ಯದಿ(!state)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  ಯದಿ((state->state == TFTP_STATE_FIN) || result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = tftp_multi_statemach(data, dophase_done);

  ಯದಿ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ಪ್ರತ್ಯಾಗಮ result;
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

ಸ್ಥಿರ CURLcode tftp_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  ಯದಿ(!state) {
    result = tftp_connect(data, done);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ಯದಿ(!state)
      ಪ್ರತ್ಯಾಗಮ CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ಯದಿ(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode tftp_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ವರ್ಣ *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ಯದಿ((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  ಅನ್ಯಥಾ ಯದಿ((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_tftp = {
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
