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

#که_تعریف_شوی USE_GSASL

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"

#ضمن <gsasl.h>

#إذا GSASL_نسخه_شمېره < 0x010600
#تېروتنه "libgsasl 1.6.0 or greater required"
#د_که_پای

bool Curl_auth_gsasl_is_supported(بنية Curl_easy *معلومات,
                                  ثابت حرف *mech,
                                  بنية gsasldata *gsasl)
{
  صحيح res;

  res = gsasl_init(&gsasl->ctx);
  إذا(res != GSASL_OK) {
    failf(معلومات, "gsasl init: %s", gsasl_strerror(res));
    أرجع FALSE;
  }

  res = gsasl_client_پیل(gsasl->ctx, mech, &gsasl->client);
  إذا(res != GSASL_OK) {
    gsasl_بشپړ(gsasl->ctx);
    gsasl->ctx = عدم;
    أرجع FALSE;
  }

  أرجع TRUE;
}

CURLcode Curl_auth_gsasl_پیل(بنية Curl_easy *معلومات,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl)
{
#إذا GSASL_نسخه_شمېره >= 0x010b00
  صحيح res;
  res =
#د_که_پای
    gsasl_property_سېټ(gsasl->client, GSASL_AUTHID, creds->user);
#إذا GSASL_نسخه_شمېره >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(معلومات, "setting AUTHID failed: %s", gsasl_strerror(res));
    أرجع CURLE_بهر_د_حافظه;
  }
#د_که_پای

#إذا GSASL_نسخه_شمېره >= 0x010b00
  res =
#د_که_پای
    gsasl_property_سېټ(gsasl->client, GSASL_PASSWORD, creds->passwd);
#إذا GSASL_نسخه_شمېره >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(معلومات, "setting PASSWORD failed: %s", gsasl_strerror(res));
    أرجع CURLE_بهر_د_حافظه;
  }
#د_که_پای

  (فراغ)معلومات;

  أرجع CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(بنية Curl_easy *معلومات,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *بهر)
{
  صحيح res;
  حرف *response;
  size_t outlen;

  res = gsasl_ګام(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  إذا(res != GSASL_OK && res != GSASL_NEEDS_نور) {
    failf(معلومات, "GSASL step: %s", gsasl_strerror(res));
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_سېټ(بهر, response, outlen, gsasl_free);
  أرجع CURLE_OK;
}

فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl)
{
  gsasl_پای(gsasl->client);
  gsasl->client = عدم;

  gsasl_بشپړ(gsasl->ctx);
  gsasl->ctx = عدم;
}
#د_که_پای
