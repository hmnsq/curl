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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿಸು TFTP_BLKSIZE_DEFAULT 512
#ವ್ಯಾಖ್ಯಾನಿಸು TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#ವ್ಯಾಖ್ಯಾನಿಸು TFTP_OPTION_TSIZE    "tsize"
#ವ್ಯಾಖ್ಯಾನಿಸು TFTP_OPTION_INTERVAL "timeout"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  TFTP_ಬಹುಲಕ_NETASCII = 0,
  TFTP_ಬಹುಲಕ_OCTET
} tftp_ಬಹುಲಕ_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  TFTP_STATE_ಆರಂಭ = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_ದತ್ತಾಂಶ = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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

ರಚನೆ tftp_packet {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ;
};

/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ರಚನೆ tftp_conn {
  ರಚನೆ Curl_sockaddr_storage local_addr;
  ರಚನೆ Curl_sockaddr_storage remote_addr;
  ರಚನೆ tftp_packet rpacket;
  ರಚನೆ tftp_packet spacket;
  tftp_state_t    state;
  tftp_ಬಹುಲಕ_t     ಬಹುಲಕ;
  tftp_error_t    error;
  tftp_event_t    event;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  curl_socket_t   sockfd;
  ಪೂರ್ಣಾಂಕ             retries;
  ಪೂರ್ಣಾಂಕ             retry_ಸಮಯ;
  ಪೂರ್ಣಾಂಕ             retry_max;
  time_t          rx_ಸಮಯ;
  curl_socklen_t  remote_addrlen;
  ಪೂರ್ಣಾಂಕ             rbytes;
  size_t          sbytes;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    blksize;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    requested_blksize;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ  ಖಂಡ;
  ಬಿಟ್(remote_pinned);
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
ಸ್ಥಿರ CURLcode tftp_ಗಣ_timeouts(ರಚನೆ tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->ದತ್ತಾಂಶ);

  ಆದರೆ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->ದತ್ತಾಂಶ, "Connection time-out");
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  ಆದರೆ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  ಇಲ್ಲದಿದ್ದರೆ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (ಪೂರ್ಣಾಂಕ)timeout / 5;

  /* Bound the total number */
  ಆದರೆ(state->retry_max < 3)
    state->retry_max = 3;

  ಆದರೆ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_ಸಮಯ = (ಪೂರ್ಣಾಂಕ)(timeout / state->retry_max);
  ಆದರೆ(state->retry_ಸಮಯ < 1)
    state->retry_ಸಮಯ = 1;

  infof(state->ದತ್ತಾಂಶ,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (ಪೂರ್ಣಾಂಕ)state->state, timeout_ms, state->retry_ಸಮಯ, state->retry_max);

  /* init RX time */
  state->rx_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

ಸ್ಥಿರ ಶೂನ್ಯ setpacketevent(ರಚನೆ tftp_packet *packet, ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಅಂಕೆ)
{
  packet->ದತ್ತಾಂಶ[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಅಂಕೆ >> 8);
  packet->ದತ್ತಾಂಶ[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಅಂಕೆ & 0xff);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpacketblock(ರಚನೆ tftp_packet *packet, ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಅಂಕೆ)
{
  packet->ದತ್ತಾಂಶ[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಅಂಕೆ >> 8);
  packet->ದತ್ತಾಂಶ[3] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಅಂಕೆ & 0xff);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ getrpacketevent(ಸ್ಥಿರಾಂಕ ರಚನೆ tftp_packet *packet)
{
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)((packet->ದತ್ತಾಂಶ[0] << 8) | packet->ದತ್ತಾಂಶ[1]);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ getrpacketblock(ಸ್ಥಿರಾಂಕ ರಚನೆ tftp_packet *packet)
{
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)((packet->ದತ್ತಾಂಶ[2] << 8) | packet->ದತ್ತಾಂಶ[3]);
}

ಸ್ಥಿರ size_t tftp_strnlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, size_t maxlen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = memchr(string, '\0', maxlen);
  ಹಿಂತಿರುಗಿ ಕೊನೆ ? (size_t)(ಕೊನೆ - string) : maxlen;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tftp_option_get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **option, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮೌಲ್ಯ)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  ಆದರೆ(loc >= len)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  ಆದರೆ(loc > len)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  *ಮೌಲ್ಯ = &buf[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*option) + 1];

  ಹಿಂತಿರುಗಿ &buf[loc];
}

ಸ್ಥಿರ CURLcode tftp_parse_option_ack(ರಚನೆ tftp_conn *state,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, ಪೂರ್ಣಾಂಕ len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tmp = ptr;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = state->ದತ್ತಾಂಶ;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ಆಗ(tmp < ptr + len) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option, *ಮೌಲ್ಯ;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &ಮೌಲ್ಯ);
    ಆದರೆ(!tmp) {
      failf(ದತ್ತಾಂಶ, "Malformed ACK packet, rejecting");
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
    }
    olen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(option);

    infof(ದತ್ತಾಂಶ, "got option=(%s) value=(%s)", option, ಮೌಲ್ಯ);

    ಆದರೆ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&ಮೌಲ್ಯ, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(ದತ್ತಾಂಶ, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      }
      ಆದರೆ(!blksize) {
        failf(ದತ್ತಾಂಶ, "invalid blocksize value in OACK packet");
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(blksize < TFTP_BLKSIZE_MIN) {
        failf(ದತ್ತಾಂಶ, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(ದತ್ತಾಂಶ, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)blksize;
      infof(ದತ್ತಾಂಶ, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      ಆದರೆ(!ದತ್ತಾಂಶ->state.upload &&
         !curlx_str_ಸಂಖ್ಯೆ(&ಮೌಲ್ಯ, &tsize, CURL_OFF_T_MAX)) {
        ಆದರೆ(!tsize) {
          failf(ದತ್ತಾಂಶ, "invalid tsize -:%s:- value in OACK packet", ಮೌಲ್ಯ);
          ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
        }
        infof(ದತ್ತಾಂಶ, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, tsize);
      }
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tftp_option_ಕೂಡಿಸು(ರಚನೆ tftp_conn *state, size_t *csize,
                                size_t ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option)
{
  ಅಕ್ಷರ *buf = (ಅಕ್ಷರ *)&state->spacket.ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ];
  size_t oplen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(option);
  size_t blen;
  ಆದರೆ((state->blksize <= ಸೂಚ್ಯಂಕ) ||
     (oplen + 1) > (size_t)(state->blksize - ಸೂಚ್ಯಂಕ))
    ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - ಸೂಚ್ಯಂಕ;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಂದಿನ_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_tx(ರಚನೆ tftp_conn *state, tftp_event_t event)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = state->ದತ್ತಾಂಶ;
  ssize_t sbytes;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  size_t cb; /* Bytes currently read */
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
  ಅಕ್ಷರ *bufptr;
  bool eos;

  ಆದರೆ(!ದತ್ತಾಂಶ->state.upload)
    ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;

  ಆಯ್ಕೆ(event) {

  ಸ್ಥಿತಿ TFTP_EVENT_ACK:
  ಸ್ಥಿತಿ TFTP_EVENT_OACK:
    ಆದರೆ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      ಪೂರ್ಣಾಂಕ rblock = getrpacketblock(&state->rpacket);

      ಆದರೆ(rblock != state->ಖಂಡ &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->ಖಂಡ == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(ದತ್ತಾಂಶ, "Received ACK for block %d, expecting %d",
              rblock, state->ಖಂಡ);
        state->retries++;
        /* Bail out if over the maximum */
        ಆದರೆ(state->retries > state->retry_max) {
          failf(ದತ್ತಾಂಶ, "tftp_tx: giving up waiting for block %d ack",
                state->ಖಂಡ);
          ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ರಚನೆ sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          ಆದರೆ(sbytes < 0) {
            failf(ದತ್ತಾಂಶ, "%s",
                  curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
            ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
          }
        }

        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      /* This is the expected packet. Reset the counters and send the next
ಖಂಡ*/
      state->rx_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);
      state->ಖಂಡ++;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      state->ಖಂಡ = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ದತ್ತಾಂಶ);
    setpacketblock(&state->spacket, state->ಖಂಡ);
    ಆದರೆ(state->ಖಂಡ > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (ಅಕ್ಷರ *)state->spacket.ದತ್ತಾಂಶ + 4;
    ಮಾಡು {
      ಫಲಿತಾಂಶ = Curl_client_read(ದತ್ತಾಂಶ, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      state->sbytes += cb;
      bufptr += cb;
    } ಆಗ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ರಚನೆ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    ಆದರೆ(sbytes < 0) {
      failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(ದತ್ತಾಂಶ, state->sbytes);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(ದತ್ತಾಂಶ, "Timeout waiting for block %d ACK. "
          "Retries = %d", ಮುಂದಿನ_BLOCKNUM(state->ಖಂಡ), state->retries);
    /* Decide if we have had enough */
    ಆದರೆ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ರಚನೆ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      ಆದರೆ(sbytes < 0) {
        failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_ಗಣ_Upload_Counter(ದತ್ತಾಂಶ, k->writebytecount);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ಖಂಡ);
    (ಶೂನ್ಯ)sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ, 4, SEND_4TH_ARG,
                 (ರಚನೆ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "tftp_tx: internal error, event: %d", (ಪೂರ್ಣಾಂಕ)event);
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode tftp_connect_for_tx(ರಚನೆ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ಫಲಿತಾಂಶ;

  infof(state->ದತ್ತಾಂಶ, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  ಫಲಿತಾಂಶ = tftp_ಗಣ_timeouts(state);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_rx(ರಚನೆ tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  ಪೂರ್ಣಾಂಕ rblock;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = state->ದತ್ತಾಂಶ;
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];

  ಆಯ್ಕೆ(event) {

  ಸ್ಥಿತಿ TFTP_EVENT_ದತ್ತಾಂಶ:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    ಆದರೆ(ಮುಂದಿನ_BLOCKNUM(state->ಖಂಡ) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state->ಖಂಡ == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(ದತ್ತಾಂಶ, "Received last DATA packet block %d again.", rblock);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* totally unexpected, log it */
      infof(ದತ್ತಾಂಶ,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, ಮುಂದಿನ_BLOCKNUM(state->ಖಂಡ));
      ನಿಲ್ಲಿಸು;
    }

    /* ACK this block. */
    state->ಖಂಡ = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ಖಂಡ);
    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                    4, SEND_4TH_ARG,
                    (ರಚನೆ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ಆದರೆ(sbytes < 0) {
      failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    ಆದರೆ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->ಖಂಡ = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->ಖಂಡ);
    sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                    4, SEND_4TH_ARG,
                    (ರಚನೆ sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    ಆದರೆ(sbytes < 0) {
      failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(ದತ್ತಾಂಶ,
          "Timeout waiting for block %d ACK. Retries = %d",
          ಮುಂದಿನ_BLOCKNUM(state->ಖಂಡ), state->retries);
    ಆದರೆ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                      4, SEND_4TH_ARG,
                      (ರಚನೆ sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      ಆದರೆ(sbytes < 0) {
        failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->ಖಂಡ);
    (ಶೂನ್ಯ)sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                 4, SEND_4TH_ARG,
                 (ರಚನೆ sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "%s", "tftp_rx: internal error");
    ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tftp_connect_for_rx(ರಚನೆ tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode ಫಲಿತಾಂಶ;

  infof(state->ದತ್ತಾಂಶ, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  ಫಲಿತಾಂಶ = tftp_ಗಣ_timeouts(state);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ tftp_rx(state, event);
}

ಸ್ಥಿರ CURLcode tftp_send_ಮೊದಲ(ರಚನೆ tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ = "octet";
  ಅಕ್ಷರ *filename;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = state->ದತ್ತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *remote_addr = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  ಆದರೆ(ದತ್ತಾಂಶ->state.prefer_ascii)
    ಬಹುಲಕ = "netascii";

  ಆಯ್ಕೆ(event) {

  ಸ್ಥಿತಿ TFTP_EVENT_INIT:    /* Send the first packet out */
  ಸ್ಥಿತಿ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    ಆದರೆ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize != -1)
        Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    ಆದರೆ(!state->ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1]) {
      failf(ದತ್ತಾಂಶ, "Missing filename");
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
    }
    ಫಲಿತಾಂಶ = Curl_urldecode(&state->ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1], 0,
                            &filename, ಖಾಲಿ, REJECT_ZERO);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(filename) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಹುಲಕ) + 4 > state->blksize) {
      failf(ದತ್ತಾಂಶ, "TFTP filename too long");
      curlx_free(filename);
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((ಅಕ್ಷರ *)state->spacket.ದತ್ತಾಂಶ + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', ಬಹುಲಕ, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.tftp_no_options) {
      ಅಕ್ಷರ buf[64];
      /* add tsize option */
      curl_msnprintf(buf, ಗಾತ್ರ(buf), "%" FMT_OFF_T,
                     ದತ್ತಾಂಶ->state.upload && (ದತ್ತಾಂಶ->state.infilesize != -1) ?
                     ದತ್ತಾಂಶ->state.infilesize : 0);

      ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK)
        ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, ಗಾತ್ರ(buf), "%u", state->requested_blksize);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK)
        ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK)
        ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, ಗಾತ್ರ(buf), "%d", state->retry_ಸಮಯ);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK)
        ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK)
        ಫಲಿತಾಂಶ = tftp_option_ಕೂಡಿಸು(state, &sbytes, sbytes, buf);

      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
        failf(ದತ್ತಾಂಶ, "TFTP buffer too small for options");
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SENDTO_ARG5(x) (x)
#ಆದರೆ_ಅಂತ್ಯ
    remote_addr = Curl_conn_get_remote_addr(ದತ್ತಾಂಶ, FIRSTSOCKET);
    ಆದರೆ(!remote_addr)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (ಶೂನ್ಯ *)state->spacket.ದತ್ತಾಂಶ,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    ಆದರೆ(senddata != (ssize_t)sbytes) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_OACK:
    ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
      ಫಲಿತಾಂಶ = tftp_connect_for_tx(state, event);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = tftp_connect_for_rx(state, event);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_ACK: /* Connected for transmit */
    ಆದರೆ(!ದತ್ತಾಂಶ->state.upload)
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
    ಫಲಿತಾಂಶ = tftp_connect_for_tx(state, event);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_ದತ್ತಾಂಶ: /* Connected for receive */
    ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
    ಫಲಿತಾಂಶ = tftp_connect_for_rx(state, event);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(state->ದತ್ತಾಂಶ, "tftp_send_first: internal error");
    ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_translate_ಸಂಕೇತ(tftp_error_t error)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(error != TFTP_ERR_NONE) {
    ಆಯ್ಕೆ(error) {
    ಸ್ಥಿತಿ TFTP_ERR_NOTFOUND:
      ಫಲಿತಾಂಶ = CURLE_TFTP_NOTFOUND;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_PERM:
      ಫಲಿತಾಂಶ = CURLE_TFTP_PERM;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_DISKFULL:
      ಫಲಿತಾಂಶ = CURLE_REMOTE_DISK_ತುಂಬಿದ;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_UNDEF:
    ಸ್ಥಿತಿ TFTP_ERR_ILLEGAL:
      ಫಲಿತಾಂಶ = CURLE_TFTP_ILLEGAL;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_UNKNOWNID:
      ಫಲಿತಾಂಶ = CURLE_TFTP_UNKNOWNID;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_EXISTS:
      ಫಲಿತಾಂಶ = CURLE_REMOTE_FILE_EXISTS;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_NOSUCHUSER:
      ಫಲಿತಾಂಶ = CURLE_TFTP_NOSUCHUSER;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_TIMEOUT:
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_ERR_NORESPONSE:
      ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = CURLE_OK;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_state_machine(ರಚನೆ tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = state->ದತ್ತಾಂಶ;

  ಆಯ್ಕೆ(state->state) {
  ಸ್ಥಿತಿ TFTP_STATE_ಆರಂಭ:
    DEBUGF(infof(ದತ್ತಾಂಶ, "TFTP_STATE_START"));
    ಫಲಿತಾಂಶ = tftp_send_ಮೊದಲ(state, event);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TFTP_STATE_RX:
    DEBUGF(infof(ದತ್ತಾಂಶ, "TFTP_STATE_RX"));
    ಫಲಿತಾಂಶ = tftp_rx(state, event);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TFTP_STATE_TX:
    DEBUGF(infof(ದತ್ತಾಂಶ, "TFTP_STATE_TX"));
    ಫಲಿತಾಂಶ = tftp_tx(state, event);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TFTP_STATE_FIN:
    infof(ದತ್ತಾಂಶ, "%s", "TFTP finished");
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGF(infof(ದತ್ತಾಂಶ, "STATE: %d", (ಪೂರ್ಣಾಂಕ)state->state));
    failf(ದತ್ತಾಂಶ, "%s", "Internal state machine error");
    ಫಲಿತಾಂಶ = CURLE_TFTP_ILLEGAL;
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ tftp_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ tftp_conn *state = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_safefree(state->rpacket.ದತ್ತಾಂಶ);
  curlx_safefree(state->spacket.ದತ್ತಾಂಶ);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ tftp_conn *state;
  ಪೂರ್ಣಾಂಕ blksize;
  ಪೂರ್ಣಾಂಕ need_blksize;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *remote_addr = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, ಗಾತ್ರ(*state));
  ಆದರೆ(!state ||
     Curl_conn_meta_ಗಣ(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* alloc pkt buffers based on specified blksize */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.tftp_blksize)
    /* range checked when set */
    blksize = (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->ಗಣ.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  ಆದರೆ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  ಆದರೆ(!state->rpacket.ದತ್ತಾಂಶ) {
    state->rpacket.ದತ್ತಾಂಶ = curlx_calloc(1, need_blksize + 2 + 2);

    ಆದರೆ(!state->rpacket.ದತ್ತಾಂಶ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(!state->spacket.ದತ್ತಾಂಶ) {
    state->spacket.ದತ್ತಾಂಶ = curlx_calloc(1, need_blksize + 2 + 2);

    ಆದರೆ(!state->spacket.ದತ್ತಾಂಶ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_ಆರಂಭ;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(ದತ್ತಾಂಶ, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  ಆದರೆ(!remote_addr)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ((ರಚನೆ sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  ಫಲಿತಾಂಶ = tftp_ಗಣ_timeouts(state);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.bound) {
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
    ಪೂರ್ಣಾಂಕ rc = bind(state->sockfd, (ರಚನೆ sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    ಆದರೆ(rc) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
    conn->ಬಿಟ್ಟುಗಳು.bound = TRUE;
  }

  *ಮುಗಿದ = TRUE;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                          bool premature)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)premature;

  ಆದರೆ(Curl_pgrs_ಮುಗಿದ(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ)))
    ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  ಆದರೆ(state)
    ಫಲಿತಾಂಶ = tftp_translate_ಸಂಕೇತ(state->error);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode tftp_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_receive_packet(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ tftp_conn *state)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = ಗಾತ್ರ(remote_addr);

  /* Receive the packet */
  state->rbytes = (ಪೂರ್ಣಾಂಕ)recvfrom(state->sockfd,
                                (ಶೂನ್ಯ *)state->rpacket.ದತ್ತಾಂಶ,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ರಚನೆ sockaddr *)&remote_addr,
                                &fromlen);
  ಆದರೆ((state->rbytes >= 0) && fromlen) {
    ಆದರೆ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      ಆದರೆ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(ದತ್ತಾಂಶ, "Data received from another address");
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      ಸ್ಮರಣೆ_ನಕಲು(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  ಆದರೆ(state->rbytes < 4) {
    failf(ದತ್ತಾಂಶ, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* The event is given by the TFTP packet time */
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    ಆಯ್ಕೆ(state->event) {
    ಸ್ಥಿತಿ TFTP_EVENT_ದತ್ತಾಂಶ:
      ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
        ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      ಆದರೆ(state->rbytes > 4 &&
         (ಮುಂದಿನ_BLOCKNUM(state->ಖಂಡ) == getrpacketblock(&state->rpacket))) {
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                   (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)state->rpacket.ದತ್ತಾಂಶ + 4,
                                   state->rbytes - 4);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_EVENT_ERROR: {
      ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ error = getrpacketblock(&state->rpacket);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)state->rpacket.ದತ್ತಾಂಶ + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      ಆದರೆ(tftp_strnlen(str, strn) < strn)
        infof(ದತ್ತಾಂಶ, "TFTP error: %s", str);
      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ TFTP_EVENT_ACK:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_EVENT_OACK:
      ಫಲಿತಾಂಶ = tftp_parse_option_ack(state,
                                     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)state->rpacket.ದತ್ತಾಂಶ + 2,
                                     state->rbytes-2);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ TFTP_EVENT_RRQ:
    ಸ್ಥಿತಿ TFTP_EVENT_WRQ:
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "%s", "Internal error: Unexpected packet");
      ನಿಲ್ಲಿಸು;
    }

    /* Update the progress meter */
    ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
ಸ್ಥಿರ timediff_t tftp_state_timeout(ರಚನೆ tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  ಆದರೆ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->ದತ್ತಾಂಶ);
  ಆದರೆ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ಹಿಂತಿರುಗಿ timeout_ms;
  }
  current = ಸಮಯ(ಖಾಲಿ);
  ಆದರೆ(current > state->rx_ಸಮಯ + state->retry_ಸಮಯ) {
    ಆದರೆ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_ಸಮಯ = ಸಮಯ(ಖಾಲಿ); /* update even though we received nothing */
  }

  ಹಿಂತಿರುಗಿ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  tftp_event_t event;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!state)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  ಆದರೆ(timeout_ms < 0) {
    failf(ದತ್ತಾಂಶ, "TFTP response timeout");
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }
  ಆದರೆ(event != TFTP_EVENT_NONE) {
    ಫಲಿತಾಂಶ = tftp_state_machine(state, event);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    *ಮುಗಿದ = (state->state == TFTP_STATE_FIN);
    ಆದರೆ(*ಮುಗಿದ)
      /* Tell curl we are done */
      Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* no timeouts to handle, check our socket */
    ಪೂರ್ಣಾಂಕ rc = SOCKET_READABLE(state->sockfd, 0);

    ಆದರೆ(rc == -1) {
      /* bail out */
      ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "%s", curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      state->event = TFTP_EVENT_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc) {
      ಫಲಿತಾಂಶ = tftp_receive_packet(ದತ್ತಾಂಶ, state);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಫಲಿತಾಂಶ = tftp_state_machine(state, state->event);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      *ಮುಗಿದ = (state->state == TFTP_STATE_FIN);
      ಆದರೆ(*ಮುಗಿದ)
        /* Tell curl we are done */
        Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
    }
    /* if rc == 0, then select() timed out */
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = tftp_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
ಸ್ಥಿರ CURLcode tftp_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_ಮುಗಿದ = FALSE;
  ಆದರೆ(!state)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = tftp_state_machine(state, TFTP_EVENT_INIT);

  ಆದರೆ((state->state == TFTP_STATE_FIN) || ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = tftp_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(*dophase_ಮುಗಿದ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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

ಸ್ಥಿರ CURLcode tftp_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode ಫಲಿತಾಂಶ;

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!state) {
    ಫಲಿತಾಂಶ = tftp_connect(ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    ಆದರೆ(!state)
      ಹಿಂತಿರುಗಿ CURLE_TFTP_ILLEGAL;
  }

  ಫಲಿತಾಂಶ = tftp_perform(ದತ್ತಾಂಶ, ಮುಗಿದ);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    /* If we have encountered an internal tftp error, translate it. */
    ಫಲಿತಾಂಶ = tftp_translate_ಸಂಕೇತ(state->error);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode tftp_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ಅಕ್ಷರ *ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  ಆದರೆ((len >= 14) && !memcmp(&ಮಾರ್ಗ[len - 14], ";mode=netascii", 14)) {
    ದತ್ತಾಂಶ->state.prefer_ascii = TRUE;
    ಮಾರ್ಗ[len - 14] = 0; /* cut it there */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((len >= 11) && !memcmp(&ಮಾರ್ಗ[len - 11], ";mode=octet", 11)) {
    ದತ್ತಾಂಶ->state.prefer_ascii = FALSE;
    ಮಾರ್ಗ[len - 11] = 0; /* cut it there */
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_tftp = {
  tftp_ಸಿದ್ಧತೆ_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_ಮುಗಿದ,                            /* done */
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

#ಆದರೆ_ಅಂತ್ಯ
