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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "sockaddr.h" /* required for Curl_sockaddr_storage */
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

തരനിർവചനം എണ്ണൽ {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

തരനിർവചനം എണ്ണൽ {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

തരനിർവചനം എണ്ണൽ {
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

തരനിർവചനം എണ്ണൽ {
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

ഘടന tftp_packet {
  unsigned അക്ഷരം *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

ഘടന tftp_conn {
  ഘടന Curl_sockaddr_storage local_addr;
  ഘടന Curl_sockaddr_storage remote_addr;
  ഘടന tftp_packet rpacket;
  ഘടന tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  ഘടന Curl_easy *data;
  curl_socket_t   sockfd;
  പൂർണ്ണസംഖ്യ             retries;
  പൂർണ്ണസംഖ്യ             retry_time;
  പൂർണ്ണസംഖ്യ             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  പൂർണ്ണസംഖ്യ             rbytes;
  size_t          sbytes;
  unsigned പൂർണ്ണസംഖ്യ    blksize;
  unsigned പൂർണ്ണസംഖ്യ    requested_blksize;
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
സ്ഥിതം CURLcode tftp_set_timeouts(ഘടന tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  എങ്കിൽ(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  എങ്കിൽ((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  അല്ലെങ്കിൽ
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (പൂർണ്ണസംഖ്യ)timeout / 5;

  /* Bound the total number */
  എങ്കിൽ(state->retry_max < 3)
    state->retry_max = 3;

  എങ്കിൽ(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (പൂർണ്ണസംഖ്യ)(timeout / state->retry_max);
  എങ്കിൽ(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (പൂർണ്ണസംഖ്യ)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(ശൂന്യ);

  തിരികെ CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

സ്ഥിതം ശൂന്യം setpacketevent(ഘടന tftp_packet *packet, unsigned short അക്കം)
{
  packet->data[0] = (unsigned അക്ഷരം)(അക്കം >> 8);
  packet->data[1] = (unsigned അക്ഷരം)(അക്കം & 0xff);
}

സ്ഥിതം ശൂന്യം setpacketblock(ഘടന tftp_packet *packet, unsigned short അക്കം)
{
  packet->data[2] = (unsigned അക്ഷരം)(അക്കം >> 8);
  packet->data[3] = (unsigned അക്ഷരം)(അക്കം & 0xff);
}

സ്ഥിതം unsigned short getrpacketevent(സ്ഥിരം ഘടന tftp_packet *packet)
{
  തിരികെ (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

സ്ഥിതം unsigned short getrpacketblock(സ്ഥിരം ഘടന tftp_packet *packet)
{
  തിരികെ (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

സ്ഥിതം size_t tftp_strnlen(സ്ഥിരം അക്ഷരം *string, size_t maxlen)
{
  സ്ഥിരം അക്ഷരം *end = memchr(string, '\0', maxlen);
  തിരികെ end ? (size_t)(end - string) : maxlen;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *tftp_option_get(സ്ഥിരം അക്ഷരം *buf, size_t len,
                                   സ്ഥിരം അക്ഷരം **option, സ്ഥിരം അക്ഷരം **മൂല്യം)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  എങ്കിൽ(loc >= len)
    തിരികെ ശൂന്യ;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  എങ്കിൽ(loc > len)
    തിരികെ ശൂന്യ;
  *മൂല്യം = &buf[strlen(*option) + 1];

  തിരികെ &buf[loc];
}

സ്ഥിതം CURLcode tftp_parse_option_ack(ഘടന tftp_conn *state,
                                      സ്ഥിരം അക്ഷരം *ptr, പൂർണ്ണസംഖ്യ len)
{
  സ്ഥിരം അക്ഷരം *tmp = ptr;
  ഘടന Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  ആകുമ്പോൾ(tmp < ptr + len) {
    സ്ഥിരം അക്ഷരം *option, *മൂല്യം;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &മൂല്യം);
    എങ്കിൽ(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      തിരികെ CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, മൂല്യം);

    എങ്കിൽ((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      എങ്കിൽ(curlx_str_സംഖ്യ(&മൂല്യം, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        തിരികെ CURLE_TFTP_ILLEGAL;
      }
      എങ്കിൽ(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        തിരികെ CURLE_TFTP_ILLEGAL;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        തിരികെ CURLE_TFTP_ILLEGAL;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        തിരികെ CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned പൂർണ്ണസംഖ്യ)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    അല്ലെങ്കിൽ എങ്കിൽ((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      എങ്കിൽ(!data->state.upload &&
         !curlx_str_സംഖ്യ(&മൂല്യം, &tsize, CURL_OFF_T_MAX)) {
        എങ്കിൽ(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", മൂല്യം);
          തിരികെ CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode tftp_option_add(ഘടന tftp_conn *state, size_t *csize,
                                size_t index, സ്ഥിരം അക്ഷരം *option)
{
  അക്ഷരം *buf = (അക്ഷരം *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  എങ്കിൽ((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    തിരികെ CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode tftp_tx(ഘടന tftp_conn *state, tftp_event_t event)
{
  ഘടന Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  ഘടന SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  അക്ഷരം buffer[STRERROR_LEN];
  അക്ഷരം *bufptr;
  bool eos;

  എങ്കിൽ(!data->state.upload)
    തിരികെ CURLE_TFTP_ILLEGAL;

  തിരഞ്ഞെടുപ്പ്(event) {

  അവസ്ഥ TFTP_EVENT_ACK:
  അവസ്ഥ TFTP_EVENT_OACK:
    എങ്കിൽ(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      പൂർണ്ണസംഖ്യ rblock = getrpacketblock(&state->rpacket);

      എങ്കിൽ(rblock != state->block &&
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
        എങ്കിൽ(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        അല്ലെങ്കിൽ {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (ഘടന sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          എങ്കിൽ(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        തിരികെ result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(ശൂന്യ);
      state->block++;
    }
    അല്ലെങ്കിൽ
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    എങ്കിൽ(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      തിരികെ CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (അക്ഷരം *)state->spacket.data + 4;
    ചെയ്യുക {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      എങ്കിൽ(result)
        തിരികെ result;
      state->sbytes += cb;
      bufptr += cb;
    } ആകുമ്പോൾ(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (ഘടന sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    എങ്കിൽ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    എങ്കിൽ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    അല്ലെങ്കിൽ {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (ഘടന sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      എങ്കിൽ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
        തിരികെ CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ശൂന്യം)sendto(state->sockfd, (ശൂന്യം *)state->spacket.data, 4, SEND_4TH_ARG,
                 (ഘടന sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    failf(data, "tftp_tx: internal error, event: %d", (പൂർണ്ണസംഖ്യ)event);
    നിർത്തുക;
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode tftp_connect_for_tx(ഘടന tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  എങ്കിൽ(result)
    തിരികെ result;
  തിരികെ tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_rx(ഘടന tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  പൂർണ്ണസംഖ്യ rblock;
  ഘടന Curl_easy *data = state->data;
  അക്ഷരം buffer[STRERROR_LEN];

  തിരഞ്ഞെടുപ്പ്(event) {

  അവസ്ഥ TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    എങ്കിൽ(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    അല്ലെങ്കിൽ {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      നിർത്തുക;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ഘടന sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    എങ്കിൽ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    എങ്കിൽ(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    അല്ലെങ്കിൽ {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(ശൂന്യ);
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (ഘടന sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    എങ്കിൽ(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(ശൂന്യ);
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    എങ്കിൽ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    അല്ലെങ്കിൽ {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (ഘടന sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      എങ്കിൽ(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
        തിരികെ CURLE_SEND_ERROR;
      }
    }
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (ശൂന്യം)sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (ഘടന sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    failf(data, "%s", "tftp_rx: internal error");
    തിരികെ CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode tftp_connect_for_rx(ഘടന tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  എങ്കിൽ(result)
    തിരികെ result;
  തിരികെ tftp_rx(state, event);
}

സ്ഥിതം CURLcode tftp_send_first(ഘടന tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  സ്ഥിരം അക്ഷരം *mode = "octet";
  അക്ഷരം *filename;
  ഘടന Curl_easy *data = state->data;
  സ്ഥിരം ഘടന Curl_sockaddr_ex *remote_addr = ശൂന്യ;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  എങ്കിൽ(data->state.prefer_ascii)
    mode = "netascii";

  തിരഞ്ഞെടുപ്പ്(event) {

  അവസ്ഥ TFTP_EVENT_INIT:    /* Send the first packet out */
  അവസ്ഥ TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    എങ്കിൽ(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      തിരികെ result;
    }

    എങ്കിൽ(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      എങ്കിൽ(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    അല്ലെങ്കിൽ {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    എങ്കിൽ(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      തിരികെ CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, ശൂന്യ, REJECT_ZERO);
    എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      തിരികെ CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((അക്ഷരം *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    എങ്കിൽ(!data->set.tftp_no_options) {
      അക്ഷരം buf[64];
      /* add tsize option */
      curl_msnprintf(buf, വലുപ്പം(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      എങ്കിൽ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, വലുപ്പം(buf), "%u", state->requested_blksize);
      എങ്കിൽ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      എങ്കിൽ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, വലുപ്പം(buf), "%d", state->retry_time);
      എങ്കിൽ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      എങ്കിൽ(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      എങ്കിൽ(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        തിരികെ CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#അല്ലെങ്കിൽ
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    എങ്കിൽ(!remote_addr)
      തിരികെ CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (ശൂന്യം *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    എങ്കിൽ(senddata != (ssize_t)sbytes) {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_SEND_ERROR;
    }
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_OACK:
    എങ്കിൽ(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    അല്ലെങ്കിൽ {
      result = tftp_connect_for_rx(state, event);
    }
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_ACK: /* Connected for transmit */
    എങ്കിൽ(!data->state.upload)
      തിരികെ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_DATA: /* Connected for receive */
    എങ്കിൽ(data->state.upload)
      തിരികെ CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    നിർത്തുക;

  അവസ്ഥ TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    failf(state->data, "tftp_send_first: internal error");
    തിരികെ CURLE_TFTP_ILLEGAL;
  }

  തിരികെ result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(error != TFTP_ERR_NONE) {
    തിരഞ്ഞെടുപ്പ്(error) {
    അവസ്ഥ TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_UNDEF:
    അവസ്ഥ TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    അവസ്ഥ TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      result = CURLE_ABORTED_BY_CALLBACK;
      നിർത്തുക;
    }
  }
  അല്ലെങ്കിൽ
    result = CURLE_OK;

  തിരികെ result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_state_machine(ഘടന tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  ഘടന Curl_easy *data = state->data;

  തിരഞ്ഞെടുപ്പ്(state->state) {
  അവസ്ഥ TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    നിർത്തുക;
  അവസ്ഥ TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    നിർത്തുക;
  അവസ്ഥ TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    നിർത്തുക;
  അവസ്ഥ TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    DEBUGF(infof(data, "STATE: %d", (പൂർണ്ണസംഖ്യ)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    നിർത്തുക;
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം tftp_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന tftp_conn *state = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
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
സ്ഥിതം CURLcode tftp_connect(ഘടന Curl_easy *data, bool *done)
{
  ഘടന tftp_conn *state;
  പൂർണ്ണസംഖ്യ blksize;
  പൂർണ്ണസംഖ്യ need_blksize;
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം ഘടന Curl_sockaddr_ex *remote_addr = ശൂന്യ;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, വലുപ്പം(*state));
  എങ്കിൽ(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  എങ്കിൽ(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (പൂർണ്ണസംഖ്യ)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  എങ്കിൽ(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  എങ്കിൽ(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    എങ്കിൽ(!state->rpacket.data)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    എങ്കിൽ(!state->spacket.data)
      തിരികെ CURLE_OUT_OF_MEMORY;
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
  എങ്കിൽ(!remote_addr)
    തിരികെ CURLE_FAILED_INIT;

  ((ഘടന sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(!conn->bits.bound) {
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
    പൂർണ്ണസംഖ്യ rc = bind(state->sockfd, (ഘടന sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    എങ്കിൽ(rc) {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  തിരികെ CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (ശൂന്യം)status;
  (ശൂന്യം)premature;

  എങ്കിൽ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    തിരികെ CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  എങ്കിൽ(state)
    result = tftp_translate_code(state->error);

  തിരികെ result;
}

സ്ഥിതം CURLcode tftp_pollset(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps)
{
  തിരികെ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_receive_packet(ഘടന Curl_easy *data,
                                    ഘടന tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  ഘടന Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = വലുപ്പം(remote_addr);

  /* Receive the packet */
  state->rbytes = (പൂർണ്ണസംഖ്യ)recvfrom(state->sockfd,
                                (ശൂന്യം *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (ഘടന sockaddr *)&remote_addr,
                                &fromlen);
  എങ്കിൽ((state->rbytes >= 0) && fromlen) {
    എങ്കിൽ(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      എങ്കിൽ((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        തിരികെ CURLE_RECV_ERROR;
      }
    }
    അല്ലെങ്കിൽ {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  എങ്കിൽ(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  അല്ലെങ്കിൽ {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    തിരഞ്ഞെടുപ്പ്(state->event) {
    അവസ്ഥ TFTP_EVENT_DATA:
      എങ്കിൽ(data->state.upload)
        തിരികെ CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      എങ്കിൽ(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (സ്ഥിരം അക്ഷരം *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        എങ്കിൽ(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          തിരികെ result;
        }
      }
      നിർത്തുക;
    അവസ്ഥ TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      സ്ഥിരം അക്ഷരം *str = (സ്ഥിരം അക്ഷരം *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      എങ്കിൽ(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      നിർത്തുക;
    }
    അവസ്ഥ TFTP_EVENT_ACK:
      നിർത്തുക;
    അവസ്ഥ TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (സ്ഥിരം അക്ഷരം *)state->rpacket.data + 2,
                                     state->rbytes-2);
      എങ്കിൽ(result)
        തിരികെ result;
      നിർത്തുക;
    അവസ്ഥ TFTP_EVENT_RRQ:
    അവസ്ഥ TFTP_EVENT_WRQ:
    സ്ഥിരസ്ഥിതി:
      failf(data, "%s", "Internal error: Unexpected packet");
      നിർത്തുക;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    എങ്കിൽ(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      തിരികെ result;
    }
  }
  തിരികെ result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
സ്ഥിതം timediff_t tftp_state_timeout(ഘടന tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  എങ്കിൽ(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  എങ്കിൽ(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    തിരികെ timeout_ms;
  }
  current = time(ശൂന്യ);
  എങ്കിൽ(current > state->rx_time + state->retry_time) {
    എങ്കിൽ(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(ശൂന്യ); /* update even though we received nothing */
  }

  തിരികെ timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_multi_statemach(ഘടന Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  എങ്കിൽ(!state)
    തിരികെ CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  എങ്കിൽ(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }
  എങ്കിൽ(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    എങ്കിൽ(result)
      തിരികെ result;
    *done = (state->state == TFTP_STATE_FIN);
    എങ്കിൽ(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  അല്ലെങ്കിൽ {
    /* no timeouts to handle, check our socket */
    പൂർണ്ണസംഖ്യ rc = SOCKET_READABLE(state->sockfd, 0);

    എങ്കിൽ(rc == -1) {
      /* bail out */
      പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(rc) {
      result = tftp_receive_packet(data, state);
      എങ്കിൽ(result)
        തിരികെ result;
      result = tftp_state_machine(state, state->event);
      എങ്കിൽ(result)
        തിരികെ result;
      *done = (state->state == TFTP_STATE_FIN);
      എങ്കിൽ(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  തിരികെ result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_doing(ഘടന Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  എങ്കിൽ(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  തിരികെ result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
സ്ഥിതം CURLcode tftp_perform(ഘടന Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  എങ്കിൽ(!state)
    തിരികെ CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  എങ്കിൽ((state->state == TFTP_STATE_FIN) || result)
    തിരികെ result;

  result = tftp_multi_statemach(data, dophase_done);

  എങ്കിൽ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  തിരികെ result;
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

സ്ഥിതം CURLcode tftp_do(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  എങ്കിൽ(!state) {
    result = tftp_connect(data, done);
    എങ്കിൽ(result)
      തിരികെ result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    എങ്കിൽ(!state)
      തിരികെ CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  എങ്കിൽ(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  തിരികെ result;
}

സ്ഥിതം CURLcode tftp_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  അക്ഷരം *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  എങ്കിൽ((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  അല്ലെങ്കിൽ എങ്കിൽ((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  തിരികെ CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_tftp = {
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
