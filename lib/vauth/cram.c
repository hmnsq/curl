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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH

#شامل "vauth/vauth.h"
#شامل "curl_hmac.h"
#شامل "curl_md5.h"

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
CURLcode Curl_auth_create_cram_md5_پیغام(ثابت ساخت bufref *chlg,
                                           ساخت Curl_creds *creds,
                                           ساخت bufref *برآمد)
{
  ساخت HMAC_context *ctxt;
  غیرعلامتی حرف digest[MD5_DIGEST_LEN];
  حرف *response;
  ثابت حرف *user = Curl_creds_user(creds);
  ثابت حرف *passwd = Curl_creds_passwd(creds);

  /* Compute the digest using the password as the key */
  ctxt = Curl_HMAC_init(&Curl_HMAC_MD5,
                        (ثابت غیرعلامتی حرف *)passwd,
                        curlx_uztoui(لڑی_لمبائی(passwd)));
  اگر(!ctxt)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Update the digest with the given challenge */
  اگر(Curl_bufref_len(chlg))
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
  اگر(!response)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_bufref_سیٹ(برآمد, response, لڑی_لمبائی(response), curl_free);
  واپس CURLE_OK;
}

#اگر_ختم /* !CURL_DISABLE_DIGEST_AUTH */
