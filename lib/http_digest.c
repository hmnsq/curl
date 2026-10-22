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
ساكن فراغ digest_flush_stale(بنية Curl_easy *معلومات,
                               بنية digestdata *digest,
                               بنية Curl_peer *peer,
                               بنية Curl_creds *creds)
{
  bool flush = FALSE;
  إذا(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(معلومات, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (بینښه صحيح)(peer ? peer->پورټ : 0));
    flush = TRUE;
  }
  وإلا إذا(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(معلومات, "http_digest, reset on creds change to %s",
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
CURLcode Curl_ننوت_digest(بنية Curl_easy *معلومات,
                           bool proxy,
                           ثابت حرف *سرلیک) /* rest of the *-authenticate:
سرلیک*/
{
  /* Point to the correct struct with this */
  بنية digestdata *digest;
  بنية Curl_peer *origin = عدم;
  CURLcode پایله;

  إذا(proxy) {
    digest = &معلومات->state.proxydigest;
#که_تعریف_شوی CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_OK;  /* just ignore such a header without proxy support */
#وإلا
    origin = معلومات->conn->http_proxy.peer;
#د_که_پای
  }
  وإلا {
    digest = &معلومات->state.digest;
    origin = معلومات->state.origin;
  }

  إذا(!checkprefix("Digest", سرلیک) || !ISBLANK(سرلیک[6])) {
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_AUTH_ERROR;
  }

  سرلیک += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&سرلیک);

  /* This resets the digest struct before decoding */
  پایله = Curl_auth_decode_digest_http_پیغام(سرلیک, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  أرجع پایله;
}

CURLcode Curl_output_digest(بنية Curl_easy *معلومات,
                            bool proxy,
                            ثابت بینښه حرف *request,
                            ثابت بینښه حرف *uripath)
{
  بنية Curl_peer *origin = عدم;
  CURLcode پایله;
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
#که_تعریف_شوی CURL_DISABLE_PROXY
    أرجع CURLE_NOT_BUILT_دننه;
#وإلا
    digest = &معلومات->state.proxydigest;
    origin = معلومات->conn->http_proxy.peer;
    creds = معلومات->conn->http_proxy.creds;
    allocuserpwd = &معلومات->req.hd_proxy_auth;
    authp = &معلومات->state.authproxy;
#د_که_پای
  }
  وإلا {
    DEBUGASSERT(معلومات->state.origin);
    digest = &معلومات->state.digest;
    origin = معلومات->state.origin;
    creds = معلومات->state.creds;
    allocuserpwd = &معلومات->req.hd_auth;
    authp = &معلومات->state.authhost;
  }

  digest_flush_stale(معلومات, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#که_تعریف_شوی USE_WINDOWS_SSPI
  have_chlg = !!digest->ننوت_token;
#وإلا
  have_chlg = !!digest->nonce;
#د_که_پای

  إذا(!have_chlg) {
    authp->بشپړ = FALSE;
    أرجع CURLE_OK;
  }

  پایله = Curl_auth_create_digest_http_پیغام(معلومات, creds, request,
                                                uripath, digest,
                                                &response, &len);
  إذا(پایله)
    أرجع پایله;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  إذا(!*allocuserpwd)
    أرجع CURLE_بهر_د_حافظه;

  authp->بشپړ = TRUE;

  أرجع CURLE_OK;
}

فراغ Curl_http_auth_cleanup_digest(بنية Curl_easy *معلومات)
{
  Curl_auth_digest_cleanup(&معلومات->state.digest);
  Curl_auth_digest_cleanup(&معلومات->state.proxydigest);
}

#د_که_پای
