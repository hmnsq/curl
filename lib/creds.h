#अगर_अघोषित HEADER_CURL_CREDS_H
#घोषणा HEADER_CURL_CREDS_H
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

काष्ठा Curl_easy;

#घोषणा CREDS_NONE   0 /* used for default username/passwd */
#घोषणा CREDS_URL    1 /* username/passwd from URL */
#घोषणा CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#घोषणा CREDS_NETRC  3 /* username/passwd found in netrc */

काष्ठा Curl_creds {
  स्थिर अक्षर *user; /* non-NULL, maybe empty string */
  स्थिर अक्षर *passwd; /* non-NULL, maybe empty string */
  स्थिर अक्षर *oauth_bearer; /* non-NULL, maybe empty string */
  स्थिर अक्षर *sasl_authzid; /* non-NULL, maybe empty string */
  स्थिर अक्षर *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t source; /* CREDS_* value */
  माप_प्रकार bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  अक्षर buf[1];
};

CURLcode Curl_creds_create(स्थिर अक्षर *user,
                           स्थिर अक्षर *passwd,
                           स्थिर अक्षर *oauth_bearer,
                           स्थिर अक्षर *sasl_authzid,
                           स्थिर अक्षर *sasl_service,
                           uint8_t source,
                           काष्ठा Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_merge(स्थिर अक्षर *user,
                          स्थिर अक्षर *passwd,
                          काष्ठा Curl_creds *creds_in,
                          uint8_t source,
                          काष्ठा Curl_creds **pcreds_out);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
व्योम Curl_creds_link(काष्ठा Curl_creds **pdest, काष्ठा Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
व्योम Curl_creds_unlink(काष्ठा Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(काष्ठा Curl_creds *c1, काष्ठा Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(काष्ठा Curl_creds *c1, काष्ठा Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#घोषणा Curl_creds_has_user(c)           ((c) && (c)->user[0])
#घोषणा Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#घोषणा Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#घोषणा Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#घोषणा Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#घोषणा Curl_creds_user(c)               ((c) ? (c)->user : "")
#घोषणा Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#घोषणा Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#घोषणा Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#घोषणा Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#अगर_घोषित CURLVERBOSE
व्योम Curl_creds_trace(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creds *creds,
                      स्थिर अक्षर *संदेश);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_CREDS_H */
