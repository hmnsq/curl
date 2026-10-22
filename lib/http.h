#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTP_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dynhds.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_ਸਿਰ
} Curl_HttpReq;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HTTP_V1x   (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HTTP_V2x   (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ http_majors;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_http;

ਬਣਤਰ dynhds;

ਬਣਤਰ http_negotiation {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(h2_ਪੂਰਵ_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  ਦੋਆਧਾਰੀ_ਅੰਕ(accept_09); /* Accept an HTTP/0.9 response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(only_10); /* When using major version 1x, use only 1.0 */
};

ਖਾਲੀ Curl_http_neg_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ http_negotiation *neg);

CURLcode Curl_bump_headersize(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(ਅਚਲ ਅੱਖਰ *headerline,  /* line to check */
                        ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ,   /* header keyword _with_ colon */
                        ਅਚਲ size_t hlen,   /* len of the keyword in bytes */
                        ਅਚਲ ਅੱਖਰ *content, /* content string to find */
                        ਅਚਲ size_t clen);   /* len of the content in bytes */

ਅੱਖਰ *Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ);

ਅੱਖਰ *Curl_checkProxyheaders(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ connectdata *conn,
                             ਅਚਲ ਅੱਖਰ *thisheader,
                             ਅਚਲ size_t thislen);

CURLcode Curl_ਜੋੜੋ_timecondition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ dynbuf *req);
CURLcode Curl_ਜੋੜੋ_custom_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool is_connect,
                                 ਪੂਰਨ httpversion, ਬਣਤਰ dynbuf *req);

ਖਾਲੀ Curl_http_ਮੰਜ਼ਿਲ_fold(ਬਣਤਰ dynbuf *bf);

ਖਾਲੀ Curl_http_ਵਿਧੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਅਚਲ ਅੱਖਰ **ਵਿਧੀ, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_ਤਿਆਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn);
CURLcode Curl_http(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);
CURLcode Curl_http_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        CURLcode ਸਥਿਤੀ, bool premature);
CURLcode Curl_http_doing_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ easy_pollset *ps);
CURLcode Curl_http_perform_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ easy_pollset *ps);
CURLcode Curl_http_write_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_ਨਿਵੇਸ਼_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool proxy,
                              ਅਚਲ ਅੱਖਰ *auth);

CURLcode Curl_http_auth_act(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* follow a redirect or not */
CURLcode Curl_http_follow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAX_INITIAL_POST_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_INITIAL_POST_ਮਾਪ (64 * 1024)
#ਜੇ_ਅੰਤ

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EXPECT_100_ਹੱਦ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPECT_100_ਹੱਦ (1024 * 1024)
#ਜੇ_ਅੰਤ

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ 5000

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *buf, size_t blen,
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
CURLcode Curl_http_output_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               ਅਚਲ ਅੱਖਰ *request,
                               Curl_HttpReq httpreq,
                               ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                               ਅਚਲ ਅੱਖਰ *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_ਸਥਿਤੀ(int32_t *pstatus, ਅਚਲ ਅੱਖਰ *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
ਬਣਤਰ httpreq {
  ਬਣਤਰ dynhds ਸਿਰਲੇਖਾਂ;
  ਬਣਤਰ dynhds trailers;
  ਅੱਖਰ *scheme;
  ਅੱਖਰ *authority;
  ਅੱਖਰ *ਰਸਤਾ;
  ਅੱਖਰ ਵਿਧੀ[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(ਬਣਤਰ httpreq **preq,
                            ਅਚਲ ਅੱਖਰ *ਵਿਧੀ, size_t m_len,
                            ਅਚਲ ਅੱਖਰ *scheme, size_t s_len,
                            ਅਚਲ ਅੱਖਰ *authority, size_t a_len,
                            ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, size_t p_len);

CURLcode Curl_http_req_make2(ਬਣਤਰ httpreq **preq,
                             ਅਚਲ ਅੱਖਰ *ਵਿਧੀ, size_t m_len,
                             CURLU *url, ਅਚਲ ਅੱਖਰ *scheme_default);

ਖਾਲੀ Curl_http_req_free(ਬਣਤਰ httpreq *req);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_PSEUDO_ਵਿਧੀ    ":method"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_PSEUDO_SCHEME    ":scheme"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_PSEUDO_AUTHORITY ":authority"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_PSEUDO_ਰਸਤਾ      ":path"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP_PSEUDO_ਸਥਿਤੀ    ":status"

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
CURLcode Curl_http_req_ਮੰਜ਼ਿਲ_h2(ਬਣਤਰ dynhds *h2_ਸਿਰਲੇਖਾਂ,
                             ਬਣਤਰ httpreq *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * All about a core HTTP response, excluding body and trailers
 */
ਬਣਤਰ http_resp {
  ਪੂਰਨ ਸਥਿਤੀ;
  ਅੱਖਰ *description;
  ਬਣਤਰ dynhds ਸਿਰਲੇਖਾਂ;
  ਬਣਤਰ dynhds trailers;
  ਬਣਤਰ http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(ਬਣਤਰ http_resp **presp,
                             ਪੂਰਨ ਸਥਿਤੀ,
                             ਅਚਲ ਅੱਖਰ *description);

ਖਾਲੀ Curl_http_resp_free(ਬਣਤਰ http_resp *resp);

#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTP_H */
