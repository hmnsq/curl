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
ساكن فراغ digest_flush_stale(بنية Curl_easy *داده,
                               بنية digestdata *digest,
                               بنية Curl_peer *peer,
                               بنية Curl_creds *creds)
{
  bool flush = FALSE;
  إذا(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(داده, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (بیعلامت صحيح)(peer ? peer->پورت : 0));
    flush = TRUE;
  }
  وإلا إذا(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(داده, "http_digest, reset on creds change to %s",
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
CURLcode Curl_درون_داد_digest(بنية Curl_easy *داده,
                           bool proxy,
                           ثابت حرف *سرآیند) /* rest of the *-authenticate:
سرآیند*/
{
  /* Point to the correct struct with this */
  بنية digestdata *digest;
  بنية Curl_peer *origin = عدم;
  CURLcode نتیجه;

  إذا(proxy) {
    digest = &داده->state.proxydigest;
#اگر_تعریف_شده CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_OK;  /* just ignore such a header without proxy support */
#وإلا
    origin = داده->conn->http_proxy.peer;
#پایان_اگر
  }
  وإلا {
    digest = &داده->state.digest;
    origin = داده->state.origin;
  }

  إذا(!checkprefix("Digest", سرآیند) || !ISBLANK(سرآیند[6])) {
    Curl_auth_digest_cleanup(digest);
    أرجع CURLE_AUTH_ERROR;
  }

  سرآیند += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&سرآیند);

  /* This resets the digest struct before decoding */
  نتیجه = Curl_auth_decode_digest_http_پیام(سرآیند, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  أرجع نتیجه;
}

CURLcode Curl_output_digest(بنية Curl_easy *داده,
                            bool proxy,
                            ثابت بیعلامت حرف *request,
                            ثابت بیعلامت حرف *uripath)
{
  بنية Curl_peer *origin = عدم;
  CURLcode نتیجه;
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
#اگر_تعریف_شده CURL_DISABLE_PROXY
    أرجع CURLE_NOT_BUILT_درون;
#وإلا
    digest = &داده->state.proxydigest;
    origin = داده->conn->http_proxy.peer;
    creds = داده->conn->http_proxy.creds;
    allocuserpwd = &داده->req.hd_proxy_auth;
    authp = &داده->state.authproxy;
#پایان_اگر
  }
  وإلا {
    DEBUGASSERT(داده->state.origin);
    digest = &داده->state.digest;
    origin = داده->state.origin;
    creds = داده->state.creds;
    allocuserpwd = &داده->req.hd_auth;
    authp = &داده->state.authhost;
  }

  digest_flush_stale(داده, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#اگر_تعریف_شده USE_WINDOWS_SSPI
  have_chlg = !!digest->درون_داد_token;
#وإلا
  have_chlg = !!digest->nonce;
#پایان_اگر

  إذا(!have_chlg) {
    authp->انجامشده = FALSE;
    أرجع CURLE_OK;
  }

  نتیجه = Curl_auth_create_digest_http_پیام(داده, creds, request,
                                                uripath, digest,
                                                &response, &len);
  إذا(نتیجه)
    أرجع نتیجه;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  إذا(!*allocuserpwd)
    أرجع CURLE_بیرون_از_حافظه;

  authp->انجامشده = TRUE;

  أرجع CURLE_OK;
}

فراغ Curl_http_auth_cleanup_digest(بنية Curl_easy *داده)
{
  Curl_auth_digest_cleanup(&داده->state.digest);
  Curl_auth_digest_cleanup(&داده->state.proxydigest);
}

#پایان_اگر
