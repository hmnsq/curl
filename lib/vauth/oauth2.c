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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"

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
CURLcode Curl_auth_create_oauth_bearer_ಸಂದೇಶ(ರಚನೆ Curl_creds *creds,
                                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                               ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ಪೋರ್ಟ್,
                                               ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ಅಕ್ಷರ *oauth;

  /* Generate the message */
  ಆದರೆ(ಪೋರ್ಟ್ == 0 || ಪೋರ್ಟ್ == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  ಇಲ್ಲದಿದ್ದರೆ
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, ಪೋರ್ಟ್,
                          Curl_creds_oauth_bearer(creds));
  ಆದರೆ(!oauth)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, oauth, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(oauth), curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_create_xoauth_bearer_ಸಂದೇಶ(ರಚನೆ Curl_creds *creds,
                                                ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  /* Generate the message */
  ಅಕ್ಷರ *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  ಆದರೆ(!xoauth)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, xoauth, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(xoauth), curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* disabled, no users */
