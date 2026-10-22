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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ضمن "vauth/vauth.h"

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
CURLcode Curl_auth_create_oauth_bearer_پيغام(بنية Curl_creds *creds,
                                               ثابت حرف *host,
                                               ثابت ڊگهو پورٽ,
                                               بنية bufref *ٻاهريون)
{
  حرف *oauth;

  /* Generate the message */
  إذا(پورٽ == 0 || پورٽ == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  وإلا
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, پورٽ,
                          Curl_creds_oauth_bearer(creds));
  إذا(!oauth)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_bufref_سيٽ(ٻاهريون, oauth, لڙي_ڊيگهه(oauth), curl_free);
  أرجع CURLE_OK;
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
CURLcode Curl_auth_create_xoauth_bearer_پيغام(بنية Curl_creds *creds,
                                                بنية bufref *ٻاهريون)
{
  /* Generate the message */
  حرف *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  إذا(!xoauth)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_bufref_سيٽ(ٻاهريون, xoauth, لڙي_ڊيگهه(xoauth), curl_free);
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* disabled, no users */
