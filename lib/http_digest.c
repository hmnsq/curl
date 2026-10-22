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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "http_digest.h"
#அந்தர்பாவ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
நிலையான வெறுமை digest_flush_stale(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு digestdata *digest,
                               கட்டமைப்பு Curl_peer *peer,
                               கட்டமைப்பு Curl_creds *creds)
{
  bool flush = FALSE;
  எனில்(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(data, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (unsigned முழு)(peer ? peer->port : 0));
    flush = TRUE;
  }
  இல்லையெனில் எனில்(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(data, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  எனில்(flush) {
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
CURLcode Curl_உள்ளீடு_digest(கட்டமைப்பு Curl_easy *data,
                           bool proxy,
                           மாறிலி எழுத்து *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  கட்டமைப்பு digestdata *digest;
  கட்டமைப்பு Curl_peer *origin = வெற்று;
  CURLcode result;

  எனில்(proxy) {
    digest = &data->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    திரும்பு CURLE_OK;  /* just ignore such a header without proxy support */
#இல்லையெனில்
    origin = data->conn->http_proxy.peer;
#endif
  }
  இல்லையெனில் {
    digest = &data->state.digest;
    origin = data->state.origin;
  }

  எனில்(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    திரும்பு CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  result = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  திரும்பு result;
}

CURLcode Curl_output_digest(கட்டமைப்பு Curl_easy *data,
                            bool proxy,
                            மாறிலி unsigned எழுத்து *request,
                            மாறிலி unsigned எழுத்து *uripath)
{
  கட்டமைப்பு Curl_peer *origin = வெற்று;
  CURLcode result;
  எழுத்து *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  எழுத்து **allocuserpwd;

  /* Point to the name and password for this */
  கட்டமைப்பு Curl_creds *creds = வெற்று;

  /* Point to the correct struct with this */
  கட்டமைப்பு digestdata *digest;
  கட்டமைப்பு auth *authp;

  எனில்(proxy) {
#ifdef CURL_DISABLE_PROXY
    திரும்பு CURLE_NOT_BUILT_IN;
#இல்லையெனில்
    digest = &data->state.proxydigest;
    origin = data->conn->http_proxy.peer;
    creds = data->conn->http_proxy.creds;
    allocuserpwd = &data->req.hd_proxy_auth;
    authp = &data->state.authproxy;
#endif
  }
  இல்லையெனில் {
    DEBUGASSERT(data->state.origin);
    digest = &data->state.digest;
    origin = data->state.origin;
    creds = data->state.creds;
    allocuserpwd = &data->req.hd_auth;
    authp = &data->state.authhost;
  }

  digest_flush_stale(data, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#ifdef USE_WINDOWS_SSPI
  have_chlg = !!digest->உள்ளீடு_token;
#இல்லையெனில்
  have_chlg = !!digest->nonce;
#endif

  எனில்(!have_chlg) {
    authp->done = FALSE;
    திரும்பு CURLE_OK;
  }

  result = Curl_auth_create_digest_http_message(data, creds, request,
                                                uripath, digest,
                                                &response, &len);
  எனில்(result)
    திரும்பு result;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  எனில்(!*allocuserpwd)
    திரும்பு CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  திரும்பு CURLE_OK;
}

வெறுமை Curl_http_auth_cleanup_digest(கட்டமைப்பு Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.digest);
  Curl_auth_digest_cleanup(&data->state.proxydigest);
}

#endif
