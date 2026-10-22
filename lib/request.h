#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_REQUEST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_REQUEST_H
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
/* This file is for lib internal stuff */
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"

/* forward declarations */
ਬਣਤਰ UserDefined;

/* Bits on the io_flags member of SingleRequest */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQ_IO_RECV       (1 << 0) /* there is or may be data to read */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQ_IO_SEND       (1 << 1) /* there is or may be data to write */

/* Low level request receive/send io_flags checks. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_WANT_SEND(d)  ((d)->req.io_flags & REQ_IO_SEND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_WANT_RECV(d)  ((d)->req.io_flags & REQ_IO_RECV)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_WANT_IO(d)    \
  ((d)->req.io_flags & (REQ_IO_RECV | REQ_IO_SEND))
/* Low level request receive/send io_flags manipulations. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_ਸੈੱਟ_SEND(d)   ((d)->req.io_flags |= REQ_IO_SEND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_ਸੈੱਟ_RECV(d)   ((d)->req.io_flags |= REQ_IO_RECV)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_CLEAR_SEND(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_SEND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_CLEAR_RECV(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_RECV)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_REQ_CLEAR_IO(d)  \
  ((d)->req.io_flags &= (uint8_t)~(REQ_IO_RECV | REQ_IO_SEND))

ਗਿਣਤੀ expect100 {
  EXP100_SEND_ਅੰਕੜੇ,           /* enough waiting, send the body now */
  EXP100_AWAITING_CONTINUE,   /* waiting for the 100 Continue header */
  EXP100_SENDING_REQUEST,     /* still sending the request but will wait for
                                 the 100 header once done with the request */
  EXP100_FAILED               /* used on 417 Expectation Failed */
};

ਗਿਣਤੀ upgrade101 {
  UPGR101_NONE,               /* default state */
  UPGR101_WS,                 /* upgrade to WebSocket requested */
  UPGR101_H2,                 /* upgrade to HTTP/2 requested */
  UPGR101_RECEIVED            /* 101 response received */
};

/*
 * Request specific data in the easy handle (Curl_easy). Previously,
 * these members were on the connectdata struct but since a conn struct may
 * now be shared between different Curl_easys, we store connection-specific
 * data here. This struct only keeps stuff that is interesting for *this*
 * request, as it will be cleared between multiple ones
 */
ਬਣਤਰ SingleRequest {
  curl_off_t ਮਾਪ;        /* -1 if unknown at this point */
  curl_off_t maxdownload; /* in bytes, the maximum amount of data to fetch,
                             -1 means unlimited */
  curl_off_t bytecount;         /* total number of bytes read */
  curl_off_t writebytecount;    /* number of bytes written */
  curl_off_t ਵਿੱਥ;            /* possible resume offset read from the
                                   Content-Range: header */

  ਬਣਤਰ curltime ਸ਼ੁਰੂ;         /* transfer started at this time */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ headerbytecount;  /* received server headers (not CONNECT
                                    headers) */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ allheadercount;   /* all received headers (server + CONNECT) */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ deductheadercount; /* this amount of bytes does not count when
                                     we check if anything has been transferred
                                     at the end of a connection. We use this
                                     counter to make only a 100 reply (without
                                     a following second response code) result
                                     in a CURLE_GOT_NOTHING error code */
  ਪੂਰਨ headerline;               /* counts header lines to better track the
                                   first one */
  ਪੂਰਨ httpcode;                 /* error code from the 'HTTP/1.? XXX' or
                                   'RTSP/1.? XXX' line */
  uint8_t httpversion_ਭੇਜਿਆ;     /* Version in request (09, 10, 11, etc.) */
  uint8_t httpversion;          /* Version in response (09, 10, 11, etc.) */
  uint8_t upgr101;              /* 101 upgrade state */
  uint8_t io_flags;             /* REQ_IO_RECV | REQ_IO_SEND */

  /* Client Writer stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  ਬਣਤਰ {
    ਬਣਤਰ Curl_cwriter *ਢੇਰ;
    ਦੋਆਧਾਰੀ_ਅੰਕ(paused);
  } writer;
  /* Client Reader stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  ਬਣਤਰ {
    ਬਣਤਰ Curl_creader *ਢੇਰ;
  } reader;
  ਬਣਤਰ bufq sendbuf; /* data which needs to be send to the server */
  size_t sendbuf_hds_len; /* amount of header bytes in sendbuf */
  time_t timeofdoc;
  ਅੱਖਰ *location;   /* This points to an allocated version of the Location:
                       header data */
  ਅੱਖਰ *newurl;     /* Set to the new URL to use when a redirect or a retry is
                       wanted */

  ਅੱਖਰ *hd_auth;      /* Authorization header, full HTTP/1.x line */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਅੱਖਰ *hd_proxy_auth; /* Proxy-Authorization header, full HTTP/1.x line */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਅੱਖਰ *cookiehost;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ setcookies;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਸਿਰਲੇਖ);        /* incoming data has HTTP header */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਾ);          /* request is done, e.g. no more send/recv should
                       * happen. This can be TRUE before `upload_done` or
                       * `download_done` is TRUE. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(content_range); /* set TRUE if Content-Range: was found */
  ਦੋਆਧਾਰੀ_ਅੰਕ(download_ਪੂਰਾ); /* set to TRUE when download is complete */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos_written);   /* iff EOS has been written to client */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos_read);      /* iff EOS has been read from the client */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos_ਭੇਜਿਆ);      /* iff EOS has been sent to the server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(rewind_read);   /* iff reader needs rewind at next start */
  ਦੋਆਧਾਰੀ_ਅੰਕ(upload_ਪੂਰਾ);   /* set to TRUE when all request data has been sent */
  ਦੋਆਧਾਰੀ_ਅੰਕ(upload_aborted); /* set to TRUE when upload was aborted. Also
                        * shows `upload_done` as TRUE. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ignorebody);    /* we read a response-body but we ignore it! */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_bodyless); /* HTTP response status code is between 100 and 199,
                         204 or 304 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(chunk);         /* if set, this is a chunked transfer-encoding */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resp_trailer);  /* response carried 'Trailer:' header field */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ignore_cl);     /* ignore content-length */
  ਦੋਆਧਾਰੀ_ਅੰਕ(upload_chunky); /* set TRUE if we are doing chunked transfer-encoding
                         on upload */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_body);      /* the response has no body */
  ਦੋਆਧਾਰੀ_ਅੰਕ(authneg);      /* TRUE when the auth phase has started, which means
                        that we are creating a request with an auth header,
                        but it is not the final request in the auth
                        negotiation. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(sendbuf_init); /* sendbuf is initialized */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown);     /* request end will shutdown connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_err_ignore); /* errors in shutdown will not fail request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reader_started); /* client reads have started */
};

/**
 * Initialize the state of the request for first use.
 */
ਖਾਲੀ Curl_req_init(ਬਣਤਰ SingleRequest *req);

/**
 * The request is about to start. Record time and do a soft reset.
 */
CURLcode Curl_req_ਸ਼ੁਰੂ(ਬਣਤਰ SingleRequest *req,
                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * The request may continue with a follow up. Reset
 * members, but keep start time for overall duration calc.
 */
CURLcode Curl_req_soft_reset(ਬਣਤਰ SingleRequest *req,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * The request is done. If not aborted, make sure that buffers are
 * flushed to the client.
 * @param req        the request
 * @param data       the transfer
 * @param aborted    TRUE iff the request was aborted/errored
 */
CURLcode Curl_req_ਪੂਰਾ(ਬਣਤਰ SingleRequest *req,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool aborted);

/**
 * Free the state of the request, not usable afterwards.
 */
ਖਾਲੀ Curl_req_free(ਬਣਤਰ SingleRequest *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Hard reset the state of the request to virgin state base on
 * transfer settings.
 */
ਖਾਲੀ Curl_req_hard_reset(ਬਣਤਰ SingleRequest *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Send request headers. If not all could be sent
 * they will be buffered. Use `Curl_req_flush()` to make sure
 * bytes are really send.
 * @param data      the transfer making the request
 * @param req       the complete header bytes, no body
 * @param httpversion version used in request (09, 10, 11, etc.)
 * @return CURLE_OK (on blocking with *pnwritten == 0) or error.
 */
CURLcode Curl_req_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ dynbuf *req,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ httpversion);

/**
 * TRUE iff the request has sent all request headers and data.
 */
bool Curl_req_ਪੂਰਾ_sending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/*
 * Read more from client and flush all buffered request bytes.
 * @return CURLE_OK on success or the error on the sending.
 *         Never returns CURLE_AGAIN.
 */
CURLcode Curl_req_send_ਹੋਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* TRUE if the request wants to send, e.g. is not done sending
 * and is not blocked. */
bool Curl_req_want_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* TRUE if the request wants to receive and is not blocked. */
bool Curl_req_want_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * TRUE iff the request has no buffered bytes yet to send.
 */
bool Curl_req_sendbuf_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Stop sending any more request data to the server.
 * Clear the send buffer and mark request sending as done.
 */
CURLcode Curl_req_abort_sending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Stop sending and receiving any more request data.
 * Abort sending if not done.
 */
CURLcode Curl_req_stop_send_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Invoked when all request data has been uploaded.
 */
CURLcode Curl_req_ਸੈੱਟ_upload_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* HEADER_CURL_REQUEST_H */
