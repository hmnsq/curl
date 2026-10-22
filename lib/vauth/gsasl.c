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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_GSASL

#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "curl_trc.h"

#અન્તર્ભાવ <gsasl.h>

#જો GSASL_VERSION_સંખ્યા < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(સંરચના Curl_easy *data,
                                  અચલ અક્ષર *mech,
                                  સંરચના gsasldata *gsasl)
{
  પૂર્ણાંક res;

  res = gsasl_init(&gsasl->ctx);
  જો(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    પાછા FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  જો(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = ખાલી;
    પાછા FALSE;
  }

  પાછા TRUE;
}

CURLcode Curl_auth_gsasl_start(સંરચના Curl_easy *data,
                               સંરચના Curl_creds *creds,
                               સંરચના gsasldata *gsasl)
{
#જો GSASL_VERSION_સંખ્યા >= 0x010b00
  પૂર્ણાંક res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#જો GSASL_VERSION_સંખ્યા >= 0x010b00
  જો(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    પાછા CURLE_OUT_OF_MEMORY;
  }
#endif

#જો GSASL_VERSION_સંખ્યા >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#જો GSASL_VERSION_સંખ્યા >= 0x010b00
  જો(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    પાછા CURLE_OUT_OF_MEMORY;
  }
#endif

  (શૂન્ય)data;

  પાછા CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(સંરચના Curl_easy *data,
                               અચલ સંરચના bufref *chlg,
                               સંરચના gsasldata *gsasl,
                               સંરચના bufref *out)
{
  પૂર્ણાંક res;
  અક્ષર *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  જો(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    પાછા CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  પાછા CURLE_OK;
}

શૂન્ય Curl_auth_gsasl_cleanup(સંરચના gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = ખાલી;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = ખાલી;
}
#endif
