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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_GSASL

#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "curl_trc.h"

#அந்தர்பாவ <gsasl.h>

#எனில் GSASL_VERSION_எண் < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *mech,
                                  கட்டமைப்பு gsasldata *gsasl)
{
  முழு res;

  res = gsasl_init(&gsasl->ctx);
  எனில்(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    திரும்பு FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  எனில்(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = வெற்று;
    திரும்பு FALSE;
  }

  திரும்பு TRUE;
}

CURLcode Curl_auth_gsasl_start(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creds *creds,
                               கட்டமைப்பு gsasldata *gsasl)
{
#எனில் GSASL_VERSION_எண் >= 0x010b00
  முழு res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#எனில் GSASL_VERSION_எண் >= 0x010b00
  எனில்(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
#endif

#எனில் GSASL_VERSION_எண் >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#எனில் GSASL_VERSION_எண் >= 0x010b00
  எனில்(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
#endif

  (வெறுமை)data;

  திரும்பு CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(கட்டமைப்பு Curl_easy *data,
                               மாறிலி கட்டமைப்பு bufref *chlg,
                               கட்டமைப்பு gsasldata *gsasl,
                               கட்டமைப்பு bufref *out)
{
  முழு res;
  எழுத்து *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  எனில்(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    திரும்பு CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  திரும்பு CURLE_OK;
}

வெறுமை Curl_auth_gsasl_cleanup(கட்டமைப்பு gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = வெற்று;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = வெற்று;
}
#endif
