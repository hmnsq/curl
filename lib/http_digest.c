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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "http_digest.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
ಸ್ಥಿರ ಶೂನ್ಯ digest_flush_stale(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ digestdata *digest,
                               ರಚನೆ Curl_peer *peer,
                               ರಚನೆ Curl_creds *creds)
{
  bool flush = FALSE;
  ಆದರೆ(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(peer ? peer->ಪೋರ್ಟ್ : 0));
    flush = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  ಆದರೆ(flush) {
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
CURLcode Curl_ಒಳಹರಿವು_digest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           bool proxy,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ) /* rest of the *-authenticate:
ಶೀರ್ಷಿಕೆ*/
{
  /* Point to the correct struct with this */
  ರಚನೆ digestdata *digest;
  ರಚನೆ Curl_peer *origin = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(proxy) {
    digest = &ದತ್ತಾಂಶ->state.proxydigest;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    ಹಿಂತಿರುಗಿ CURLE_OK;  /* just ignore such a header without proxy support */
#ಇಲ್ಲದಿದ್ದರೆ
    origin = ದತ್ತಾಂಶ->conn->http_proxy.peer;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    digest = &ದತ್ತಾಂಶ->state.digest;
    origin = ದತ್ತಾಂಶ->state.origin;
  }

  ಆದರೆ(!checkprefix("Digest", ಶೀರ್ಷಿಕೆ) || !ISBLANK(ಶೀರ್ಷಿಕೆ[6])) {
    Curl_auth_digest_cleanup(digest);
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  ಶೀರ್ಷಿಕೆ += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&ಶೀರ್ಷಿಕೆ);

  /* This resets the digest struct before decoding */
  ಫಲಿತಾಂಶ = Curl_auth_decode_digest_http_ಸಂದೇಶ(ಶೀರ್ಷಿಕೆ, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_output_digest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool proxy,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *request,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uripath)
{
  ರಚನೆ Curl_peer *origin = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ಅಕ್ಷರ **allocuserpwd;

  /* Point to the name and password for this */
  ರಚನೆ Curl_creds *creds = ಖಾಲಿ;

  /* Point to the correct struct with this */
  ರಚನೆ digestdata *digest;
  ರಚನೆ auth *authp;

  ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಇಲ್ಲದಿದ್ದರೆ
    digest = &ದತ್ತಾಂಶ->state.proxydigest;
    origin = ದತ್ತಾಂಶ->conn->http_proxy.peer;
    creds = ದತ್ತಾಂಶ->conn->http_proxy.creds;
    allocuserpwd = &ದತ್ತಾಂಶ->req.hd_proxy_auth;
    authp = &ದತ್ತಾಂಶ->state.authproxy;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(ದತ್ತಾಂಶ->state.origin);
    digest = &ದತ್ತಾಂಶ->state.digest;
    origin = ದತ್ತಾಂಶ->state.origin;
    creds = ದತ್ತಾಂಶ->state.creds;
    allocuserpwd = &ದತ್ತಾಂಶ->req.hd_auth;
    authp = &ದತ್ತಾಂಶ->state.authhost;
  }

  digest_flush_stale(ದತ್ತಾಂಶ, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  have_chlg = !!digest->ಒಳಹರಿವು_token;
#ಇಲ್ಲದಿದ್ದರೆ
  have_chlg = !!digest->nonce;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!have_chlg) {
    authp->ಮುಗಿದ = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = Curl_auth_create_digest_http_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds, request,
                                                uripath, digest,
                                                &response, &len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  ಆದರೆ(!*allocuserpwd)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  authp->ಮುಗಿದ = TRUE;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_http_auth_cleanup_digest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_auth_digest_cleanup(&ದತ್ತಾಂಶ->state.digest);
  Curl_auth_digest_cleanup(&ದತ್ತಾಂಶ->state.proxydigest);
}

#ಆದರೆ_ಅಂತ್ಯ
