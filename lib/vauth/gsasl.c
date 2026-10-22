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

#اگر_تعریف_شدہ USE_GSASL

#شامل "vauth/vauth.h"
#شامل "curl_trc.h"

#شامل <gsasl.h>

#اگر GSASL_نسخہ_نمبر < 0x010600
#غلطی "libgsasl 1.6.0 or greater required"
#اگر_ختم

bool Curl_auth_gsasl_is_supported(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *mech,
                                  ساخت gsasldata *gsasl)
{
  صحیح res;

  res = gsasl_init(&gsasl->ctx);
  اگر(res != GSASL_OK) {
    failf(ڈیٹا, "gsasl init: %s", gsasl_strerror(res));
    واپس FALSE;
  }

  res = gsasl_client_آغاز(gsasl->ctx, mech, &gsasl->client);
  اگر(res != GSASL_OK) {
    gsasl_مکمل(gsasl->ctx);
    gsasl->ctx = تہی;
    واپس FALSE;
  }

  واپس TRUE;
}

CURLcode Curl_auth_gsasl_آغاز(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creds *creds,
                               ساخت gsasldata *gsasl)
{
#اگر GSASL_نسخہ_نمبر >= 0x010b00
  صحیح res;
  res =
#اگر_ختم
    gsasl_property_سیٹ(gsasl->client, GSASL_AUTHID, creds->user);
#اگر GSASL_نسخہ_نمبر >= 0x010b00
  اگر(res != GSASL_OK) {
    failf(ڈیٹا, "setting AUTHID failed: %s", gsasl_strerror(res));
    واپس CURLE_برآمد_کا_حافظہ;
  }
#اگر_ختم

#اگر GSASL_نسخہ_نمبر >= 0x010b00
  res =
#اگر_ختم
    gsasl_property_سیٹ(gsasl->client, GSASL_PASSWORD, creds->passwd);
#اگر GSASL_نسخہ_نمبر >= 0x010b00
  اگر(res != GSASL_OK) {
    failf(ڈیٹا, "setting PASSWORD failed: %s", gsasl_strerror(res));
    واپس CURLE_برآمد_کا_حافظہ;
  }
#اگر_ختم

  (خالی)ڈیٹا;

  واپس CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ساخت Curl_easy *ڈیٹا,
                               ثابت ساخت bufref *chlg,
                               ساخت gsasldata *gsasl,
                               ساخت bufref *برآمد)
{
  صحیح res;
  حرف *response;
  size_t outlen;

  res = gsasl_قدم(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  اگر(res != GSASL_OK && res != GSASL_NEEDS_مزید) {
    failf(ڈیٹا, "GSASL step: %s", gsasl_strerror(res));
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_سیٹ(برآمد, response, outlen, gsasl_free);
  واپس CURLE_OK;
}

خالی Curl_auth_gsasl_cleanup(ساخت gsasldata *gsasl)
{
  gsasl_اختتام(gsasl->client);
  gsasl->client = تہی;

  gsasl_مکمل(gsasl->ctx);
  gsasl->ctx = تہی;
}
#اگر_ختم
