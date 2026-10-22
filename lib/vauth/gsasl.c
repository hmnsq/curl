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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_GSASL

#অন্তর্ভাৱ "vauth/vauth.h"
#অন্তর্ভাৱ "curl_trc.h"

#অন্তর্ভাৱ <gsasl.h>

#যদি GSASL_VERSION_সংখ্যা < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(সংরচনা Curl_easy *data,
                                  ধ্রুৱ ৱর্ণ *mech,
                                  সংরচনা gsasldata *gsasl)
{
  পূর্ণাঙ্ক res;

  res = gsasl_init(&gsasl->ctx);
  যদি(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    ঘূৰাই FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  যদি(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = রিক্ত;
    ঘূৰাই FALSE;
  }

  ঘূৰাই TRUE;
}

CURLcode Curl_auth_gsasl_start(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creds *creds,
                               সংরচনা gsasldata *gsasl)
{
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  পূর্ণাঙ্ক res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  যদি(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
#endif

#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  যদি(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
#endif

  (শূন্য)data;

  ঘূৰাই CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(সংরচনা Curl_easy *data,
                               ধ্রুৱ সংরচনা bufref *chlg,
                               সংরচনা gsasldata *gsasl,
                               সংরচনা bufref *out)
{
  পূর্ণাঙ্ক res;
  ৱর্ণ *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  যদি(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_auth_gsasl_cleanup(সংরচনা gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = রিক্ত;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = রিক্ত;
}
#endif
