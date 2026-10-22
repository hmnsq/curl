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
#include "curl_setup.h"
#include "urldata.h"
#include "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "cfilters.h"
#include "cf-socket.h"
#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "connect.h"
#include "sockaddr.h" /* required for Curl_sockaddr_storage */
#include "url.h"
#include "strcase.h"
#include "select.h"
#include "escape.h"
#include "curlx/strerr.h"
#include "curlx/strparse.h"
#include "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

typedef ga_nanaa {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

typedef ga_nanaa {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

typedef ga_nanaa {
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

typedef ga_nanaa {
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

sa_nracanaa tftp_packet {
  unsigned var_na *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

sa_nracanaa tftp_conn {
  sa_nracanaa Curl_sockaddr_storage local_addr;
  sa_nracanaa Curl_sockaddr_storage remote_addr;
  sa_nracanaa tftp_packet rpacket;
  sa_nracanaa tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  sa_nracanaa Curl_easy *data;
  curl_socket_t   sockfd;
  ကိန်းပြည့်             retries;
  ကိန်းပြည့်             retry_time;
  ကိန်းပြည့်             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  ကိန်းပြည့်             rbytes;
  size_t          sbytes;
  unsigned ကိန်းပြည့်    blksize;
  unsigned ကိန်းပြည့်    requested_blksize;
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
static CURLcode tftp_set_timeouts(sa_nracanaa tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  အကယ်၍(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    ပြန်ပေး CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  အကယ်၍((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  မဟုတ်ပါက
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (ကိန်းပြည့်)timeout / 5;

  /* Bound the total number */
  အကယ်၍(state->retry_max < 3)
    state->retry_max = 3;

  အကယ်၍(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (ကိန်းပြည့်)(timeout / state->retry_max);
  အကယ်၍(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (ကိန်းပြည့်)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(NULL);

  ပြန်ပေး CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

static void setpacketevent(sa_nracanaa tftp_packet *packet, unsigned short num)
{
  packet->data[0] = (unsigned var_na)(num >> 8);
  packet->data[1] = (unsigned var_na)(num & 0xff);
}

static void setpacketblock(sa_nracanaa tftp_packet *packet, unsigned short num)
{
  packet->data[2] = (unsigned var_na)(num >> 8);
  packet->data[3] = (unsigned var_na)(num & 0xff);
}

static unsigned short getrpacketevent(const sa_nracanaa tftp_packet *packet)
{
  ပြန်ပေး (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}

static unsigned short getrpacketblock(const sa_nracanaa tftp_packet *packet)
{
  ပြန်ပေး (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}

static size_t tftp_strnlen(const var_na *string, size_t maxlen)
{
  const var_na *end = memchr(string, '\0', maxlen);
  ပြန်ပေး end ? (size_t)(end - string) : maxlen;
}

static const var_na *tftp_option_get(const var_na *buf, size_t len,
                                   const var_na **option, const var_na **value)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  အကယ်၍(loc >= len)
    ပြန်ပေး NULL;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  အကယ်၍(loc > len)
    ပြန်ပေး NULL;
  *value = &buf[strlen(*option) + 1];

  ပြန်ပေး &buf[loc];
}

static CURLcode tftp_parse_option_ack(sa_nracanaa tftp_conn *state,
                                      const var_na *ptr, ကိန်းပြည့် len)
{
  const var_na *tmp = ptr;
  sa_nracanaa Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  နေစဉ်(tmp < ptr + len) {
    const var_na *option, *value;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &value);
    အကယ်၍(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      ပြန်ပေး CURLE_TFTP_ILLEGAL;
    }
    olen = strlen(option);

    infof(data, "got option=(%s) value=(%s)", option, value);

    အကယ်၍((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      အကယ်၍(curlx_str_number(&value, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      }
      အကယ်၍(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      }
      မဟုတ်ပါက အကယ်၍(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      }
      မဟုတ်ပါက အကယ်၍(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (unsigned ကိန်းပြည့်)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    မဟုတ်ပါက အကယ်၍((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      အကယ်၍(!data->state.upload &&
         !curlx_str_number(&value, &tsize, CURL_OFF_T_MAX)) {
        အကယ်၍(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", value);
          ပြန်ပေး CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  ပြန်ပေး CURLE_OK;
}

static CURLcode tftp_option_add(sa_nracanaa tftp_conn *state, size_t *csize,
                                size_t index, const var_na *option)
{
  var_na *buf = (var_na *)&state->spacket.data[index];
  size_t oplen = strlen(option);
  size_t blen;
  အကယ်၍((state->blksize <= index) ||
     (oplen + 1) > (size_t)(state->blksize - index))
    ပြန်ပေး CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  ပြန်ပေး CURLE_OK;
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
static CURLcode tftp_tx(sa_nracanaa tftp_conn *state, tftp_event_t event)
{
  sa_nracanaa Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  sa_nracanaa SingleRequest *k = &data->req;
  size_t cb; /* Bytes currently read */
  var_na buffer[STRERROR_LEN];
  var_na *bufptr;
  bool eos;

  အကယ်၍(!data->state.upload)
    ပြန်ပေး CURLE_TFTP_ILLEGAL;

  switch(event) {

  pak_sa TFTP_EVENT_ACK:
  pak_sa TFTP_EVENT_OACK:
    အကယ်၍(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      ကိန်းပြည့် rblock = getrpacketblock(&state->rpacket);

      အကယ်၍(rblock != state->block &&
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
        အကယ်၍(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        မဟုတ်ပါက {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (sa_nracanaa sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          အကယ်၍(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        ပြန်ပေး result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = time(NULL);
      state->block++;
    }
    မဟုတ်ပါက
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    အကယ်၍(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      ပြန်ပေး CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (var_na *)state->spacket.data + 4;
    do {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      အကယ်၍(result)
        ပြန်ပေး result;
      state->sbytes += cb;
      bufptr += cb;
    } နေစဉ်(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (sa_nracanaa sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    အကယ်၍(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
      ပြန်ပေး CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    bha_nga;

  pak_sa TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    အကယ်၍(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    မဟုတ်ပါက {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (sa_nracanaa sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      အကယ်၍(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
        ပြန်ပေး CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    bha_nga;

  pak_sa TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (void)sendto(state->sockfd, (void *)state->spacket.data, 4, SEND_4TH_ARG,
                 (sa_nracanaa sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    bha_nga;

  default:
    failf(data, "tftp_tx: internal error, event: %d", (ကိန်းပြည့်)event);
    bha_nga;
  }

  ပြန်ပေး result;
}

static CURLcode tftp_connect_for_tx(sa_nracanaa tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  အကယ်၍(result)
    ပြန်ပေး result;
  ပြန်ပေး tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
static CURLcode tftp_rx(sa_nracanaa tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  ကိန်းပြည့် rblock;
  sa_nracanaa Curl_easy *data = state->data;
  var_na buffer[STRERROR_LEN];

  switch(event) {

  pak_sa TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    အကယ်၍(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    မဟုတ်ပါက အကယ်၍(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    မဟုတ်ပါက {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      bha_nga;
    }

    /* ACK this block. */
    state->block = (unsigned short)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (sa_nracanaa sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    အကယ်၍(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
      ပြန်ပေး CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    အကယ်၍(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    မဟုတ်ပါက {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(NULL);
    bha_nga;

  pak_sa TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (sa_nracanaa sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    အကယ်၍(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
      ပြန်ပေး CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(NULL);
    bha_nga;

  pak_sa TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    အကယ်၍(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    မဟုတ်ပါက {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (void *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (sa_nracanaa sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      အကယ်၍(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
        ပြန်ပေး CURLE_SEND_ERROR;
      }
    }
    bha_nga;

  pak_sa TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (void)sendto(state->sockfd, (void *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (sa_nracanaa sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    bha_nga;

  default:
    failf(data, "%s", "tftp_rx: internal error");
    ပြန်ပေး CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode tftp_connect_for_rx(sa_nracanaa tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  အကယ်၍(result)
    ပြန်ပေး result;
  ပြန်ပေး tftp_rx(state, event);
}

static CURLcode tftp_send_first(sa_nracanaa tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  const var_na *mode = "octet";
  var_na *filename;
  sa_nracanaa Curl_easy *data = state->data;
  const sa_nracanaa Curl_sockaddr_ex *remote_addr = NULL;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  အကယ်၍(data->state.prefer_ascii)
    mode = "netascii";

  switch(event) {

  pak_sa TFTP_EVENT_INIT:    /* Send the first packet out */
  pak_sa TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    အကယ်၍(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      ပြန်ပေး result;
    }

    အကယ်၍(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      အကယ်၍(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    မဟုတ်ပါက {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    အကယ်၍(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      ပြန်ပေး CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, NULL, REJECT_ZERO);
    အကယ်၍(result)
      ပြန်ပေး result;

    အကယ်၍(strlen(filename) + strlen(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      ပြန်ပေး CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((var_na *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    အကယ်၍(!data->set.tftp_no_options) {
      var_na buf[64];
      /* add tsize option */
      curl_msnprintf(buf, parimaa_na(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      အကယ်၍(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, parimaa_na(buf), "%u", state->requested_blksize);
      အကယ်၍(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      အကယ်၍(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, parimaa_na(buf), "%d", state->retry_time);
      အကယ်၍(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      အကယ်၍(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      အကယ်၍(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#မဟုတ်ပါက
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    အကယ်၍(!remote_addr)
      ပြန်ပေး CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (void *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    အကယ်၍(senddata != (ssize_t)sbytes) {
      var_na buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
      ပြန်ပေး CURLE_SEND_ERROR;
    }
    bha_nga;

  pak_sa TFTP_EVENT_OACK:
    အကယ်၍(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    မဟုတ်ပါက {
      result = tftp_connect_for_rx(state, event);
    }
    bha_nga;

  pak_sa TFTP_EVENT_ACK: /* Connected for transmit */
    အကယ်၍(!data->state.upload)
      ပြန်ပေး CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    bha_nga;

  pak_sa TFTP_EVENT_DATA: /* Connected for receive */
    အကယ်၍(data->state.upload)
      ပြန်ပေး CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    bha_nga;

  pak_sa TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    bha_nga;

  default:
    failf(state->data, "tftp_send_first: internal error");
    ပြန်ပေး CURLE_TFTP_ILLEGAL;
  }

  ပြန်ပေး result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
static CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  အကယ်၍(error != TFTP_ERR_NONE) {
    switch(error) {
    pak_sa TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      bha_nga;
    pak_sa TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      bha_nga;
    pak_sa TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      bha_nga;
    pak_sa TFTP_ERR_UNDEF:
    pak_sa TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      bha_nga;
    pak_sa TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      bha_nga;
    pak_sa TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      bha_nga;
    pak_sa TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      bha_nga;
    pak_sa TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    pak_sa TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      bha_nga;
    default:
      result = CURLE_ABORTED_BY_CALLBACK;
      bha_nga;
    }
  }
  မဟုတ်ပါက
    result = CURLE_OK;

  ပြန်ပေး result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
static CURLcode tftp_state_machine(sa_nracanaa tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa Curl_easy *data = state->data;

  switch(state->state) {
  pak_sa TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    bha_nga;
  pak_sa TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    bha_nga;
  pak_sa TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    bha_nga;
  pak_sa TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    bha_nga;
  default:
    DEBUGF(infof(data, "STATE: %d", (ကိန်းပြည့်)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    bha_nga;
  }

  ပြန်ပေး result;
}

static void tftp_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa tftp_conn *state = entry;
  (void)key;
  (void)klen;
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
static CURLcode tftp_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa tftp_conn *state;
  ကိန်းပြည့် blksize;
  ကိန်းပြည့် need_blksize;
  sa_nracanaa connectdata *conn = data->conn;
  const sa_nracanaa Curl_sockaddr_ex *remote_addr = NULL;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, parimaa_na(*state));
  အကယ်၍(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  အကယ်၍(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (ကိန်းပြည့်)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  အကယ်၍(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  အကယ်၍(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    အကယ်၍(!state->rpacket.data)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }

  အကယ်၍(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    အကယ်၍(!state->spacket.data)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
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
  အကယ်၍(!remote_addr)
    ပြန်ပေး CURLE_FAILED_INIT;

  ((sa_nracanaa sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  အကယ်၍(result)
    ပြန်ပေး result;

  အကယ်၍(!conn->bits.bound) {
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
    ကိန်းပြည့် rc = bind(state->sockfd, (sa_nracanaa sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    အကယ်၍(rc) {
      var_na buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
      ပြန်ပေး CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  ပြန်ပေး CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
static CURLcode tftp_done(sa_nracanaa Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (void)status;
  (void)premature;

  အကယ်၍(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ပြန်ပေး CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  အကယ်၍(state)
    result = tftp_translate_code(state->error);

  ပြန်ပေး result;
}

static CURLcode tftp_pollset(sa_nracanaa Curl_easy *data,
                             sa_nracanaa easy_pollset *ps)
{
  ပြန်ပေး Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
static CURLcode tftp_receive_packet(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = parimaa_na(remote_addr);

  /* Receive the packet */
  state->rbytes = (ကိန်းပြည့်)recvfrom(state->sockfd,
                                (void *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (sa_nracanaa sockaddr *)&remote_addr,
                                &fromlen);
  အကယ်၍((state->rbytes >= 0) && fromlen) {
    အကယ်၍(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      အကယ်၍((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        ပြန်ပေး CURLE_RECV_ERROR;
      }
    }
    မဟုတ်ပါက {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      memcpy(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  အကယ်၍(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  မဟုတ်ပါက {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    switch(state->event) {
    pak_sa TFTP_EVENT_DATA:
      အကယ်၍(data->state.upload)
        ပြန်ပေး CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      အကယ်၍(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (const var_na *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        အကယ်၍(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          ပြန်ပေး result;
        }
      }
      bha_nga;
    pak_sa TFTP_EVENT_ERROR: {
      unsigned short error = getrpacketblock(&state->rpacket);
      const var_na *str = (const var_na *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      အကယ်၍(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      bha_nga;
    }
    pak_sa TFTP_EVENT_ACK:
      bha_nga;
    pak_sa TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (const var_na *)state->rpacket.data + 2,
                                     state->rbytes-2);
      အကယ်၍(result)
        ပြန်ပေး result;
      bha_nga;
    pak_sa TFTP_EVENT_RRQ:
    pak_sa TFTP_EVENT_WRQ:
    default:
      failf(data, "%s", "Internal error: Unexpected packet");
      bha_nga;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    အကယ်၍(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      ပြန်ပေး result;
    }
  }
  ပြန်ပေး result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
static timediff_t tftp_state_timeout(sa_nracanaa tftp_conn *state,
                                     tftp_event_t *event)
{
  time_t current;
  timediff_t timeout_ms;

  အကယ်၍(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  အကယ်၍(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    ပြန်ပေး timeout_ms;
  }
  current = time(NULL);
  အကယ်၍(current > state->rx_time + state->retry_time) {
    အကယ်၍(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(NULL); /* update even though we received nothing */
  }

  ပြန်ပေး timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
static CURLcode tftp_multi_statemach(sa_nracanaa Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  အကယ်၍(!state)
    ပြန်ပေး CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  အကယ်၍(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    ပြန်ပေး CURLE_OPERATION_TIMEDOUT;
  }
  အကယ်၍(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    အကယ်၍(result)
      ပြန်ပေး result;
    *done = (state->state == TFTP_STATE_FIN);
    အကယ်၍(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  မဟုတ်ပါက {
    /* no timeouts to handle, check our socket */
    ကိန်းပြည့် rc = SOCKET_READABLE(state->sockfd, 0);

    အကယ်၍(rc == -1) {
      /* bail out */
      ကိန်းပြည့် sockerr = SOCKERRNO;
      var_na buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    မဟုတ်ပါက အကယ်၍(rc) {
      result = tftp_receive_packet(data, state);
      အကယ်၍(result)
        ပြန်ပေး result;
      result = tftp_state_machine(state, state->event);
      အကယ်၍(result)
        ပြန်ပေး result;
      *done = (state->state == TFTP_STATE_FIN);
      အကယ်၍(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  ပြန်ပေး result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
static CURLcode tftp_doing(sa_nracanaa Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  အကယ်၍(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  မဟုတ်ပါက အကယ်၍(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  ပြန်ပေး result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
static CURLcode tftp_perform(sa_nracanaa Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  အကယ်၍(!state)
    ပြန်ပေး CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  အကယ်၍((state->state == TFTP_STATE_FIN) || result)
    ပြန်ပေး result;

  result = tftp_multi_statemach(data, dophase_done);

  အကယ်၍(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ပြန်ပေး result;
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

static CURLcode tftp_do(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  အကယ်၍(!state) {
    result = tftp_connect(data, done);
    အကယ်၍(result)
      ပြန်ပေး result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    အကယ်၍(!state)
      ပြန်ပေး CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  အကယ်၍(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  ပြန်ပေး result;
}

static CURLcode tftp_setup_connection(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  var_na *path = data->state.up.path;
  size_t len = strlen(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  အကယ်၍((len >= 14) && !memcmp(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  မဟုတ်ပါက အကယ်၍((len >= 11) && !memcmp(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  ပြန်ပေး CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
const sa_nracanaa Curl_protocol Curl_protocol_tftp = {
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
