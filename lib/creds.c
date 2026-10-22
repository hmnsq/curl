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
                           uint8_t ذريعو,
                           بنية Curl_creds **pcreds)
{
  بنية Curl_creds *creds = عدم;
  size_t ulen = user ? لڙي_ڊيگهه(user) : 0;
  size_t plen = passwd ? لڙي_ڊيگهه(passwd) : 0;
  size_t olen = oauth_bearer ? لڙي_ڊيگهه(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? لڙي_ڊيگهه(sasl_authzid) : 0;
  size_t sslen = sasl_service ? لڙي_ڊيگهه(sasl_service) : 0;
  حرف *s, *buf;
  size_t bufsize;
  CURLcode نتيجو = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  إذا(!user && !passwd && !olen && !salen && !sslen)
    اذهب ٻاهريون;

  إذا((ulen > CURL_MAX_داخلا_ڊيگهائي) ||
     (plen > CURL_MAX_داخلا_ڊيگهائي) ||
     (olen > CURL_MAX_داخلا_ڊيگهائي) ||
     (salen > CURL_MAX_داخلا_ڊيگهائي) ||
     (sslen > CURL_MAX_داخلا_ڊيگهائي)) {
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  إذا(!creds) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->ذريعو = ذريعو;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(بنية Curl_creds, buf);
  creds->user = s = buf;
  إذا(ulen)
    يادگيري_نقل(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  إذا(plen)
    يادگيري_نقل(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  إذا(olen)
    يادگيري_نقل(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  إذا(salen)
    يادگيري_نقل(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  إذا(sslen)
    يادگيري_نقل(s, sasl_service, sslen + 1);

ٻاهريون:
  إذا(!نتيجو)
    *pcreds = creds;
  وإلا
    Curl_creds_unlink(&creds);
  أرجع نتيجو;
}

CURLcode Curl_creds_ملايو(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_اندريون,
                          uint8_t ذريعو,
                          بنية Curl_creds **pcreds_ٻاهريون)
{
  بنية Curl_creds *creds_ٻاهريون = عدم;
  CURLcode نتيجو;

  إذا(!creds_اندريون) {
    نتيجو = Curl_creds_create(user, passwd, عدم, عدم, عدم,
                               ذريعو, &creds_ٻاهريون);
  }
  وإلا {
    نتيجو = Curl_creds_create(user ? user : Curl_creds_user(creds_اندريون),
                               passwd ? passwd : Curl_creds_passwd(creds_اندريون),
                               Curl_creds_oauth_bearer(creds_اندريون),
                               Curl_creds_sasl_authzid(creds_اندريون),
                               Curl_creds_sasl_service(creds_اندريون),
                               ذريعو, &creds_ٻاهريون);
  }
  Curl_creds_link(pcreds_ٻاهريون, creds_ٻاهريون);
  Curl_creds_unlink(&creds_ٻاهريون);
  أرجع نتيجو;
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
         ((c1 == c2) || (c1 && c2 && (c1->ذريعو == c2->ذريعو)));
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *معلومات, بنية Curl_creds *creds,
                      ثابت حرف *نياپو)
{
  إذا(creds) {
    CURL_TRC_M(معلومات, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               نياپو,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->ذريعو);
  }
  وإلا
    CURL_TRC_M(معلومات, "%s: -", نياپو);
}
#جيڪڏهن_پورو
