#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_PINGPONG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_PINGPONG_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_POP3) || !defined(CURL_DISABLE_SMTP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_PINGPONG
#ਜੇ_ਅੰਤ

/* forward-declaration, this is defined in urldata.h */
ਬਣਤਰ connectdata;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
ਬਣਤਰ pingpong {
  size_t nread_resp;  /* number of bytes currently read of a server response */
  ਅੱਖਰ *sendthis; /* pointer to a buffer that is to be sent to the server */
  size_t sendleft; /* number of bytes left to send from the sendthis buffer */
  size_t sendsize; /* total size of the sendthis buffer */
  ਬਣਤਰ curltime response; /* set to Curl_now() when a command has been sent
                               off, used to time-out response reading */
  ਬਣਤਰ dynbuf sendbuf;
  ਬਣਤਰ dynbuf recvbuf;
  size_t ਉਛਾਲ; /* number of bytes left after a final response line */
  size_t nfinal;   /* number of bytes in the final response line, which
                      after a match is first in the receive buffer */

  /* Function pointers the protocols MUST implement and provide for the
     pingpong layer to function */

  CURLcode (*statemachine)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn);
  bool (*endofresp)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                    ਅਚਲ ਅੱਖਰ *ptr, size_t len, ਪੂਰਨ *ਕੋਡ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
  ਦੋਆਧਾਰੀ_ਅੰਕ(pending_resp);  /* set TRUE when a server response is pending or in
                         progress, and is cleared once the last response is
                         read */
};

/* Default pingpong response timeout in milliseconds, unless a transfer
 * has CURLOPT_SERVER_RESPONSE_TIMEOUT(_MS) set. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PINGPONG_TIMEOUT_MS      (60 * 1000)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PINGPONG_ਤਿਆਰੀ(pp, s, e) \
  ਕਰੋ {                           \
    (pp)->statemachine = s;      \
    (pp)->endofresp = e;         \
  } ਜਦੋਂ(0)

/*
 * Curl_pp_statemach()
 *
 * called repeatedly until done. Set 'wait' to make it wait a while on the
 * socket if there is no traffic.
 */
CURLcode Curl_pp_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ pingpong *pp,
                           bool ਖੰਡ, bool disconnecting);

/* initialize stuff to prepare for reading a fresh new response */
ਖਾਲੀ Curl_pp_init(ਬਣਤਰ pingpong *pp, ਅਚਲ ਬਣਤਰ curltime *pnow);

/* Returns time remaining in ms. 0 or negative number means the
  timeout has already triggered */
timediff_t Curl_pp_state_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ pingpong *pp);

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
CURLcode Curl_pp_sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ pingpong *pp,
                       ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(3, 4);

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
CURLcode Curl_pp_vsendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ pingpong *pp,
                        ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ,
                        ਅਚਲ ਅੱਖਰ *fmt,
                        va_list ਦਲੀਲਾਂ) CURL_PRINTF(4, 0);

/*
 * Curl_pp_readresp()
 *
 * Reads a piece of a server response.
 */
CURLcode Curl_pp_readresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex,
                          ਬਣਤਰ pingpong *pp,
                          ਪੂਰਨ *ਕੋਡ, /* return the server code if done */
                          size_t *ਮਾਪ); /* size of the response */

bool Curl_pp_needs_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ pingpong *pp);

CURLcode Curl_pp_flushsend(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ pingpong *pp);

/* call this when a pingpong connection is disconnected */
CURLcode Curl_pp_disconnect(ਬਣਤਰ pingpong *pp);

CURLcode Curl_pp_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ pingpong *pp,
                         ਬਣਤਰ easy_pollset *ps);

/***********************************************************************
 *
 * Curl_pp_moredata()
 *
 * Returns whether there are still more data in the cache and so a call
 * to Curl_pp_readresp() will not block.
 */
bool Curl_pp_moredata(ਬਣਤਰ pingpong *pp);

#ਜੇ_ਅੰਤ /* HEADER_CURL_PINGPONG_H */
