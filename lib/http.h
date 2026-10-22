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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "dynhds.h"

வகைவரை எண்ணிக்கை {
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
வகைவரை unsigned எழுத்து http_majors;

#ifndef CURL_DISABLE_HTTP

வெளி மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_http;

கட்டமைப்பு dynhds;

கட்டமைப்பு http_negotiation {
  unsigned எழுத்து rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  BIT(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  BIT(h2_prior_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  BIT(accept_09); /* Accept an HTTP/0.9 response */
  BIT(only_10); /* When using major version 1x, use only 1.0 */
};

வெறுமை Curl_http_neg_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு http_negotiation *neg);

CURLcode Curl_bump_headersize(கட்டமைப்பு Curl_easy *data,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(மாறிலி எழுத்து *headerline,  /* line to check */
                        மாறிலி எழுத்து *header,   /* header keyword _with_ colon */
                        மாறிலி size_t hlen,   /* len of the keyword in bytes */
                        மாறிலி எழுத்து *content, /* content string to find */
                        மாறிலி size_t clen);   /* len of the content in bytes */

எழுத்து *Curl_copy_header_மதிப்பு(மாறிலி எழுத்து *header);

எழுத்து *Curl_checkProxyheaders(கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு connectdata *conn,
                             மாறிலி எழுத்து *thisheader,
                             மாறிலி size_t thislen);

CURLcode Curl_add_timecondition(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு dynbuf *req);
CURLcode Curl_add_custom_headers(கட்டமைப்பு Curl_easy *data, bool is_connect,
                                 முழு httpversion, கட்டமைப்பு dynbuf *req);

வெறுமை Curl_http_to_fold(கட்டமைப்பு dynbuf *bf);

வெறுமை Curl_http_method(கட்டமைப்பு Curl_easy *data,
                      மாறிலி எழுத்து **method, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_setup_conn(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn);
CURLcode Curl_http(கட்டமைப்பு Curl_easy *data, bool *done);
CURLcode Curl_http_done(கட்டமைப்பு Curl_easy *data,
                        CURLcode status, bool premature);
CURLcode Curl_http_doing_pollset(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு easy_pollset *ps);
CURLcode Curl_http_perform_pollset(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு easy_pollset *ps);
CURLcode Curl_http_write_resp(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_header(கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_உள்ளீடு_auth(கட்டமைப்பு Curl_easy *data, bool proxy,
                              மாறிலி எழுத்து *auth);

CURLcode Curl_http_auth_act(கட்டமைப்பு Curl_easy *data);

/* follow a redirect or not */
CURLcode Curl_http_follow(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *newurl,
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
#define MAX_HTTP_RESP_HEADER_மொத்தம் 5000

#endif /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *buf, size_t blen,
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
CURLcode Curl_http_output_auth(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               மாறிலி எழுத்து *request,
                               Curl_HttpReq httpreq,
                               மாறிலி எழுத்து *path,
                               மாறிலி எழுத்து *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, மாறிலி எழுத்து *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
கட்டமைப்பு httpreq {
  கட்டமைப்பு dynhds headers;
  கட்டமைப்பு dynhds trailers;
  எழுத்து *scheme;
  எழுத்து *authority;
  எழுத்து *path;
  எழுத்து method[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(கட்டமைப்பு httpreq **preq,
                            மாறிலி எழுத்து *method, size_t m_len,
                            மாறிலி எழுத்து *scheme, size_t s_len,
                            மாறிலி எழுத்து *authority, size_t a_len,
                            மாறிலி எழுத்து *path, size_t p_len);

CURLcode Curl_http_req_make2(கட்டமைப்பு httpreq **preq,
                             மாறிலி எழுத்து *method, size_t m_len,
                             CURLU *url, மாறிலி எழுத்து *scheme_default);

வெறுமை Curl_http_req_free(கட்டமைப்பு httpreq *req);

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
CURLcode Curl_http_req_to_h2(கட்டமைப்பு dynhds *h2_headers,
                             கட்டமைப்பு httpreq *req, கட்டமைப்பு Curl_easy *data);

/**
 * All about a core HTTP response, excluding body and trailers
 */
கட்டமைப்பு http_resp {
  முழு status;
  எழுத்து *description;
  கட்டமைப்பு dynhds headers;
  கட்டமைப்பு dynhds trailers;
  கட்டமைப்பு http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(கட்டமைப்பு http_resp **presp,
                             முழு status,
                             மாறிலி எழுத்து *description);

வெறுமை Curl_http_resp_free(கட்டமைப்பு http_resp *resp);

#endif /* HEADER_CURL_HTTP_H */
