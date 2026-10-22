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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "http_digest.h"
#অন্তর্ভাব "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
স্থির শূন্য digest_flush_stale(কাঠামো Curl_easy *তথ্য,
                               কাঠামো digestdata *digest,
                               কাঠামো Curl_peer *peer,
                               কাঠামো Curl_creds *creds)
{
  bool flush = FALSE;
  যদি(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(তথ্য, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (unsigned পূর্ণসংখ্যা)(peer ? peer->port : 0));
    flush = TRUE;
  }
  অন্যথা যদি(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(তথ্য, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  যদি(flush) {
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
CURLcode Curl_প্রবেশ_digest(কাঠামো Curl_easy *তথ্য,
                           bool proxy,
                           ধ্রুবক অক্ষর *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  কাঠামো digestdata *digest;
  কাঠামো Curl_peer *origin = ফাঁকা;
  CURLcode ফলাফল;

  যদি(proxy) {
    digest = &তথ্য->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    ফেরত CURLE_OK;  /* just ignore such a header without proxy support */
#অন্যথা
    origin = তথ্য->conn->http_proxy.peer;
#endif
  }
  অন্যথা {
    digest = &তথ্য->state.digest;
    origin = তথ্য->state.origin;
  }

  যদি(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    ফেরত CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  ফলাফল = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  ফেরত ফলাফল;
}

CURLcode Curl_output_digest(কাঠামো Curl_easy *তথ্য,
                            bool proxy,
                            ধ্রুবক unsigned অক্ষর *request,
                            ধ্রুবক unsigned অক্ষর *uripath)
{
  কাঠামো Curl_peer *origin = ফাঁকা;
  CURLcode ফলাফল;
  অক্ষর *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  অক্ষর **allocuserpwd;

  /* Point to the name and password for this */
  কাঠামো Curl_creds *creds = ফাঁকা;

  /* Point to the correct struct with this */
  কাঠামো digestdata *digest;
  কাঠামো auth *authp;

  যদি(proxy) {
#ifdef CURL_DISABLE_PROXY
    ফেরত CURLE_NOT_BUILT_IN;
#অন্যথা
    digest = &তথ্য->state.proxydigest;
    origin = তথ্য->conn->http_proxy.peer;
    creds = তথ্য->conn->http_proxy.creds;
    allocuserpwd = &তথ্য->req.hd_proxy_auth;
    authp = &তথ্য->state.authproxy;
#endif
  }
  অন্যথা {
    DEBUGASSERT(তথ্য->state.origin);
    digest = &তথ্য->state.digest;
    origin = তথ্য->state.origin;
    creds = তথ্য->state.creds;
    allocuserpwd = &তথ্য->req.hd_auth;
    authp = &তথ্য->state.authhost;
  }

  digest_flush_stale(তথ্য, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#ifdef USE_WINDOWS_SSPI
  have_chlg = !!digest->প্রবেশ_token;
#অন্যথা
  have_chlg = !!digest->nonce;
#endif

  যদি(!have_chlg) {
    authp->done = FALSE;
    ফেরত CURLE_OK;
  }

  ফলাফল = Curl_auth_create_digest_http_message(তথ্য, creds, request,
                                                uripath, digest,
                                                &response, &len);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  যদি(!*allocuserpwd)
    ফেরত CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  ফেরত CURLE_OK;
}

শূন্য Curl_http_auth_cleanup_digest(কাঠামো Curl_easy *তথ্য)
{
  Curl_auth_digest_cleanup(&তথ্য->state.digest);
  Curl_auth_digest_cleanup(&তথ্য->state.proxydigest);
}

#endif
