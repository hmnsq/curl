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
 * RFC4616 PLAIN authentication
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#समावेश "vauth/vauth.h"

/*
 * Curl_auth_create_plain_message()
 *
 * This is used to generate an already encoded PLAIN message ready
 * for sending to the recipient.
 *
 * Parameters:
 *
 * creds   [in]     - The credentials.
 * passwd  [in]     - The password.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_plain_message(काष्ठा Curl_creds *creds,
                                        काष्ठा bufref *out)
{
  माप_प्रकार len;
  अक्षर *auth;

  माप_प्रकार zlen = म_माप(Curl_creds_sasl_authzid(creds));
  माप_प्रकार clen = म_माप(Curl_creds_user(creds));
  माप_प्रकार plen = म_माप(Curl_creds_passwd(creds));

  अगर((zlen > CURL_MAX_निवेश_लंबाई) || (clen > CURL_MAX_निवेश_लंबाई) ||
     (plen > CURL_MAX_निवेश_लंबाई))
    वापस CURLE_TOO_LARGE;

  len = zlen + clen + plen + 2;

  auth = curl_maprintf("%s%c%s%c%s",
                       Curl_creds_sasl_authzid(creds), '\0',
                       Curl_creds_user(creds), '\0',
                       Curl_creds_passwd(creds));
  अगर(!auth)
    वापस CURLE_OUT_OF_MEMORY;
  Curl_bufref_set(out, auth, len, curl_free);
  वापस CURLE_OK;
}

/*
 * Curl_auth_create_login_message()
 *
 * This is used to generate an already encoded LOGIN message containing the
 * username or password ready for sending to the recipient.
 *
 * Parameters:
 *
 * value   [in]     - The username or user's password.
 * out     [out]    - The result storage.
 *
 * Returns void.
 */
व्योम Curl_auth_create_login_message(स्थिर अक्षर *मूल्य, काष्ठा bufref *out)
{
  Curl_bufref_set(out, मूल्य, म_माप(मूल्य), शून्य);
}

/*
 * Curl_auth_create_external_message()
 *
 * This is used to generate an already encoded EXTERNAL message containing
 * the username ready for sending to the recipient.
 *
 * Parameters:
 *
 * user    [in]     - The username.
 * out     [out]    - The result storage.
 *
 * Returns void.
 */
व्योम Curl_auth_create_external_message(स्थिर अक्षर *user, काष्ठा bufref *out)
{
  /* This is the same formatting as the login message */
  Curl_auth_create_login_message(user, out);
}

#पूर्ण_अगर /* if no users */
