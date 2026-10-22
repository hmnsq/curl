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
  size_t ulen = user ? वाक्यदैर्घ्यम्(user) : 0;
  size_t plen = passwd ? वाक्यदैर्घ्यम्(passwd) : 0;
  size_t olen = oauth_bearer ? वाक्यदैर्घ्यम्(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? वाक्यदैर्घ्यम्(sasl_authzid) : 0;
  size_t sslen = sasl_service ? वाक्यदैर्घ्यम्(sasl_service) : 0;
  वर्ण *s, *buf;
  size_t bufsize;
  CURLcode परिणाम = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  यदि(!user && !passwd && !olen && !salen && !sslen)
    गच्छ बहिर्गमनम्;

  यदि((ulen > CURL_MAX_निवेशः_दीर्घता) ||
     (plen > CURL_MAX_निवेशः_दीर्घता) ||
     (olen > CURL_MAX_निवेशः_दीर्घता) ||
     (salen > CURL_MAX_निवेशः_दीर्घता) ||
     (sslen > CURL_MAX_निवेशः_दीर्घता)) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, परिमाण(*creds) + bufsize);
  यदि(!creds) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((वर्ण *)creds) + offsetof(संरचना Curl_creds, buf);
  creds->user = s = buf;
  यदि(ulen)
    स्मृतिं_प्रतिलिख(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  यदि(plen)
    स्मृतिं_प्रतिलिख(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  यदि(olen)
    स्मृतिं_प्रतिलिख(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  यदि(salen)
    स्मृतिं_प्रतिलिख(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  यदि(sslen)
    स्मृतिं_प्रतिलिख(s, sasl_service, sslen + 1);

बहिर्गमनम्:
  यदि(!परिणाम)
    *pcreds = creds;
  अन्यथा
    Curl_creds_unlink(&creds);
  प्रत्यागम परिणाम;
}

CURLcode Curl_creds_merge(ध्रुव वर्ण *user,
                          ध्रुव वर्ण *passwd,
                          संरचना Curl_creds *creds_अन्तर्,
                          uint8_t source,
                          संरचना Curl_creds **pcreds_बहिर्गमनम्)
{
  संरचना Curl_creds *creds_बहिर्गमनम् = रिक्त;
  CURLcode परिणाम;

  यदि(!creds_अन्तर्) {
    परिणाम = Curl_creds_create(user, passwd, रिक्त, रिक्त, रिक्त,
                               source, &creds_बहिर्गमनम्);
  }
  अन्यथा {
    परिणाम = Curl_creds_create(user ? user : Curl_creds_user(creds_अन्तर्),
                               passwd ? passwd : Curl_creds_passwd(creds_अन्तर्),
                               Curl_creds_oauth_bearer(creds_अन्तर्),
                               Curl_creds_sasl_authzid(creds_अन्तर्),
                               Curl_creds_sasl_service(creds_अन्तर्),
                               source, &creds_बहिर्गमनम्);
  }
  Curl_creds_link(pcreds_बहिर्गमनम्, creds_बहिर्गमनम्);
  Curl_creds_unlink(&creds_बहिर्गमनम्);
  प्रत्यागम परिणाम;
}

शून्य Curl_creds_link(संरचना Curl_creds **pdest, संरचना Curl_creds *src)
{
  यदि(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    यदि(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_creds_unlink(संरचना Curl_creds **pcreds)
{
  यदि(*pcreds) {
    संरचना Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = रिक्त;
    यदि(creds->refcount)
      creds->refcount--;
    यदि(!creds->refcount) {
      curlx_memzero(creds, परिमाण(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  प्रत्यागम (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  प्रत्यागम Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#यदि_परिभाषितम् CURLVERBOSE
शून्य Curl_creds_trace(संरचना Curl_easy *दत्त, संरचना Curl_creds *creds,
                      ध्रुव वर्ण *सन्देशः)
{
  यदि(creds) {
    CURL_TRC_M(दत्त, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               सन्देशः,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  अन्यथा
    CURL_TRC_M(दत्त, "%s: -", सन्देशः);
}
#यदि_अन्तः
