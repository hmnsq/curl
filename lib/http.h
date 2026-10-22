#جے_بنا_تعریف سرخی_CURL_HTTP_H
#تعریف_کرو سرخی_CURL_HTTP_H
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
#ضمن "curl_setup.h"

#ضمن "bufq.h"
#ضمن "dynhds.h"

تعريفنوع تعداد {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_سر
} Curl_HttpReq;

#تعریف_کرو CURL_HTTP_V1x   (1 << 0)
#تعریف_کرو CURL_HTTP_V2x   (1 << 1)
#تعریف_کرو CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
تعريفنوع بنانشانی حرف http_majors;

#جے_بنا_تعریف CURL_DISABLE_HTTP

خارجي ثابت بنية Curl_protocol Curl_protocol_http;

بنية dynhds;

بنية http_negotiation {
  بنانشانی حرف rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  ثنائی_ہندسہ(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  ثنائی_ہندسہ(h2_پہلوں_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  ثنائی_ہندسہ(accept_09); /* Accept an HTTP/0.9 response */
  ثنائی_ہندسہ(only_10); /* When using major version 1x, use only 1.0 */
};

فراغ Curl_http_neg_init(بنية Curl_easy *انکڑے, بنية http_negotiation *neg);

CURLcode Curl_bump_headersize(بنية Curl_easy *انکڑے,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(ثابت حرف *headerline,  /* line to check */
                        ثابت حرف *سرخی,   /* header keyword _with_ colon */
                        ثابت size_t hlen,   /* len of the keyword in bytes */
                        ثابت حرف *content, /* content string to find */
                        ثابت size_t clen);   /* len of the content in bytes */

حرف *Curl_copy_سرخی_قدر(ثابت حرف *سرخی);

حرف *Curl_checkProxyheaders(بنية Curl_easy *انکڑے,
                             ثابت بنية connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen);

CURLcode Curl_جوڑو_timecondition(بنية Curl_easy *انکڑے, بنية dynbuf *req);
CURLcode Curl_جوڑو_custom_سرخیاں(بنية Curl_easy *انکڑے, bool is_connect,
                                 صحيح httpversion, بنية dynbuf *req);

فراغ Curl_http_منزل_fold(بنية dynbuf *bf);

فراغ Curl_http_طریقہ(بنية Curl_easy *انکڑے,
                      ثابت حرف **طریقہ, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_تیاری_conn(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn);
CURLcode Curl_http(بنية Curl_easy *انکڑے, bool *پورا);
CURLcode Curl_http_پورا(بنية Curl_easy *انکڑے,
                        CURLcode حالت, bool premature);
CURLcode Curl_http_doing_pollset(بنية Curl_easy *انکڑے,
                                 بنية easy_pollset *ps);
CURLcode Curl_http_perform_pollset(بنية Curl_easy *انکڑے,
                                   بنية easy_pollset *ps);
CURLcode Curl_http_write_resp(بنية Curl_easy *انکڑے,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(بنية Curl_easy *انکڑے,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_سرخی(بنية Curl_easy *انکڑے,
                            ثابت حرف *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_داخلہ_auth(بنية Curl_easy *انکڑے, bool proxy,
                              ثابت حرف *auth);

CURLcode Curl_http_auth_act(بنية Curl_easy *انکڑے);

/* follow a redirect or not */
CURLcode Curl_http_follow(بنية Curl_easy *انکڑے, ثابت حرف *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#تعریف_کرو CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#جے_بنا_تعریف MAX_INITIAL_POST_ناپ
#تعریف_کرو MAX_INITIAL_POST_ناپ (64 * 1024)
#جے_انت

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#جے_بنا_تعریف EXPECT_100_حد
#تعریف_کرو EXPECT_100_حد (1024 * 1024)
#جے_انت

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#تعریف_کرو MAX_HTTP_RESP_سرخی_ناپ (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#تعریف_کرو MAX_HTTP_RESP_سرخی_گنتی 5000

#جے_انت /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(بنية Curl_easy *انکڑے,
                                  ثابت حرف *buf, size_t blen,
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
CURLcode Curl_http_output_auth(بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *رستہ,
                               ثابت حرف *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_حالت(int32_t *pstatus, ثابت حرف *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
بنية httpreq {
  بنية dynhds سرخیاں;
  بنية dynhds trailers;
  حرف *scheme;
  حرف *authority;
  حرف *رستہ;
  حرف طریقہ[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(بنية httpreq **preq,
                            ثابت حرف *طریقہ, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *رستہ, size_t p_len);

CURLcode Curl_http_req_make2(بنية httpreq **preq,
                             ثابت حرف *طریقہ, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default);

فراغ Curl_http_req_free(بنية httpreq *req);

#تعریف_کرو HTTP_PSEUDO_طریقہ    ":method"
#تعریف_کرو HTTP_PSEUDO_SCHEME    ":scheme"
#تعریف_کرو HTTP_PSEUDO_AUTHORITY ":authority"
#تعریف_کرو HTTP_PSEUDO_رستہ      ":path"
#تعریف_کرو HTTP_PSEUDO_حالت    ":status"

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
CURLcode Curl_http_req_منزل_h2(بنية dynhds *h2_سرخیاں,
                             بنية httpreq *req, بنية Curl_easy *انکڑے);

/**
 * All about a core HTTP response, excluding body and trailers
 */
بنية http_resp {
  صحيح حالت;
  حرف *description;
  بنية dynhds سرخیاں;
  بنية dynhds trailers;
  بنية http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(بنية http_resp **presp,
                             صحيح حالت,
                             ثابت حرف *description);

فراغ Curl_http_resp_free(بنية http_resp *resp);

#جے_انت /* HEADER_CURL_HTTP_H */
