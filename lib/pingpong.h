#अगर_अघोषित HEADER_CURL_PINGPONG_H
#घोषणा HEADER_CURL_PINGPONG_H
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

#अगर !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_POP3) || !defined(CURL_DISABLE_SMTP)
#घोषणा USE_PINGPONG
#पूर्ण_अगर

/* forward-declaration, this is defined in urldata.h */
काष्ठा connectdata;

प्रकार क्रमागत {
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
काष्ठा pingpong {
  माप_प्रकार nread_resp;  /* number of bytes currently read of a server response */
  अक्षर *sendthis; /* pointer to a buffer that is to be sent to the server */
  माप_प्रकार sendleft; /* number of bytes left to send from the sendthis buffer */
  माप_प्रकार sendsize; /* total size of the sendthis buffer */
  काष्ठा curltime response; /* set to Curl_now() when a command has been sent
                               off, used to time-out response reading */
  काष्ठा dynbuf sendbuf;
  काष्ठा dynbuf recvbuf;
  माप_प्रकार overflow; /* number of bytes left after a final response line */
  माप_प्रकार nfinal;   /* number of bytes in the final response line, which
                      after a match is first in the receive buffer */

  /* Function pointers the protocols MUST implement and provide for the
     pingpong layer to function */

  CURLcode (*statemachine)(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn);
  bool (*endofresp)(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                    स्थिर अक्षर *ptr, माप_प्रकार len, पूर्णांक *code);
  BIT(initialized);
  BIT(pending_resp);  /* set TRUE when a server response is pending or in
                         progress, and is cleared once the last response is
                         read */
};

/* Default pingpong response timeout in milliseconds, unless a transfer
 * has CURLOPT_SERVER_RESPONSE_TIMEOUT(_MS) set. */
#घोषणा PINGPONG_TIMEOUT_MS      (60 * 1000)

#घोषणा PINGPONG_SETUP(pp, s, e) \
  करो {                           \
    (pp)->statemachine = s;      \
    (pp)->endofresp = e;         \
  } जबतक(0)

/*
 * Curl_pp_statemach()
 *
 * called repeatedly until done. Set 'wait' to make it wait a while on the
 * socket if there is no traffic.
 */
CURLcode Curl_pp_statemach(काष्ठा Curl_easy *तथ्य, काष्ठा pingpong *pp,
                           bool block, bool disconnecting);

/* initialize stuff to prepare for reading a fresh new response */
व्योम Curl_pp_init(काष्ठा pingpong *pp, स्थिर काष्ठा curltime *pnow);

/* Returns time remaining in ms. 0 or negative number means the
  timeout has already triggered */
timediff_t Curl_pp_state_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा pingpong *pp);

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
CURLcode Curl_pp_sendf(काष्ठा Curl_easy *तथ्य,
                       काष्ठा pingpong *pp,
                       स्थिर अक्षर *fmt, ...) CURL_PRINTF(3, 4);

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
CURLcode Curl_pp_vsendf(काष्ठा Curl_easy *तथ्य,
                        काष्ठा pingpong *pp,
                        स्थिर अक्षर *prefix,
                        स्थिर अक्षर *fmt,
                        बहु_सूची args) CURL_PRINTF(4, 0);

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(काष्ठा Curl_easy *तथ्य,
                          int8_t sockindex,
                          काष्ठा pingpong *pp,
                          पूर्णांक *code, /* return the server code if done */
                          माप_प्रकार *आकार); /* size of the response */

bool Curl_pp_needs_flush(काष्ठा Curl_easy *तथ्य,
                         काष्ठा pingpong *pp);

CURLcode Curl_pp_flushsend(काष्ठा Curl_easy *तथ्य,
                           काष्ठा pingpong *pp);

/* call this when a pingpong connection is disconnected */
CURLcode Curl_pp_disconnect(काष्ठा pingpong *pp);

CURLcode Curl_pp_pollset(काष्ठा Curl_easy *तथ्य,
                         काष्ठा pingpong *pp,
                         काष्ठा easy_pollset *ps);

/***********************************************************************
 *
 * Curl_pp_moredata()
 *
 * Returns whether there are still more data in the cache and so a call
 * to Curl_pp_readresp() will not block.
 */
bool Curl_pp_moredata(काष्ठा pingpong *pp);

#पूर्ण_अगर /* HEADER_CURL_PINGPONG_H */
