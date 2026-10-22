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


CURLcode Curl_creds_create(स्थिरांक अक्षर *user,
                           स्थिरांक अक्षर *passwd,
                           स्थिरांक अक्षर *oauth_bearer,
                           स्थिरांक अक्षर *sasl_authzid,
                           स्थिरांक अक्षर *sasl_service,
                           uint8_t source,
                           संरचना Curl_creds **pcreds)
{
  संरचना Curl_creds *creds = रिक्त;
  माप_प्रकार ulen = user ? म_माप(user) : 0;
  माप_प्रकार plen = passwd ? म_माप(passwd) : 0;
  माप_प्रकार olen = oauth_bearer ? म_माप(oauth_bearer) : 0;
  माप_प्रकार salen = sasl_authzid ? म_माप(sasl_authzid) : 0;
  माप_प्रकार sslen = sasl_service ? म_माप(sasl_service) : 0;
  अक्षर *s, *buf;
  माप_प्रकार bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  जर(!user && !passwd && !olen && !salen && !sslen)
    जा out;

  जर((ulen > CURL_MAX_आदान_LENGTH) ||
     (plen > CURL_MAX_आदान_LENGTH) ||
     (olen > CURL_MAX_आदान_LENGTH) ||
     (salen > CURL_MAX_आदान_LENGTH) ||
     (sslen > CURL_MAX_आदान_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, आकार(*creds) + bufsize);
  जर(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((अक्षर *)creds) + दुरत्व(संरचना Curl_creds, buf);
  creds->user = s = buf;
  जर(ulen)
    स_नकल(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  जर(plen)
    स_नकल(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  जर(olen)
    स_नकल(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  जर(salen)
    स_नकल(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  जर(sslen)
    स_नकल(s, sasl_service, sslen + 1);

out:
  जर(!result)
    *pcreds = creds;
  अन्यथा
    Curl_creds_unlink(&creds);
  परत result;
}

CURLcode Curl_creds_merge(स्थिरांक अक्षर *user,
                          स्थिरांक अक्षर *passwd,
                          संरचना Curl_creds *creds_in,
                          uint8_t source,
                          संरचना Curl_creds **pcreds_out)
{
  संरचना Curl_creds *creds_out = रिक्त;
  CURLcode result;

  जर(!creds_in) {
    result = Curl_creds_create(user, passwd, रिक्त, रिक्त, रिक्त,
                               source, &creds_out);
  }
  अन्यथा {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  परत result;
}

शून्य Curl_creds_link(संरचना Curl_creds **pdest, संरचना Curl_creds *src)
{
  जर(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    जर(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_creds_unlink(संरचना Curl_creds **pcreds)
{
  जर(*pcreds) {
    संरचना Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = रिक्त;
    जर(creds->refcount)
      creds->refcount--;
    जर(!creds->refcount) {
      curlx_memzero(creds, आकार(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  परत (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(संरचना Curl_creds *c1, संरचना Curl_creds *c2)
{
  परत Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
शून्य Curl_creds_trace(संरचना Curl_easy *data, संरचना Curl_creds *creds,
                      स्थिरांक अक्षर *संदेश)
{
  जर(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               संदेश,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  अन्यथा
    CURL_TRC_M(data, "%s: -", संदेश);
}
#endif
