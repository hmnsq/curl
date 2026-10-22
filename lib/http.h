#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HTTP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HTTP_H
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_ತಲೆ
} Curl_HttpReq;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HTTP_V1x   (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HTTP_V2x   (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ http_majors;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_http;

ರಚನೆ dynhds;

ರಚನೆ http_negotiation {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  ಬಿಟ್(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  ಬಿಟ್(h2_ಪೂರ್ವ_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  ಬಿಟ್(accept_09); /* Accept an HTTP/0.9 response */
  ಬಿಟ್(only_10); /* When using major version 1x, use only 1.0 */
};

ಶೂನ್ಯ Curl_http_neg_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ http_negotiation *neg);

CURLcode Curl_bump_headersize(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *headerline,  /* line to check */
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ,   /* header keyword _with_ colon */
                        ಸ್ಥಿರಾಂಕ size_t hlen,   /* len of the keyword in bytes */
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content, /* content string to find */
                        ಸ್ಥಿರಾಂಕ size_t clen);   /* len of the content in bytes */

ಅಕ್ಷರ *Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ);

ಅಕ್ಷರ *Curl_checkProxyheaders(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *thisheader,
                             ಸ್ಥಿರಾಂಕ size_t thislen);

CURLcode Curl_ಕೂಡಿಸು_timecondition(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ dynbuf *req);
CURLcode Curl_ಕೂಡಿಸು_custom_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool is_connect,
                                 ಪೂರ್ಣಾಂಕ httpversion, ರಚನೆ dynbuf *req);

ಶೂನ್ಯ Curl_http_ಗಮ್ಯ_fold(ರಚನೆ dynbuf *bf);

ಶೂನ್ಯ Curl_http_ವಿಧಾನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ವಿಧಾನ, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_ಸಿದ್ಧತೆ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn);
CURLcode Curl_http(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);
CURLcode Curl_http_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        CURLcode ಸ್ಥಿತಿ_2, bool premature);
CURLcode Curl_http_doing_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ easy_pollset *ps);
CURLcode Curl_http_perform_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ easy_pollset *ps);
CURLcode Curl_http_write_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_ಒಳಹರಿವು_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool proxy,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth);

CURLcode Curl_http_auth_act(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* follow a redirect or not */
CURLcode Curl_http_follow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAX_INITIAL_POST_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_INITIAL_POST_ಗಾತ್ರ (64 * 1024)
#ಆದರೆ_ಅಂತ್ಯ

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EXPECT_100_ಮಿತಿ
#ವ್ಯಾಖ್ಯಾನಿಸು EXPECT_100_ಮಿತಿ (1024 * 1024)
#ಆದರೆ_ಅಂತ್ಯ

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ 5000

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
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
CURLcode Curl_http_output_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request,
                               Curl_HttpReq httpreq,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_ಸ್ಥಿತಿ(int32_t *pstatus, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
ರಚನೆ httpreq {
  ರಚನೆ dynhds ಶೀರ್ಷಿಕೆಗಳು;
  ರಚನೆ dynhds trailers;
  ಅಕ್ಷರ *scheme;
  ಅಕ್ಷರ *authority;
  ಅಕ್ಷರ *ಮಾರ್ಗ;
  ಅಕ್ಷರ ವಿಧಾನ[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(ರಚನೆ httpreq **preq,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ, size_t m_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme, size_t s_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority, size_t a_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, size_t p_len);

CURLcode Curl_http_req_make2(ರಚನೆ httpreq **preq,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ, size_t m_len,
                             CURLU *url, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme_default);

ಶೂನ್ಯ Curl_http_req_free(ರಚನೆ httpreq *req);

#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_PSEUDO_ವಿಧಾನ    ":method"
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_PSEUDO_SCHEME    ":scheme"
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_PSEUDO_AUTHORITY ":authority"
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_PSEUDO_ಮಾರ್ಗ      ":path"
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_PSEUDO_ಸ್ಥಿತಿ    ":status"

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
CURLcode Curl_http_req_ಗಮ್ಯ_h2(ರಚನೆ dynhds *h2_ಶೀರ್ಷಿಕೆಗಳು,
                             ರಚನೆ httpreq *req, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * All about a core HTTP response, excluding body and trailers
 */
ರಚನೆ http_resp {
  ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
  ಅಕ್ಷರ *description;
  ರಚನೆ dynhds ಶೀರ್ಷಿಕೆಗಳು;
  ರಚನೆ dynhds trailers;
  ರಚನೆ http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(ರಚನೆ http_resp **presp,
                             ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description);

ಶೂನ್ಯ Curl_http_resp_free(ರಚನೆ http_resp *resp);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HTTP_H */
