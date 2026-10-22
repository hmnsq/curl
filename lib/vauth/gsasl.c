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
#شامل "curl_setup.h"

#ifdef USE_GSASL

#شامل "vauth/vauth.h"
#شامل "curl_trc.h"

#شامل <gsasl.h>

#اگر GSASL_VERSION_عدد < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(ساختار Curl_easy *data,
                                  ثابت نویسه *mech,
                                  ساختار gsasldata *gsasl)
{
  صحیح res;

  res = gsasl_init(&gsasl->ctx);
  اگر(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    بازگشت FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  اگر(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = پوچ;
    بازگشت FALSE;
  }

  بازگشت TRUE;
}

CURLcode Curl_auth_gsasl_start(ساختار Curl_easy *data,
                               ساختار Curl_creds *creds,
                               ساختار gsasldata *gsasl)
{
#اگر GSASL_VERSION_عدد >= 0x010b00
  صحیح res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#اگر GSASL_VERSION_عدد >= 0x010b00
  اگر(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    بازگشت CURLE_OUT_OF_MEMORY;
  }
#endif

#اگر GSASL_VERSION_عدد >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#اگر GSASL_VERSION_عدد >= 0x010b00
  اگر(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    بازگشت CURLE_OUT_OF_MEMORY;
  }
#endif

  (تهی)data;

  بازگشت CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ساختار Curl_easy *data,
                               ثابت ساختار bufref *chlg,
                               ساختار gsasldata *gsasl,
                               ساختار bufref *out)
{
  صحیح res;
  نویسه *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  اگر(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    بازگشت CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  بازگشت CURLE_OK;
}

تهی Curl_auth_gsasl_cleanup(ساختار gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = پوچ;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = پوچ;
}
#endif
