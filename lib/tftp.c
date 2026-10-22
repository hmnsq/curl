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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "sockaddr.h" /* required for Curl_sockaddr_storage */
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

પ્રકારવ્યાખ્યા ગણના {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

પ્રકારવ્યાખ્યા ગણના {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

પ્રકારવ્યાખ્યા ગણના {
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

પ્રકારવ્યાખ્યા ગણના {
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

સંરચના tftp_packet {
  unsigned અક્ષર *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

સંરચના tftp_conn {
  સંરચના Curl_sockaddr_storage local_addr;
  સંરચના Curl_sockaddr_storage remote_addr;
  સંરચના tftp_packet rpacket;
  સંરચના tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  સંરચના Curl_easy *data;
  curl_socket_t   sockfd;
  પૂર્ણાંક             retries;
  પૂર્ણાંક             retry_time;
  પૂર્ણાંક             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  પૂર્ણાંક             rbytes;
  size_t          sbytes;
  unsigned પૂર્ણાંક    blksize;
  unsigned પૂર્ણાંક    requested_blksize;
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
સ્થિર CURLcode tftp_set_timeouts(સંરચના tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  જો(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    પાછા CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  જો((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  નહીંતર
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (પૂર્ણાંક)timeout / 5;

  /* Bound the total number */
  જો(state->retry_max < 3)
    state->retry_max = 3;

  જો(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (પૂર્ણાંક)(timeout / state->retry_max);
  જો(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (પૂર્ણાંક)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(ખાલી);

  પાછા CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

સ્થિર શૂન્ય setpacketevent(સંરચના tftp_packet *packet, unsigned short અંક)
{
  packet->data[0] = (unsigned અક્ષર)(અંક >> 8);
  packet->data[1] = (unsigned અક્ષર)(અંક & 0xff);
}

સ્થિર શૂન્ય setpacketblock(સંરચના tftp_packet *packet, unsigned short અંક)
{
  packet->data[2] = (unsigned અક્ષર)(અંક >> 8);
  packet->data[3] = (unsigned અક્ષર)(અંક & 0xff);
}

સ્થિર unsigned short getrpacketevent(અચલ સંરચના tftp_packet *packet)
{
  પાછા (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

સ્થિર unsigned short getrpacketblock(અચલ સંરચના tftp_packet *packet)
{
  પાછા (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

સ્થિર size_t tftp_strnlen(અચલ અક્ષર *string, size_t maxlen)
{
  અચલ અક્ષર *end = memchr(string, '\0', maxlen);
  પાછા end ? (size_t)(end - string) : maxlen;
}

સ્થિર અચલ અક્ષર *tftp_option_get(અચલ અક્ષર *buf, size_t len,
                                   અચલ અક્ષર **option, અચલ અક્ષર **કિંમત)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  જો(loc >= len)
    પાછા ખાલી;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  જો(loc > len)
    પાછા ખાલી;
  *કિંમત = &buf[strlen(*option) + 1];

  પાછા &buf[loc];
}

સ્થિર CURLcode tftp_parse_option_ack(સંરચના tftp_conn *state,
                                      અચલ અક્ષર *ptr, પૂર્ણાંક len)
{
  અચલ અક્ષર *tmp = ptr;
  સંરચના Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  જ્યારે(tmp < ptr + len) {
    અચલ અક્ષર *option, *કિંમત;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &કિંમત);
    જો(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      પાછા CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, કિંમત);

    જો((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      જો(curlx_str_સંખ્યા(&કિંમત, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        પાછા CURLE_TFTP_ILLEGAL;
      }
      જો(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        પાછા CURLE_TFTP_ILLEGAL;
      }
      નહીંતર જો(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        પાછા CURLE_TFTP_ILLEGAL;
      }
      નહીંતર જો(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        પાછા CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned પૂર્ણાંક)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    નહીંતર જો((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      જો(!data->state.upload &&
         !curlx_str_સંખ્યા(&કિંમત, &tsize, CURL_OFF_T_MAX)) {
        જો(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", કિંમત);
          પાછા CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode tftp_option_add(સંરચના tftp_conn *state, size_t *csize,
                                size_t index, અચલ અક્ષર *option)
{
  અક્ષર *buf = (અક્ષર *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  જો((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    પાછા CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  પાછા CURLE_OK;
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
સ્થિર CURLcode tftp_tx(સંરચના tftp_conn *state, tftp_event_t event)
{
  સંરચના Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  સંરચના SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  અક્ષર buffer[STRERROR_LEN];
  અક્ષર *bufptr;
  bool eos;

  જો(!data->state.upload)
    પાછા CURLE_TFTP_ILLEGAL;

  પસંદગી(event) {

  સ્થિતિ TFTP_EVENT_ACK:
  સ્થિતિ TFTP_EVENT_OACK:
    જો(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      પૂર્ણાંક rblock = getrpacketblock(&state->rpacket);

      જો(rblock != state->block &&
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
        જો(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        નહીંતર {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (સંરચના sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          જો(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        પાછા result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(ખાલી);
      state->block++;
    }
    નહીંતર
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    જો(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      પાછા CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (અક્ષર *)state->spacket.data + 4;
    કરો {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      જો(result)
        પાછા result;
      state->sbytes += cb;
      bufptr += cb;
    } જ્યારે(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (સંરચના sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    જો(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
      પાછા CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    તોડો;

  સ્થિતિ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    જો(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    નહીંતર {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (સંરચના sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      જો(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
        પાછા CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    તોડો;

  સ્થિતિ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (શૂન્ય)sendto(state->sockfd, (શૂન્ય *)state->spacket.data, 4, SEND_4TH_ARG,
                 (સંરચના sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    તોડો;

  મૂળભૂત:
    failf(data, "tftp_tx: internal error, event: %d", (પૂર્ણાંક)event);
    તોડો;
  }

  પાછા result;
}

સ્થિર CURLcode tftp_connect_for_tx(સંરચના tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  જો(result)
    પાછા result;
  પાછા tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
સ્થિર CURLcode tftp_rx(સંરચના tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  પૂર્ણાંક rblock;
  સંરચના Curl_easy *data = state->data;
  અક્ષર buffer[STRERROR_LEN];

  પસંદગી(event) {

  સ્થિતિ TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    જો(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    નહીંતર જો(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    નહીંતર {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      તોડો;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (સંરચના sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    જો(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
      પાછા CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    જો(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    નહીંતર {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(ખાલી);
    તોડો;

  સ્થિતિ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (સંરચના sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    જો(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
      પાછા CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(ખાલી);
    તોડો;

  સ્થિતિ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    જો(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    નહીંતર {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (સંરચના sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      જો(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
        પાછા CURLE_SEND_ERROR;
      }
    }
    તોડો;

  સ્થિતિ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (શૂન્ય)sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (સંરચના sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    તોડો;

  મૂળભૂત:
    failf(data, "%s", "tftp_rx: internal error");
    પાછા CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode tftp_connect_for_rx(સંરચના tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  જો(result)
    પાછા result;
  પાછા tftp_rx(state, event);
}

સ્થિર CURLcode tftp_send_first(સંરચના tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  અચલ અક્ષર *mode = "octet";
  અક્ષર *filename;
  સંરચના Curl_easy *data = state->data;
  અચલ સંરચના Curl_sockaddr_ex *remote_addr = ખાલી;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  જો(data->state.prefer_ascii)
    mode = "netascii";

  પસંદગી(event) {

  સ્થિતિ TFTP_EVENT_INIT:    /* Send the first packet out */
  સ્થિતિ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    જો(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      પાછા result;
    }

    જો(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      જો(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    નહીંતર {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    જો(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      પાછા CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, ખાલી, REJECT_ZERO);
    જો(result)
      પાછા result;

    જો(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      પાછા CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((અક્ષર *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    જો(!data->set.tftp_no_options) {
      અક્ષર buf[64];
      /* add tsize option */
      curl_msnprintf(buf, કદ(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      જો(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, કદ(buf), "%u", state->requested_blksize);
      જો(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      જો(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, કદ(buf), "%d", state->retry_time);
      જો(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      જો(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      જો(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        પાછા CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#નહીંતર
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    જો(!remote_addr)
      પાછા CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (શૂન્ય *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    જો(senddata != (ssize_t)sbytes) {
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
      પાછા CURLE_SEND_ERROR;
    }
    તોડો;

  સ્થિતિ TFTP_EVENT_OACK:
    જો(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    નહીંતર {
      result = tftp_connect_for_rx(state, event);
    }
    તોડો;

  સ્થિતિ TFTP_EVENT_ACK: /* Connected for transmit */
    જો(!data->state.upload)
      પાછા CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    તોડો;

  સ્થિતિ TFTP_EVENT_DATA: /* Connected for receive */
    જો(data->state.upload)
      પાછા CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    તોડો;

  સ્થિતિ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    તોડો;

  મૂળભૂત:
    failf(state->data, "tftp_send_first: internal error");
    પાછા CURLE_TFTP_ILLEGAL;
  }

  પાછા result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
સ્થિર CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  જો(error != TFTP_ERR_NONE) {
    પસંદગી(error) {
    સ્થિતિ TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      તોડો;
    સ્થિતિ TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      તોડો;
    સ્થિતિ TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      તોડો;
    સ્થિતિ TFTP_ERR_UNDEF:
    સ્થિતિ TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      તોડો;
    સ્થિતિ TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      તોડો;
    સ્થિતિ TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      તોડો;
    સ્થિતિ TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      તોડો;
    સ્થિતિ TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    સ્થિતિ TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      તોડો;
    મૂળભૂત:
      result = CURLE_ABORTED_BY_CALLBACK;
      તોડો;
    }
  }
  નહીંતર
    result = CURLE_OK;

  પાછા result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
સ્થિર CURLcode tftp_state_machine(સંરચના tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  સંરચના Curl_easy *data = state->data;

  પસંદગી(state->state) {
  સ્થિતિ TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    તોડો;
  સ્થિતિ TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    તોડો;
  સ્થિતિ TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    તોડો;
  સ્થિતિ TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    તોડો;
  મૂળભૂત:
    DEBUGF(infof(data, "STATE: %d", (પૂર્ણાંક)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    તોડો;
  }

  પાછા result;
}

સ્થિર શૂન્ય tftp_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના tftp_conn *state = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
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
સ્થિર CURLcode tftp_connect(સંરચના Curl_easy *data, bool *done)
{
  સંરચના tftp_conn *state;
  પૂર્ણાંક blksize;
  પૂર્ણાંક need_blksize;
  સંરચના connectdata *conn = data->conn;
  અચલ સંરચના Curl_sockaddr_ex *remote_addr = ખાલી;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, કદ(*state));
  જો(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  જો(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (પૂર્ણાંક)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  જો(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  જો(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    જો(!state->rpacket.data)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    જો(!state->spacket.data)
      પાછા CURLE_OUT_OF_MEMORY;
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
  જો(!remote_addr)
    પાછા CURLE_FAILED_INIT;

  ((સંરચના sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  જો(result)
    પાછા result;

  જો(!conn->bits.bound) {
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
    પૂર્ણાંક rc = bind(state->sockfd, (સંરચના sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    જો(rc) {
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
      પાછા CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  પાછા CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
સ્થિર CURLcode tftp_done(સંરચના Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (શૂન્ય)status;
  (શૂન્ય)premature;

  જો(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    પાછા CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  જો(state)
    result = tftp_translate_code(state->error);

  પાછા result;
}

સ્થિર CURLcode tftp_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps)
{
  પાછા Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
સ્થિર CURLcode tftp_receive_packet(સંરચના Curl_easy *data,
                                    સંરચના tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  સંરચના Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = કદ(remote_addr);

  /* Receive the packet */
  state->rbytes = (પૂર્ણાંક)recvfrom(state->sockfd,
                                (શૂન્ય *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (સંરચના sockaddr *)&remote_addr,
                                &fromlen);
  જો((state->rbytes >= 0) && fromlen) {
    જો(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      જો((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        પાછા CURLE_RECV_ERROR;
      }
    }
    નહીંતર {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  જો(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  નહીંતર {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    પસંદગી(state->event) {
    સ્થિતિ TFTP_EVENT_DATA:
      જો(data->state.upload)
        પાછા CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      જો(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (અચલ અક્ષર *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        જો(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          પાછા result;
        }
      }
      તોડો;
    સ્થિતિ TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      અચલ અક્ષર *str = (અચલ અક્ષર *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      જો(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      તોડો;
    }
    સ્થિતિ TFTP_EVENT_ACK:
      તોડો;
    સ્થિતિ TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (અચલ અક્ષર *)state->rpacket.data + 2,
                                     state->rbytes-2);
      જો(result)
        પાછા result;
      તોડો;
    સ્થિતિ TFTP_EVENT_RRQ:
    સ્થિતિ TFTP_EVENT_WRQ:
    મૂળભૂત:
      failf(data, "%s", "Internal error: Unexpected packet");
      તોડો;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    જો(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      પાછા result;
    }
  }
  પાછા result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
સ્થિર timediff_t tftp_state_timeout(સંરચના tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  જો(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  જો(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    પાછા timeout_ms;
  }
  current = time(ખાલી);
  જો(current > state->rx_time + state->retry_time) {
    જો(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(ખાલી); /* update even though we received nothing */
  }

  પાછા timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
સ્થિર CURLcode tftp_multi_statemach(સંરચના Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  જો(!state)
    પાછા CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  જો(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    પાછા CURLE_OPERATION_TIMEDOUT;
  }
  જો(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    જો(result)
      પાછા result;
    *done = (state->state == TFTP_STATE_FIN);
    જો(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  નહીંતર {
    /* no timeouts to handle, check our socket */
    પૂર્ણાંક rc = SOCKET_READABLE(state->sockfd, 0);

    જો(rc == -1) {
      /* bail out */
      પૂર્ણાંક sockerr = SOCKERRNO;
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, કદ(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    નહીંતર જો(rc) {
      result = tftp_receive_packet(data, state);
      જો(result)
        પાછા result;
      result = tftp_state_machine(state, state->event);
      જો(result)
        પાછા result;
      *done = (state->state == TFTP_STATE_FIN);
      જો(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  પાછા result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
સ્થિર CURLcode tftp_doing(સંરચના Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  જો(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  નહીંતર જો(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  પાછા result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
સ્થિર CURLcode tftp_perform(સંરચના Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  જો(!state)
    પાછા CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  જો((state->state == TFTP_STATE_FIN) || result)
    પાછા result;

  result = tftp_multi_statemach(data, dophase_done);

  જો(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  પાછા result;
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

સ્થિર CURLcode tftp_do(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  જો(!state) {
    result = tftp_connect(data, done);
    જો(result)
      પાછા result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    જો(!state)
      પાછા CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  જો(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  પાછા result;
}

સ્થિર CURLcode tftp_setup_connection(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  અક્ષર *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  જો((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  નહીંતર જો((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  પાછા CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_tftp = {
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
