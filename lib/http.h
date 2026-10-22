#ifndef HEADER_CURL_HTTP_H
#define HEADER_CURL_HTTP_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "dynhds.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_HEAD
} Curl_HttpReq;

#define CURL_HTTP_V1x   (1 << 0)
#define CURL_HTTP_V2x   (1 << 1)
#define CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ವರ್ಣ http_majors;

#ifndef CURL_DISABLE_HTTP

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_http;

ಸಂರಚನಾ dynhds;

ಸಂರಚನಾ http_negotiation {
  unsigned ವರ್ಣ rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  BIT(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  BIT(h2_prior_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  BIT(accept_09); /* Accept an HTTP/0.9 response */
  BIT(only_10); /* When using major version 1x, use only 1.0 */
};

ಶೂನ್ಯ Curl_http_neg_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ http_negotiation *neg);

CURLcode Curl_bump_headersize(ಸಂರಚನಾ Curl_easy *data,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(ಧ್ರುವ ವರ್ಣ *headerline,  /* line to check */
                        ಧ್ರುವ ವರ್ಣ *header,   /* header keyword _with_ colon */
                        ಧ್ರುವ size_t hlen,   /* len of the keyword in bytes */
                        ಧ್ರುವ ವರ್ಣ *content, /* content string to find */
                        ಧ್ರುವ size_t clen);   /* len of the content in bytes */

ವರ್ಣ *Curl_copy_header_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *header);

ವರ್ಣ *Curl_checkProxyheaders(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ connectdata *conn,
                             ಧ್ರುವ ವರ್ಣ *thisheader,
                             ಧ್ರುವ size_t thislen);

CURLcode Curl_add_timecondition(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ dynbuf *req);
CURLcode Curl_add_custom_headers(ಸಂರಚನಾ Curl_easy *data, bool is_connect,
                                 ಪೂರ್ಣಾಙ್ಕ httpversion, ಸಂರಚನಾ dynbuf *req);

ಶೂನ್ಯ Curl_http_to_fold(ಸಂರಚನಾ dynbuf *bf);

ಶೂನ್ಯ Curl_http_method(ಸಂರಚನಾ Curl_easy *data,
                      ಧ್ರುವ ವರ್ಣ **method, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_setup_conn(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn);
CURLcode Curl_http(ಸಂರಚನಾ Curl_easy *data, bool *done);
CURLcode Curl_http_done(ಸಂರಚನಾ Curl_easy *data,
                        CURLcode status, bool premature);
CURLcode Curl_http_doing_pollset(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ easy_pollset *ps);
CURLcode Curl_http_perform_pollset(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ easy_pollset *ps);
CURLcode Curl_http_write_resp(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_header(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_ಒಳಹರಿವು_auth(ಸಂರಚನಾ Curl_easy *data, bool proxy,
                              ಧ್ರುವ ವರ್ಣ *auth);

CURLcode Curl_http_auth_act(ಸಂರಚನಾ Curl_easy *data);

/* follow a redirect or not */
CURLcode Curl_http_follow(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#define CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#ifndef MAX_INITIAL_POST_SIZE
#define MAX_INITIAL_POST_SIZE (64 * 1024)
#endif

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#ifndef EXPECT_100_THRESHOLD
#define EXPECT_100_THRESHOLD (1024 * 1024)
#endif

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#define MAX_HTTP_RESP_HEADER_SIZE (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#define MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ 5000

#endif /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                  size_t *pconsumed);

/**
 * Curl_http_output_auth() setups the authentication headers for the
 * host/proxy and the correct authentication
 * method. data->state.authdone is set to TRUE when authentication is
 * done.
 *
 * @param data all information about the current transfer
 * @param conn all information about the current connection
 * @param request pointer to the request keyword
 * @param httpreq is the request type
 * @param path pointer to the requested path
 * @param query pointer to the requested query or NULL
 * @param is_connect boolean if this is a CONNECT request
 *        (where httpreq is HTTPREQ_GET since there is no HTTPREQ_CONNECT)
 *
 * @returns CURLcode
 */
CURLcode Curl_http_output_auth(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               ಧ್ರುವ ವರ್ಣ *request,
                               Curl_HttpReq httpreq,
                               ಧ್ರುವ ವರ್ಣ *path,
                               ಧ್ರುವ ವರ್ಣ *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ಧ್ರುವ ವರ್ಣ *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
ಸಂರಚನಾ httpreq {
  ಸಂರಚನಾ dynhds headers;
  ಸಂರಚನಾ dynhds trailers;
  ವರ್ಣ *scheme;
  ವರ್ಣ *authority;
  ವರ್ಣ *path;
  ವರ್ಣ method[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(ಸಂರಚನಾ httpreq **preq,
                            ಧ್ರುವ ವರ್ಣ *method, size_t m_len,
                            ಧ್ರುವ ವರ್ಣ *scheme, size_t s_len,
                            ಧ್ರುವ ವರ್ಣ *authority, size_t a_len,
                            ಧ್ರುವ ವರ್ಣ *path, size_t p_len);

CURLcode Curl_http_req_make2(ಸಂರಚನಾ httpreq **preq,
                             ಧ್ರುವ ವರ್ಣ *method, size_t m_len,
                             CURLU *url, ಧ್ರುವ ವರ್ಣ *scheme_default);

ಶೂನ್ಯ Curl_http_req_free(ಸಂರಚನಾ httpreq *req);

#define HTTP_PSEUDO_METHOD    ":method"
#define HTTP_PSEUDO_SCHEME    ":scheme"
#define HTTP_PSEUDO_AUTHORITY ":authority"
#define HTTP_PSEUDO_PATH      ":path"
#define HTTP_PSEUDO_STATUS    ":status"

/**
 * Create the list of HTTP/2 headers which represent the request,
 * using HTTP/2 pseudo headers preceding the `req->headers`.
 *
 * Applies the following transformations:
 * - if `authority` is set, any "Host" header is removed.
 * - if `authority` is unset and a "Host" header is present, use
 *   that as `authority` and remove "Host"
 * - removes and Connection header fields as defined in rfc9113 ch. 8.2.2
 * - lower-cases the header field names
 *
 * @param h2_headers will contain the HTTP/2 headers on success
 * @param req        the request to transform
 * @param data       the handle to lookup defaults like ' :scheme' from
 */
CURLcode Curl_http_req_to_h2(ಸಂರಚನಾ dynhds *h2_headers,
                             ಸಂರಚನಾ httpreq *req, ಸಂರಚನಾ Curl_easy *data);

/**
 * All about a core HTTP response, excluding body and trailers
 */
ಸಂರಚನಾ http_resp {
  ಪೂರ್ಣಾಙ್ಕ status;
  ವರ್ಣ *description;
  ಸಂರಚನಾ dynhds headers;
  ಸಂರಚನಾ dynhds trailers;
  ಸಂರಚನಾ http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(ಸಂರಚನಾ http_resp **presp,
                             ಪೂರ್ಣಾಙ್ಕ status,
                             ಧ್ರುವ ವರ್ಣ *description);

ಶೂನ್ಯ Curl_http_resp_free(ಸಂರಚನಾ http_resp *resp);

#endif /* HEADER_CURL_HTTP_H */
