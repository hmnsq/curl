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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_GSASL

#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"

#ଅନ୍ତର୍ଭାଵ <gsasl.h>

#ଯଦି GSASL_VERSION_ସଂଖ୍ୟା < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *mech,
                                  ସଂରଚନା gsasldata *gsasl)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;

  res = gsasl_init(&gsasl->ctx);
  ଯଦି(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    ଫେରନ୍ତୁ FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  ଯଦି(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = ଖାଲି;
    ଫେରନ୍ତୁ FALSE;
  }

  ଫେରନ୍ତୁ TRUE;
}

CURLcode Curl_auth_gsasl_start(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creds *creds,
                               ସଂରଚନା gsasldata *gsasl)
{
#ଯଦି GSASL_VERSION_ସଂଖ୍ୟା >= 0x010b00
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#ଯଦି GSASL_VERSION_ସଂଖ୍ୟା >= 0x010b00
  ଯଦି(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
#endif

#ଯଦି GSASL_VERSION_ସଂଖ୍ୟା >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ଯଦି GSASL_VERSION_ସଂଖ୍ୟା >= 0x010b00
  ଯଦି(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
#endif

  (ଶୂନ୍ୟ)data;

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ସଂରଚନା Curl_easy *data,
                               ଅଚଳ ସଂରଚନା bufref *chlg,
                               ସଂରଚନା gsasldata *gsasl,
                               ସଂରଚନା bufref *out)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
  ଅକ୍ଷର *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ଯଦି(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_auth_gsasl_cleanup(ସଂରଚନା gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = ଖାଲି;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = ଖାଲି;
}
#endif
