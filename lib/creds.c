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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব <stddef.h>  /* for offsetof() */

#অন্তর্ভাব "creds.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "urldata.h"


CURLcode Curl_creds_create(ধ্রুব বর্ণ *user,
                           ধ্রুব বর্ণ *passwd,
                           ধ্রুব বর্ণ *oauth_bearer,
                           ধ্রুব বর্ণ *sasl_authzid,
                           ধ্রুব বর্ণ *sasl_service,
                           uint8_t source,
                           সংরচনা Curl_creds **pcreds)
{
  সংরচনা Curl_creds *creds = রিক্ত;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  বর্ণ *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  যদি(!user && !passwd && !olen && !salen && !sslen)
    গচ্ছ out;

  যদি((ulen > CURL_MAX_প্রবেশ_LENGTH) ||
     (plen > CURL_MAX_প্রবেশ_LENGTH) ||
     (olen > CURL_MAX_প্রবেশ_LENGTH) ||
     (salen > CURL_MAX_প্রবেশ_LENGTH) ||
     (sslen > CURL_MAX_প্রবেশ_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, পরিমাণ(*creds) + bufsize);
  যদি(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((বর্ণ *)creds) + offsetof(সংরচনা Curl_creds, buf);
  creds->user = s = buf;
  যদি(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  যদি(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  যদি(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  যদি(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  যদি(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  যদি(!result)
    *pcreds = creds;
  অন্যথা
    Curl_creds_unlink(&creds);
  প্রত্যাগম result;
}

CURLcode Curl_creds_merge(ধ্রুব বর্ণ *user,
                          ধ্রুব বর্ণ *passwd,
                          সংরচনা Curl_creds *creds_in,
                          uint8_t source,
                          সংরচনা Curl_creds **pcreds_out)
{
  সংরচনা Curl_creds *creds_out = রিক্ত;
  CURLcode result;

  যদি(!creds_in) {
    result = Curl_creds_create(user, passwd, রিক্ত, রিক্ত, রিক্ত,
                               source, &creds_out);
  }
  অন্যথা {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  প্রত্যাগম result;
}

শূন্য Curl_creds_link(সংরচনা Curl_creds **pdest, সংরচনা Curl_creds *src)
{
  যদি(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    যদি(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

শূন্য Curl_creds_unlink(সংরচনা Curl_creds **pcreds)
{
  যদি(*pcreds) {
    সংরচনা Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = রিক্ত;
    যদি(creds->refcount)
      creds->refcount--;
    যদি(!creds->refcount) {
      curlx_memzero(creds, পরিমাণ(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(সংরচনা Curl_creds *c1, সংরচনা Curl_creds *c2)
{
  প্রত্যাগম (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(সংরচনা Curl_creds *c1, সংরচনা Curl_creds *c2)
{
  প্রত্যাগম Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
শূন্য Curl_creds_trace(সংরচনা Curl_easy *data, সংরচনা Curl_creds *creds,
                      ধ্রুব বর্ণ *বার্তা)
{
  যদি(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               বার্তা,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  অন্যথা
    CURL_TRC_M(data, "%s: -", বার্তা);
}
#endif
