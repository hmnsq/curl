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
#شامل "curl_setup.h"

#شامل <stddef.h>  /* for offsetof() */

#شامل "creds.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "urldata.h"


CURLcode Curl_creds_create(ثابت حرف *user,
                           ثابت حرف *passwd,
                           ثابت حرف *oauth_bearer,
                           ثابت حرف *sasl_authzid,
                           ثابت حرف *sasl_service,
                           uint8_t ماخذ,
                           ساخت Curl_creds **pcreds)
{
  ساخت Curl_creds *creds = تہی;
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
  اگر(!user && !passwd && !olen && !salen && !sslen)
    جاؤ برآمد;

  اگر((ulen > CURL_MAX_داخلہ_لمبائی) ||
     (plen > CURL_MAX_داخلہ_لمبائی) ||
     (olen > CURL_MAX_داخلہ_لمبائی) ||
     (salen > CURL_MAX_داخلہ_لمبائی) ||
     (sslen > CURL_MAX_داخلہ_لمبائی)) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  اگر(!creds) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->ماخذ = ماخذ;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(ساخت Curl_creds, buf);
  creds->user = s = buf;
  اگر(ulen)
    حافظہ_نقل(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  اگر(plen)
    حافظہ_نقل(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  اگر(olen)
    حافظہ_نقل(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  اگر(salen)
    حافظہ_نقل(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  اگر(sslen)
    حافظہ_نقل(s, sasl_service, sslen + 1);

برآمد:
  اگر(!نتیجہ)
    *pcreds = creds;
  ورنہ
    Curl_creds_unlink(&creds);
  واپس نتیجہ;
}

CURLcode Curl_creds_ملاؤ(ثابت حرف *user,
                          ثابت حرف *passwd,
                          ساخت Curl_creds *creds_اندر,
                          uint8_t ماخذ,
                          ساخت Curl_creds **pcreds_برآمد)
{
  ساخت Curl_creds *creds_برآمد = تہی;
  CURLcode نتیجہ;

  اگر(!creds_اندر) {
    نتیجہ = Curl_creds_create(user, passwd, تہی, تہی, تہی,
                               ماخذ, &creds_برآمد);
  }
  ورنہ {
    نتیجہ = Curl_creds_create(user ? user : Curl_creds_user(creds_اندر),
                               passwd ? passwd : Curl_creds_passwd(creds_اندر),
                               Curl_creds_oauth_bearer(creds_اندر),
                               Curl_creds_sasl_authzid(creds_اندر),
                               Curl_creds_sasl_service(creds_اندر),
                               ماخذ, &creds_برآمد);
  }
  Curl_creds_link(pcreds_برآمد, creds_برآمد);
  Curl_creds_unlink(&creds_برآمد);
  واپس نتیجہ;
}

خالی Curl_creds_link(ساخت Curl_creds **pdest, ساخت Curl_creds *src)
{
  اگر(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    اگر(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

خالی Curl_creds_unlink(ساخت Curl_creds **pcreds)
{
  اگر(*pcreds) {
    ساخت Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = تہی;
    اگر(creds->refcount)
      creds->refcount--;
    اگر(!creds->refcount) {
      curlx_memzero(creds, حجم(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ساخت Curl_creds *c1, ساخت Curl_creds *c2)
{
  واپس (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ساخت Curl_creds *c1, ساخت Curl_creds *c2)
{
  واپس Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->ماخذ == c2->ماخذ)));
}

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_creds_trace(ساخت Curl_easy *ڈیٹا, ساخت Curl_creds *creds,
                      ثابت حرف *پیغام)
{
  اگر(creds) {
    CURL_TRC_M(ڈیٹا, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               پیغام,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->ماخذ);
  }
  ورنہ
    CURL_TRC_M(ڈیٹا, "%s: -", پیغام);
}
#اگر_ختم
