/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
 ***************************************************************************/
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ <stddef.h>  /* for offsetof() */

#അന്തര്ഭാവ "creds.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "urldata.h"


CURLcode Curl_creds_create(സ്ഥിരം അക്ഷരം *user,
                           സ്ഥിരം അക്ഷരം *passwd,
                           സ്ഥിരം അക്ഷരം *oauth_bearer,
                           സ്ഥിരം അക്ഷരം *sasl_authzid,
                           സ്ഥിരം അക്ഷരം *sasl_service,
                           uint8_t source,
                           ഘടന Curl_creds **pcreds)
{
  ഘടന Curl_creds *creds = ശൂന്യ;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  അക്ഷരം *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  എങ്കിൽ(!user && !passwd && !olen && !salen && !sslen)
    പോകുക out;

  എങ്കിൽ((ulen > CURL_MAX_നിവേശം_LENGTH) ||
     (plen > CURL_MAX_നിവേശം_LENGTH) ||
     (olen > CURL_MAX_നിവേശം_LENGTH) ||
     (salen > CURL_MAX_നിവേശം_LENGTH) ||
     (sslen > CURL_MAX_നിവേശം_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, വലുപ്പം(*creds) + bufsize);
  എങ്കിൽ(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((അക്ഷരം *)creds) + offsetof(ഘടന Curl_creds, buf);
  creds->user = s = buf;
  എങ്കിൽ(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  എങ്കിൽ(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  എങ്കിൽ(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  എങ്കിൽ(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  എങ്കിൽ(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  എങ്കിൽ(!result)
    *pcreds = creds;
  അല്ലെങ്കിൽ
    Curl_creds_unlink(&creds);
  തിരികെ result;
}

CURLcode Curl_creds_merge(സ്ഥിരം അക്ഷരം *user,
                          സ്ഥിരം അക്ഷരം *passwd,
                          ഘടന Curl_creds *creds_in,
                          uint8_t source,
                          ഘടന Curl_creds **pcreds_out)
{
  ഘടന Curl_creds *creds_out = ശൂന്യ;
  CURLcode result;

  എങ്കിൽ(!creds_in) {
    result = Curl_creds_create(user, passwd, ശൂന്യ, ശൂന്യ, ശൂന്യ,
                               source, &creds_out);
  }
  അല്ലെങ്കിൽ {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  തിരികെ result;
}

ശൂന്യം Curl_creds_link(ഘടന Curl_creds **pdest, ഘടന Curl_creds *src)
{
  എങ്കിൽ(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    എങ്കിൽ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ശൂന്യം Curl_creds_unlink(ഘടന Curl_creds **pcreds)
{
  എങ്കിൽ(*pcreds) {
    ഘടന Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ശൂന്യ;
    എങ്കിൽ(creds->refcount)
      creds->refcount--;
    എങ്കിൽ(!creds->refcount) {
      curlx_memzero(creds, വലുപ്പം(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ഘടന Curl_creds *c1, ഘടന Curl_creds *c2)
{
  തിരികെ (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ഘടന Curl_creds *c1, ഘടന Curl_creds *c2)
{
  തിരികെ Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
ശൂന്യം Curl_creds_trace(ഘടന Curl_easy *data, ഘടന Curl_creds *creds,
                      സ്ഥിരം അക്ഷരം *സന്ദേശം)
{
  എങ്കിൽ(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               സന്ദേശം,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  അല്ലെങ്കിൽ
    CURL_TRC_M(data, "%s: -", സന്ദേശം);
}
#endif
