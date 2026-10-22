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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ <stddef.h>  /* for offsetof() */

#அந்தர்பாவ "creds.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "urldata.h"


CURLcode Curl_creds_create(மாறிலி எழுத்து *user,
                           மாறிலி எழுத்து *passwd,
                           மாறிலி எழுத்து *oauth_bearer,
                           மாறிலி எழுத்து *sasl_authzid,
                           மாறிலி எழுத்து *sasl_service,
                           uint8_t source,
                           கட்டமைப்பு Curl_creds **pcreds)
{
  கட்டமைப்பு Curl_creds *creds = வெற்று;
  size_t ulen = user ? strlen(user) : 0;
  size_t plen = passwd ? strlen(passwd) : 0;
  size_t olen = oauth_bearer ? strlen(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? strlen(sasl_authzid) : 0;
  size_t sslen = sasl_service ? strlen(sasl_service) : 0;
  எழுத்து *s, *buf;
  size_t bufsize;
  CURLcode result = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  எனில்(!user && !passwd && !olen && !salen && !sslen)
    செல் out;

  எனில்((ulen > CURL_MAX_உள்ளீடு_LENGTH) ||
     (plen > CURL_MAX_உள்ளீடு_LENGTH) ||
     (olen > CURL_MAX_உள்ளீடு_LENGTH) ||
     (salen > CURL_MAX_உள்ளீடு_LENGTH) ||
     (sslen > CURL_MAX_உள்ளீடு_LENGTH)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, அளவு(*creds) + bufsize);
  எனில்(!creds) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->source = source;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((எழுத்து *)creds) + offsetof(கட்டமைப்பு Curl_creds, buf);
  creds->user = s = buf;
  எனில்(ulen)
    memcpy(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  எனில்(plen)
    memcpy(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  எனில்(olen)
    memcpy(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  எனில்(salen)
    memcpy(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  எனில்(sslen)
    memcpy(s, sasl_service, sslen + 1);

out:
  எனில்(!result)
    *pcreds = creds;
  இல்லையெனில்
    Curl_creds_unlink(&creds);
  திரும்பு result;
}

CURLcode Curl_creds_merge(மாறிலி எழுத்து *user,
                          மாறிலி எழுத்து *passwd,
                          கட்டமைப்பு Curl_creds *creds_in,
                          uint8_t source,
                          கட்டமைப்பு Curl_creds **pcreds_out)
{
  கட்டமைப்பு Curl_creds *creds_out = வெற்று;
  CURLcode result;

  எனில்(!creds_in) {
    result = Curl_creds_create(user, passwd, வெற்று, வெற்று, வெற்று,
                               source, &creds_out);
  }
  இல்லையெனில் {
    result = Curl_creds_create(user ? user : Curl_creds_user(creds_in),
                               passwd ? passwd : Curl_creds_passwd(creds_in),
                               Curl_creds_oauth_bearer(creds_in),
                               Curl_creds_sasl_authzid(creds_in),
                               Curl_creds_sasl_service(creds_in),
                               source, &creds_out);
  }
  Curl_creds_link(pcreds_out, creds_out);
  Curl_creds_unlink(&creds_out);
  திரும்பு result;
}

வெறுமை Curl_creds_link(கட்டமைப்பு Curl_creds **pdest, கட்டமைப்பு Curl_creds *src)
{
  எனில்(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    எனில்(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

வெறுமை Curl_creds_unlink(கட்டமைப்பு Curl_creds **pcreds)
{
  எனில்(*pcreds) {
    கட்டமைப்பு Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = வெற்று;
    எனில்(creds->refcount)
      creds->refcount--;
    எனில்(!creds->refcount) {
      curlx_memzero(creds, அளவு(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(கட்டமைப்பு Curl_creds *c1, கட்டமைப்பு Curl_creds *c2)
{
  திரும்பு (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(கட்டமைப்பு Curl_creds *c1, கட்டமைப்பு Curl_creds *c2)
{
  திரும்பு Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->source == c2->source)));
}

#ifdef CURLVERBOSE
வெறுமை Curl_creds_trace(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creds *creds,
                      மாறிலி எழுத்து *செய்தி)
{
  எனில்(creds) {
    CURL_TRC_M(data, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               செய்தி,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->source);
  }
  இல்லையெனில்
    CURL_TRC_M(data, "%s: -", செய்தி);
}
#endif
