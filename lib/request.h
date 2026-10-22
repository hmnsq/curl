#اگر_تعریف_نشده سرآیند_CURL_REQUEST_H
#تعریف_کن سرآیند_CURL_REQUEST_H
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
#ضمن "curl_setup.h"

#ضمن "bufq.h"

/* forward declarations */
بنية UserDefined;

/* Bits on the io_flags member of SingleRequest */
#تعریف_کن REQ_IO_RECV       (1 << 0) /* there is or may be data to read */
#تعریف_کن REQ_IO_SEND       (1 << 1) /* there is or may be data to write */

/* Low level request receive/send io_flags checks. */
#تعریف_کن CURL_REQ_WANT_SEND(d)  ((d)->req.io_flags & REQ_IO_SEND)
#تعریف_کن CURL_REQ_WANT_RECV(d)  ((d)->req.io_flags & REQ_IO_RECV)
#تعریف_کن CURL_REQ_WANT_IO(d)    \
  ((d)->req.io_flags & (REQ_IO_RECV | REQ_IO_SEND))
/* Low level request receive/send io_flags manipulations. */
#تعریف_کن CURL_REQ_مجموعه_SEND(d)   ((d)->req.io_flags |= REQ_IO_SEND)
#تعریف_کن CURL_REQ_مجموعه_RECV(d)   ((d)->req.io_flags |= REQ_IO_RECV)
#تعریف_کن CURL_REQ_CLEAR_SEND(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_SEND)
#تعریف_کن CURL_REQ_CLEAR_RECV(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_RECV)
#تعریف_کن CURL_REQ_CLEAR_IO(d)  \
  ((d)->req.io_flags &= (uint8_t)~(REQ_IO_RECV | REQ_IO_SEND))

تعداد expect100 {
  EXP100_SEND_داده,           /* enough waiting, send the body now */
  EXP100_AWAITING_CONTINUE,   /* waiting for the 100 Continue header */
  EXP100_SENDING_REQUEST,     /* still sending the request but will wait for
                                 the 100 header once done with the request */
  EXP100_FAILED               /* used on 417 Expectation Failed */
};

تعداد upgrade101 {
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
بنية SingleRequest {
  curl_off_t اندازه;        /* -1 if unknown at this point */
  curl_off_t maxdownload; /* in bytes, the maximum amount of data to fetch,
                             -1 means unlimited */
  curl_off_t bytecount;         /* total number of bytes read */
  curl_off_t writebytecount;    /* number of bytes written */
  curl_off_t جابه_جایی;            /* possible resume offset read from the
                                   Content-Range: header */

  بنية curltime شروع;         /* transfer started at this time */
  بیعلامت صحيح headerbytecount;  /* received server headers (not CONNECT
                                    headers) */
  بیعلامت صحيح allheadercount;   /* all received headers (server + CONNECT) */
  بیعلامت صحيح deductheadercount; /* this amount of bytes does not count when
                                     we check if anything has been transferred
                                     at the end of a connection. We use this
                                     counter to make only a 100 reply (without
                                     a following second response code) result
                                     in a CURLE_GOT_NOTHING error code */
  صحيح headerline;               /* counts header lines to better track the
                                   first one */
  صحيح httpcode;                 /* error code from the 'HTTP/1.? XXX' or
                                   'RTSP/1.? XXX' line */
  uint8_t httpversion_فرستاده;     /* Version in request (09, 10, 11, etc.) */
  uint8_t httpversion;          /* Version in response (09, 10, 11, etc.) */
  uint8_t upgr101;              /* 101 upgrade state */
  uint8_t io_flags;             /* REQ_IO_RECV | REQ_IO_SEND */

  /* Client Writer stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  بنية {
    بنية Curl_cwriter *پشته;
    بیت(paused);
  } writer;
  /* Client Reader stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  بنية {
    بنية Curl_creader *پشته;
  } reader;
  بنية bufq sendbuf; /* data which needs to be send to the server */
  size_t sendbuf_hds_len; /* amount of header bytes in sendbuf */
  time_t timeofdoc;
  حرف *location;   /* This points to an allocated version of the Location:
                       header data */
  حرف *newurl;     /* Set to the new URL to use when a redirect or a retry is
                       wanted */

  حرف *hd_auth;      /* Authorization header, full HTTP/1.x line */
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  حرف *hd_proxy_auth; /* Proxy-Authorization header, full HTTP/1.x line */
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  حرف *cookiehost;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  بیعلامت حرف setcookies;
#پایان_اگر
  بیت(سرآیند);        /* incoming data has HTTP header */
  بیت(انجامشده);          /* request is done, e.g. no more send/recv should
                       * happen. This can be TRUE before `upload_done` or
                       * `download_done` is TRUE. */
  بیت(content_range); /* set TRUE if Content-Range: was found */
  بیت(download_انجامشده); /* set to TRUE when download is complete */
  بیت(eos_written);   /* iff EOS has been written to client */
  بیت(eos_read);      /* iff EOS has been read from the client */
  بیت(eos_فرستاده);      /* iff EOS has been sent to the server */
  بیت(rewind_read);   /* iff reader needs rewind at next start */
  بیت(upload_انجامشده);   /* set to TRUE when all request data has been sent */
  بیت(upload_aborted); /* set to TRUE when upload was aborted. Also
                        * shows `upload_done` as TRUE. */
  بیت(ignorebody);    /* we read a response-body but we ignore it! */
  بیت(http_bodyless); /* HTTP response status code is between 100 and 199,
                         204 or 304 */
  بیت(chunk);         /* if set, this is a chunked transfer-encoding */
  بیت(resp_trailer);  /* response carried 'Trailer:' header field */
  بیت(ignore_cl);     /* ignore content-length */
  بیت(upload_chunky); /* set TRUE if we are doing chunked transfer-encoding
                         on upload */
  بیت(no_body);      /* the response has no body */
  بیت(authneg);      /* TRUE when the auth phase has started, which means
                        that we are creating a request with an auth header,
                        but it is not the final request in the auth
                        negotiation. */
  بیت(sendbuf_init); /* sendbuf is initialized */
  بیت(shutdown);     /* request end will shutdown connection */
  بیت(shutdown_err_ignore); /* errors in shutdown will not fail request */
  بیت(reader_started); /* client reads have started */
};

/**
 * Initialize the state of the request for first use.
 */
فراغ Curl_req_init(بنية SingleRequest *req);

/**
 * The request is about to start. Record time and do a soft reset.
 */
CURLcode Curl_req_شروع(بنية SingleRequest *req,
                        بنية Curl_easy *داده);

/**
 * The request may continue with a follow up. Reset
 * members, but keep start time for overall duration calc.
 */
CURLcode Curl_req_soft_reset(بنية SingleRequest *req,
                             بنية Curl_easy *داده);

/**
 * The request is done. If not aborted, make sure that buffers are
 * flushed to the client.
 * @param req        the request
 * @param data       the transfer
 * @param aborted    TRUE iff the request was aborted/errored
 */
CURLcode Curl_req_انجامشده(بنية SingleRequest *req,
                       بنية Curl_easy *داده, bool aborted);

/**
 * Free the state of the request, not usable afterwards.
 */
فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *داده);

/**
 * Hard reset the state of the request to virgin state base on
 * transfer settings.
 */
فراغ Curl_req_hard_reset(بنية SingleRequest *req, بنية Curl_easy *داده);

/**
 * Send request headers. If not all could be sent
 * they will be buffered. Use `Curl_req_flush()` to make sure
 * bytes are really send.
 * @param data      the transfer making the request
 * @param req       the complete header bytes, no body
 * @param httpversion version used in request (09, 10, 11, etc.)
 * @return CURLE_OK (on blocking with *pnwritten == 0) or error.
 */
CURLcode Curl_req_send(بنية Curl_easy *داده, بنية dynbuf *req,
                       بیعلامت حرف httpversion);

/**
 * TRUE iff the request has sent all request headers and data.
 */
bool Curl_req_انجامشده_sending(بنية Curl_easy *داده);

/*
 * Read more from client and flush all buffered request bytes.
 * @return CURLE_OK on success or the error on the sending.
 *         Never returns CURLE_AGAIN.
 */
CURLcode Curl_req_send_بیشتر(بنية Curl_easy *داده);

/* TRUE if the request wants to send, e.g. is not done sending
 * and is not blocked. */
bool Curl_req_want_send(بنية Curl_easy *داده);

/* TRUE if the request wants to receive and is not blocked. */
bool Curl_req_want_recv(بنية Curl_easy *داده);

/**
 * TRUE iff the request has no buffered bytes yet to send.
 */
bool Curl_req_sendbuf_خالی(بنية Curl_easy *داده);

/**
 * Stop sending any more request data to the server.
 * Clear the send buffer and mark request sending as done.
 */
CURLcode Curl_req_abort_sending(بنية Curl_easy *داده);

/**
 * Stop sending and receiving any more request data.
 * Abort sending if not done.
 */
CURLcode Curl_req_stop_send_recv(بنية Curl_easy *داده);

/**
 * Invoked when all request data has been uploaded.
 */
CURLcode Curl_req_مجموعه_upload_انجامشده(بنية Curl_easy *داده);

#پایان_اگر /* HEADER_CURL_REQUEST_H */
