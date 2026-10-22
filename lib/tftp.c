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

#यदि_अपरिभाषितम् CURL_DISABLE_TFTP

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

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
#परिभाषय TFTP_BLKSIZE_DEFAULT 512
#परिभाषय TFTP_OPTION_BLKSIZE  "blksize"

/* from RFC2349: */
#परिभाषय TFTP_OPTION_TSIZE    "tsize"
#परिभाषय TFTP_OPTION_INTERVAL "timeout"

प्रकारनिर्देश गणना {
  TFTP_बहुलकम्_NETASCII = 0,
  TFTP_बहुलकम्_OCTET
} tftp_बहुलकम्_t;

प्रकारनिर्देश गणना {
  TFTP_STATE_START = 0,
  TFTP_STATE_RX,
  TFTP_STATE_TX,
  TFTP_STATE_FIN
} tftp_state_t;

प्रकारनिर्देश गणना {
  TFTP_EVENT_NONE = -1,
  TFTP_EVENT_INIT = 0,
  TFTP_EVENT_RRQ = 1,
  TFTP_EVENT_WRQ = 2,
  TFTP_EVENT_दत्त = 3,
  TFTP_EVENT_ACK = 4,
  TFTP_EVENT_ERROR = 5,
  TFTP_EVENT_OACK = 6,
  TFTP_EVENT_TIMEOUT
} tftp_event_t;

प्रकारनिर्देश गणना {
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
  अचिह्नित वर्ण *दत्त;
};

/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_TFTP_CONN   "meta:proto:tftp:conn"

संरचना tftp_conn {
  संरचना Curl_sockaddr_storage local_addr;
  संरचना Curl_sockaddr_storage remote_addr;
  संरचना tftp_packet rpacket;
  संरचना tftp_packet spacket;
  tftp_state_t    state;
  tftp_बहुलकम्_t     बहुलकम्;
  tftp_error_t    error;
  tftp_event_t    event;
  संरचना Curl_easy *दत्त;
  curl_socket_t   sockfd;
  पूर्णाङ्क             retries;
  पूर्णाङ्क             retry_time;
  पूर्णाङ्क             retry_max;
  time_t          rx_time;
  curl_socklen_t  remote_addrlen;
  पूर्णाङ्क             rbytes;
  size_t          sbytes;
  अचिह्नित पूर्णाङ्क    blksize;
  अचिह्नित पूर्णाङ्क    requested_blksize;
  अचिह्नित ह्रस्व  खण्डम्;
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
स्थिर CURLcode tftp_समुच्चयः_timeouts(संरचना tftp_conn *state)
{
  time_t timeout;
  timediff_t timeout_ms;

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft_ms(state->दत्त);

  यदि(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->दत्त, "Connection time-out");
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }

  /* Set per-block timeout to total */
  यदि((timeout_ms > 0) && (timeout_ms < 3600000))
    /* do the calculation only if the timeout is "reasonable" */
    timeout = (time_t)(timeout_ms + 500) / 1000;
  अन्यथा
    timeout = 15;

  /* Average reposting an ACK after 5 seconds */
  state->retry_max = (पूर्णाङ्क)timeout / 5;

  /* Bound the total number */
  यदि(state->retry_max < 3)
    state->retry_max = 3;

  यदि(state->retry_max > 50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (पूर्णाङ्क)(timeout / state->retry_max);
  यदि(state->retry_time < 1)
    state->retry_time = 1;

  infof(state->दत्त,
        "set timeouts for state %d; Total %" FMT_OFF_T ", retry %d maxtry %d",
        (पूर्णाङ्क)state->state, timeout_ms, state->retry_time, state->retry_max);

  /* init RX time */
  state->rx_time = time(रिक्त);

  प्रत्यागम CURLE_OK;
}

/**********************************************************
 *
 * tftp_set_send_first
 *
 * Event handler for the START state
 *
 **********************************************************/

स्थिर शून्य setpacketevent(संरचना tftp_packet *packet, अचिह्नित ह्रस्व अङ्कः)
{
  packet->दत्त[0] = (अचिह्नित वर्ण)(अङ्कः >> 8);
  packet->दत्त[1] = (अचिह्नित वर्ण)(अङ्कः & 0xff);
}

स्थिर शून्य setpacketblock(संरचना tftp_packet *packet, अचिह्नित ह्रस्व अङ्कः)
{
  packet->दत्त[2] = (अचिह्नित वर्ण)(अङ्कः >> 8);
  packet->दत्त[3] = (अचिह्नित वर्ण)(अङ्कः & 0xff);
}

स्थिर अचिह्नित ह्रस्व getrpacketevent(ध्रुव संरचना tftp_packet *packet)
{
  प्रत्यागम (अचिह्नित ह्रस्व)((packet->दत्त[0] << 8) | packet->दत्त[1]);
}

स्थिर अचिह्नित ह्रस्व getrpacketblock(ध्रुव संरचना tftp_packet *packet)
{
  प्रत्यागम (अचिह्नित ह्रस्व)((packet->दत्त[2] << 8) | packet->दत्त[3]);
}

स्थिर size_t tftp_strnlen(ध्रुव वर्ण *string, size_t maxlen)
{
  ध्रुव वर्ण *अन्तः = memchr(string, '\0', maxlen);
  प्रत्यागम अन्तः ? (size_t)(अन्तः - string) : maxlen;
}

स्थिर ध्रुव वर्ण *tftp_option_get(ध्रुव वर्ण *buf, size_t len,
                                   ध्रुव वर्ण **option, ध्रुव वर्ण **मानम्)
{
  size_t loc;

  loc = tftp_strnlen(buf, len);
  loc++; /* NULL term */

  यदि(loc >= len)
    प्रत्यागम रिक्त;
  *option = buf;

  loc += tftp_strnlen(buf + loc, len - loc);
  loc++; /* NULL term */

  यदि(loc > len)
    प्रत्यागम रिक्त;
  *मानम् = &buf[वाक्यदैर्घ्यम्(*option) + 1];

  प्रत्यागम &buf[loc];
}

स्थिर CURLcode tftp_parse_option_ack(संरचना tftp_conn *state,
                                      ध्रुव वर्ण *ptr, पूर्णाङ्क len)
{
  ध्रुव वर्ण *tmp = ptr;
  संरचना Curl_easy *दत्त = state->दत्त;

  /* if OACK does not contain blksize option, the default (512) must be used */
  state->blksize = TFTP_BLKSIZE_DEFAULT;

  यावत्(tmp < ptr + len) {
    ध्रुव वर्ण *option, *मानम्;
    size_t olen;

    tmp = tftp_option_get(tmp, ptr + len - tmp, &option, &मानम्);
    यदि(!tmp) {
      failf(दत्त, "Malformed ACK packet, rejecting");
      प्रत्यागम CURLE_TFTP_ILLEGAL;
    }
    olen = वाक्यदैर्घ्यम्(option);

    infof(दत्त, "got option=(%s) value=(%s)", option, मानम्);

    यदि((CURL_CSTRLEN(TFTP_OPTION_BLKSIZE) == olen) &&
       checkprefix(TFTP_OPTION_BLKSIZE, option)) {
      curl_off_t blksize;
      यदि(curlx_str_सङ्ख्या(&मानम्, &blksize, TFTP_BLKSIZE_MAX)) {
        failf(दत्त, "%s (%d)", "blksize is larger than max supported",
              TFTP_BLKSIZE_MAX);
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      }
      यदि(!blksize) {
        failf(दत्त, "invalid blocksize value in OACK packet");
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      }
      अन्यथा यदि(blksize < TFTP_BLKSIZE_MIN) {
        failf(दत्त, "%s (%d)", "blksize is smaller than min supported",
              TFTP_BLKSIZE_MIN);
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      }
      अन्यथा यदि(blksize > state->requested_blksize) {
        /* could realloc pkt buffers here, but the spec does not call out
         * support for the server requesting a bigger blksize than the client
         * requests */
        failf(दत्त, "server requested blksize larger than allocated (%"
              CURL_FORMAT_CURL_OFF_T ")", blksize);
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      }

      state->blksize = (अचिह्नित पूर्णाङ्क)blksize;
      infof(दत्त, "blksize parsed from OACK (%u) requested (%u)",
            state->blksize, state->requested_blksize);
    }
    अन्यथा यदि((CURL_CSTRLEN(TFTP_OPTION_TSIZE) == olen) &&
            checkprefix(TFTP_OPTION_TSIZE, option)) {
      curl_off_t tsize = 0;
      /* tsize should be ignored on upload: Who cares about the size of the
         remote file? */
      यदि(!दत्त->state.upload &&
         !curlx_str_सङ्ख्या(&मानम्, &tsize, CURL_OFF_T_MAX)) {
        यदि(!tsize) {
          failf(दत्त, "invalid tsize -:%s:- value in OACK packet", मानम्);
          प्रत्यागम CURLE_TFTP_ILLEGAL;
        }
        infof(दत्त, "tsize parsed from OACK (%" CURL_FORMAT_CURL_OFF_T ")",
              tsize);
        Curl_pgrs_समुच्चयः_Download_आकार(दत्त, tsize);
      }
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode tftp_option_सङ्कलय(संरचना tftp_conn *state, size_t *csize,
                                size_t सूचकांक, ध्रुव वर्ण *option)
{
  वर्ण *buf = (वर्ण *)&state->spacket.दत्त[सूचकांक];
  size_t oplen = वाक्यदैर्घ्यम्(option);
  size_t blen;
  यदि((state->blksize <= सूचकांक) ||
     (oplen + 1) > (size_t)(state->blksize - सूचकांक))
    प्रत्यागम CURLE_TFTP_ILLEGAL;
  blen = state->blksize - सूचकांक;
  curlx_strcopy(buf, blen, option, oplen);
  *csize += oplen + 1;
  प्रत्यागम CURLE_OK;
}

/* the next blocknum is x + 1 but it needs to wrap at an unsigned 16-bit
   boundary */
#परिभाषय अग्र_BLOCKNUM(x) (((x) + 1) & 0xffff)

/**********************************************************
 *
 * tftp_tx
 *
 * Event handler for the TX state
 *
 **********************************************************/
स्थिर CURLcode tftp_tx(संरचना tftp_conn *state, tftp_event_t event)
{
  संरचना Curl_easy *दत्त = state->दत्त;
  ssize_t sbytes;
  CURLcode परिणाम = CURLE_OK;
  संरचना SingleRequest *k = &दत्त->req;
  size_t cb; /* Bytes currently read */
  वर्ण बफर[STRERROR_LEN];
  वर्ण *bufptr;
  bool eos;

  यदि(!दत्त->state.upload)
    प्रत्यागम CURLE_TFTP_ILLEGAL;

  विकल्प(event) {

  पक्ष TFTP_EVENT_ACK:
  पक्ष TFTP_EVENT_OACK:
    यदि(event == TFTP_EVENT_ACK) {
      /* Ack the packet */
      पूर्णाङ्क rblock = getrpacketblock(&state->rpacket);

      यदि(rblock != state->खण्डम् &&
         /* There is a bug in tftpd-hpa that causes it to send us an ACK for
          * 65535 when the block number wraps to 0. To handle it, when we are
          * expecting 0, also accept 65535. See
          * https://www.syslinux.org/archives/2010-September/015612.html
          * */
         !(state->खण्डम् == 0 && rblock == 65535)) {
        /* This is not the expected block. Log it and up the retry counter */
        infof(दत्त, "Received ACK for block %d, expecting %d",
              rblock, state->खण्डम्);
        state->retries++;
        /* Bail out if over the maximum */
        यदि(state->retries > state->retry_max) {
          failf(दत्त, "tftp_tx: giving up waiting for block %d ack",
                state->खण्डम्);
          परिणाम = CURLE_SEND_ERROR;
        }
        अन्यथा {
          /* Re-send the data packet */
          sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                          4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                          (संरचना sockaddr *)&state->remote_addr,
                          state->remote_addrlen);
          /* Check all sbytes were sent */
          यदि(sbytes < 0) {
            failf(दत्त, "%s",
                  curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
            परिणाम = CURLE_SEND_ERROR;
          }
        }

        प्रत्यागम परिणाम;
      }
      /* This is the expected packet. Reset the counters and send the next
खण्डम्*/
      state->rx_time = time(रिक्त);
      state->खण्डम्++;
    }
    अन्यथा
      state->खण्डम् = 1; /* first data block is 1 when using OACK */

    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_दत्त);
    setpacketblock(&state->spacket, state->खण्डम्);
    यदि(state->खण्डम् > 1 && state->sbytes < state->blksize) {
      state->state = TFTP_STATE_FIN;
      प्रत्यागम CURLE_OK;
    }

    /* TFTP considers data block size < 512 bytes as an end of session, so
     * in some cases we must wait for additional data to build full (512 bytes)
     * data block.
     * */
    state->sbytes = 0;
    bufptr = (वर्ण *)state->spacket.दत्त + 4;
    कुरु {
      परिणाम = Curl_client_read(दत्त, bufptr, state->blksize - state->sbytes,
                                &cb, &eos);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      state->sbytes += cb;
      bufptr += cb;
    } यावत्(state->sbytes < state->blksize && cb);

    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                    4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    /* Check all sbytes were sent */
    यदि(sbytes < 0) {
      failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SEND_ERROR;
    }
    /* Update the progress meter */
    k->writebytecount += state->sbytes;
    Curl_pgrs_upload_inc(दत्त, state->sbytes);
    भङ्ग;

  पक्ष TFTP_EVENT_TIMEOUT:
    /* Increment the retry counter and log the timeout */
    state->retries++;
    infof(दत्त, "Timeout waiting for block %d ACK. "
          "Retries = %d", अग्र_BLOCKNUM(state->खण्डम्), state->retries);
    /* Decide if we have had enough */
    यदि(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Re-send the data packet */
      sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                      4 + (SEND_TYPE_ARG3)state->sbytes, SEND_4TH_ARG,
                      (संरचना sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      /* Check all sbytes were sent */
      यदि(sbytes < 0) {
        failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
        प्रत्यागम CURLE_SEND_ERROR;
      }
      /* since this was a re-send, we remain at the still byte position */
      Curl_pgrs_समुच्चयः_Upload_गणक(दत्त, k->writebytecount);
    }
    भङ्ग;

  पक्ष TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->खण्डम्);
    (शून्य)sendto(state->sockfd, (शून्य *)state->spacket.दत्त, 4, SEND_4TH_ARG,
                 (संरचना sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    भङ्ग;

  स्वभाव:
    failf(दत्त, "tftp_tx: internal error, event: %d", (पूर्णाङ्क)event);
    भङ्ग;
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode tftp_connect_for_tx(संरचना tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode परिणाम;

  infof(state->दत्त, "%s", "Connected for transmit");

  state->state = TFTP_STATE_TX;
  परिणाम = tftp_समुच्चयः_timeouts(state);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  प्रत्यागम tftp_tx(state, event);
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
  पूर्णाङ्क rblock;
  संरचना Curl_easy *दत्त = state->दत्त;
  वर्ण बफर[STRERROR_LEN];

  विकल्प(event) {

  पक्ष TFTP_EVENT_दत्त:
    /* Is this the block we expect? */
    rblock = getrpacketblock(&state->rpacket);
    यदि(अग्र_BLOCKNUM(state->खण्डम्) == rblock) {
      /* This is the expected block. Reset counters and ACK it. */
      state->retries = 0;
    }
    अन्यथा यदि(state->खण्डम् == rblock) {
      /* This is the last recently received block again. Log it and ACK it
         again. */
      infof(दत्त, "Received last DATA packet block %d again.", rblock);
    }
    अन्यथा {
      /* totally unexpected, log it */
      infof(दत्त,
            "Received unexpected DATA packet block %d, expecting block %d",
            rblock, अग्र_BLOCKNUM(state->खण्डम्));
      भङ्ग;
    }

    /* ACK this block. */
    state->खण्डम् = (अचिह्नित ह्रस्व)rblock;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->खण्डम्);
    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                    4, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    यदि(sbytes < 0) {
      failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SEND_ERROR;
    }

    /* Check if completed (That is, a less than full packet is received) */
    यदि(state->rbytes < (ssize_t)state->blksize + 4) {
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      state->state = TFTP_STATE_RX;
    }
    state->rx_time = time(रिक्त);
    भङ्ग;

  पक्ष TFTP_EVENT_OACK:
    /* ACK option acknowledgement so we can move on to data */
    state->खण्डम् = 0;
    state->retries = 0;
    setpacketevent(&state->spacket, TFTP_EVENT_ACK);
    setpacketblock(&state->spacket, state->खण्डम्);
    sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                    4, SEND_4TH_ARG,
                    (संरचना sockaddr *)&state->remote_addr,
                    state->remote_addrlen);
    यदि(sbytes < 0) {
      failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SEND_ERROR;
    }

    /* we are ready to RX data */
    state->state = TFTP_STATE_RX;
    state->rx_time = time(रिक्त);
    भङ्ग;

  पक्ष TFTP_EVENT_TIMEOUT:
    /* Increment the retry count and fail if over the limit */
    state->retries++;
    infof(दत्त,
          "Timeout waiting for block %d ACK. Retries = %d",
          अग्र_BLOCKNUM(state->खण्डम्), state->retries);
    यदि(state->retries > state->retry_max) {
      state->error = TFTP_ERR_TIMEOUT;
      state->state = TFTP_STATE_FIN;
    }
    अन्यथा {
      /* Resend the previous ACK */
      sbytes = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                      4, SEND_4TH_ARG,
                      (संरचना sockaddr *)&state->remote_addr,
                      state->remote_addrlen);
      यदि(sbytes < 0) {
        failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }
    भङ्ग;

  पक्ष TFTP_EVENT_ERROR:
    setpacketevent(&state->spacket, TFTP_EVENT_ERROR);
    setpacketblock(&state->spacket, state->खण्डम्);
    (शून्य)sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                 4, SEND_4TH_ARG,
                 (संरचना sockaddr *)&state->remote_addr,
                 state->remote_addrlen);
    /* do not bother with the return code, but if the socket is still up we
     * should be a good TFTP client and let the server know we are done */
    state->state = TFTP_STATE_FIN;
    भङ्ग;

  स्वभाव:
    failf(दत्त, "%s", "tftp_rx: internal error");
    प्रत्यागम CURLE_TFTP_ILLEGAL; /* not really the perfect return code for
                                  this */
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode tftp_connect_for_rx(संरचना tftp_conn *state,
                                    tftp_event_t event)
{
  CURLcode परिणाम;

  infof(state->दत्त, "%s", "Connected for receive");

  state->state = TFTP_STATE_RX;
  परिणाम = tftp_समुच्चयः_timeouts(state);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  प्रत्यागम tftp_rx(state, event);
}

स्थिर CURLcode tftp_send_first(संरचना tftp_conn *state,
                                tftp_event_t event)
{
  size_t sbytes;
  ssize_t senddata;
  ध्रुव वर्ण *बहुलकम् = "octet";
  वर्ण *filename;
  संरचना Curl_easy *दत्त = state->दत्त;
  ध्रुव संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  /* Set ASCII mode if -B flag was used */
  यदि(दत्त->state.prefer_ascii)
    बहुलकम् = "netascii";

  विकल्प(event) {

  पक्ष TFTP_EVENT_INIT:    /* Send the first packet out */
  पक्ष TFTP_EVENT_TIMEOUT: /* Resend the first packet out */
    /* Increment the retry counter, quit if over the limit */
    state->retries++;
    यदि(state->retries > state->retry_max) {
      state->error = TFTP_ERR_NORESPONSE;
      state->state = TFTP_STATE_FIN;
      प्रत्यागम परिणाम;
    }

    यदि(दत्त->state.upload) {
      /* If we are uploading, send a WRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_WRQ);
      यदि(दत्त->state.infilesize != -1)
        Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
    }
    अन्यथा {
      /* If we are downloading, send an RRQ */
      setpacketevent(&state->spacket, TFTP_EVENT_RRQ);
    }
    /* As RFC3617 describes the separator slash is not actually part of the
       filename so we skip the always-present first letter of the path
       string. */
    यदि(!state->दत्त->state.ऊर्ध्वम्.मार्गः[1]) {
      failf(दत्त, "Missing filename");
      प्रत्यागम CURLE_TFTP_ILLEGAL;
    }
    परिणाम = Curl_urldecode(&state->दत्त->state.ऊर्ध्वम्.मार्गः[1], 0,
                            &filename, रिक्त, REJECT_ZERO);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि(वाक्यदैर्घ्यम्(filename) + वाक्यदैर्घ्यम्(बहुलकम्) + 4 > state->blksize) {
      failf(दत्त, "TFTP filename too long");
      curlx_free(filename);
      प्रत्यागम CURLE_TFTP_ILLEGAL; /* too long filename field */
    }

    sbytes = 2 +
      curl_msnprintf((वर्ण *)state->spacket.दत्त + 2,
                     state->blksize,
                     "%s%c%s%c", filename, '\0', बहुलकम्, '\0');
    curlx_free(filename);

    /* optional addition of TFTP options */
    यदि(!दत्त->समुच्चयः.tftp_no_options) {
      वर्ण buf[64];
      /* add tsize option */
      curl_msnprintf(buf, परिमाण(buf), "%" FMT_OFF_T,
                     दत्त->state.upload && (दत्त->state.infilesize != -1) ?
                     दत्त->state.infilesize : 0);

      परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, TFTP_OPTION_TSIZE);
      यदि(परिणाम == CURLE_OK)
        परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, buf);

      /* add blksize option */
      curl_msnprintf(buf, परिमाण(buf), "%u", state->requested_blksize);
      यदि(परिणाम == CURLE_OK)
        परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, TFTP_OPTION_BLKSIZE);
      यदि(परिणाम == CURLE_OK)
        परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, buf);

      /* add timeout option */
      curl_msnprintf(buf, परिमाण(buf), "%d", state->retry_time);
      यदि(परिणाम == CURLE_OK)
        परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, TFTP_OPTION_INTERVAL);
      यदि(परिणाम == CURLE_OK)
        परिणाम = tftp_option_सङ्कलय(state, &sbytes, sbytes, buf);

      यदि(परिणाम != CURLE_OK) {
        failf(दत्त, "TFTP buffer too small for options");
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      }
    }

    /* the typecast for the 3rd argument is mostly for systems that do
       not have a size_t argument, like older unixes that want an 'int' */
#यदि_परिभाषितम् __AMIGA__
#परिभाषय CURL_SENDTO_ARG5(x) CURL_UNCONST(x)
#अन्यथा
#परिभाषय CURL_SENDTO_ARG5(x) (x)
#यदि_अन्तः
    remote_addr = Curl_conn_get_remote_addr(दत्त, FIRSTSOCKET);
    यदि(!remote_addr)
      प्रत्यागम CURLE_FAILED_INIT;

    senddata = sendto(state->sockfd, (शून्य *)state->spacket.दत्त,
                      (SEND_TYPE_ARG3)sbytes, 0,
                      CURL_SENDTO_ARG5(&remote_addr->curl_sa_addr),
                      (curl_socklen_t)remote_addr->addrlen);
    यदि(senddata != (ssize_t)sbytes) {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "%s", curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SEND_ERROR;
    }
    भङ्ग;

  पक्ष TFTP_EVENT_OACK:
    यदि(दत्त->state.upload) {
      परिणाम = tftp_connect_for_tx(state, event);
    }
    अन्यथा {
      परिणाम = tftp_connect_for_rx(state, event);
    }
    भङ्ग;

  पक्ष TFTP_EVENT_ACK: /* Connected for transmit */
    यदि(!दत्त->state.upload)
      प्रत्यागम CURLE_TFTP_ILLEGAL;
    परिणाम = tftp_connect_for_tx(state, event);
    भङ्ग;

  पक्ष TFTP_EVENT_दत्त: /* Connected for receive */
    यदि(दत्त->state.upload)
      प्रत्यागम CURLE_TFTP_ILLEGAL;
    परिणाम = tftp_connect_for_rx(state, event);
    भङ्ग;

  पक्ष TFTP_EVENT_ERROR:
    state->state = TFTP_STATE_FIN;
    भङ्ग;

  स्वभाव:
    failf(state->दत्त, "tftp_send_first: internal error");
    प्रत्यागम CURLE_TFTP_ILLEGAL;
  }

  प्रत्यागम परिणाम;
}

/**********************************************************
 *
 * tftp_translate_code
 *
 * Translate internal error codes to CURL error codes
 *
 **********************************************************/
स्थिर CURLcode tftp_translate_संहिता(tftp_error_t error)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(error != TFTP_ERR_NONE) {
    विकल्प(error) {
    पक्ष TFTP_ERR_NOTFOUND:
      परिणाम = CURLE_TFTP_NOTFOUND;
      भङ्ग;
    पक्ष TFTP_ERR_PERM:
      परिणाम = CURLE_TFTP_PERM;
      भङ्ग;
    पक्ष TFTP_ERR_DISKFULL:
      परिणाम = CURLE_REMOTE_DISK_पूर्णम्;
      भङ्ग;
    पक्ष TFTP_ERR_UNDEF:
    पक्ष TFTP_ERR_ILLEGAL:
      परिणाम = CURLE_TFTP_ILLEGAL;
      भङ्ग;
    पक्ष TFTP_ERR_UNKNOWNID:
      परिणाम = CURLE_TFTP_UNKNOWNID;
      भङ्ग;
    पक्ष TFTP_ERR_EXISTS:
      परिणाम = CURLE_REMOTE_FILE_EXISTS;
      भङ्ग;
    पक्ष TFTP_ERR_NOSUCHUSER:
      परिणाम = CURLE_TFTP_NOSUCHUSER;
      भङ्ग;
    पक्ष TFTP_ERR_TIMEOUT:
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    पक्ष TFTP_ERR_NORESPONSE:
      परिणाम = CURLE_COULDNT_CONNECT;
      भङ्ग;
    स्वभाव:
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      भङ्ग;
    }
  }
  अन्यथा
    परिणाम = CURLE_OK;

  प्रत्यागम परिणाम;
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
  CURLcode परिणाम = CURLE_OK;
  संरचना Curl_easy *दत्त = state->दत्त;

  विकल्प(state->state) {
  पक्ष TFTP_STATE_START:
    DEBUGF(infof(दत्त, "TFTP_STATE_START"));
    परिणाम = tftp_send_first(state, event);
    भङ्ग;
  पक्ष TFTP_STATE_RX:
    DEBUGF(infof(दत्त, "TFTP_STATE_RX"));
    परिणाम = tftp_rx(state, event);
    भङ्ग;
  पक्ष TFTP_STATE_TX:
    DEBUGF(infof(दत्त, "TFTP_STATE_TX"));
    परिणाम = tftp_tx(state, event);
    भङ्ग;
  पक्ष TFTP_STATE_FIN:
    infof(दत्त, "%s", "TFTP finished");
    भङ्ग;
  स्वभाव:
    DEBUGF(infof(दत्त, "STATE: %d", (पूर्णाङ्क)state->state));
    failf(दत्त, "%s", "Internal state machine error");
    परिणाम = CURLE_TFTP_ILLEGAL;
    भङ्ग;
  }

  प्रत्यागम परिणाम;
}

स्थिर शून्य tftp_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना tftp_conn *state = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(state->rpacket.दत्त);
  curlx_safefree(state->spacket.दत्त);
  curlx_free(state);
}

/**********************************************************
 *
 * tftp_connect
 *
 * The connect callback
 *
 **********************************************************/
स्थिर CURLcode tftp_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना tftp_conn *state;
  पूर्णाङ्क blksize;
  पूर्णाङ्क need_blksize;
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  CURLcode परिणाम;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = curlx_calloc(1, परिमाण(*state));
  यदि(!state ||
     Curl_conn_meta_समुच्चयः(conn, CURL_META_TFTP_CONN, state, tftp_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* alloc pkt buffers based on specified blksize */
  यदि(दत्त->समुच्चयः.tftp_blksize)
    /* range checked when set */
    blksize = (पूर्णाङ्क)दत्त->समुच्चयः.tftp_blksize;

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  यदि(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  यदि(!state->rpacket.दत्त) {
    state->rpacket.दत्त = curlx_calloc(1, need_blksize + 2 + 2);

    यदि(!state->rpacket.दत्त)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(!state->spacket.दत्त) {
    state->spacket.दत्त = curlx_calloc(1, need_blksize + 2 + 2);

    यदि(!state->spacket.दत्त)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* we do not keep TFTP connections up because there is none or little gain
   * for UDP */
  connclose(conn);

  state->दत्त = दत्त;
  state->sockfd = conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_START;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  remote_addr = Curl_conn_get_remote_addr(दत्त, FIRSTSOCKET);
  DEBUGASSERT(remote_addr);
  यदि(!remote_addr)
    प्रत्यागम CURLE_FAILED_INIT;

  ((संरचना sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)remote_addr->family;

  परिणाम = tftp_समुच्चयः_timeouts(state);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!conn->द्वयङ्काः.bound) {
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
    पूर्णाङ्क rc = bind(state->sockfd, (संरचना sockaddr *)&state->local_addr,
                  (curl_socklen_t)remote_addr->addrlen);
    यदि(rc) {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "bind() failed; %s",
            curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
    conn->द्वयङ्काः.bound = TRUE;
  }

  *समाप्तम् = TRUE;

  प्रत्यागम CURLE_OK;
}

/**********************************************************
 *
 * tftp_done
 *
 * The done callback
 *
 **********************************************************/
स्थिर CURLcode tftp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  (शून्य)स्थितिः;
  (शून्य)premature;

  यदि(Curl_pgrs_समाप्तम्(दत्त, Curl_pgrs_now(दत्त)))
    प्रत्यागम CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  यदि(state)
    परिणाम = tftp_translate_संहिता(state->error);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode tftp_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  प्रत्यागम Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
}

/**********************************************************
 *
 * tftp_receive_packet
 *
 * Called once select fires and data is ready on the socket
 *
 **********************************************************/
स्थिर CURLcode tftp_receive_packet(संरचना Curl_easy *दत्त,
                                    संरचना tftp_conn *state)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना Curl_sockaddr_storage remote_addr;
  curl_socklen_t fromlen = परिमाण(remote_addr);

  /* Receive the packet */
  state->rbytes = (पूर्णाङ्क)recvfrom(state->sockfd,
                                (शून्य *)state->rpacket.दत्त,
                                (RECV_TYPE_ARG3)state->blksize + 4,
                                0,
                                (संरचना sockaddr *)&remote_addr,
                                &fromlen);
  यदि((state->rbytes >= 0) && fromlen) {
    यदि(state->remote_pinned) {
      /* pinned, verify that it comes from the same address */
      यदि((state->remote_addrlen != fromlen) ||
         memcmp(&remote_addr, &state->remote_addr, fromlen)) {
        failf(दत्त, "Data received from another address");
        प्रत्यागम CURLE_RECV_ERROR;
      }
    }
    अन्यथा {
      /* pin address on first use */
      state->remote_pinned = TRUE;
      state->remote_addrlen = fromlen;
      स्मृतिं_प्रतिलिख(&state->remote_addr, &remote_addr, fromlen);
    }
  }

  /* Sanity check packet length */
  यदि(state->rbytes < 4) {
    failf(दत्त, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  अन्यथा {
    /* The event is given by the TFTP packet time */
    अचिह्नित ह्रस्व event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    विकल्प(state->event) {
    पक्ष TFTP_EVENT_दत्त:
      यदि(दत्त->state.upload)
        प्रत्यागम CURLE_TFTP_ILLEGAL;
      /* Do not pass to the client empty or retransmitted packets */
      यदि(state->rbytes > 4 &&
         (अग्र_BLOCKNUM(state->खण्डम्) == getrpacketblock(&state->rpacket))) {
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                   (ध्रुव वर्ण *)state->rpacket.दत्त + 4,
                                   state->rbytes - 4);
        यदि(परिणाम) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          प्रत्यागम परिणाम;
        }
      }
      भङ्ग;
    पक्ष TFTP_EVENT_ERROR: {
      अचिह्नित ह्रस्व error = getrpacketblock(&state->rpacket);
      ध्रुव वर्ण *str = (ध्रुव वर्ण *)state->rpacket.दत्त + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      यदि(tftp_strnlen(str, strn) < strn)
        infof(दत्त, "TFTP error: %s", str);
      भङ्ग;
    }
    पक्ष TFTP_EVENT_ACK:
      भङ्ग;
    पक्ष TFTP_EVENT_OACK:
      परिणाम = tftp_parse_option_ack(state,
                                     (ध्रुव वर्ण *)state->rpacket.दत्त + 2,
                                     state->rbytes-2);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      भङ्ग;
    पक्ष TFTP_EVENT_RRQ:
    पक्ष TFTP_EVENT_WRQ:
    स्वभाव:
      failf(दत्त, "%s", "Internal error: Unexpected packet");
      भङ्ग;
    }

    /* Update the progress meter */
    परिणाम = Curl_pgrsUpdate(दत्त);
    यदि(परिणाम) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      प्रत्यागम परिणाम;
    }
  }
  प्रत्यागम परिणाम;
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
  time_t current;
  timediff_t timeout_ms;

  यदि(event)
    *event = TFTP_EVENT_NONE;

  timeout_ms = Curl_timeleft_ms(state->दत्त);
  यदि(timeout_ms < 0) {
    state->error = TFTP_ERR_TIMEOUT;
    state->state = TFTP_STATE_FIN;
    प्रत्यागम timeout_ms;
  }
  current = time(रिक्त);
  यदि(current > state->rx_time + state->retry_time) {
    यदि(event)
      *event = TFTP_EVENT_TIMEOUT;
    state->rx_time = time(रिक्त); /* update even though we received nothing */
  }

  प्रत्यागम timeout_ms;
}

/**********************************************************
 *
 * tftp_multi_statemach
 *
 * Handle single RX socket event and return
 *
 **********************************************************/
स्थिर CURLcode tftp_multi_statemach(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  tftp_event_t event;
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  timediff_t timeout_ms;

  *समाप्तम् = FALSE;
  यदि(!state)
    प्रत्यागम CURLE_FAILED_INIT;

  timeout_ms = tftp_state_timeout(state, &event);
  यदि(timeout_ms < 0) {
    failf(दत्त, "TFTP response timeout");
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }
  यदि(event != TFTP_EVENT_NONE) {
    परिणाम = tftp_state_machine(state, event);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    *समाप्तम् = (state->state == TFTP_STATE_FIN);
    यदि(*समाप्तम्)
      /* Tell curl we are done */
      Curl_xfer_सज्जता_nop(दत्त);
  }
  अन्यथा {
    /* no timeouts to handle, check our socket */
    पूर्णाङ्क rc = SOCKET_READABLE(state->sockfd, 0);

    यदि(rc == -1) {
      /* bail out */
      पूर्णाङ्क sockerr = SOCKERRNO;
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "%s", curlx_strerror(sockerr, बफर, परिमाण(बफर)));
      state->event = TFTP_EVENT_ERROR;
    }
    अन्यथा यदि(rc) {
      परिणाम = tftp_receive_packet(दत्त, state);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      परिणाम = tftp_state_machine(state, state->event);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      *समाप्तम् = (state->state == TFTP_STATE_FIN);
      यदि(*समाप्तम्)
        /* Tell curl we are done */
        Curl_xfer_सज्जता_nop(दत्त);
    }
    /* if rc == 0, then select() timed out */
  }

  प्रत्यागम परिणाम;
}

/**********************************************************
 *
 * tftp_doing
 *
 * Called from multi.c while DOing
 *
 **********************************************************/
स्थिर CURLcode tftp_doing(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  CURLcode परिणाम;
  परिणाम = tftp_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(*dophase_समाप्तम्) {
    DEBUGF(infof(दत्त, "DO phase is complete"));
  }
  अन्यथा यदि(!परिणाम) {
    /* The multi code does not have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    परिणाम = Curl_pgrsCheck(दत्त);
  }
  प्रत्यागम परिणाम;
}

/**********************************************************
 *
 * tftp_perform
 *
 * Entry point for transfer from tftp_do, starts state mach
 *
 **********************************************************/
स्थिर CURLcode tftp_perform(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);

  *dophase_समाप्तम् = FALSE;
  यदि(!state)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = tftp_state_machine(state, TFTP_EVENT_INIT);

  यदि((state->state == TFTP_STATE_FIN) || परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = tftp_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(*dophase_समाप्तम्)
    DEBUGF(infof(दत्त, "DO phase is complete"));

  प्रत्यागम परिणाम;
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

स्थिर CURLcode tftp_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना tftp_conn *state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
  CURLcode परिणाम;

  *समाप्तम् = FALSE;

  यदि(!state) {
    परिणाम = tftp_connect(दत्त, समाप्तम्);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    state = Curl_conn_meta_get(conn, CURL_META_TFTP_CONN);
    यदि(!state)
      प्रत्यागम CURLE_TFTP_ILLEGAL;
  }

  परिणाम = tftp_perform(दत्त, समाप्तम्);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  यदि(!परिणाम)
    /* If we have encountered an internal tftp error, translate it. */
    परिणाम = tftp_translate_संहिता(state->error);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode tftp_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  वर्ण *मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  size_t len = वाक्यदैर्घ्यम्(मार्गः);

  conn->transport_wanted = TRNSPRT_UDP;

  /* TFTP URLs support a trailing ";mode=netascii" or ";mode=octet" */
  यदि((len >= 14) && !memcmp(&मार्गः[len - 14], ";mode=netascii", 14)) {
    दत्त->state.prefer_ascii = TRUE;
    मार्गः[len - 14] = 0; /* cut it there */
  }
  अन्यथा यदि((len >= 11) && !memcmp(&मार्गः[len - 11], ";mode=octet", 11)) {
    दत्त->state.prefer_ascii = FALSE;
    मार्गः[len - 11] = 0; /* cut it there */
  }

  प्रत्यागम CURLE_OK;
}

/*
 * TFTP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_tftp = {
  tftp_सज्जता_connection,                /* setup_connection */
  tftp_do,                              /* do_it */
  tftp_समाप्तम्,                            /* done */
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

#यदि_अन्तः
