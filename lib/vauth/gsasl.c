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
#समावेश "curl_setup.h"

#अगर_घोषित USE_GSASL

#समावेश "vauth/vauth.h"
#समावेश "curl_trc.h"

#समावेश <gsasl.h>

#अगर GSASL_VERSION_संख्या < 0x010600
#त्रुटि "libgsasl 1.6.0 or greater required"
#पूर्ण_अगर

bool Curl_auth_gsasl_is_supported(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *mech,
                                  काष्ठा gsasldata *gsasl)
{
  पूर्णांक res;

  res = gsasl_init(&gsasl->ctx);
  अगर(res != GSASL_OK) {
    failf(तथ्य, "gsasl init: %s", gsasl_strerror(res));
    वापस FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  अगर(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = शून्य;
    वापस FALSE;
  }

  वापस TRUE;
}

CURLcode Curl_auth_gsasl_start(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creds *creds,
                               काष्ठा gsasldata *gsasl)
{
#अगर GSASL_VERSION_संख्या >= 0x010b00
  पूर्णांक res;
  res =
#पूर्ण_अगर
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#अगर GSASL_VERSION_संख्या >= 0x010b00
  अगर(res != GSASL_OK) {
    failf(तथ्य, "setting AUTHID failed: %s", gsasl_strerror(res));
    वापस CURLE_OUT_OF_MEMORY;
  }
#पूर्ण_अगर

#अगर GSASL_VERSION_संख्या >= 0x010b00
  res =
#पूर्ण_अगर
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#अगर GSASL_VERSION_संख्या >= 0x010b00
  अगर(res != GSASL_OK) {
    failf(तथ्य, "setting PASSWORD failed: %s", gsasl_strerror(res));
    वापस CURLE_OUT_OF_MEMORY;
  }
#पूर्ण_अगर

  (व्योम)तथ्य;

  वापस CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(काष्ठा Curl_easy *तथ्य,
                               स्थिर काष्ठा bufref *chlg,
                               काष्ठा gsasldata *gsasl,
                               काष्ठा bufref *out)
{
  पूर्णांक res;
  अक्षर *response;
  माप_प्रकार outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  अगर(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(तथ्य, "GSASL step: %s", gsasl_strerror(res));
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  वापस CURLE_OK;
}

व्योम Curl_auth_gsasl_cleanup(काष्ठा gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = शून्य;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = शून्य;
}
#पूर्ण_अगर
