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
 * RFC2195 CRAM-MD5 authentication
 *
 ***************************************************************************/
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_hmac.h"
#అన్తర్భావ "curl_md5.h"

/*
 * Curl_auth_create_cram_md5_message()
 *
 * This is used to generate a CRAM-MD5 response message ready for sending to
 * the recipient.
 *
 * Parameters:
 *
 * chlg    [in]     - The challenge.
 * userp   [in]     - The username.
 * passwdp [in]     - The user's password.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_cram_md5_సందేశం(స్థిరరాశి నిర్మాణం bufref *chlg,
                                           నిర్మాణం Curl_creds *creds,
                                           నిర్మాణం bufref *బహిర్గతం)
{
  నిర్మాణం HMAC_context *ctxt;
  గుర్తులేని అక్షరం digest[MD5_DIGEST_LEN];
  అక్షరం *response;
  స్థిరరాశి అక్షరం *user = Curl_creds_user(creds);
  స్థిరరాశి అక్షరం *passwd = Curl_creds_passwd(creds);

  /* Compute the digest using the password as the key */
  ctxt = Curl_HMAC_init(&Curl_HMAC_MD5,
                        (స్థిరరాశి గుర్తులేని అక్షరం *)passwd,
                        curlx_uztoui(అక్షరశ్రేణి_పొడవు(passwd)));
  ఒకవేళ(!ctxt)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Update the digest with the given challenge */
  ఒకవేళ(Curl_bufref_len(chlg))
    Curl_HMAC_update(ctxt, Curl_bufref_uptr(chlg),
                     curlx_uztoui(Curl_bufref_len(chlg)));

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, digest);

  /* Generate the response */
  response = curl_maprintf(
    "%s %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    user, digest[0], digest[1], digest[2], digest[3], digest[4],
    digest[5], digest[6], digest[7], digest[8], digest[9], digest[10],
    digest[11], digest[12], digest[13], digest[14], digest[15]);
  ఒకవేళ(!response)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_bufref_సమితి(బహిర్గతం, response, అక్షరశ్రేణి_పొడవు(response), curl_free);
  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* !CURL_DISABLE_DIGEST_AUTH */
