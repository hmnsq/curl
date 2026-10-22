/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Simon Josefsson, <simon@josefsson.org>, et al.
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
 * RFC5802 SCRAM-SHA-1 authentication
 *
 ***************************************************************************/
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ಅನ್ತರ್ಭಾವ <gsasl.h>

#ಆದರೆ GSASL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x010600
#ದೋಷ "libgsasl 1.6.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

bool Curl_auth_gsasl_is_supported(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                  ರಚನೆ gsasldata *gsasl)
{
  ಪೂರ್ಣಾಂಕ res;

  res = gsasl_init(&gsasl->ctx);
  ಆದರೆ(res != GSASL_OK) {
    failf(ದತ್ತಾಂಶ, "gsasl init: %s", gsasl_strerror(res));
    ಹಿಂತಿರುಗಿ FALSE;
  }

  res = gsasl_client_ಆರಂಭ(gsasl->ctx, mech, &gsasl->client);
  ಆದರೆ(res != GSASL_OK) {
    gsasl_ಮುಗಿದ(gsasl->ctx);
    gsasl->ctx = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಹಿಂತಿರುಗಿ TRUE;
}

CURLcode Curl_auth_gsasl_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creds *creds,
                               ರಚನೆ gsasldata *gsasl)
{
#ಆದರೆ GSASL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x010b00
  ಪೂರ್ಣಾಂಕ res;
  res =
#ಆದರೆ_ಅಂತ್ಯ
    gsasl_property_ಗಣ(gsasl->client, GSASL_AUTHID, creds->user);
#ಆದರೆ GSASL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x010b00
  ಆದರೆ(res != GSASL_OK) {
    failf(ದತ್ತಾಂಶ, "setting AUTHID failed: %s", gsasl_strerror(res));
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ GSASL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x010b00
  res =
#ಆದರೆ_ಅಂತ್ಯ
    gsasl_property_ಗಣ(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ಆದರೆ GSASL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x010b00
  ಆದರೆ(res != GSASL_OK) {
    failf(ದತ್ತಾಂಶ, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                               ರಚನೆ gsasldata *gsasl,
                               ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ಪೂರ್ಣಾಂಕ res;
  ಅಕ್ಷರ *response;
  size_t outlen;

  res = gsasl_ಹೆಜ್ಜೆ(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ಆದರೆ(res != GSASL_OK && res != GSASL_NEEDS_ಇನ್ನಷ್ಟು) {
    failf(ದತ್ತಾಂಶ, "GSASL step: %s", gsasl_strerror(res));
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, response, outlen, gsasl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_auth_gsasl_cleanup(ರಚನೆ gsasldata *gsasl)
{
  gsasl_ಮುಕ್ತಾಯ(gsasl->client);
  gsasl->client = ಖಾಲಿ;

  gsasl_ಮುಗಿದ(gsasl->ctx);
  gsasl->ctx = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ
