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
#ضمن "curl_setup.h"

#𐤀𐤌 !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ضمن "vauth/vauth.h"

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
CURLcode Curl_auth_create_plain_message(بنية Curl_creds *creds,
                                        بنية bufref *out)
{
  size_t len;
  حرف *auth;

  size_t zlen = strlen(Curl_creds_sasl_authzid(creds));
  size_t clen = strlen(Curl_creds_user(creds));
  size_t plen = strlen(Curl_creds_passwd(creds));

  𐤀𐤌((zlen > CURL_MAX_INPUT_LENGTH) || (clen > CURL_MAX_INPUT_LENGTH) ||
     (plen > CURL_MAX_INPUT_LENGTH))
    أرجع CURLE_TOO_LARGE;

  len = zlen + clen + plen + 2;

  auth = curl_maprintf("%s%c%s%c%s",
                       Curl_creds_sasl_authzid(creds), '\0',
                       Curl_creds_user(creds), '\0',
                       Curl_creds_passwd(creds));
  𐤀𐤌(!auth)
    أرجع CURLE_OUT_OF_MEMORY;
  Curl_bufref_set(out, auth, len, curl_free);
  أرجع CURLE_OK;
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
فراغ Curl_auth_create_login_message(ثابت حرف *value, بنية bufref *out)
{
  Curl_bufref_set(out, value, strlen(value), عدم);
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
فراغ Curl_auth_create_external_message(ثابت حرف *user, بنية bufref *out)
{
  /* This is the same formatting as the login message */
  Curl_auth_create_login_message(user, out);
}

#endif /* if no users */
