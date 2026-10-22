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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "vauth/vauth.h"
#समावेश "http_digest.h"
#समावेश "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
अटल व्योम digest_flush_stale(काष्ठा Curl_easy *तथ्य,
                               काष्ठा digestdata *digest,
                               काष्ठा Curl_peer *peer,
                               काष्ठा Curl_creds *creds)
{
  bool flush = FALSE;
  अगर(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(तथ्य, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (अचिन्हित पूर्णांक)(peer ? peer->port : 0));
    flush = TRUE;
  }
  अन्यथा अगर(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(तथ्य, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  अगर(flush) {
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
CURLcode Curl_निवेश_digest(काष्ठा Curl_easy *तथ्य,
                           bool proxy,
                           स्थिर अक्षर *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  काष्ठा digestdata *digest;
  काष्ठा Curl_peer *origin = शून्य;
  CURLcode परिणाम;

  अगर(proxy) {
    digest = &तथ्य->state.proxydigest;
#अगर_घोषित CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    वापस CURLE_OK;  /* just ignore such a header without proxy support */
#अन्यथा
    origin = तथ्य->conn->http_proxy.peer;
#पूर्ण_अगर
  }
  अन्यथा {
    digest = &तथ्य->state.digest;
    origin = तथ्य->state.origin;
  }

  अगर(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    वापस CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  परिणाम = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  वापस परिणाम;
}

CURLcode Curl_output_digest(काष्ठा Curl_easy *तथ्य,
                            bool proxy,
                            स्थिर अचिन्हित अक्षर *request,
                            स्थिर अचिन्हित अक्षर *uripath)
{
  काष्ठा Curl_peer *origin = शून्य;
  CURLcode परिणाम;
  अक्षर *response;
  माप_प्रकार len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  अक्षर **allocuserpwd;

  /* Point to the name and password for this */
  काष्ठा Curl_creds *creds = शून्य;

  /* Point to the correct struct with this */
  काष्ठा digestdata *digest;
  काष्ठा auth *authp;

  अगर(proxy) {
#अगर_घोषित CURL_DISABLE_PROXY
    वापस CURLE_NOT_BUILT_IN;
#अन्यथा
    digest = &तथ्य->state.proxydigest;
    origin = तथ्य->conn->http_proxy.peer;
    creds = तथ्य->conn->http_proxy.creds;
    allocuserpwd = &तथ्य->req.hd_proxy_auth;
    authp = &तथ्य->state.authproxy;
#पूर्ण_अगर
  }
  अन्यथा {
    DEBUGASSERT(तथ्य->state.origin);
    digest = &तथ्य->state.digest;
    origin = तथ्य->state.origin;
    creds = तथ्य->state.creds;
    allocuserpwd = &तथ्य->req.hd_auth;
    authp = &तथ्य->state.authhost;
  }

  digest_flush_stale(तथ्य, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#अगर_घोषित USE_WINDOWS_SSPI
  have_chlg = !!digest->निवेश_token;
#अन्यथा
  have_chlg = !!digest->nonce;
#पूर्ण_अगर

  अगर(!have_chlg) {
    authp->done = FALSE;
    वापस CURLE_OK;
  }

  परिणाम = Curl_auth_create_digest_http_message(तथ्य, creds, request,
                                                uripath, digest,
                                                &response, &len);
  अगर(परिणाम)
    वापस परिणाम;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  अगर(!*allocuserpwd)
    वापस CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  वापस CURLE_OK;
}

व्योम Curl_http_auth_cleanup_digest(काष्ठा Curl_easy *तथ्य)
{
  Curl_auth_digest_cleanup(&तथ्य->state.digest);
  Curl_auth_digest_cleanup(&तथ्य->state.proxydigest);
}

#पूर्ण_अगर
