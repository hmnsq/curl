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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "vauth/vauth.h"
#ضمن "http_digest.h"
#ضمن "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
ساكن فراغ digest_flush_stale(بنية Curl_easy *انکڑے,
                               بنية digestdata *digest,
                               بنية Curl_peer *peer,
                               بنية Curl_creds *creds)
{
  bool flush = FALSE;
  إذا(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(انکڑے, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (بنانشانی صحيح)(peer ? peer->پورٹ : 0));
    flush = TRUE;
  }
  وإلا إذا(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(انکڑے, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  إذا(flush) {
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
CURLcode Curl_داخلہ_digest(بنية Curl_easy *انکڑے,
                           bool proxy,
                           ثابت حرف *سرخی) /* rest of the *-authenticate:
سرخی*/
{
  /* Point to the correct struct with this */
  بنية digestdata *digest;
  بنية Curl_peer *origin = عدم;
  CURLcode نتیجہ;

  إذا(proxy) {
    digest = &انکڑے->state.proxydigest;
#جے_تعریف_شدہ CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_OK;  /* just ignore such a header without proxy support */
#وإلا
    origin = انکڑے->conn->http_proxy.peer;
#جے_انت
  }
  وإلا {
    digest = &انکڑے->state.digest;
    origin = انکڑے->state.origin;
  }

  إذا(!checkprefix("Digest", سرخی) || !ISBLANK(سرخی[6])) {
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_AUTH_ERROR;
  }

  سرخی += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&سرخی);

  /* This resets the digest struct before decoding */
  نتیجہ = Curl_auth_decode_digest_http_سنیہا(سرخی, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  أرجع نتیجہ;
}

CURLcode Curl_output_digest(بنية Curl_easy *انکڑے,
                            bool proxy,
                            ثابت بنانشانی حرف *request,
                            ثابت بنانشانی حرف *uripath)
{
  بنية Curl_peer *origin = عدم;
  CURLcode نتیجہ;
  حرف *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  حرف **allocuserpwd;

  /* Point to the name and password for this */
  بنية Curl_creds *creds = عدم;

  /* Point to the correct struct with this */
  بنية digestdata *digest;
  بنية auth *authp;

  إذا(proxy) {
#جے_تعریف_شدہ CURL_DISABLE_PROXY
    أرجع CURLE_NOT_BUILT_وچ;
#وإلا
    digest = &انکڑے->state.proxydigest;
    origin = انکڑے->conn->http_proxy.peer;
    creds = انکڑے->conn->http_proxy.creds;
    allocuserpwd = &انکڑے->req.hd_proxy_auth;
    authp = &انکڑے->state.authproxy;
#جے_انت
  }
  وإلا {
    DEBUGASSERT(انکڑے->state.origin);
    digest = &انکڑے->state.digest;
    origin = انکڑے->state.origin;
    creds = انکڑے->state.creds;
    allocuserpwd = &انکڑے->req.hd_auth;
    authp = &انکڑے->state.authhost;
  }

  digest_flush_stale(انکڑے, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#جے_تعریف_شدہ USE_WINDOWS_SSPI
  have_chlg = !!digest->داخلہ_token;
#وإلا
  have_chlg = !!digest->nonce;
#جے_انت

  إذا(!have_chlg) {
    authp->پورا = FALSE;
    أرجع CURLE_OK;
  }

  نتیجہ = Curl_auth_create_digest_http_سنیہا(انکڑے, creds, request,
                                                uripath, digest,
                                                &response, &len);
  إذا(نتیجہ)
    أرجع نتیجہ;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  إذا(!*allocuserpwd)
    أرجع CURLE_باہرلا_دا_حافظہ;

  authp->پورا = TRUE;

  أرجع CURLE_OK;
}

فراغ Curl_http_auth_cleanup_digest(بنية Curl_easy *انکڑے)
{
  Curl_auth_digest_cleanup(&انکڑے->state.digest);
  Curl_auth_digest_cleanup(&انکڑے->state.proxydigest);
}

#جے_انت
