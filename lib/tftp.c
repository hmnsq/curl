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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "sockaddr.h" /* required for Curl_sockaddr_storage */
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

প্রকারলেখ গণনা {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

প্রকারলেখ গণনা {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

প্রকারলেখ গণনা {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_তথ্য = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

প্রকারলেখ গণনা {
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

কাঠামো tftp_packet {
  unsigned অক্ষর *তথ্য;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

কাঠামো tftp_conn {
  কাঠামো Curl_sockaddr_storage local_addr;
  কাঠামো Curl_sockaddr_storage remote_addr;
  কাঠামো tftp_packet rpacket;
  কাঠামো tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  কাঠামো Curl_easy *তথ্য;
  curl_socket_t   sockfd;
  পূর্ণসংখ্যা             retries;
  পূর্ণসংখ্যা             retry_time;
  পূর্ণসংখ্যা             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  পূর্ণসংখ্যা             rbytes;
  size_t          sbytes;
  unsigned পূর্ণসংখ্যা    blksize;
  unsigned পূর্ণসংখ্যা    requested_blksize;
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
স্থির CURLcode tftp_set_timeouts(কাঠামো tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->তথ্য);

  যদি(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->তথ্য, "Connection time-out");
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  যদি((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  অন্যথা
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (পূর্ণসংখ্যা)timeout / 5;

  /* Bound the total number */
  যদি(state->retry_max < 3)
    state->retry_max = 3;

  যদি(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (পূর্ণসংখ্যা)(timeout / state->retry_max);
  যদি(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->তথ্য,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (পূর্ণসংখ্যা)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(ফাঁকা);

  ফেরত CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

স্থির শূন্য setpacketevent(কাঠামো tftp_packet *packet, unsigned short অঙ্ক)
{
  packet->তথ্য[0] = (unsigned অক্ষর)(অঙ্ক >> 8);
  packet->তথ্য[1] = (unsigned অক্ষর)(অঙ্ক & 0xff);
}

স্থির শূন্য setpacketblock(কাঠামো tftp_packet *packet, unsigned short অঙ্ক)
{
  packet->তথ্য[2] = (unsigned অক্ষর)(অঙ্ক >> 8);
  packet->তথ্য[3] = (unsigned অক্ষর)(অঙ্ক & 0xff);
}

স্থির unsigned short getrpacketevent(ধ্রুবক কাঠামো tftp_packet *packet)
{
  ফেরত (unsigned short)((packet->তথ্য[0] << 8) | packet->তথ্য[1]);
}

স্থির unsigned short getrpacketblock(ধ্রুবক কাঠামো tftp_packet *packet)
{
  ফেরত (unsigned short)((packet->তথ্য[2] << 8) | packet->তথ্য[3]);
}

স্থির size_t tftp_strnlen(ধ্রুবক অক্ষর *string, size_t maxlen)
{
  ধ্রুবক অক্ষর *end = memchr(string, '\0', maxlen);
  ফেরত end ? (size_t)(end - string) : maxlen;
}

স্থির ধ্রুবক অক্ষর *tftp_option_get(ধ্রুবক অক্ষর *buf, size_t len,
                                   ধ্রুবক অক্ষর **option, ধ্রুবক অক্ষর **মান)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  যদি(loc >= len)
    ফেরত ফাঁকা;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  যদি(loc > len)
    ফেরত ফাঁকা;
  *মান = &buf[strlen(*option) + 1];

  ফেরত &buf[loc];
}

স্থির CURLcode tftp_parse_option_ack(কাঠামো tftp_conn *state,
                                      ধ্রুবক অক্ষর *ptr, পূর্ণসংখ্যা len)
{
  ধ্রুবক অক্ষর *tmp = ptr;
  কাঠামো Curl_easy *তথ্য = state->তথ্য;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  যখন(tmp < ptr + len) {
    ধ্রুবক অক্ষর *option, *মান;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &মান);
    যদি(!tmp) {
      failf(তথ্য, "Malformed ACK packet, rejecting");
      ফেরত CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(তথ্য, "got option=(%s) value=(%s)", option, মান);

    যদি((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      যদি(curlx_str_সংখ্যা(&মান, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(তথ্য, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ফেরত CURLE_TFTP_ILLEGAL;
      }
      যদি(!blksize) {
        failf(তথ্য, "invalid blocksize value in OACK packet");
        ফেরত CURLE_TFTP_ILLEGAL;
      }
      অন্যথা যদি(blksize < TFTP_BLKSIZE_MIN) {
        failf(তথ্য, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ফেরত CURLE_TFTP_ILLEGAL;
      }
      অন্যথা যদি(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(তথ্য, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ফেরত CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned পূর্ণসংখ্যা)blksize;
      infof(তথ্য, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    অন্যথা যদি((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      যদি(!তথ্য->state.upload &&
         !curlx_str_সংখ্যা(&মান, &tsize, CURL_OFF_T_MAX)) {
        যদি(!tsize) {
          failf(তথ্য, "invalid tsize -:%s:- value in OACK packet", মান);
          ফেরত CURLE_TFTP_ILLEGAL;
        }
        infof(তথ্য, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_Set_Download_আকার(তথ্য, tsize);
      }
    }
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode tftp_option_add(কাঠামো tftp_conn *state, size_t *csize,
                                size_t সূচক, ধ্রুবক অক্ষর *option)
{
  অক্ষর *buf = (অক্ষর *)&state->spacket.তথ্য[সূচক];
  size_t oplen = strlen(option);
  size_t blen;
  যদি((state->blksize <= সূচক) ||
     (oplen + 1) > (size_t)(state->blksize - সূচক))
    ফেরত CURLE_TFTP_ILLEGAL;
  blen = state->blksize - সূচক;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ফেরত CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#define পরবর্তী_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
স্থির CURLcode tftp_tx(কাঠামো tftp_conn *state, tftp_event_t event)
{
  কাঠামো Curl_easy *তথ্য = state->তথ্য;
  ssize_t sbytes;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SingleRequest *k = &তথ্য->req;
  size_t cb; /* Bytes currently read */
  অক্ষর বাফার[STRERROR_LEN];
  অক্ষর *bufptr;
  bool eos;

  যদি(!তথ্য->state.upload)
    ফেরত CURLE_TFTP_ILLEGAL;

  নির্বাচন(event) {

  ক্ষেত্র TFTP_EVENT_ACK:
  ক্ষেত্র TFTP_EVENT_OACK:
    যদি(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      পূর্ণসংখ্যা rblock = getrpacketblock(&state->rpacket);

      যদি(rblock != state->block &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->block == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(তথ্য, "Received ACK for block %d, expecting %d",
              rblock, state->block);
        state->retries++;
        /* Bail out if over the maximum */
        যদি(state->retries > state->retry_max) {
          failf(তথ্য, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          ফলাফল = CURLE_SEND_ERROR;
        }
        অন্যথা {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (কাঠামো sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          যদি(sbytes < 0) {
            failf(তথ্য, "%s",
                  curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
            ফলাফল = CURLE_SEND_ERROR;
          }
        }

        ফেরত ফলাফল;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(ফাঁকা);
      state->block++;
    }
    অন্যথা
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_তথ্য);
    setpacketblock(&state->spacket, state->block);
    যদি(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ফেরত CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (অক্ষর *)state->spacket.তথ্য + 4;
    করো {
      ফলাফল = Curl_client_read(তথ্য, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      state->sbytes += cb;
      bufptr += cb;
    } যখন(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (কাঠামো sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    যদি(sbytes < 0) {
      failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
      ফেরত CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(তথ্য, state->sbytes);
    বিরতি;

  ক্ষেত্র TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(তথ্য, "Timeout waiting for block %d ACK. "
          "Retries = %d", পরবর্তী_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (কাঠামো sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      যদি(sbytes < 0) {
        failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
        ফেরত CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_Set_Upload_গণক(তথ্য, k->writebytecount);
    }
    বিরতি;

  ক্ষেত্র TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (শূন্য)sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য, 4, SEND_4TH_ARG,
                 (কাঠামো sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    বিরতি;

  পূর্বনির্ধারিত:
    failf(তথ্য, "tftp_tx: internal error, event: %d", (পূর্ণসংখ্যা)event);
    বিরতি;
  }

  ফেরত ফলাফল;
}

স্থির CURLcode tftp_connect_for_tx(কাঠামো tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ফলাফল;

  infof(state->তথ্য, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  ফলাফল = tftp_set_timeouts(state);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  ফেরত tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
স্থির CURLcode tftp_rx(কাঠামো tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  পূর্ণসংখ্যা rblock;
  কাঠামো Curl_easy *তথ্য = state->তথ্য;
  অক্ষর বাফার[STRERROR_LEN];

  নির্বাচন(event) {

  ক্ষেত্র TFTP_EVENT_তথ্য:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    যদি(পরবর্তী_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    অন্যথা যদি(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(তথ্য, "Received last DATA packet block %d again.", rblock);
    }
    অন্যথা {
      /* totally unexpected, log it */
      infof(তথ্য,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, পরবর্তী_BLOCKNUM(state->block));
      বিরতি;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                    4, SEND_4TH_ARG,
                    (কাঠামো sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    যদি(sbytes < 0) {
      failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
      ফেরত CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    যদি(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(ফাঁকা);
    বিরতি;

  ক্ষেত্র TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                    4, SEND_4TH_ARG,
                    (কাঠামো sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    যদি(sbytes < 0) {
      failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
      ফেরত CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(ফাঁকা);
    বিরতি;

  ক্ষেত্র TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(তথ্য,
          "Timeout waiting for block %d ACK. Retries = %d",
          পরবর্তী_BLOCKNUM(state->block), state->retries);
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    অন্যথা {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                      4, SEND_4TH_ARG,
                      (কাঠামো sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      যদি(sbytes < 0) {
        failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
        ফেরত CURLE_SEND_ERROR;
      }
    }
    বিরতি;

  ক্ষেত্র TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (শূন্য)sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                 4, SEND_4TH_ARG,
                 (কাঠামো sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    বিরতি;

  পূর্বনির্ধারিত:
    failf(তথ্য, "%s", "tftp_rx: internal error");
    ফেরত CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode tftp_connect_for_rx(কাঠামো tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ফলাফল;

  infof(state->তথ্য, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  ফলাফল = tftp_set_timeouts(state);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  ফেরত tftp_rx(state, event);
}

স্থির CURLcode tftp_send_first(কাঠামো tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ধ্রুবক অক্ষর *mode = "octet";
  অক্ষর *filename;
  কাঠামো Curl_easy *তথ্য = state->তথ্য;
  ধ্রুবক কাঠামো Curl_sockaddr_ex *remote_addr = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  যদি(তথ্য->state.prefer_ascii)
    mode = "netascii";

  নির্বাচন(event) {

  ক্ষেত্র TFTP_EVENT_INIT:    /* Send the first packet out */
  ক্ষেত্র TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    যদি(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ফেরত ফলাফল;
    }

    যদি(তথ্য->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      যদি(তথ্য->state.infilesize != -1)
        Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
    }
    অন্যথা {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    যদি(!state->তথ্য->state.up.path[1]) {
      failf(তথ্য, "Missing filename");
      ফেরত CURLE_TFTP_ILLEGAL;
    }
    ফলাফল = Curl_urldecode(&state->তথ্য->state.up.path[1], 0,
                            &filename, ফাঁকা, REJECT_ZERO);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    যদি(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(তথ্য, "TFTP filename too long");
      curlx_free(filename);
      ফেরত CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((অক্ষর *)state->spacket.তথ্য + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    যদি(!তথ্য->set.tftp_no_options) {
      অক্ষর buf[64];
      /* add tsize option */
      curl_msnprintf(buf, আকার(buf), "%" FMT_OFF_T,
                     তথ্য->state.upload && (তথ্য->state.infilesize != -1) ?
                     তথ্য->state.infilesize : 0);

      ফলাফল = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      যদি(ফলাফল == CURLE_OK)
        ফলাফল = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, আকার(buf), "%u", state->requested_blksize);
      যদি(ফলাফল == CURLE_OK)
        ফলাফল = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      যদি(ফলাফল == CURLE_OK)
        ফলাফল = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, আকার(buf), "%d", state->retry_time);
      যদি(ফলাফল == CURLE_OK)
        ফলাফল = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      যদি(ফলাফল == CURLE_OK)
        ফলাফল = tftp_option_add(state, &sbytes, sbytes, buf);

      যদি(ফলাফল != CURLE_OK) {
        failf(তথ্য, "TFTP buffer too small for options");
        ফেরত CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#অন্যথা
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(তথ্য, FIRSTSOCKET);
    যদি(!remote_addr)
      ফেরত CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (শূন্য *)state->spacket.তথ্য,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    যদি(senddata != (ssize_t)sbytes) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "%s", curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
      ফেরত CURLE_SEND_ERROR;
    }
    বিরতি;

  ক্ষেত্র TFTP_EVENT_OACK:
    যদি(তথ্য->state.upload) {
      ফলাফল = tftp_connect_for_tx(state, event);
    }
    অন্যথা {
      ফলাফল = tftp_connect_for_rx(state, event);
    }
    বিরতি;

  ক্ষেত্র TFTP_EVENT_ACK: /* Connected for transmit */
    যদি(!তথ্য->state.upload)
      ফেরত CURLE_TFTP_ILLEGAL;
    ফলাফল = tftp_connect_for_tx(state, event);
    বিরতি;

  ক্ষেত্র TFTP_EVENT_তথ্য: /* Connected for receive */
    যদি(তথ্য->state.upload)
      ফেরত CURLE_TFTP_ILLEGAL;
    ফলাফল = tftp_connect_for_rx(state, event);
    বিরতি;

  ক্ষেত্র TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    বিরতি;

  পূর্বনির্ধারিত:
    failf(state->তথ্য, "tftp_send_first: internal error");
    ফেরত CURLE_TFTP_ILLEGAL;
  }

  ফেরত ফলাফল;
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
  CURLcode ফলাফল = CURLE_OK;

  যদি(error != TFTP_ERR_NONE) {
    নির্বাচন(error) {
    ক্ষেত্র TFTP_ERR_NOTFOUND:
      ফলাফল = CURLE_TFTP_NOTFOUND;
      বিরতি;
    ক্ষেত্র TFTP_ERR_PERM:
      ফলাফল = CURLE_TFTP_PERM;
      বিরতি;
    ক্ষেত্র TFTP_ERR_DISKFULL:
      ফলাফল = CURLE_REMOTE_DISK_FULL;
      বিরতি;
    ক্ষেত্র TFTP_ERR_UNDEF:
    ক্ষেত্র TFTP_ERR_ILLEGAL:
      ফলাফল = CURLE_TFTP_ILLEGAL;
      বিরতি;
    ক্ষেত্র TFTP_ERR_UNKNOWNID:
      ফলাফল = CURLE_TFTP_UNKNOWNID;
      বিরতি;
    ক্ষেত্র TFTP_ERR_EXISTS:
      ফলাফল = CURLE_REMOTE_FILE_EXISTS;
      বিরতি;
    ক্ষেত্র TFTP_ERR_NOSUCHUSER:
      ফলাফল = CURLE_TFTP_NOSUCHUSER;
      বিরতি;
    ক্ষেত্র TFTP_ERR_TIMEOUT:
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
    ক্ষেত্র TFTP_ERR_NORESPONSE:
      ফলাফল = CURLE_COULDNT_CONNECT;
      বিরতি;
    পূর্বনির্ধারিত:
      ফলাফল = CURLE_ABORTED_BY_CALLBACK;
      বিরতি;
    }
  }
  অন্যথা
    ফলাফল = CURLE_OK;

  ফেরত ফলাফল;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
স্থির CURLcode tftp_state_machine(কাঠামো tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো Curl_easy *তথ্য = state->তথ্য;

  নির্বাচন(state->state) {
  ক্ষেত্র TFTP_STATE_START:
    DEBUGF(infof(তথ্য, "TFTP_STATE_START"));
    ফলাফল = tftp_send_first(state, event);
    বিরতি;
  ক্ষেত্র TFTP_STATE_RX:
    DEBUGF(infof(তথ্য, "TFTP_STATE_RX"));
    ফলাফল = tftp_rx(state, event);
    বিরতি;
  ক্ষেত্র TFTP_STATE_TX:
    DEBUGF(infof(তথ্য, "TFTP_STATE_TX"));
    ফলাফল = tftp_tx(state, event);
    বিরতি;
  ক্ষেত্র TFTP_STATE_FIN:
    infof(তথ্য, "%s", "TFTP finished");
    বিরতি;
  পূর্বনির্ধারিত:
    DEBUGF(infof(তথ্য, "STATE: %d", (পূর্ণসংখ্যা)state->state));
    failf(তথ্য, "%s", "Internal state machine error");
    ফলাফল = CURLE_TFTP_ILLEGAL;
    বিরতি;
  }

  ফেরত ফলাফল;
}

স্থির শূন্য tftp_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো tftp_conn *state = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(state->rpacket.তথ্য);
  curlx_safefree(state->spacket.তথ্য);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
স্থির CURLcode tftp_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো tftp_conn *state;
  পূর্ণসংখ্যা blksize;
  পূর্ণসংখ্যা need_blksize;
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক কাঠামো Curl_sockaddr_ex *remote_addr = ফাঁকা;
  CURLcode ফলাফল;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, আকার(*state));
  যদি(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  যদি(তথ্য->set.tftp_blksize)
    /* range checked when set */
    blksize = (পূর্ণসংখ্যা)তথ্য->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  যদি(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  যদি(!state->rpacket.তথ্য) {
    state->rpacket.তথ্য = curlx_calloc(1, need_blksize + 2 + 2);

    যদি(!state->rpacket.তথ্য)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(!state->spacket.তথ্য) {
    state->spacket.তথ্য = curlx_calloc(1, need_blksize + 2 + 2);

    যদি(!state->spacket.তথ্য)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->তথ্য = তথ্য;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_START;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(তথ্য, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  যদি(!remote_addr)
    ফেরত CURLE_FAILED_INIT;

  ((কাঠামো sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  ফলাফল = tftp_set_timeouts(state);
  যদি(ফলাফল)
    ফেরত ফলাফল;

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
    পূর্ণসংখ্যা rc = bind(state->sockfd, (কাঠামো sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    যদি(rc) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
      ফেরত CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ফেরত CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
স্থির CURLcode tftp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                          bool premature)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (শূন্য)status;
  (শূন্য)premature;

  যদি(Curl_pgrsDone(তথ্য, Curl_pgrs_now(তথ্য)))
    ফেরত CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  যদি(state)
    ফলাফল = tftp_translate_code(state->error);

  ফেরত ফলাফল;
}

স্থির CURLcode tftp_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  ফেরত Curl_pollset_add_in(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
স্থির CURLcode tftp_receive_packet(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো tftp_conn *state)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = আকার(remote_addr);

  /* Receive the packet */
  state->rbytes = (পূর্ণসংখ্যা)recvfrom(state->sockfd,
                                (শূন্য *)state->rpacket.তথ্য,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (কাঠামো sockaddr *)&remote_addr,
                                &fromlen);
  যদি((state->rbytes >= 0) && fromlen) {
    যদি(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      যদি((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(তথ্য, "Data received from another address");
        ফেরত CURLE_RECV_ERROR;
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
    failf(তথ্য, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  অন্যথা {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    নির্বাচন(state->event) {
    ক্ষেত্র TFTP_EVENT_তথ্য:
      যদি(তথ্য->state.upload)
        ফেরত CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      যদি(state->rbytes > 4 &&
         (পরবর্তী_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                   (ধ্রুবক অক্ষর *)state->rpacket.তথ্য + 4,
                                   state->rbytes - 4);
        যদি(ফলাফল) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ফেরত ফলাফল;
        }
      }
      বিরতি;
    ক্ষেত্র TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      ধ্রুবক অক্ষর *str = (ধ্রুবক অক্ষর *)state->rpacket.তথ্য + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      যদি(tftp_strnlen(str, strn) < strn)
        infof(তথ্য, "TFTP error: %s", str);
      বিরতি;
    }
    ক্ষেত্র TFTP_EVENT_ACK:
      বিরতি;
    ক্ষেত্র TFTP_EVENT_OACK:
      ফলাফল = tftp_parse_option_ack(state,
                                     (ধ্রুবক অক্ষর *)state->rpacket.তথ্য + 2,
                                     state->rbytes-2);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      বিরতি;
    ক্ষেত্র TFTP_EVENT_RRQ:
    ক্ষেত্র TFTP_EVENT_WRQ:
    পূর্বনির্ধারিত:
      failf(তথ্য, "%s", "Internal error: Unexpected packet");
      বিরতি;
    }

    /* Update the progress meter */
    ফলাফল = Curl_pgrsUpdate(তথ্য);
    যদি(ফলাফল) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ফেরত ফলাফল;
    }
  }
  ফেরত ফলাফল;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
স্থির timediff_t tftp_state_timeout(কাঠামো tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  যদি(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->তথ্য);
  যদি(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ফেরত timeout_ms;
  }
  current = time(ফাঁকা);
  যদি(current > state->rx_time + state->retry_time) {
    যদি(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(ফাঁকা); /* update even though we received nothing */
  }

  ফেরত timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
স্থির CURLcode tftp_multi_statemach(কাঠামো Curl_easy *তথ্য, bool *done)
{
  tftp_event_t event;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  যদি(!state)
    ফেরত CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  যদি(timeout_ms < 0) {
    failf(তথ্য, "TFTP response timeout");
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }
  যদি(event != TFTP_EVENT_NONE) {
    ফলাফল = tftp_state_machine(state, event);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    *done = (state->state == TFTP_STATE_FIN);
    যদি(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(তথ্য);
  }
  অন্যথা {
    /* no timeouts to handle, check our socket */
    পূর্ণসংখ্যা rc = SOCKET_READABLE(state->sockfd, 0);

    যদি(rc == -1) {
      /* bail out */
      পূর্ণসংখ্যা sockerr = SOCKERRNO;
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "%s", curlx_strerror(sockerr, বাফার, আকার(বাফার)));
      state->event = TFTP_EVENT_ERROR;
    }
    অন্যথা যদি(rc) {
      ফলাফল = tftp_receive_packet(তথ্য, state);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      ফলাফল = tftp_state_machine(state, state->event);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      *done = (state->state == TFTP_STATE_FIN);
      যদি(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(তথ্য);
    }
    /* if rc == 0, then select() timed out */
  }

  ফেরত ফলাফল;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
স্থির CURLcode tftp_doing(কাঠামো Curl_easy *তথ্য, bool *dophase_done)
{
  CURLcode ফলাফল;
  ফলাফল = tftp_multi_statemach(তথ্য, dophase_done);

  যদি(*dophase_done) {
    DEBUGF(infof(তথ্য, "DO phase is complete"));
  }
  অন্যথা যদি(!ফলাফল) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    ফলাফল = Curl_pgrsCheck(তথ্য);
  }
  ফেরত ফলাফল;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
স্থির CURLcode tftp_perform(কাঠামো Curl_easy *তথ্য, bool *dophase_done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  যদি(!state)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = tftp_state_machine(state, TFTP_EVENT_INIT);

  যদি((state->state == TFTP_STATE_FIN) || ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = tftp_multi_statemach(তথ্য, dophase_done);

  যদি(*dophase_done)
    DEBUGF(infof(তথ্য, "DO phase is complete"));

  ফেরত ফলাফল;
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

স্থির CURLcode tftp_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode ফলাফল;

  *done = FALSE;

  যদি(!state) {
    ফলাফল = tftp_connect(তথ্য, done);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    যদি(!state)
      ফেরত CURLE_TFTP_ILLEGAL;
  }

  ফলাফল = tftp_perform(তথ্য, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  যদি(!ফলাফল)
    /* If we have encountered an internal tftp error, translate it. */
    ফলাফল = tftp_translate_code(state->error);

  ফেরত ফলাফল;
}

স্থির CURLcode tftp_setup_connection(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  অক্ষর *path = তথ্য->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  যদি((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    তথ্য->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  অন্যথা যদি((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    তথ্য->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ফেরত CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_tftp = {
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
