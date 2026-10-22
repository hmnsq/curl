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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <gsasl.h>

#ਜੇ GSASL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x010600
#ਗਲਤੀ "libgsasl 1.6.0 or greater required"
#ਜੇ_ਅੰਤ

bool Curl_auth_gsasl_is_supported(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *mech,
                                  ਬਣਤਰ gsasldata *gsasl)
{
  ਪੂਰਨ res;

  res = gsasl_init(&gsasl->ctx);
  ਜੇ(res != GSASL_OK) {
    failf(ਅੰਕੜੇ, "gsasl init: %s", gsasl_strerror(res));
    ਵਾਪਸ FALSE;
  }

  res = gsasl_client_ਸ਼ੁਰੂ(gsasl->ctx, mech, &gsasl->client);
  ਜੇ(res != GSASL_OK) {
    gsasl_ਪੂਰਾ(gsasl->ctx);
    gsasl->ctx = ਨਿਰਾ;
    ਵਾਪਸ FALSE;
  }

  ਵਾਪਸ TRUE;
}

CURLcode Curl_auth_gsasl_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creds *creds,
                               ਬਣਤਰ gsasldata *gsasl)
{
#ਜੇ GSASL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x010b00
  ਪੂਰਨ res;
  res =
#ਜੇ_ਅੰਤ
    gsasl_property_ਸੈੱਟ(gsasl->client, GSASL_AUTHID, creds->user);
#ਜੇ GSASL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x010b00
  ਜੇ(res != GSASL_OK) {
    failf(ਅੰਕੜੇ, "setting AUTHID failed: %s", gsasl_strerror(res));
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ GSASL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x010b00
  res =
#ਜੇ_ਅੰਤ
    gsasl_property_ਸੈੱਟ(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ਜੇ GSASL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x010b00
  ਜੇ(res != GSASL_OK) {
    failf(ਅੰਕੜੇ, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਅੰਤ

  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ ਬਣਤਰ bufref *chlg,
                               ਬਣਤਰ gsasldata *gsasl,
                               ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਪੂਰਨ res;
  ਅੱਖਰ *response;
  size_t outlen;

  res = gsasl_ਕਦਮ(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ਜੇ(res != GSASL_OK && res != GSASL_NEEDS_ਹੋਰ) {
    failf(ਅੰਕੜੇ, "GSASL step: %s", gsasl_strerror(res));
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, response, outlen, gsasl_free);
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_auth_gsasl_cleanup(ਬਣਤਰ gsasldata *gsasl)
{
  gsasl_ਅੰਤ(gsasl->client);
  gsasl->client = ਨਿਰਾ;

  gsasl_ਪੂਰਾ(gsasl->ctx);
  gsasl->ctx = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ
