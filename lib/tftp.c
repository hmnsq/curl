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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tftp.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFTP_BLKSIZE_DEFAULT 512
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFTP_OPTION_TSIZE    "tsize"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFTP_OPTION_INTERVAL "timeout"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  TFTP_ਬਹੁਲਕ_NETASCII = 0,
  TFTP_ਬਹੁਲਕ_OCTET
} tftp_ਬਹੁਲਕ_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  TFTP_STATE_ਸ਼ੁਰੂ = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_ਅੰਕੜੇ = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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

ਬਣਤਰ tftp_packet {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ;
};

/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ਬਣਤਰ tftp_conn {
  ਬਣਤਰ Curl_sockaddr_storage local_addr;
  ਬਣਤਰ Curl_sockaddr_storage remote_addr;
  ਬਣਤਰ tftp_packet rpacket;
  ਬਣਤਰ tftp_packet spacket;
  tftp_state_t    state;
  tftp_ਬਹੁਲਕ_t     ਬਹੁਲਕ;
  tftp_error_t    error;
  tftp_event_t    event;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  curl_socket_t   sockfd;
  ਪੂਰਨ             retries;
  ਪੂਰਨ             retry_ਸਮਾਂ;
  ਪੂਰਨ             retry_max;
  time_t          rx_ਸਮਾਂ;
  curl_socklen_t  remote_addrlen;
  ਪੂਰਨ             rbytes;
  size_t          sbytes;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    blksize;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    requested_blksize;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ  ਖੰਡ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(remote_pinned);
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
ਸਥਿਰ CURLcode tftp_ਸੈੱਟ_timeouts(ਬਣਤਰ tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->ਅੰਕੜੇ);

  ਜੇ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->ਅੰਕੜੇ, "Connection time-out");
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ਜੇ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ਨਹੀਂਤਾਂ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (ਪੂਰਨ)timeout / 5;

  /* Bound the total number */
  ਜੇ(state->retry_max < 3)
    state->retry_max = 3;

  ਜੇ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_ਸਮਾਂ = (ਪੂਰਨ)(timeout / state->retry_max);
  ਜੇ(state->retry_ਸਮਾਂ < 1)
    state->retry_ਸਮਾਂ = 1;

  infof(state->ਅੰਕੜੇ,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (ਪੂਰਨ)state->state, timeout_ms, state->retry_ਸਮਾਂ, state->retry_max);

  /* init RX time */
  state->rx_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);

  ਵਾਪਸ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ਸਥਿਰ ਖਾਲੀ setpacketevent(ਬਣਤਰ tftp_packet *packet, ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਅੰਕ)
{
  packet->ਅੰਕੜੇ[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਅੰਕ >> 8);
  packet->ਅੰਕੜੇ[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਅੰਕ & 0xff);
}

ਸਥਿਰ ਖਾਲੀ setpacketblock(ਬਣਤਰ tftp_packet *packet, ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਅੰਕ)
{
  packet->ਅੰਕੜੇ[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਅੰਕ >> 8);
  packet->ਅੰਕੜੇ[3] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਅੰਕ & 0xff);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ getrpacketevent(ਅਚਲ ਬਣਤਰ tftp_packet *packet)
{
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)((packet->ਅੰਕੜੇ[0] << 8) | packet->ਅੰਕੜੇ[1]);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ getrpacketblock(ਅਚਲ ਬਣਤਰ tftp_packet *packet)
{
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)((packet->ਅੰਕੜੇ[2] << 8) | packet->ਅੰਕੜੇ[3]);
}

ਸਥਿਰ size_t tftp_strnlen(ਅਚਲ ਅੱਖਰ *string, size_t maxlen)
{
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = memchr(string, '\0', maxlen);
  ਵਾਪਸ ਸਿਰਾ ? (size_t)(ਸਿਰਾ - string) : maxlen;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *tftp_option_get(ਅਚਲ ਅੱਖਰ *buf, size_t len,
                                   ਅਚਲ ਅੱਖਰ **option, ਅਚਲ ਅੱਖਰ **ਮੁੱਲ)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ਜੇ(loc >= len)
    ਵਾਪਸ ਨਿਰਾ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ਜੇ(loc > len)
    ਵਾਪਸ ਨਿਰਾ;
  *ਮੁੱਲ = &buf[ਲੜੀ_ਲੰਬਾਈ(*option) + 1];

  ਵਾਪਸ &buf[loc];
}

ਸਥਿਰ CURLcode tftp_parse_option_ack(ਬਣਤਰ tftp_conn *state,
                                      ਅਚਲ ਅੱਖਰ *ptr, ਪੂਰਨ len)
{
  ਅਚਲ ਅੱਖਰ *tmp = ptr;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = state->ਅੰਕੜੇ;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ਜਦੋਂ(tmp < ptr + len) {
    ਅਚਲ ਅੱਖਰ *option, *ਮੁੱਲ;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &ਮੁੱਲ);
    ਜੇ(!tmp) {
      failf(ਅੰਕੜੇ, "Malformed ACK packet, rejecting");
      ਵਾਪਸ CURLE_TFTP_ILLEGAL;
    }
    olen = ਲੜੀ_ਲੰਬਾਈ(option);

    infof(ਅੰਕੜੇ, "got option=(%s) value=(%s)", option, ਮੁੱਲ);

    ਜੇ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ਜੇ(curlx_str_ਸੰਖਿਆ(&ਮੁੱਲ, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(ਅੰਕੜੇ, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      }
      ਜੇ(!blksize) {
        failf(ਅੰਕੜੇ, "invalid blocksize value in OACK packet");
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      }
      ਨਹੀਂਤਾਂ ਜੇ(blksize < TFTP_BLKSIZE_MIN) {
        failf(ਅੰਕੜੇ, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      }
      ਨਹੀਂਤਾਂ ਜੇ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(ਅੰਕੜੇ, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)blksize;
      infof(ਅੰਕੜੇ, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ਨਹੀਂਤਾਂ ਜੇ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ਜੇ(!ਅੰਕੜੇ->state.upload &&
         !curlx_str_ਸੰਖਿਆ(&ਮੁੱਲ, &tsize, CURL_OFF_T_MAX)) {
        ਜੇ(!tsize) {
          failf(ਅੰਕੜੇ, "invalid tsize -:%s:- value in OACK packet", ਮੁੱਲ);
          ਵਾਪਸ CURLE_TFTP_ILLEGAL;
        }
        infof(ਅੰਕੜੇ, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, tsize);
      }
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode tftp_option_ਜੋੜੋ(ਬਣਤਰ tftp_conn *state, size_t *csize,
                                size_t ਸੂਚਕਾਂਕ, ਅਚਲ ਅੱਖਰ *option)
{
  ਅੱਖਰ *buf = (ਅੱਖਰ *)&state->spacket.ਅੰਕੜੇ[ਸੂਚਕਾਂਕ];
  size_t oplen = ਲੜੀ_ਲੰਬਾਈ(option);
  size_t blen;
  ਜੇ((state->blksize <= ਸੂਚਕਾਂਕ) ||
     (oplen + 1) > (size_t)(state->blksize - ਸੂਚਕਾਂਕ))
    ਵਾਪਸ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - ਸੂਚਕਾਂਕ;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ਵਾਪਸ CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਗਲਾ_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_tx(ਬਣਤਰ tftp_conn *state, tftp_event_t event)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = state->ਅੰਕੜੇ;
  ssize_t sbytes;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  size_t cb; /* Bytes currently read */
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
  ਅੱਖਰ *bufptr;
  bool eos;

  ਜੇ(!ਅੰਕੜੇ->state.upload)
    ਵਾਪਸ CURLE_TFTP_ILLEGAL;

  ਚੋਣ(event) {

  ਹਾਲਤ TFTP_EVENT_ACK:
  ਹਾਲਤ TFTP_EVENT_OACK:
    ਜੇ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      ਪੂਰਨ rblock = getrpacketblock(&state->rpacket);

      ਜੇ(rblock != state->ਖੰਡ &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->ਖੰਡ == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(ਅੰਕੜੇ, "Received ACK for block %d, expecting %d",
              rblock, state->ਖੰਡ);
        state->retries++;
        /* Bail out if over the maximum */
        ਜੇ(state->retries > state->retry_max) {
          failf(ਅੰਕੜੇ, "tftp_tx: giving up waiting for block %d ack",
                state->ਖੰਡ);
          ਨਤੀਜਾ = CURLE_SEND_ERROR;
        }
        ਨਹੀਂਤਾਂ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ਬਣਤਰ sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ਜੇ(sbytes < 0) {
            failf(ਅੰਕੜੇ, "%s",
                  curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
            ਨਤੀਜਾ = CURLE_SEND_ERROR;
          }
        }

        ਵਾਪਸ ਨਤੀਜਾ;
      }
      /* This is the expected packet. Reset the counters and send the next
ਖੰਡ*/
      state->rx_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);
      state->ਖੰਡ++;
    }
    ਨਹੀਂਤਾਂ
      state->ਖੰਡ = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ਅੰਕੜੇ);
    setpacketblock(&state->spacket, state->ਖੰਡ);
    ਜੇ(state->ਖੰਡ > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ਵਾਪਸ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (ਅੱਖਰ *)state->spacket.ਅੰਕੜੇ + 4;
    ਕਰੋ {
      ਨਤੀਜਾ = Curl_client_read(ਅੰਕੜੇ, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      state->sbytes += cb;
      bufptr += cb;
    } ਜਦੋਂ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ਬਣਤਰ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ਜੇ(sbytes < 0) {
      failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(ਅੰਕੜੇ, state->sbytes);
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(ਅੰਕੜੇ, "Timeout waiting for block %d ACK. "
          "Retries = %d", ਅਗਲਾ_BLOCKNUM(state->ਖੰਡ), state->retries);
    /* Decide if we have had enough */
    ਜੇ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ਨਹੀਂਤਾਂ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ਬਣਤਰ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ਜੇ(sbytes < 0) {
        failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_ਸੈੱਟ_Upload_Counter(ਅੰਕੜੇ, k->writebytecount);
    }
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ਖੰਡ);
    (ਖਾਲੀ)sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ, 4, SEND_4TH_ARG,
                 (ਬਣਤਰ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ਰੋਕੋ;

  ਮੂਲ:
    failf(ਅੰਕੜੇ, "tftp_tx: internal error, event: %d", (ਪੂਰਨ)event);
    ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode tftp_connect_for_tx(ਬਣਤਰ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ਨਤੀਜਾ;

  infof(state->ਅੰਕੜੇ, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  ਨਤੀਜਾ = tftp_ਸੈੱਟ_timeouts(state);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਵਾਪਸ tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_rx(ਬਣਤਰ tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  ਪੂਰਨ rblock;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = state->ਅੰਕੜੇ;
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];

  ਚੋਣ(event) {

  ਹਾਲਤ TFTP_EVENT_ਅੰਕੜੇ:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ਜੇ(ਅਗਲਾ_BLOCKNUM(state->ਖੰਡ) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ(state->ਖੰਡ == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(ਅੰਕੜੇ, "Received last DATA packet block %d again.", rblock);
    }
    ਨਹੀਂਤਾਂ {
      /* totally unexpected, log it */
      infof(ਅੰਕੜੇ,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, ਅਗਲਾ_BLOCKNUM(state->ਖੰਡ));
      ਰੋਕੋ;
    }

    /* ACK this block. */
    state->ਖੰਡ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ਖੰਡ);
    sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                    4, SEND_4TH_ARG,
                    (ਬਣਤਰ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ਜੇ(sbytes < 0) {
      failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ਜੇ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ਨਹੀਂਤਾਂ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->ਖੰਡ = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ਖੰਡ);
    sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                    4, SEND_4TH_ARG,
                    (ਬਣਤਰ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ਜੇ(sbytes < 0) {
      failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(ਅੰਕੜੇ,
          "Timeout waiting for block %d ACK. Retries = %d",
          ਅਗਲਾ_BLOCKNUM(state->ਖੰਡ), state->retries);
    ਜੇ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ਨਹੀਂਤਾਂ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                      4, SEND_4TH_ARG,
                      (ਬਣਤਰ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ਜੇ(sbytes < 0) {
        failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
    }
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ਖੰਡ);
    (ਖਾਲੀ)sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                 4, SEND_4TH_ARG,
                 (ਬਣਤਰ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ਰੋਕੋ;

  ਮੂਲ:
    failf(ਅੰਕੜੇ, "%s", "tftp_rx: internal error");
    ਵਾਪਸ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode tftp_connect_for_rx(ਬਣਤਰ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ਨਤੀਜਾ;

  infof(state->ਅੰਕੜੇ, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  ਨਤੀਜਾ = tftp_ਸੈੱਟ_timeouts(state);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਵਾਪਸ tftp_rx(state, event);
}

ਸਥਿਰ CURLcode tftp_send_ਪਹਿਲਾ(ਬਣਤਰ tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ = "octet";
  ਅੱਖਰ *filename;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = state->ਅੰਕੜੇ;
  ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *remote_addr = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ਜੇ(ਅੰਕੜੇ->state.prefer_ascii)
    ਬਹੁਲਕ = "netascii";

  ਚੋਣ(event) {

  ਹਾਲਤ TFTP_EVENT_INIT:    /* Send the first packet out */
  ਹਾਲਤ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ਜੇ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ(ਅੰਕੜੇ->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ਜੇ(ਅੰਕੜੇ->state.infilesize != -1)
        Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    }
    ਨਹੀਂਤਾਂ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ਜੇ(!state->ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1]) {
      failf(ਅੰਕੜੇ, "Missing filename");
      ਵਾਪਸ CURLE_TFTP_ILLEGAL;
    }
    ਨਤੀਜਾ = Curl_urldecode(&state->ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1], 0,
                            &filename, ਨਿਰਾ, REJECT_ZERO);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(filename) + ਲੜੀ_ਲੰਬਾਈ(ਬਹੁਲਕ) + 4 > state->blksize) {
      failf(ਅੰਕੜੇ, "TFTP filename too long");
      curlx_free(filename);
      ਵਾਪਸ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((ਅੱਖਰ *)state->spacket.ਅੰਕੜੇ + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', ਬਹੁਲਕ, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.tftp_no_options) {
      ਅੱਖਰ buf[64];
      /* add tsize option */
      curl_msnprintf(buf, ਆਕਾਰ(buf), "%" FMT_OFF_T,
                     ਅੰਕੜੇ->state.upload && (ਅੰਕੜੇ->state.infilesize != -1) ?
                     ਅੰਕੜੇ->state.infilesize : 0);

      ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ਜੇ(ਨਤੀਜਾ == CURLE_OK)
        ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, ਆਕਾਰ(buf), "%u", state->requested_blksize);
      ਜੇ(ਨਤੀਜਾ == CURLE_OK)
        ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ਜੇ(ਨਤੀਜਾ == CURLE_OK)
        ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, ਆਕਾਰ(buf), "%d", state->retry_ਸਮਾਂ);
      ਜੇ(ਨਤੀਜਾ == CURLE_OK)
        ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ਜੇ(ਨਤੀਜਾ == CURLE_OK)
        ਨਤੀਜਾ = tftp_option_ਜੋੜੋ(state, &sbytes, sbytes, buf);

      ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
        failf(ਅੰਕੜੇ, "TFTP buffer too small for options");
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SENDTO_ARG5(x) (x)
#ਜੇ_ਅੰਤ
    remote_addr = Curl_conn_get_remote_addr(ਅੰਕੜੇ, FIRSTSOCKET);
    ਜੇ(!remote_addr)
      ਵਾਪਸ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (ਖਾਲੀ *)state->spacket.ਅੰਕੜੇ,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ਜੇ(senddata != (ssize_t)sbytes) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "%s", curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_OACK:
    ਜੇ(ਅੰਕੜੇ->state.upload) {
      ਨਤੀਜਾ = tftp_connect_for_tx(state, event);
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = tftp_connect_for_rx(state, event);
    }
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_ACK: /* Connected for transmit */
    ਜੇ(!ਅੰਕੜੇ->state.upload)
      ਵਾਪਸ CURLE_TFTP_ILLEGAL;
    ਨਤੀਜਾ = tftp_connect_for_tx(state, event);
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_ਅੰਕੜੇ: /* Connected for receive */
    ਜੇ(ਅੰਕੜੇ->state.upload)
      ਵਾਪਸ CURLE_TFTP_ILLEGAL;
    ਨਤੀਜਾ = tftp_connect_for_rx(state, event);
    ਰੋਕੋ;

  ਹਾਲਤ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ਰੋਕੋ;

  ਮੂਲ:
    failf(state->ਅੰਕੜੇ, "tftp_send_first: internal error");
    ਵਾਪਸ CURLE_TFTP_ILLEGAL;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_translate_ਕੋਡ(tftp_error_t error)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(error != TFTP_ERR_NONE) {
    ਚੋਣ(error) {
    ਹਾਲਤ TFTP_ERR_NOTFOUND:
      ਨਤੀਜਾ = CURLE_TFTP_NOTFOUND;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_PERM:
      ਨਤੀਜਾ = CURLE_TFTP_PERM;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_DISKFULL:
      ਨਤੀਜਾ = CURLE_REMOTE_DISK_ਭਰਿਆ;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_UNDEF:
    ਹਾਲਤ TFTP_ERR_ILLEGAL:
      ਨਤੀਜਾ = CURLE_TFTP_ILLEGAL;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_UNKNOWNID:
      ਨਤੀਜਾ = CURLE_TFTP_UNKNOWNID;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_EXISTS:
      ਨਤੀਜਾ = CURLE_REMOTE_FILE_EXISTS;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_NOSUCHUSER:
      ਨਤੀਜਾ = CURLE_TFTP_NOSUCHUSER;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_TIMEOUT:
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_ERR_NORESPONSE:
      ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
      ਰੋਕੋ;
    ਮੂਲ:
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
      ਰੋਕੋ;
    }
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = CURLE_OK;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_state_machine(ਬਣਤਰ tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = state->ਅੰਕੜੇ;

  ਚੋਣ(state->state) {
  ਹਾਲਤ TFTP_STATE_ਸ਼ੁਰੂ:
    DEBUGF(infof(ਅੰਕੜੇ, "TFTP_STATE_START"));
    ਨਤੀਜਾ = tftp_send_ਪਹਿਲਾ(state, event);
    ਰੋਕੋ;
  ਹਾਲਤ TFTP_STATE_RX:
    DEBUGF(infof(ਅੰਕੜੇ, "TFTP_STATE_RX"));
    ਨਤੀਜਾ = tftp_rx(state, event);
    ਰੋਕੋ;
  ਹਾਲਤ TFTP_STATE_TX:
    DEBUGF(infof(ਅੰਕੜੇ, "TFTP_STATE_TX"));
    ਨਤੀਜਾ = tftp_tx(state, event);
    ਰੋਕੋ;
  ਹਾਲਤ TFTP_STATE_FIN:
    infof(ਅੰਕੜੇ, "%s", "TFTP finished");
    ਰੋਕੋ;
  ਮੂਲ:
    DEBUGF(infof(ਅੰਕੜੇ, "STATE: %d", (ਪੂਰਨ)state->state));
    failf(ਅੰਕੜੇ, "%s", "Internal state machine error");
    ਨਤੀਜਾ = CURLE_TFTP_ILLEGAL;
    ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ tftp_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ tftp_conn *state = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_safefree(state->rpacket.ਅੰਕੜੇ);
  curlx_safefree(state->spacket.ਅੰਕੜੇ);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ tftp_conn *state;
  ਪੂਰਨ blksize;
  ਪੂਰਨ need_blksize;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *remote_addr = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, ਆਕਾਰ(*state));
  ਜੇ(!state ||
     Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* alloc pkt buffers based on specified blksize */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.tftp_blksize)
    /* range checked when set */
    blksize = (ਪੂਰਨ)ਅੰਕੜੇ->ਸੈੱਟ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ਜੇ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ਜੇ(!state->rpacket.ਅੰਕੜੇ) {
    state->rpacket.ਅੰਕੜੇ = curlx_calloc(1, need_blksize + 2 + 2);

    ਜੇ(!state->rpacket.ਅੰਕੜੇ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(!state->spacket.ਅੰਕੜੇ) {
    state->spacket.ਅੰਕੜੇ = curlx_calloc(1, need_blksize + 2 + 2);

    ਜੇ(!state->spacket.ਅੰਕੜੇ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->ਅੰਕੜੇ = ਅੰਕੜੇ;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_ਸ਼ੁਰੂ;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(ਅੰਕੜੇ, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  ਜੇ(!remote_addr)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ((ਬਣਤਰ sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  ਨਤੀਜਾ = tftp_ਸੈੱਟ_timeouts(state);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!conn->ਬਿੱਟ.bound) {
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
    ਪੂਰਨ rc = bind(state->sockfd, (ਬਣਤਰ sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    ਜੇ(rc) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
    conn->ਬਿੱਟ.bound = TRUE;
  }

  *ਪੂਰਾ = TRUE;

  ਵਾਪਸ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                          bool premature)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (ਖਾਲੀ)ਸਥਿਤੀ;
  (ਖਾਲੀ)premature;

  ਜੇ(Curl_pgrs_ਪੂਰਾ(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ)))
    ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ਜੇ(state)
    ਨਤੀਜਾ = tftp_translate_ਕੋਡ(state->error);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode tftp_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_receive_packet(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ tftp_conn *state)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = ਆਕਾਰ(remote_addr);

  /* Receive the packet */
  state->rbytes = (ਪੂਰਨ)recvfrom(state->sockfd,
                                (ਖਾਲੀ *)state->rpacket.ਅੰਕੜੇ,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ਬਣਤਰ sockaddr *)&remote_addr,
                                &fromlen);
  ਜੇ((state->rbytes >= 0) && fromlen) {
    ਜੇ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ਜੇ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(ਅੰਕੜੇ, "Data received from another address");
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ਜੇ(state->rbytes < 4) {
    failf(ਅੰਕੜੇ, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ਨਹੀਂਤਾਂ {
    /* The event is given by the TFTP packet time */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ਚੋਣ(state->event) {
    ਹਾਲਤ TFTP_EVENT_ਅੰਕੜੇ:
      ਜੇ(ਅੰਕੜੇ->state.upload)
        ਵਾਪਸ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ਜੇ(state->rbytes > 4 &&
         (ਅਗਲਾ_BLOCKNUM(state->ਖੰਡ) == getrpacketblock(&state->rpacket))) {
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                   (ਅਚਲ ਅੱਖਰ *)state->rpacket.ਅੰਕੜੇ + 4,
                                   state->rbytes - 4);
        ਜੇ(ਨਤੀਜਾ) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_EVENT_ERROR: {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ error = getrpacketblock(&state->rpacket);
      ਅਚਲ ਅੱਖਰ *str = (ਅਚਲ ਅੱਖਰ *)state->rpacket.ਅੰਕੜੇ + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ਜੇ(tftp_strnlen(str, strn) < strn)
        infof(ਅੰਕੜੇ, "TFTP error: %s", str);
      ਰੋਕੋ;
    }
    ਹਾਲਤ TFTP_EVENT_ACK:
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_EVENT_OACK:
      ਨਤੀਜਾ = tftp_parse_option_ack(state,
                                     (ਅਚਲ ਅੱਖਰ *)state->rpacket.ਅੰਕੜੇ + 2,
                                     state->rbytes-2);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਰੋਕੋ;
    ਹਾਲਤ TFTP_EVENT_RRQ:
    ਹਾਲਤ TFTP_EVENT_WRQ:
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "%s", "Internal error: Unexpected packet");
      ਰੋਕੋ;
    }

    /* Update the progress meter */
    ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ਸਥਿਰ timediff_t tftp_state_timeout(ਬਣਤਰ tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  ਜੇ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->ਅੰਕੜੇ);
  ਜੇ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ਵਾਪਸ timeout_ms;
  }
  current = ਸਮਾਂ(ਨਿਰਾ);
  ਜੇ(current > state->rx_ਸਮਾਂ + state->retry_ਸਮਾਂ) {
    ਜੇ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ); /* update even though we received nothing */
  }

  ਵਾਪਸ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  tftp_event_t event;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *ਪੂਰਾ = FALSE;
  ਜੇ(!state)
    ਵਾਪਸ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ਜੇ(timeout_ms < 0) {
    failf(ਅੰਕੜੇ, "TFTP response timeout");
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }
  ਜੇ(event != TFTP_EVENT_NONE) {
    ਨਤੀਜਾ = tftp_state_machine(state, event);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    *ਪੂਰਾ = (state->state == TFTP_STATE_FIN);
    ਜੇ(*ਪੂਰਾ)
      /* Tell curl we are done */
      Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    /* no timeouts to handle, check our socket */
    ਪੂਰਨ rc = SOCKET_READABLE(state->sockfd, 0);

    ਜੇ(rc == -1) {
      /* bail out */
      ਪੂਰਨ sockerr = SOCKERRNO;
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "%s", curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      state->event = TFTP_EVENT_ERROR;
    }
    ਨਹੀਂਤਾਂ ਜੇ(rc) {
      ਨਤੀਜਾ = tftp_receive_packet(ਅੰਕੜੇ, state);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਨਤੀਜਾ = tftp_state_machine(state, state->event);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      *ਪੂਰਾ = (state->state == TFTP_STATE_FIN);
      ਜੇ(*ਪੂਰਾ)
        /* Tell curl we are done */
        Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
    }
    /* if rc == 0, then select() timed out */
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਨਤੀਜਾ = tftp_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(*dophase_ਪੂਰਾ) {
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਨਤੀਜਾ) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ਸਥਿਰ CURLcode tftp_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_ਪੂਰਾ = FALSE;
  ਜੇ(!state)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = tftp_state_machine(state, TFTP_EVENT_INIT);

  ਜੇ((state->state == TFTP_STATE_FIN) || ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = tftp_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(*dophase_ਪੂਰਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));

  ਵਾਪਸ ਨਤੀਜਾ;
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

ਸਥਿਰ CURLcode tftp_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode ਨਤੀਜਾ;

  *ਪੂਰਾ = FALSE;

  ਜੇ(!state) {
    ਨਤੀਜਾ = tftp_connect(ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ਜੇ(!state)
      ਵਾਪਸ CURLE_TFTP_ILLEGAL;
  }

  ਨਤੀਜਾ = tftp_perform(ਅੰਕੜੇ, ਪੂਰਾ);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ਜੇ(!ਨਤੀਜਾ)
    /* If we have encountered an internal tftp error, translate it. */
    ਨਤੀਜਾ = tftp_translate_ਕੋਡ(state->error);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode tftp_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਅੱਖਰ *ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  size_t len = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ਜੇ((len >= 14) && !memcmp(&ਰਸਤਾ[len - 14], ";mode=netascii", 14)) {
    ਅੰਕੜੇ->state.prefer_ascii = TRUE;
    ਰਸਤਾ[len - 14] = 0; /* cut it there */
  }
  ਨਹੀਂਤਾਂ ਜੇ((len >= 11) && !memcmp(&ਰਸਤਾ[len - 11], ";mode=octet", 11)) {
    ਅੰਕੜੇ->state.prefer_ascii = FALSE;
    ਰਸਤਾ[len - 11] = 0; /* cut it there */
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_tftp = {
  tftp_ਤਿਆਰੀ_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_ਪੂਰਾ,                            /* done */
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

#ਜੇ_ਅੰਤ
