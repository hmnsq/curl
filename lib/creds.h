#اگر_تعریف_نشده سرآیند_CURL_CREDS_H
#تعریف_کن سرآیند_CURL_CREDS_H
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

بنية Curl_easy;

#تعریف_کن CREDS_NONE   0 /* used for default username/passwd */
#تعریف_کن CREDS_URL    1 /* username/passwd from URL */
#تعریف_کن CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#تعریف_کن CREDS_NETRC  3 /* username/passwd found in netrc */

بنية Curl_creds {
  ثابت حرف *user; /* non-NULL, maybe empty string */
  ثابت حرف *passwd; /* non-NULL, maybe empty string */
  ثابت حرف *oauth_bearer; /* non-NULL, maybe empty string */
  ثابت حرف *sasl_authzid; /* non-NULL, maybe empty string */
  ثابت حرف *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t منبع; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  حرف buf[1];
};

CURLcode Curl_creds_create(ثابت حرف *user,
                           ثابت حرف *passwd,
                           ثابت حرف *oauth_bearer,
                           ثابت حرف *sasl_authzid,
                           ثابت حرف *sasl_service,
                           uint8_t منبع,
                           بنية Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_ادغام(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_درون,
                          uint8_t منبع,
                          بنية Curl_creds **pcreds_بیرون);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
فراغ Curl_creds_link(بنية Curl_creds **pdest, بنية Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
فراغ Curl_creds_unlink(بنية Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(بنية Curl_creds *c1, بنية Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(بنية Curl_creds *c1, بنية Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#تعریف_کن Curl_creds_has_user(c)           ((c) && (c)->user[0])
#تعریف_کن Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#تعریف_کن Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#تعریف_کن Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#تعریف_کن Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#تعریف_کن Curl_creds_user(c)               ((c) ? (c)->user : "")
#تعریف_کن Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#تعریف_کن Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#تعریف_کن Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#تعریف_کن Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#اگر_تعریف_شده CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *داده, بنية Curl_creds *creds,
                      ثابت حرف *پیام);
#پایان_اگر

#پایان_اگر /* HEADER_CURL_CREDS_H */
