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
#अन्तर्भाव "curl_setup.h"

#सचे !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_POP3) || !defined(CURL_DISABLE_SMTP)
#define USE_PINGPONG
#endif

/* forward-declaration, this is defined in urldata.h */
संरचना connectdata;

प्रकारनिर्देश गणना {
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
संरचना pingpong {
  size_t nread_resp;  /* number of bytes currently read of a server response */
  वर्ण *sendthis; /* pointer to a buffer that is to be sent to the server */
  size_t sendleft; /* number of bytes left to send from the sendthis buffer */
  size_t sendsize; /* total size of the sendthis buffer */
  संरचना curltime response; /* set to Curl_now() when a command has been sent
                               off, used to time-out response reading */
  संरचना dynbuf sendbuf;
  संरचना dynbuf recvbuf;
  size_t overflow; /* number of bytes left after a final response line */
  size_t nfinal;   /* number of bytes in the final response line, which
                      after a match is first in the receive buffer */

  /* Function pointers the protocols MUST implement and provide for the
     pingpong layer to function */

  CURLcode (*statemachine)(संरचना Curl_easy *data, संरचना connectdata *conn);
  bool (*endofresp)(संरचना Curl_easy *data, संरचना connectdata *conn,
                    ध्रुव वर्ण *ptr, size_t len, पूर्णाङ्क *code);
  BIT(initialized);
  BIT(pending_resp);  /* set TRUE when a server response is pending or in
                         progress, and is cleared once the last response is
                         read */
};

/* Default pingpong response timeout in milliseconds, unless a transfer
 * has CURLOPT_SERVER_RESPONSE_TIMEOUT(_MS) set. */
#define PINGPONG_TIMEOUT_MS      (60 * 1000)

#define PINGPONG_SETUP(pp, s, e) \
  करोहि {                           \
    (pp)->statemachine = s;      \
    (pp)->endofresp = e;         \
  } याव(0)

/*
 * Curl_pp_statemach()
 *
 * called repeatedly until done. Set 'wait' to make it wait a while on the
 * socket if there is no traffic.
 */
CURLcode Curl_pp_statemach(संरचना Curl_easy *data, संरचना pingpong *pp,
                           bool block, bool disconnecting);

/* initialize stuff to prepare for reading a fresh new response */
शून्य Curl_pp_init(संरचना pingpong *pp, ध्रुव संरचना curltime *pnow);

/* Returns time remaining in ms. 0 or negative number means the
  timeout has already triggered */
timediff_t Curl_pp_state_timeleft_ms(संरचना Curl_easy *data,
                                     संरचना pingpong *pp);

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
CURLcode Curl_pp_sendf(संरचना Curl_easy *data,
                       संरचना pingpong *pp,
                       ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);

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
CURLcode Curl_pp_vsendf(संरचना Curl_easy *data,
                        संरचना pingpong *pp,
                        ध्रुव वर्ण *prefix,
                        ध्रुव वर्ण *fmt,
                        va_list args) CURL_PRINTF(4, 0);

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(संरचना Curl_easy *data,
                          int8_t sockindex,
                          संरचना pingpong *pp,
                          पूर्णाङ्क *code, /* return the server code if done */
                          size_t *size); /* size of the response */

bool Curl_pp_needs_flush(संरचना Curl_easy *data,
                         संरचना pingpong *pp);

CURLcode Curl_pp_flushsend(संरचना Curl_easy *data,
                           संरचना pingpong *pp);

/* call this when a pingpong connection is disconnected */
CURLcode Curl_pp_disconnect(संरचना pingpong *pp);

CURLcode Curl_pp_pollset(संरचना Curl_easy *data,
                         संरचना pingpong *pp,
                         संरचना easy_pollset *ps);

/***********************************************************************
 *
 * Curl_pp_moredata()
 *
 * Returns whether there are still more data in the cache and so a call
 * to Curl_pp_readresp() will not block.
 */
bool Curl_pp_moredata(संरचना pingpong *pp);

#endif /* HEADER_CURL_PINGPONG_H */
