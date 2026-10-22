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

#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_config.h"


#ವ್ಯಾಖ್ಯಾನಿಸು CLONE_STRING(var)                    \
  ಮಾಡು {                                       \
    ಆದರೆ(ಮೂಲ->var) {                        \
      dest->var = curlx_strdup(ಮೂಲ->var); \
      ಆದರೆ(!dest->var)                         \
        ಹಿಂತಿರುಗಿ FALSE;                        \
    }                                        \
    ಇಲ್ಲದಿದ್ದರೆ                                     \
      dest->var = ಖಾಲಿ;                      \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CLONE_BLOB(var)                  \
  ಮಾಡು {                                   \
    ಆದರೆ(blobdup(&dest->var, ಮೂಲ->var)) \
      ಹಿಂತಿರುಗಿ FALSE;                      \
  } ಆಗ(0)

ಸ್ಥಿರ CURLcode blobdup(ರಚನೆ curl_blob **dest, ರಚನೆ curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  ಆದರೆ(src) {
    /* only if there is data to dupe! */
    ರಚನೆ curl_blob *d;
    d = curlx_malloc(ಗಾತ್ರ(ರಚನೆ curl_blob) + src->len);
    ಆದರೆ(!d)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->ದತ್ತಾಂಶ = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)d + ಗಾತ್ರ(ರಚನೆ curl_blob));
    ಸ್ಮರಣೆ_ನಕಲು(d->ದತ್ತಾಂಶ, src->ದತ್ತಾಂಶ, src->len);
    *dest = d;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* returns TRUE if the blobs are identical */
ಸ್ಥಿರ bool blobcmp(ರಚನೆ curl_blob *ಮೊದಲ, ರಚನೆ curl_blob *ಎರಡನೇ)
{
  ಆದರೆ(!ಮೊದಲ && !ಎರಡನೇ) /* both are NULL */
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(!ಮೊದಲ || !ಎರಡನೇ) /* one is NULL */
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(ಮೊದಲ->len != ಎರಡನೇ->len) /* different sizes */
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ !memcmp(ಮೊದಲ->ದತ್ತಾಂಶ, ಎರಡನೇ->ದತ್ತಾಂಶ, ಮೊದಲ->len); /* same data */
}

ಶೂನ್ಯ Curl_ssl_config_init(ರಚನೆ ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

ಶೂನ್ಯ Curl_ssl_config_cleanup(ರಚನೆ ssl_filter_config *sslc)
{
  ಆದರೆ(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_ಕೀಲಿ);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->ಕೀಲಿ_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->ಕೀಲಿ);
    curlx_safefree(sslc->ಕೀಲಿ_type);
    curlx_safefree(sslc->ಕೀಲಿ_passwd);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_ಬಹಿರಂಗ);
#ಆದರೆ_ಅಂತ್ಯ
    sslc->deep_copy = FALSE;
  }
}

ಸ್ಥಿರ bool ಹೊಂದಿಕೆ_ssl_primary_config(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssl_filter_config *c1,
                                     ರಚನೆ ssl_filter_config *c2)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ((c1->ಆವೃತ್ತಿ_2 == c2->ಆವೃತ್ತಿ_2) &&
     (c1->ಆವೃತ್ತಿ_max == c2->ಆವೃತ್ತಿ_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_ಬಹಿರಂಗ, c2->ech_ಬಹಿರಂಗ) &&
#ಆದರೆ_ಅಂತ್ಯ
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->ಕೀಲಿ_blob, c2->ಕೀಲಿ_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_ಕೀಲಿ, c2->pinned_ಕೀಲಿ) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->ಕೀಲಿ, c2->ಕೀಲಿ) &&
     curl_strequal(c1->ಕೀಲಿ_type, c2->ಕೀಲಿ_type) &&
     !Curl_timestrcmp(c1->ಕೀಲಿ_passwd, c2->ಕೀಲಿ_passwd))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_ssl_conn_config_ಹೊಂದಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssl_filter_config *conn_config,
                                ರಚನೆ connectdata *candidate,
                                bool proxy)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(proxy)
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_ssl_primary_config(ದತ್ತಾಂಶ, conn_config,
                                    &candidate->proxy_ssl_config);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)proxy;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_ssl_primary_config(ದತ್ತಾಂಶ, conn_config,
                                  &candidate->ssl_config);
}

ಸ್ಥಿರ bool clone_ssl_primary_config(ರಚನೆ ssl_filter_config *ಮೂಲ,
                                     ರಚನೆ ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->ಆವೃತ್ತಿ_2 = ಮೂಲ->ಆವೃತ್ತಿ_2;
  dest->ಆವೃತ್ತಿ_max = ಮೂಲ->ಆವೃತ್ತಿ_max;
  dest->verifypeer = ಮೂಲ->verifypeer;
  dest->verifyhost = ಮೂಲ->verifyhost;
  dest->verifystatus = ಮೂಲ->verifystatus;
  dest->ssl_options = ಮೂಲ->ssl_options;
  dest->native_ca_store = ಮೂಲ->native_ca_store;
  dest->cache_session = ಮೂಲ->cache_session;
  dest->auto_client_cert = ಮೂಲ->auto_client_cert;
  dest->earlydata = ಮೂಲ->earlydata;
  dest->enable_beast = ಮೂಲ->enable_beast;
  dest->no_partialchain = ಮೂಲ->no_partialchain;
  dest->no_revoke = ಮೂಲ->no_revoke;
  dest->revoke_ಉತ್ತಮ_effort = ಮೂಲ->revoke_ಉತ್ತಮ_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_ಕೀಲಿ);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(ಕೀಲಿ);
  CLONE_STRING(ಕೀಲಿ_type);
  CLONE_STRING(ಕೀಲಿ_passwd);
  CLONE_BLOB(ಕೀಲಿ_blob);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  dest->ech = ಮೂಲ->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_ಬಹಿರಂಗ);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ssl_easy_config_compl_options(ರಚನೆ Curl_peer *origin,
                              ರಚನೆ Curl_peer *initial_origin,
                              ರಚನೆ ssl_easy_config *sslc,
                              ರಚನೆ ssl_filter_config *primary)
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
  primary->revoke_ಉತ್ತಮ_effort = !!(options & CURLSSLOPT_REVOKE_ಉತ್ತಮ_EFFORT);
}

ಸ್ಥಿರ ಅಕ್ಷರ *ssl_easy_steal(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಎಣಿಕೆ dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  ಹಿಂತಿರುಗಿ CURL_UNCONST(CURL_EASY_STR(ದತ್ತಾಂಶ, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_peer *origin,
  ರಚನೆ ssl_filter_config *ssl_origin,
  ರಚನೆ ssl_filter_config *ssl_proxy)
{
  ರಚನೆ ssl_easy_config *sslc = &ದತ್ತಾಂಶ->ಗಣ.ssl;
#ಆದರೆ defined(CURL_CA_ಮಾರ್ಗ) || defined(CURL_CA_BUNDLE)
  CURLcode ಫಲಿತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

  ssl_easy_config_compl_options(origin, ದತ್ತಾಂಶ->state.initial_origin, sslc,
                                ssl_origin);

  ಆದರೆ(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#ಆದರೆ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ಆದರೆ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_ಮಾರ್ಗ
    ಆದರೆ(!sslc->custom_capath && !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAPATH)) {
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAPATH, CURL_CA_ಮಾರ್ಗ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_BUNDLE
    ಆದರೆ(!sslc->custom_cafile && !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAFILE)) {
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ssl_origin->ಆವೃತ್ತಿ_2 = sslc->ಆವೃತ್ತಿ_2;
  ssl_origin->ಆವೃತ್ತಿ_max = sslc->ಆವೃತ್ತಿ_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_SSL_ISSUERCERT];

  ಆದರೆ(Curl_peer_equal(ದತ್ತಾಂಶ->state.initial_origin, origin)) {
    ssl_origin->pinned_ಕೀಲಿ =
      ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_CERT_TYPE);
    ssl_origin->ಕೀಲಿ = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ);
    ssl_origin->ಕೀಲಿ_type = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_TYPE);
    ssl_origin->ಕೀಲಿ_passwd = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_CERT);
    ssl_origin->ಕೀಲಿ_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_ಕೀಲಿ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
    ssl_origin->ech = ದತ್ತಾಂಶ->ಗಣ.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ECH_CONFIG);
    ssl_origin->ech_ಬಹಿರಂಗ = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ECH_ಬಹಿರಂಗ);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ssl_origin->pinned_ಕೀಲಿ = ಖಾಲಿ;
    ssl_origin->cert_blob = ಖಾಲಿ;
    ssl_origin->cert_type = ಖಾಲಿ;
    ssl_origin->ಕೀಲಿ = ಖಾಲಿ;
    ssl_origin->ಕೀಲಿ_type = ಖಾಲಿ;
    ssl_origin->ಕೀಲಿ_passwd = ಖಾಲಿ;
    ssl_origin->clientcert = ಖಾಲಿ;
    ssl_origin->ಕೀಲಿ_blob = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = ಖಾಲಿ;
    ssl_origin->ech_ಬಹಿರಂಗ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  sslc = &ದತ್ತಾಂಶ->ಗಣ.proxy_ssl;

  ssl_easy_config_compl_options(ಖಾಲಿ, ಖಾಲಿ, sslc, ssl_proxy);
  ಆದರೆ((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#ಆದರೆ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ಆದರೆ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_ಮಾರ್ಗ
    ಆದರೆ(!sslc->custom_capath &&
       !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAPATH_PROXY)) {
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAPATH_PROXY, CURL_CA_ಮಾರ್ಗ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_BUNDLE
    ಆದರೆ(!sslc->custom_cafile &&
       !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAFILE_PROXY)) {
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ssl_proxy->ಆವೃತ್ತಿ_2 = sslc->ಆವೃತ್ತಿ_2;
  ssl_proxy->ಆವೃತ್ತಿ_max = sslc->ಆವೃತ್ತಿ_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_ಕೀಲಿ =
    ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_CERT_TYPE_PROXY);
  ssl_proxy->ಕೀಲಿ = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PROXY);
  ssl_proxy->ಕೀಲಿ_type = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_TYPE_PROXY);
  ssl_proxy->ಕೀಲಿ_passwd = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(ದತ್ತಾಂಶ, STRING_CERT_PROXY);
  ssl_proxy->ಕೀಲಿ_blob = ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_ಕೀಲಿ_PROXY];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = ಖಾಲಿ;
  ssl_proxy->ech_ಬಹಿರಂಗ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ssl_proxy;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(ರಚನೆ ssl_filter_config *ssl_config,
                                    ರಚನೆ ssl_filter_config *proxy_ssl_config,
                                    ರಚನೆ connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  ಆದರೆ(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)proxy_ssl_config;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_conn_config_cleanup(ರಚನೆ connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_ssl_conn_config_update(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  ಆದರೆ(ದತ್ತಾಂಶ->conn) {
    ರಚನೆ ssl_easy_config *src;
    ರಚನೆ ssl_filter_config *dest;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    src = for_proxy ? &ದತ್ತಾಂಶ->ಗಣ.proxy_ssl : &ದತ್ತಾಂಶ->ಗಣ.ssl;
    dest = for_proxy ? &ದತ್ತಾಂಶ->conn->proxy_ssl_config : &ದತ್ತಾಂಶ->conn->ssl_config;
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)for_proxy;
    src = &ದತ್ತಾಂಶ->ಗಣ.ssl;
    dest = &ದತ್ತಾಂಶ->conn->ssl_config;
#ಆದರೆ_ಅಂತ್ಯ
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
    dest->ech = src->ech;
#ಆದರೆ_ಅಂತ್ಯ
  }
}
