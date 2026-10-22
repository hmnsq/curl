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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#అన్తర్భావ "vauth/vauth.h"

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
CURLcode Curl_auth_create_oauth_bearer_సందేశం(నిర్మాణం Curl_creds *creds,
                                               స్థిరరాశి అక్షరం *host,
                                               స్థిరరాశి దీర్ఘ పోర్టు,
                                               నిర్మాణం bufref *బహిర్గతం)
{
  అక్షరం *oauth;

  /* Generate the message */
  ఒకవేళ(పోర్టు == 0 || పోర్టు == 80)
    oauth = curl_maprintf("n,a=%s,\1host=%s\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host,
                          Curl_creds_oauth_bearer(creds));
  లేకపోతే
    oauth = curl_maprintf("n,a=%s,\1host=%s\1port=%ld\1auth=Bearer %s\1\1",
                          Curl_creds_user(creds), host, పోర్టు,
                          Curl_creds_oauth_bearer(creds));
  ఒకవేళ(!oauth)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_bufref_సమితి(బహిర్గతం, oauth, అక్షరశ్రేణి_పొడవు(oauth), curl_free);
  తిరిగి CURLE_OK;
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
CURLcode Curl_auth_create_xoauth_bearer_సందేశం(నిర్మాణం Curl_creds *creds,
                                                నిర్మాణం bufref *బహిర్గతం)
{
  /* Generate the message */
  అక్షరం *xoauth = curl_maprintf("user=%s\1auth=Bearer %s\1\1",
                               Curl_creds_user(creds),
                               Curl_creds_oauth_bearer(creds));
  ఒకవేళ(!xoauth)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_bufref_సమితి(బహిర్గతం, xoauth, అక్షరశ్రేణి_పొడవు(xoauth), curl_free);
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* disabled, no users */
