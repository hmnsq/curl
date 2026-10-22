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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "tftp.h"

#ifndef CURL_DISABLE_TFTP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "sockaddr.h" /* required for Curl_sockaddr_storage */
#अन्तर्भाव "url.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#define TFTP_BLKSIZE_DEFAULT 512
#define TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#define TFTP_OPTION_TSIZE    "tsize"
#define TFTP_OPTION_INTERVAL "timeout"

प्रकारव्याख्या गणना {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

प्रकारव्याख्या गणना {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

प्रकारव्याख्या गणना {
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

प्रकारव्याख्या गणना {
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

संरचना tftp_packet {
  अचिन्हित अक्षर *data;
};

/* meta key for storing protocol meta at connection */
#define CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

संरचना tftp_conn {
  संरचना Curl_sockaddr_storage local_addr;
  संरचना Curl_sockaddr_storage remote_addr;
  संरचना tftp_packet rpacket;
  संरचना tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  संरचना Curl_easy *data;
  curl_socket_t   sockfd;
  पूर्णांक             retries;
  पूर्णांक             retry_time;
  पूर्णांक             retry_max;
  समय_प्रकार          rx_time;
  curl_socklen_t  remote_addrlen;
  पूर्णांक             rbytes;
  माप_प्रकार          sbytes;
  अचिन्हित पूर्णांक    blksize;
  अचिन्हित पूर्णांक    requested_blksize;
  अचिन्हित लघु  block;
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
स्थिर CURLcode tftp_set_timeouts(संरचना tftp_conn *state)
{
  समय_प्रकार timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->data);

  जर(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->data, "Connection time-out");
    परत CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  जर((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (समय_प्रकार)(timeout_ms + 500) / 1000;
  अन्यथा
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (पूर्णांक)timeout / 5;

  /* Bound the total number */
  जर(state->retry_max < 3)
    state->retry_max = 3;

  जर(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (पूर्णांक)(timeout / state->retry_max);
  जर(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->data,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (पूर्णांक)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = समय(रिक्त);

  परत CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

स्थिर शून्य setpacketevent(संरचना tftp_packet *packet, अचिन्हित लघु अंक)
{
  packet->data[0] = (अचिन्हित अक्षर)(अंक >> 8);
  packet->data[1] = (अचिन्हित अक्षर)(अंक & 0xff);
}

स्थिर शून्य setpacketblock(संरचना tftp_packet *packet, अचिन्हित लघु अंक)
{
  packet->data[2] = (अचिन्हित अक्षर)(अंक >> 8);
  packet->data[3] = (अचिन्हित अक्षर)(अंक & 0xff);
}

स्थिर अचिन्हित लघु getrpacketevent(स्थिरांक संरचना tftp_packet *packet)
{
  परत (अचिन्हित लघु)((packet->data[0] << 8) | packet->data[1]);
}

स्थिर अचिन्हित लघु getrpacketblock(स्थिरांक संरचना tftp_packet *packet)
{
  परत (अचिन्हित लघु)((packet->data[2] << 8) | packet->data[3]);
}

स्थिर माप_प्रकार tftp_strnlen(स्थिरांक अक्षर *string, माप_प्रकार maxlen)
{
  स्थिरांक अक्षर *end = स_प्रथम(string, '\0', maxlen);
  परत end ? (माप_प्रकार)(end - string) : maxlen;
}

स्थिर स्थिरांक अक्षर *tftp_option_get(स्थिरांक अक्षर *buf, माप_प्रकार len,
                                   स्थिरांक अक्षर **option, स्थिरांक अक्षर **मूल्य)
{
  माप_प्रकार loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  जर(loc >= len)
    परत रिक्त;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  जर(loc > len)
    परत रिक्त;
  *मूल्य = &buf[म_माप(*option) + 1];

  परत &buf[loc];
}

स्थिर CURLcode tftp_parse_option_ack(संरचना tftp_conn *state,
                                      स्थिरांक अक्षर *ptr, पूर्णांक len)
{
  स्थिरांक अक्षर *tmp = ptr;
  संरचना Curl_easy *data = state->data;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  पर्यंत(tmp < ptr + len) {
    स्थिरांक अक्षर *option, *मूल्य;
    माप_प्रकार olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &मूल्य);
    जर(!tmp) {
      failf(data, "Malformed ACK packet, rejecting");
      परत CURLE_TFTP_ILLEGAL;
    }
    olen = म_माप(option);

    infof(data, "got option=(%s) value=(%s)", option, मूल्य);

    जर((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      जर(curlx_str_संख्या(&मूल्य, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(data, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        परत CURLE_TFTP_ILLEGAL;
      }
      जर(!blksize) {
        failf(data, "invalid blocksize value in OACK packet");
        परत CURLE_TFTP_ILLEGAL;
      }
      अन्यथा जर(blksize < TFTP_BLKSIZE_MIN) {
        failf(data, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        परत CURLE_TFTP_ILLEGAL;
      }
      अन्यथा जर(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(data, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        परत CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (अचिन्हित पूर्णांक)blksize;
      infof(data, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    अन्यथा जर((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      जर(!data->state.upload &&
         !curlx_str_संख्या(&मूल्य, &tsize, CURL_OFF_T_MAX)) {
        जर(!tsize) {
          failf(data, "invalid tsize -:%s:- value in OACK packet", मूल्य);
          परत CURLE_TFTP_ILLEGAL;
        }
        infof(data, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrsSetDownloadSize(data, tsize);
      }
    }
  }

  परत CURLE_OK;
}

स्थिर CURLcode tftp_option_add(संरचना tftp_conn *state, माप_प्रकार *csize,
                                माप_प्रकार index, स्थिरांक अक्षर *option)
{
  अक्षर *buf = (अक्षर *)&state->spacket.data[index];
  माप_प्रकार oplen = म_माप(option);
  माप_प्रकार blen;
  जर((state->blksize <= index) ||
     (oplen + 1) > (माप_प्रकार)(state->blksize - index))
    परत CURLE_TFTP_ILLEGAL;
  blen = state->blksize - index;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  परत CURLE_OK;
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
स्थिर CURLcode tftp_tx(संरचना tftp_conn *state, tftp_event_t event)
{
  संरचना Curl_easy *data = state->data;
  ssize_t sbytes;
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  माप_प्रकार cb; /* Bytes currently read */
  अक्षर buffer[STRERROR_LEN];
  अक्षर *bufptr;
  bool eos;

  जर(!data->state.upload)
    परत CURLE_TFTP_ILLEGAL;

  निवड(event) {

  स्थिती TFTP_EVENT_ACK:
  स्थिती TFTP_EVENT_OACK:
    जर(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      पूर्णांक rblock = getrpacketblock(&state->rpacket);

      जर(rblock != state->block &&
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
        जर(state->retries > state->retry_max) {
          failf(data, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          result = CURLE_SEND_ERROR;
        }
        अन्यथा {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (संरचना sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          जर(sbytes < 0) {
            failf(data, "%s",
                  curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
            result = CURLE_SEND_ERROR;
          }
        }

        परत result;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = समय(रिक्त);
      state->block++;
    }
    अन्यथा
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_DATA);
    setpacketblock(&state->spacket, state->block);
    जर(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      परत CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (अक्षर *)state->spacket.data + 4;
    करा {
      result = Curl_client_read(data, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      जर(result)
        परत result;
      state->sbytes += cb;
      bufptr += cb;
    } पर्यंत(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    जर(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
      परत CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(data, state->sbytes);
    थांबा;

  स्थिती TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(data, "Timeout waiting for block %d ACK. "
          "Retries = %d", NEXT_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    जर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (संरचना sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      जर(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
        परत CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }
    थांबा;

  स्थिती TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (शून्य)sendto(state->sockfd, (शून्य *)state->spacket.data, 4, SEND_4TH_ARG,
                 (संरचना sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    थांबा;

  पूर्वनिर्धारित:
    failf(data, "tftp_tx: internal error, event: %d", (पूर्णांक)event);
    थांबा;
  }

  परत result;
}

स्थिर CURLcode tftp_connect_for_tx(संरचना tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  result = tftp_set_timeouts(state);
  जर(result)
    परत result;
  परत tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
स्थिर CURLcode tftp_rx(संरचना tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  पूर्णांक rblock;
  संरचना Curl_easy *data = state->data;
  अक्षर buffer[STRERROR_LEN];

  निवड(event) {

  स्थिती TFTP_EVENT_DATA:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    जर(NEXT_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    अन्यथा जर(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(data, "Received last DATA packet block %d again.", rblock);
    }
    अन्यथा {
      /* totally unexpected, log it */
      infof(data,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, NEXT_BLOCKNUM(state->block));
      थांबा;
    }

    /* ACK this block. */
    state->block = (अचिन्हित लघु)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    जर(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
      परत CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    जर(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = समय(रिक्त);
    थांबा;

  स्थिती TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                    4, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    जर(sbytes < 0) {
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
      परत CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = समय(रिक्त);
    थांबा;

  स्थिती TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(data,
          "Timeout waiting for block %d ACK. Retries = %d",
          NEXT_BLOCKNUM(state->block), state->retries);
    जर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (शून्य *)state->spacket.data,
                      4, SEND_4TH_ARG,
                      (संरचना sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      जर(sbytes < 0) {
        failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
        परत CURLE_SEND_ERROR;
      }
    }
    थांबा;

  स्थिती TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (शून्य)sendto(state->sockfd, (शून्य *)state->spacket.data,
                 4, SEND_4TH_ARG,
                 (संरचना sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    थांबा;

  पूर्वनिर्धारित:
    failf(data, "%s", "tftp_rx: internal error");
    परत CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  परत CURLE_OK;
}

स्थिर CURLcode tftp_connect_for_rx(संरचना tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode result;

  infof(state->data, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  जर(result)
    परत result;
  परत tftp_rx(state, event);
}

स्थिर CURLcode tftp_send_first(संरचना tftp_conn *state,
                                tftp_event_t event)
{
  माप_प्रकार sbytes;
  ssize_t senddata;
  स्थिरांक अक्षर *mode = "octet";
  अक्षर *filename;
  संरचना Curl_easy *data = state->data;
  स्थिरांक संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  CURLcode result = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  जर(data->state.prefer_ascii)
    mode = "netascii";

  निवड(event) {

  स्थिती TFTP_EVENT_INIT:    /* Send the first packet out */
  स्थिती TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    जर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      परत result;
    }

    जर(data->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      जर(data->state.infilesize != -1)
        Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }
    अन्यथा {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    जर(!state->data->state.up.path[1]) {
      failf(data, "Missing filename");
      परत CURLE_TFTP_ILLEGAL;
    }
    result = Curl_urldecode(&state->data->state.up.path[1], 0,
                            &filename, रिक्त, REJECT_ZERO);
    जर(result)
      परत result;

    जर(म_माप(filename) + म_माप(mode) + 4 > state->blksize) {
      failf(data, "TFTP filename too long");
      curlx_free(filename);
      परत CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((अक्षर *)state->spacket.data + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    जर(!data->set.tftp_no_options) {
      अक्षर buf[64];
      /* add tsize option */
      curl_msnprintf(buf, आकार(buf), "%" FMT_OFF_T,
                     data->state.upload && (data->state.infilesize != -1) ?
                     data->state.infilesize : 0);

      result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      जर(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, आकार(buf), "%u", state->requested_blksize);
      जर(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      जर(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, आकार(buf), "%d", state->retry_time);
      जर(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      जर(result == CURLE_OK)
        result = tftp_option_add(state, &sbytes, sbytes, buf);

      जर(result != CURLE_OK) {
        failf(data, "TFTP buffer too small for options");
        परत CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#ifdef __AMIGA__
#define CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#अन्यथा
#define CURL_SENDTO_ARG5(x) (x)
#endif
    remote_addr = Curl_conn_get_remote_addr(data, FIRSTSOCKET);
    जर(!remote_addr)
      परत CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (शून्य *)state->spacket.data,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    जर(senddata != (ssize_t)sbytes) {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
      परत CURLE_SEND_ERROR;
    }
    थांबा;

  स्थिती TFTP_EVENT_OACK:
    जर(data->state.upload) {
      result = tftp_connect_for_tx(state, event);
    }
    अन्यथा {
      result = tftp_connect_for_rx(state, event);
    }
    थांबा;

  स्थिती TFTP_EVENT_ACK: /* Connected for transmit */
    जर(!data->state.upload)
      परत CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_tx(state, event);
    थांबा;

  स्थिती TFTP_EVENT_DATA: /* Connected for receive */
    जर(data->state.upload)
      परत CURLE_TFTP_ILLEGAL;
    result = tftp_connect_for_rx(state, event);
    थांबा;

  स्थिती TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    थांबा;

  पूर्वनिर्धारित:
    failf(state->data, "tftp_send_first: internal error");
    परत CURLE_TFTP_ILLEGAL;
  }

  परत result;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
स्थिर CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  जर(error != TFTP_ERR_NONE) {
    निवड(error) {
    स्थिती TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      थांबा;
    स्थिती TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      थांबा;
    स्थिती TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      थांबा;
    स्थिती TFTP_ERR_UNDEF:
    स्थिती TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      थांबा;
    स्थिती TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      थांबा;
    स्थिती TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      थांबा;
    स्थिती TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      थांबा;
    स्थिती TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      थांबा;
    स्थिती TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      थांबा;
    पूर्वनिर्धारित:
      result = CURLE_ABORTED_BY_CALLBACK;
      थांबा;
    }
  }
  अन्यथा
    result = CURLE_OK;

  परत result;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
स्थिर CURLcode tftp_state_machine(संरचना tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  संरचना Curl_easy *data = state->data;

  निवड(state->state) {
  स्थिती TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START"));
    result = tftp_send_first(state, event);
    थांबा;
  स्थिती TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX"));
    result = tftp_rx(state, event);
    थांबा;
  स्थिती TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX"));
    result = tftp_tx(state, event);
    थांबा;
  स्थिती TFTP_STATE_FIN:
    infof(data, "%s", "TFTP finished");
    थांबा;
  पूर्वनिर्धारित:
    DEBUGF(infof(data, "STATE: %d", (पूर्णांक)state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    थांबा;
  }

  परत result;
}

स्थिर शून्य tftp_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना tftp_conn *state = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
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
स्थिर CURLcode tftp_connect(संरचना Curl_easy *data, bool *done)
{
  संरचना tftp_conn *state;
  पूर्णांक blksize;
  पूर्णांक need_blksize;
  संरचना connectdata *conn = data->conn;
  स्थिरांक संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  CURLcode result;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, आकार(*state));
  जर(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    परत CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  जर(data->set.tftp_blksize)
    /* range checked when set */
    blksize = (पूर्णांक)data->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  जर(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  जर(!state->rpacket.data) {
    state->rpacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    जर(!state->rpacket.data)
      परत CURLE_OUT_OF_MEMORY;
  }

  जर(!state->spacket.data) {
    state->spacket.data = curlx_calloc(1, need_blksize + 2 + 2);

    जर(!state->spacket.data)
      परत CURLE_OUT_OF_MEMORY;
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
  जर(!remote_addr)
    परत CURLE_FAILED_INIT;

  ((संरचना sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  result = tftp_set_timeouts(state);
  जर(result)
    परत result;

  जर(!conn->bits.bound) {
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
    पूर्णांक rc = bind(state->sockfd, (संरचना sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    जर(rc) {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
      परत CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  परत CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
स्थिर CURLcode tftp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (शून्य)status;
  (शून्य)premature;

  जर(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    परत CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  जर(state)
    result = tftp_translate_code(state->error);

  परत result;
}

स्थिर CURLcode tftp_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  परत Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
स्थिर CURLcode tftp_receive_packet(संरचना Curl_easy *data,
                                    संरचना tftp_conn *state)
{
  CURLcode result = CURLE_OK;
  संरचना Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = आकार(remote_addr);

  /* Receive the packet */
  state->rbytes = (पूर्णांक)recvfrom(state->sockfd,
                                (शून्य *)state->rpacket.data,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (संरचना sockaddr *)&remote_addr,
                                &fromlen);
  जर((state->rbytes >= 0) && fromlen) {
    जर(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      जर((state->remote_addrlen != fromlen) ||
         स_भेद(&remote_addr, &state->remote_addr, fromlen)) {
        failf(data, "Data received from another address");
        परत CURLE_RECV_ERROR;
      }
    }
    अन्यथा {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      स_नकल(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  जर(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  अन्यथा {
    /* The event is given by the TFTP packet time */
    अचिन्हित लघु event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    निवड(state->event) {
    स्थिती TFTP_EVENT_DATA:
      जर(data->state.upload)
        परत CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      जर(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   (स्थिरांक अक्षर *)state->rpacket.data + 4,
                                   state->rbytes - 4);
        जर(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          परत result;
        }
      }
      थांबा;
    स्थिती TFTP_EVENT_ERROR: {
      अचिन्हित लघु error = getrpacketblock(&state->rpacket);
      स्थिरांक अक्षर *str = (स्थिरांक अक्षर *)state->rpacket.data + 4;
      माप_प्रकार strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      जर(tftp_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s", str);
      थांबा;
    }
    स्थिती TFTP_EVENT_ACK:
      थांबा;
    स्थिती TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (स्थिरांक अक्षर *)state->rpacket.data + 2,
                                     state->rbytes-2);
      जर(result)
        परत result;
      थांबा;
    स्थिती TFTP_EVENT_RRQ:
    स्थिती TFTP_EVENT_WRQ:
    पूर्वनिर्धारित:
      failf(data, "%s", "Internal error: Unexpected packet");
      थांबा;
    }

    /* Update the progress meter */
    result = Curl_pgrsUpdate(data);
    जर(result) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      परत result;
    }
  }
  परत result;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
स्थिर timediff_t tftp_state_timeout(संरचना tftp_conn *state,
                                     tftp_event_t *event)
{
  समय_प्रकार current;
  timediff_t timeout_ms;

  जर(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->data);
  जर(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    परत timeout_ms;
  }
  current = समय(रिक्त);
  जर(current > state->rx_time + state->retry_time) {
    जर(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = समय(रिक्त); /* update even though we received nothing */
  }

  परत timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
स्थिर CURLcode tftp_multi_statemach(संरचना Curl_easy *data, bool *done)
{
  tftp_event_t event;
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  जर(!state)
    परत CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  जर(timeout_ms < 0) {
    failf(data, "TFTP response timeout");
    परत CURLE_OPERATION_TIMEDOUT;
  }
  जर(event != TFTP_EVENT_NONE) {
    result = tftp_state_machine(state, event);
    जर(result)
      परत result;
    *done = (state->state == TFTP_STATE_FIN);
    जर(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(data);
  }
  अन्यथा {
    /* no timeouts to handle, check our socket */
    पूर्णांक rc = SOCKET_READABLE(state->sockfd, 0);

    जर(rc == -1) {
      /* bail out */
      पूर्णांक sockerr = SOCKERRNO;
      अक्षर buffer[STRERROR_LEN];
      failf(data, "%s", curlx_strerror(sockerr, buffer, आकार(buffer)));
      state->event = TFTP_EVENT_ERROR;
    }
    अन्यथा जर(rc) {
      result = tftp_receive_packet(data, state);
      जर(result)
        परत result;
      result = tftp_state_machine(state, state->event);
      जर(result)
        परत result;
      *done = (state->state == TFTP_STATE_FIN);
      जर(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(data);
    }
    /* if rc == 0, then select() timed out */
  }

  परत result;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
स्थिर CURLcode tftp_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(data, dophase_done);

  जर(*dophase_done) {
    DEBUGF(infof(data, "DO phase is complete"));
  }
  अन्यथा जर(!result) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    result = Curl_pgrsCheck(data);
  }
  परत result;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
स्थिर CURLcode tftp_perform(संरचना Curl_easy *data, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  जर(!state)
    परत CURLE_FAILED_INIT;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  जर((state->state == TFTP_STATE_FIN) || result)
    परत result;

  result = tftp_multi_statemach(data, dophase_done);

  जर(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  परत result;
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

स्थिर CURLcode tftp_do(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode result;

  *done = FALSE;

  जर(!state) {
    result = tftp_connect(data, done);
    जर(result)
      परत result;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    जर(!state)
      परत CURLE_TFTP_ILLEGAL;
  }

  result = tftp_perform(data, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  जर(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  परत result;
}

स्थिर CURLcode tftp_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  अक्षर *path = data->state.up.path;
  माप_प्रकार len = म_माप(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  जर((len >= 14) && !स_भेद(&path[len - 14], ";mode=netascii", 14)) {
    data->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  अन्यथा जर((len >= 11) && !स_भेद(&path[len - 11], ";mode=octet", 11)) {
    data->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  परत CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_tftp = {
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
