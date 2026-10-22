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

#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "setopt.h"
#شامل "strcase.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_config.h"


#تعریف_کرو CLONE_STRING(var)                    \
  کرو {                                       \
    اگر(ماخذ->var) {                        \
      dest->var = curlx_strdup(ماخذ->var); \
      اگر(!dest->var)                         \
        واپس FALSE;                        \
    }                                        \
    ورنہ                                     \
      dest->var = تہی;                      \
  } جب(0)

#تعریف_کرو CLONE_BLOB(var)                  \
  کرو {                                   \
    اگر(blobdup(&dest->var, ماخذ->var)) \
      واپس FALSE;                      \
  } جب(0)

ساکن CURLcode blobdup(ساخت curl_blob **dest, ساخت curl_blob *src)
{
  DEBUGASSERT(dest);
  DEBUGASSERT(!*dest);
  اگر(src) {
    /* only if there is data to dupe! */
    ساخت curl_blob *d;
    d = curlx_malloc(حجم(ساخت curl_blob) + src->len);
    اگر(!d)
      واپس CURLE_برآمد_کا_حافظہ;
    d->len = src->len;
    /* Always duplicate because the connection may survive longer than the
       handle that passed in the blob. */
    d->flags = CURL_BLOB_COPY;
    d->ڈیٹا = (خالی *)((حرف *)d + حجم(ساخت curl_blob));
    حافظہ_نقل(d->ڈیٹا, src->ڈیٹا, src->len);
    *dest = d;
  }
  واپس CURLE_OK;
}

/* returns TRUE if the blobs are identical */
ساکن bool blobcmp(ساخت curl_blob *پہلا, ساخت curl_blob *دوسرا)
{
  اگر(!پہلا && !دوسرا) /* both are NULL */
    واپس TRUE;
  اگر(!پہلا || !دوسرا) /* one is NULL */
    واپس FALSE;
  اگر(پہلا->len != دوسرا->len) /* different sizes */
    واپس FALSE;
  واپس !memcmp(پہلا->ڈیٹا, دوسرا->ڈیٹا, پہلا->len); /* same data */
}

خالی Curl_ssl_config_init(ساخت ssl_easy_config *sslc)
{
  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  sslc->verifypeer = TRUE;
  sslc->verifyhost = TRUE;
  sslc->cache_session = TRUE; /* caching by default */
}

خالی Curl_ssl_config_cleanup(ساخت ssl_filter_config *sslc)
{
  اگر(sslc->deep_copy) {
    curlx_safefree(sslc->CApath);
    curlx_safefree(sslc->CAfile);
    curlx_safefree(sslc->issuercert);
    curlx_safefree(sslc->clientcert);
    curlx_safefree(sslc->cipher_list);
    curlx_safefree(sslc->cipher_list13);
    curlx_safefree(sslc->pinned_کلید);
    curlx_safefree(sslc->cert_blob);
    curlx_safefree(sslc->ca_info_blob);
    curlx_safefree(sslc->issuercert_blob);
    curlx_safefree(sslc->کلید_blob);
    curlx_safefree(sslc->curves);
    curlx_safefree(sslc->signature_algorithms);
    curlx_safefree(sslc->CRLfile);
    curlx_safefree(sslc->cert_type);
    curlx_safefree(sslc->کلید);
    curlx_safefree(sslc->کلید_type);
    curlx_safefree(sslc->کلید_passwd);
#اگر_تعریف_شدہ USE_ECH
    curlx_safefree(sslc->ech_config);
    curlx_safefree(sslc->ech_عوامی);
#اگر_ختم
    sslc->deep_copy = FALSE;
  }
}

ساکن bool مطابقت_ssl_primary_config(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssl_filter_config *c1,
                                     ساخت ssl_filter_config *c2)
{
  (خالی)ڈیٹا;
  اگر((c1->نسخہ_2 == c2->نسخہ_2) &&
     (c1->نسخہ_max == c2->نسخہ_max) &&
     (c1->ssl_options == c2->ssl_options) &&
     (c1->native_ca_store == c2->native_ca_store) &&
     (c1->verifypeer == c2->verifypeer) &&
     (c1->verifyhost == c2->verifyhost) &&
     (c1->verifystatus == c2->verifystatus) &&
     (c1->auto_client_cert == c2->auto_client_cert) &&
#اگر_تعریف_شدہ USE_ECH
     (c1->ech == c2->ech) &&
     Curl_safecmp(c1->ech_config, c2->ech_config) &&
     Curl_safecmp(c1->ech_عوامی, c2->ech_عوامی) &&
#اگر_ختم
     blobcmp(c1->cert_blob, c2->cert_blob) &&
     blobcmp(c1->ca_info_blob, c2->ca_info_blob) &&
     blobcmp(c1->issuercert_blob, c2->issuercert_blob) &&
     blobcmp(c1->کلید_blob, c2->کلید_blob) &&
     Curl_safecmp(c1->CApath, c2->CApath) &&
     Curl_safecmp(c1->CAfile, c2->CAfile) &&
     Curl_safecmp(c1->issuercert, c2->issuercert) &&
     Curl_safecmp(c1->clientcert, c2->clientcert) &&
     curl_strequal(c1->cipher_list, c2->cipher_list) &&
     curl_strequal(c1->cipher_list13, c2->cipher_list13) &&
     curl_strequal(c1->curves, c2->curves) &&
     curl_strequal(c1->signature_algorithms, c2->signature_algorithms) &&
     Curl_safecmp(c1->CRLfile, c2->CRLfile) &&
     Curl_safecmp(c1->pinned_کلید, c2->pinned_کلید) &&
     curl_strequal(c1->cert_type, c2->cert_type) &&
     Curl_safecmp(c1->کلید, c2->کلید) &&
     curl_strequal(c1->کلید_type, c2->کلید_type) &&
     !Curl_timestrcmp(c1->کلید_passwd, c2->کلید_passwd))
    واپس TRUE;

  واپس FALSE;
}

bool Curl_ssl_conn_config_مطابقت(ساخت Curl_easy *ڈیٹا,
                                ساخت ssl_filter_config *conn_config,
                                ساخت connectdata *candidate,
                                bool proxy)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(proxy)
    واپس مطابقت_ssl_primary_config(ڈیٹا, conn_config,
                                    &candidate->proxy_ssl_config);
#ورنہ
  (خالی)proxy;
#اگر_ختم
  واپس مطابقت_ssl_primary_config(ڈیٹا, conn_config,
                                  &candidate->ssl_config);
}

ساکن bool clone_ssl_primary_config(ساخت ssl_filter_config *ماخذ,
                                     ساخت ssl_filter_config *dest)
{
  DEBUGASSERT(!dest->deep_copy);
  dest->deep_copy = TRUE;
  dest->نسخہ_2 = ماخذ->نسخہ_2;
  dest->نسخہ_max = ماخذ->نسخہ_max;
  dest->verifypeer = ماخذ->verifypeer;
  dest->verifyhost = ماخذ->verifyhost;
  dest->verifystatus = ماخذ->verifystatus;
  dest->ssl_options = ماخذ->ssl_options;
  dest->native_ca_store = ماخذ->native_ca_store;
  dest->cache_session = ماخذ->cache_session;
  dest->auto_client_cert = ماخذ->auto_client_cert;
  dest->earlydata = ماخذ->earlydata;
  dest->enable_beast = ماخذ->enable_beast;
  dest->no_partialchain = ماخذ->no_partialchain;
  dest->no_revoke = ماخذ->no_revoke;
  dest->revoke_بہترین_effort = ماخذ->revoke_بہترین_effort;

  CLONE_BLOB(cert_blob);
  CLONE_BLOB(ca_info_blob);
  CLONE_BLOB(issuercert_blob);
  CLONE_STRING(CApath);
  CLONE_STRING(CAfile);
  CLONE_STRING(issuercert);
  CLONE_STRING(cipher_list);
  CLONE_STRING(cipher_list13);
  CLONE_STRING(pinned_کلید);
  CLONE_STRING(curves);
  CLONE_STRING(signature_algorithms);
  CLONE_STRING(CRLfile);
  /* SSL credentials: client certificate */
  CLONE_STRING(clientcert);
  CLONE_STRING(cert_type);
  CLONE_STRING(کلید);
  CLONE_STRING(کلید_type);
  CLONE_STRING(کلید_passwd);
  CLONE_BLOB(کلید_blob);
#اگر_تعریف_شدہ USE_ECH
  dest->ech = ماخذ->ech;
  CLONE_STRING(ech_config);
  CLONE_STRING(ech_عوامی);
#اگر_ختم
  واپس TRUE;
}

ساکن خالی
ssl_easy_config_compl_options(ساخت Curl_peer *origin,
                              ساخت Curl_peer *initial_origin,
                              ساخت ssl_easy_config *sslc,
                              ساخت ssl_filter_config *primary)
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
  primary->revoke_بہترین_effort = !!(options & CURLSSLOPT_REVOKE_بہترین_EFFORT);
}

ساکن حرف *ssl_easy_steal(ساخت Curl_easy *ڈیٹا, شمار dupstring id)
{
  /* For connection matching, we borrow string references from data
   * THIS IS NOT REALLY NICE. */
  واپس CURL_UNCONST(CURL_EASY_STR(ڈیٹا, id));
}

CURLcode Curl_ssl_filter_config_tmp_init(
  ساخت Curl_easy *ڈیٹا, ساخت Curl_peer *origin,
  ساخت ssl_filter_config *ssl_origin,
  ساخت ssl_filter_config *ssl_proxy)
{
  ساخت ssl_easy_config *sslc = &ڈیٹا->سیٹ.ssl;
#اگر defined(CURL_CA_راستہ) || defined(CURL_CA_BUNDLE)
  CURLcode نتیجہ;
#اگر_ختم

  ssl_easy_config_compl_options(origin, ڈیٹا->state.initial_origin, sslc,
                                ssl_origin);

  اگر(Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL) {
#اگر defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    اگر(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_origin->native_ca_store = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ CURL_CA_راستہ
    اگر(!sslc->custom_capath && !CURL_EASY_STR(ڈیٹا, STRING_SSL_CAPATH)) {
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAPATH, CURL_CA_راستہ);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
#اگر_ختم
#اگر_تعریف_شدہ CURL_CA_BUNDLE
    اگر(!sslc->custom_cafile && !CURL_EASY_STR(ڈیٹا, STRING_SSL_CAFILE)) {
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAFILE, CURL_CA_BUNDLE);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
#اگر_ختم
  }

  ssl_origin->نسخہ_2 = sslc->نسخہ_2;
  ssl_origin->نسخہ_max = sslc->نسخہ_max;
  ssl_origin->verifypeer = sslc->verifypeer;
  ssl_origin->verifyhost = sslc->verifyhost;
  ssl_origin->verifystatus = sslc->verifystatus;
  ssl_origin->cache_session = sslc->cache_session;
  ssl_origin->ssl_options = sslc->ssl_options;
  ssl_origin->CAfile = ssl_easy_steal(ڈیٹا, STRING_SSL_CAFILE);
  ssl_origin->CRLfile = ssl_easy_steal(ڈیٹا, STRING_SSL_CRLFILE);
  ssl_origin->CApath = ssl_easy_steal(ڈیٹا, STRING_SSL_CAPATH);
  ssl_origin->cipher_list = ssl_easy_steal(ڈیٹا, STRING_SSL_CIPHER_LIST);
  ssl_origin->cipher_list13 = ssl_easy_steal(ڈیٹا, STRING_SSL_CIPHER13_LIST);
  ssl_origin->signature_algorithms =
    ssl_easy_steal(ڈیٹا, STRING_SSL_SIGNATURE_ALGORITHMS);
  ssl_origin->ca_info_blob = ڈیٹا->سیٹ.blobs[BLOB_CAINFO];
  ssl_origin->curves = ssl_easy_steal(ڈیٹا, STRING_SSL_EC_CURVES);
  /* Maybe these should not be used for another origin. But for
   * backwards compatibility, keep them in. */
  ssl_origin->issuercert = ssl_easy_steal(ڈیٹا, STRING_SSL_ISSUERCERT);
  ssl_origin->issuercert_blob = ڈیٹا->سیٹ.blobs[BLOB_SSL_ISSUERCERT];

  اگر(Curl_peer_equal(ڈیٹا->state.initial_origin, origin)) {
    ssl_origin->pinned_کلید =
      ssl_easy_steal(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
    ssl_origin->cert_blob = ڈیٹا->سیٹ.blobs[BLOB_CERT];
    ssl_origin->cert_type = ssl_easy_steal(ڈیٹا, STRING_CERT_TYPE);
    ssl_origin->کلید = ssl_easy_steal(ڈیٹا, STRING_کلید);
    ssl_origin->کلید_type = ssl_easy_steal(ڈیٹا, STRING_کلید_TYPE);
    ssl_origin->کلید_passwd = ssl_easy_steal(ڈیٹا, STRING_کلید_PASSWD);
    ssl_origin->clientcert = ssl_easy_steal(ڈیٹا, STRING_CERT);
    ssl_origin->کلید_blob = ڈیٹا->سیٹ.blobs[BLOB_کلید];
#اگر_تعریف_شدہ USE_ECH
    ssl_origin->ech = ڈیٹا->سیٹ.ssl.ech;
    ssl_origin->ech_config = ssl_easy_steal(ڈیٹا, STRING_ECH_CONFIG);
    ssl_origin->ech_عوامی = ssl_easy_steal(ڈیٹا, STRING_ECH_عوامی);
#اگر_ختم
  }
  ورنہ {
    ssl_origin->pinned_کلید = تہی;
    ssl_origin->cert_blob = تہی;
    ssl_origin->cert_type = تہی;
    ssl_origin->کلید = تہی;
    ssl_origin->کلید_type = تہی;
    ssl_origin->کلید_passwd = تہی;
    ssl_origin->clientcert = تہی;
    ssl_origin->کلید_blob = تہی;
#اگر_تعریف_شدہ USE_ECH
    ssl_origin->ech = 0;
    ssl_origin->ech_config = تہی;
    ssl_origin->ech_عوامی = تہی;
#اگر_ختم
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  sslc = &ڈیٹا->سیٹ.proxy_ssl;

  ssl_easy_config_compl_options(تہی, تہی, sslc, ssl_proxy);
  اگر((Curl_ssl_backend() != CURLSSLBACKEND_SCHANNEL)) {
    /* no initial origin for proxy, it is not changed for redirects */
#اگر defined(USE_APPLE_SECTRUST) || defined(CURL_CA_NATIVE)
    اگر(!sslc->custom_capath && !sslc->custom_cafile && !sslc->custom_cablob)
      ssl_proxy->native_ca_store = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ CURL_CA_راستہ
    اگر(!sslc->custom_capath &&
       !CURL_EASY_STR(ڈیٹا, STRING_SSL_CAPATH_PROXY)) {
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAPATH_PROXY, CURL_CA_راستہ);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
#اگر_ختم
#اگر_تعریف_شدہ CURL_CA_BUNDLE
    اگر(!sslc->custom_cafile &&
       !CURL_EASY_STR(ڈیٹا, STRING_SSL_CAFILE_PROXY)) {
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAFILE_PROXY, CURL_CA_BUNDLE);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
#اگر_ختم
  }

  ssl_proxy->نسخہ_2 = sslc->نسخہ_2;
  ssl_proxy->نسخہ_max = sslc->نسخہ_max;
  ssl_proxy->verifypeer = sslc->verifypeer;
  ssl_proxy->verifyhost = sslc->verifyhost;
  ssl_proxy->verifystatus = sslc->verifystatus;
  ssl_proxy->cache_session = sslc->cache_session;
  ssl_proxy->ssl_options = sslc->ssl_options;
  ssl_proxy->CAfile = ssl_easy_steal(ڈیٹا, STRING_SSL_CAFILE_PROXY);
  ssl_proxy->CApath = ssl_easy_steal(ڈیٹا, STRING_SSL_CAPATH_PROXY);
  ssl_proxy->cipher_list =
    ssl_easy_steal(ڈیٹا, STRING_SSL_CIPHER_LIST_PROXY);
  ssl_proxy->cipher_list13 =
    ssl_easy_steal(ڈیٹا, STRING_SSL_CIPHER13_LIST_PROXY);
  ssl_proxy->pinned_کلید =
    ssl_easy_steal(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY);
  ssl_proxy->cert_blob = ڈیٹا->سیٹ.blobs[BLOB_CERT_PROXY];
  ssl_proxy->ca_info_blob = ڈیٹا->سیٹ.blobs[BLOB_CAINFO_PROXY];
  ssl_proxy->issuercert = ssl_easy_steal(ڈیٹا, STRING_SSL_ISSUERCERT_PROXY);
  ssl_proxy->issuercert_blob = ڈیٹا->سیٹ.blobs[BLOB_SSL_ISSUERCERT_PROXY];
  ssl_proxy->CRLfile = ssl_easy_steal(ڈیٹا, STRING_SSL_CRLFILE_PROXY);
  ssl_proxy->cert_type = ssl_easy_steal(ڈیٹا, STRING_CERT_TYPE_PROXY);
  ssl_proxy->کلید = ssl_easy_steal(ڈیٹا, STRING_کلید_PROXY);
  ssl_proxy->کلید_type = ssl_easy_steal(ڈیٹا, STRING_کلید_TYPE_PROXY);
  ssl_proxy->کلید_passwd = ssl_easy_steal(ڈیٹا, STRING_کلید_PASSWD_PROXY);
  ssl_proxy->clientcert = ssl_easy_steal(ڈیٹا, STRING_CERT_PROXY);
  ssl_proxy->کلید_blob = ڈیٹا->سیٹ.blobs[BLOB_کلید_PROXY];
#اگر_تعریف_شدہ USE_ECH
  /* Not configurable via a CURLOPT_* */
  ssl_proxy->ech = 0;
  ssl_proxy->ech_config = تہی;
  ssl_proxy->ech_عوامی = تہی;
#اگر_ختم
#ورنہ
  (خالی)ssl_proxy;
#اگر_ختم /* CURL_DISABLE_PROXY */

  واپس CURLE_OK;
}

CURLcode Curl_ssl_conn_config_clone(ساخت ssl_filter_config *ssl_config,
                                    ساخت ssl_filter_config *proxy_ssl_config,
                                    ساخت connectdata *conn)
{
  /* Clone "primary" SSL configurations from the easy handle to
   * the connection. They are used for connection cache matching and
   * probably outlive the easy handle */
  اگر(!clone_ssl_primary_config(ssl_config, &conn->ssl_config))
    واپس CURLE_برآمد_کا_حافظہ;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(proxy_ssl_config &&
     !clone_ssl_primary_config(proxy_ssl_config, &conn->proxy_ssl_config))
    واپس CURLE_برآمد_کا_حافظہ;
#ورنہ
  (خالی)proxy_ssl_config;
#اگر_ختم
  واپس CURLE_OK;
}

خالی Curl_ssl_conn_config_cleanup(ساخت connectdata *conn)
{
  Curl_ssl_config_cleanup(&conn->ssl_config);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&conn->proxy_ssl_config);
#اگر_ختم
}

خالی Curl_ssl_conn_config_update(ساخت Curl_easy *ڈیٹا, bool for_proxy)
{
  /* May be called on an easy that has no connection yet */
  اگر(ڈیٹا->conn) {
    ساخت ssl_easy_config *src;
    ساخت ssl_filter_config *dest;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    src = for_proxy ? &ڈیٹا->سیٹ.proxy_ssl : &ڈیٹا->سیٹ.ssl;
    dest = for_proxy ? &ڈیٹا->conn->proxy_ssl_config : &ڈیٹا->conn->ssl_config;
#ورنہ
    (خالی)for_proxy;
    src = &ڈیٹا->سیٹ.ssl;
    dest = &ڈیٹا->conn->ssl_config;
#اگر_ختم
    dest->verifyhost = src->verifyhost;
    dest->verifypeer = src->verifypeer;
    dest->verifystatus = src->verifystatus;
#اگر_تعریف_شدہ USE_ECH
    dest->ech = src->ech;
#اگر_ختم
  }
}
