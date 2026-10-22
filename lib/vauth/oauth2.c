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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#അന്തര്ഭാവ "vauth/vauth.h"

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
CURLcode Curl_auth_create_oauth_bearer_message(ഘടന Curl_creds *creds,
                                               സ്ഥിരം അക്ഷരം *host,
                                               സ്ഥിരം long port,
                                               ഘടന bufref *out)
{
  അക്ഷരം *oauth;

  /* Generate the message */
  എങ്കിൽ(port == 0 || port == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  അല്ലെങ്കിൽ
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, port,
                          Curl_creds_oauth_bearer(creds));
  എങ്കിൽ(!oauth)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(out, oauth, strlen(oauth), curl_free);
  തിരികെ CURLE_OK;
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
CURLcode Curl_auth_create_xoauth_bearer_message(ഘടന Curl_creds *creds,
                                                ഘടന bufref *out)
{
  /* Generate the message */
  അക്ഷരം *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  എങ്കിൽ(!xoauth)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(out, xoauth, strlen(xoauth), curl_free);
  തിരികെ CURLE_OK;
}
#endif /* disabled, no users */
