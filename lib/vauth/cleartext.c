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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#अन्तर्भाव "vauth/vauth.h"

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
CURLcode Curl_auth_create_plain_सन्देशः(संरचना Curl_creds *creds,
                                        संरचना bufref *बहिर्गमनम्)
{
  size_t len;
  वर्ण *auth;

  size_t zlen = वाक्यदैर्घ्यम्(Curl_creds_sasl_authzid(creds));
  size_t clen = वाक्यदैर्घ्यम्(Curl_creds_user(creds));
  size_t plen = वाक्यदैर्घ्यम्(Curl_creds_passwd(creds));

  यदि((zlen > CURL_MAX_निवेशः_दीर्घता) || (clen > CURL_MAX_निवेशः_दीर्घता) ||
     (plen > CURL_MAX_निवेशः_दीर्घता))
    प्रत्यागम CURLE_TOO_LARGE;

  len = zlen + clen + plen + 2;

  auth = curl_maprintf("%s%c%s%c%s",
                       Curl_creds_sasl_authzid(creds), '\0',
                       Curl_creds_user(creds), '\0',
                       Curl_creds_passwd(creds));
  यदि(!auth)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  Curl_bufref_समुच्चयः(बहिर्गमनम्, auth, len, curl_free);
  प्रत्यागम CURLE_OK;
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
शून्य Curl_auth_create_login_सन्देशः(ध्रुव वर्ण *मानम्, संरचना bufref *बहिर्गमनम्)
{
  Curl_bufref_समुच्चयः(बहिर्गमनम्, मानम्, वाक्यदैर्घ्यम्(मानम्), रिक्त);
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
शून्य Curl_auth_create_external_सन्देशः(ध्रुव वर्ण *user, संरचना bufref *बहिर्गमनम्)
{
  /* This is the same formatting as the login message */
  Curl_auth_create_login_सन्देशः(user, बहिर्गमनम्);
}

#यदि_अन्तः /* if no users */
