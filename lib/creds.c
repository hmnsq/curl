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


CURLcode Curl_creds_create(ধ্রুবক অক্ষর *user,
                           ধ্রুবক অক্ষর *passwd,
                           ধ্রুবক অক্ষর *oauth_bearer,
                           ধ্রুবক অক্ষর *sasl_authzid,
                           ধ্রুবক অক্ষর *sasl_service,
                           uint8_t source,
                           কাঠামো Curl_creds **pcreds)
{
  কাঠামো Curl_creds *creds = ফাঁকা;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  অক্ষর *s, *buf;
  size_t bufsize;
  CURLcode ফলাফল = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  যদি(!user && !passwd && !olen && !salen && !sslen)
    যাও out;

  যদি((ulen > CURL_MAX_প্রবেশ_দৈর্ঘ্য) ||
     (plen > CURL_MAX_প্রবেশ_দৈর্ঘ্য) ||
     (olen > CURL_MAX_প্রবেশ_দৈর্ঘ্য) ||
     (salen > CURL_MAX_প্রবেশ_দৈর্ঘ্য) ||
     (sslen > CURL_MAX_প্রবেশ_দৈর্ঘ্য)) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, আকার(*creds) + bufsize);
  যদি(!creds) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((অক্ষর *)creds) + offsetof(কাঠামো Curl_creds, buf);
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
  যদি(!ফলাফল)
    *pcreds = creds;
  অন্যথা
    Curl_creds_unlink(&creds);
  ফেরত ফলাফল;
}

CURLcode Curl_creds_merge(ধ্রুবক অক্ষর *user,
                          ধ্রুবক অক্ষর *passwd,
                          কাঠামো Curl_creds *creds_in,
                          uint8_t source,
                          কাঠামো Curl_creds **pcreds_out)
{
  কাঠামো Curl_creds *creds_out = ফাঁকা;
  CURLcode ফলাফল;

  যদি(!creds_in) {
    ফলাফল = Curl_creds_create(user, passwd, ফাঁকা, ফাঁকা, ফাঁকা,
                               source, &creds_out);
  }
  অন্যথা {
    ফলাফল = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  ফেরত ফলাফল;
}

শূন্য Curl_creds_link(কাঠামো Curl_creds **pdest, কাঠামো Curl_creds *src)
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

শূন্য Curl_creds_unlink(কাঠামো Curl_creds **pcreds)
{
  যদি(*pcreds) {
    কাঠামো Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ফাঁকা;
    যদি(creds->refcount)
      creds->refcount--;
    যদি(!creds->refcount) {
      curlx_memzero(creds, আকার(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(কাঠামো Curl_creds *c1, কাঠামো Curl_creds *c2)
{
  ফেরত (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(কাঠামো Curl_creds *c1, কাঠামো Curl_creds *c2)
{
  ফেরত Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
শূন্য Curl_creds_trace(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creds *creds,
                      ধ্রুবক অক্ষর *বার্তা)
{
  যদি(creds) {
    CURL_TRC_M(তথ্য, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               বার্তা,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  অন্যথা
    CURL_TRC_M(তথ্য, "%s: -", বার্তা);
}
#endif
