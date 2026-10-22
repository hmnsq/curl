#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HTTP_H
#నిర్వచించు శీర్షిక_CURL_HTTP_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "bufq.h"
#అన్తర్భావ "dynhds.h"

రకనిర్వచనం గణన {
  HTTPREQ_GET,
  HTTPREQ_POST,
  HTTPREQ_POST_FORM, /* we make a difference internally */
  HTTPREQ_POST_MIME, /* we make a difference internally */
  HTTPREQ_PUT,
  HTTPREQ_తల
} Curl_HttpReq;

#నిర్వచించు CURL_HTTP_V1x   (1 << 0)
#నిర్వచించు CURL_HTTP_V2x   (1 << 1)
#నిర్వచించు CURL_HTTP_V3x   (1 << 2)
/* bitmask of CURL_HTTP_V* values */
రకనిర్వచనం గుర్తులేని అక్షరం http_majors;

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

బాహ్య స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_http;

నిర్మాణం dynhds;

నిర్మాణం http_negotiation {
  గుర్తులేని అక్షరం rcvd_min; /* minimum version seen in responses, 09, 10, 11 */
  http_majors wanted;  /* wanted major versions when talking to server */
  http_majors allowed; /* allowed major versions when talking to server */
  http_majors preferred; /* preferred major version when talking to server */
  బిట్టు(h2_upgrade);  /* Do HTTP Upgrade from 1.1 to 2 */
  బిట్టు(h2_పూర్వ_knowledge); /* Directly do HTTP/2 without ALPN/SSL */
  బిట్టు(accept_09); /* Accept an HTTP/0.9 response */
  బిట్టు(only_10); /* When using major version 1x, use only 1.0 */
};

శూన్య Curl_http_neg_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం http_negotiation *neg);

CURLcode Curl_bump_headersize(నిర్మాణం Curl_easy *దత్తాంశం,
                              size_t delta,
                              bool connect_only);

/* Header specific functions */
bool Curl_compareheader(స్థిరరాశి అక్షరం *headerline,  /* line to check */
                        స్థిరరాశి అక్షరం *శీర్షిక,   /* header keyword _with_ colon */
                        స్థిరరాశి size_t hlen,   /* len of the keyword in bytes */
                        స్థిరరాశి అక్షరం *content, /* content string to find */
                        స్థిరరాశి size_t clen);   /* len of the content in bytes */

అక్షరం *Curl_copy_శీర్షిక_విలువ(స్థిరరాశి అక్షరం *శీర్షిక);

అక్షరం *Curl_checkProxyheaders(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం connectdata *conn,
                             స్థిరరాశి అక్షరం *thisheader,
                             స్థిరరాశి size_t thislen);

CURLcode Curl_కూడు_timecondition(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *req);
CURLcode Curl_కూడు_custom_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం, bool is_connect,
                                 పూర్ణాంకం httpversion, నిర్మాణం dynbuf *req);

శూన్య Curl_http_గమ్యం_fold(నిర్మాణం dynbuf *bf);

శూన్య Curl_http_పద్ధతి(నిర్మాణం Curl_easy *దత్తాంశం,
                      స్థిరరాశి అక్షరం **పద్ధతి, Curl_HttpReq *reqp);

/* protocol-specific functions set up to be called by the main engine */
CURLcode Curl_http_అమరిక_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn);
CURLcode Curl_http(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);
CURLcode Curl_http_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                        CURLcode స్థితి_2, bool premature);
CURLcode Curl_http_doing_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం easy_pollset *ps);
CURLcode Curl_http_perform_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం easy_pollset *ps);
CURLcode Curl_http_write_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *buf, size_t blen,
                              bool is_eos);
CURLcode Curl_http_write_resp_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *hd, size_t hdlen,
                                 bool is_eos);

/* check a received header line for forbidden bytes/format, the same checks
   applied to regular response headers */
CURLcode Curl_verify_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *hd, size_t hdlen);

/* These functions are in http.c */
CURLcode Curl_http_ఆదానం_auth(నిర్మాణం Curl_easy *దత్తాంశం, bool proxy,
                              స్థిరరాశి అక్షరం *auth);

CURLcode Curl_http_auth_act(నిర్మాణం Curl_easy *దత్తాంశం);

/* follow a redirect or not */
CURLcode Curl_http_follow(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *newurl,
                          followtype type);

/* If only the PICKNONE bit is set, there has been a round-trip and we
   selected to use no auth at all. Ie, we actively select no auth, as opposed
   to not having one selected. The other CURLAUTH_* defines are present in the
   public curl/curl.h header. */
#నిర్వచించు CURLAUTH_PICKNONE (1 << 30) /* do not use auth */

/* MAX_INITIAL_POST_SIZE indicates the number of bytes that will make the POST
   data get included in the initial data chunk sent to the server. If the
   data is larger than this, it will automatically get split up in multiple
   system calls.

   This value used to be fairly big (100K), but we must take into account that
   if the server rejects the POST due for authentication reasons, this data
   will always be unconditionally sent and thus it may not be larger than can
   always be afforded to send twice.

   It must not be greater than 64K to work on VMS. */
#నిర్వచితం_కాకపోతే MAX_INITIAL_POST_కొలత
#నిర్వచించు MAX_INITIAL_POST_కొలత (64 * 1024)
#అయితే_ముగింపు

/* EXPECT_100_THRESHOLD is the request body size limit for when libcurl will
 * automatically add an "Expect: 100-continue" header in HTTP requests. When
 * the size is unknown, it will always add it.
 *
 */
#నిర్వచితం_కాకపోతే EXPECT_100_పరిమితి
#నిర్వచించు EXPECT_100_పరిమితి (1024 * 1024)
#అయితే_ముగింపు

/* MAX_HTTP_RESP_HEADER_SIZE is the maximum size of all response headers
   combined that libcurl allows for a single HTTP response, any HTTP
   version. This count includes CONNECT response headers. */
#నిర్వచించు MAX_HTTP_RESP_శీర్షిక_కొలత (300 * 1024)

/* MAX_HTTP_RESP_HEADER_COUNT is the maximum number of response headers that
   libcurl allows for a single HTTP response, including CONNECT and
   redirects. */
#నిర్వచించు MAX_HTTP_RESP_శీర్షిక_లెక్క 5000

#అయితే_ముగింపు /* CURL_DISABLE_HTTP */

/****************************************************************************
 * HTTP unique setup
 ***************************************************************************/

CURLcode Curl_http_write_resp_hds(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *buf, size_t blen,
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
CURLcode Curl_http_output_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               స్థిరరాశి అక్షరం *request,
                               Curl_HttpReq httpreq,
                               స్థిరరాశి అక్షరం *మార్గం,
                               స్థిరరాశి అక్షరం *query,
                               bool is_connect);

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_స్థితి(int32_t *pstatus, స్థిరరాశి అక్షరం *s, size_t len);

/**
 * All about a core HTTP request, excluding body and trailers
 */
నిర్మాణం httpreq {
  నిర్మాణం dynhds శీర్షికలు;
  నిర్మాణం dynhds trailers;
  అక్షరం *scheme;
  అక్షరం *authority;
  అక్షరం *మార్గం;
  అక్షరం పద్ధతి[1];
};

/**
 * Create an HTTP request struct.
 */
CURLcode Curl_http_req_make(నిర్మాణం httpreq **preq,
                            స్థిరరాశి అక్షరం *పద్ధతి, size_t m_len,
                            స్థిరరాశి అక్షరం *scheme, size_t s_len,
                            స్థిరరాశి అక్షరం *authority, size_t a_len,
                            స్థిరరాశి అక్షరం *మార్గం, size_t p_len);

CURLcode Curl_http_req_make2(నిర్మాణం httpreq **preq,
                             స్థిరరాశి అక్షరం *పద్ధతి, size_t m_len,
                             CURLU *url, స్థిరరాశి అక్షరం *scheme_default);

శూన్య Curl_http_req_free(నిర్మాణం httpreq *req);

#నిర్వచించు HTTP_PSEUDO_పద్ధతి    ":method"
#నిర్వచించు HTTP_PSEUDO_SCHEME    ":scheme"
#నిర్వచించు HTTP_PSEUDO_AUTHORITY ":authority"
#నిర్వచించు HTTP_PSEUDO_మార్గం      ":path"
#నిర్వచించు HTTP_PSEUDO_స్థితి    ":status"

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
CURLcode Curl_http_req_గమ్యం_h2(నిర్మాణం dynhds *h2_శీర్షికలు,
                             నిర్మాణం httpreq *req, నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * All about a core HTTP response, excluding body and trailers
 */
నిర్మాణం http_resp {
  పూర్ణాంకం స్థితి_2;
  అక్షరం *description;
  నిర్మాణం dynhds శీర్షికలు;
  నిర్మాణం dynhds trailers;
  నిర్మాణం http_resp *prev;
};

/**
 * Create an HTTP response struct.
 */
CURLcode Curl_http_resp_make(నిర్మాణం http_resp **presp,
                             పూర్ణాంకం స్థితి_2,
                             స్థిరరాశి అక్షరం *description);

శూన్య Curl_http_resp_free(నిర్మాణం http_resp *resp);

#అయితే_ముగింపు /* HEADER_CURL_HTTP_H */
