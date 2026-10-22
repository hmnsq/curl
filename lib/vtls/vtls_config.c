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

#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "setopt.h"
#समावेश "strcase.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_config.h"


#घोषणा CLONE_STRING(var)                    \
  करो {                                       \
    अगर(source->var) {                        \
      dest->var = curlx_strdup(source->var); \
      अगर(!dest->var)                         \
        वापस FALSE;                        \
    }                                        \
    अन्यथा                                     \
      dest->var = शून्य;                      \
  } जबतक(0)

#घोषणा CLONE_BLOB(var)                  \
  करो {                                   \
    अगर(blobdup(&dest->var, source->var)) \
      वापस FALSE;                      \
  } जबतक(0)

अटल CURLcode blobdup(काष्ठा curl_blob **dest, काष्ठा curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  अगर(src) {
    /* only if there is data to dupe! */
    काष्ठा curl_blob *d;
    d = curlx_malloc(माप(काष्ठा curl_blob) + src->len);
    अगर(!d)
      वापस CURLE_OUT_OF_MEMORY;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->तथ्य = (व्योम *)((अक्षर *)d + माप(काष्ठा curl_blob));
    स_नकल(d->तथ्य, src->तथ्य, src->len);
    *dest = d;
  }
  वापस CURLE_OK;
}

/* returns TRUE if the blobs are identical */
अटल bool blobcmp(काष्ठा curl_blob *first, काष्ठा curl_blob *second)
{
  अगर(!first && !second) /* both are NULL */
    वापस TRUE;
  अगर(!first || !second) /* one is NULL */
    वापस FALSE;
  अगर(first->len != second->len) /* different sizes */
    वापस FALSE;
  वापस !स_भेद(first->तथ्य, second->तथ्य, first->len); /* same data */
}

व्योम Curl_ssl_config_init(काष्ठा ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

व्योम Curl_ssl_config_cleanup(काष्ठा ssl_filter_config *sslc)
{
  अगर(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_कुंजी);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->कुंजी_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->कुंजी);
    curlx_safefree(sslc->कुंजी_type);
    curlx_safefree(sslc->कुंजी_passwd);
#अगर_घोषित USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_public);
#पूर्ण_अगर
    sslc->deep_copy = FALSE;
  }
}

अटल bool match_ssl_primary_config(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssl_filter_config *c1,
                                     काष्ठा ssl_filter_config *c2)
{
  (व्योम)तथ्य;
  अगर((c1->version == c2->version) &&
     (c1->version_max == c2->version_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#अगर_घोषित USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_public, c2->ech_public) &&
#पूर्ण_अगर
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->कुंजी_blob, c2->कुंजी_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_कुंजी, c2->pinned_कुंजी) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->कुंजी, c2->कुंजी) &&
     curl_strequal(c1->कुंजी_type, c2->कुंजी_type) &&
     !Curl_timestrcmp(c1->कुंजी_passwd, c2->कुंजी_passwd))
    वापस TRUE;

  वापस FALSE;
}

bool Curl_ssl_conn_config_match(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssl_filter_config *conn_config,
                                काष्ठा connectdata *candidate,
                                bool proxy)
{
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(proxy)
    वापस match_ssl_primary_config(तथ्य, conn_config,
                                    &candidate->proxy_ssl_config);
#अन्यथा
  (व्योम)proxy;
#पूर्ण_अगर
  वापस match_ssl_primary_config(तथ्य, conn_config,
                                  &candidate->ssl_config);
}

अटल bool clone_ssl_primary_config(काष्ठा ssl_filter_config *source,
                                     काष्ठा ssl_filter_config *dest)
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
  CLONE_STRING(pinned_कुंजी);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(कुंजी);
  CLONE_STRING(कुंजी_type);
  CLONE_STRING(कुंजी_passwd);
  CLONE_BLOB(कुंजी_blob);
#अगर_घोषित USE_ECH
  dest->ech = source->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_public);
#पूर्ण_अगर
  वापस TRUE;
}

अटल व्योम
ssl_easy_config_compl_options(काष्ठा Curl_peer *origin,
                              काष्ठा Curl_peer *initial_origin,
                              काष्ठा ssl_easy_config *sslc,
                              काष्ठा ssl_filter_config *primary)
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

अटल अक्षर *ssl_easy_steal(काष्ठा Curl_easy *तथ्य, क्रमागत dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  वापस CURL_UNCONST(CURL_EASY_STR(तथ्य, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  काष्ठा Curl_easy *तथ्य, काष्ठा Curl_peer *origin,
  काष्ठा ssl_filter_config *ssl_origin,
  काष्ठा ssl_filter_config *ssl_proxy)
{
  काष्ठा ssl_easy_config *sslc = &तथ्य->set.ssl;
#अगर defined(CURL_CA_PATH) || defined(CURL_CA_BUNDLE)
  CURLcode परिणाम;
#पूर्ण_अगर

  ssl_easy_config_compl_options(origin, तथ्य->state.initial_origin, sslc,
                                ssl_origin);

  अगर(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#अगर defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    अगर(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#पूर्ण_अगर
#अगर_घोषित CURL_CA_PATH
    अगर(!sslc->custom_capath && !CURL_EASY_STR(तथ्य, STRING_SSL_CAPATH)) {
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAPATH, CURL_CA_PATH);
      अगर(परिणाम)
        वापस परिणाम;
    }
#पूर्ण_अगर
#अगर_घोषित CURL_CA_BUNDLE
    अगर(!sslc->custom_cafile && !CURL_EASY_STR(तथ्य, STRING_SSL_CAFILE)) {
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      अगर(परिणाम)
        वापस परिणाम;
    }
#पूर्ण_अगर
  }

  ssl_origin->version = sslc->version;
  ssl_origin->version_max = sslc->version_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(तथ्य, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(तथ्य, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(तथ्य, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(तथ्य, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(तथ्य, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(तथ्य, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = तथ्य->set.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(तथ्य, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(तथ्य, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = तथ्य->set.blobs[BLOB_SSL_ISSUERCERT];

  अगर(Curl_peer_equal(तथ्य->state.initial_origin, origin)) {
    ssl_origin->pinned_कुंजी =
      ssl_easy_steal(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = तथ्य->set.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(तथ्य, STRING_CERT_TYPE);
    ssl_origin->कुंजी = ssl_easy_steal(तथ्य, STRING_कुंजी);
    ssl_origin->कुंजी_type = ssl_easy_steal(तथ्य, STRING_कुंजी_TYPE);
    ssl_origin->कुंजी_passwd = ssl_easy_steal(तथ्य, STRING_कुंजी_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(तथ्य, STRING_CERT);
    ssl_origin->कुंजी_blob = तथ्य->set.blobs[BLOB_कुंजी];
#अगर_घोषित USE_ECH
    ssl_origin->ech = तथ्य->set.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(तथ्य, STRING_ECH_CONFIG);
    ssl_origin->ech_public = ssl_easy_steal(तथ्य, STRING_ECH_PUBLIC);
#पूर्ण_अगर
  }
  अन्यथा {
    ssl_origin->pinned_कुंजी = शून्य;
    ssl_origin->cert_blob = शून्य;
    ssl_origin->cert_type = शून्य;
    ssl_origin->कुंजी = शून्य;
    ssl_origin->कुंजी_type = शून्य;
    ssl_origin->कुंजी_passwd = शून्य;
    ssl_origin->clientcert = शून्य;
    ssl_origin->कुंजी_blob = शून्य;
#अगर_घोषित USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = शून्य;
    ssl_origin->ech_public = शून्य;
#पूर्ण_अगर
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  sslc = &तथ्य->set.proxy_ssl;

  ssl_easy_config_compl_options(शून्य, शून्य, sslc, ssl_proxy);
  अगर((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#अगर defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    अगर(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#पूर्ण_अगर
#अगर_घोषित CURL_CA_PATH
    अगर(!sslc->custom_capath &&
       !CURL_EASY_STR(तथ्य, STRING_SSL_CAPATH_PROXY)) {
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAPATH_PROXY, CURL_CA_PATH);
      अगर(परिणाम)
        वापस परिणाम;
    }
#पूर्ण_अगर
#अगर_घोषित CURL_CA_BUNDLE
    अगर(!sslc->custom_cafile &&
       !CURL_EASY_STR(तथ्य, STRING_SSL_CAFILE_PROXY)) {
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      अगर(परिणाम)
        वापस परिणाम;
    }
#पूर्ण_अगर
  }

  ssl_proxy->version = sslc->version;
  ssl_proxy->version_max = sslc->version_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(तथ्य, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(तथ्य, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(तथ्य, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(तथ्य, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_कुंजी =
    ssl_easy_steal(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = तथ्य->set.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = तथ्य->set.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(तथ्य, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = तथ्य->set.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(तथ्य, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(तथ्य, STRING_CERT_TYPE_PROXY);
  ssl_proxy->कुंजी = ssl_easy_steal(तथ्य, STRING_कुंजी_PROXY);
  ssl_proxy->कुंजी_type = ssl_easy_steal(तथ्य, STRING_कुंजी_TYPE_PROXY);
  ssl_proxy->कुंजी_passwd = ssl_easy_steal(तथ्य, STRING_कुंजी_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(तथ्य, STRING_CERT_PROXY);
  ssl_proxy->कुंजी_blob = तथ्य->set.blobs[BLOB_कुंजी_PROXY];
#अगर_घोषित USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = शून्य;
  ssl_proxy->ech_public = शून्य;
#पूर्ण_अगर
#अन्यथा
  (व्योम)ssl_proxy;
#पूर्ण_अगर /* CURL_DISABLE_PROXY */

  वापस CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(काष्ठा ssl_filter_config *ssl_config,
                                    काष्ठा ssl_filter_config *proxy_ssl_config,
                                    काष्ठा connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  अगर(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    वापस CURLE_OUT_OF_MEMORY;
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    वापस CURLE_OUT_OF_MEMORY;
#अन्यथा
  (व्योम)proxy_ssl_config;
#पूर्ण_अगर
  वापस CURLE_OK;
}

व्योम Curl_ssl_conn_config_cleanup(काष्ठा connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#अगर_अघोषित CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#पूर्ण_अगर
}

व्योम Curl_ssl_conn_config_update(काष्ठा Curl_easy *तथ्य, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  अगर(तथ्य->conn) {
    काष्ठा ssl_easy_config *src;
    काष्ठा ssl_filter_config *dest;
#अगर_अघोषित CURL_DISABLE_PROXY
    src = for_proxy ? &तथ्य->set.proxy_ssl : &तथ्य->set.ssl;
    dest = for_proxy ? &तथ्य->conn->proxy_ssl_config : &तथ्य->conn->ssl_config;
#अन्यथा
    (व्योम)for_proxy;
    src = &तथ्य->set.ssl;
    dest = &तथ्य->conn->ssl_config;
#पूर्ण_अगर
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#अगर_घोषित USE_ECH
    dest->ech = src->ech;
#पूर्ण_अगर
  }
}
