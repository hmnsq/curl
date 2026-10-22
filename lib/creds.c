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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव <stddef.h>  /* for offsetof() */

#अन्तर्भाव "creds.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "urldata.h"


CURLcode Curl_creds_create(ध्रुव वर्ण *user,
                           ध्रुव वर्ण *passwd,
                           ध्रुव वर्ण *oauth_bearer,
                           ध्रुव वर्ण *sasl_authzid,
                           ध्रुव वर्ण *sasl_service,
                           uint8_t source,
                           संरचना Curl_creds **pcreds)
{
  संरचना Curl_creds *creds = रिक्त;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  वर्ण *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  सचे(!user && !passwd && !olen && !salen && !sslen)
    गच्छ out;

  सचे((ulen > CURL_MAX_निवेशः_LENGTH) ||
     (plen > CURL_MAX_निवेशः_LENGTH) ||
     (olen > CURL_MAX_निवेशः_LENGTH) ||
     (salen > CURL_MAX_निवेशः_LENGTH) ||
     (sslen > CURL_MAX_निवेशः_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, परिमाण(*creds) + bufsize);
  सचे(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((वर्ण *)creds) + offsetof(संरचना Curl_creds, buf);
  creds->user = s = buf;
  सचे(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  सचे(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  सचे(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  सचे(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  सचे(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  सचे(!result)
    *pcreds = creds;
  अञ्ञथा
    Curl_creds_unlink(&creds);
  पटिनिवत्त result;
}

CURLcode Curl_creds_merge(ध्रुव वर्ण *user,
                          ध्रुव वर्ण *passwd,
                          संरचना Curl_creds *creds_in,
                          uint8_t source,
                          संरचना Curl_creds **pcreds_out)
{
  संरचना Curl_creds *creds_out = रिक्त;
  CURLcode result;

  सचे(!creds_in) {
    result = Curl_creds_create(user, passwd, रिक्त, रिक्त, रिक्त,
                               source, &creds_out);
  }
  अञ्ञथा {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  पटिनिवत्त result;
}

शून्य Curl_creds_link(संरचना Curl_creds **pdest, संरचना Curl_creds *src)
{
  सचे(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    सचे(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_creds_unlink(संरचना Curl_creds **pcreds)
{
  सचे(*pcreds) {
    संरचना Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = रिक्त;
    सचे(creds->refcount)
      creds->refcount--;
    सचे(!creds->refcount) {
      curlx_memzero(creds, परिमाण(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  पटिनिवत्त (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  पटिनिवत्त Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
शून्य Curl_creds_trace(संरचना Curl_easy *data, संरचना Curl_creds *creds,
                      ध्रुव वर्ण *सन्देशः)
{
  सचे(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               सन्देशः,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  अञ्ञथा
    CURL_TRC_M(data, "%s: -", सन्देशः);
}
#endif
