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
#ضمن "curl_setup.h"

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "creds.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "urldata.h"


CURLcode Curl_creds_create(ثابت حرف *user,
                           ثابت حرف *passwd,
                           ثابت حرف *oauth_bearer,
                           ثابت حرف *sasl_authzid,
                           ثابت حرف *sasl_service,
                           uint8_t سروت,
                           بنية Curl_creds **pcreds)
{
  بنية Curl_creds *creds = عدم;
  size_t ulen = user ? لڑی_لمبائی(user) : 0;
  size_t plen = passwd ? لڑی_لمبائی(passwd) : 0;
  size_t olen = oauth_bearer ? لڑی_لمبائی(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? لڑی_لمبائی(sasl_authzid) : 0;
  size_t sslen = sasl_service ? لڑی_لمبائی(sasl_service) : 0;
  حرف *s, *buf;
  size_t bufsize;
  CURLcode نتیجہ = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  إذا(!user && !passwd && !olen && !salen && !sslen)
    اذهب باہرلا;

  إذا((ulen > CURL_MAX_داخلہ_لمائی) ||
     (plen > CURL_MAX_داخلہ_لمائی) ||
     (olen > CURL_MAX_داخلہ_لمائی) ||
     (salen > CURL_MAX_داخلہ_لمائی) ||
     (sslen > CURL_MAX_داخلہ_لمائی)) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  إذا(!creds) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->سروت = سروت;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(بنية Curl_creds, buf);
  creds->user = s = buf;
  إذا(ulen)
    حافظہ_نقل(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  إذا(plen)
    حافظہ_نقل(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  إذا(olen)
    حافظہ_نقل(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  إذا(salen)
    حافظہ_نقل(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  إذا(sslen)
    حافظہ_نقل(s, sasl_service, sslen + 1);

باہرلا:
  إذا(!نتیجہ)
    *pcreds = creds;
  وإلا
    Curl_creds_unlink(&creds);
  أرجع نتیجہ;
}

CURLcode Curl_creds_ملاؤ(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_وچ,
                          uint8_t سروت,
                          بنية Curl_creds **pcreds_باہرلا)
{
  بنية Curl_creds *creds_باہرلا = عدم;
  CURLcode نتیجہ;

  إذا(!creds_وچ) {
    نتیجہ = Curl_creds_create(user, passwd, عدم, عدم, عدم,
                               سروت, &creds_باہرلا);
  }
  وإلا {
    نتیجہ = Curl_creds_create(user ? user : Curl_creds_user(creds_وچ),
                               passwd ? passwd : Curl_creds_passwd(creds_وچ),
                               Curl_creds_oauth_bearer(creds_وچ),
                               Curl_creds_sasl_authzid(creds_وچ),
                               Curl_creds_sasl_service(creds_وچ),
                               سروت, &creds_باہرلا);
  }
  Curl_creds_link(pcreds_باہرلا, creds_باہرلا);
  Curl_creds_unlink(&creds_باہرلا);
  أرجع نتیجہ;
}

فراغ Curl_creds_link(بنية Curl_creds **pdest, بنية Curl_creds *src)
{
  إذا(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    إذا(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

فراغ Curl_creds_unlink(بنية Curl_creds **pcreds)
{
  إذا(*pcreds) {
    بنية Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = عدم;
    إذا(creds->refcount)
      creds->refcount--;
    إذا(!creds->refcount) {
      curlx_memzero(creds, حجم(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(بنية Curl_creds *c1, بنية Curl_creds *c2)
{
  أرجع (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(بنية Curl_creds *c1, بنية Curl_creds *c2)
{
  أرجع Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->سروت == c2->سروت)));
}

#جے_تعریف_شدہ CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *انکڑے, بنية Curl_creds *creds,
                      ثابت حرف *پیغام)
{
  إذا(creds) {
    CURL_TRC_M(انکڑے, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               پیغام,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->سروت);
  }
  وإلا
    CURL_TRC_M(انکڑے, "%s: -", پیغام);
}
#جے_انت
