#ifndef HEADER_CURL_PINGPONG_H
#define HEADER_CURL_PINGPONG_H
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

#যদি !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_POP3) || !defined(CURL_DISABLE_SMTP)
#define USE_PINGPONG
#endif

/* forward-declaration, this is defined in urldata.h */
সংরচনা connectdata;

প্রকারনির্দেশ গণনা {
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
সংরচনা pingpong {
  size_t nread_resp;  /* number of bytes currently read of a server response */
  বর্ণ *sendthis; /* pointer to a buffer that is to be sent to the server */
  size_t sendleft; /* number of bytes left to send from the sendthis buffer */
  size_t sendsize; /* total size of the sendthis buffer */
  সংরচনা curltime response; /* set to Curl_now() when a command has been sent
                               off, used to time-out response reading */
  সংরচনা dynbuf sendbuf;
  সংরচনা dynbuf recvbuf;
  size_t overflow; /* number of bytes left after a final response line */
  size_t nfinal;   /* number of bytes in the final response line, which
                      after a match is first in the receive buffer */

  /* Function pointers the protocols MUST implement and provide for the
     pingpong layer to function */

  CURLcode (*statemachine)(সংরচনা Curl_easy *data, সংরচনা connectdata *conn);
  bool (*endofresp)(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                    ধ্রুব বর্ণ *ptr, size_t len, পূর্ণাঙ্ক *code);
  BIT(initialized);
  BIT(pending_resp);  /* set TRUE when a server response is pending or in
                         progress, and is cleared once the last response is
                         read */
};

/* Default pingpong response timeout in milliseconds, unless a transfer
 * has CURLOPT_SERVER_RESPONSE_TIMEOUT(_MS) set. */
#define PINGPONG_TIMEOUT_MS      (60 * 1000)

#define PINGPONG_SETUP(pp, s, e) \
  কুরু {                           \
    (pp)->statemachine = s;      \
    (pp)->endofresp = e;         \
  } যাবত্(0)

/*
 * Curl_pp_statemach()
 *
 * called repeatedly until done. Set 'wait' to make it wait a while on the
 * socket if there is no traffic.
 */
CURLcode Curl_pp_statemach(সংরচনা Curl_easy *data, সংরচনা pingpong *pp,
                           bool block, bool disconnecting);

/* initialize stuff to prepare for reading a fresh new response */
শূন্য Curl_pp_init(সংরচনা pingpong *pp, ধ্রুব সংরচনা curltime *pnow);

/* Returns time remaining in ms. 0 or negative number means the
  timeout has already triggered */
timediff_t Curl_pp_state_timeleft_ms(সংরচনা Curl_easy *data,
                                     সংরচনা pingpong *pp);

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
CURLcode Curl_pp_sendf(সংরচনা Curl_easy *data,
                       সংরচনা pingpong *pp,
                       ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(3, 4);

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
CURLcode Curl_pp_vsendf(সংরচনা Curl_easy *data,
                        সংরচনা pingpong *pp,
                        ধ্রুব বর্ণ *prefix,
                        ধ্রুব বর্ণ *fmt,
                        va_list args) CURL_PRINTF(4, 0);

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(সংরচনা Curl_easy *data,
                          int8_t sockindex,
                          সংরচনা pingpong *pp,
                          পূর্ণাঙ্ক *code, /* return the server code if done */
                          size_t *size); /* size of the response */

bool Curl_pp_needs_flush(সংরচনা Curl_easy *data,
                         সংরচনা pingpong *pp);

CURLcode Curl_pp_flushsend(সংরচনা Curl_easy *data,
                           সংরচনা pingpong *pp);

/* call this when a pingpong connection is disconnected */
CURLcode Curl_pp_disconnect(সংরচনা pingpong *pp);

CURLcode Curl_pp_pollset(সংরচনা Curl_easy *data,
                         সংরচনা pingpong *pp,
                         সংরচনা easy_pollset *ps);

/***********************************************************************
 *
 * Curl_pp_moredata()
 *
 * Returns whether there are still more data in the cache and so a call
 * to Curl_pp_readresp() will not block.
 */
bool Curl_pp_moredata(সংরচনা pingpong *pp);

#endif /* HEADER_CURL_PINGPONG_H */
