#నిర్వచితం_కాకపోతే శీర్షిక_CURL_REQUEST_H
#నిర్వచించు శీర్షిక_CURL_REQUEST_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "bufq.h"

/* forward declarations */
నిర్మాణం UserDefined;

/* Bits on the io_flags member of SingleRequest */
#నిర్వచించు REQ_IO_RECV       (1 << 0) /* there is or may be data to read */
#నిర్వచించు REQ_IO_SEND       (1 << 1) /* there is or may be data to write */

/* Low level request receive/send io_flags checks. */
#నిర్వచించు CURL_REQ_WANT_SEND(d)  ((d)->req.io_flags & REQ_IO_SEND)
#నిర్వచించు CURL_REQ_WANT_RECV(d)  ((d)->req.io_flags & REQ_IO_RECV)
#నిర్వచించు CURL_REQ_WANT_IO(d)    \
  ((d)->req.io_flags & (REQ_IO_RECV | REQ_IO_SEND))
/* Low level request receive/send io_flags manipulations. */
#నిర్వచించు CURL_REQ_సమితి_SEND(d)   ((d)->req.io_flags |= REQ_IO_SEND)
#నిర్వచించు CURL_REQ_సమితి_RECV(d)   ((d)->req.io_flags |= REQ_IO_RECV)
#నిర్వచించు CURL_REQ_CLEAR_SEND(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_SEND)
#నిర్వచించు CURL_REQ_CLEAR_RECV(d) \
  ((d)->req.io_flags &= (uint8_t)~REQ_IO_RECV)
#నిర్వచించు CURL_REQ_CLEAR_IO(d)  \
  ((d)->req.io_flags &= (uint8_t)~(REQ_IO_RECV | REQ_IO_SEND))

గణన expect100 {
  EXP100_SEND_దత్తాంశం,           /* enough waiting, send the body now */
  EXP100_AWAITING_CONTINUE,   /* waiting for the 100 Continue header */
  EXP100_SENDING_REQUEST,     /* still sending the request but will wait for
                                 the 100 header once done with the request */
  EXP100_FAILED               /* used on 417 Expectation Failed */
};

గణన upgrade101 {
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
నిర్మాణం SingleRequest {
  curl_off_t కొలత;        /* -1 if unknown at this point */
  curl_off_t maxdownload; /* in bytes, the maximum amount of data to fetch,
                             -1 means unlimited */
  curl_off_t bytecount;         /* total number of bytes read */
  curl_off_t writebytecount;    /* number of bytes written */
  curl_off_t స్థానభ్రంశం;            /* possible resume offset read from the
                                   Content-Range: header */

  నిర్మాణం curltime ప్రారంభం;         /* transfer started at this time */
  గుర్తులేని పూర్ణాంకం headerbytecount;  /* received server headers (not CONNECT
                                    headers) */
  గుర్తులేని పూర్ణాంకం allheadercount;   /* all received headers (server + CONNECT) */
  గుర్తులేని పూర్ణాంకం deductheadercount; /* this amount of bytes does not count when
                                     we check if anything has been transferred
                                     at the end of a connection. We use this
                                     counter to make only a 100 reply (without
                                     a following second response code) result
                                     in a CURLE_GOT_NOTHING error code */
  పూర్ణాంకం headerline;               /* counts header lines to better track the
                                   first one */
  పూర్ణాంకం httpcode;                 /* error code from the 'HTTP/1.? XXX' or
                                   'RTSP/1.? XXX' line */
  uint8_t httpversion_పంపిన;     /* Version in request (09, 10, 11, etc.) */
  uint8_t httpversion;          /* Version in response (09, 10, 11, etc.) */
  uint8_t upgr101;              /* 101 upgrade state */
  uint8_t io_flags;             /* REQ_IO_RECV | REQ_IO_SEND */

  /* Client Writer stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  నిర్మాణం {
    నిర్మాణం Curl_cwriter *దొంతర;
    బిట్టు(paused);
  } writer;
  /* Client Reader stack, handles transfer- and content-encodings, protocol
   * checks, pausing by client callbacks. */
  నిర్మాణం {
    నిర్మాణం Curl_creader *దొంతర;
  } reader;
  నిర్మాణం bufq sendbuf; /* data which needs to be send to the server */
  size_t sendbuf_hds_len; /* amount of header bytes in sendbuf */
  time_t timeofdoc;
  అక్షరం *location;   /* This points to an allocated version of the Location:
                       header data */
  అక్షరం *newurl;     /* Set to the new URL to use when a redirect or a retry is
                       wanted */

  అక్షరం *hd_auth;      /* Authorization header, full HTTP/1.x line */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  అక్షరం *hd_proxy_auth; /* Proxy-Authorization header, full HTTP/1.x line */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  అక్షరం *cookiehost;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  గుర్తులేని అక్షరం setcookies;
#అయితే_ముగింపు
  బిట్టు(శీర్షిక);        /* incoming data has HTTP header */
  బిట్టు(పూర్తి);          /* request is done, e.g. no more send/recv should
                       * happen. This can be TRUE before `upload_done` or
                       * `download_done` is TRUE. */
  బిట్టు(content_range); /* set TRUE if Content-Range: was found */
  బిట్టు(download_పూర్తి); /* set to TRUE when download is complete */
  బిట్టు(eos_written);   /* iff EOS has been written to client */
  బిట్టు(eos_read);      /* iff EOS has been read from the client */
  బిట్టు(eos_పంపిన);      /* iff EOS has been sent to the server */
  బిట్టు(rewind_read);   /* iff reader needs rewind at next start */
  బిట్టు(upload_పూర్తి);   /* set to TRUE when all request data has been sent */
  బిట్టు(upload_aborted); /* set to TRUE when upload was aborted. Also
                        * shows `upload_done` as TRUE. */
  బిట్టు(ignorebody);    /* we read a response-body but we ignore it! */
  బిట్టు(http_bodyless); /* HTTP response status code is between 100 and 199,
                         204 or 304 */
  బిట్టు(chunk);         /* if set, this is a chunked transfer-encoding */
  బిట్టు(resp_trailer);  /* response carried 'Trailer:' header field */
  బిట్టు(ignore_cl);     /* ignore content-length */
  బిట్టు(upload_chunky); /* set TRUE if we are doing chunked transfer-encoding
                         on upload */
  బిట్టు(no_body);      /* the response has no body */
  బిట్టు(authneg);      /* TRUE when the auth phase has started, which means
                        that we are creating a request with an auth header,
                        but it is not the final request in the auth
                        negotiation. */
  బిట్టు(sendbuf_init); /* sendbuf is initialized */
  బిట్టు(shutdown);     /* request end will shutdown connection */
  బిట్టు(shutdown_err_ignore); /* errors in shutdown will not fail request */
  బిట్టు(reader_started); /* client reads have started */
};

/**
 * Initialize the state of the request for first use.
 */
శూన్య Curl_req_init(నిర్మాణం SingleRequest *req);

/**
 * The request is about to start. Record time and do a soft reset.
 */
CURLcode Curl_req_ప్రారంభం(నిర్మాణం SingleRequest *req,
                        నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * The request may continue with a follow up. Reset
 * members, but keep start time for overall duration calc.
 */
CURLcode Curl_req_soft_reset(నిర్మాణం SingleRequest *req,
                             నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * The request is done. If not aborted, make sure that buffers are
 * flushed to the client.
 * @param req        the request
 * @param data       the transfer
 * @param aborted    TRUE iff the request was aborted/errored
 */
CURLcode Curl_req_పూర్తి(నిర్మాణం SingleRequest *req,
                       నిర్మాణం Curl_easy *దత్తాంశం, bool aborted);

/**
 * Free the state of the request, not usable afterwards.
 */
శూన్య Curl_req_free(నిర్మాణం SingleRequest *req, నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Hard reset the state of the request to virgin state base on
 * transfer settings.
 */
శూన్య Curl_req_hard_reset(నిర్మాణం SingleRequest *req, నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Send request headers. If not all could be sent
 * they will be buffered. Use `Curl_req_flush()` to make sure
 * bytes are really send.
 * @param data      the transfer making the request
 * @param req       the complete header bytes, no body
 * @param httpversion version used in request (09, 10, 11, etc.)
 * @return CURLE_OK (on blocking with *pnwritten == 0) or error.
 */
CURLcode Curl_req_send(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *req,
                       గుర్తులేని అక్షరం httpversion);

/**
 * TRUE iff the request has sent all request headers and data.
 */
bool Curl_req_పూర్తి_sending(నిర్మాణం Curl_easy *దత్తాంశం);

/*
 * Read more from client and flush all buffered request bytes.
 * @return CURLE_OK on success or the error on the sending.
 *         Never returns CURLE_AGAIN.
 */
CURLcode Curl_req_send_మరిన్ని(నిర్మాణం Curl_easy *దత్తాంశం);

/* TRUE if the request wants to send, e.g. is not done sending
 * and is not blocked. */
bool Curl_req_want_send(నిర్మాణం Curl_easy *దత్తాంశం);

/* TRUE if the request wants to receive and is not blocked. */
bool Curl_req_want_recv(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * TRUE iff the request has no buffered bytes yet to send.
 */
bool Curl_req_sendbuf_ఖాళీలు(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Stop sending any more request data to the server.
 * Clear the send buffer and mark request sending as done.
 */
CURLcode Curl_req_abort_sending(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Stop sending and receiving any more request data.
 * Abort sending if not done.
 */
CURLcode Curl_req_stop_send_recv(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Invoked when all request data has been uploaded.
 */
CURLcode Curl_req_సమితి_upload_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* HEADER_CURL_REQUEST_H */
