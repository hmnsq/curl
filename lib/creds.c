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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ <stddef.h>  /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "creds.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"


CURLcode Curl_creds_create(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oauth_bearer,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_authzid,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sasl_service,
                           uint8_t ಮೂಲ,
                           ರಚನೆ Curl_creds **pcreds)
{
  ರಚನೆ Curl_creds *creds = ಖಾಲಿ;
  size_t ulen = user ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user) : 0;
  size_t plen = passwd ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd) : 0;
  size_t olen = oauth_bearer ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sasl_authzid) : 0;
  size_t sslen = sasl_service ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sasl_service) : 0;
  ಅಕ್ಷರ *s, *buf;
  size_t bufsize;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  ಆದರೆ(!user && !passwd && !olen && !salen && !sslen)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ((ulen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) ||
     (plen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) ||
     (olen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) ||
     (salen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) ||
     (sslen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, ಗಾತ್ರ(*creds) + bufsize);
  ಆದರೆ(!creds) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->ಮೂಲ = ಮೂಲ;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((ಅಕ್ಷರ *)creds) + offsetof(ರಚನೆ Curl_creds, buf);
  creds->user = s = buf;
  ಆದರೆ(ulen)
    ಸ್ಮರಣೆ_ನಕಲು(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  ಆದರೆ(plen)
    ಸ್ಮರಣೆ_ನಕಲು(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  ಆದರೆ(olen)
    ಸ್ಮರಣೆ_ನಕಲು(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  ಆದರೆ(salen)
    ಸ್ಮರಣೆ_ನಕಲು(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  ಆದರೆ(sslen)
    ಸ್ಮರಣೆ_ನಕಲು(s, sasl_service, sslen + 1);

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *pcreds = creds;
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_creds_unlink(&creds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_creds_ವಿಲೀನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd,
                          ರಚನೆ Curl_creds *creds_ಒಳ,
                          uint8_t ಮೂಲ,
                          ರಚನೆ Curl_creds **pcreds_ಹೊರಹೋಗು)
{
  ರಚನೆ Curl_creds *creds_ಹೊರಹೋಗು = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!creds_ಒಳ) {
    ಫಲಿತಾಂಶ = Curl_creds_create(user, passwd, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                               ಮೂಲ, &creds_ಹೊರಹೋಗು);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_creds_create(user ? user : Curl_creds_user(creds_ಒಳ),
                               passwd ? passwd : Curl_creds_passwd(creds_ಒಳ),
                               Curl_creds_oauth_bearer(creds_ಒಳ),
                               Curl_creds_sasl_authzid(creds_ಒಳ),
                               Curl_creds_sasl_service(creds_ಒಳ),
                               ಮೂಲ, &creds_ಹೊರಹೋಗು);
  }
  Curl_creds_link(pcreds_ಹೊರಹೋಗು, creds_ಹೊರಹೋಗು);
  Curl_creds_unlink(&creds_ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_creds_link(ರಚನೆ Curl_creds **pdest, ರಚನೆ Curl_creds *src)
{
  ಆದರೆ(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    ಆದರೆ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ಶೂನ್ಯ Curl_creds_unlink(ರಚನೆ Curl_creds **pcreds)
{
  ಆದರೆ(*pcreds) {
    ರಚನೆ Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ಖಾಲಿ;
    ಆದರೆ(creds->refcount)
      creds->refcount--;
    ಆದರೆ(!creds->refcount) {
      curlx_memzero(creds, ಗಾತ್ರ(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(ರಚನೆ Curl_creds *c1, ರಚನೆ Curl_creds *c2)
{
  ಹಿಂತಿರುಗಿ (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(ರಚನೆ Curl_creds *c1, ರಚನೆ Curl_creds *c2)
{
  ಹಿಂತಿರುಗಿ Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->ಮೂಲ == c2->ಮೂಲ)));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_creds_trace(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creds *creds,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
  ಆದರೆ(creds) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               ಸಂದೇಶ,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->ಮೂಲ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_M(ದತ್ತಾಂಶ, "%s: -", ಸಂದೇಶ);
}
#ಆದರೆ_ಅಂತ್ಯ
