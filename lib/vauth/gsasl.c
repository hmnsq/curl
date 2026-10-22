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
#include "curl_setup.h"

#ifdef USE_GSASL

#include "vauth/vauth.h"
#include "curl_trc.h"

#include <gsasl.h>

#ᯀᯮᯝ GSASL_VERSION_NUMBER < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(sa_nracanaa Curl_easy *data,
                                  const var_na *mech,
                                  sa_nracanaa gsasldata *gsasl)
{
  puur_naa_nka res;

  res = gsasl_init(&gsasl->ctx);
  ᯀᯮᯝ(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    ᯀᯮᯞ FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  ᯀᯮᯝ(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = NULL;
    ᯀᯮᯞ FALSE;
  }

  ᯀᯮᯞ TRUE;
}

CURLcode Curl_auth_gsasl_start(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creds *creds,
                               sa_nracanaa gsasldata *gsasl)
{
#ᯀᯮᯝ GSASL_VERSION_NUMBER >= 0x010b00
  puur_naa_nka res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#ᯀᯮᯝ GSASL_VERSION_NUMBER >= 0x010b00
  ᯀᯮᯝ(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
  }
#endif

#ᯀᯮᯝ GSASL_VERSION_NUMBER >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ᯀᯮᯝ GSASL_VERSION_NUMBER >= 0x010b00
  ᯀᯮᯝ(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
  }
#endif

  (void)data;

  ᯀᯮᯞ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(sa_nracanaa Curl_easy *data,
                               const sa_nracanaa bufref *chlg,
                               sa_nracanaa gsasldata *gsasl,
                               sa_nracanaa bufref *out)
{
  puur_naa_nka res;
  var_na *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ᯀᯮᯝ(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    ᯀᯮᯞ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  ᯀᯮᯞ CURLE_OK;
}

void Curl_auth_gsasl_cleanup(sa_nracanaa gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = NULL;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = NULL;
}
#endif
