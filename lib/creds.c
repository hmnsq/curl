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
  माप_प्रकार ulen = user ? म_माप(user) : 0;
  माप_प्रकार plen = passwd ? म_माप(passwd) : 0;
  माप_प्रकार olen = oauth_bearer ? म_माप(oauth_bearer) : 0;
  माप_प्रकार salen = sasl_authzid ? म_माप(sasl_authzid) : 0;
  माप_प्रकार sslen = sasl_service ? म_माप(sasl_service) : 0;
  वर्ण *s, *buf;
  माप_प्रकार bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  यदि(!user && !passwd && !olen && !salen && !sslen)
    गच्छ out;

  यदि((ulen > CURL_MAX_निवेश_LENGTH) ||
     (plen > CURL_MAX_निवेश_LENGTH) ||
     (olen > CURL_MAX_निवेश_LENGTH) ||
     (salen > CURL_MAX_निवेश_LENGTH) ||
     (sslen > CURL_MAX_निवेश_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, परिमाण(*creds) + bufsize);
  यदि(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((वर्ण *)creds) + दुरत्व(संरचना Curl_creds, buf);
  creds->user = s = buf;
  यदि(ulen)
    स_नकल(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  यदि(plen)
    स_नकल(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  यदि(olen)
    स_नकल(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  यदि(salen)
    स_नकल(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  यदि(sslen)
    स_नकल(s, sasl_service, sslen + 1);

out:
  यदि(!result)
    *pcreds = creds;
  अन्यथा
    Curl_creds_unlink(&creds);
  प्रत्यागम result;
}

CURLcode Curl_creds_merge(ध्रुव वर्ण *user,
                          ध्रुव वर्ण *passwd,
                          संरचना Curl_creds *creds_in,
                          uint8_t source,
                          संरचना Curl_creds **pcreds_out)
{
  संरचना Curl_creds *creds_out = रिक्त;
  CURLcode result;

  यदि(!creds_in) {
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
  प्रत्यागम result;
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

#ifdef CURLVERBOSE
शून्य Curl_creds_trace(संरचना Curl_easy *data, संरचना Curl_creds *creds,
                      ध्रुव वर्ण *संदेश)
{
  यदि(creds) {
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
