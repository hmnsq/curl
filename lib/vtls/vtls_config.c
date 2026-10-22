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

#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_config.h"


#నిర్వచించు CLONE_STRING(var)                    \
  చేయి {                                       \
    ఒకవేళ(ఆధారం->var) {                        \
      dest->var = curlx_strdup(ఆధారం->var); \
      ఒకవేళ(!dest->var)                         \
        తిరిగి FALSE;                        \
    }                                        \
    లేకపోతే                                     \
      dest->var = ఖాళీ;                      \
  } అయితే(0)

#నిర్వచించు CLONE_BLOB(var)                  \
  చేయి {                                   \
    ఒకవేళ(blobdup(&dest->var, ఆధారం->var)) \
      తిరిగి FALSE;                      \
  } అయితే(0)

స్థిర CURLcode blobdup(నిర్మాణం curl_blob **dest, నిర్మాణం curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  ఒకవేళ(src) {
    /* only if there is data to dupe! */
    నిర్మాణం curl_blob *d;
    d = curlx_malloc(పరిమాణం(నిర్మాణం curl_blob) + src->len);
    ఒకవేళ(!d)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->దత్తాంశం = (శూన్య *)((అక్షరం *)d + పరిమాణం(నిర్మాణం curl_blob));
    జ్ఞాపకం_నకలు(d->దత్తాంశం, src->దత్తాంశం, src->len);
    *dest = d;
  }
  తిరిగి CURLE_OK;
}

/* returns TRUE if the blobs are identical */
స్థిర bool blobcmp(నిర్మాణం curl_blob *మొదటి, నిర్మాణం curl_blob *రెండవ)
{
  ఒకవేళ(!మొదటి && !రెండవ) /* both are NULL */
    తిరిగి TRUE;
  ఒకవేళ(!మొదటి || !రెండవ) /* one is NULL */
    తిరిగి FALSE;
  ఒకవేళ(మొదటి->len != రెండవ->len) /* different sizes */
    తిరిగి FALSE;
  తిరిగి !memcmp(మొదటి->దత్తాంశం, రెండవ->దత్తాంశం, మొదటి->len); /* same data */
}

శూన్య Curl_ssl_config_init(నిర్మాణం ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

శూన్య Curl_ssl_config_cleanup(నిర్మాణం ssl_filter_config *sslc)
{
  ఒకవేళ(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_కీలకం);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->కీలకం_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->కీలకం);
    curlx_safefree(sslc->కీలకం_type);
    curlx_safefree(sslc->కీలకం_passwd);
#నిర్వచితమైతే USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_ప్రజా);
#అయితే_ముగింపు
    sslc->deep_copy = FALSE;
  }
}

స్థిర bool సరిపోలిక_ssl_primary_config(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssl_filter_config *c1,
                                     నిర్మాణం ssl_filter_config *c2)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ((c1->సంస్కరణ_2 == c2->సంస్కరణ_2) &&
     (c1->సంస్కరణ_max == c2->సంస్కరణ_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#నిర్వచితమైతే USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_ప్రజా, c2->ech_ప్రజా) &&
#అయితే_ముగింపు
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->కీలకం_blob, c2->కీలకం_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_కీలకం, c2->pinned_కీలకం) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->కీలకం, c2->కీలకం) &&
     curl_strequal(c1->కీలకం_type, c2->కీలకం_type) &&
     !Curl_timestrcmp(c1->కీలకం_passwd, c2->కీలకం_passwd))
    తిరిగి TRUE;

  తిరిగి FALSE;
}

bool Curl_ssl_conn_config_సరిపోలిక(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssl_filter_config *conn_config,
                                నిర్మాణం connectdata *candidate,
                                bool proxy)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(proxy)
    తిరిగి సరిపోలిక_ssl_primary_config(దత్తాంశం, conn_config,
                                    &candidate->proxy_ssl_config);
#లేకపోతే
  (శూన్య)proxy;
#అయితే_ముగింపు
  తిరిగి సరిపోలిక_ssl_primary_config(దత్తాంశం, conn_config,
                                  &candidate->ssl_config);
}

స్థిర bool clone_ssl_primary_config(నిర్మాణం ssl_filter_config *ఆధారం,
                                     నిర్మాణం ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->సంస్కరణ_2 = ఆధారం->సంస్కరణ_2;
  dest->సంస్కరణ_max = ఆధారం->సంస్కరణ_max;
  dest->verifypeer = ఆధారం->verifypeer;
  dest->verifyhost = ఆధారం->verifyhost;
  dest->verifystatus = ఆధారం->verifystatus;
  dest->ssl_options = ఆధారం->ssl_options;
  dest->native_ca_store = ఆధారం->native_ca_store;
  dest->cache_session = ఆధారం->cache_session;
  dest->auto_client_cert = ఆధారం->auto_client_cert;
  dest->earlydata = ఆధారం->earlydata;
  dest->enable_beast = ఆధారం->enable_beast;
  dest->no_partialchain = ఆధారం->no_partialchain;
  dest->no_revoke = ఆధారం->no_revoke;
  dest->revoke_ఉత్తమ_effort = ఆధారం->revoke_ఉత్తమ_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_కీలకం);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(కీలకం);
  CLONE_STRING(కీలకం_type);
  CLONE_STRING(కీలకం_passwd);
  CLONE_BLOB(కీలకం_blob);
#నిర్వచితమైతే USE_ECH
  dest->ech = ఆధారం->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_ప్రజా);
#అయితే_ముగింపు
  తిరిగి TRUE;
}

స్థిర శూన్య
ssl_easy_config_compl_options(నిర్మాణం Curl_peer *origin,
                              నిర్మాణం Curl_peer *initial_origin,
                              నిర్మాణం ssl_easy_config *sslc,
                              నిర్మాణం ssl_filter_config *primary)
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
  primary->revoke_ఉత్తమ_effort = !!(options & CURLSSLOPT_REVOKE_ఉత్తమ_EFFORT);
}

స్థిర అక్షరం *ssl_easy_steal(నిర్మాణం Curl_easy *దత్తాంశం, గణన dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  తిరిగి CURL_UNCONST(CURL_EASY_STR(దత్తాంశం, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_peer *origin,
  నిర్మాణం ssl_filter_config *ssl_origin,
  నిర్మాణం ssl_filter_config *ssl_proxy)
{
  నిర్మాణం ssl_easy_config *sslc = &దత్తాంశం->సమితి.ssl;
#ఒకవేళ defined(CURL_CA_మార్గం) || defined(CURL_CA_BUNDLE)
  CURLcode ఫలితం;
#అయితే_ముగింపు

  ssl_easy_config_compl_options(origin, దత్తాంశం->state.initial_origin, sslc,
                                ssl_origin);

  ఒకవేళ(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#ఒకవేళ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ఒకవేళ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_CA_మార్గం
    ఒకవేళ(!sslc->custom_capath && !CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAPATH)) {
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAPATH, CURL_CA_మార్గం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_CA_BUNDLE
    ఒకవేళ(!sslc->custom_cafile && !CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAFILE)) {
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
  }

  ssl_origin->సంస్కరణ_2 = sslc->సంస్కరణ_2;
  ssl_origin->సంస్కరణ_max = sslc->సంస్కరణ_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(దత్తాంశం, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(దత్తాంశం, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(దత్తాంశం, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(దత్తాంశం, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(దత్తాంశం, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(దత్తాంశం, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = దత్తాంశం->సమితి.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(దత్తాంశం, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(దత్తాంశం, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = దత్తాంశం->సమితి.blobs[BLOB_SSL_ISSUERCERT];

  ఒకవేళ(Curl_peer_equal(దత్తాంశం->state.initial_origin, origin)) {
    ssl_origin->pinned_కీలకం =
      ssl_easy_steal(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = దత్తాంశం->సమితి.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(దత్తాంశం, STRING_CERT_TYPE);
    ssl_origin->కీలకం = ssl_easy_steal(దత్తాంశం, STRING_కీలకం);
    ssl_origin->కీలకం_type = ssl_easy_steal(దత్తాంశం, STRING_కీలకం_TYPE);
    ssl_origin->కీలకం_passwd = ssl_easy_steal(దత్తాంశం, STRING_కీలకం_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(దత్తాంశం, STRING_CERT);
    ssl_origin->కీలకం_blob = దత్తాంశం->సమితి.blobs[BLOB_కీలకం];
#నిర్వచితమైతే USE_ECH
    ssl_origin->ech = దత్తాంశం->సమితి.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(దత్తాంశం, STRING_ECH_CONFIG);
    ssl_origin->ech_ప్రజా = ssl_easy_steal(దత్తాంశం, STRING_ECH_ప్రజా);
#అయితే_ముగింపు
  }
  లేకపోతే {
    ssl_origin->pinned_కీలకం = ఖాళీ;
    ssl_origin->cert_blob = ఖాళీ;
    ssl_origin->cert_type = ఖాళీ;
    ssl_origin->కీలకం = ఖాళీ;
    ssl_origin->కీలకం_type = ఖాళీ;
    ssl_origin->కీలకం_passwd = ఖాళీ;
    ssl_origin->clientcert = ఖాళీ;
    ssl_origin->కీలకం_blob = ఖాళీ;
#నిర్వచితమైతే USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = ఖాళీ;
    ssl_origin->ech_ప్రజా = ఖాళీ;
#అయితే_ముగింపు
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  sslc = &దత్తాంశం->సమితి.proxy_ssl;

  ssl_easy_config_compl_options(ఖాళీ, ఖాళీ, sslc, ssl_proxy);
  ఒకవేళ((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#ఒకవేళ defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    ఒకవేళ(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_CA_మార్గం
    ఒకవేళ(!sslc->custom_capath &&
       !CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAPATH_PROXY)) {
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAPATH_PROXY, CURL_CA_మార్గం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_CA_BUNDLE
    ఒకవేళ(!sslc->custom_cafile &&
       !CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAFILE_PROXY)) {
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
  }

  ssl_proxy->సంస్కరణ_2 = sslc->సంస్కరణ_2;
  ssl_proxy->సంస్కరణ_max = sslc->సంస్కరణ_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(దత్తాంశం, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(దత్తాంశం, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(దత్తాంశం, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(దత్తాంశం, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_కీలకం =
    ssl_easy_steal(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = దత్తాంశం->సమితి.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = దత్తాంశం->సమితి.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(దత్తాంశం, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = దత్తాంశం->సమితి.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(దత్తాంశం, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(దత్తాంశం, STRING_CERT_TYPE_PROXY);
  ssl_proxy->కీలకం = ssl_easy_steal(దత్తాంశం, STRING_కీలకం_PROXY);
  ssl_proxy->కీలకం_type = ssl_easy_steal(దత్తాంశం, STRING_కీలకం_TYPE_PROXY);
  ssl_proxy->కీలకం_passwd = ssl_easy_steal(దత్తాంశం, STRING_కీలకం_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(దత్తాంశం, STRING_CERT_PROXY);
  ssl_proxy->కీలకం_blob = దత్తాంశం->సమితి.blobs[BLOB_కీలకం_PROXY];
#నిర్వచితమైతే USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = ఖాళీ;
  ssl_proxy->ech_ప్రజా = ఖాళీ;
#అయితే_ముగింపు
#లేకపోతే
  (శూన్య)ssl_proxy;
#అయితే_ముగింపు /* CURL_DISABLE_PROXY */

  తిరిగి CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(నిర్మాణం ssl_filter_config *ssl_config,
                                    నిర్మాణం ssl_filter_config *proxy_ssl_config,
                                    నిర్మాణం connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  ఒకవేళ(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
#లేకపోతే
  (శూన్య)proxy_ssl_config;
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssl_conn_config_cleanup(నిర్మాణం connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#అయితే_ముగింపు
}

శూన్య Curl_ssl_conn_config_update(నిర్మాణం Curl_easy *దత్తాంశం, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  ఒకవేళ(దత్తాంశం->conn) {
    నిర్మాణం ssl_easy_config *src;
    నిర్మాణం ssl_filter_config *dest;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    src = for_proxy ? &దత్తాంశం->సమితి.proxy_ssl : &దత్తాంశం->సమితి.ssl;
    dest = for_proxy ? &దత్తాంశం->conn->proxy_ssl_config : &దత్తాంశం->conn->ssl_config;
#లేకపోతే
    (శూన్య)for_proxy;
    src = &దత్తాంశం->సమితి.ssl;
    dest = &దత్తాంశం->conn->ssl_config;
#అయితే_ముగింపు
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#నిర్వచితమైతే USE_ECH
    dest->ech = src->ech;
#అయితే_ముగింపు
  }
}
