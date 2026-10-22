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

#جيڪڏهن_وصف_ٿيل USE_GSASL

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"

#ضمن <gsasl.h>

#إذا GSASL_نسخو_عدد < 0x010600
#غلطي "libgsasl 1.6.0 or greater required"
#جيڪڏهن_پورو

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

  res = gsasl_client_شروعات(gsasl->ctx, mech, &gsasl->client);
  إذا(res != GSASL_OK) {
    gsasl_پورو(gsasl->ctx);
    gsasl->ctx = عدم;
    أرجع FALSE;
  }

  أرجع TRUE;
}

CURLcode Curl_auth_gsasl_شروعات(بنية Curl_easy *معلومات,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl)
{
#إذا GSASL_نسخو_عدد >= 0x010b00
  صحيح res;
  res =
#جيڪڏهن_پورو
    gsasl_property_سيٽ(gsasl->client, GSASL_AUTHID, creds->user);
#إذا GSASL_نسخو_عدد >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(معلومات, "setting AUTHID failed: %s", gsasl_strerror(res));
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_پورو

#إذا GSASL_نسخو_عدد >= 0x010b00
  res =
#جيڪڏهن_پورو
    gsasl_property_سيٽ(gsasl->client, GSASL_PASSWORD, creds->passwd);
#إذا GSASL_نسخو_عدد >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(معلومات, "setting PASSWORD failed: %s", gsasl_strerror(res));
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_پورو

  (فراغ)معلومات;

  أرجع CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(بنية Curl_easy *معلومات,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *ٻاهريون)
{
  صحيح res;
  حرف *response;
  size_t outlen;

  res = gsasl_قدم(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  إذا(res != GSASL_OK && res != GSASL_NEEDS_وڌيڪ) {
    failf(معلومات, "GSASL step: %s", gsasl_strerror(res));
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_سيٽ(ٻاهريون, response, outlen, gsasl_free);
  أرجع CURLE_OK;
}

فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl)
{
  gsasl_پڄاڻي(gsasl->client);
  gsasl->client = عدم;

  gsasl_پورو(gsasl->ctx);
  gsasl->ctx = عدم;
}
#جيڪڏهن_پورو
