#اگر_غیر_تعریف_شدہ سرخی_CURL_CREDS_H
#تعریف_کرو سرخی_CURL_CREDS_H
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

ساخت Curl_easy;

#تعریف_کرو CREDS_NONE   0 /* used for default username/passwd */
#تعریف_کرو CREDS_URL    1 /* username/passwd from URL */
#تعریف_کرو CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#تعریف_کرو CREDS_NETRC  3 /* username/passwd found in netrc */

ساخت Curl_creds {
  ثابت حرف *user; /* non-NULL, maybe empty string */
  ثابت حرف *passwd; /* non-NULL, maybe empty string */
  ثابت حرف *oauth_bearer; /* non-NULL, maybe empty string */
  ثابت حرف *sasl_authzid; /* non-NULL, maybe empty string */
  ثابت حرف *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t ماخذ; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  حرف buf[1];
};

CURLcode Curl_creds_create(ثابت حرف *user,
                           ثابت حرف *passwd,
                           ثابت حرف *oauth_bearer,
                           ثابت حرف *sasl_authzid,
                           ثابت حرف *sasl_service,
                           uint8_t ماخذ,
                           ساخت Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_ملاؤ(ثابت حرف *user,
                          ثابت حرف *passwd,
                          ساخت Curl_creds *creds_اندر,
                          uint8_t ماخذ,
                          ساخت Curl_creds **pcreds_برآمد);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
خالی Curl_creds_link(ساخت Curl_creds **pdest, ساخت Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
خالی Curl_creds_unlink(ساخت Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(ساخت Curl_creds *c1, ساخت Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(ساخت Curl_creds *c1, ساخت Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#تعریف_کرو Curl_creds_has_user(c)           ((c) && (c)->user[0])
#تعریف_کرو Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#تعریف_کرو Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#تعریف_کرو Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#تعریف_کرو Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#تعریف_کرو Curl_creds_user(c)               ((c) ? (c)->user : "")
#تعریف_کرو Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#تعریف_کرو Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#تعریف_کرو Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#تعریف_کرو Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_creds_trace(ساخت Curl_easy *ڈیٹا, ساخت Curl_creds *creds,
                      ثابت حرف *پیغام);
#اگر_ختم

#اگر_ختم /* HEADER_CURL_CREDS_H */
