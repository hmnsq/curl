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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#అన్తర్భావ "vauth/vauth.h"

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
CURLcode Curl_auth_create_plain_సందేశం(నిర్మాణం Curl_creds *creds,
                                        నిర్మాణం bufref *బహిర్గతం)
{
  size_t len;
  అక్షరం *auth;

  size_t zlen = అక్షరశ్రేణి_పొడవు(Curl_creds_sasl_authzid(creds));
  size_t clen = అక్షరశ్రేణి_పొడవు(Curl_creds_user(creds));
  size_t plen = అక్షరశ్రేణి_పొడవు(Curl_creds_passwd(creds));

  ఒకవేళ((zlen > CURL_MAX_ఆదానం_నిడివి) || (clen > CURL_MAX_ఆదానం_నిడివి) ||
     (plen > CURL_MAX_ఆదానం_నిడివి))
    తిరిగి CURLE_TOO_LARGE;

  len = zlen + clen + plen + 2;

  auth = curl_maprintf("%s%c%s%c%s",
                       Curl_creds_sasl_authzid(creds), '\0',
                       Curl_creds_user(creds), '\0',
                       Curl_creds_passwd(creds));
  ఒకవేళ(!auth)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  Curl_bufref_సమితి(బహిర్గతం, auth, len, curl_free);
  తిరిగి CURLE_OK;
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
శూన్య Curl_auth_create_login_సందేశం(స్థిరరాశి అక్షరం *విలువ, నిర్మాణం bufref *బహిర్గతం)
{
  Curl_bufref_సమితి(బహిర్గతం, విలువ, అక్షరశ్రేణి_పొడవు(విలువ), ఖాళీ);
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
శూన్య Curl_auth_create_external_సందేశం(స్థిరరాశి అక్షరం *user, నిర్మాణం bufref *బహిర్గతం)
{
  /* This is the same formatting as the login message */
  Curl_auth_create_login_సందేశం(user, బహిర్గతం);
}

#అయితే_ముగింపు /* if no users */
