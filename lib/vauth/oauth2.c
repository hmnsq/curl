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
 * RFC6749 OAuth 2.0 Authorization Framework
 *
 ***************************************************************************/
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#अन्तर्भाव "vauth/vauth.h"

/*
 * Curl_auth_create_oauth_bearer_message()
 *
 * This is used to generate an OAuth 2.0 message ready for sending to the
 * recipient.
 *
 * Parameters:
 *
 * user[in]         - The username.
 * host[in]         - The hostname.
 * port[in]         - The port(when not Port 80).
 * bearer[in]       - The bearer token.
 * out[out]         - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_oauth_bearer_सन्देशः(संरचना Curl_creds *creds,
                                               ध्रुव वर्ण *host,
                                               ध्रुव दीर्घ पत्तनम्,
                                               संरचना bufref *बहिर्गमनम्)
{
  वर्ण *oauth;

  /* Generate the message */
  यदि(पत्तनम् == 0 || पत्तनम् == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  अन्यथा
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, पत्तनम्,
                          Curl_creds_oauth_bearer(creds));
  यदि(!oauth)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_bufref_समुच्चयः(बहिर्गमनम्, oauth, वाक्यदैर्घ्यम्(oauth), curl_free);
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_auth_create_xoauth_bearer_message()
 *
 * This is used to generate a XOAuth 2.0 message ready for * sending to the
 * recipient.
 *
 * Parameters:
 *
 * user[in]         - The username.
 * bearer[in]       - The bearer token.
 * out[out]         - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_xoauth_bearer_सन्देशः(संरचना Curl_creds *creds,
                                                संरचना bufref *बहिर्गमनम्)
{
  /* Generate the message */
  वर्ण *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  यदि(!xoauth)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_bufref_समुच्चयः(बहिर्गमनम्, xoauth, वाक्यदैर्घ्यम्(xoauth), curl_free);
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* disabled, no users */
