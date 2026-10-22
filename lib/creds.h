#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CREDS_H
#నిర్వచించు శీర్షిక_CURL_CREDS_H
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

నిర్మాణం Curl_easy;

#నిర్వచించు CREDS_NONE   0 /* used for default username/passwd */
#నిర్వచించు CREDS_URL    1 /* username/passwd from URL */
#నిర్వచించు CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#నిర్వచించు CREDS_NETRC  3 /* username/passwd found in netrc */

నిర్మాణం Curl_creds {
  స్థిరరాశి అక్షరం *user; /* non-NULL, maybe empty string */
  స్థిరరాశి అక్షరం *passwd; /* non-NULL, maybe empty string */
  స్థిరరాశి అక్షరం *oauth_bearer; /* non-NULL, maybe empty string */
  స్థిరరాశి అక్షరం *sasl_authzid; /* non-NULL, maybe empty string */
  స్థిరరాశి అక్షరం *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t ఆధారం; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  అక్షరం buf[1];
};

CURLcode Curl_creds_create(స్థిరరాశి అక్షరం *user,
                           స్థిరరాశి అక్షరం *passwd,
                           స్థిరరాశి అక్షరం *oauth_bearer,
                           స్థిరరాశి అక్షరం *sasl_authzid,
                           స్థిరరాశి అక్షరం *sasl_service,
                           uint8_t ఆధారం,
                           నిర్మాణం Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_విలీనం(స్థిరరాశి అక్షరం *user,
                          స్థిరరాశి అక్షరం *passwd,
                          నిర్మాణం Curl_creds *creds_లోపల,
                          uint8_t ఆధారం,
                          నిర్మాణం Curl_creds **pcreds_బహిర్గతం);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
శూన్య Curl_creds_link(నిర్మాణం Curl_creds **pdest, నిర్మాణం Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
శూన్య Curl_creds_unlink(నిర్మాణం Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(నిర్మాణం Curl_creds *c1, నిర్మాణం Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(నిర్మాణం Curl_creds *c1, నిర్మాణం Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#నిర్వచించు Curl_creds_has_user(c)           ((c) && (c)->user[0])
#నిర్వచించు Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#నిర్వచించు Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#నిర్వచించు Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#నిర్వచించు Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#నిర్వచించు Curl_creds_user(c)               ((c) ? (c)->user : "")
#నిర్వచించు Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#నిర్వచించు Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#నిర్వచించు Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#నిర్వచించు Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_creds_trace(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creds *creds,
                      స్థిరరాశి అక్షరం *సందేశం);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_CREDS_H */
