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

#यदि_परिभाषितम् USE_GSASL

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_trc.h"

#अन्तर्भाव <gsasl.h>

#यदि GSASL_संस्करणम्_सङ्ख्या < 0x010600
#दोषः "libgsasl 1.6.0 or greater required"
#यदि_अन्तः

bool Curl_auth_gsasl_is_supported(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *mech,
                                  संरचना gsasldata *gsasl)
{
  पूर्णाङ्क res;

  res = gsasl_init(&gsasl->ctx);
  यदि(res != GSASL_OK) {
    failf(दत्त, "gsasl init: %s", gsasl_strerror(res));
    प्रत्यागम FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  यदि(res != GSASL_OK) {
    gsasl_समाप्तम्(gsasl->ctx);
    gsasl->ctx = रिक्त;
    प्रत्यागम FALSE;
  }

  प्रत्यागम TRUE;
}

CURLcode Curl_auth_gsasl_start(संरचना Curl_easy *दत्त,
                               संरचना Curl_creds *creds,
                               संरचना gsasldata *gsasl)
{
#यदि GSASL_संस्करणम्_सङ्ख्या >= 0x010b00
  पूर्णाङ्क res;
  res =
#यदि_अन्तः
    gsasl_property_समुच्चयः(gsasl->client, GSASL_AUTHID, creds->user);
#यदि GSASL_संस्करणम्_सङ्ख्या >= 0x010b00
  यदि(res != GSASL_OK) {
    failf(दत्त, "setting AUTHID failed: %s", gsasl_strerror(res));
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_अन्तः

#यदि GSASL_संस्करणम्_सङ्ख्या >= 0x010b00
  res =
#यदि_अन्तः
    gsasl_property_समुच्चयः(gsasl->client, GSASL_PASSWORD, creds->passwd);
#यदि GSASL_संस्करणम्_सङ्ख्या >= 0x010b00
  यदि(res != GSASL_OK) {
    failf(दत्त, "setting PASSWORD failed: %s", gsasl_strerror(res));
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_अन्तः

  (शून्य)दत्त;

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(संरचना Curl_easy *दत्त,
                               ध्रुव संरचना bufref *chlg,
                               संरचना gsasldata *gsasl,
                               संरचना bufref *बहिर्गमनम्)
{
  पूर्णाङ्क res;
  वर्ण *response;
  size_t outlen;

  res = gsasl_चरणः(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  यदि(res != GSASL_OK && res != GSASL_NEEDS_अधिकम्) {
    failf(दत्त, "GSASL step: %s", gsasl_strerror(res));
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_समुच्चयः(बहिर्गमनम्, response, outlen, gsasl_free);
  प्रत्यागम CURLE_OK;
}

शून्य Curl_auth_gsasl_cleanup(संरचना gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = रिक्त;

  gsasl_समाप्तम्(gsasl->ctx);
  gsasl->ctx = रिक्त;
}
#यदि_अन्तः
