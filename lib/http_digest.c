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
स्थिर शून्य digest_flush_stale(संरचना Curl_easy *दत्त,
                               संरचना digestdata *digest,
                               संरचना Curl_peer *peer,
                               संरचना Curl_creds *creds)
{
  bool flush = FALSE;
  यदि(!digest->origin || !Curl_peer_equal(peer, digest->origin)) {
    CURL_TRC_M(दत्त, "http_digest, reset on peer change to %s:%u",
               peer ? peer->hostname : "-",
               (अचिह्नित पूर्णाङ्क)(peer ? peer->पत्तनम् : 0));
    flush = TRUE;
  }
  अन्यथा यदि(digest->creds && !Curl_creds_same(creds, digest->creds)) {
    CURL_TRC_M(दत्त, "http_digest, reset on creds change to %s",
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
CURLcode Curl_निवेशः_digest(संरचना Curl_easy *दत्त,
                           bool proxy,
                           ध्रुव वर्ण *शीर्षकम्) /* rest of the *-authenticate:
शीर्षकम्*/
{
  /* Point to the correct struct with this */
  संरचना digestdata *digest;
  संरचना Curl_peer *origin = रिक्त;
  CURLcode परिणाम;

  यदि(proxy) {
    digest = &दत्त->state.proxydigest;
#यदि_परिभाषितम् CURL_DISABLE_PROXY
    Curl_auth_digest_cleanup(digest);
    प्रत्यागम CURLE_OK;  /* just ignore such a header without proxy support */
#अन्यथा
    origin = दत्त->conn->http_proxy.peer;
#यदि_अन्तः
  }
  अन्यथा {
    digest = &दत्त->state.digest;
    origin = दत्त->state.origin;
  }

  यदि(!checkprefix("Digest", शीर्षकम्) || !ISBLANK(शीर्षकम्[6])) {
    Curl_auth_digest_cleanup(digest);
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  शीर्षकम् += CURL_CSTRLEN("Digest");
  curlx_str_passblanks(&शीर्षकम्);

  /* This resets the digest struct before decoding */
  परिणाम = Curl_auth_decode_digest_http_सन्देशः(शीर्षकम्, digest);
  /* Remember only the peer, the data we take in has no relation to creds
   * at this time. We can use it even if creds change. */
  Curl_peer_link(&digest->origin, origin);
  प्रत्यागम परिणाम;
}

CURLcode Curl_output_digest(संरचना Curl_easy *दत्त,
                            bool proxy,
                            ध्रुव अचिह्नित वर्ण *request,
                            ध्रुव अचिह्नित वर्ण *uripath)
{
  संरचना Curl_peer *origin = रिक्त;
  CURLcode परिणाम;
  वर्ण *response;
  size_t len;
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
#यदि_परिभाषितम् CURL_DISABLE_PROXY
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#अन्यथा
    digest = &दत्त->state.proxydigest;
    origin = दत्त->conn->http_proxy.peer;
    creds = दत्त->conn->http_proxy.creds;
    allocuserpwd = &दत्त->req.hd_proxy_auth;
    authp = &दत्त->state.authproxy;
#यदि_अन्तः
  }
  अन्यथा {
    DEBUGASSERT(दत्त->state.origin);
    digest = &दत्त->state.digest;
    origin = दत्त->state.origin;
    creds = दत्त->state.creds;
    allocuserpwd = &दत्त->req.hd_auth;
    authp = &दत्त->state.authhost;
  }

  digest_flush_stale(दत्त, digest, origin, creds);
  curlx_safefree(*allocuserpwd);

#यदि_परिभाषितम् USE_WINDOWS_SSPI
  have_chlg = !!digest->निवेशः_token;
#अन्यथा
  have_chlg = !!digest->nonce;
#यदि_अन्तः

  यदि(!have_chlg) {
    authp->समाप्तम् = FALSE;
    प्रत्यागम CURLE_OK;
  }

  परिणाम = Curl_auth_create_digest_http_सन्देशः(दत्त, creds, request,
                                                uripath, digest,
                                                &response, &len);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  *allocuserpwd = curl_maprintf("%sAuthorization: Digest %s\r\n",
                                proxy ? "Proxy-" : "", response);
  curlx_free(response);
  यदि(!*allocuserpwd)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  authp->समाप्तम् = TRUE;

  प्रत्यागम CURLE_OK;
}

शून्य Curl_http_auth_cleanup_digest(संरचना Curl_easy *दत्त)
{
  Curl_auth_digest_cleanup(&दत्त->state.digest);
  Curl_auth_digest_cleanup(&दत्त->state.proxydigest);
}

#यदि_अन्तः
