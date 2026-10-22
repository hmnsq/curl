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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"

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
CURLcode Curl_auth_create_oauth_bearer_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_creds *creds,
                                               ਅਚਲ ਅੱਖਰ *host,
                                               ਅਚਲ ਲੰਮਾ ਪੋਰਟ,
                                               ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਅੱਖਰ *oauth;

  /* Generate the message */
  ਜੇ(ਪੋਰਟ == 0 || ਪੋਰਟ == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  ਨਹੀਂਤਾਂ
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, ਪੋਰਟ,
                          Curl_creds_oauth_bearer(creds));
  ਜੇ(!oauth)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, oauth, ਲੜੀ_ਲੰਬਾਈ(oauth), curl_free);
  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_create_xoauth_bearer_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_creds *creds,
                                                ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  /* Generate the message */
  ਅੱਖਰ *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  ਜੇ(!xoauth)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, xoauth, ਲੜੀ_ਲੰਬਾਈ(xoauth), curl_free);
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* disabled, no users */
