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

#ifdef USE_GSASL

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"

#ضمن <gsasl.h>

#إذا GSASL_VERSION_نمبر < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(بنية Curl_easy *data,
                                  ثابت حرف *mech,
                                  بنية gsasldata *gsasl)
{
  صحيح res;

  res = gsasl_init(&gsasl->ctx);
  إذا(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    أرجع FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  إذا(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = عدم;
    أرجع FALSE;
  }

  أرجع TRUE;
}

CURLcode Curl_auth_gsasl_start(بنية Curl_easy *data,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl)
{
#إذا GSASL_VERSION_نمبر >= 0x010b00
  صحيح res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#إذا GSASL_VERSION_نمبر >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    أرجع CURLE_OUT_OF_MEMORY;
  }
#endif

#إذا GSASL_VERSION_نمبر >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#إذا GSASL_VERSION_نمبر >= 0x010b00
  إذا(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    أرجع CURLE_OUT_OF_MEMORY;
  }
#endif

  (فراغ)data;

  أرجع CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(بنية Curl_easy *data,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *out)
{
  صحيح res;
  حرف *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  إذا(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  أرجع CURLE_OK;
}

فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = عدم;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = عدم;
}
#endif
