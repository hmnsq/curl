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

/* This file is for implementing all "generic" SSL functions that all libcurl
   internals should use. It is then responsible for calling the proper
   "backend" function.

   SSL-functions in libcurl should call functions in this source file, and not
   to any specific SSL-layer.

   Curl_ssl_ - prefix for generic ones

   Note that this source code uses the functions of the configured SSL
   backend via the global Curl_ssl instance.

   "SSL/TLS Strong Encryption: An Introduction"
   https://httpd.apache.org/docs/2.0/ssl/ssl_intro.html
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_config.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLONE_STRING(var)                    \
  ਕਰੋ {                                       \
    ਜੇ(ਸਰੋਤ->var) {                        \
      dest->var = curlx_strdup(ਸਰੋਤ->var); \
      ਜੇ(!dest->var)                         \
        ਵਾਪਸ FALSE;                        \
    }                                        \
    ਨਹੀਂਤਾਂ                                     \
      dest->var = ਨਿਰਾ;                      \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLONE_BLOB(var)                  \
  ਕਰੋ {                                   \
    ਜੇ(blobdup(&dest->var, ਸਰੋਤ->var)) \
      ਵਾਪਸ FALSE;                      \
  } ਜਦੋਂ(0)

ਸਥਿਰ CURLcode blobdup(ਬਣਤਰ curl_blob **dest, ਬਣਤਰ curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  ਜੇ(src) {
    /* only if there is data to dupe! */
    ਬਣਤਰ curl_blob *d;
    d = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ curl_blob) + src->len);
    ਜੇ(!d)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->ਅੰਕੜੇ = (ਖਾਲੀ *)((ਅੱਖਰ *)d + ਆਕਾਰ(ਬਣਤਰ curl_blob));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(d->ਅੰਕੜੇ, src->ਅੰਕੜੇ, src->len);
    *dest = d;
  }
  ਵਾਪਸ CURLE_OK;
}

/* returns TRUE if the blobs are identical */
ਸਥਿਰ bool blobcmp(ਬਣਤਰ curl_blob *ਪਹਿਲਾ, ਬਣਤਰ curl_blob *ਦੂਜਾ)
{
  ਜੇ(!ਪਹਿਲਾ && !ਦੂਜਾ) /* both are NULL */
    ਵਾਪਸ TRUE;
  ਜੇ(!ਪਹਿਲਾ || !ਦੂਜਾ) /* one is NULL */
    ਵਾਪਸ FALSE;
  ਜੇ(ਪਹਿਲਾ->len != ਦੂਜਾ->len) /* different sizes */
    ਵਾਪਸ FALSE;
  ਵਾਪਸ !memcmp(ਪਹਿਲਾ->ਅੰਕੜੇ, ਦੂਜਾ->ਅੰਕੜੇ, ਪਹਿਲਾ->len); /* same data */
}

ਖਾਲੀ Curl_ssl_config_init(ਬਣਤਰ ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

ਖਾਲੀ Curl_ssl_config_cleanup(ਬਣਤਰ ssl_filter_config *sslc)
{
  ਜੇ(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_ਕੁੰਜੀ);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->ਕੁੰਜੀ_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->ਕੁੰਜੀ);
    curlx_safefree(sslc->ਕੁੰਜੀ_type);
    curlx_safefree(sslc->ਕੁੰਜੀ_passwd);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_ਜਨਤਕ);
#ਜੇ_ਅੰਤ
    sslc->deep_copy = FALSE;
  }
}

ਸਥਿਰ bool ਮੇਲ_ssl_primary_config(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssl_filter_config *c1,
                                     ਬਣਤਰ ssl_filter_config *c2)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ((c1->ਸੰਸਕਰਣ_2 == c2->ਸੰਸਕਰਣ_2) &&
     (c1->ਸੰਸਕਰਣ_max == c2->ਸੰਸਕਰਣ_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_ਜਨਤਕ, c2->ech_ਜਨਤਕ) &&
#ਜੇ_ਅੰਤ
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->ਕੁੰਜੀ_blob, c2->ਕੁੰਜੀ_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_ਕੁੰਜੀ, c2->pinned_ਕੁੰਜੀ) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->ਕੁੰਜੀ, c2->ਕੁੰਜੀ) &&
     curl_strequal(c1->ਕੁੰਜੀ_type, c2->ਕੁੰਜੀ_type) &&
     !Curl_timestrcmp(c1->ਕੁੰਜੀ_passwd, c2->ਕੁੰਜੀ_passwd))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

bool Curl_ssl_conn_config_ਮੇਲ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssl_filter_config *conn_config,
                                ਬਣਤਰ connectdata *candidate,
                                bool proxy)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(proxy)
    ਵਾਪਸ ਮੇਲ_ssl_primary_config(ਅੰਕੜੇ, conn_config,
                                    &candidate->proxy_ssl_config);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)proxy;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਮੇਲ_ssl_primary_config(ਅੰਕੜੇ, conn_config,
                                  &candidate->ssl_config);
}

ਸਥਿਰ bool clone_ssl_primary_config(ਬਣਤਰ ssl_filter_config *ਸਰੋਤ,
                                     ਬਣਤਰ ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->ਸੰਸਕਰਣ_2 = ਸਰੋਤ->ਸੰਸਕਰਣ_2;
  dest->ਸੰਸਕਰਣ_max = ਸਰੋਤ->ਸੰਸਕਰਣ_max;
  dest->verifypeer = ਸਰੋਤ->verifypeer;
  dest->verifyhost = ਸਰੋਤ->verifyhost;
  dest->verifystatus = ਸਰੋਤ->verifystatus;
  dest->ssl_options = ਸਰੋਤ->ssl_options;
  dest->native_ca_store = ਸਰੋਤ->native_ca_store;
  dest->cache_session = ਸਰੋਤ->cache_session;
  dest->auto_client_cert = ਸਰੋਤ->auto_client_cert;
  dest->earlydata = ਸਰੋਤ->earlydata;
  dest->enable_beast = ਸਰੋਤ->enable_beast;
  dest->no_partialchain = ਸਰੋਤ->no_partialchain;
  dest->no_revoke = ਸਰੋਤ->no_revoke;
  dest->revoke_ਉੱਤਮ_effort = ਸਰੋਤ->revoke_ਉੱਤਮ_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_ਕੁੰਜੀ);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(ਕੁੰਜੀ);
  CLONE_STRING(ਕੁੰਜੀ_type);
  CLONE_STRING(ਕੁੰਜੀ_passwd);
  CLONE_BLOB(ਕੁੰਜੀ_blob);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  dest->ech = ਸਰੋਤ->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_ਜਨਤਕ);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਖਾਲੀ
ssl_easy_config_compl_options(ਬਣਤਰ Curl_peer *origin,
                              ਬਣਤਰ Curl_peer *initial_origin,
                              ਬਣਤਰ ssl_easy_config *sslc,
                              ਬਣਤਰ ssl_filter_config *primary)
{
  uint8_t options = sslc->ssl_options;
  /* If set via CURLOPT_(PROXY_)SSL_OPTIONS, we definitely use it.
   * If not, we switch it on for supported backends if no custom
   * CA settings exist. */
  primary->native_ca_store = !!(options & CURLSSLOPT_NATIVE_CA);
  primary->earlydata = !!(options & CURLSSLOPT_EARLYDATA);
  primary->auto_client_cert =
    Curl_peer_equal(origin, initial_origin) &&
    !!(options & CURLSSLOPT_AUTO_CLIENT_CERT);
  primary->enable_beast = !!(options & CURLSSLOPT_ALLOW_BEAST);
  primary->no_partialchain = !!(options & CURLSSLOPT_NO_PARTIALCHAIN);
  primary->no_revoke = !!(options & CURLSSLOPT_NO_REVOKE);
  primary->revoke_ਉੱਤਮ_effort = !!(options & CURLSSLOPT_REVOKE_ਉੱਤਮ_EFFORT);
}

ਸਥਿਰ ਅੱਖਰ *ssl_easy_steal(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਗਿਣਤੀ dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  ਵਾਪਸ CURL_UNCONST(CURL_EASY_STR(ਅੰਕੜੇ, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_peer *origin,
  ਬਣਤਰ ssl_filter_config *ssl_origin,
  ਬਣਤਰ ssl_filter_config *ssl_proxy)
{
  ਬਣਤਰ ssl_easy_config *sslc = &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
#ਜੇ defined(CURL_CA_ਰਸਤਾ) || defined(CURL_CA_BUNDLE)
  CURLcode ਨਤੀਜਾ;
#ਜੇ_ਅੰਤ

  ssl_easy_config_compl_options(origin, ਅੰਕੜੇ->state.initial_origin, sslc,
                                ssl_origin);

  ਜੇ(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#ਜੇ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ਜੇ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_ਰਸਤਾ
    ਜੇ(!sslc->custom_capath && !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAPATH)) {
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAPATH, CURL_CA_ਰਸਤਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_BUNDLE
    ਜੇ(!sslc->custom_cafile && !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAFILE)) {
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
  }

  ssl_origin->ਸੰਸਕਰਣ_2 = sslc->ਸੰਸਕਰਣ_2;
  ssl_origin->ਸੰਸਕਰਣ_max = sslc->ਸੰਸਕਰਣ_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_SSL_ISSUERCERT];

  ਜੇ(Curl_peer_equal(ਅੰਕੜੇ->state.initial_origin, origin)) {
    ssl_origin->pinned_ਕੁੰਜੀ =
      ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(ਅੰਕੜੇ, STRING_CERT_TYPE);
    ssl_origin->ਕੁੰਜੀ = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ);
    ssl_origin->ਕੁੰਜੀ_type = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_TYPE);
    ssl_origin->ਕੁੰਜੀ_passwd = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(ਅੰਕੜੇ, STRING_CERT);
    ssl_origin->ਕੁੰਜੀ_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_ਕੁੰਜੀ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
    ssl_origin->ech = ਅੰਕੜੇ->ਸੈੱਟ.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(ਅੰਕੜੇ, STRING_ECH_CONFIG);
    ssl_origin->ech_ਜਨਤਕ = ssl_easy_steal(ਅੰਕੜੇ, STRING_ECH_ਜਨਤਕ);
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    ssl_origin->pinned_ਕੁੰਜੀ = ਨਿਰਾ;
    ssl_origin->cert_blob = ਨਿਰਾ;
    ssl_origin->cert_type = ਨਿਰਾ;
    ssl_origin->ਕੁੰਜੀ = ਨਿਰਾ;
    ssl_origin->ਕੁੰਜੀ_type = ਨਿਰਾ;
    ssl_origin->ਕੁੰਜੀ_passwd = ਨਿਰਾ;
    ssl_origin->clientcert = ਨਿਰਾ;
    ssl_origin->ਕੁੰਜੀ_blob = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = ਨਿਰਾ;
    ssl_origin->ech_ਜਨਤਕ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  sslc = &ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl;

  ssl_easy_config_compl_options(ਨਿਰਾ, ਨਿਰਾ, sslc, ssl_proxy);
  ਜੇ((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#ਜੇ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ਜੇ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_ਰਸਤਾ
    ਜੇ(!sslc->custom_capath &&
       !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAPATH_PROXY)) {
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAPATH_PROXY, CURL_CA_ਰਸਤਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_BUNDLE
    ਜੇ(!sslc->custom_cafile &&
       !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAFILE_PROXY)) {
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
  }

  ssl_proxy->ਸੰਸਕਰਣ_2 = sslc->ਸੰਸਕਰਣ_2;
  ssl_proxy->ਸੰਸਕਰਣ_max = sslc->ਸੰਸਕਰਣ_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_ਕੁੰਜੀ =
    ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(ਅੰਕੜੇ, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(ਅੰਕੜੇ, STRING_CERT_TYPE_PROXY);
  ssl_proxy->ਕੁੰਜੀ = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PROXY);
  ssl_proxy->ਕੁੰਜੀ_type = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_TYPE_PROXY);
  ssl_proxy->ਕੁੰਜੀ_passwd = ssl_easy_steal(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(ਅੰਕੜੇ, STRING_CERT_PROXY);
  ssl_proxy->ਕੁੰਜੀ_blob = ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_ਕੁੰਜੀ_PROXY];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = ਨਿਰਾ;
  ssl_proxy->ech_ਜਨਤਕ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ssl_proxy;
#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(ਬਣਤਰ ssl_filter_config *ssl_config,
                                    ਬਣਤਰ ssl_filter_config *proxy_ssl_config,
                                    ਬਣਤਰ connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  ਜੇ(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)proxy_ssl_config;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssl_conn_config_cleanup(ਬਣਤਰ connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_ssl_conn_config_update(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  ਜੇ(ਅੰਕੜੇ->conn) {
    ਬਣਤਰ ssl_easy_config *src;
    ਬਣਤਰ ssl_filter_config *dest;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    src = for_proxy ? &ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl : &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
    dest = for_proxy ? &ਅੰਕੜੇ->conn->proxy_ssl_config : &ਅੰਕੜੇ->conn->ssl_config;
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)for_proxy;
    src = &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
    dest = &ਅੰਕੜੇ->conn->ssl_config;
#ਜੇ_ਅੰਤ
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
    dest->ech = src->ech;
#ਜੇ_ਅੰਤ
  }
}
