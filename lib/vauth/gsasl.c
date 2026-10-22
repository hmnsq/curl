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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_GSASL

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_trc.h"

#అన్తర్భావ <gsasl.h>

#ఒకవేళ GSASL_సంస్కరణ_సంఖ్య < 0x010600
#దోషం "libgsasl 1.6.0 or greater required"
#అయితే_ముగింపు

bool Curl_auth_gsasl_is_supported(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *mech,
                                  నిర్మాణం gsasldata *gsasl)
{
  పూర్ణాంకం res;

  res = gsasl_init(&gsasl->ctx);
  ఒకవేళ(res != GSASL_OK) {
    failf(దత్తాంశం, "gsasl init: %s", gsasl_strerror(res));
    తిరిగి FALSE;
  }

  res = gsasl_client_ప్రారంభం(gsasl->ctx, mech, &gsasl->client);
  ఒకవేళ(res != GSASL_OK) {
    gsasl_పూర్తి(gsasl->ctx);
    gsasl->ctx = ఖాళీ;
    తిరిగి FALSE;
  }

  తిరిగి TRUE;
}

CURLcode Curl_auth_gsasl_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creds *creds,
                               నిర్మాణం gsasldata *gsasl)
{
#ఒకవేళ GSASL_సంస్కరణ_సంఖ్య >= 0x010b00
  పూర్ణాంకం res;
  res =
#అయితే_ముగింపు
    gsasl_property_సమితి(gsasl->client, GSASL_AUTHID, creds->user);
#ఒకవేళ GSASL_సంస్కరణ_సంఖ్య >= 0x010b00
  ఒకవేళ(res != GSASL_OK) {
    failf(దత్తాంశం, "setting AUTHID failed: %s", gsasl_strerror(res));
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#అయితే_ముగింపు

#ఒకవేళ GSASL_సంస్కరణ_సంఖ్య >= 0x010b00
  res =
#అయితే_ముగింపు
    gsasl_property_సమితి(gsasl->client, GSASL_PASSWORD, creds->passwd);
#ఒకవేళ GSASL_సంస్కరణ_సంఖ్య >= 0x010b00
  ఒకవేళ(res != GSASL_OK) {
    failf(దత్తాంశం, "setting PASSWORD failed: %s", gsasl_strerror(res));
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#అయితే_ముగింపు

  (శూన్య)దత్తాంశం;

  తిరిగి CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి నిర్మాణం bufref *chlg,
                               నిర్మాణం gsasldata *gsasl,
                               నిర్మాణం bufref *బహిర్గతం)
{
  పూర్ణాంకం res;
  అక్షరం *response;
  size_t outlen;

  res = gsasl_అడుగు(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  ఒకవేళ(res != GSASL_OK && res != GSASL_NEEDS_మరిన్ని) {
    failf(దత్తాంశం, "GSASL step: %s", gsasl_strerror(res));
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_సమితి(బహిర్గతం, response, outlen, gsasl_free);
  తిరిగి CURLE_OK;
}

శూన్య Curl_auth_gsasl_cleanup(నిర్మాణం gsasldata *gsasl)
{
  gsasl_ముగింపు(gsasl->client);
  gsasl->client = ఖాళీ;

  gsasl_పూర్తి(gsasl->ctx);
  gsasl->ctx = ఖాళీ;
}
#అయితే_ముగింపు
