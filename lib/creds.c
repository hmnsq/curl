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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ <stddef.h>  /* for offsetof() */

#అన్తర్భావ "creds.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "urldata.h"


CURLcode Curl_creds_create(స్థిరరాశి అక్షరం *user,
                           స్థిరరాశి అక్షరం *passwd,
                           స్థిరరాశి అక్షరం *oauth_bearer,
                           స్థిరరాశి అక్షరం *sasl_authzid,
                           స్థిరరాశి అక్షరం *sasl_service,
                           uint8_t ఆధారం,
                           నిర్మాణం Curl_creds **pcreds)
{
  నిర్మాణం Curl_creds *creds = ఖాళీ;
  size_t ulen = user ? అక్షరశ్రేణి_పొడవు(user) : 0;
  size_t plen = passwd ? అక్షరశ్రేణి_పొడవు(passwd) : 0;
  size_t olen = oauth_bearer ? అక్షరశ్రేణి_పొడవు(oauth_bearer) : 0;
  size_t salen = sasl_authzid ? అక్షరశ్రేణి_పొడవు(sasl_authzid) : 0;
  size_t sslen = sasl_service ? అక్షరశ్రేణి_పొడవు(sasl_service) : 0;
  అక్షరం *s, *buf;
  size_t bufsize;
  CURLcode ఫలితం = CURLE_OK;

  Curl_creds_unlink(pcreds);

  /* Everything empty/NULL, this is the NULL credential */
  ఒకవేళ(!user && !passwd && !olen && !salen && !sslen)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ((ulen > CURL_MAX_ఆదానం_నిడివి) ||
     (plen > CURL_MAX_ఆదానం_నిడివి) ||
     (olen > CURL_MAX_ఆదానం_నిడివి) ||
     (salen > CURL_MAX_ఆదానం_నిడివి) ||
     (sslen > CURL_MAX_ఆదానం_నిడివి)) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }

  /* null-terminator for user already part of struct */
  bufsize = ulen + plen + 1 + olen + 1 + salen + 1 + sslen + 1;
  creds = curlx_calloc(1, పరిమాణం(*creds) + bufsize);
  ఒకవేళ(!creds) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  creds->bufsize = bufsize;
  creds->refcount = 1;
  creds->ఆధారం = ఆధారం;
  /* Some compilers try to be too smart about our dynamic struct size */
  buf = ((అక్షరం *)creds) + offsetof(నిర్మాణం Curl_creds, buf);
  creds->user = s = buf;
  ఒకవేళ(ulen)
    జ్ఞాపకం_నకలు(s, user, ulen + 1);
  creds->passwd = s = buf + ulen + 1;
  ఒకవేళ(plen)
    జ్ఞాపకం_నకలు(s, passwd, plen + 1);
  creds->oauth_bearer = s = buf + ulen + 1 + plen + 1;
  ఒకవేళ(olen)
    జ్ఞాపకం_నకలు(s, oauth_bearer, olen + 1);
  creds->sasl_authzid = s = buf + ulen + 1 + plen + 1 + olen + 1;
  ఒకవేళ(salen)
    జ్ఞాపకం_నకలు(s, sasl_authzid, salen + 1);
  creds->sasl_service = s = buf + ulen + 1 + plen + 1 + olen + 1 + salen + 1;
  ఒకవేళ(sslen)
    జ్ఞాపకం_నకలు(s, sasl_service, sslen + 1);

బహిర్గతం:
  ఒకవేళ(!ఫలితం)
    *pcreds = creds;
  లేకపోతే
    Curl_creds_unlink(&creds);
  తిరిగి ఫలితం;
}

CURLcode Curl_creds_విలీనం(స్థిరరాశి అక్షరం *user,
                          స్థిరరాశి అక్షరం *passwd,
                          నిర్మాణం Curl_creds *creds_లోపల,
                          uint8_t ఆధారం,
                          నిర్మాణం Curl_creds **pcreds_బహిర్గతం)
{
  నిర్మాణం Curl_creds *creds_బహిర్గతం = ఖాళీ;
  CURLcode ఫలితం;

  ఒకవేళ(!creds_లోపల) {
    ఫలితం = Curl_creds_create(user, passwd, ఖాళీ, ఖాళీ, ఖాళీ,
                               ఆధారం, &creds_బహిర్గతం);
  }
  లేకపోతే {
    ఫలితం = Curl_creds_create(user ? user : Curl_creds_user(creds_లోపల),
                               passwd ? passwd : Curl_creds_passwd(creds_లోపల),
                               Curl_creds_oauth_bearer(creds_లోపల),
                               Curl_creds_sasl_authzid(creds_లోపల),
                               Curl_creds_sasl_service(creds_లోపల),
                               ఆధారం, &creds_బహిర్గతం);
  }
  Curl_creds_link(pcreds_బహిర్గతం, creds_బహిర్గతం);
  Curl_creds_unlink(&creds_బహిర్గతం);
  తిరిగి ఫలితం;
}

శూన్య Curl_creds_link(నిర్మాణం Curl_creds **pdest, నిర్మాణం Curl_creds *src)
{
  ఒకవేళ(*pdest != src) {
    Curl_creds_unlink(pdest);
    *pdest = src;
    ఒకవేళ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

శూన్య Curl_creds_unlink(నిర్మాణం Curl_creds **pcreds)
{
  ఒకవేళ(*pcreds) {
    నిర్మాణం Curl_creds *creds = *pcreds;

    DEBUGASSERT(creds->refcount);
    *pcreds = ఖాళీ;
    ఒకవేళ(creds->refcount)
      creds->refcount--;
    ఒకవేళ(!creds->refcount) {
      curlx_memzero(creds, పరిమాణం(*creds) + creds->bufsize);
      curlx_free(creds);
    }
  }
}

bool Curl_creds_same(నిర్మాణం Curl_creds *c1, నిర్మాణం Curl_creds *c2)
{
  తిరిగి (c1 == c2) ||
         (c1 && c2 &&
          !Curl_timestrcmp(c1->user, c2->user) &&
          !Curl_timestrcmp(c1->passwd, c2->passwd) &&
          !Curl_timestrcmp(c1->oauth_bearer, c2->oauth_bearer) &&
          !Curl_timestrcmp(c1->sasl_authzid, c2->sasl_authzid) &&
          !Curl_timestrcmp(c1->sasl_service, c2->sasl_service));
}

bool Curl_creds_equal(నిర్మాణం Curl_creds *c1, నిర్మాణం Curl_creds *c2)
{
  తిరిగి Curl_creds_same(c1, c2) &&
         ((c1 == c2) || (c1 && c2 && (c1->ఆధారం == c2->ఆధారం)));
}

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_creds_trace(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creds *creds,
                      స్థిరరాశి అక్షరం *సందేశం)
{
  ఒకవేళ(creds) {
    CURL_TRC_M(దత్తాంశం, "%s: user=%s, passwd=%s, "
               "sasl_authzid=%s, oauth_bearer=%s, source=%d",
               సందేశం,
               Curl_creds_user(creds),
               Curl_creds_has_passwd(creds) ? "***" : "",
               Curl_creds_sasl_authzid(creds),
               Curl_creds_has_oauth_bearer(creds) ? "***" : "",
               creds->ఆధారం);
  }
  లేకపోతే
    CURL_TRC_M(దత్తాంశం, "%s: -", సందేశం);
}
#అయితే_ముగింపు
