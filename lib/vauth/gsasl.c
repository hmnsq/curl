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

#ifdef USE_GSASL

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ಅನ್ತರ್ಭಾವ <gsasl.h>

#ಯದಿ GSASL_VERSION_ಸಂಖ್ಯೆ < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *mech,
                                  ಸಂರಚನಾ gsasldata *gsasl)
{
  ಪೂರ್ಣಾಙ್ಕ res;

  res = gsasl_init(&gsasl->ctx);
  ಯದಿ(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  ಯದಿ(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  ಪ್ರತ್ಯಾಗಮ TRUE;
}

CURLcode Curl_auth_gsasl_start(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creds *creds,
                               ಸಂರಚನಾ gsasldata *gsasl)
{
#ಯದಿ GSASL_VERSION_ಸಂಖ್ಯೆ >= 0x010b00
  ಪೂರ್ಣಾಙ್ಕ res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#ಯದಿ GSASL_VERSION_ಸಂಖ್ಯೆ >= 0x010b00
  ಯದಿ(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
#endif

#ಯದಿ GSASL_VERSION_ಸಂಖ್ಯೆ >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ಯದಿ GSASL_VERSION_ಸಂಖ್ಯೆ >= 0x010b00
  ಯದಿ(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
#endif

  (ಶೂನ್ಯ)data;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                               ಸಂರಚನಾ gsasldata *gsasl,
                               ಸಂರಚನಾ bufref *out)
{
  ಪೂರ್ಣಾಙ್ಕ res;
  ವರ್ಣ *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ಯದಿ(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_auth_gsasl_cleanup(ಸಂರಚನಾ gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = ರಿಕ್ತ;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = ರಿಕ್ತ;
}
#endif
