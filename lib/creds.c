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
#समावेश "curl_setup.h"

#समावेश <stddef.h>  /* for offsetof() */

#समावेश "creds.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "urldata.h"


CURLcode Curl_creds_create(स्थिर अक्षर *user,
                           स्थिर अक्षर *passwd,
                           स्थिर अक्षर *oauth_bearer,
                           स्थिर अक्षर *sasl_authzid,
                           स्थिर अक्षर *sasl_service,
                           uint8_t source,
                           काष्ठा Curl_creds **pcreds)
{
  काष्ठा Curl_creds *creds = शून्य;
  माप_प्रकार ulen = user ? म_माप(user) : 0;
  माप_प्रकार plen = passwd ? म_माप(passwd) : 0;
  माप_प्रकार olen = oauth_bearer ? म_माप(oauth_bearer) : 0;
  माप_प्रकार salen = sasl_authzid ? म_माप(sasl_authzid) : 0;
  माप_प्रकार sslen = sasl_service ? म_माप(sasl_service) : 0;
  अक्षर *s, *buf;
  माप_प्रकार bufsize;
  CURLcode परिणाम = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  अगर(!user && !passwd && !olen && !salen && !sslen)
    जाओ out;

  अगर((ulen > CURL_MAX_निवेश_लंबाई) ||
     (plen > CURL_MAX_निवेश_लंबाई) ||
     (olen > CURL_MAX_निवेश_लंबाई) ||
     (salen > CURL_MAX_निवेश_लंबाई) ||
     (sslen > CURL_MAX_निवेश_लंबाई)) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, माप(*creds) + bufsize);
  अगर(!creds) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((अक्षर *)creds) + दुरत्व(काष्ठा Curl_creds, buf);
  creds->user = s = buf;
  अगर(ulen)
    स_नकल(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  अगर(plen)
    स_नकल(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  अगर(olen)
    स_नकल(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  अगर(salen)
    स_नकल(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  अगर(sslen)
    स_नकल(s, sasl_service, sslen + 1);

out:
  अगर(!परिणाम)
    *pcreds = creds;
  अन्यथा
    Curl_creds_unlink(&creds);
  वापस परिणाम;
}

CURLcode Curl_creds_merge(स्थिर अक्षर *user,
                          स्थिर अक्षर *passwd,
                          काष्ठा Curl_creds *creds_in,
                          uint8_t source,
                          काष्ठा Curl_creds **pcreds_out)
{
  काष्ठा Curl_creds *creds_out = शून्य;
  CURLcode परिणाम;

  अगर(!creds_in) {
    परिणाम = Curl_creds_create(user, passwd, शून्य, शून्य, शून्य,
                               source, &creds_out);
  }
  अन्यथा {
    परिणाम = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  वापस परिणाम;
}

व्योम Curl_creds_link(काष्ठा Curl_creds **pdest, काष्ठा Curl_creds *src)
{
  अगर(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    अगर(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

व्योम Curl_creds_unlink(काष्ठा Curl_creds **pcreds)
{
  अगर(*pcreds) {
    काष्ठा Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = शून्य;
    अगर(creds->refcount)
      creds->refcount--;
    अगर(!creds->refcount) {
      curlx_memzero(creds, माप(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(काष्ठा Curl_creds *c1, काष्ठा Curl_creds *c2)
{
  वापस (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(काष्ठा Curl_creds *c1, काष्ठा Curl_creds *c2)
{
  वापस Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#अगर_घोषित CURLVERBOSE
व्योम Curl_creds_trace(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creds *creds,
                      स्थिर अक्षर *संदेश)
{
  अगर(creds) {
    CURL_TRC_M(तथ्य, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               संदेश,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  अन्यथा
    CURL_TRC_M(तथ्य, "%s: -", संदेश);
}
#पूर्ण_अगर
