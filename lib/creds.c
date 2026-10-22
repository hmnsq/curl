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


CURLcode Curl_creds_create(ثابت نویسه *user,
                           ثابت نویسه *passwd,
                           ثابت نویسه *oauth_bearer,
                           ثابت نویسه *sasl_authzid,
                           ثابت نویسه *sasl_service,
                           uint8_t source,
                           ساختار Curl_creds **pcreds)
{
  ساختار Curl_creds *creds = پوچ;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  نویسه *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  اگر(!user && !passwd && !olen && !salen && !sslen)
    برو out;

  اگر((ulen > CURL_MAX_درون_داد_LENGTH) ||
     (plen > CURL_MAX_درون_داد_LENGTH) ||
     (olen > CURL_MAX_درون_داد_LENGTH) ||
     (salen > CURL_MAX_درون_داد_LENGTH) ||
     (sslen > CURL_MAX_درون_داد_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, اندازه(*creds) + bufsize);
  اگر(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((نویسه *)creds) + offsetof(ساختار Curl_creds, buf);
  creds->user = s = buf;
  اگر(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  اگر(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  اگر(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  اگر(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  اگر(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  اگر(!result)
    *pcreds = creds;
  وگرنه
    Curl_creds_unlink(&creds);
  بازگشت result;
}

CURLcode Curl_creds_merge(ثابت نویسه *user,
                          ثابت نویسه *passwd,
                          ساختار Curl_creds *creds_in,
                          uint8_t source,
                          ساختار Curl_creds **pcreds_out)
{
  ساختار Curl_creds *creds_out = پوچ;
  CURLcode result;

  اگر(!creds_in) {
    result = Curl_creds_create(user, passwd, پوچ, پوچ, پوچ,
                               source, &creds_out);
  }
  وگرنه {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  بازگشت result;
}

تهی Curl_creds_link(ساختار Curl_creds **pdest, ساختار Curl_creds *src)
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

تهی Curl_creds_unlink(ساختار Curl_creds **pcreds)
{
  اگر(*pcreds) {
    ساختار Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = پوچ;
    اگر(creds->refcount)
      creds->refcount--;
    اگر(!creds->refcount) {
      curlx_memzero(creds, اندازه(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ساختار Curl_creds *c1, ساختار Curl_creds *c2)
{
  بازگشت (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ساختار Curl_creds *c1, ساختار Curl_creds *c2)
{
  بازگشت Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
تهی Curl_creds_trace(ساختار Curl_easy *data, ساختار Curl_creds *creds,
                      ثابت نویسه *پیام)
{
  اگر(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               پیام,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  وگرنه
    CURL_TRC_M(data, "%s: -", پیام);
}
#endif
