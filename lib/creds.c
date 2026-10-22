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
                           uint8_t سرچینه,
                           بنية Curl_creds **pcreds)
{
  بنية Curl_creds *creds = عدم;
  size_t ulen = user ? مزي_اوږدوالی(user) : 0;
  size_t plen = passwd ? مزي_اوږدوالی(passwd) : 0;
  size_t olen = oauth_bearer ? مزي_اوږدوالی(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? مزي_اوږدوالی(sasl_authzid) : 0;
  size_t sslen = sasl_service ? مزي_اوږدوالی(sasl_service) : 0;
  حرف *s, *buf;
  size_t bufsize;
  CURLcode پایله = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  إذا(!user && !passwd && !olen && !salen && !sslen)
    اذهب بهر;

  إذا((ulen > CURL_MAX_ننوت_اوږدتیا) ||
     (plen > CURL_MAX_ننوت_اوږدتیا) ||
     (olen > CURL_MAX_ننوت_اوږدتیا) ||
     (salen > CURL_MAX_ننوت_اوږدتیا) ||
     (sslen > CURL_MAX_ننوت_اوږدتیا)) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  إذا(!creds) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->سرچینه = سرچینه;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(بنية Curl_creds, buf);
  creds->user = s = buf;
  إذا(ulen)
    حافظه_نقل(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  إذا(plen)
    حافظه_نقل(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  إذا(olen)
    حافظه_نقل(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  إذا(salen)
    حافظه_نقل(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  إذا(sslen)
    حافظه_نقل(s, sasl_service, sslen + 1);

بهر:
  إذا(!پایله)
    *pcreds = creds;
  وإلا
    Curl_creds_unlink(&creds);
  أرجع پایله;
}

CURLcode Curl_creds_یوځای(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_دننه,
                          uint8_t سرچینه,
                          بنية Curl_creds **pcreds_بهر)
{
  بنية Curl_creds *creds_بهر = عدم;
  CURLcode پایله;

  إذا(!creds_دننه) {
    پایله = Curl_creds_create(user, passwd, عدم, عدم, عدم,
                               سرچینه, &creds_بهر);
  }
  وإلا {
    پایله = Curl_creds_create(user ? user : Curl_creds_user(creds_دننه),
                               passwd ? passwd : Curl_creds_passwd(creds_دننه),
                               Curl_creds_oauth_bearer(creds_دننه),
                               Curl_creds_sasl_authzid(creds_دننه),
                               Curl_creds_sasl_service(creds_دننه),
                               سرچینه, &creds_بهر);
  }
  Curl_creds_link(pcreds_بهر, creds_بهر);
  Curl_creds_unlink(&creds_بهر);
  أرجع پایله;
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
         ((c1 == c2) || (c1 && c2 && (c1->سرچینه == c2->سرچینه)));
}

#که_تعریف_شوی CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *معلومات, بنية Curl_creds *creds,
                      ثابت حرف *پیغام)
{
  إذا(creds) {
    CURL_TRC_M(معلومات, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               پیغام,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->سرچینه);
  }
  وإلا
    CURL_TRC_M(معلومات, "%s: -", پیغام);
}
#د_که_پای
