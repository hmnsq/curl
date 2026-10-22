#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CREDS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CREDS_H
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

ਬਣਤਰ Curl_easy;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CREDS_NONE   0 /* used for default username/passwd */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CREDS_URL    1 /* username/passwd from URL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CREDS_NETRC  3 /* username/passwd found in netrc */

ਬਣਤਰ Curl_creds {
  ਅਚਲ ਅੱਖਰ *user; /* non-NULL, maybe empty string */
  ਅਚਲ ਅੱਖਰ *passwd; /* non-NULL, maybe empty string */
  ਅਚਲ ਅੱਖਰ *oauth_bearer; /* non-NULL, maybe empty string */
  ਅਚਲ ਅੱਖਰ *sasl_authzid; /* non-NULL, maybe empty string */
  ਅਚਲ ਅੱਖਰ *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t ਸਰੋਤ; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  ਅੱਖਰ buf[1];
};

CURLcode Curl_creds_create(ਅਚਲ ਅੱਖਰ *user,
                           ਅਚਲ ਅੱਖਰ *passwd,
                           ਅਚਲ ਅੱਖਰ *oauth_bearer,
                           ਅਚਲ ਅੱਖਰ *sasl_authzid,
                           ਅਚਲ ਅੱਖਰ *sasl_service,
                           uint8_t ਸਰੋਤ,
                           ਬਣਤਰ Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_ਮਿਲਾਓ(ਅਚਲ ਅੱਖਰ *user,
                          ਅਚਲ ਅੱਖਰ *passwd,
                          ਬਣਤਰ Curl_creds *creds_ਵਿੱਚ,
                          uint8_t ਸਰੋਤ,
                          ਬਣਤਰ Curl_creds **pcreds_ਬਾਹਰਲਾ);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
ਖਾਲੀ Curl_creds_link(ਬਣਤਰ Curl_creds **pdest, ਬਣਤਰ Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
ਖਾਲੀ Curl_creds_unlink(ਬਣਤਰ Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(ਬਣਤਰ Curl_creds *c1, ਬਣਤਰ Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(ਬਣਤਰ Curl_creds *c1, ਬਣਤਰ Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_has_user(c)           ((c) && (c)->user[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_user(c)               ((c) ? (c)->user : "")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_creds_trace(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creds *creds,
                      ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_CREDS_H */
