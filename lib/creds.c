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
                           uint8_t source,
                           بنية Curl_creds **pcreds)
{
  بنية Curl_creds *creds = عدم;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  حرف *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  إذا(!user && !passwd && !olen && !salen && !sslen)
    اذهب out;

  إذا((ulen > CURL_MAX_ماسوقن_LENGTH) ||
     (plen > CURL_MAX_ماسوقن_LENGTH) ||
     (olen > CURL_MAX_ماسوقن_LENGTH) ||
     (salen > CURL_MAX_ماسوقن_LENGTH) ||
     (sslen > CURL_MAX_ماسوقن_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, حجم(*creds) + bufsize);
  إذا(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((حرف *)creds) + offsetof(بنية Curl_creds, buf);
  creds->user = s = buf;
  إذا(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  إذا(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  إذا(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  إذا(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  إذا(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  إذا(!result)
    *pcreds = creds;
  وإلا
    Curl_creds_unlink(&creds);
  أرجع result;
}

CURLcode Curl_creds_merge(ثابت حرف *user,
                          ثابت حرف *passwd,
                          بنية Curl_creds *creds_in,
                          uint8_t source,
                          بنية Curl_creds **pcreds_out)
{
  بنية Curl_creds *creds_out = عدم;
  CURLcode result;

  إذا(!creds_in) {
    result = Curl_creds_create(user, passwd, عدم, عدم, عدم,
                               source, &creds_out);
  }
  وإلا {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  أرجع result;
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
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
فراغ Curl_creds_trace(بنية Curl_easy *data, بنية Curl_creds *creds,
                      ثابت حرف *ڤسنن)
{
  إذا(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               ڤسنن,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  وإلا
    CURL_TRC_M(data, "%s: -", ڤسنن);
}
#endif
