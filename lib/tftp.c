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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "sockaddr.h" /* required for Curl_sockaddr_storage */
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

வகைவரை எண்ணிக்கை {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

வகைவரை எண்ணிக்கை {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

வகைவரை எண்ணிக்கை {
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

வகைவரை எண்ணிக்கை {
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

கட்டமைப்பு tftp_packet {
  unsigned எழுத்து *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

கட்டமைப்பு tftp_conn {
  கட்டமைப்பு Curl_sockaddr_storage local_addr;
  கட்டமைப்பு Curl_sockaddr_storage remote_addr;
  கட்டமைப்பு tftp_packet rpacket;
  கட்டமைப்பு tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  கட்டமைப்பு Curl_easy *data;
  curl_socket_t   sockfd;
  முழு             retries;
  முழு             retry_time;
  முழு             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  முழு             rbytes;
  size_t          sbytes;
  unsigned முழு    blksize;
  unsigned முழு    requested_blksize;
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
நிலையான CURLcode tftp_set_timeouts(கட்டமைப்பு tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  எனில்(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  எனில்((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  இல்லையெனில்
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (முழு)timeout / 5;

  /* Bound the total number */
  எனில்(state->retry_max < 3)
    state->retry_max = 3;

  எனில்(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (முழு)(timeout / state->retry_max);
  எனில்(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (முழு)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(வெற்று);

  திரும்பு CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

நிலையான வெறுமை setpacketevent(கட்டமைப்பு tftp_packet *packet, unsigned short இலக்கம்)
{
  packet->data[0] = (unsigned எழுத்து)(இலக்கம் >> 8);
  packet->data[1] = (unsigned எழுத்து)(இலக்கம் & 0xff);
}

நிலையான வெறுமை setpacketblock(கட்டமைப்பு tftp_packet *packet, unsigned short இலக்கம்)
{
  packet->data[2] = (unsigned எழுத்து)(இலக்கம் >> 8);
  packet->data[3] = (unsigned எழுத்து)(இலக்கம் & 0xff);
}

நிலையான unsigned short getrpacketevent(மாறிலி கட்டமைப்பு tftp_packet *packet)
{
  திரும்பு (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

நிலையான unsigned short getrpacketblock(மாறிலி கட்டமைப்பு tftp_packet *packet)
{
  திரும்பு (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

நிலையான size_t tftp_strnlen(மாறிலி எழுத்து *string, size_t maxlen)
{
  மாறிலி எழுத்து *end = memchr(string, '\0', maxlen);
  திரும்பு end ? (size_t)(end - string) : maxlen;
}

நிலையான மாறிலி எழுத்து *tftp_option_get(மாறிலி எழுத்து *buf, size_t len,
                                   மாறிலி எழுத்து **option, மாறிலி எழுத்து **மதிப்பு)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  எனில்(loc >= len)
    திரும்பு வெற்று;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  எனில்(loc > len)
    திரும்பு வெற்று;
  *மதிப்பு = &buf[strlen(*option) + 1];

  திரும்பு &buf[loc];
}

நிலையான CURLcode tftp_parse_option_ack(கட்டமைப்பு tftp_conn *state,
                                      மாறிலி எழுத்து *ptr, முழு len)
{
  மாறிலி எழுத்து *tmp = ptr;
  கட்டமைப்பு Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  போது(tmp < ptr + len) {
    மாறிலி எழுத்து *option, *மதிப்பு;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &மதிப்பு);
    எனில்(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      திரும்பு CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, மதிப்பு);

    எனில்((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      எனில்(curlx_str_எண்(&மதிப்பு, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        திரும்பு CURLE_TFTP_ILLEGAL;
      }
      எனில்(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        திரும்பு CURLE_TFTP_ILLEGAL;
      }
      இல்லையெனில் எனில்(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        திரும்பு CURLE_TFTP_ILLEGAL;
      }
      இல்லையெனில் எனில்(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        திரும்பு CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned முழு)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    இல்லையெனில் எனில்((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      எனில்(!data->state.upload &&
         !curlx_str_எண்(&மதிப்பு, &tsize, CURL_OFF_T_MAX)) {
        எனில்(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", மதிப்பு);
          திரும்பு CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode tftp_option_add(கட்டமைப்பு tftp_conn *state, size_t *csize,
                                size_t index, மாறிலி எழுத்து *option)
{
  எழுத்து *buf = (எழுத்து *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  எனில்((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    திரும்பு CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  திரும்பு CURLE_OK;
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
நிலையான CURLcode tftp_tx(கட்டமைப்பு tftp_conn *state, tftp_event_t event)
{
  கட்டமைப்பு Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  எழுத்து buffer[STRERROR_LEN];
  எழுத்து *bufptr;
  bool eos;

  எனில்(!data->state.upload)
    திரும்பு CURLE_TFTP_ILLEGAL;

  தேர்வு(event) {

  நிலை TFTP_EVENT_ACK:
  நிலை TFTP_EVENT_OACK:
    எனில்(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      முழு rblock = getrpacketblock(&state->rpacket);

      எனில்(rblock != state->block &&
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
        எனில்(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        இல்லையெனில் {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (கட்டமைப்பு sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          எனில்(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        திரும்பு result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(வெற்று);
      state->block++;
    }
    இல்லையெனில்
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    எனில்(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      திரும்பு CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (எழுத்து *)state->spacket.data + 4;
    செய் {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      எனில்(result)
        திரும்பு result;
      state->sbytes += cb;
      bufptr += cb;
    } போது(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (கட்டமைப்பு sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    எனில்(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
      திரும்பு CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    நிறுத்து;

  நிலை TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    எனில்(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    இல்லையெனில் {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (கட்டமைப்பு sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      எனில்(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
        திரும்பு CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    நிறுத்து;

  நிலை TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (வெறுமை)sendto(state->sockfd, (வெறுமை *)state->spacket.data, 4, SEND_4TH_ARG,
                 (கட்டமைப்பு sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    நிறுத்து;

  இயல்பு:
    failf(data, "tftp_tx: internal error, event: %d", (முழு)event);
    நிறுத்து;
  }

  திரும்பு result;
}

நிலையான CURLcode tftp_connect_for_tx(கட்டமைப்பு tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  எனில்(result)
    திரும்பு result;
  திரும்பு tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
நிலையான CURLcode tftp_rx(கட்டமைப்பு tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  முழு rblock;
  கட்டமைப்பு Curl_easy *data = state->data;
  எழுத்து buffer[STRERROR_LEN];

  தேர்வு(event) {

  நிலை TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    எனில்(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    இல்லையெனில் எனில்(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    இல்லையெனில் {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      நிறுத்து;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (கட்டமைப்பு sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    எனில்(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
      திரும்பு CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    எனில்(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    இல்லையெனில் {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(வெற்று);
    நிறுத்து;

  நிலை TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (கட்டமைப்பு sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    எனில்(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
      திரும்பு CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(வெற்று);
    நிறுத்து;

  நிலை TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    எனில்(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    இல்லையெனில் {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (கட்டமைப்பு sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      எனில்(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
        திரும்பு CURLE_SEND_ERROR;
      }
    }
    நிறுத்து;

  நிலை TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (வெறுமை)sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (கட்டமைப்பு sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    நிறுத்து;

  இயல்பு:
    failf(data, "%s", "tftp_rx: internal error");
    திரும்பு CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode tftp_connect_for_rx(கட்டமைப்பு tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  எனில்(result)
    திரும்பு result;
  திரும்பு tftp_rx(state, event);
}

நிலையான CURLcode tftp_send_first(கட்டமைப்பு tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  மாறிலி எழுத்து *mode = "octet";
  எழுத்து *filename;
  கட்டமைப்பு Curl_easy *data = state->data;
  மாறிலி கட்டமைப்பு Curl_sockaddr_ex *remote_addr = வெற்று;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  எனில்(data->state.prefer_ascii)
    mode = "netascii";

  தேர்வு(event) {

  நிலை TFTP_EVENT_INIT:    /* Send the first packet out */
  நிலை TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    எனில்(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      திரும்பு result;
    }

    எனில்(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      எனில்(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    இல்லையெனில் {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    எனில்(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      திரும்பு CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, வெற்று, REJECT_ZERO);
    எனில்(result)
      திரும்பு result;

    எனில்(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      திரும்பு CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((எழுத்து *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    எனில்(!data->set.tftp_no_options) {
      எழுத்து buf[64];
      /* add tsize option */
      curl_msnprintf(buf, அளவு(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      எனில்(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, அளவு(buf), "%u", state->requested_blksize);
      எனில்(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      எனில்(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, அளவு(buf), "%d", state->retry_time);
      எனில்(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      எனில்(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      எனில்(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        திரும்பு CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#இல்லையெனில்
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    எனில்(!remote_addr)
      திரும்பு CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (வெறுமை *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    எனில்(senddata != (ssize_t)sbytes) {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
      திரும்பு CURLE_SEND_ERROR;
    }
    நிறுத்து;

  நிலை TFTP_EVENT_OACK:
    எனில்(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    இல்லையெனில் {
      result = tftp_connect_for_rx(state, event);
    }
    நிறுத்து;

  நிலை TFTP_EVENT_ACK: /* Connected for transmit */
    எனில்(!data->state.upload)
      திரும்பு CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    நிறுத்து;

  நிலை TFTP_EVENT_DATA: /* Connected for receive */
    எனில்(data->state.upload)
      திரும்பு CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    நிறுத்து;

  நிலை TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    நிறுத்து;

  இயல்பு:
    failf(state->data, "tftp_send_first: internal error");
    திரும்பு CURLE_TFTP_ILLEGAL;
  }

  திரும்பு result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
நிலையான CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  எனில்(error != TFTP_ERR_NONE) {
    தேர்வு(error) {
    நிலை TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      நிறுத்து;
    நிலை TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      நிறுத்து;
    நிலை TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      நிறுத்து;
    நிலை TFTP_ERR_UNDEF:
    நிலை TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      நிறுத்து;
    நிலை TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      நிறுத்து;
    நிலை TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      நிறுத்து;
    நிலை TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      நிறுத்து;
    நிலை TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    நிலை TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      நிறுத்து;
    இயல்பு:
      result = CURLE_ABORTED_BY_CALLBACK;
      நிறுத்து;
    }
  }
  இல்லையெனில்
    result = CURLE_OK;

  திரும்பு result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
நிலையான CURLcode tftp_state_machine(கட்டமைப்பு tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு Curl_easy *data = state->data;

  தேர்வு(state->state) {
  நிலை TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    நிறுத்து;
  நிலை TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    நிறுத்து;
  நிலை TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    நிறுத்து;
  நிலை TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    நிறுத்து;
  இயல்பு:
    DEBUGF(infof(data, "STATE: %d", (முழு)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    நிறுத்து;
  }

  திரும்பு result;
}

நிலையான வெறுமை tftp_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு tftp_conn *state = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
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
நிலையான CURLcode tftp_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு tftp_conn *state;
  முழு blksize;
  முழு need_blksize;
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி கட்டமைப்பு Curl_sockaddr_ex *remote_addr = வெற்று;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, அளவு(*state));
  எனில்(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  எனில்(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (முழு)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  எனில்(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  எனில்(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    எனில்(!state->rpacket.data)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    எனில்(!state->spacket.data)
      திரும்பு CURLE_OUT_OF_MEMORY;
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
  எனில்(!remote_addr)
    திரும்பு CURLE_FAILED_INIT;

  ((கட்டமைப்பு sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  எனில்(result)
    திரும்பு result;

  எனில்(!conn->bits.bound) {
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
    முழு rc = bind(state->sockfd, (கட்டமைப்பு sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    எனில்(rc) {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
      திரும்பு CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  திரும்பு CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
நிலையான CURLcode tftp_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (வெறுமை)status;
  (வெறுமை)premature;

  எனில்(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    திரும்பு CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  எனில்(state)
    result = tftp_translate_code(state->error);

  திரும்பு result;
}

நிலையான CURLcode tftp_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  திரும்பு Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
நிலையான CURLcode tftp_receive_packet(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = அளவு(remote_addr);

  /* Receive the packet */
  state->rbytes = (முழு)recvfrom(state->sockfd,
                                (வெறுமை *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (கட்டமைப்பு sockaddr *)&remote_addr,
                                &fromlen);
  எனில்((state->rbytes >= 0) && fromlen) {
    எனில்(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      எனில்((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        திரும்பு CURLE_RECV_ERROR;
      }
    }
    இல்லையெனில் {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  எனில்(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  இல்லையெனில் {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    தேர்வு(state->event) {
    நிலை TFTP_EVENT_DATA:
      எனில்(data->state.upload)
        திரும்பு CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      எனில்(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (மாறிலி எழுத்து *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        எனில்(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          திரும்பு result;
        }
      }
      நிறுத்து;
    நிலை TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      மாறிலி எழுத்து *str = (மாறிலி எழுத்து *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      எனில்(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      நிறுத்து;
    }
    நிலை TFTP_EVENT_ACK:
      நிறுத்து;
    நிலை TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (மாறிலி எழுத்து *)state->rpacket.data + 2,
                                     state->rbytes-2);
      எனில்(result)
        திரும்பு result;
      நிறுத்து;
    நிலை TFTP_EVENT_RRQ:
    நிலை TFTP_EVENT_WRQ:
    இயல்பு:
      failf(data, "%s", "Internal error: Unexpected packet");
      நிறுத்து;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    எனில்(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      திரும்பு result;
    }
  }
  திரும்பு result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
நிலையான timediff_t tftp_state_timeout(கட்டமைப்பு tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  எனில்(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  எனில்(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    திரும்பு timeout_ms;
  }
  current = time(வெற்று);
  எனில்(current > state->rx_time + state->retry_time) {
    எனில்(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(வெற்று); /* update even though we received nothing */
  }

  திரும்பு timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
நிலையான CURLcode tftp_multi_statemach(கட்டமைப்பு Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  எனில்(!state)
    திரும்பு CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  எனில்(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }
  எனில்(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    எனில்(result)
      திரும்பு result;
    *done = (state->state == TFTP_STATE_FIN);
    எனில்(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  இல்லையெனில் {
    /* no timeouts to handle, check our socket */
    முழு rc = SOCKET_READABLE(state->sockfd, 0);

    எனில்(rc == -1) {
      /* bail out */
      முழு sockerr = SOCKERRNO;
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, அளவு(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    இல்லையெனில் எனில்(rc) {
      result = tftp_receive_packet(data, state);
      எனில்(result)
        திரும்பு result;
      result = tftp_state_machine(state, state->event);
      எனில்(result)
        திரும்பு result;
      *done = (state->state == TFTP_STATE_FIN);
      எனில்(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  திரும்பு result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
நிலையான CURLcode tftp_doing(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  எனில்(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  இல்லையெனில் எனில்(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  திரும்பு result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
நிலையான CURLcode tftp_perform(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  எனில்(!state)
    திரும்பு CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  எனில்((state->state == TFTP_STATE_FIN) || result)
    திரும்பு result;

  result = tftp_multi_statemach(data, dophase_done);

  எனில்(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  திரும்பு result;
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

நிலையான CURLcode tftp_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  எனில்(!state) {
    result = tftp_connect(data, done);
    எனில்(result)
      திரும்பு result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    எனில்(!state)
      திரும்பு CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  எனில்(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  திரும்பு result;
}

நிலையான CURLcode tftp_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  எழுத்து *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  எனில்((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  இல்லையெனில் எனில்((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  திரும்பு CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_tftp = {
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
