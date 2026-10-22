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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "http_digest.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
ସ୍ଥିର ଶୂନ୍ୟ digest_flush_stale(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା digestdata *digest,
                               ସଂରଚନା Curl_peer *peer,
                               ସଂରଚନା Curl_creds *creds)
{
  bool flush = FALSE;
  ଯଦି(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(data, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(peer ? peer->port : 0));
    flush = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(data, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  ଯଦି(flush) {
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
CURLcode Curl_ନିବେଶ_digest(ସଂରଚନା Curl_easy *data,
                           bool proxy,
                           ଅଚଳ ଅକ୍ଷର *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  ସଂରଚନା digestdata *digest;
  ସଂରଚନା Curl_peer *origin = ଖାଲି;
  CURLcode result;

  ଯଦି(proxy) {
    digest = &data->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    ଫେରନ୍ତୁ CURLE_OK;  /* just ignore such a header without proxy support */
#ଅନ୍ୟଥା
    origin = data->conn->http_proxy.peer;
#endif
  }
  ଅନ୍ୟଥା {
    digest = &data->state.digest;
    origin = data->state.origin;
  }

  ଯଦି(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  result = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_output_digest(ସଂରଚନା Curl_easy *data,
                            bool proxy,
                            ଅଚଳ unsigned ଅକ୍ଷର *request,
                            ଅଚଳ unsigned ଅକ୍ଷର *uripath)
{
  ସଂରଚନା Curl_peer *origin = ଖାଲି;
  CURLcode result;
  ଅକ୍ଷର *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ଅକ୍ଷର **allocuserpwd;

  /* Point to the name and password for this */
  ସଂରଚନା Curl_creds *creds = ଖାଲି;

  /* Point to the correct struct with this */
  ସଂରଚନା digestdata *digest;
  ସଂରଚନା auth *authp;

  ଯଦି(proxy) {
#ifdef CURL_DISABLE_PROXY
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#ଅନ୍ୟଥା
    digest = &data->state.proxydigest;
    origin = data->conn->http_proxy.peer;
    creds = data->conn->http_proxy.creds;
    allocuserpwd = &data->req.hd_proxy_auth;
    authp = &data->state.authproxy;
#endif
  }
  ଅନ୍ୟଥା {
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
  have_chlg = !!digest->ନିବେଶ_token;
#ଅନ୍ୟଥା
  have_chlg = !!digest->nonce;
#endif

  ଯଦି(!have_chlg) {
    authp->done = FALSE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = Curl_auth_create_digest_http_message(data, creds, request,
                                                uripath, digest,
                                                &response, &len);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  ଯଦି(!*allocuserpwd)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_http_auth_cleanup_digest(ସଂରଚନା Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.digest);
  Curl_auth_digest_cleanup(&data->state.proxydigest);
}

#endif
