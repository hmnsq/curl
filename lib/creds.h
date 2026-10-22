#ifndef HEADER_CURL_CREDS_H
#define HEADER_CURL_CREDS_H
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

সংরচনা Curl_easy;

#define CREDS_NONE   0 /* used for default username/passwd */
#define CREDS_URL    1 /* username/passwd from URL */
#define CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#define CREDS_NETRC  3 /* username/passwd found in netrc */

সংরচনা Curl_creds {
  ধ্রুৱ ৱর্ণ *user; /* non-NULL, maybe empty string */
  ধ্রুৱ ৱর্ণ *passwd; /* non-NULL, maybe empty string */
  ধ্রুৱ ৱর্ণ *oauth_bearer; /* non-NULL, maybe empty string */
  ধ্রুৱ ৱর্ণ *sasl_authzid; /* non-NULL, maybe empty string */
  ধ্রুৱ ৱর্ণ *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t source; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  ৱর্ণ buf[1];
};

CURLcode Curl_creds_create(ধ্রুৱ ৱর্ণ *user,
                           ধ্রুৱ ৱর্ণ *passwd,
                           ধ্রুৱ ৱর্ণ *oauth_bearer,
                           ধ্রুৱ ৱর্ণ *sasl_authzid,
                           ধ্রুৱ ৱর্ণ *sasl_service,
                           uint8_t source,
                           সংরচনা Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_merge(ধ্রুৱ ৱর্ণ *user,
                          ধ্রুৱ ৱর্ণ *passwd,
                          সংরচনা Curl_creds *creds_in,
                          uint8_t source,
                          সংরচনা Curl_creds **pcreds_out);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
শূন্য Curl_creds_link(সংরচনা Curl_creds **pdest, সংরচনা Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
শূন্য Curl_creds_unlink(সংরচনা Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(সংরচনা Curl_creds *c1, সংরচনা Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(সংরচনা Curl_creds *c1, সংরচনা Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#define Curl_creds_has_user(c)           ((c) && (c)->user[0])
#define Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#define Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#define Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#define Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#define Curl_creds_user(c)               ((c) ? (c)->user : "")
#define Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#define Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#define Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#define Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#ifdef CURLVERBOSE
শূন্য Curl_creds_trace(সংরচনা Curl_easy *data, সংরচনা Curl_creds *creds,
                      ধ্রুৱ ৱর্ণ *বাৰ্তা);
#endif

#endif /* HEADER_CURL_CREDS_H */
