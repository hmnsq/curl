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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_GSASL

#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "curl_trc.h"

#അന്തര്ഭാവ <gsasl.h>

#എങ്കിൽ GSASL_VERSION_സംഖ്യ < 0x010600
#error "libgsasl 1.6.0 or greater required"
#endif

bool Curl_auth_gsasl_is_supported(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *mech,
                                  ഘടന gsasldata *gsasl)
{
  പൂർണ്ണസംഖ്യ res;

  res = gsasl_init(&gsasl->ctx);
  എങ്കിൽ(res != GSASL_OK) {
    failf(data, "gsasl init: %s", gsasl_strerror(res));
    തിരികെ FALSE;
  }

  res = gsasl_client_start(gsasl->ctx, mech, &gsasl->client);
  എങ്കിൽ(res != GSASL_OK) {
    gsasl_done(gsasl->ctx);
    gsasl->ctx = ശൂന്യ;
    തിരികെ FALSE;
  }

  തിരികെ TRUE;
}

CURLcode Curl_auth_gsasl_start(ഘടന Curl_easy *data,
                               ഘടന Curl_creds *creds,
                               ഘടന gsasldata *gsasl)
{
#എങ്കിൽ GSASL_VERSION_സംഖ്യ >= 0x010b00
  പൂർണ്ണസംഖ്യ res;
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_AUTHID, creds->user);
#എങ്കിൽ GSASL_VERSION_സംഖ്യ >= 0x010b00
  എങ്കിൽ(res != GSASL_OK) {
    failf(data, "setting AUTHID failed: %s", gsasl_strerror(res));
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
#endif

#എങ്കിൽ GSASL_VERSION_സംഖ്യ >= 0x010b00
  res =
#endif
    gsasl_property_set(gsasl->client, GSASL_PASSWORD, creds->passwd);
#എങ്കിൽ GSASL_VERSION_സംഖ്യ >= 0x010b00
  എങ്കിൽ(res != GSASL_OK) {
    failf(data, "setting PASSWORD failed: %s", gsasl_strerror(res));
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
#endif

  (ശൂന്യം)data;

  തിരികെ CURLE_OK;
}

CURLcode Curl_auth_gsasl_token(ഘടന Curl_easy *data,
                               സ്ഥിരം ഘടന bufref *chlg,
                               ഘടന gsasldata *gsasl,
                               ഘടന bufref *out)
{
  പൂർണ്ണസംഖ്യ res;
  അക്ഷരം *response;
  size_t outlen;

  res = gsasl_step(gsasl->client, Curl_bufref_ptr(chlg), Curl_bufref_len(chlg),
                   &response, &outlen);
  എങ്കിൽ(res != GSASL_OK && res != GSASL_NEEDS_MORE) {
    failf(data, "GSASL step: %s", gsasl_strerror(res));
    തിരികെ CURLE_BAD_CONTENT_ENCODING;
  }

  Curl_bufref_set(out, response, outlen, gsasl_free);
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_auth_gsasl_cleanup(ഘടന gsasldata *gsasl)
{
  gsasl_finish(gsasl->client);
  gsasl->client = ശൂന്യ;

  gsasl_done(gsasl->ctx);
  gsasl->ctx = ശൂന്യ;
}
#endif
