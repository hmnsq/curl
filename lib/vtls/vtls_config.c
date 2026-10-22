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

#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "setopt.h"
#ضمن "strcase.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_config.h"


#وصف_ڪريو CLONE_STRING(var)                    \
  افعل {                                       \
    إذا(ذريعو->var) {                        \
      dest->var = curlx_strdup(ذريعو->var); \
      إذا(!dest->var)                         \
        أرجع FALSE;                        \
    }                                        \
    وإلا                                     \
      dest->var = عدم;                      \
  } طالما(0)

#وصف_ڪريو CLONE_BLOB(var)                  \
  افعل {                                   \
    إذا(blobdup(&dest->var, ذريعو->var)) \
      أرجع FALSE;                      \
  } طالما(0)

ساكن CURLcode blobdup(بنية curl_blob **dest, بنية curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  إذا(src) {
    /* only if there is data to dupe! */
    بنية curl_blob *d;
    d = curlx_malloc(حجم(بنية curl_blob) + src->len);
    إذا(!d)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->معلومات = (فراغ *)((حرف *)d + حجم(بنية curl_blob));
    يادگيري_نقل(d->معلومات, src->معلومات, src->len);
    *dest = d;
  }
  أرجع CURLE_OK;
}

/* returns TRUE if the blobs are identical */
ساكن bool blobcmp(بنية curl_blob *پهريون, بنية curl_blob *ٻيو)
{
  إذا(!پهريون && !ٻيو) /* both are NULL */
    أرجع TRUE;
  إذا(!پهريون || !ٻيو) /* one is NULL */
    أرجع FALSE;
  إذا(پهريون->len != ٻيو->len) /* different sizes */
    أرجع FALSE;
  أرجع !memcmp(پهريون->معلومات, ٻيو->معلومات, پهريون->len); /* same data */
}

فراغ Curl_ssl_config_init(بنية ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

فراغ Curl_ssl_config_cleanup(بنية ssl_filter_config *sslc)
{
  إذا(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_ڪنجي);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->ڪنجي_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->ڪنجي);
    curlx_safefree(sslc->ڪنجي_type);
    curlx_safefree(sslc->ڪنجي_passwd);
#جيڪڏهن_وصف_ٿيل USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_عوامي);
#جيڪڏهن_پورو
    sslc->deep_copy = FALSE;
  }
}

ساكن bool ميلاپ_ssl_primary_config(بنية Curl_easy *معلومات,
                                     بنية ssl_filter_config *c1,
                                     بنية ssl_filter_config *c2)
{
  (فراغ)معلومات;
  إذا((c1->نسخو_2 == c2->نسخو_2) &&
     (c1->نسخو_max == c2->نسخو_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#جيڪڏهن_وصف_ٿيل USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_عوامي, c2->ech_عوامي) &&
#جيڪڏهن_پورو
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->ڪنجي_blob, c2->ڪنجي_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_ڪنجي, c2->pinned_ڪنجي) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->ڪنجي, c2->ڪنجي) &&
     curl_strequal(c1->ڪنجي_type, c2->ڪنجي_type) &&
     !Curl_timestrcmp(c1->ڪنجي_passwd, c2->ڪنجي_passwd))
    أرجع TRUE;

  أرجع FALSE;
}

bool Curl_ssl_conn_config_ميلاپ(بنية Curl_easy *معلومات,
                                بنية ssl_filter_config *conn_config,
                                بنية connectdata *candidate,
                                bool proxy)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(proxy)
    أرجع ميلاپ_ssl_primary_config(معلومات, conn_config,
                                    &candidate->proxy_ssl_config);
#وإلا
  (فراغ)proxy;
#جيڪڏهن_پورو
  أرجع ميلاپ_ssl_primary_config(معلومات, conn_config,
                                  &candidate->ssl_config);
}

ساكن bool clone_ssl_primary_config(بنية ssl_filter_config *ذريعو,
                                     بنية ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->نسخو_2 = ذريعو->نسخو_2;
  dest->نسخو_max = ذريعو->نسخو_max;
  dest->verifypeer = ذريعو->verifypeer;
  dest->verifyhost = ذريعو->verifyhost;
  dest->verifystatus = ذريعو->verifystatus;
  dest->ssl_options = ذريعو->ssl_options;
  dest->native_ca_store = ذريعو->native_ca_store;
  dest->cache_session = ذريعو->cache_session;
  dest->auto_client_cert = ذريعو->auto_client_cert;
  dest->earlydata = ذريعو->earlydata;
  dest->enable_beast = ذريعو->enable_beast;
  dest->no_partialchain = ذريعو->no_partialchain;
  dest->no_revoke = ذريعو->no_revoke;
  dest->revoke_بهترين_effort = ذريعو->revoke_بهترين_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_ڪنجي);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(ڪنجي);
  CLONE_STRING(ڪنجي_type);
  CLONE_STRING(ڪنجي_passwd);
  CLONE_BLOB(ڪنجي_blob);
#جيڪڏهن_وصف_ٿيل USE_ECH
  dest->ech = ذريعو->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_عوامي);
#جيڪڏهن_پورو
  أرجع TRUE;
}

ساكن فراغ
ssl_easy_config_compl_options(بنية Curl_peer *origin,
                              بنية Curl_peer *initial_origin,
                              بنية ssl_easy_config *sslc,
                              بنية ssl_filter_config *primary)
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
  primary->revoke_بهترين_effort = !!(options & CURLSSLOPT_REVOKE_بهترين_EFFORT);
}

ساكن حرف *ssl_easy_steal(بنية Curl_easy *معلومات, تعداد dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  أرجع CURL_UNCONST(CURL_EASY_STR(معلومات, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  بنية Curl_easy *معلومات, بنية Curl_peer *origin,
  بنية ssl_filter_config *ssl_origin,
  بنية ssl_filter_config *ssl_proxy)
{
  بنية ssl_easy_config *sslc = &معلومات->سيٽ.ssl;
#إذا defined(CURL_CA_رستو) || defined(CURL_CA_BUNDLE)
  CURLcode نتيجو;
#جيڪڏهن_پورو

  ssl_easy_config_compl_options(origin, معلومات->state.initial_origin, sslc,
                                ssl_origin);

  إذا(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#إذا defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    إذا(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_CA_رستو
    إذا(!sslc->custom_capath && !CURL_EASY_STR(معلومات, STRING_SSL_CAPATH)) {
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAPATH, CURL_CA_رستو);
      إذا(نتيجو)
        أرجع نتيجو;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_CA_BUNDLE
    إذا(!sslc->custom_cafile && !CURL_EASY_STR(معلومات, STRING_SSL_CAFILE)) {
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      إذا(نتيجو)
        أرجع نتيجو;
    }
#جيڪڏهن_پورو
  }

  ssl_origin->نسخو_2 = sslc->نسخو_2;
  ssl_origin->نسخو_max = sslc->نسخو_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(معلومات, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(معلومات, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(معلومات, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(معلومات, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(معلومات, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(معلومات, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = معلومات->سيٽ.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(معلومات, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(معلومات, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = معلومات->سيٽ.blobs[BLOB_SSL_ISSUERCERT];

  إذا(Curl_peer_equal(معلومات->state.initial_origin, origin)) {
    ssl_origin->pinned_ڪنجي =
      ssl_easy_steal(معلومات, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = معلومات->سيٽ.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(معلومات, STRING_CERT_TYPE);
    ssl_origin->ڪنجي = ssl_easy_steal(معلومات, STRING_ڪنجي);
    ssl_origin->ڪنجي_type = ssl_easy_steal(معلومات, STRING_ڪنجي_TYPE);
    ssl_origin->ڪنجي_passwd = ssl_easy_steal(معلومات, STRING_ڪنجي_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(معلومات, STRING_CERT);
    ssl_origin->ڪنجي_blob = معلومات->سيٽ.blobs[BLOB_ڪنجي];
#جيڪڏهن_وصف_ٿيل USE_ECH
    ssl_origin->ech = معلومات->سيٽ.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(معلومات, STRING_ECH_CONFIG);
    ssl_origin->ech_عوامي = ssl_easy_steal(معلومات, STRING_ECH_عوامي);
#جيڪڏهن_پورو
  }
  وإلا {
    ssl_origin->pinned_ڪنجي = عدم;
    ssl_origin->cert_blob = عدم;
    ssl_origin->cert_type = عدم;
    ssl_origin->ڪنجي = عدم;
    ssl_origin->ڪنجي_type = عدم;
    ssl_origin->ڪنجي_passwd = عدم;
    ssl_origin->clientcert = عدم;
    ssl_origin->ڪنجي_blob = عدم;
#جيڪڏهن_وصف_ٿيل USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = عدم;
    ssl_origin->ech_عوامي = عدم;
#جيڪڏهن_پورو
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  sslc = &معلومات->سيٽ.proxy_ssl;

  ssl_easy_config_compl_options(عدم, عدم, sslc, ssl_proxy);
  إذا((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#إذا defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    إذا(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_CA_رستو
    إذا(!sslc->custom_capath &&
       !CURL_EASY_STR(معلومات, STRING_SSL_CAPATH_PROXY)) {
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAPATH_PROXY, CURL_CA_رستو);
      إذا(نتيجو)
        أرجع نتيجو;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_CA_BUNDLE
    إذا(!sslc->custom_cafile &&
       !CURL_EASY_STR(معلومات, STRING_SSL_CAFILE_PROXY)) {
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      إذا(نتيجو)
        أرجع نتيجو;
    }
#جيڪڏهن_پورو
  }

  ssl_proxy->نسخو_2 = sslc->نسخو_2;
  ssl_proxy->نسخو_max = sslc->نسخو_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(معلومات, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(معلومات, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(معلومات, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(معلومات, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_ڪنجي =
    ssl_easy_steal(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = معلومات->سيٽ.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = معلومات->سيٽ.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(معلومات, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = معلومات->سيٽ.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(معلومات, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(معلومات, STRING_CERT_TYPE_PROXY);
  ssl_proxy->ڪنجي = ssl_easy_steal(معلومات, STRING_ڪنجي_PROXY);
  ssl_proxy->ڪنجي_type = ssl_easy_steal(معلومات, STRING_ڪنجي_TYPE_PROXY);
  ssl_proxy->ڪنجي_passwd = ssl_easy_steal(معلومات, STRING_ڪنجي_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(معلومات, STRING_CERT_PROXY);
  ssl_proxy->ڪنجي_blob = معلومات->سيٽ.blobs[BLOB_ڪنجي_PROXY];
#جيڪڏهن_وصف_ٿيل USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = عدم;
  ssl_proxy->ech_عوامي = عدم;
#جيڪڏهن_پورو
#وإلا
  (فراغ)ssl_proxy;
#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */

  أرجع CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(بنية ssl_filter_config *ssl_config,
                                    بنية ssl_filter_config *proxy_ssl_config,
                                    بنية connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  إذا(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    أرجع CURLE_ٻاهريون_جو_يادگيري;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    أرجع CURLE_ٻاهريون_جو_يادگيري;
#وإلا
  (فراغ)proxy_ssl_config;
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

فراغ Curl_ssl_conn_config_cleanup(بنية connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#جيڪڏهن_پورو
}

فراغ Curl_ssl_conn_config_update(بنية Curl_easy *معلومات, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  إذا(معلومات->conn) {
    بنية ssl_easy_config *src;
    بنية ssl_filter_config *dest;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    src = for_proxy ? &معلومات->سيٽ.proxy_ssl : &معلومات->سيٽ.ssl;
    dest = for_proxy ? &معلومات->conn->proxy_ssl_config : &معلومات->conn->ssl_config;
#وإلا
    (فراغ)for_proxy;
    src = &معلومات->سيٽ.ssl;
    dest = &معلومات->conn->ssl_config;
#جيڪڏهن_پورو
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#جيڪڏهن_وصف_ٿيل USE_ECH
    dest->ech = src->ech;
#جيڪڏهن_پورو
  }
}
