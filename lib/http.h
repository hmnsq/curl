#अगर_अघोषित HEADER_CURL_HTTP_H
#घोषणा HEADER_CURL_HTTP_H
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

#समावेश "bufq.h"
#समावेश "dynhds.h"

प्रकार क्रमागत {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_शीर्ष
} Curl_HttpReq;

#घोषणा CURL_HTTP_V1x   (1 << 0)
#घोषणा CURL_HTTP_V2x   (1 << 1)
#घोषणा CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
प्रकार अचिन्हित अक्षर http_majors;

#अगर_अघोषित CURL_DISABLE_HTTP

बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_http;

काष्ठा dynhds;

काष्ठा http_negotiation {
  अचिन्हित अक्षर rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  BIT(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  BIT(h2_prior_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  BIT(accept_09); /* Accept an HTTP/0.9 response */
  BIT(only_10); /* When using major version 1x, use only 1.0 */
};

व्योम Curl_http_neg_init(काष्ठा Curl_easy *तथ्य, काष्ठा http_negotiation *neg);

CURLcode Curl_bump_headersize(काष्ठा Curl_easy *तथ्य,
                              माप_प्रकार delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(स्थिर अक्षर *headerline,  /* line to check */
                        स्थिर अक्षर *header,   /* header keyword _with_ colon */
                        स्थिर माप_प्रकार hlen,   /* len of the keyword in bytes */
                        स्थिर अक्षर *content, /* content string to find */
                        स्थिर माप_प्रकार clen);   /* len of the content in bytes */

अक्षर *Curl_copy_header_मूल्य(स्थिर अक्षर *header);

अक्षर *Curl_checkProxyheaders(काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा connectdata *conn,
                             स्थिर अक्षर *thisheader,
                             स्थिर माप_प्रकार thislen);

CURLcode Curl_add_timecondition(काष्ठा Curl_easy *तथ्य, काष्ठा dynbuf *req);
CURLcode Curl_add_custom_headers(काष्ठा Curl_easy *तथ्य, bool is_connect,
                                 पूर्णांक httpversion, काष्ठा dynbuf *req);

व्योम Curl_http_to_fold(काष्ठा dynbuf *bf);

व्योम Curl_http_method(काष्ठा Curl_easy *तथ्य,
                      स्थिर अक्षर **method, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_setup_conn(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn);
CURLcode Curl_http(काष्ठा Curl_easy *तथ्य, bool *done);
CURLcode Curl_http_done(काष्ठा Curl_easy *तथ्य,
                        CURLcode status, bool premature);
CURLcode Curl_http_doing_pollset(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा easy_pollset *ps);
CURLcode Curl_http_perform_pollset(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा easy_pollset *ps);
CURLcode Curl_http_write_resp(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *buf, माप_प्रकार blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *hd, माप_प्रकार hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_header(काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *hd, माप_प्रकार hdlen);

/* These functions are in http.c */
CURLcode Curl_http_निवेश_auth(काष्ठा Curl_easy *तथ्य, bool proxy,
                              स्थिर अक्षर *auth);

CURLcode Curl_http_auth_act(काष्ठा Curl_easy *तथ्य);

/* follow a redirect or not */
CURLcode Curl_http_follow(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#घोषणा CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#अगर_अघोषित MAX_INITIAL_POST_आकार
#घोषणा MAX_INITIAL_POST_आकार (64 * 1024)
#पूर्ण_अगर

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#अगर_अघोषित EXPECT_100_THRESHOLD
#घोषणा EXPECT_100_THRESHOLD (1024 * 1024)
#पूर्ण_अगर

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#घोषणा MAX_HTTP_RESP_HEADER_आकार (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#घोषणा MAX_HTTP_RESP_HEADER_गिनती 5000

#पूर्ण_अगर /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *buf, माप_प्रकार blen,
                                  माप_प्रकार *pconsumed);

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
CURLcode Curl_http_output_auth(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               स्थिर अक्षर *request,
                               Curl_HttpReq httpreq,
                               स्थिर अक्षर *path,
                               स्थिर अक्षर *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, स्थिर अक्षर *s, माप_प्रकार len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
काष्ठा httpreq {
  काष्ठा dynhds headers;
  काष्ठा dynhds trailers;
  अक्षर *scheme;
  अक्षर *authority;
  अक्षर *path;
  अक्षर method[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(काष्ठा httpreq **preq,
                            स्थिर अक्षर *method, माप_प्रकार m_len,
                            स्थिर अक्षर *scheme, माप_प्रकार s_len,
                            स्थिर अक्षर *authority, माप_प्रकार a_len,
                            स्थिर अक्षर *path, माप_प्रकार p_len);

CURLcode Curl_http_req_make2(काष्ठा httpreq **preq,
                             स्थिर अक्षर *method, माप_प्रकार m_len,
                             CURLU *url, स्थिर अक्षर *scheme_default);

व्योम Curl_http_req_free(काष्ठा httpreq *req);

#घोषणा HTTP_PSEUDO_METHOD    ":method"
#घोषणा HTTP_PSEUDO_SCHEME    ":scheme"
#घोषणा HTTP_PSEUDO_AUTHORITY ":authority"
#घोषणा HTTP_PSEUDO_PATH      ":path"
#घोषणा HTTP_PSEUDO_STATUS    ":status"

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
CURLcode Curl_http_req_to_h2(काष्ठा dynhds *h2_headers,
                             काष्ठा httpreq *req, काष्ठा Curl_easy *तथ्य);

/**
 * All about a core HTTP response, excluding body and trailers
 */
काष्ठा http_resp {
  पूर्णांक status;
  अक्षर *description;
  काष्ठा dynhds headers;
  काष्ठा dynhds trailers;
  काष्ठा http_resp *पूर्व;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(काष्ठा http_resp **presp,
                             पूर्णांक status,
                             स्थिर अक्षर *description);

व्योम Curl_http_resp_free(काष्ठा http_resp *resp);

#पूर्ण_अगर /* HEADER_CURL_HTTP_H */
