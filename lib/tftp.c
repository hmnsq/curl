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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "tftp.h"

#अगर_अघोषित CURL_DISABLE_TFTP

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "connect.h"
#समावेश "sockaddr.h" /* required for Curl_sockaddr_storage */
#समावेश "url.h"
#समावेश "strcase.h"
#समावेश "select.h"
#समावेश "escape.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/strcopy.h"

/* RFC2348 allows the block size to be negotiated */
#घोषणा TFTP_BLKSIZE_DEFAULT 512
#घोषणा TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#घोषणा TFTP_OPTION_TSIZE    "tsize"
#घोषणा TFTP_OPTION_INTERVAL "timeout"

प्रकार क्रमागत {
  TFTP_MODE_NETASCII = 0,
  TFTP_MODE_OCTET
} tftp_mode_t;

प्रकार क्रमागत {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

प्रकार क्रमागत {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_तथ्य = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

प्रकार क्रमागत {
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

काष्ठा tftp_packet {
  अचिन्हित अक्षर *तथ्य;
};

/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

काष्ठा tftp_conn {
  काष्ठा Curl_sockaddr_storage local_addr;
  काष्ठा Curl_sockaddr_storage remote_addr;
  काष्ठा tftp_packet rpacket;
  काष्ठा tftp_packet spacket;
  tftp_state_t    state;
  tftp_mode_t     mode;
  tftp_error_t    error;
  tftp_event_t    event;
  काष्ठा Curl_easy *तथ्य;
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
अटल CURLcode tftp_set_timeouts(काष्ठा tftp_conn *state)
{
  समय_प्रकार timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->तथ्य);

  अगर(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->तथ्य, "Connection time-out");
    वापस CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  अगर((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (समय_प्रकार)(timeout_ms + 500) / 1000;
  अन्यथा
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (पूर्णांक)timeout / 5;

  /* Bound the total number */
  अगर(state->retry_max < 3)
    state->retry_max = 3;

  अगर(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (पूर्णांक)(timeout / state->retry_max);
  अगर(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->तथ्य,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (पूर्णांक)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = समय(शून्य);

  वापस CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

अटल व्योम setpacketevent(काष्ठा tftp_packet *packet, अचिन्हित लघु अंक)
{
  packet->तथ्य[0] = (अचिन्हित अक्षर)(अंक >> 8);
  packet->तथ्य[1] = (अचिन्हित अक्षर)(अंक & 0xff);
}

अटल व्योम setpacketblock(काष्ठा tftp_packet *packet, अचिन्हित लघु अंक)
{
  packet->तथ्य[2] = (अचिन्हित अक्षर)(अंक >> 8);
  packet->तथ्य[3] = (अचिन्हित अक्षर)(अंक & 0xff);
}

अटल अचिन्हित लघु getrpacketevent(स्थिर काष्ठा tftp_packet *packet)
{
  वापस (अचिन्हित लघु)((packet->तथ्य[0] << 8) | packet->तथ्य[1]);
}

अटल अचिन्हित लघु getrpacketblock(स्थिर काष्ठा tftp_packet *packet)
{
  वापस (अचिन्हित लघु)((packet->तथ्य[2] << 8) | packet->तथ्य[3]);
}

अटल माप_प्रकार tftp_strnlen(स्थिर अक्षर *string, माप_प्रकार maxlen)
{
  स्थिर अक्षर *end = स_प्रथम(string, '\0', maxlen);
  वापस end ? (माप_प्रकार)(end - string) : maxlen;
}

अटल स्थिर अक्षर *tftp_option_get(स्थिर अक्षर *buf, माप_प्रकार len,
                                   स्थिर अक्षर **option, स्थिर अक्षर **मूल्य)
{
  माप_प्रकार loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  अगर(loc >= len)
    वापस शून्य;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  अगर(loc > len)
    वापस शून्य;
  *मूल्य = &buf[म_माप(*option) + 1];

  वापस &buf[loc];
}

अटल CURLcode tftp_parse_option_ack(काष्ठा tftp_conn *state,
                                      स्थिर अक्षर *ptr, पूर्णांक len)
{
  स्थिर अक्षर *tmp = ptr;
  काष्ठा Curl_easy *तथ्य = state->तथ्य;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  जबतक(tmp < ptr + len) {
    स्थिर अक्षर *option, *मूल्य;
    माप_प्रकार olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &मूल्य);
    अगर(!tmp) {
      failf(तथ्य, "Malformed ACK packet, rejecting");
      वापस CURLE_TFTP_ILLEGAL;
    }
    olen = म_माप(option);

    infof(तथ्य, "got option=(%s) value=(%s)", option, मूल्य);

    अगर((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      अगर(curlx_str_संख्या(&मूल्य, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(तथ्य, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        वापस CURLE_TFTP_ILLEGAL;
      }
      अगर(!blksize) {
        failf(तथ्य, "invalid blocksize value in OACK packet");
        वापस CURLE_TFTP_ILLEGAL;
      }
      अन्यथा अगर(blksize < TFTP_BLKSIZE_MIN) {
        failf(तथ्य, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        वापस CURLE_TFTP_ILLEGAL;
      }
      अन्यथा अगर(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(तथ्य, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        वापस CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (अचिन्हित पूर्णांक)blksize;
      infof(तथ्य, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    अन्यथा अगर((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      अगर(!तथ्य->state.upload &&
         !curlx_str_संख्या(&मूल्य, &tsize, CURL_OFF_T_MAX)) {
        अगर(!tsize) {
          failf(तथ्य, "invalid tsize -:%s:- value in OACK packet", मूल्य);
          वापस CURLE_TFTP_ILLEGAL;
        }
        infof(तथ्य, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_Set_Download_आकार(तथ्य, tsize);
      }
    }
  }

  वापस CURLE_OK;
}

अटल CURLcode tftp_option_add(काष्ठा tftp_conn *state, माप_प्रकार *csize,
                                माप_प्रकार सूचकांक, स्थिर अक्षर *option)
{
  अक्षर *buf = (अक्षर *)&state->spacket.तथ्य[सूचकांक];
  माप_प्रकार oplen = म_माप(option);
  माप_प्रकार blen;
  अगर((state->blksize <= सूचकांक) ||
     (oplen + 1) > (माप_प्रकार)(state->blksize - सूचकांक))
    वापस CURLE_TFTP_ILLEGAL;
  blen = state->blksize - सूचकांक;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  वापस CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#घोषणा अग्र_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
अटल CURLcode tftp_tx(काष्ठा tftp_conn *state, tftp_event_t event)
{
  काष्ठा Curl_easy *तथ्य = state->तथ्य;
  ssize_t sbytes;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SingleRequest *k = &तथ्य->req;
  माप_प्रकार cb; /* Bytes currently read */
  अक्षर बफर[STRERROR_LEN];
  अक्षर *bufptr;
  bool eos;

  अगर(!तथ्य->state.upload)
    वापस CURLE_TFTP_ILLEGAL;

  चयन(event) {

  हाल TFTP_EVENT_ACK:
  हाल TFTP_EVENT_OACK:
    अगर(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      पूर्णांक rblock = getrpacketblock(&state->rpacket);

      अगर(rblock != state->block &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->block == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(तथ्य, "Received ACK for block %d, expecting %d",
              rblock, state->block);
        state->retries++;
        /* Bail out if over the maximum */
        अगर(state->retries > state->retry_max) {
          failf(तथ्य, "tftp_tx: giving up waiting for block %d ack",
                state->block);
          परिणाम = CURLE_SEND_ERROR;
        }
        अन्यथा {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (काष्ठा sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          अगर(sbytes < 0) {
            failf(तथ्य, "%s",
                  curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
            परिणाम = CURLE_SEND_ERROR;
          }
        }

        वापस परिणाम;
      }
      /* This is the expected packet. Reset the counters and send the next
         block */
      state->rx_time = समय(शून्य);
      state->block++;
    }
    अन्यथा
      state->block = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_तथ्य);
    setpacketblock(&state->spacket, state->block);
    अगर(state->block > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      वापस CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (अक्षर *)state->spacket.तथ्य + 4;
    करो {
      परिणाम = Curl_client_read(तथ्य, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      अगर(परिणाम)
        वापस परिणाम;
      state->sbytes += cb;
      bufptr += cb;
    } जबतक(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (काष्ठा sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    अगर(sbytes < 0) {
      failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
      वापस CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(तथ्य, state->sbytes);
    अवरोध;

  हाल TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(तथ्य, "Timeout waiting for block %d ACK. "
          "Retries = %d", अग्र_BLOCKNUM(state->block), state->retries);
    /* Decide if we have had enough */
    अगर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (काष्ठा sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      अगर(sbytes < 0) {
        failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
        वापस CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_Set_Upload_गणक(तथ्य, k->writebytecount);
    }
    अवरोध;

  हाल TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (व्योम)sendto(state->sockfd, (व्योम *)state->spacket.तथ्य, 4, SEND_4TH_ARG,
                 (काष्ठा sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    अवरोध;

  शेष:
    failf(तथ्य, "tftp_tx: internal error, event: %d", (पूर्णांक)event);
    अवरोध;
  }

  वापस परिणाम;
}

अटल CURLcode tftp_connect_for_tx(काष्ठा tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode परिणाम;

  infof(state->तथ्य, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  परिणाम = tftp_set_timeouts(state);
  अगर(परिणाम)
    वापस परिणाम;
  वापस tftp_tx(state, event);
}

/**********************************************************
 *
 * tftp_rx
 *
 * Event handler for the RX state
 *
 **********************************************************/
अटल CURLcode tftp_rx(काष्ठा tftp_conn *state, tftp_event_t event)
{
  ssize_t sbytes;
  पूर्णांक rblock;
  काष्ठा Curl_easy *तथ्य = state->तथ्य;
  अक्षर बफर[STRERROR_LEN];

  चयन(event) {

  हाल TFTP_EVENT_तथ्य:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    अगर(अग्र_BLOCKNUM(state->block) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    अन्यथा अगर(state->block == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(तथ्य, "Received last DATA packet block %d again.", rblock);
    }
    अन्यथा {
      /* totally unexpected, log it */
      infof(तथ्य,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, अग्र_BLOCKNUM(state->block));
      अवरोध;
    }

    /* ACK this block. */
    state->block = (अचिन्हित लघु)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                    4, SEND_4TH_ARG,
                    (काष्ठा sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    अगर(sbytes < 0) {
      failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
      वापस CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    अगर(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = समय(शून्य);
    अवरोध;

  हाल TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->block = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->block);
    sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                    4, SEND_4TH_ARG,
                    (काष्ठा sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    अगर(sbytes < 0) {
      failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
      वापस CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = समय(शून्य);
    अवरोध;

  हाल TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(तथ्य,
          "Timeout waiting for block %d ACK. Retries = %d",
          अग्र_BLOCKNUM(state->block), state->retries);
    अगर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                      4, SEND_4TH_ARG,
                      (काष्ठा sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      अगर(sbytes < 0) {
        failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
        वापस CURLE_SEND_ERROR;
      }
    }
    अवरोध;

  हाल TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->block);
    (व्योम)sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                 4, SEND_4TH_ARG,
                 (काष्ठा sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    अवरोध;

  शेष:
    failf(तथ्य, "%s", "tftp_rx: internal error");
    वापस CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  वापस CURLE_OK;
}

अटल CURLcode tftp_connect_for_rx(काष्ठा tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode परिणाम;

  infof(state->तथ्य, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  परिणाम = tftp_set_timeouts(state);
  अगर(परिणाम)
    वापस परिणाम;
  वापस tftp_rx(state, event);
}

अटल CURLcode tftp_send_first(काष्ठा tftp_conn *state,
                                tftp_event_t event)
{
  माप_प्रकार sbytes;
  ssize_t senddata;
  स्थिर अक्षर *mode = "octet";
  अक्षर *filename;
  काष्ठा Curl_easy *तथ्य = state->तथ्य;
  स्थिर काष्ठा Curl_sockaddr_ex *remote_addr = शून्य;
  CURLcode परिणाम = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  अगर(तथ्य->state.prefer_ascii)
    mode = "netascii";

  चयन(event) {

  हाल TFTP_EVENT_INIT:    /* Send the first packet out */
  हाल TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    अगर(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      वापस परिणाम;
    }

    अगर(तथ्य->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      अगर(तथ्य->state.infilesize != -1)
        Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
    }
    अन्यथा {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    अगर(!state->तथ्य->state.up.path[1]) {
      failf(तथ्य, "Missing filename");
      वापस CURLE_TFTP_ILLEGAL;
    }
    परिणाम = Curl_urldecode(&state->तथ्य->state.up.path[1], 0,
                            &filename, शून्य, REJECT_ZERO);
    अगर(परिणाम)
      वापस परिणाम;

    अगर(म_माप(filename) + म_माप(mode) + 4 > state->blksize) {
      failf(तथ्य, "TFTP filename too long");
      curlx_free(filename);
      वापस CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((अक्षर *)state->spacket.तथ्य + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', mode, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    अगर(!तथ्य->set.tftp_no_options) {
      अक्षर buf[64];
      /* add tsize option */
      curl_msnprintf(buf, माप(buf), "%" FMT_OFF_T,
                     तथ्य->state.upload && (तथ्य->state.infilesize != -1) ?
                     तथ्य->state.infilesize : 0);

      परिणाम = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      अगर(परिणाम == CURLE_OK)
        परिणाम = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, माप(buf), "%u", state->requested_blksize);
      अगर(परिणाम == CURLE_OK)
        परिणाम = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      अगर(परिणाम == CURLE_OK)
        परिणाम = tftp_option_add(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, माप(buf), "%d", state->retry_time);
      अगर(परिणाम == CURLE_OK)
        परिणाम = tftp_option_add(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      अगर(परिणाम == CURLE_OK)
        परिणाम = tftp_option_add(state, &sbytes, sbytes, buf);

      अगर(परिणाम != CURLE_OK) {
        failf(तथ्य, "TFTP buffer too small for options");
        वापस CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#अगर_घोषित __AMIGA__
#घोषणा CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#अन्यथा
#घोषणा CURL_SENDTO_ARG5(x) (x)
#पूर्ण_अगर
    remote_addr = Curl_conn_get_remote_addr(तथ्य, FIRSTSOCKET);
    अगर(!remote_addr)
      वापस CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (व्योम *)state->spacket.तथ्य,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    अगर(senddata != (ssize_t)sbytes) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "%s", curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
      वापस CURLE_SEND_ERROR;
    }
    अवरोध;

  हाल TFTP_EVENT_OACK:
    अगर(तथ्य->state.upload) {
      परिणाम = tftp_connect_for_tx(state, event);
    }
    अन्यथा {
      परिणाम = tftp_connect_for_rx(state, event);
    }
    अवरोध;

  हाल TFTP_EVENT_ACK: /* Connected for transmit */
    अगर(!तथ्य->state.upload)
      वापस CURLE_TFTP_ILLEGAL;
    परिणाम = tftp_connect_for_tx(state, event);
    अवरोध;

  हाल TFTP_EVENT_तथ्य: /* Connected for receive */
    अगर(तथ्य->state.upload)
      वापस CURLE_TFTP_ILLEGAL;
    परिणाम = tftp_connect_for_rx(state, event);
    अवरोध;

  हाल TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    अवरोध;

  शेष:
    failf(state->तथ्य, "tftp_send_first: internal error");
    वापस CURLE_TFTP_ILLEGAL;
  }

  वापस परिणाम;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
अटल CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(error != TFTP_ERR_NONE) {
    चयन(error) {
    हाल TFTP_ERR_NOTFOUND:
      परिणाम = CURLE_TFTP_NOTFOUND;
      अवरोध;
    हाल TFTP_ERR_PERM:
      परिणाम = CURLE_TFTP_PERM;
      अवरोध;
    हाल TFTP_ERR_DISKFULL:
      परिणाम = CURLE_REMOTE_DISK_FULL;
      अवरोध;
    हाल TFTP_ERR_UNDEF:
    हाल TFTP_ERR_ILLEGAL:
      परिणाम = CURLE_TFTP_ILLEGAL;
      अवरोध;
    हाल TFTP_ERR_UNKNOWNID:
      परिणाम = CURLE_TFTP_UNKNOWNID;
      अवरोध;
    हाल TFTP_ERR_EXISTS:
      परिणाम = CURLE_REMOTE_FILE_EXISTS;
      अवरोध;
    हाल TFTP_ERR_NOSUCHUSER:
      परिणाम = CURLE_TFTP_NOSUCHUSER;
      अवरोध;
    हाल TFTP_ERR_TIMEOUT:
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    हाल TFTP_ERR_NORESPONSE:
      परिणाम = CURLE_COULDNT_CONNECT;
      अवरोध;
    शेष:
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      अवरोध;
    }
  }
  अन्यथा
    परिणाम = CURLE_OK;

  वापस परिणाम;
}

/**********************************************************
 *
 * tftp_state_machine
 *
 * The tftp state machine event dispatcher
 *
 **********************************************************/
अटल CURLcode tftp_state_machine(काष्ठा tftp_conn *state,
                                   tftp_event_t event)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा Curl_easy *तथ्य = state->तथ्य;

  चयन(state->state) {
  हाल TFTP_STATE_START:
    DEBUGF(infof(तथ्य, "TFTP_STATE_START"));
    परिणाम = tftp_send_first(state, event);
    अवरोध;
  हाल TFTP_STATE_RX:
    DEBUGF(infof(तथ्य, "TFTP_STATE_RX"));
    परिणाम = tftp_rx(state, event);
    अवरोध;
  हाल TFTP_STATE_TX:
    DEBUGF(infof(तथ्य, "TFTP_STATE_TX"));
    परिणाम = tftp_tx(state, event);
    अवरोध;
  हाल TFTP_STATE_FIN:
    infof(तथ्य, "%s", "TFTP finished");
    अवरोध;
  शेष:
    DEBUGF(infof(तथ्य, "STATE: %d", (पूर्णांक)state->state));
    failf(तथ्य, "%s", "Internal state machine error");
    परिणाम = CURLE_TFTP_ILLEGAL;
    अवरोध;
  }

  वापस परिणाम;
}

अटल व्योम tftp_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा tftp_conn *state = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_safefree(state->rpacket.तथ्य);
  curlx_safefree(state->spacket.तथ्य);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
अटल CURLcode tftp_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा tftp_conn *state;
  पूर्णांक blksize;
  पूर्णांक need_blksize;
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर काष्ठा Curl_sockaddr_ex *remote_addr = शून्य;
  CURLcode परिणाम;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, माप(*state));
  अगर(!state ||
     Curl_conn_meta_set(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  अगर(तथ्य->set.tftp_blksize)
    /* range checked when set */
    blksize = (पूर्णांक)तथ्य->set.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  अगर(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  अगर(!state->rpacket.तथ्य) {
    state->rpacket.तथ्य = curlx_calloc(1, need_blksize + 2 + 2);

    अगर(!state->rpacket.तथ्य)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(!state->spacket.तथ्य) {
    state->spacket.तथ्य = curlx_calloc(1, need_blksize + 2 + 2);

    अगर(!state->spacket.तथ्य)
      वापस CURLE_OUT_OF_MEMORY;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->तथ्य = तथ्य;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_START;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(तथ्य, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  अगर(!remote_addr)
    वापस CURLE_FAILED_INIT;

  ((काष्ठा sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  परिणाम = tftp_set_timeouts(state);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!conn->bits.bound) {
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
    पूर्णांक rc = bind(state->sockfd, (काष्ठा sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    अगर(rc) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
      वापस CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  *done = TRUE;

  वापस CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
अटल CURLcode tftp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (व्योम)status;
  (व्योम)premature;

  अगर(Curl_pgrsDone(तथ्य, Curl_pgrs_now(तथ्य)))
    वापस CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  अगर(state)
    परिणाम = tftp_translate_code(state->error);

  वापस परिणाम;
}

अटल CURLcode tftp_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  वापस Curl_pollset_add_in(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
अटल CURLcode tftp_receive_packet(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा tftp_conn *state)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = माप(remote_addr);

  /* Receive the packet */
  state->rbytes = (पूर्णांक)recvfrom(state->sockfd,
                                (व्योम *)state->rpacket.तथ्य,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (काष्ठा sockaddr *)&remote_addr,
                                &fromlen);
  अगर((state->rbytes >= 0) && fromlen) {
    अगर(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      अगर((state->remote_addrlen != fromlen) ||
         स_भेद(&remote_addr, &state->remote_addr, fromlen)) {
        failf(तथ्य, "Data received from another address");
        वापस CURLE_RECV_ERROR;
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
  अगर(state->rbytes < 4) {
    failf(तथ्य, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  अन्यथा {
    /* The event is given by the TFTP packet time */
    अचिन्हित लघु event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    चयन(state->event) {
    हाल TFTP_EVENT_तथ्य:
      अगर(तथ्य->state.upload)
        वापस CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      अगर(state->rbytes > 4 &&
         (अग्र_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                   (स्थिर अक्षर *)state->rpacket.तथ्य + 4,
                                   state->rbytes - 4);
        अगर(परिणाम) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          वापस परिणाम;
        }
      }
      अवरोध;
    हाल TFTP_EVENT_ERROR: {
      अचिन्हित लघु error = getrpacketblock(&state->rpacket);
      स्थिर अक्षर *str = (स्थिर अक्षर *)state->rpacket.तथ्य + 4;
      माप_प्रकार strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      अगर(tftp_strnlen(str, strn) < strn)
        infof(तथ्य, "TFTP error: %s", str);
      अवरोध;
    }
    हाल TFTP_EVENT_ACK:
      अवरोध;
    हाल TFTP_EVENT_OACK:
      परिणाम = tftp_parse_option_ack(state,
                                     (स्थिर अक्षर *)state->rpacket.तथ्य + 2,
                                     state->rbytes-2);
      अगर(परिणाम)
        वापस परिणाम;
      अवरोध;
    हाल TFTP_EVENT_RRQ:
    हाल TFTP_EVENT_WRQ:
    शेष:
      failf(तथ्य, "%s", "Internal error: Unexpected packet");
      अवरोध;
    }

    /* Update the progress meter */
    परिणाम = Curl_pgrsUpdate(तथ्य);
    अगर(परिणाम) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      वापस परिणाम;
    }
  }
  वापस परिणाम;
}

/**********************************************************
 *
 * tftp_state_timeout
 *
 * Check if timeouts have been reached
 *
 **********************************************************/
अटल timediff_t tftp_state_timeout(काष्ठा tftp_conn *state,
                                     tftp_event_t *event)
{
  समय_प्रकार current;
  timediff_t timeout_ms;

  अगर(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->तथ्य);
  अगर(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    वापस timeout_ms;
  }
  current = समय(शून्य);
  अगर(current > state->rx_time + state->retry_time) {
    अगर(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = समय(शून्य); /* update even though we received nothing */
  }

  वापस timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
अटल CURLcode tftp_multi_statemach(काष्ठा Curl_easy *तथ्य, bool *done)
{
  tftp_event_t event;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *done = FALSE;
  अगर(!state)
    वापस CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  अगर(timeout_ms < 0) {
    failf(तथ्य, "TFTP response timeout");
    वापस CURLE_OPERATION_TIMEDOUT;
  }
  अगर(event != TFTP_EVENT_NONE) {
    परिणाम = tftp_state_machine(state, event);
    अगर(परिणाम)
      वापस परिणाम;
    *done = (state->state == TFTP_STATE_FIN);
    अगर(*done)
      /* Tell curl we are done */
      Curl_xfer_setup_nop(तथ्य);
  }
  अन्यथा {
    /* no timeouts to handle, check our socket */
    पूर्णांक rc = SOCKET_READABLE(state->sockfd, 0);

    अगर(rc == -1) {
      /* bail out */
      पूर्णांक sockerr = SOCKERRNO;
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "%s", curlx_strerror(sockerr, बफर, माप(बफर)));
      state->event = TFTP_EVENT_ERROR;
    }
    अन्यथा अगर(rc) {
      परिणाम = tftp_receive_packet(तथ्य, state);
      अगर(परिणाम)
        वापस परिणाम;
      परिणाम = tftp_state_machine(state, state->event);
      अगर(परिणाम)
        वापस परिणाम;
      *done = (state->state == TFTP_STATE_FIN);
      अगर(*done)
        /* Tell curl we are done */
        Curl_xfer_setup_nop(तथ्य);
    }
    /* if rc == 0, then select() timed out */
  }

  वापस परिणाम;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
अटल CURLcode tftp_doing(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  CURLcode परिणाम;
  परिणाम = tftp_multi_statemach(तथ्य, dophase_done);

  अगर(*dophase_done) {
    DEBUGF(infof(तथ्य, "DO phase is complete"));
  }
  अन्यथा अगर(!परिणाम) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    परिणाम = Curl_pgrsCheck(तथ्य);
  }
  वापस परिणाम;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
अटल CURLcode tftp_perform(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_done = FALSE;
  अगर(!state)
    वापस CURLE_FAILED_INIT;

  परिणाम = tftp_state_machine(state, TFTP_EVENT_INIT);

  अगर((state->state == TFTP_STATE_FIN) || परिणाम)
    वापस परिणाम;

  परिणाम = tftp_multi_statemach(तथ्य, dophase_done);

  अगर(*dophase_done)
    DEBUGF(infof(तथ्य, "DO phase is complete"));

  वापस परिणाम;
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

अटल CURLcode tftp_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode परिणाम;

  *done = FALSE;

  अगर(!state) {
    परिणाम = tftp_connect(तथ्य, done);
    अगर(परिणाम)
      वापस परिणाम;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    अगर(!state)
      वापस CURLE_TFTP_ILLEGAL;
  }

  परिणाम = tftp_perform(तथ्य, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  अगर(!परिणाम)
    /* If we have encountered an internal tftp error, translate it. */
    परिणाम = tftp_translate_code(state->error);

  वापस परिणाम;
}

अटल CURLcode tftp_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  अक्षर *path = तथ्य->state.up.path;
  माप_प्रकार len = म_माप(path);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  अगर((len >= 14) && !स_भेद(&path[len - 14], ";mode=netascii", 14)) {
    तथ्य->state.prefer_ascii = TRUE;
    path[len - 14] = 0; /* cut it there */
  }
  अन्यथा अगर((len >= 11) && !स_भेद(&path[len - 11], ";mode=octet", 11)) {
    तथ्य->state.prefer_ascii = FALSE;
    path[len - 11] = 0; /* cut it there */
  }

  वापस CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_tftp = {
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

#पूर्ण_अगर
