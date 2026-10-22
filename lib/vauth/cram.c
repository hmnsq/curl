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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_hmac.h"
#ಅನ್ತರ್ಭಾವ "curl_md5.h"

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
CURLcode Curl_auth_create_cram_md5_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                           ರಚನೆ Curl_creds *creds,
                                           ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ರಚನೆ HMAC_context *ctxt;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ digest[MD5_DIGEST_LEN];
  ಅಕ್ಷರ *response;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = Curl_creds_user(creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = Curl_creds_passwd(creds);

  /* Compute the digest using the password as the key */
  ctxt = Curl_HMAC_init(&Curl_HMAC_MD5,
                        (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)passwd,
                        curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd)));
  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Update the digest with the given challenge */
  ಆದರೆ(Curl_bufref_len(chlg))
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
  ಆದರೆ(!response)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, response, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(response), curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DIGEST_AUTH */
