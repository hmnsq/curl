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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "http_digest.h"
#अन्तर्भाव "curlx/strparse.h"

/* Flush the Digest state if it was created for a different origin or with
   different credentials than the ones now in use, then link the current
   ones. */
स्थिर शून्य digest_flush_stale(संरचना Curl_easy *data,
                               संरचना digestdata *digest,
                               संरचना Curl_peer *peer,
                               संरचना Curl_creds *creds)
{
  bool flush = FALSE;
  यदि(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(data, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (अचिन्हित पूर्णाङ्क)(peer ? peer->port : 0));
    flush = TRUE;
  }
  अन्यथा यदि(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(data, "http_digest, reset on creds change to %s",
               creds ? creds->user : "-");
    flush = TRUE;
  }

  यदि(flush) {
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
CURLcode Curl_आगत_digest(संरचना Curl_easy *data,
                           bool proxy,
                           अचर वर्ण *header) /* rest of the *-authenticate:
                                                  header */
{
  /* Point to the correct struct with this */
  संरचना digestdata *digest;
  संरचना Curl_peer *origin = रिक्त;
  CURLcode result;

  यदि(proxy) {
    digest = &data->state.proxydigest;
#ifdef CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    फर्काऊ CURLE_OK;  /* just ignore such a header without proxy support */
#अन्यथा
    origin = data->conn->http_proxy.peer;
#endif
  }
  अन्यथा {
    digest = &data->state.digest;
    origin = data->state.origin;
  }

  यदि(!checkprefix("Digest", header) || !ISBLANK(header[6])) {
    Curl_auth_digest_cleanup(digest);
    फर्काऊ CURLE_AUTH_ERROR;
  }

  header += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&header);

  /* This resets the digest struct before decoding */
  result = Curl_auth_decode_digest_http_message(header, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  फर्काऊ result;
}

CURLcode Curl_output_digest(संरचना Curl_easy *data,
                            bool proxy,
                            अचर अचिन्हित वर्ण *request,
                            अचर अचिन्हित वर्ण *uripath)
{
  संरचना Curl_peer *origin = रिक्त;
  CURLcode result;
  वर्ण *response;
  माप_प्रकार len;
  bool have_chlg;

  /* Point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  वर्ण **allocuserpwd;

  /* Point to the name and password for this */
  संरचना Curl_creds *creds = रिक्त;

  /* Point to the correct struct with this */
  संरचना digestdata *digest;
  संरचना auth *authp;

  यदि(proxy) {
#ifdef CURL_DISABLE_PROXY
    फर्काऊ CURLE_NOT_BUILT_IN;
#अन्यथा
    digest = &data->state.proxydigest;
    origin = data->conn->http_proxy.peer;
    creds = data->conn->http_proxy.creds;
    allocuserpwd = &data->req.hd_proxy_auth;
    authp = &data->state.authproxy;
#endif
  }
  अन्यथा {
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
  have_chlg = !!digest->आगत_token;
#अन्यथा
  have_chlg = !!digest->nonce;
#endif

  यदि(!have_chlg) {
    authp->done = FALSE;
    फर्काऊ CURLE_OK;
  }

  result = Curl_auth_create_digest_http_message(data, creds, request,
                                                uripath, digest,
                                                &response, &len);
  यदि(result)
    फर्काऊ result;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  यदि(!*allocuserpwd)
    फर्काऊ CURLE_OUT_OF_MEMORY;

  authp->done = TRUE;

  फर्काऊ CURLE_OK;
}

शून्य Curl_http_auth_cleanup_digest(संरचना Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.digest);
  Curl_auth_digest_cleanup(&data->state.proxydigest);
}

#endif
