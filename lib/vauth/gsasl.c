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

#यदि GSASL_VERSION_सङ्ख्या < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(संरचना Curl_easy *data,
                                  अचर वर्ण *mech,
                                  संरचना gsasldata *gsasl)
{
  पूर्णाङ्क res;

  res = gsasl_init(&gsasl->ctx);
  यदि(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    फर्काऊ FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  यदि(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = रिक्त;
    फर्काऊ FALSE;
  }

  फर्काऊ TRUE;
}

CURLcode Curl_auth_gsasl_start(संरचना Curl_easy *data,
                               संरचना Curl_creds *creds,
                               संरचना gsasldata *gsasl)
{
#यदि GSASL_VERSION_सङ्ख्या >= 0x010b00
  पूर्णाङ्क res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#यदि GSASL_VERSION_सङ्ख्या >= 0x010b00
  यदि(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    फर्काऊ CURLE_OUT_OF_MEMORY;
  }
#endif

#यदि GSASL_VERSION_सङ्ख्या >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#यदि GSASL_VERSION_सङ्ख्या >= 0x010b00
  यदि(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    फर्काऊ CURLE_OUT_OF_MEMORY;
  }
#endif

  (शून्य)data;

  फर्काऊ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(संरचना Curl_easy *data,
                               अचर संरचना bufref *chlg,
                               संरचना gsasldata *gsasl,
                               संरचना bufref *out)
{
  पूर्णाङ्क res;
  वर्ण *response;
  माप_प्रकार outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  यदि(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    फर्काऊ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  फर्काऊ CURLE_OK;
}

शून्य Curl_auth_gsasl_cleanup(संरचना gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = रिक्त;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = रिक्त;
}
#endif
