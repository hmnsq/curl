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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_GSASL

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"

#ضمن <gsasl.h>

#إذا GSASL_نسخه_عدد < 0x010600
#خطا "libgsasl 1.6.0 or greater required"
#پایان_اگر

bool Curl_auth_gsasl_is_supported(بنية Curl_easy *داده,
                                  ثابت حرف *mech,
                                  بنية gsasldata *gsasl)
{
  صحيح res;

  res = gsasl_init(&gsasl->ctx);
  إذا(res != GSASL_OK) {
    failf(داده, "gsasl init: %s", gsasl_strerror(res));
    أرجع FALSE;
  }

  res = gsasl_client_شروع(gsasl->ctx, mech, &gsasl->client);
  إذا(res != GSASL_OK) {
    gsasl_انجامشده(gsasl->ctx);
    gsasl->ctx = عدم;
    أرجع FALSE;
  }

  أرجع TRUE;
}

CURLcode Curl_auth_gsasl_شروع(بنية Curl_easy *داده,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl)
{
#إذا GSASL_نسخه_عدد >= 0x010b00
  صحيح res;
  res =
#پایان_اگر
    gsasl_property_مجموعه(gsasl->client, GSASL_AUTHID, creds->user);
#إذا GSASL_نسخه_عدد >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(داده, "setting AUTHID failed: %s", gsasl_strerror(res));
    أرجع CURLE_بیرون_از_حافظه;
  }
#پایان_اگر

#إذا GSASL_نسخه_عدد >= 0x010b00
  res =
#پایان_اگر
    gsasl_property_مجموعه(gsasl->client, GSASL_PASSWORD, creds->passwd);
#إذا GSASL_نسخه_عدد >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(داده, "setting PASSWORD failed: %s", gsasl_strerror(res));
    أرجع CURLE_بیرون_از_حافظه;
  }
#پایان_اگر

  (فراغ)داده;

  أرجع CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(بنية Curl_easy *داده,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *بیرون)
{
  صحيح res;
  حرف *response;
  size_t outlen;

  res = gsasl_گام(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  إذا(res != GSASL_OK && res != GSASL_NEEDS_بیشتر) {
    failf(داده, "GSASL step: %s", gsasl_strerror(res));
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_مجموعه(بیرون, response, outlen, gsasl_free);
  أرجع CURLE_OK;
}

فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl)
{
  gsasl_پایان(gsasl->client);
  gsasl->client = عدم;

  gsasl_انجامشده(gsasl->ctx);
  gsasl->ctx = عدم;
}
#پایان_اگر
