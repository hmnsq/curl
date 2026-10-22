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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ସଂରଚନା tftp_packet {
  unsigned ଅକ୍ଷର *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ସଂରଚନା tftp_conn {
  ସଂରଚନା Curl_sockaddr_storage local_addr;
  ସଂରଚନା Curl_sockaddr_storage remote_addr;
  ସଂରଚନା tftp_packet rpacket;
  ସଂରଚନା tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  ସଂରଚନା Curl_easy *data;
  curl_socket_t   sockfd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା             retries;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା             retry_time;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା             rbytes;
  size_t          sbytes;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    blksize;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    requested_blksize;
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
ସ୍ଥିର CURLcode tftp_set_timeouts(ସଂରଚନା tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  ଯଦି(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ଯଦି((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ଅନ୍ୟଥା
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)timeout / 5;

  /* Bound the total number */
  ଯଦି(state->retry_max < 3)
    state->retry_max = 3;

  ଯଦି(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(timeout / state->retry_max);
  ଯଦି(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(ଖାଲି);

  ଫେରନ୍ତୁ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ସ୍ଥିର ଶୂନ୍ୟ setpacketevent(ସଂରଚନା tftp_packet *packet, unsigned short ଅଙ୍କ)
{
  packet->data[0] = (unsigned ଅକ୍ଷର)(ଅଙ୍କ >> 8);
  packet->data[1] = (unsigned ଅକ୍ଷର)(ଅଙ୍କ & 0xff);
}

ସ୍ଥିର ଶୂନ୍ୟ setpacketblock(ସଂରଚନା tftp_packet *packet, unsigned short ଅଙ୍କ)
{
  packet->data[2] = (unsigned ଅକ୍ଷର)(ଅଙ୍କ >> 8);
  packet->data[3] = (unsigned ଅକ୍ଷର)(ଅଙ୍କ & 0xff);
}

ସ୍ଥିର unsigned short getrpacketevent(ଅଚଳ ସଂରଚନା tftp_packet *packet)
{
  ଫେରନ୍ତୁ (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

ସ୍ଥିର unsigned short getrpacketblock(ଅଚଳ ସଂରଚନା tftp_packet *packet)
{
  ଫେରନ୍ତୁ (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

ସ୍ଥିର size_t tftp_strnlen(ଅଚଳ ଅକ୍ଷର *string, size_t maxlen)
{
  ଅଚଳ ଅକ୍ଷର *end = memchr(string, '\0', maxlen);
  ଫେରନ୍ତୁ end ? (size_t)(end - string) : maxlen;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *tftp_option_get(ଅଚଳ ଅକ୍ଷର *buf, size_t len,
                                   ଅଚଳ ଅକ୍ଷର **option, ଅଚଳ ଅକ୍ଷର **ମୂଲ୍ୟ)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ଯଦି(loc >= len)
    ଫେରନ୍ତୁ ଖାଲି;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ଯଦି(loc > len)
    ଫେରନ୍ତୁ ଖାଲି;
  *ମୂଲ୍ୟ = &buf[strlen(*option) + 1];

  ଫେରନ୍ତୁ &buf[loc];
}

ସ୍ଥିର CURLcode tftp_parse_option_ack(ସଂରଚନା tftp_conn *state,
                                      ଅଚଳ ଅକ୍ଷର *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
  ଅଚଳ ଅକ୍ଷର *tmp = ptr;
  ସଂରଚନା Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ଯେତେବେଳେ(tmp < ptr + len) {
    ଅଚଳ ଅକ୍ଷର *option, *ମୂଲ୍ୟ;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &ମୂଲ୍ୟ);
    ଯଦି(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, ମୂଲ୍ୟ);

    ଯଦି((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ଯଦି(curlx_str_ସଂଖ୍ୟା(&ମୂଲ୍ୟ, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      }
      ଯଦି(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      }
      ଅନ୍ୟଥା ଯଦି(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      }
      ଅନ୍ୟଥା ଯଦି(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ଅନ୍ୟଥା ଯଦି((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ଯଦି(!data->state.upload &&
         !curlx_str_ସଂଖ୍ୟା(&ମୂଲ୍ୟ, &tsize, CURL_OFF_T_MAX)) {
        ଯଦି(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", ମୂଲ୍ୟ);
          ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode tftp_option_add(ସଂରଚନା tftp_conn *state, size_t *csize,
                                size_t index, ଅଚଳ ଅକ୍ଷର *option)
{
  ଅକ୍ଷର *buf = (ଅକ୍ଷର *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  ଯଦି((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର CURLcode tftp_tx(ସଂରଚନା tftp_conn *state, tftp_event_t event)
{
  ସଂରଚନା Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  ସଂରଚନା SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  ଅକ୍ଷର buffer[STRERROR_LEN];
  ଅକ୍ଷର *bufptr;
  bool eos;

  ଯଦି(!data->state.upload)
    ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;

  ବାଛ(event) {

  ସ୍ଥିତି TFTP_EVENT_ACK:
  ସ୍ଥିତି TFTP_EVENT_OACK:
    ଯଦି(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rblock = getrpacketblock(&state->rpacket);

      ଯଦି(rblock != state->block &&
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
        ଯଦି(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        ଅନ୍ୟଥା {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ସଂରଚନା sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ଯଦି(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        ଫେରନ୍ତୁ result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(ଖାଲି);
      state->block++;
    }
    ଅନ୍ୟଥା
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    ଯଦି(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (ଅକ୍ଷର *)state->spacket.data + 4;
    କର {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      state->sbytes += cb;
      bufptr += cb;
    } ଯେତେବେଳେ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ସଂରଚନା sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ଯଦି(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    ଯଦି(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ଅନ୍ୟଥା {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ସଂରଚନା sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ଯଦି(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ଶୂନ୍ୟ)sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data, 4, SEND_4TH_ARG,
                 (ସଂରଚନା sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "tftp_tx: internal error, event: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event);
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode tftp_connect_for_tx(ସଂରଚନା tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଫେରନ୍ତୁ tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_rx(ସଂରଚନା tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rblock;
  ସଂରଚନା Curl_easy *data = state->data;
  ଅକ୍ଷର buffer[STRERROR_LEN];

  ବାଛ(event) {

  ସ୍ଥିତି TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ଯଦି(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ଅନ୍ୟଥା ଯଦି(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    ଅନ୍ୟଥା {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      ଭାଙ୍ଗ;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ସଂରଚନା sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ଯଦି(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ଯଦି(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ଅନ୍ୟଥା {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(ଖାଲି);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ସଂରଚନା sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ଯଦି(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(ଖାଲି);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    ଯଦି(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ଅନ୍ୟଥା {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (ସଂରଚନା sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ଯଦି(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ଶୂନ୍ୟ)sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (ସଂରଚନା sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "%s", "tftp_rx: internal error");
    ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode tftp_connect_for_rx(ସଂରଚନା tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଫେରନ୍ତୁ tftp_rx(state, event);
}

ସ୍ଥିର CURLcode tftp_send_first(ସଂରଚନା tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ଅଚଳ ଅକ୍ଷର *mode = "octet";
  ଅକ୍ଷର *filename;
  ସଂରଚନା Curl_easy *data = state->data;
  ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *remote_addr = ଖାଲି;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ଯଦି(data->state.prefer_ascii)
    mode = "netascii";

  ବାଛ(event) {

  ସ୍ଥିତି TFTP_EVENT_INIT:    /* Send the first packet out */
  ସ୍ଥିତି TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ଯଦି(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ଫେରନ୍ତୁ result;
    }

    ଯଦି(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ଯଦି(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    ଅନ୍ୟଥା {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ଯଦି(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, ଖାଲି, REJECT_ZERO);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯଦି(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((ଅକ୍ଷର *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ଯଦି(!data->set.tftp_no_options) {
      ଅକ୍ଷର buf[64];
      /* add tsize option */
      curl_msnprintf(buf, ଆକାର(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ଯଦି(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, ଆକାର(buf), "%u", state->requested_blksize);
      ଯଦି(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ଯଦି(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, ଆକାର(buf), "%d", state->retry_time);
      ଯଦି(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ଯଦି(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      ଯଦି(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ଅନ୍ୟଥା
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    ଯଦି(!remote_addr)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (ଶୂନ୍ୟ *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ଯଦି(senddata != (ssize_t)sbytes) {
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_OACK:
    ଯଦି(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    ଅନ୍ୟଥା {
      result = tftp_connect_for_rx(state, event);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_ACK: /* Connected for transmit */
    ଯଦି(!data->state.upload)
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_DATA: /* Connected for receive */
    ଯଦି(data->state.upload)
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(state->data, "tftp_send_first: internal error");
    ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
  }

  ଫେରନ୍ତୁ result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  ଯଦି(error != TFTP_ERR_NONE) {
    ବାଛ(error) {
    ସ୍ଥିତି TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_UNDEF:
    ସ୍ଥିତି TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = CURLE_ABORTED_BY_CALLBACK;
      ଭାଙ୍ଗ;
    }
  }
  ଅନ୍ୟଥା
    result = CURLE_OK;

  ଫେରନ୍ତୁ result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_state_machine(ସଂରଚନା tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା Curl_easy *data = state->data;

  ବାଛ(state->state) {
  ସ୍ଥିତି TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGF(infof(data, "STATE: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ tftp_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା tftp_conn *state = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
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
ସ୍ଥିର CURLcode tftp_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା tftp_conn *state;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା blksize;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_blksize;
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *remote_addr = ଖାଲି;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, ଆକାର(*state));
  ଯଦି(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  ଯଦି(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ଯଦି(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ଯଦି(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ଯଦି(!state->rpacket.data)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    ଯଦି(!state->spacket.data)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
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
  ଯଦି(!remote_addr)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ((ସଂରଚନା sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(!conn->bits.bound) {
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = bind(state->sockfd, (ସଂରଚନା sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    ଯଦି(rc) {
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ଫେରନ୍ତୁ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (ଶୂନ୍ୟ)status;
  (ଶୂନ୍ୟ)premature;

  ଯଦି(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ଫେରନ୍ତୁ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ଯଦି(state)
    result = tftp_translate_code(state->error);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode tftp_pollset(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps)
{
  ଫେରନ୍ତୁ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_receive_packet(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = ଆକାର(remote_addr);

  /* Receive the packet */
  state->rbytes = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)recvfrom(state->sockfd,
                                (ଶୂନ୍ୟ *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ସଂରଚନା sockaddr *)&remote_addr,
                                &fromlen);
  ଯଦି((state->rbytes >= 0) && fromlen) {
    ଯଦି(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ଯଦି((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
    }
    ଅନ୍ୟଥା {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ଯଦି(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ଅନ୍ୟଥା {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ବାଛ(state->event) {
    ସ୍ଥିତି TFTP_EVENT_DATA:
      ଯଦି(data->state.upload)
        ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ଯଦି(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (ଅଚଳ ଅକ୍ଷର *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        ଯଦି(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ଫେରନ୍ତୁ result;
        }
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ଅଚଳ ଅକ୍ଷର *str = (ଅଚଳ ଅକ୍ଷର *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ଯଦି(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି TFTP_EVENT_ACK:
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (ଅଚଳ ଅକ୍ଷର *)state->rpacket.data + 2,
                                     state->rbytes-2);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି TFTP_EVENT_RRQ:
    ସ୍ଥିତି TFTP_EVENT_WRQ:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "%s", "Internal error: Unexpected packet");
      ଭାଙ୍ଗ;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    ଯଦି(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ଫେରନ୍ତୁ result;
    }
  }
  ଫେରନ୍ତୁ result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ସ୍ଥିର timediff_t tftp_state_timeout(ସଂରଚନା tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  ଯଦି(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  ଯଦି(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ଫେରନ୍ତୁ timeout_ms;
  }
  current = time(ଖାଲି);
  ଯଦି(current > state->rx_time + state->retry_time) {
    ଯଦି(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(ଖାଲି); /* update even though we received nothing */
  }

  ଫେରନ୍ତୁ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_multi_statemach(ସଂରଚନା Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  ଯଦି(!state)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ଯଦି(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }
  ଯଦି(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    *done = (state->state == TFTP_STATE_FIN);
    ଯଦି(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  ଅନ୍ୟଥା {
    /* no timeouts to handle, check our socket */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = SOCKET_READABLE(state->sockfd, 0);

    ଯଦି(rc == -1) {
      /* bail out */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    ଅନ୍ୟଥା ଯଦି(rc) {
      result = tftp_receive_packet(data, state);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      result = tftp_state_machine(state, state->event);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      *done = (state->state == TFTP_STATE_FIN);
      ଯଦି(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  ଫେରନ୍ତୁ result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_doing(ସଂରଚନା Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  ଯଦି(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  ଅନ୍ୟଥା ଯଦି(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  ଫେରନ୍ତୁ result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ସ୍ଥିର CURLcode tftp_perform(ସଂରଚନା Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  ଯଦି(!state)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  ଯଦି((state->state == TFTP_STATE_FIN) || result)
    ଫେରନ୍ତୁ result;

  result = tftp_multi_statemach(data, dophase_done);

  ଯଦି(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ଫେରନ୍ତୁ result;
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

ସ୍ଥିର CURLcode tftp_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  ଯଦି(!state) {
    result = tftp_connect(data, done);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ଯଦି(!state)
      ଫେରନ୍ତୁ CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ଯଦି(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode tftp_setup_connection(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ଅକ୍ଷର *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ଯଦି((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  ଅନ୍ୟଥା ଯଦି((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_tftp = {
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
