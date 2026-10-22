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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "http_digest.h"
#അന്തര്ഭാവ "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
സ്ഥിതം ശൂന്യം digest_flush_stale(ഘടന Curl_easy *data,
                               ഘടന digestdata *digest,
                               ഘടന Curl_peer *peer,
                               ഘടന Curl_creds *creds)
{
  bool flush = FALSE;
  എങ്കിൽ(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(data, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (unsigned പൂർണ്ണസംഖ്യ)(peer ? peer->port : 0));
    flush = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(data, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  എങ്കിൽ(flush) {
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
CURLcode Curl_നിവേശം_digest(ഘടന Curl_easy *data,
                           bool proxy,
                           സ്ഥിരം അക്ഷരം *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  ഘടന digestdata *digest;
  ഘടന Curl_peer *origin = ശൂന്യ;
  CURLcode result;

  എങ്കിൽ(proxy) {
    digest = &data->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    തിരികെ CURLE_OK;  /* just ignore such a header without proxy support */
#അല്ലെങ്കിൽ
    origin = data->conn->http_proxy.peer;
#endif
  }
  അല്ലെങ്കിൽ {
    digest = &data->state.digest;
    origin = data->state.origin;
  }

  എങ്കിൽ(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    തിരികെ CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  result = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  തിരികെ result;
}

CURLcode Curl_output_digest(ഘടന Curl_easy *data,
                            bool proxy,
                            സ്ഥിരം unsigned അക്ഷരം *request,
                            സ്ഥിരം unsigned അക്ഷരം *uripath)
{
  ഘടന Curl_peer *origin = ശൂന്യ;
  CURLcode result;
  അക്ഷരം *response;
  size_t len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  അക്ഷരം **allocuserpwd;

  /* Point to the name and password for this */
  ഘടന Curl_creds *creds = ശൂന്യ;

  /* Point to the correct struct with this */
  ഘടന digestdata *digest;
  ഘടന auth *authp;

  എങ്കിൽ(proxy) {
#ifdef CURL_DISABLE_PROXY
    തിരികെ CURLE_NOT_BUILT_IN;
#അല്ലെങ്കിൽ
    digest = &data->state.proxydigest;
    origin = data->conn->http_proxy.peer;
    creds = data->conn->http_proxy.creds;
    allocuserpwd = &data->req.hd_proxy_auth;
    authp = &data->state.authproxy;
#endif
  }
  അല്ലെങ്കിൽ {
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
  have_chlg = !!digest->നിവേശം_token;
#അല്ലെങ്കിൽ
  have_chlg = !!digest->nonce;
#endif

  എങ്കിൽ(!have_chlg) {
    authp->done = FALSE;
    തിരികെ CURLE_OK;
  }

  result = Curl_auth_create_digest_http_message(data, creds, request,
                                                uripath, digest,
                                                &response, &len);
  എങ്കിൽ(result)
    തിരികെ result;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  എങ്കിൽ(!*allocuserpwd)
    തിരികെ CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_http_auth_cleanup_digest(ഘടന Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.digest);
  Curl_auth_digest_cleanup(&data->state.proxydigest);
}

#endif
