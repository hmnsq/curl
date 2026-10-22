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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
ਸਥਿਰ ਖਾਲੀ digest_flush_stale(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ digestdata *digest,
                               ਬਣਤਰ Curl_peer *peer,
                               ਬਣਤਰ Curl_creds *creds)
{
  bool flush = FALSE;
  ਜੇ(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(ਅੰਕੜੇ, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(peer ? peer->ਪੋਰਟ : 0));
    flush = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(ਅੰਕੜੇ, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  ਜੇ(flush) {
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
CURLcode Curl_ਨਿਵੇਸ਼_digest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           bool proxy,
                           ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ) /* rest of the *-authenticate:
ਸਿਰਲੇਖ*/
{
  /* Point to the correct struct with this */
  ਬਣਤਰ digestdata *digest;
  ਬਣਤਰ Curl_peer *origin = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(proxy) {
    digest = &ਅੰਕੜੇ->state.proxydigest;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    ਵਾਪਸ CURLE_OK;  /* just ignore such a header without proxy support */
#ਨਹੀਂਤਾਂ
    origin = ਅੰਕੜੇ->conn->http_proxy.peer;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    digest = &ਅੰਕੜੇ->state.digest;
    origin = ਅੰਕੜੇ->state.origin;
  }

  ਜੇ(!checkprefix("Digest", ਸਿਰਲੇਖ) || !ISBLANK(ਸਿਰਲੇਖ[6])) {
    Curl_auth_digest_cleanup(digest);
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  ਸਿਰਲੇਖ += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&ਸਿਰਲੇਖ);

  /* This resets the digest struct before decoding */
  ਨਤੀਜਾ = Curl_auth_decode_digest_http_ਸੁਨੇਹਾ(ਸਿਰਲੇਖ, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_output_digest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool proxy,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *request,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uripath)
{
  ਬਣਤਰ Curl_peer *origin = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ਅੱਖਰ **allocuserpwd;

  /* Point to the name and password for this */
  ਬਣਤਰ Curl_creds *creds = ਨਿਰਾ;

  /* Point to the correct struct with this */
  ਬਣਤਰ digestdata *digest;
  ਬਣਤਰ auth *authp;

  ਜੇ(proxy) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਨਹੀਂਤਾਂ
    digest = &ਅੰਕੜੇ->state.proxydigest;
    origin = ਅੰਕੜੇ->conn->http_proxy.peer;
    creds = ਅੰਕੜੇ->conn->http_proxy.creds;
    allocuserpwd = &ਅੰਕੜੇ->req.hd_proxy_auth;
    authp = &ਅੰਕੜੇ->state.authproxy;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(ਅੰਕੜੇ->state.origin);
    digest = &ਅੰਕੜੇ->state.digest;
    origin = ਅੰਕੜੇ->state.origin;
    creds = ਅੰਕੜੇ->state.creds;
    allocuserpwd = &ਅੰਕੜੇ->req.hd_auth;
    authp = &ਅੰਕੜੇ->state.authhost;
  }

  digest_flush_stale(ਅੰਕੜੇ, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  have_chlg = !!digest->ਨਿਵੇਸ਼_token;
#ਨਹੀਂਤਾਂ
  have_chlg = !!digest->nonce;
#ਜੇ_ਅੰਤ

  ਜੇ(!have_chlg) {
    authp->ਪੂਰਾ = FALSE;
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = Curl_auth_create_digest_http_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds, request,
                                                uripath, digest,
                                                &response, &len);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  ਜੇ(!*allocuserpwd)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  authp->ਪੂਰਾ = TRUE;

  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_http_auth_cleanup_digest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_auth_digest_cleanup(&ਅੰਕੜੇ->state.digest);
  Curl_auth_digest_cleanup(&ਅੰਕੜੇ->state.proxydigest);
}

#ਜੇ_ਅੰਤ
