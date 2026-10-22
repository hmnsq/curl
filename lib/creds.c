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
                           uint8_t منبع,
                           بنية Curl_creds **pcreds)
{
  بنية Curl_creds *creds = عدم;
  size_t ulen = user ? طول_رشته(user) : 0;
  size_t plen = passwd ? طول_رشته(passwd) : 0;
  size_t olen = oauth_bearer ? طول_رشته(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? طول_رشته(sasl_authzid) : 0;
  size_t sslen = sasl_service ? طول_رشته(sasl_service) : 0;
  حرف *s, *buf;
  size_t bufsize;
  CURLcode نتیجه = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  إذا(!user && !passwd && !olen && !salen && !sslen)
    اذهب بیرون;

  إذا((ulen > CURL_MAX_درون_داد_درازی) ||
     (plen > CURL_MAX_درون_داد_درازی) ||
     (olen > CURL_MAX_درون_داد_درازی) ||
     (salen > CURL_MAX_درون_داد_درازی) ||
     (sslen > CURL_MAX_درون_داد_درازی)) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  إذا(!creds) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->منبع = منبع;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(بنية Curl_creds, buf);
  creds->user = s = buf;
  إذا(ulen)
    کپی_حافظه(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  إذا(plen)
    کپی_حافظه(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  إذا(olen)
    کپی_حافظه(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  إذا(salen)
    کپی_حافظه(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  إذا(sslen)
    کپی_حافظه(s, sasl_service, sslen + 1);

بیرون:
  إذا(!نتیجه)
    *pcreds = creds;
  وإلا
    Curl_creds_unlink(&creds);
  أرجع نتیجه;
}

CURLcode Curl_creds_ادغام(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_درون,
                          uint8_t منبع,
                          بنية Curl_creds **pcreds_بیرون)
{
  بنية Curl_creds *creds_بیرون = عدم;
  CURLcode نتیجه;

  إذا(!creds_درون) {
    نتیجه = Curl_creds_create(user, passwd, عدم, عدم, عدم,
                               منبع, &creds_بیرون);
  }
  وإلا {
    نتیجه = Curl_creds_create(user ? user : Curl_creds_user(creds_درون),
                               passwd ? passwd : Curl_creds_passwd(creds_درون),
                               Curl_creds_oauth_bearer(creds_درون),
                               Curl_creds_sasl_authzid(creds_درون),
                               Curl_creds_sasl_service(creds_درون),
                               منبع, &creds_بیرون);
  }
  Curl_creds_link(pcreds_بیرون, creds_بیرون);
  Curl_creds_unlink(&creds_بیرون);
  أرجع نتیجه;
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
         ((c1 == c2) || (c1 && c2 && (c1->منبع == c2->منبع)));
}

#اگر_تعریف_شده CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *داده, بنية Curl_creds *creds,
                      ثابت حرف *پیام)
{
  إذا(creds) {
    CURL_TRC_M(داده, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               پیام,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->منبع);
  }
  وإلا
    CURL_TRC_M(داده, "%s: -", پیام);
}
#پایان_اگر
