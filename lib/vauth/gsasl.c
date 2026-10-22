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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_GSASL

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curl_trc.h"

#অন্তর্ভাব <gsasl.h>

#যদি GSASL_VERSION_সংখ্যা < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *mech,
                                  কাঠামো gsasldata *gsasl)
{
  পূর্ণসংখ্যা res;

  res = gsasl_init(&gsasl->ctx);
  যদি(res != GSASL_OK) {
    failf(তথ্য, "gsasl init: %s", gsasl_strerror(res));
    ফেরত FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  যদি(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = ফাঁকা;
    ফেরত FALSE;
  }

  ফেরত TRUE;
}

CURLcode Curl_auth_gsasl_start(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creds *creds,
                               কাঠামো gsasldata *gsasl)
{
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  পূর্ণসংখ্যা res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  যদি(res != GSASL_OK) {
    failf(তথ্য, "setting AUTHID failed: %s", gsasl_strerror(res));
    ফেরত CURLE_OUT_OF_MEMORY;
  }
#endif

#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#যদি GSASL_VERSION_সংখ্যা >= 0x010b00
  যদি(res != GSASL_OK) {
    failf(তথ্য, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ফেরত CURLE_OUT_OF_MEMORY;
  }
#endif

  (শূন্য)তথ্য;

  ফেরত CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক কাঠামো bufref *chlg,
                               কাঠামো gsasldata *gsasl,
                               কাঠামো bufref *out)
{
  পূর্ণসংখ্যা res;
  অক্ষর *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  যদি(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(তথ্য, "GSASL step: %s", gsasl_strerror(res));
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  ফেরত CURLE_OK;
}

শূন্য Curl_auth_gsasl_cleanup(কাঠামো gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = ফাঁকা;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = ফাঁকা;
}
#endif
