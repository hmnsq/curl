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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_GSASL

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_trc.h"

#अन्तर्भाव <gsasl.h>

#जर GSASL_VERSION_संख्या < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *mech,
                                  संरचना gsasldata *gsasl)
{
  पूर्णांक res;

  res = gsasl_init(&gsasl->ctx);
  जर(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    परत FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  जर(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = रिक्त;
    परत FALSE;
  }

  परत TRUE;
}

CURLcode Curl_auth_gsasl_start(संरचना Curl_easy *data,
                               संरचना Curl_creds *creds,
                               संरचना gsasldata *gsasl)
{
#जर GSASL_VERSION_संख्या >= 0x010b00
  पूर्णांक res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#जर GSASL_VERSION_संख्या >= 0x010b00
  जर(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    परत CURLE_OUT_OF_MEMORY;
  }
#endif

#जर GSASL_VERSION_संख्या >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#जर GSASL_VERSION_संख्या >= 0x010b00
  जर(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    परत CURLE_OUT_OF_MEMORY;
  }
#endif

  (शून्य)data;

  परत CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(संरचना Curl_easy *data,
                               स्थिरांक संरचना bufref *chlg,
                               संरचना gsasldata *gsasl,
                               संरचना bufref *out)
{
  पूर्णांक res;
  अक्षर *response;
  माप_प्रकार outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  जर(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    परत CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  परत CURLE_OK;
}

शून्य Curl_auth_gsasl_cleanup(संरचना gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = रिक्त;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = रिक्त;
}
#endif
