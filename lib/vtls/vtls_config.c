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

#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "setopt.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_config.h"


#define CLONE_STRING(var)                    \
  କର {                                       \
    ଯଦି(source->var) {                        \
      dest->var = curlx_strdup(source->var); \
      ଯଦି(!dest->var)                         \
        ଫେରନ୍ତୁ FALSE;                        \
    }                                        \
    ଅନ୍ୟଥା                                     \
      dest->var = ଖାଲି;                      \
  } ଯେତେବେଳେ(0)

#define CLONE_BLOB(var)                  \
  କର {                                   \
    ଯଦି(blobdup(&dest->var, source->var)) \
      ଫେରନ୍ତୁ FALSE;                      \
  } ଯେତେବେଳେ(0)

ସ୍ଥିର CURLcode blobdup(ସଂରଚନା curl_blob **dest, ସଂରଚନା curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  ଯଦି(src) {
    /* only if there is data to dupe! */
    ସଂରଚନା curl_blob *d;
    d = curlx_malloc(ଆକାର(ସଂରଚନା curl_blob) + src->len);
    ଯଦି(!d)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->data = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)d + ଆକାର(ସଂରଚନା curl_blob));
    memcpy(d->data, src->data, src->len);
    *dest = d;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* returns TRUE if the blobs are identical */
ସ୍ଥିର bool blobcmp(ସଂରଚନା curl_blob *first, ସଂରଚନା curl_blob *second)
{
  ଯଦି(!first && !second) /* both are NULL */
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(!first || !second) /* one is NULL */
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(first->len != second->len) /* different sizes */
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ !memcmp(first->data, second->data, first->len); /* same data */
}

ଶୂନ୍ୟ Curl_ssl_config_init(ସଂରଚନା ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

ଶୂନ୍ୟ Curl_ssl_config_cleanup(ସଂରଚନା ssl_filter_config *sslc)
{
  ଯଦି(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_ଚାବି);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->ଚାବି_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->ଚାବି);
    curlx_safefree(sslc->ଚାବି_type);
    curlx_safefree(sslc->ଚାବି_passwd);
#ifdef USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_public);
#endif
    sslc->deep_copy = FALSE;
  }
}

ସ୍ଥିର bool match_ssl_primary_config(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssl_filter_config *c1,
                                     ସଂରଚନା ssl_filter_config *c2)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି((c1->version == c2->version) &&
     (c1->version_max == c2->version_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#ifdef USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_public, c2->ech_public) &&
#endif
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->ଚାବି_blob, c2->ଚାବି_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_ଚାବି, c2->pinned_ଚାବି) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->ଚାବି, c2->ଚାବି) &&
     curl_strequal(c1->ଚାବି_type, c2->ଚାବି_type) &&
     !Curl_timestrcmp(c1->ଚାବି_passwd, c2->ଚାବି_passwd))
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

bool Curl_ssl_conn_config_match(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssl_filter_config *conn_config,
                                ସଂରଚନା connectdata *candidate,
                                bool proxy)
{
#ifndef CURL_DISABLE_PROXY
  ଯଦି(proxy)
    ଫେରନ୍ତୁ match_ssl_primary_config(data, conn_config,
                                    &candidate->proxy_ssl_config);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)proxy;
#endif
  ଫେରନ୍ତୁ match_ssl_primary_config(data, conn_config,
                                  &candidate->ssl_config);
}

ସ୍ଥିର bool clone_ssl_primary_config(ସଂରଚନା ssl_filter_config *source,
                                     ସଂରଚନା ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->version = source->version;
  dest->version_max = source->version_max;
  dest->verifypeer = source->verifypeer;
  dest->verifyhost = source->verifyhost;
  dest->verifystatus = source->verifystatus;
  dest->ssl_options = source->ssl_options;
  dest->native_ca_store = source->native_ca_store;
  dest->cache_session = source->cache_session;
  dest->auto_client_cert = source->auto_client_cert;
  dest->earlydata = source->earlydata;
  dest->enable_beast = source->enable_beast;
  dest->no_partialchain = source->no_partialchain;
  dest->no_revoke = source->no_revoke;
  dest->revoke_best_effort = source->revoke_best_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_ଚାବି);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(ଚାବି);
  CLONE_STRING(ଚାବି_type);
  CLONE_STRING(ଚାବି_passwd);
  CLONE_BLOB(ଚାବି_blob);
#ifdef USE_ECH
  dest->ech = source->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_public);
#endif
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ଶୂନ୍ୟ
ssl_easy_config_compl_options(ସଂରଚନା Curl_peer *origin,
                              ସଂରଚନା Curl_peer *initial_origin,
                              ସଂରଚନା ssl_easy_config *sslc,
                              ସଂରଚନା ssl_filter_config *primary)
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
  primary->revoke_best_effort = !!(options & CURLSSLOPT_REVOKE_BEST_EFFORT);
}

ସ୍ଥିର ଅକ୍ଷର *ssl_easy_steal(ସଂରଚନା Curl_easy *data, ଗଣନା dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  ଫେରନ୍ତୁ CURL_UNCONST(CURL_EASY_STR(data, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_peer *origin,
  ସଂରଚନା ssl_filter_config *ssl_origin,
  ସଂରଚନା ssl_filter_config *ssl_proxy)
{
  ସଂରଚନା ssl_easy_config *sslc = &data->set.ssl;
#ଯଦି defined(CURL_CA_PATH) || defined(CURL_CA_BUNDLE)
  CURLcode result;
#endif

  ssl_easy_config_compl_options(origin, data->state.initial_origin, sslc,
                                ssl_origin);

  ଯଦି(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#ଯଦି defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ଯଦି(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#endif
#ifdef CURL_CA_PATH
    ଯଦି(!sslc->custom_capath && !CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      result = Curl_setstropt(data, STRING_SSL_CAPATH, CURL_CA_PATH);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
#endif
#ifdef CURL_CA_BUNDLE
    ଯଦି(!sslc->custom_cafile && !CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      result = Curl_setstropt(data, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
#endif
  }

  ssl_origin->version = sslc->version;
  ssl_origin->version_max = sslc->version_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(data, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(data, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(data, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(data, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(data, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(data, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = data->set.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(data, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(data, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = data->set.blobs[BLOB_SSL_ISSUERCERT];

  ଯଦି(Curl_peer_equal(data->state.initial_origin, origin)) {
    ssl_origin->pinned_ଚାବି =
      ssl_easy_steal(data, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = data->set.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(data, STRING_CERT_TYPE);
    ssl_origin->ଚାବି = ssl_easy_steal(data, STRING_ଚାବି);
    ssl_origin->ଚାବି_type = ssl_easy_steal(data, STRING_ଚାବି_TYPE);
    ssl_origin->ଚାବି_passwd = ssl_easy_steal(data, STRING_ଚାବି_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(data, STRING_CERT);
    ssl_origin->ଚାବି_blob = data->set.blobs[BLOB_ଚାବି];
#ifdef USE_ECH
    ssl_origin->ech = data->set.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(data, STRING_ECH_CONFIG);
    ssl_origin->ech_public = ssl_easy_steal(data, STRING_ECH_PUBLIC);
#endif
  }
  ଅନ୍ୟଥା {
    ssl_origin->pinned_ଚାବି = ଖାଲି;
    ssl_origin->cert_blob = ଖାଲି;
    ssl_origin->cert_type = ଖାଲି;
    ssl_origin->ଚାବି = ଖାଲି;
    ssl_origin->ଚାବି_type = ଖାଲି;
    ssl_origin->ଚାବି_passwd = ଖାଲି;
    ssl_origin->clientcert = ଖାଲି;
    ssl_origin->ଚାବି_blob = ଖାଲି;
#ifdef USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = ଖାଲି;
    ssl_origin->ech_public = ଖାଲି;
#endif
  }

#ifndef CURL_DISABLE_PROXY
  sslc = &data->set.proxy_ssl;

  ssl_easy_config_compl_options(ଖାଲି, ଖାଲି, sslc, ssl_proxy);
  ଯଦି((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#ଯଦି defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ଯଦି(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#endif
#ifdef CURL_CA_PATH
    ଯଦି(!sslc->custom_capath &&
       !CURL_EASY_STR(data, STRING_SSL_CAPATH_PROXY)) {
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, CURL_CA_PATH);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
#endif
#ifdef CURL_CA_BUNDLE
    ଯଦି(!sslc->custom_cafile &&
       !CURL_EASY_STR(data, STRING_SSL_CAFILE_PROXY)) {
      result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
#endif
  }

  ssl_proxy->version = sslc->version;
  ssl_proxy->version_max = sslc->version_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(data, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(data, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(data, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(data, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_ଚାବି =
    ssl_easy_steal(data, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = data->set.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = data->set.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(data, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = data->set.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(data, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(data, STRING_CERT_TYPE_PROXY);
  ssl_proxy->ଚାବି = ssl_easy_steal(data, STRING_ଚାବି_PROXY);
  ssl_proxy->ଚାବି_type = ssl_easy_steal(data, STRING_ଚାବି_TYPE_PROXY);
  ssl_proxy->ଚାବି_passwd = ssl_easy_steal(data, STRING_ଚାବି_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(data, STRING_CERT_PROXY);
  ssl_proxy->ଚାବି_blob = data->set.blobs[BLOB_ଚାବି_PROXY];
#ifdef USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = ଖାଲି;
  ssl_proxy->ech_public = ଖାଲି;
#endif
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)ssl_proxy;
#endif /* CURL_DISABLE_PROXY */

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(ସଂରଚନା ssl_filter_config *ssl_config,
                                    ସଂରଚନା ssl_filter_config *proxy_ssl_config,
                                    ସଂରଚନା connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  ଯଦି(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
#ifndef CURL_DISABLE_PROXY
  ଯଦି(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)proxy_ssl_config;
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssl_conn_config_cleanup(ସଂରଚନା connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#ifndef CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#endif
}

ଶୂନ୍ୟ Curl_ssl_conn_config_update(ସଂରଚନା Curl_easy *data, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  ଯଦି(data->conn) {
    ସଂରଚନା ssl_easy_config *src;
    ସଂରଚନା ssl_filter_config *dest;
#ifndef CURL_DISABLE_PROXY
    src = for_proxy ? &data->set.proxy_ssl : &data->set.ssl;
    dest = for_proxy ? &data->conn->proxy_ssl_config : &data->conn->ssl_config;
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)for_proxy;
    src = &data->set.ssl;
    dest = &data->conn->ssl_config;
#endif
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#ifdef USE_ECH
    dest->ech = src->ech;
#endif
  }
}
