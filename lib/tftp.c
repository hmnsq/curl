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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "tftp.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "sockaddr.h" /* required for Curl_sockaddr_storage */
#అన్తర్భావ "url.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#నిర్వచించు TFTP_BLKSIZE_DEFAULT 512
#నిర్వచించు TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#నిర్వచించు TFTP_OPTION_TSIZE    "tsize"
#నిర్వచించు TFTP_OPTION_INTERVAL "timeout"

రకనిర్వచనం గణన {
  TFTP_బహుళకం_NETASCII = 0,
  TFTP_బహుళకం_OCTET
} tftp_బహుళకం_t;

రకనిర్వచనం గణన {
  TFTP_STATE_ప్రారంభం = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

రకనిర్వచనం గణన {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_దత్తాంశం = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

రకనిర్వచనం గణన {
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

నిర్మాణం tftp_packet {
  గుర్తులేని అక్షరం *దత్తాంశం;
};

/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

నిర్మాణం tftp_conn {
  నిర్మాణం Curl_sockaddr_storage local_addr;
  నిర్మాణం Curl_sockaddr_storage remote_addr;
  నిర్మాణం tftp_packet rpacket;
  నిర్మాణం tftp_packet spacket;
  tftp_state_t    state;
  tftp_బహుళకం_t     బహుళకం;
  tftp_error_t    error;
  tftp_event_t    event;
  నిర్మాణం Curl_easy *దత్తాంశం;
  curl_socket_t   sockfd;
  పూర్ణాంకం             retries;
  పూర్ణాంకం             retry_సమయం;
  పూర్ణాంకం             retry_max;
  time_t          rx_సమయం;
  curl_socklen_t  remote_addrlen;
  పూర్ణాంకం             rbytes;
  size_t          sbytes;
  గుర్తులేని పూర్ణాంకం    blksize;
  గుర్తులేని పూర్ణాంకం    requested_blksize;
  గుర్తులేని పొట్టి  ఖండం;
  బిట్టు(remote_pinned);
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
స్థిర CURLcode tftp_సమితి_timeouts(నిర్మాణం tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->దత్తాంశం);

  ఒకవేళ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->దత్తాంశం, "Connection time-out");
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ఒకవేళ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  లేకపోతే
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (పూర్ణాంకం)timeout / 5;

  /* Bound the total number */
  ఒకవేళ(state->retry_max < 3)
    state->retry_max = 3;

  ఒకవేళ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_సమయం = (పూర్ణాంకం)(timeout / state->retry_max);
  ఒకవేళ(state->retry_సమయం < 1)
    state->retry_సమయం = 1;

  infof(state->దత్తాంశం,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (పూర్ణాంకం)state->state, timeout_ms, state->retry_సమయం, state->retry_max);

  /* init RX time */
  state->rx_సమయం = సమయం(ఖాళీ);

  తిరిగి CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

స్థిర శూన్య setpacketevent(నిర్మాణం tftp_packet *packet, గుర్తులేని పొట్టి అంకె)
{
  packet->దత్తాంశం[0] = (గుర్తులేని అక్షరం)(అంకె >> 8);
  packet->దత్తాంశం[1] = (గుర్తులేని అక్షరం)(అంకె & 0xff);
}

స్థిర శూన్య setpacketblock(నిర్మాణం tftp_packet *packet, గుర్తులేని పొట్టి అంకె)
{
  packet->దత్తాంశం[2] = (గుర్తులేని అక్షరం)(అంకె >> 8);
  packet->దత్తాంశం[3] = (గుర్తులేని అక్షరం)(అంకె & 0xff);
}

స్థిర గుర్తులేని పొట్టి getrpacketevent(స్థిరరాశి నిర్మాణం tftp_packet *packet)
{
  తిరిగి (గుర్తులేని పొట్టి)((packet->దత్తాంశం[0] << 8) | packet->దత్తాంశం[1]);
}

స్థిర గుర్తులేని పొట్టి getrpacketblock(స్థిరరాశి నిర్మాణం tftp_packet *packet)
{
  తిరిగి (గుర్తులేని పొట్టి)((packet->దత్తాంశం[2] << 8) | packet->దత్తాంశం[3]);
}

స్థిర size_t tftp_strnlen(స్థిరరాశి అక్షరం *string, size_t maxlen)
{
  స్థిరరాశి అక్షరం *చివర = memchr(string, '\0', maxlen);
  తిరిగి చివర ? (size_t)(చివర - string) : maxlen;
}

స్థిర స్థిరరాశి అక్షరం *tftp_option_get(స్థిరరాశి అక్షరం *buf, size_t len,
                                   స్థిరరాశి అక్షరం **option, స్థిరరాశి అక్షరం **విలువ)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ఒకవేళ(loc >= len)
    తిరిగి ఖాళీ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ఒకవేళ(loc > len)
    తిరిగి ఖాళీ;
  *విలువ = &buf[అక్షరశ్రేణి_పొడవు(*option) + 1];

  తిరిగి &buf[loc];
}

స్థిర CURLcode tftp_parse_option_ack(నిర్మాణం tftp_conn *state,
                                      స్థిరరాశి అక్షరం *ptr, పూర్ణాంకం len)
{
  స్థిరరాశి అక్షరం *tmp = ptr;
  నిర్మాణం Curl_easy *దత్తాంశం = state->దత్తాంశం;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  అయితే(tmp < ptr + len) {
    స్థిరరాశి అక్షరం *option, *విలువ;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &విలువ);
    ఒకవేళ(!tmp) {
      failf(దత్తాంశం, "Malformed ACK packet, rejecting");
      తిరిగి CURLE_TFTP_ILLEGAL;
    }
    olen = అక్షరశ్రేణి_పొడవు(option);

    infof(దత్తాంశం, "got option=(%s) value=(%s)", option, విలువ);

    ఒకవేళ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ఒకవేళ(curlx_str_సంఖ్య(&విలువ, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(దత్తాంశం, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        తిరిగి CURLE_TFTP_ILLEGAL;
      }
      ఒకవేళ(!blksize) {
        failf(దత్తాంశం, "invalid blocksize value in OACK packet");
        తిరిగి CURLE_TFTP_ILLEGAL;
      }
      లేకపోతే ఒకవేళ(blksize < TFTP_BLKSIZE_MIN) {
        failf(దత్తాంశం, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        తిరిగి CURLE_TFTP_ILLEGAL;
      }
      లేకపోతే ఒకవేళ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(దత్తాంశం, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        తిరిగి CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (గుర్తులేని పూర్ణాంకం)blksize;
      infof(దత్తాంశం, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    లేకపోతే ఒకవేళ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ఒకవేళ(!దత్తాంశం->state.upload &&
         !curlx_str_సంఖ్య(&విలువ, &tsize, CURL_OFF_T_MAX)) {
        ఒకవేళ(!tsize) {
          failf(దత్తాంశం, "invalid tsize -:%s:- value in OACK packet", విలువ);
          తిరిగి CURLE_TFTP_ILLEGAL;
        }
        infof(దత్తాంశం, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, tsize);
      }
    }
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode tftp_option_కూడు(నిర్మాణం tftp_conn *state, size_t *csize,
                                size_t సూచీ, స్థిరరాశి అక్షరం *option)
{
  అక్షరం *buf = (అక్షరం *)&state->spacket.దత్తాంశం[సూచీ];
  size_t oplen = అక్షరశ్రేణి_పొడవు(option);
  size_t blen;
  ఒకవేళ((state->blksize <= సూచీ) ||
     (oplen + 1) > (size_t)(state->blksize - సూచీ))
    తిరిగి CURLE_TFTP_ILLEGAL;
  blen = state->blksize - సూచీ;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  తిరిగి CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#నిర్వచించు తదుపరి_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
స్థిర CURLcode tftp_tx(నిర్మాణం tftp_conn *state, tftp_event_t event)
{
  నిర్మాణం Curl_easy *దత్తాంశం = state->దత్తాంశం;
  ssize_t sbytes;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  size_t cb; /* Bytes currently read */
  అక్షరం బఫరు[STRERROR_LEN];
  అక్షరం *bufptr;
  bool eos;

  ఒకవేళ(!దత్తాంశం->state.upload)
    తిరిగి CURLE_TFTP_ILLEGAL;

  ఎంపిక(event) {

  స్థితి TFTP_EVENT_ACK:
  స్థితి TFTP_EVENT_OACK:
    ఒకవేళ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      పూర్ణాంకం rblock = getrpacketblock(&state->rpacket);

      ఒకవేళ(rblock != state->ఖండం &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->ఖండం == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(దత్తాంశం, "Received ACK for block %d, expecting %d",
              rblock, state->ఖండం);
        state->retries++;
        /* Bail out if over the maximum */
        ఒకవేళ(state->retries > state->retry_max) {
          failf(దత్తాంశం, "tftp_tx: giving up waiting for block %d ack",
                state->ఖండం);
          ఫలితం = CURLE_SEND_ERROR;
        }
        లేకపోతే {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (నిర్మాణం sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ఒకవేళ(sbytes < 0) {
            failf(దత్తాంశం, "%s",
                  curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
            ఫలితం = CURLE_SEND_ERROR;
          }
        }

        తిరిగి ఫలితం;
      }
      /* This is the expected packet. Reset the counters and send the next
ఖండం*/
      state->rx_సమయం = సమయం(ఖాళీ);
      state->ఖండం++;
    }
    లేకపోతే
      state->ఖండం = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_దత్తాంశం);
    setpacketblock(&state->spacket, state->ఖండం);
    ఒకవేళ(state->ఖండం > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      తిరిగి CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (అక్షరం *)state->spacket.దత్తాంశం + 4;
    చేయి {
      ఫలితం = Curl_client_read(దత్తాంశం, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      state->sbytes += cb;
      bufptr += cb;
    } అయితే(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (నిర్మాణం sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ఒకవేళ(sbytes < 0) {
      failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(దత్తాంశం, state->sbytes);
    ఆపు;

  స్థితి TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(దత్తాంశం, "Timeout waiting for block %d ACK. "
          "Retries = %d", తదుపరి_BLOCKNUM(state->ఖండం), state->retries);
    /* Decide if we have had enough */
    ఒకవేళ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    లేకపోతే {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (నిర్మాణం sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ఒకవేళ(sbytes < 0) {
        failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
        తిరిగి CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_సమితి_Upload_Counter(దత్తాంశం, k->writebytecount);
    }
    ఆపు;

  స్థితి TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ఖండం);
    (శూన్య)sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం, 4, SEND_4TH_ARG,
                 (నిర్మాణం sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ఆపు;

  అప్రమేయ:
    failf(దత్తాంశం, "tftp_tx: internal error, event: %d", (పూర్ణాంకం)event);
    ఆపు;
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode tftp_connect_for_tx(నిర్మాణం tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ఫలితం;

  infof(state->దత్తాంశం, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  ఫలితం = tftp_సమితి_timeouts(state);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  తిరిగి tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
స్థిర CURLcode tftp_rx(నిర్మాణం tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  పూర్ణాంకం rblock;
  నిర్మాణం Curl_easy *దత్తాంశం = state->దత్తాంశం;
  అక్షరం బఫరు[STRERROR_LEN];

  ఎంపిక(event) {

  స్థితి TFTP_EVENT_దత్తాంశం:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ఒకవేళ(తదుపరి_BLOCKNUM(state->ఖండం) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    లేకపోతే ఒకవేళ(state->ఖండం == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(దత్తాంశం, "Received last DATA packet block %d again.", rblock);
    }
    లేకపోతే {
      /* totally unexpected, log it */
      infof(దత్తాంశం,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, తదుపరి_BLOCKNUM(state->ఖండం));
      ఆపు;
    }

    /* ACK this block. */
    state->ఖండం = (గుర్తులేని పొట్టి)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ఖండం);
    sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                    4, SEND_4TH_ARG,
                    (నిర్మాణం sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ఒకవేళ(sbytes < 0) {
      failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ఒకవేళ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    లేకపోతే {
      state->state = TFTP_STATE_RX;
    }
    state->rx_సమయం = సమయం(ఖాళీ);
    ఆపు;

  స్థితి TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->ఖండం = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ఖండం);
    sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                    4, SEND_4TH_ARG,
                    (నిర్మాణం sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ఒకవేళ(sbytes < 0) {
      failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_సమయం = సమయం(ఖాళీ);
    ఆపు;

  స్థితి TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(దత్తాంశం,
          "Timeout waiting for block %d ACK. Retries = %d",
          తదుపరి_BLOCKNUM(state->ఖండం), state->retries);
    ఒకవేళ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    లేకపోతే {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                      4, SEND_4TH_ARG,
                      (నిర్మాణం sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ఒకవేళ(sbytes < 0) {
        failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
        తిరిగి CURLE_SEND_ERROR;
      }
    }
    ఆపు;

  స్థితి TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ఖండం);
    (శూన్య)sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                 4, SEND_4TH_ARG,
                 (నిర్మాణం sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ఆపు;

  అప్రమేయ:
    failf(దత్తాంశం, "%s", "tftp_rx: internal error");
    తిరిగి CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode tftp_connect_for_rx(నిర్మాణం tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ఫలితం;

  infof(state->దత్తాంశం, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  ఫలితం = tftp_సమితి_timeouts(state);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  తిరిగి tftp_rx(state, event);
}

స్థిర CURLcode tftp_send_మొదటి(నిర్మాణం tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  స్థిరరాశి అక్షరం *బహుళకం = "octet";
  అక్షరం *filename;
  నిర్మాణం Curl_easy *దత్తాంశం = state->దత్తాంశం;
  స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *remote_addr = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ఒకవేళ(దత్తాంశం->state.prefer_ascii)
    బహుళకం = "netascii";

  ఎంపిక(event) {

  స్థితి TFTP_EVENT_INIT:    /* Send the first packet out */
  స్థితి TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ఒకవేళ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      తిరిగి ఫలితం;
    }

    ఒకవేళ(దత్తాంశం->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ఒకవేళ(దత్తాంశం->state.infilesize != -1)
        Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
    }
    లేకపోతే {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ఒకవేళ(!state->దత్తాంశం->state.పైకి.మార్గం[1]) {
      failf(దత్తాంశం, "Missing filename");
      తిరిగి CURLE_TFTP_ILLEGAL;
    }
    ఫలితం = Curl_urldecode(&state->దత్తాంశం->state.పైకి.మార్గం[1], 0,
                            &filename, ఖాళీ, REJECT_ZERO);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ(అక్షరశ్రేణి_పొడవు(filename) + అక్షరశ్రేణి_పొడవు(బహుళకం) + 4 > state->blksize) {
      failf(దత్తాంశం, "TFTP filename too long");
      curlx_free(filename);
      తిరిగి CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((అక్షరం *)state->spacket.దత్తాంశం + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', బహుళకం, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ఒకవేళ(!దత్తాంశం->సమితి.tftp_no_options) {
      అక్షరం buf[64];
      /* add tsize option */
      curl_msnprintf(buf, పరిమాణం(buf), "%" FMT_OFF_T,
                     దత్తాంశం->state.upload && (దత్తాంశం->state.infilesize != -1) ?
                     దత్తాంశం->state.infilesize : 0);

      ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ఒకవేళ(ఫలితం == CURLE_OK)
        ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, పరిమాణం(buf), "%u", state->requested_blksize);
      ఒకవేళ(ఫలితం == CURLE_OK)
        ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ఒకవేళ(ఫలితం == CURLE_OK)
        ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, పరిమాణం(buf), "%d", state->retry_సమయం);
      ఒకవేళ(ఫలితం == CURLE_OK)
        ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ఒకవేళ(ఫలితం == CURLE_OK)
        ఫలితం = tftp_option_కూడు(state, &sbytes, sbytes, buf);

      ఒకవేళ(ఫలితం != CURLE_OK) {
        failf(దత్తాంశం, "TFTP buffer too small for options");
        తిరిగి CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#నిర్వచితమైతే __AMIGA__
#నిర్వచించు CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#లేకపోతే
#నిర్వచించు CURL_SENDTO_ARG5(x) (x)
#అయితే_ముగింపు
    remote_addr = Curl_conn_get_remote_addr(దత్తాంశం, FIRSTSOCKET);
    ఒకవేళ(!remote_addr)
      తిరిగి CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (శూన్య *)state->spacket.దత్తాంశం,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ఒకవేళ(senddata != (ssize_t)sbytes) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "%s", curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SEND_ERROR;
    }
    ఆపు;

  స్థితి TFTP_EVENT_OACK:
    ఒకవేళ(దత్తాంశం->state.upload) {
      ఫలితం = tftp_connect_for_tx(state, event);
    }
    లేకపోతే {
      ఫలితం = tftp_connect_for_rx(state, event);
    }
    ఆపు;

  స్థితి TFTP_EVENT_ACK: /* Connected for transmit */
    ఒకవేళ(!దత్తాంశం->state.upload)
      తిరిగి CURLE_TFTP_ILLEGAL;
    ఫలితం = tftp_connect_for_tx(state, event);
    ఆపు;

  స్థితి TFTP_EVENT_దత్తాంశం: /* Connected for receive */
    ఒకవేళ(దత్తాంశం->state.upload)
      తిరిగి CURLE_TFTP_ILLEGAL;
    ఫలితం = tftp_connect_for_rx(state, event);
    ఆపు;

  స్థితి TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ఆపు;

  అప్రమేయ:
    failf(state->దత్తాంశం, "tftp_send_first: internal error");
    తిరిగి CURLE_TFTP_ILLEGAL;
  }

  తిరిగి ఫలితం;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
స్థిర CURLcode tftp_translate_సంకేతం(tftp_error_t error)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(error != TFTP_ERR_NONE) {
    ఎంపిక(error) {
    స్థితి TFTP_ERR_NOTFOUND:
      ఫలితం = CURLE_TFTP_NOTFOUND;
      ఆపు;
    స్థితి TFTP_ERR_PERM:
      ఫలితం = CURLE_TFTP_PERM;
      ఆపు;
    స్థితి TFTP_ERR_DISKFULL:
      ఫలితం = CURLE_REMOTE_DISK_నిండిన;
      ఆపు;
    స్థితి TFTP_ERR_UNDEF:
    స్థితి TFTP_ERR_ILLEGAL:
      ఫలితం = CURLE_TFTP_ILLEGAL;
      ఆపు;
    స్థితి TFTP_ERR_UNKNOWNID:
      ఫలితం = CURLE_TFTP_UNKNOWNID;
      ఆపు;
    స్థితి TFTP_ERR_EXISTS:
      ఫలితం = CURLE_REMOTE_FILE_EXISTS;
      ఆపు;
    స్థితి TFTP_ERR_NOSUCHUSER:
      ఫలితం = CURLE_TFTP_NOSUCHUSER;
      ఆపు;
    స్థితి TFTP_ERR_TIMEOUT:
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    స్థితి TFTP_ERR_NORESPONSE:
      ఫలితం = CURLE_COULDNT_CONNECT;
      ఆపు;
    అప్రమేయ:
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
      ఆపు;
    }
  }
  లేకపోతే
    ఫలితం = CURLE_OK;

  తిరిగి ఫలితం;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
స్థిర CURLcode tftp_state_machine(నిర్మాణం tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం Curl_easy *దత్తాంశం = state->దత్తాంశం;

  ఎంపిక(state->state) {
  స్థితి TFTP_STATE_ప్రారంభం:
    DEBUGF(infof(దత్తాంశం, "TFTP_STATE_START"));
    ఫలితం = tftp_send_మొదటి(state, event);
    ఆపు;
  స్థితి TFTP_STATE_RX:
    DEBUGF(infof(దత్తాంశం, "TFTP_STATE_RX"));
    ఫలితం = tftp_rx(state, event);
    ఆపు;
  స్థితి TFTP_STATE_TX:
    DEBUGF(infof(దత్తాంశం, "TFTP_STATE_TX"));
    ఫలితం = tftp_tx(state, event);
    ఆపు;
  స్థితి TFTP_STATE_FIN:
    infof(దత్తాంశం, "%s", "TFTP finished");
    ఆపు;
  అప్రమేయ:
    DEBUGF(infof(దత్తాంశం, "STATE: %d", (పూర్ణాంకం)state->state));
    failf(దత్తాంశం, "%s", "Internal state machine error");
    ఫలితం = CURLE_TFTP_ILLEGAL;
    ఆపు;
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య tftp_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం tftp_conn *state = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_safefree(state->rpacket.దత్తాంశం);
  curlx_safefree(state->spacket.దత్తాంశం);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
స్థిర CURLcode tftp_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం tftp_conn *state;
  పూర్ణాంకం blksize;
  పూర్ణాంకం need_blksize;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *remote_addr = ఖాళీ;
  CURLcode ఫలితం;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, పరిమాణం(*state));
  ఒకవేళ(!state ||
     Curl_conn_meta_సమితి(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* alloc pkt buffers based on specified blksize */
  ఒకవేళ(దత్తాంశం->సమితి.tftp_blksize)
    /* range checked when set */
    blksize = (పూర్ణాంకం)దత్తాంశం->సమితి.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ఒకవేళ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ఒకవేళ(!state->rpacket.దత్తాంశం) {
    state->rpacket.దత్తాంశం = curlx_calloc(1, need_blksize + 2 + 2);

    ఒకవేళ(!state->rpacket.దత్తాంశం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(!state->spacket.దత్తాంశం) {
    state->spacket.దత్తాంశం = curlx_calloc(1, need_blksize + 2 + 2);

    ఒకవేళ(!state->spacket.దత్తాంశం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->దత్తాంశం = దత్తాంశం;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_ప్రారంభం;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(దత్తాంశం, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  ఒకవేళ(!remote_addr)
    తిరిగి CURLE_FAILED_INIT;

  ((నిర్మాణం sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  ఫలితం = tftp_సమితి_timeouts(state);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!conn->బిట్లు.bound) {
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
    పూర్ణాంకం rc = bind(state->sockfd, (నిర్మాణం sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    ఒకవేళ(rc) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_COULDNT_CONNECT;
    }
    conn->బిట్లు.bound = TRUE;
  }

  *పూర్తి = TRUE;

  తిరిగి CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
స్థిర CURLcode tftp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (శూన్య)స్థితి_2;
  (శూన్య)premature;

  ఒకవేళ(Curl_pgrs_పూర్తి(దత్తాంశం, Curl_pgrs_now(దత్తాంశం)))
    తిరిగి CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ఒకవేళ(state)
    ఫలితం = tftp_translate_సంకేతం(state->error);

  తిరిగి ఫలితం;
}

స్థిర CURLcode tftp_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  తిరిగి Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
స్థిర CURLcode tftp_receive_packet(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం tftp_conn *state)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = పరిమాణం(remote_addr);

  /* Receive the packet */
  state->rbytes = (పూర్ణాంకం)recvfrom(state->sockfd,
                                (శూన్య *)state->rpacket.దత్తాంశం,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (నిర్మాణం sockaddr *)&remote_addr,
                                &fromlen);
  ఒకవేళ((state->rbytes >= 0) && fromlen) {
    ఒకవేళ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ఒకవేళ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(దత్తాంశం, "Data received from another address");
        తిరిగి CURLE_RECV_ERROR;
      }
    }
    లేకపోతే {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      జ్ఞాపకం_నకలు(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ఒకవేళ(state->rbytes < 4) {
    failf(దత్తాంశం, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  లేకపోతే {
    /* The event is given by the TFTP packet time */
    గుర్తులేని పొట్టి event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ఎంపిక(state->event) {
    స్థితి TFTP_EVENT_దత్తాంశం:
      ఒకవేళ(దత్తాంశం->state.upload)
        తిరిగి CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ఒకవేళ(state->rbytes > 4 &&
         (తదుపరి_BLOCKNUM(state->ఖండం) == getrpacketblock(&state->rpacket))) {
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                   (స్థిరరాశి అక్షరం *)state->rpacket.దత్తాంశం + 4,
                                   state->rbytes - 4);
        ఒకవేళ(ఫలితం) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          తిరిగి ఫలితం;
        }
      }
      ఆపు;
    స్థితి TFTP_EVENT_ERROR: {
      గుర్తులేని పొట్టి error = getrpacketblock(&state->rpacket);
      స్థిరరాశి అక్షరం *str = (స్థిరరాశి అక్షరం *)state->rpacket.దత్తాంశం + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ఒకవేళ(tftp_strnlen(str, strn) < strn)
        infof(దత్తాంశం, "TFTP error: %s", str);
      ఆపు;
    }
    స్థితి TFTP_EVENT_ACK:
      ఆపు;
    స్థితి TFTP_EVENT_OACK:
      ఫలితం = tftp_parse_option_ack(state,
                                     (స్థిరరాశి అక్షరం *)state->rpacket.దత్తాంశం + 2,
                                     state->rbytes-2);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఆపు;
    స్థితి TFTP_EVENT_RRQ:
    స్థితి TFTP_EVENT_WRQ:
    అప్రమేయ:
      failf(దత్తాంశం, "%s", "Internal error: Unexpected packet");
      ఆపు;
    }

    /* Update the progress meter */
    ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
    ఒకవేళ(ఫలితం) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      తిరిగి ఫలితం;
    }
  }
  తిరిగి ఫలితం;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
స్థిర timediff_t tftp_state_timeout(నిర్మాణం tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  ఒకవేళ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->దత్తాంశం);
  ఒకవేళ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    తిరిగి timeout_ms;
  }
  current = సమయం(ఖాళీ);
  ఒకవేళ(current > state->rx_సమయం + state->retry_సమయం) {
    ఒకవేళ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_సమయం = సమయం(ఖాళీ); /* update even though we received nothing */
  }

  తిరిగి timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
స్థిర CURLcode tftp_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  tftp_event_t event;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *పూర్తి = FALSE;
  ఒకవేళ(!state)
    తిరిగి CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ఒకవేళ(timeout_ms < 0) {
    failf(దత్తాంశం, "TFTP response timeout");
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }
  ఒకవేళ(event != TFTP_EVENT_NONE) {
    ఫలితం = tftp_state_machine(state, event);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    *పూర్తి = (state->state == TFTP_STATE_FIN);
    ఒకవేళ(*పూర్తి)
      /* Tell curl we are done */
      Curl_xfer_అమరిక_nop(దత్తాంశం);
  }
  లేకపోతే {
    /* no timeouts to handle, check our socket */
    పూర్ణాంకం rc = SOCKET_READABLE(state->sockfd, 0);

    ఒకవేళ(rc == -1) {
      /* bail out */
      పూర్ణాంకం sockerr = SOCKERRNO;
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "%s", curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
      state->event = TFTP_EVENT_ERROR;
    }
    లేకపోతే ఒకవేళ(rc) {
      ఫలితం = tftp_receive_packet(దత్తాంశం, state);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఫలితం = tftp_state_machine(state, state->event);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      *పూర్తి = (state->state == TFTP_STATE_FIN);
      ఒకవేళ(*పూర్తి)
        /* Tell curl we are done */
        Curl_xfer_అమరిక_nop(దత్తాంశం);
    }
    /* if rc == 0, then select() timed out */
  }

  తిరిగి ఫలితం;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
స్థిర CURLcode tftp_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  CURLcode ఫలితం;
  ఫలితం = tftp_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(*dophase_పూర్తి) {
    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));
  }
  లేకపోతే ఒకవేళ(!ఫలితం) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    ఫలితం = Curl_pgrsCheck(దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
స్థిర CURLcode tftp_perform(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_పూర్తి = FALSE;
  ఒకవేళ(!state)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = tftp_state_machine(state, TFTP_EVENT_INIT);

  ఒకవేళ((state->state == TFTP_STATE_FIN) || ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = tftp_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(*dophase_పూర్తి)
    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));

  తిరిగి ఫలితం;
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

స్థిర CURLcode tftp_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode ఫలితం;

  *పూర్తి = FALSE;

  ఒకవేళ(!state) {
    ఫలితం = tftp_connect(దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ఒకవేళ(!state)
      తిరిగి CURLE_TFTP_ILLEGAL;
  }

  ఫలితం = tftp_perform(దత్తాంశం, పూర్తి);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ఒకవేళ(!ఫలితం)
    /* If we have encountered an internal tftp error, translate it. */
    ఫలితం = tftp_translate_సంకేతం(state->error);

  తిరిగి ఫలితం;
}

స్థిర CURLcode tftp_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  అక్షరం *మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  size_t len = అక్షరశ్రేణి_పొడవు(మార్గం);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ఒకవేళ((len >= 14) && !memcmp(&మార్గం[len - 14], ";mode=netascii", 14)) {
    దత్తాంశం->state.prefer_ascii = TRUE;
    మార్గం[len - 14] = 0; /* cut it there */
  }
  లేకపోతే ఒకవేళ((len >= 11) && !memcmp(&మార్గం[len - 11], ";mode=octet", 11)) {
    దత్తాంశం->state.prefer_ascii = FALSE;
    మార్గం[len - 11] = 0; /* cut it there */
  }

  తిరిగి CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_tftp = {
  tftp_అమరిక_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_పూర్తి,                            /* done */
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

#అయితే_ముగింపు
