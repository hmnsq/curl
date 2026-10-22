#यदि_अपरिभाषितम् शीर्षकम्_CURL_HTTP_H
#परिभाषय शीर्षकम्_CURL_HTTP_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "dynhds.h"

प्रकारनिर्देश गणना {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_शीर्ष
} Curl_HttpReq;

#परिभाषय CURL_HTTP_V1x   (1 << 0)
#परिभाषय CURL_HTTP_V2x   (1 << 1)
#परिभाषय CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
प्रकारनिर्देश अचिह्नित वर्ण http_majors;

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

बाह्य ध्रुव संरचना Curl_protocol Curl_protocol_http;

संरचना dynhds;

संरचना http_negotiation {
  अचिह्नित वर्ण rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  BIT(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  BIT(h2_प्राक्_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  BIT(accept_09); /* Accept an HTTP/0.9 response */
  BIT(only_10); /* When using major version 1x, use only 1.0 */
};

शून्य Curl_http_neg_init(संरचना Curl_easy *दत्त, संरचना http_negotiation *neg);

CURLcode Curl_bump_headersize(संरचना Curl_easy *दत्त,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(ध्रुव वर्ण *headerline,  /* line to check */
                        ध्रुव वर्ण *शीर्षकम्,   /* header keyword _with_ colon */
                        ध्रुव size_t hlen,   /* len of the keyword in bytes */
                        ध्रुव वर्ण *content, /* content string to find */
                        ध्रुव size_t clen);   /* len of the content in bytes */

वर्ण *Curl_copy_शीर्षकम्_मानम्(ध्रुव वर्ण *शीर्षकम्);

वर्ण *Curl_checkProxyheaders(संरचना Curl_easy *दत्त,
                             ध्रुव संरचना connectdata *conn,
                             ध्रुव वर्ण *thisheader,
                             ध्रुव size_t thislen);

CURLcode Curl_सङ्कलय_timecondition(संरचना Curl_easy *दत्त, संरचना dynbuf *req);
CURLcode Curl_सङ्कलय_custom_शीर्षकाणि(संरचना Curl_easy *दत्त, bool is_connect,
                                 पूर्णाङ्क httpversion, संरचना dynbuf *req);

शून्य Curl_http_गन्तव्यम्_fold(संरचना dynbuf *bf);

शून्य Curl_http_विधानम्(संरचना Curl_easy *दत्त,
                      ध्रुव वर्ण **विधानम्, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_सज्जता_conn(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn);
CURLcode Curl_http(संरचना Curl_easy *दत्त, bool *समाप्तम्);
CURLcode Curl_http_समाप्तम्(संरचना Curl_easy *दत्त,
                        CURLcode स्थितिः, bool premature);
CURLcode Curl_http_doing_pollset(संरचना Curl_easy *दत्त,
                                 संरचना easy_pollset *ps);
CURLcode Curl_http_perform_pollset(संरचना Curl_easy *दत्त,
                                   संरचना easy_pollset *ps);
CURLcode Curl_http_write_resp(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_शीर्षकम्(संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_निवेशः_auth(संरचना Curl_easy *दत्त, bool proxy,
                              ध्रुव वर्ण *auth);

CURLcode Curl_http_auth_act(संरचना Curl_easy *दत्त);

/* follow a redirect or not */
CURLcode Curl_http_follow(संरचना Curl_easy *दत्त, ध्रुव वर्ण *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#परिभाषय CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#यदि_अपरिभाषितम् MAX_INITIAL_POST_आकार
#परिभाषय MAX_INITIAL_POST_आकार (64 * 1024)
#यदि_अन्तः

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#यदि_अपरिभाषितम् EXPECT_100_देहली
#परिभाषय EXPECT_100_देहली (1024 * 1024)
#यदि_अन्तः

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#परिभाषय MAX_HTTP_RESP_शीर्षकम्_आकार (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#परिभाषय MAX_HTTP_RESP_शीर्षकम्_गणनम् 5000

#यदि_अन्तः /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *buf, size_t blen,
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
CURLcode Curl_http_output_auth(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               ध्रुव वर्ण *request,
                               Curl_HttpReq httpreq,
                               ध्रुव वर्ण *मार्गः,
                               ध्रुव वर्ण *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_स्थितिः(int32_t *pstatus, ध्रुव वर्ण *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
संरचना httpreq {
  संरचना dynhds शीर्षकाणि;
  संरचना dynhds trailers;
  वर्ण *scheme;
  वर्ण *authority;
  वर्ण *मार्गः;
  वर्ण विधानम्[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(संरचना httpreq **preq,
                            ध्रुव वर्ण *विधानम्, size_t m_len,
                            ध्रुव वर्ण *scheme, size_t s_len,
                            ध्रुव वर्ण *authority, size_t a_len,
                            ध्रुव वर्ण *मार्गः, size_t p_len);

CURLcode Curl_http_req_make2(संरचना httpreq **preq,
                             ध्रुव वर्ण *विधानम्, size_t m_len,
                             CURLU *url, ध्रुव वर्ण *scheme_default);

शून्य Curl_http_req_free(संरचना httpreq *req);

#परिभाषय HTTP_PSEUDO_विधानम्    ":method"
#परिभाषय HTTP_PSEUDO_SCHEME    ":scheme"
#परिभाषय HTTP_PSEUDO_AUTHORITY ":authority"
#परिभाषय HTTP_PSEUDO_मार्गः      ":path"
#परिभाषय HTTP_PSEUDO_स्थितिः    ":status"

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
CURLcode Curl_http_req_गन्तव्यम्_h2(संरचना dynhds *h2_शीर्षकाणि,
                             संरचना httpreq *req, संरचना Curl_easy *दत्त);

/**
 * All about a core HTTP response, excluding body and trailers
 */
संरचना http_resp {
  पूर्णाङ्क स्थितिः;
  वर्ण *description;
  संरचना dynhds शीर्षकाणि;
  संरचना dynhds trailers;
  संरचना http_resp *पूर्व;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(संरचना http_resp **presp,
                             पूर्णाङ्क स्थितिः,
                             ध्रुव वर्ण *description);

शून्य Curl_http_resp_free(संरचना http_resp *resp);

#यदि_अन्तः /* HEADER_CURL_HTTP_H */
