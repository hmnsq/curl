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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>  /* for offsetof() */

#ਅਨ੍ਤਰ੍ਭਾਵ "creds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"


CURLcode Curl_creds_create(ਅਚਲ ਅੱਖਰ *user,
                           ਅਚਲ ਅੱਖਰ *passwd,
                           ਅਚਲ ਅੱਖਰ *oauth_bearer,
                           ਅਚਲ ਅੱਖਰ *sasl_authzid,
                           ਅਚਲ ਅੱਖਰ *sasl_service,
                           uint8_t ਸਰੋਤ,
                           ਬਣਤਰ Curl_creds **pcreds)
{
  ਬਣਤਰ Curl_creds *creds = ਨਿਰਾ;
  size_t ulen = user ? ਲੜੀ_ਲੰਬਾਈ(user) : 0;
  size_t plen = passwd ? ਲੜੀ_ਲੰਬਾਈ(passwd) : 0;
  size_t olen = oauth_bearer ? ਲੜੀ_ਲੰਬਾਈ(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? ਲੜੀ_ਲੰਬਾਈ(sasl_authzid) : 0;
  size_t sslen = sasl_service ? ਲੜੀ_ਲੰਬਾਈ(sasl_service) : 0;
  ਅੱਖਰ *s, *buf;
  size_t bufsize;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  ਜੇ(!user && !passwd && !olen && !salen && !sslen)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ((ulen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) ||
     (plen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) ||
     (olen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) ||
     (salen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) ||
     (sslen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, ਆਕਾਰ(*creds) + bufsize);
  ਜੇ(!creds) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->ਸਰੋਤ = ਸਰੋਤ;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((ਅੱਖਰ *)creds) + offsetof(ਬਣਤਰ Curl_creds, buf);
  creds->user = s = buf;
  ਜੇ(ulen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  ਜੇ(plen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  ਜੇ(olen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  ਜੇ(salen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  ਜੇ(sslen)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(s, sasl_service, sslen + 1);

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ)
    *pcreds = creds;
  ਨਹੀਂਤਾਂ
    Curl_creds_unlink(&creds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_creds_ਮਿਲਾਓ(ਅਚਲ ਅੱਖਰ *user,
                          ਅਚਲ ਅੱਖਰ *passwd,
                          ਬਣਤਰ Curl_creds *creds_ਵਿੱਚ,
                          uint8_t ਸਰੋਤ,
                          ਬਣਤਰ Curl_creds **pcreds_ਬਾਹਰਲਾ)
{
  ਬਣਤਰ Curl_creds *creds_ਬਾਹਰਲਾ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!creds_ਵਿੱਚ) {
    ਨਤੀਜਾ = Curl_creds_create(user, passwd, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                               ਸਰੋਤ, &creds_ਬਾਹਰਲਾ);
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_creds_create(user ? user : Curl_creds_user(creds_ਵਿੱਚ),
                               passwd ? passwd : Curl_creds_passwd(creds_ਵਿੱਚ),
                               Curl_creds_oauth_bearer(creds_ਵਿੱਚ),
                               Curl_creds_sasl_authzid(creds_ਵਿੱਚ),
                               Curl_creds_sasl_service(creds_ਵਿੱਚ),
                               ਸਰੋਤ, &creds_ਬਾਹਰਲਾ);
  }
  Curl_creds_link(pcreds_ਬਾਹਰਲਾ, creds_ਬਾਹਰਲਾ);
  Curl_creds_unlink(&creds_ਬਾਹਰਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_creds_link(ਬਣਤਰ Curl_creds **pdest, ਬਣਤਰ Curl_creds *src)
{
  ਜੇ(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    ਜੇ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ਖਾਲੀ Curl_creds_unlink(ਬਣਤਰ Curl_creds **pcreds)
{
  ਜੇ(*pcreds) {
    ਬਣਤਰ Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ਨਿਰਾ;
    ਜੇ(creds->refcount)
      creds->refcount--;
    ਜੇ(!creds->refcount) {
      curlx_memzero(creds, ਆਕਾਰ(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ਬਣਤਰ Curl_creds *c1, ਬਣਤਰ Curl_creds *c2)
{
  ਵਾਪਸ (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ਬਣਤਰ Curl_creds *c1, ਬਣਤਰ Curl_creds *c2)
{
  ਵਾਪਸ Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->ਸਰੋਤ == c2->ਸਰੋਤ)));
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_creds_trace(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creds *creds,
                      ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
  ਜੇ(creds) {
    CURL_TRC_M(ਅੰਕੜੇ, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               ਸੁਨੇਹਾ,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->ਸਰੋਤ);
  }
  ਨਹੀਂਤਾਂ
    CURL_TRC_M(ਅੰਕੜੇ, "%s: -", ਸੁਨੇਹਾ);
}
#ਜੇ_ਅੰਤ
