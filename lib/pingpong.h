#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PINGPONG_H
#నిర్వచించు శీర్షిక_CURL_PINGPONG_H
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

#ఒకవేళ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_POP3) || !defined(CURL_DISABLE_SMTP)
#నిర్వచించు USE_PINGPONG
#అయితే_ముగింపు

/* forward-declaration, this is defined in urldata.h */
నిర్మాణం connectdata;

రకనిర్వచనం గణన {
  PPTRANSFER_BODY, /* yes do transfer a body */
  PPTRANSFER_INFO, /* do still go through to get info/headers */
  PPTRANSFER_NONE  /* do not get anything and do not get info */
} curl_pp_transfer;

/*
 * 'pingpong' is the generic struct used for protocols doing server<->client
 * conversations in a back-and-forth style such as FTP, IMAP, POP3, SMTP etc.
 *
 * It holds response cache and non-blocking sending data.
 */
నిర్మాణం pingpong {
  size_t nread_resp;  /* number of bytes currently read of a server response */
  అక్షరం *sendthis; /* pointer to a buffer that is to be sent to the server */
  size_t sendleft; /* number of bytes left to send from the sendthis buffer */
  size_t sendsize; /* total size of the sendthis buffer */
  నిర్మాణం curltime response; /* set to Curl_now() when a command has been sent
                               off, used to time-out response reading */
  నిర్మాణం dynbuf sendbuf;
  నిర్మాణం dynbuf recvbuf;
  size_t అధికప్రవాహం; /* number of bytes left after a final response line */
  size_t nfinal;   /* number of bytes in the final response line, which
                      after a match is first in the receive buffer */

  /* Function pointers the protocols MUST implement and provide for the
     pingpong layer to function */

  CURLcode (*statemachine)(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn);
  bool (*endofresp)(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                    స్థిరరాశి అక్షరం *ptr, size_t len, పూర్ణాంకం *సంకేతం_2);
  బిట్టు(initialized);
  బిట్టు(pending_resp);  /* set TRUE when a server response is pending or in
                         progress, and is cleared once the last response is
                         read */
};

/* Default pingpong response timeout in milliseconds, unless a transfer
 * has CURLOPT_SERVER_RESPONSE_TIMEOUT(_MS) set. */
#నిర్వచించు PINGPONG_TIMEOUT_MS      (60 * 1000)

#నిర్వచించు PINGPONG_అమరిక(pp, s, e) \
  చేయి {                           \
    (pp)->statemachine = s;      \
    (pp)->endofresp = e;         \
  } అయితే(0)

/*
 * Curl_pp_statemach()
 *
 * called repeatedly until done. Set 'wait' to make it wait a while on the
 * socket if there is no traffic.
 */
CURLcode Curl_pp_statemach(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం pingpong *pp,
                           bool ఖండం, bool disconnecting);

/* initialize stuff to prepare for reading a fresh new response */
శూన్య Curl_pp_init(నిర్మాణం pingpong *pp, స్థిరరాశి నిర్మాణం curltime *pnow);

/* Returns time remaining in ms. 0 or negative number means the
  timeout has already triggered */
timediff_t Curl_pp_state_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం pingpong *pp);

/***********************************************************************
 *
 * Curl_pp_sendf()
 *
 * Send the formatted string as a command to a pingpong server. Note that
 * the string should not have any CRLF appended, as this function will
 * append the necessary things itself.
 *
 * made to never block
 */
CURLcode Curl_pp_sendf(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం pingpong *pp,
                       స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(3, 4);

/***********************************************************************
 *
 * Curl_pp_vsendf()
 *
 * Send the formatted string as a command to a pingpong server. Note that
 * the string should not have any CRLF appended, as this function will
 * append the necessary things itself.
 * If `prefix` is non-NULL, prepend it with an added space before the format.
 *
 * made to never block
 */
CURLcode Curl_pp_vsendf(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం pingpong *pp,
                        స్థిరరాశి అక్షరం *ఉపసర్గ,
                        స్థిరరాశి అక్షరం *fmt,
                        va_list వాదనలు) CURL_PRINTF(4, 0);

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex,
                          నిర్మాణం pingpong *pp,
                          పూర్ణాంకం *సంకేతం_2, /* return the server code if done */
                          size_t *కొలత); /* size of the response */

bool Curl_pp_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం pingpong *pp);

CURLcode Curl_pp_flushsend(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం pingpong *pp);

/* call this when a pingpong connection is disconnected */
CURLcode Curl_pp_disconnect(నిర్మాణం pingpong *pp);

CURLcode Curl_pp_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం pingpong *pp,
                         నిర్మాణం easy_pollset *ps);

/***********************************************************************
 *
 * Curl_pp_moredata()
 *
 * Returns whether there are still more data in the cache and so a call
 * to Curl_pp_readresp() will not block.
 */
bool Curl_pp_moredata(నిర్మాణం pingpong *pp);

#అయితే_ముగింపు /* HEADER_CURL_PINGPONG_H */
