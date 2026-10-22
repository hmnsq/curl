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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "http_digest.h"
#అన్తర్భావ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
స్థిర శూన్య digest_flush_stale(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం digestdata *digest,
                               నిర్మాణం Curl_peer *peer,
                               నిర్మాణం Curl_creds *creds)
{
  bool flush = FALSE;
  ఒకవేళ(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(దత్తాంశం, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (గుర్తులేని పూర్ణాంకం)(peer ? peer->పోర్టు : 0));
    flush = TRUE;
  }
  లేకపోతే ఒకవేళ(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(దత్తాంశం, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  ఒకవేళ(flush) {
    /* flush Digest state */
    Curl_auth_digest_cleanup(digest);
  }

  Curl_peer_link(&digest->origin, peer);
  Curl_creds_link(&digest->creds, creds);
}

/* Test example headers:

   WWW-Authenticate: Digest realm="testrealm", nonce="1053604598"
   Proxy-Authenticate: Digest realm="testrealm", nonce="1053604598"
 */
CURLcode Curl_ఆదానం_digest(నిర్మాణం Curl_easy *దత్తాంశం,
                           bool proxy,
                           స్థిరరాశి అక్షరం *శీర్షిక) /* rest of the *-authenticate:
శీర్షిక*/
{
  /* Point to the correct struct with this */
  నిర్మాణం digestdata *digest;
  నిర్మాణం Curl_peer *origin = ఖాళీ;
  CURLcode ఫలితం;

  ఒకవేళ(proxy) {
    digest = &దత్తాంశం->state.proxydigest;
#నిర్వచితమైతే CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    తిరిగి CURLE_OK;  /* just ignore such a header without proxy support */
#లేకపోతే
    origin = దత్తాంశం->conn->http_proxy.peer;
#అయితే_ముగింపు
  }
  లేకపోతే {
    digest = &దత్తాంశం->state.digest;
    origin = దత్తాంశం->state.origin;
  }

  ఒకవేళ(!checkprefix("Digest", శీర్షిక) || !ISBLANK(శీర్షిక[6])) {
    Curl_auth_digest_cleanup(digest);
    తిరిగి CURLE_AUTH_ERROR;
  }

  శీర్షిక += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&శీర్షిక);

  /* This resets the digest struct before decoding */
  ఫలితం = Curl_auth_decode_digest_http_సందేశం(శీర్షిక, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  తిరిగి ఫలితం;
}

CURLcode Curl_output_digest(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool proxy,
                            స్థిరరాశి గుర్తులేని అక్షరం *request,
                            స్థిరరాశి గుర్తులేని అక్షరం *uripath)
{
  నిర్మాణం Curl_peer *origin = ఖాళీ;
  CURLcode ఫలితం;
  అక్షరం *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  అక్షరం **allocuserpwd;

  /* Point to the name and password for this */
  నిర్మాణం Curl_creds *creds = ఖాళీ;

  /* Point to the correct struct with this */
  నిర్మాణం digestdata *digest;
  నిర్మాణం auth *authp;

  ఒకవేళ(proxy) {
#నిర్వచితమైతే CURL_DISABLE_PROXY
    తిరిగి CURLE_NOT_BUILT_లోపల;
#లేకపోతే
    digest = &దత్తాంశం->state.proxydigest;
    origin = దత్తాంశం->conn->http_proxy.peer;
    creds = దత్తాంశం->conn->http_proxy.creds;
    allocuserpwd = &దత్తాంశం->req.hd_proxy_auth;
    authp = &దత్తాంశం->state.authproxy;
#అయితే_ముగింపు
  }
  లేకపోతే {
    DEBUGASSERT(దత్తాంశం->state.origin);
    digest = &దత్తాంశం->state.digest;
    origin = దత్తాంశం->state.origin;
    creds = దత్తాంశం->state.creds;
    allocuserpwd = &దత్తాంశం->req.hd_auth;
    authp = &దత్తాంశం->state.authhost;
  }

  digest_flush_stale(దత్తాంశం, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#నిర్వచితమైతే USE_WINDOWS_SSPI
  have_chlg = !!digest->ఆదానం_token;
#లేకపోతే
  have_chlg = !!digest->nonce;
#అయితే_ముగింపు

  ఒకవేళ(!have_chlg) {
    authp->పూర్తి = FALSE;
    తిరిగి CURLE_OK;
  }

  ఫలితం = Curl_auth_create_digest_http_సందేశం(దత్తాంశం, creds, request,
                                                uripath, digest,
                                                &response, &len);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  ఒకవేళ(!*allocuserpwd)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  authp->పూర్తి = TRUE;

  తిరిగి CURLE_OK;
}

శూన్య Curl_http_auth_cleanup_digest(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_auth_digest_cleanup(&దత్తాంశం->state.digest);
  Curl_auth_digest_cleanup(&దత్తాంశం->state.proxydigest);
}

#అయితే_ముగింపు
