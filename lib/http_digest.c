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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "vauth/vauth.h"
#شامل "http_digest.h"
#شامل "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
ساکن خالی digest_flush_stale(ساخت Curl_easy *ڈیٹا,
                               ساخت digestdata *digest,
                               ساخت Curl_peer *peer,
                               ساخت Curl_creds *creds)
{
  bool flush = FALSE;
  اگر(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(ڈیٹا, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (غیرعلامتی صحیح)(peer ? peer->پورٹ : 0));
    flush = TRUE;
  }
  ورنہ اگر(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(ڈیٹا, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  اگر(flush) {
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
CURLcode Curl_داخلہ_digest(ساخت Curl_easy *ڈیٹا,
                           bool proxy,
                           ثابت حرف *سرخی) /* rest of the *-authenticate:
سرخی*/
{
  /* Point to the correct struct with this */
  ساخت digestdata *digest;
  ساخت Curl_peer *origin = تہی;
  CURLcode نتیجہ;

  اگر(proxy) {
    digest = &ڈیٹا->state.proxydigest;
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    واپس CURLE_OK;  /* just ignore such a header without proxy support */
#ورنہ
    origin = ڈیٹا->conn->http_proxy.peer;
#اگر_ختم
  }
  ورنہ {
    digest = &ڈیٹا->state.digest;
    origin = ڈیٹا->state.origin;
  }

  اگر(!checkprefix("Digest", سرخی) || !ISBLANK(سرخی[6])) {
    Curl_auth_digest_cleanup(digest);
    واپس CURLE_AUTH_ERROR;
  }

  سرخی += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&سرخی);

  /* This resets the digest struct before decoding */
  نتیجہ = Curl_auth_decode_digest_http_پیغام(سرخی, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  واپس نتیجہ;
}

CURLcode Curl_output_digest(ساخت Curl_easy *ڈیٹا,
                            bool proxy,
                            ثابت غیرعلامتی حرف *request,
                            ثابت غیرعلامتی حرف *uripath)
{
  ساخت Curl_peer *origin = تہی;
  CURLcode نتیجہ;
  حرف *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  حرف **allocuserpwd;

  /* Point to the name and password for this */
  ساخت Curl_creds *creds = تہی;

  /* Point to the correct struct with this */
  ساخت digestdata *digest;
  ساخت auth *authp;

  اگر(proxy) {
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
    واپس CURLE_NOT_BUILT_اندر;
#ورنہ
    digest = &ڈیٹا->state.proxydigest;
    origin = ڈیٹا->conn->http_proxy.peer;
    creds = ڈیٹا->conn->http_proxy.creds;
    allocuserpwd = &ڈیٹا->req.hd_proxy_auth;
    authp = &ڈیٹا->state.authproxy;
#اگر_ختم
  }
  ورنہ {
    DEBUGASSERT(ڈیٹا->state.origin);
    digest = &ڈیٹا->state.digest;
    origin = ڈیٹا->state.origin;
    creds = ڈیٹا->state.creds;
    allocuserpwd = &ڈیٹا->req.hd_auth;
    authp = &ڈیٹا->state.authhost;
  }

  digest_flush_stale(ڈیٹا, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  have_chlg = !!digest->داخلہ_token;
#ورنہ
  have_chlg = !!digest->nonce;
#اگر_ختم

  اگر(!have_chlg) {
    authp->مکمل = FALSE;
    واپس CURLE_OK;
  }

  نتیجہ = Curl_auth_create_digest_http_پیغام(ڈیٹا, creds, request,
                                                uripath, digest,
                                                &response, &len);
  اگر(نتیجہ)
    واپس نتیجہ;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  اگر(!*allocuserpwd)
    واپس CURLE_برآمد_کا_حافظہ;

  authp->مکمل = TRUE;

  واپس CURLE_OK;
}

خالی Curl_http_auth_cleanup_digest(ساخت Curl_easy *ڈیٹا)
{
  Curl_auth_digest_cleanup(&ڈیٹا->state.digest);
  Curl_auth_digest_cleanup(&ڈیٹا->state.proxydigest);
}

#اگر_ختم
