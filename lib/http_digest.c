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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "http_digest.h"
#અન્તર્ભાવ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
સ્થિર શૂન્ય digest_flush_stale(સંરચના Curl_easy *data,
                               સંરચના digestdata *digest,
                               સંરચના Curl_peer *peer,
                               સંરચના Curl_creds *creds)
{
  bool flush = FALSE;
  જો(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(data, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (unsigned પૂર્ણાંક)(peer ? peer->port : 0));
    flush = TRUE;
  }
  નહીંતર જો(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(data, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  જો(flush) {
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
CURLcode Curl_આગત_digest(સંરચના Curl_easy *data,
                           bool proxy,
                           અચલ અક્ષર *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  સંરચના digestdata *digest;
  સંરચના Curl_peer *origin = ખાલી;
  CURLcode result;

  જો(proxy) {
    digest = &data->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    પાછા CURLE_OK;  /* just ignore such a header without proxy support */
#નહીંતર
    origin = data->conn->http_proxy.peer;
#endif
  }
  નહીંતર {
    digest = &data->state.digest;
    origin = data->state.origin;
  }

  જો(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    પાછા CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  result = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  પાછા result;
}

CURLcode Curl_output_digest(સંરચના Curl_easy *data,
                            bool proxy,
                            અચલ unsigned અક્ષર *request,
                            અચલ unsigned અક્ષર *uripath)
{
  સંરચના Curl_peer *origin = ખાલી;
  CURLcode result;
  અક્ષર *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  અક્ષર **allocuserpwd;

  /* Point to the name and password for this */
  સંરચના Curl_creds *creds = ખાલી;

  /* Point to the correct struct with this */
  સંરચના digestdata *digest;
  સંરચના auth *authp;

  જો(proxy) {
#ifdef CURL_DISABLE_PROXY
    પાછા CURLE_NOT_BUILT_IN;
#નહીંતર
    digest = &data->state.proxydigest;
    origin = data->conn->http_proxy.peer;
    creds = data->conn->http_proxy.creds;
    allocuserpwd = &data->req.hd_proxy_auth;
    authp = &data->state.authproxy;
#endif
  }
  નહીંતર {
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
  have_chlg = !!digest->આગત_token;
#નહીંતર
  have_chlg = !!digest->nonce;
#endif

  જો(!have_chlg) {
    authp->done = FALSE;
    પાછા CURLE_OK;
  }

  result = Curl_auth_create_digest_http_message(data, creds, request,
                                                uripath, digest,
                                                &response, &len);
  જો(result)
    પાછા result;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  જો(!*allocuserpwd)
    પાછા CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  પાછા CURLE_OK;
}

શૂન્ય Curl_http_auth_cleanup_digest(સંરચના Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.digest);
  Curl_auth_digest_cleanup(&data->state.proxydigest);
}

#endif
