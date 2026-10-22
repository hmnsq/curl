#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CREDS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CREDS_H
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

ರಚನೆ Curl_easy;

#ವ್ಯಾಖ್ಯಾನಿಸು CREDS_NONE   0 /* used for default username/passwd */
#ವ್ಯಾಖ್ಯಾನಿಸು CREDS_URL    1 /* username/passwd from URL */
#ವ್ಯಾಖ್ಯಾನಿಸು CREDS_OPTION 2 /* username/passwd set with a CURLOPT_ */
#ವ್ಯಾಖ್ಯಾನಿಸು CREDS_NETRC  3 /* username/passwd found in netrc */

ರಚನೆ Curl_creds {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user; /* non-NULL, maybe empty string */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd; /* non-NULL, maybe empty string */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oauth_bearer; /* non-NULL, maybe empty string */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_authzid; /* non-NULL, maybe empty string */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_service; /* non-NULL, maybe empty string */
  uint32_t refcount;
  uint8_t ಮೂಲ; /* CREDS_* value */
  size_t bufsize; /* extra bytes added to sizeof(struct Curl_creds) */
  ಅಕ್ಷರ buf[1];
};

CURLcode Curl_creds_create(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oauth_bearer,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_authzid,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_service,
                           uint8_t ಮೂಲ,
                           ರಚನೆ Curl_creds **pcreds);

/* Create credentials by overriding `user` and/or `passwd` in `creds_in` */
CURLcode Curl_creds_ವಿಲೀನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd,
                          ರಚನೆ Curl_creds *creds_ಒಳ,
                          uint8_t ಮೂಲ,
                          ರಚನೆ Curl_creds **pcreds_ಹೊರಹೋಗು);

/* Unlink any creds in `*pdest`, assign src, increase src
 * refcount when not NULL. */
ಶೂನ್ಯ Curl_creds_link(ರಚನೆ Curl_creds **pdest, ರಚನೆ Curl_creds *src);

/* Drop a reference, creds may be passed as NULL */
ಶೂನ್ಯ Curl_creds_unlink(ರಚನೆ Curl_creds **pcreds);

/* TRUE if both creds are NULL or have same values, except source. */
bool Curl_creds_same(ರಚನೆ Curl_creds *c1, ರಚನೆ Curl_creds *c2);

/* TRUE if both creds are NULL or have all values equal. */
bool Curl_creds_equal(ರಚನೆ Curl_creds *c1, ರಚನೆ Curl_creds *c2);

/* Provides properties for creds or, if creds is NULL, the empty string */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_has_user(c)           ((c) && (c)->user[0])
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_has_passwd(c)         ((c) && (c)->passwd[0])
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_has_user_or_pass(c) \
    ((c) && ((c)->user[0] || (c)->passwd[0]))
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_has_oauth_bearer(c)   ((c) && (c)->oauth_bearer[0])
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_has_sasl_service(c)   ((c) && (c)->sasl_service[0])
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_user(c)               ((c) ? (c)->user : "")
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_passwd(c)             ((c) ? (c)->passwd : "")
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_oauth_bearer(c)       ((c) ? (c)->oauth_bearer : "")
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_sasl_authzid(c)       ((c) ? (c)->sasl_authzid : "")
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creds_sasl_service(c)       ((c) ? (c)->sasl_service : "")

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_creds_trace(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creds *creds,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CREDS_H */
