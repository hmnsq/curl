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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#അന്തര്ഭാവ "vauth/vauth.h"

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
CURLcode Curl_auth_create_plain_message(ഘടന Curl_creds *creds,
                                        ഘടന bufref *out)
{
  size_t len;
  അക്ഷരം *auth;

  size_t zlen = strlen(Curl_creds_sasl_authzid(creds));
  size_t clen = strlen(Curl_creds_user(creds));
  size_t plen = strlen(Curl_creds_passwd(creds));

  എങ്കിൽ((zlen > CURL_MAX_നിവേശം_LENGTH) || (clen > CURL_MAX_നിവേശം_LENGTH) ||
     (plen > CURL_MAX_നിവേശം_LENGTH))
    തിരികെ CURLE_TOO_LARGE;

  len = zlen + clen + plen + 2;

  auth = curl_maprintf("%s%c%s%c%s",
                       Curl_creds_sasl_authzid(creds), '\0',
                       Curl_creds_user(creds), '\0',
                       Curl_creds_passwd(creds));
  എങ്കിൽ(!auth)
    തിരികെ CURLE_OUT_OF_MEMORY;
  Curl_bufref_set(out, auth, len, curl_free);
  തിരികെ CURLE_OK;
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
ശൂന്യം Curl_auth_create_login_message(സ്ഥിരം അക്ഷരം *മൂല്യം, ഘടന bufref *out)
{
  Curl_bufref_set(out, മൂല്യം, strlen(മൂല്യം), ശൂന്യ);
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
ശൂന്യം Curl_auth_create_external_message(സ്ഥിരം അക്ഷരം *user, ഘടന bufref *out)
{
  /* This is the same formatting as the login message */
  Curl_auth_create_login_message(user, out);
}

#endif /* if no users */
