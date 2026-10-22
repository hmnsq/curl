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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ <stddef.h>  /* for offsetof() */

#ଅନ୍ତର୍ଭାଵ "creds.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"


CURLcode Curl_creds_create(ଅଚଳ ଅକ୍ଷର *user,
                           ଅଚଳ ଅକ୍ଷର *passwd,
                           ଅଚଳ ଅକ୍ଷର *oauth_bearer,
                           ଅଚଳ ଅକ୍ଷର *sasl_authzid,
                           ଅଚଳ ଅକ୍ଷର *sasl_service,
                           uint8_t source,
                           ସଂରଚନା Curl_creds **pcreds)
{
  ସଂରଚନା Curl_creds *creds = ଖାଲି;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  ଅକ୍ଷର *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  ଯଦି(!user && !passwd && !olen && !salen && !sslen)
    ଯାଅ out;

  ଯଦି((ulen > CURL_MAX_ନିବେଶ_LENGTH) ||
     (plen > CURL_MAX_ନିବେଶ_LENGTH) ||
     (olen > CURL_MAX_ନିବେଶ_LENGTH) ||
     (salen > CURL_MAX_ନିବେଶ_LENGTH) ||
     (sslen > CURL_MAX_ନିବେଶ_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, ଆକାର(*creds) + bufsize);
  ଯଦି(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((ଅକ୍ଷର *)creds) + offsetof(ସଂରଚନା Curl_creds, buf);
  creds->user = s = buf;
  ଯଦି(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  ଯଦି(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  ଯଦି(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  ଯଦି(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  ଯଦି(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  ଯଦି(!result)
    *pcreds = creds;
  ଅନ୍ୟଥା
    Curl_creds_unlink(&creds);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_creds_merge(ଅଚଳ ଅକ୍ଷର *user,
                          ଅଚଳ ଅକ୍ଷର *passwd,
                          ସଂରଚନା Curl_creds *creds_in,
                          uint8_t source,
                          ସଂରଚନା Curl_creds **pcreds_out)
{
  ସଂରଚନା Curl_creds *creds_out = ଖାଲି;
  CURLcode result;

  ଯଦି(!creds_in) {
    result = Curl_creds_create(user, passwd, ଖାଲି, ଖାଲି, ଖାଲି,
                               source, &creds_out);
  }
  ଅନ୍ୟଥା {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_creds_link(ସଂରଚନା Curl_creds **pdest, ସଂରଚନା Curl_creds *src)
{
  ଯଦି(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    ଯଦି(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ଶୂନ୍ୟ Curl_creds_unlink(ସଂରଚନା Curl_creds **pcreds)
{
  ଯଦି(*pcreds) {
    ସଂରଚନା Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ଖାଲି;
    ଯଦି(creds->refcount)
      creds->refcount--;
    ଯଦି(!creds->refcount) {
      curlx_memzero(creds, ଆକାର(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ସଂରଚନା Curl_creds *c1, ସଂରଚନା Curl_creds *c2)
{
  ଫେରନ୍ତୁ (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ସଂରଚନା Curl_creds *c1, ସଂରଚନା Curl_creds *c2)
{
  ଫେରନ୍ତୁ Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
ଶୂନ୍ୟ Curl_creds_trace(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creds *creds,
                      ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
  ଯଦି(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               ସନ୍ଦେଶ,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  ଅନ୍ୟଥା
    CURL_TRC_M(data, "%s: -", ସନ୍ଦେଶ);
}
#endif
